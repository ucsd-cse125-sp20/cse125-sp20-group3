#include "GLTFObject.h"

// Initialize static variables

VertexLayout GLTFObject::pVertexLayoutModel = {
	3,
	{
		{SEMANTIC_POSITION, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 0, 0, 0, 0},
		{SEMANTIC_NORMAL, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 1, 0, 3 * sizeof(float), 0},
		{SEMANTIC_TEXCOORD0, 0, NULL, TinyImageFormat_R32G32_SFLOAT, 2, 0, 6 * sizeof(float), 0}
	}
};

int GLTFObject::instanceCount = 0;
int GLTFObject::modelCount = 0;
bool GLTFObject::countingInstances = false;
Buffer* GLTFObject::pNodeTransformsBuffer = NULL;


void GLTFObject::Init(const Path* path, Renderer* renderer, Sampler* defaultSampler)
{
	pRenderer = renderer;
	pDefaultSampler = defaultSampler;

	mSamplers.resize(pData->mSamplerCount);
	for (uint32_t i = 0; i < pData->mSamplerCount; ++i)
		addSampler(pRenderer, pData->pSamplers + i, &mSamplers[i]);

	mTextures.resize(pData->pHandle->images_count);
	SyncToken token = {};
	for (uint32_t i = 0; i < pData->pHandle->images_count; ++i)
		gltfLoadTextureAtIndex(pData, path, i, false, &token, &mTextures[i]);

	if (pData->mNodeCount)
		createNodeTransformsBuffer();

	// Missing texture handling (Might need to move)
	TextureDesc defaultTextureDesc = {};
	defaultTextureDesc.mArraySize = 1;
	defaultTextureDesc.mDepth = 1;
	defaultTextureDesc.mFormat = TinyImageFormat_R8G8B8A8_UNORM;
	defaultTextureDesc.mWidth = 4;
	defaultTextureDesc.mHeight = 4;
	defaultTextureDesc.mMipLevels = 1;
	defaultTextureDesc.mSampleCount = SAMPLE_COUNT_1;
	defaultTextureDesc.mStartState = RESOURCE_STATE_COMMON;
	defaultTextureDesc.mDescriptors = DESCRIPTOR_TYPE_TEXTURE;
	defaultTextureDesc.mFlags = TEXTURE_CREATION_FLAG_OWN_MEMORY_BIT;
	defaultTextureDesc.pDebugName = L"MissingTexture";
	TextureLoadDesc defaultLoadDesc = {};
	defaultLoadDesc.pDesc = &defaultTextureDesc;
	RawImageData idata = {};
	unsigned char blackData[64];
	memset(blackData, 0, sizeof(unsigned char) * 64);

	idata.mArraySize = 1;
	idata.mDepth = defaultTextureDesc.mDepth;
	idata.mWidth = defaultTextureDesc.mWidth;
	idata.mHeight = defaultTextureDesc.mHeight;
	idata.mFormat = defaultTextureDesc.mFormat;
	idata.mMipLevels = defaultTextureDesc.mMipLevels;
	idata.pRawData = (uint8_t*)blackData;
	defaultLoadDesc.pRawImageData = &idata;

	defaultLoadDesc.ppTexture = &pTextureBlack;
	addResource(&defaultLoadDesc, NULL, LOAD_PRIORITY_NORMAL);
}

bool GLTFObject::LoadModel(GLTFObject* asset, Renderer* renderer, Sampler* sampler, const Path* modelFilePath)
{
	eastl::string modelFileName = fsPathComponentToString(fsGetPathFileName(modelFilePath));

	eastl::vector<PathHandle> validFileLists;
	eastl::vector<PathHandle> intermediateFileLists;

#if defined(TARGET_IOS)
	PathHandle meshDirectory = fsCopyParentPath(modelFilePath);
#else
	PathHandle meshDirectory = fsCopyPathForResourceDirectory(RD_MESHES);
#endif
	eastl::vector<PathHandle> fileLists = fsGetFilesWithExtension(meshDirectory, "gltf");

	eastl::string fileNameOnly = modelFileName;

	asset->removeResources();

	// Create vertex layout
	GLTFObject::pVertexLayoutModel.mAttribCount = 3;

	GLTFObject::pVertexLayoutModel.mAttribs[0].mSemantic = SEMANTIC_POSITION;
	GLTFObject::pVertexLayoutModel.mAttribs[0].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	GLTFObject::pVertexLayoutModel.mAttribs[0].mBinding = 0;
	GLTFObject::pVertexLayoutModel.mAttribs[0].mLocation = 0;
	GLTFObject::pVertexLayoutModel.mAttribs[0].mOffset = 0;

	//normals
	GLTFObject::pVertexLayoutModel.mAttribs[1].mSemantic = SEMANTIC_NORMAL;
	GLTFObject::pVertexLayoutModel.mAttribs[1].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	GLTFObject::pVertexLayoutModel.mAttribs[1].mLocation = 1;
	GLTFObject::pVertexLayoutModel.mAttribs[1].mBinding = 0;
	GLTFObject::pVertexLayoutModel.mAttribs[1].mOffset = 3 * sizeof(float);

	//texture
	GLTFObject::pVertexLayoutModel.mAttribs[2].mSemantic = SEMANTIC_TEXCOORD0;
	GLTFObject::pVertexLayoutModel.mAttribs[2].mFormat = TinyImageFormat_R32G32_SFLOAT;
	GLTFObject::pVertexLayoutModel.mAttribs[2].mLocation = 2;
	GLTFObject::pVertexLayoutModel.mAttribs[2].mBinding = 0;
	GLTFObject::pVertexLayoutModel.mAttribs[2].mOffset = 6 * sizeof(float);    // first attribute contains 3 floats

	GeometryLoadDesc loadDesc = {};
	loadDesc.ppGeometry = &asset->pGeom;
	loadDesc.pFilePath = modelFilePath;
	loadDesc.pVertexLayout = &pVertexLayoutModel;
	addResource(&loadDesc, NULL, LOAD_PRIORITY_NORMAL);

	uint32_t res = gltfLoadContainer(modelFilePath, GLTF_FLAG_CALCULATE_BOUNDS, &asset->pData);
	if (res)
	{
		return false;
	}

	asset->Init(modelFilePath, renderer, sampler);
	waitForAllResourceLoads();
	return true;
}

void GLTFObject::updateTransform(size_t nodeIndex, mat4* nodeTransforms, bool* nodeTransformsInited)
{
	if (nodeTransformsInited[nodeIndex]) { return; }

	if (pData->pNodes[nodeIndex].mScale.getX() != pData->pNodes[nodeIndex].mScale.getY() ||
		pData->pNodes[nodeIndex].mScale.getX() != pData->pNodes[nodeIndex].mScale.getZ())
	{
		LOGF(LogLevel::eWARNING, "Node %llu has a non-uniform scale and will have an incorrect normal when rendered.", (uint64_t)nodeIndex);
	}

	mat4 matrix = pData->pNodes[nodeIndex].mMatrix;
	if (pData->pNodes[nodeIndex].mParentIndex != UINT_MAX)
	{
		updateTransform((size_t)pData->pNodes[nodeIndex].mParentIndex, nodeTransforms, nodeTransformsInited);
		matrix = nodeTransforms[pData->pNodes[nodeIndex].mParentIndex] * matrix;
	}
	nodeTransforms[nodeIndex] = matrix;
	nodeTransformsInited[nodeIndex] = true;
}

void GLTFObject::createNodeTransformsBuffer()
{
	bool* nodeTransformsInited = (bool*)alloca(sizeof(bool) * pData->mNodeCount);
	memset(nodeTransformsInited, 0, sizeof(bool) * pData->mNodeCount);

	mat4* nodeTransforms = (mat4*)alloca(sizeof(mat4) * MAX_GLTF_MODELS * MAX_GLTF_NODES);

	for (uint32_t i = 0; i < pData->mNodeCount; ++i)
	{
		updateTransform(i, nodeTransforms, nodeTransformsInited);
	}

	// Scale and centre the model.

	Point3 modelBounds[2] = { Point3(FLT_MAX), Point3(-FLT_MAX) };
	size_t nodeIndex = 0;
	for (uint32_t n = 0; n < pData->mNodeCount; ++n)
	{
		GLTFNode& node = pData->pNodes[n];

		if (node.mMeshIndex != UINT_MAX)
		{
			for (uint32_t i = 0; i < node.mMeshCount; ++i)
			{
				Point3 minBound = pData->pMeshes[node.mMeshIndex + i].mMin;
				Point3 maxBound = pData->pMeshes[node.mMeshIndex + i].mMax;
				Point3 localPoints[] = {
					Point3(minBound.getX(), minBound.getY(), minBound.getZ()),
					Point3(minBound.getX(), minBound.getY(), maxBound.getZ()),
					Point3(minBound.getX(), maxBound.getY(), minBound.getZ()),
					Point3(minBound.getX(), maxBound.getY(), maxBound.getZ()),
					Point3(maxBound.getX(), minBound.getY(), minBound.getZ()),
					Point3(maxBound.getX(), minBound.getY(), maxBound.getZ()),
					Point3(maxBound.getX(), maxBound.getY(), minBound.getZ()),
					Point3(maxBound.getX(), maxBound.getY(), maxBound.getZ()),
				};
				for (size_t j = 0; j < 8; j += 1)
				{
					vec4 worldPoint = nodeTransforms[nodeIndex] * localPoints[j];
					modelBounds[0] = minPerElem(modelBounds[0], Point3(worldPoint.getXYZ()));
					modelBounds[1] = maxPerElem(modelBounds[1], Point3(worldPoint.getXYZ()));
				}
			}
		}
		nodeIndex += 1;
	}

	const float targetSize = 1.0;

	vec3 modelSize = modelBounds[1] - modelBounds[0];
	float largestDim = max(modelSize.getX(), max(modelSize.getY(), modelSize.getZ()));
	Point3 modelCentreBase = Point3(
		0.5f * (modelBounds[0].getX() + modelBounds[1].getX()),
		modelBounds[0].getY(),
		0.5f * (modelBounds[0].getZ() + modelBounds[1].getZ()));
	Vector3 scaleVector = Vector3(1);
	scaleVector.setZ(-scaleVector.getZ());
	mat4 translateScale = mat4::scale(scaleVector) * mat4::translation(Vector3(modelCentreBase));

	for (uint32_t i = 0; i < pData->mNodeCount; ++i)
	{
		nodeTransforms[i] = translateScale * nodeTransforms[i];
	}

	modelID = modelCount++;
		
	if (!pNodeTransformsBuffer) {
		BufferLoadDesc nodeTransformsBufferLoadDesc = {};
		nodeTransformsBufferLoadDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
		nodeTransformsBufferLoadDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_BUFFER;
		nodeTransformsBufferLoadDesc.mDesc.mStructStride = sizeof(mat4);
		nodeTransformsBufferLoadDesc.mDesc.mElementCount = MAX_GLTF_MODELS * MAX_GLTF_NODES;
		nodeTransformsBufferLoadDesc.mDesc.mSize = MAX_GLTF_MODELS * MAX_GLTF_NODES * sizeof(mat4);
		nodeTransformsBufferLoadDesc.pData = nodeTransforms;
		nodeTransformsBufferLoadDesc.mDesc.pDebugName = L"GLTF Node Transforms Buffer";
		nodeTransformsBufferLoadDesc.ppBuffer = &pNodeTransformsBuffer;
		addResource(&nodeTransformsBufferLoadDesc, NULL, LOAD_PRIORITY_NORMAL);
	}
	else {
		BufferUpdateDesc bufferUpdate = { pNodeTransformsBuffer };
		beginUpdateResource(&bufferUpdate);
		mat4* bufferedNodeTransforms = (mat4*)bufferUpdate.pMappedData;
		for (uint32_t i = 0; i < pData->mNodeCount; ++i) {
			bufferedNodeTransforms[modelID * MAX_GLTF_NODES + i] = nodeTransforms[i];
		}
		endUpdateResource(&bufferUpdate, NULL);
	}
}

void GLTFObject::updateTextureProperties(const GLTFTextureView& textureView, GLTFTextureProperties& textureProperties)
{
	textureProperties.mTextureIndex = (int16_t)textureView.mTextureIndex;
	textureProperties.mSamplerIndex = (int16_t)textureView.mSamplerIndex;
	textureProperties.mUVStreamIndex = textureView.mUVStreamIndex;
	textureProperties.mValueScale = textureView.mScale;
	textureProperties.mScale = textureView.mTransform.mScale;
	textureProperties.mOffset = textureView.mTransform.mOffset;
	textureProperties.mRotation = textureView.mTransform.mRotation;
}

void GLTFObject::updateParam(DescriptorData* params, const GLTFTextureView& textureView, const char* textureName, const char* samplerName, uint32_t& nextParamIdx)
{
	if (textureView.mTextureIndex >= 0)
	{
		ASSERT(textureView.mTextureIndex < (ssize_t)mTextures.size());
		uint32_t index = nextParamIdx++;
		params[index].pName = textureName;
		params[index].ppTextures = &mTextures[textureView.mTextureIndex];
	}
	else
	{
		uint32_t index = nextParamIdx++;
		params[index].pName = textureName;
		params[index].ppTextures = &pTextureBlack;
	}

	uint32_t samplerIndex = nextParamIdx++;
	params[samplerIndex].pName = samplerName;
	if (textureView.mSamplerIndex >= 0)
	{
		ASSERT(textureView.mSamplerIndex < (ssize_t)mSamplers.size());
		params[samplerIndex].ppSamplers = &mSamplers[textureView.mSamplerIndex];
	}
	else
	{
		params[samplerIndex].ppSamplers = &pDefaultSampler;
	}
}

void GLTFObject::createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet)
{
	uint64_t materialDataStride = pBindlessTexturesSamplersSet ? sizeof(GLTFMaterialData) : round_up_64(sizeof(GLTFMaterialData), 256);

	BufferLoadDesc materialBufferLoadDesc = {};
	materialBufferLoadDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	materialBufferLoadDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	materialBufferLoadDesc.mDesc.mStructStride = materialDataStride;
	materialBufferLoadDesc.mDesc.mSize = pData->mMaterialCount * materialDataStride;
	materialBufferLoadDesc.mDesc.pDebugName = L"GLTF Materials Buffer";
	materialBufferLoadDesc.ppBuffer = &pMaterialBuffer;
	addResource(&materialBufferLoadDesc, NULL, LOAD_PRIORITY_NORMAL);

	waitForAllResourceLoads();

	if (pBindlessTexturesSamplersSet)
	{
		// Use a single descriptor set and indexing into it using push constants.
		DescriptorSetDesc desc = {};
		desc.mMaxSets = 1;
		desc.mUpdateFrequency = DESCRIPTOR_UPDATE_FREQ_NONE;
		desc.pRootSignature = pRootSignature;
		addDescriptorSet(pRenderer, &desc, &pBindlessTexturesSamplersSet);

		DescriptorData params[3] = {};
		params[0].pName = "cbMaterialData";
		params[0].ppBuffers = &pMaterialBuffer;

		params[1].pName = "gltfTextures";
		params[1].ppTextures = mTextures.data();
		params[1].mCount = (uint32_t)mTextures.size();

		params[2].pName = "gltfSamplers";
		params[2].ppSamplers = mSamplers.data();
		params[2].mCount = (uint32_t)mSamplers.size();

		updateDescriptorSet(pRenderer, 0, pBindlessTexturesSamplersSet, 3, params);
	}
	else
	{
		// Use one set per material.
		DescriptorSetDesc desc = {};
		desc.mMaxSets = (uint32_t)pData->mMaterialCount;
		desc.mUpdateFrequency = DESCRIPTOR_UPDATE_FREQ_PER_DRAW;
		desc.pRootSignature = pRootSignature;
		addDescriptorSet(pRenderer, &desc, &pMaterialSet);
	}

	SyncToken token = {};

	size_t i = 0;
	for (uint32_t m = 0; m < pData->mMaterialCount; ++m)
	{
		const GLTFMaterial& material = pData->pMaterials[m];
		uint64_t materialBufferOffset = i * materialDataStride;

		BufferUpdateDesc updateDesc = {};
		updateDesc.pBuffer = pMaterialBuffer;
		updateDesc.mDstOffset = materialBufferOffset;
		beginUpdateResource(&updateDesc);
		{
			GLTFMaterialData* materialData = (GLTFMaterialData*)updateDesc.pMappedData;
			materialData->mAlphaMode = (uint32_t)material.mAlphaMode;
			materialData->mAlphaCutoff = material.mAlphaCutoff;
			materialData->mEmissiveGBScale = float2(material.mEmissiveFactor.y, material.mEmissiveFactor.z);

			switch (material.mMaterialType)
			{
			case GLTF_MATERIAL_TYPE_METALLIC_ROUGHNESS:
			{
				const GLTFMetallicRoughnessMaterial& metallicRoughness = material.mMetallicRoughness;
				materialData->mBaseColorFactor = metallicRoughness.mBaseColorFactor;
				materialData->mMetallicRoughnessFactors = float4(0.0f, metallicRoughness.mRoughnessFactor, metallicRoughness.mMetallicFactor, 0.f);
				updateTextureProperties(metallicRoughness.mBaseColorTexture, materialData->mBaseColorProperties);
				updateTextureProperties(metallicRoughness.mMetallicRoughnessTexture, materialData->mMetallicRoughnessProperties);
				break;
			}
			case GLTF_MATERIAL_TYPE_SPECULAR_GLOSSINESS:
			{
				const GLTFSpecularGlossinessMaterial& specularGlossiness = material.mSpecularGlossiness;
				materialData->mBaseColorFactor = specularGlossiness.mDiffuseFactor;
				materialData->mMetallicRoughnessFactors = float4(specularGlossiness.mSpecularFactor, specularGlossiness.mGlossinessFactor);
				updateTextureProperties(specularGlossiness.mDiffuseTexture, materialData->mBaseColorProperties);
				updateTextureProperties(specularGlossiness.mSpecularGlossinessTexture, materialData->mMetallicRoughnessProperties);
				break;
			}
			}

			updateTextureProperties(material.mNormalTexture, materialData->mNormalTextureProperties);
			updateTextureProperties(material.mOcclusionTexture, materialData->mOcclusionTextureProperties);
			updateTextureProperties(material.mEmissiveTexture, materialData->mEmissiveTextureProperties);

			if (materialData->mEmissiveTextureProperties.mTextureIndex >= 0)
			{
				materialData->mEmissiveGBScale *= materialData->mEmissiveTextureProperties.mValueScale;
				materialData->mEmissiveTextureProperties.mValueScale *= material.mEmissiveFactor.x;
			}
			else
			{
				materialData->mEmissiveTextureProperties.mValueScale = material.mEmissiveFactor.x;
			}
		}
		endUpdateResource(&updateDesc, &token);

		DescriptorData params[11] = {};
		params[0].pName = "cbMaterialData";
		params[0].ppBuffers = &pMaterialBuffer;
		params[0].pOffsets = &materialBufferOffset;
		params[0].pSizes = &materialDataStride;
		uint32_t paramIdx = 1;

		switch (material.mMaterialType)
		{
		case GLTF_MATERIAL_TYPE_METALLIC_ROUGHNESS:
		{
			const GLTFMetallicRoughnessMaterial& metallicRoughness = material.mMetallicRoughness;
			updateParam(params, metallicRoughness.mBaseColorTexture, "baseColorMap", "baseColorSampler", paramIdx);
			updateParam(params, metallicRoughness.mMetallicRoughnessTexture, "metallicRoughnessMap", "metallicRoughnessSampler", paramIdx);
			break;
		}
		case GLTF_MATERIAL_TYPE_SPECULAR_GLOSSINESS:
		{
			const GLTFSpecularGlossinessMaterial& specularGlossiness = material.mSpecularGlossiness;
			updateParam(params, specularGlossiness.mDiffuseTexture, "baseColorMap", "baseColorSampler", paramIdx);
			updateParam(params, specularGlossiness.mSpecularGlossinessTexture, "metallicRoughnessMap", "metallicRoughnessSampler", paramIdx);
			break;
		}
		}

		updateParam(params, material.mNormalTexture, "normalMap", "normalMapSampler", paramIdx);
		updateParam(params, material.mOcclusionTexture, "occlusionMap", "occlusionMapSampler", paramIdx);
		updateParam(params, material.mEmissiveTexture, "emissiveMap", "emissiveMapSampler", paramIdx);

		if (!pBindlessTexturesSamplersSet)
		{
			updateDescriptorSet(pRenderer, (uint32_t)i, pMaterialSet, paramIdx, params);
		}

		i += 1;
	}

	waitForToken(&token);
}

void GLTFObject::bindBuffers(Cmd* cmd)
{
	cmdBindVertexBuffer(cmd, 1, &pGeom->pVertexBuffers[0], &pGeom->mVertexStrides[0], NULL);
	cmdBindIndexBuffer(cmd, pGeom->pIndexBuffer, pGeom->mIndexType, 0);
}

void GLTFObject::removeResources()
{
	if (!pData)
		return;

	for (Sampler* sampler : mSamplers)
		removeSampler(pRenderer, sampler);

	mSamplers.set_capacity(0);

	for (Texture* texture : mTextures)
		removeResource(texture);

	gltfUnloadContainer(pData);
	removeResource(pGeom);
	removeResource(pTextureBlack);

	mTextures.set_capacity(0);

	if (pMaterialSet)
		removeDescriptorSet(pRenderer, pMaterialSet);

	if (pMaterialBuffer)
		removeResource(pMaterialBuffer);

	if (pNodeTransformsBuffer) {
		removeResource(pNodeTransformsBuffer);
		pNodeTransformsBuffer = NULL;
	}

	pData = NULL;
	pMaterialBuffer = NULL;
	pMaterialSet = NULL;
	pRenderer = NULL;
}

void GLTFObject::update(float deltaTime)
{
	if (!countingInstances) {
		countingInstances = true;
		instanceCount = 1;
	}
	instanceID = instanceCount++;
}

void GLTFObject::draw(Cmd* cmd, RootSignature* rootSignature, bool useMaterial)
{
	countingInstances = false;

	cmdBindVertexBuffer(cmd, 1, &pGeom->pVertexBuffers[0], &pGeom->mVertexStrides[0], NULL);
	cmdBindIndexBuffer(cmd, pGeom->pIndexBuffer, pGeom->mIndexType, 0);

	MeshPushConstants pushConstants = {};
	pushConstants.nodeIndex = 0;
	pushConstants.instanceIndex = instanceID;
	pushConstants.modelIndex = modelID;

	for (uint32_t n = 0; n < pData->mNodeCount; ++n)
	{
		//printf("%d %d %d %d\n", pushConstants.instanceIndex, pushConstants.nodeIndex, pushConstants.modelIndex, pGeom->mVertexCount);

		GLTFNode& node = pData->pNodes[n];
		if (node.mMeshIndex != UINT_MAX)
		{
			cmdBindPushConstants(cmd, rootSignature, "cbRootConstants", &pushConstants);
			for (uint32_t i = 0; i < node.mMeshCount; ++i)
			{
				GLTFMesh& mesh = pData->pMeshes[node.mMeshIndex + i];

				if (useMaterial)
					cmdBindDescriptorSet(cmd, (uint32_t)pData->pMaterialIndices[node.mMeshIndex + i], pMaterialSet);
				cmdDrawIndexed(cmd, mesh.mIndexCount, mesh.mStartIndex, 0);
			}
		}

		pushConstants.nodeIndex += 1;
	}
}

void GLTFObject::setTranslate(vec3 position) {
	model[3] = vec4(position, 1.0f);
}

void GLTFObject::setScaleRot(vec3 scale, float deg, vec3 axis) {
	mat4 scaleRot = mat4::scale(vec3(scale)) * mat4::rotation(deg, axis);
	model[0] = scaleRot[0];
	model[1] = scaleRot[1];
	model[2] = scaleRot[2];
}

void GLTFObject::setPositionDirection(vec3 position, vec3 direction, vec3 up) {
	vec3 forward = normalize(direction);
	vec3 right = cross(forward, up);

	model[0] = vec4(right, 0);
	model[1] = vec4(up, 0);
	model[2] = vec4(-forward, 0);
	model[3] = vec4(position, 1);
}

void GLTFObject::setPositionDirection(vec3 position, vec3 direction) {
	setPositionDirection(position, direction, vec3(0, 1, 0));
}

void GLTFObject::setPositionDirection(vec2 position, float angle)
{
	
}

void GLTFObject::applyTransform(mat4 transform)
{
	model = transform * model;
}

vec3 GLTFObject::getPosition()
{
	return model[3].getXYZ();
}
