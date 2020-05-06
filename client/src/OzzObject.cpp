#include "OzzObject.h"
#include "Application.h"

VertexLayout OzzObject::pVertexLayoutSkinned = {
	5,
	{
		{SEMANTIC_POSITION, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 0, 0, 0, 0},
		{SEMANTIC_NORMAL, 0, NULL, TinyImageFormat_R32G32B32_SFLOAT, 1, 0, 3 * sizeof(float), 0},
		{SEMANTIC_TEXCOORD0, 0, NULL, TinyImageFormat_R32G32_SFLOAT, 2, 0, 6 * sizeof(float), 0},
		{SEMANTIC_WEIGHTS, 0, NULL, TinyImageFormat_R32G32B32A32_SFLOAT, 3, 0, 8 * sizeof(float), 0},
		{SEMANTIC_JOINTS, 0, NULL, TinyImageFormat_R16G16B16A16_UINT, 4, 0, 12 * sizeof(float), 0}
	}
};

OzzObject::OzzObject(Renderer* renderer, std::string directory)
{
	this->modelID = modelCount++;
	this->directory = directory;
	this->pRenderer = renderer;

	PathHandle skeletonPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/skeleton.ozz").c_str());
	
	// Initialize the rig with the path to its ozz file
	gStickFigureRig.Initialize(skeletonPath);

	PathHandle meshPath = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/riggedMesh.gltf").c_str());

	OzzObject::pVertexLayoutSkinned.mAttribCount = 5;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mSemantic = SEMANTIC_POSITION;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mLocation = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[0].mOffset = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mSemantic = SEMANTIC_NORMAL;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mLocation = 1;
	OzzObject::pVertexLayoutSkinned.mAttribs[1].mOffset = 3 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mSemantic = SEMANTIC_TEXCOORD0;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mFormat = TinyImageFormat_R32G32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mLocation = 2;
	OzzObject::pVertexLayoutSkinned.mAttribs[2].mOffset = 6 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mSemantic = SEMANTIC_WEIGHTS;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mFormat = TinyImageFormat_R32G32B32A32_SFLOAT;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mLocation = 3;
	OzzObject::pVertexLayoutSkinned.mAttribs[3].mOffset = 8 * sizeof(float);
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mSemantic = SEMANTIC_JOINTS;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mFormat = TinyImageFormat_R16G16B16A16_UINT;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mBinding = 0;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mLocation = 4;
	OzzObject::pVertexLayoutSkinned.mAttribs[4].mOffset = 12 * sizeof(float);

	GeometryLoadDesc loadDesc = {};
	loadDesc.pFilePath = meshPath;
	loadDesc.pVertexLayout = &OzzObject::pVertexLayoutSkinned;
	loadDesc.ppGeometry = &pGeom;
	addResource(&loadDesc, NULL, LOAD_PRIORITY_NORMAL);

	uint32_t res = gltfLoadContainer(meshPath, GLTF_FLAG_CALCULATE_BOUNDS, &pData);
	ASSERT(!res);

	mSamplers.resize(pData->mSamplerCount);
	for (uint32_t i = 0; i < pData->mSamplerCount; ++i)
		addSampler(pRenderer, pData->pSamplers + i, &mSamplers[i]);

	mTextures.resize(pData->pHandle->images_count);
	SyncToken token = {};
	for (uint32_t i = 0; i < pData->pHandle->images_count; ++i)
		gltfLoadTextureAtIndex(pData, meshPath, i, false, &token, &mTextures[i]);

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

	waitForAllResourceLoads();

	PathHandle animationDirectory = fsCopyPathInResourceDirectory(RD_ANIMATIONS, (directory + "/animations/").c_str());
	eastl::vector<PathHandle> actions = fsGetFilesWithExtension(animationDirectory, ".ozz");
	for (auto action : actions) {
		std::string actionPath(fsGetPathAsNativeString(action));
		char drive[256], dir[256], file[256], ext[256];
		_splitpath(actionPath.c_str(), drive, dir, file, ext);
		Clip* clip = conf_new(Clip);
		clips.emplace(std::string(file), clip);
		clips[file]->Initialize(action, &gStickFigureRig);
	}
}

void OzzObject::SetClip(std::string clipName)
{
	// CLIP CONTROLLERS
	//
	// Initialize with the length of the clip they are controlling and an
	// optional external time to set based on their updating
	gClipController.Initialize(clips[clipName]->GetDuration(), &this->time);


	// ANIMATIONS
	//
	AnimationDesc animationDesc{};
	animationDesc.mRig = &gStickFigureRig;
	animationDesc.mNumLayers = 1;
	animationDesc.mLayerProperties[0].mClip = clips[clipName];
	animationDesc.mLayerProperties[0].mClipController = &gClipController;

	gAnimation.Initialize(animationDesc);

	// ANIMATED OBJECTS
	//
	gStickFigureAnimObject.Initialize(&gStickFigureRig, &gAnimation);

}

void OzzObject::removeResources()
{
	GLTFObject::removeResources();

	gStickFigureRig.Destroy();
	for (auto& clip : clips) {
		clip.second->Destroy();
		conf_delete(clip.second);
	}
	gAnimation.Destroy();
	gStickFigureAnimObject.Destroy();
}

void OzzObject::update(float deltaTime)
{
	//gAnimationUpdateTimer.Reset();

	// Update the animated object for this frame
	if (!gStickFigureAnimObject.Update(deltaTime))
		LOGF(eINFO, "Animation NOT Updating!");

	//gStickFigureAnimObject.PoseRigInBind();
	gStickFigureAnimObject.PoseRig();


	// Record animation update time
	//gAnimationUpdateTimer.GetUSec(true);

	// Update uniforms that will be shared between all skeletons
	//gSkeletonBatcher.SetSharedUniforms(Application::projMat * Application::viewMat, vec3(0.0f, 1000.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));

	for (uint i = 0; i < pGeom->mJointCount; ++i) {
		gUniformDataBones.mBoneMatrix[i] = mat4::scale(vec3(100, 100, -100)) * gStickFigureRig.GetJointWorldMat(pGeom->pJointRemaps[i]) *
			//mat4::scale(vec3(1, 1, 1)) *
			pGeom->pInverseBindPoses[i];
		//print(gUniformDataBones.mBoneMatrix[i]);
	}
}

void OzzObject::draw(Cmd* cmd)
{
	//gSkeletonBatcher.SetPerInstanceUniforms(Application::gFrameIndex);

	cmdBindVertexBuffer(cmd, 1, &pGeom->pVertexBuffers[0], pGeom->mVertexStrides, (uint64_t*)NULL);
	cmdBindIndexBuffer(cmd, pGeom->pIndexBuffer, pGeom->mIndexType, (uint64_t)NULL);
	cmdBindDescriptorSet(cmd, 0, pMaterialSet);
	cmdDrawIndexed(cmd, pGeom->mIndexCount, 0, 0);
}
