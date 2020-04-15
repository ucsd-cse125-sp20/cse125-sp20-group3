#include "OBJObject.h"
#include "Application.h"

OBJObject::OBJObject(std::string objFilename, RootSignature* rootSignature, DescriptorSet* uniform) {
	pRootSignature = rootSignature;
	pDescUniforms = uniform;

	parse(objFilename.c_str());

	uint64_t       dataSize = points.size() * sizeof(vec3);
	BufferLoadDesc vbDesc = {};
	vbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
	vbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	vbDesc.mDesc.mSize = dataSize;
	vbDesc.pData = points.data();
	vbDesc.ppBuffer = &pVertexBuffer;
	addResource(&vbDesc, NULL, LOAD_PRIORITY_NORMAL);

	size = (int)points.size();
	points.clear();

	BufferLoadDesc ubDesc = {};
	ubDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	ubDesc.mDesc.mSize = sizeof(UniformBlock);
	ubDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
	ubDesc.pData = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)	{
		ubDesc.ppBuffer = &pUniformBuffer[i];
		addResource(&ubDesc, NULL, LOAD_PRIORITY_NORMAL);
	}
}

void OBJObject::prep(Renderer* renderer) {
	DescriptorData params[1] = {};

	if (pTexture) {
		params[0].pName = "Texture";
		params[0].ppTextures = &pTexture;
		updateDescriptorSet(renderer, 0, pDescTexture, 6, params);
	}

	for (uint32_t i = 0; i < Application::gImageCount; ++i) {
		params[0].pName = "uniformBlock";
		params[0].ppBuffers = &pUniformBuffer[i];
		updateDescriptorSet(renderer, i, pDescUniforms, 1, params);
	}
}

OBJObject::~OBJObject() {
	for (uint32_t i = 0; i < Application::gImageCount; ++i) {
		removeResource(pUniformBuffer[i]);
	}

	removeResource(pVertexBuffer);
}

void OBJObject::parse(const char* objFilename) {
	FILE* fp;
	float x, y, z;
	float r, g, b;
	unsigned int iv0, iv1, iv2, in0, in1, in2, it0, it1, it2;
	char c1, c2;
	std::vector<unsigned int> vInds, tInds, nInds;
	std::vector<vec3> inPoints, inNormals;
	std::vector<vec2> inUVs;

	fopen_s(&fp, objFilename, "rb");
	if (fp == NULL) { std::cerr << "error loading file " << objFilename << std::endl; exit(-1); }

	while ((c1 = fgetc(fp)) != EOF) {
		if ((c1 == 'v' || c1 == 'f') && (c2 = fgetc(fp)) != EOF) {
			if (c1 == 'v' && c2 == ' ') {
				fscanf_s(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				inPoints.push_back(vec3(x, y, z));

			}
			else if (c1 == 'v' && c2 == 'n') {
				fscanf_s(fp, "%f %f %f", &x, &y, &z);
				inNormals.push_back(vec3(x, y, z));
			}
			else if (c1 == 'v' && c2 == 't') {
				fscanf_s(fp, "%f %f", &x, &y);
				inUVs.push_back(vec2(x, y));
			}
			else if (c1 == 'f' && c2 == ' ') {
				fscanf_s(fp, "%u/%u/%u %u/%u/%u %u/%u/%u", &iv0, &it0, &in0, &iv1, &it1, &in1, &iv2, &it2, &in2);
				vInds.push_back(iv0 - 1);
				vInds.push_back(iv1 - 1);
				vInds.push_back(iv2 - 1);
				tInds.push_back(it0 - 1);
				tInds.push_back(it1 - 1);
				tInds.push_back(it2 - 1);
				nInds.push_back(in0 - 1);
				nInds.push_back(in1 - 1);
				nInds.push_back(in2 - 1);
			}
		}
	}

	for (unsigned i = 0; i < vInds.size(); i++) {
		VertexData data = {};
		data.position = vec4( inPoints[vInds[i]], 1);
		data.normal = vec4(inNormals[nInds[i]], 0);
		data.uv = inUVs[tInds[i]];
		points.push_back(data);
	}
}

void OBJObject::loadTexure(std::string filename, DescriptorSet* texDescSet) {
	pDescTexture = texDescSet;

	PathHandle textureFilePath = fsCopyPathInResourceDirectory(RD_TEXTURES, filename.c_str());
	TextureLoadDesc textureDesc = {};
	textureDesc.pFilePath = textureFilePath;
	textureDesc.ppTexture = &pTexture;
	addResource(&textureDesc, NULL, LOAD_PRIORITY_NORMAL);
}

void OBJObject::setTranslate(vec3 position) {
	model[3] = vec4(position, 1.0f);
}

void OBJObject::setScaleRot(vec3 scale, float deg, vec3 axis) {
	mat4 scaleRot = mat4::scale(vec3(scale)) * mat4::rotation(deg, axis);
	model[0] = scaleRot[0];
	model[1] = scaleRot[1];
	model[2] = scaleRot[2];
}

void OBJObject::setPositionDirection(vec3 position, vec3 direction, vec3 up) {
	vec3 forward = normalize(direction);
	vec3 right = cross(forward, up);

	model[0] = vec4(-forward, 0);
	model[1] = vec4(-right, 0);
	model[2] = vec4(up, 0);
	model[3] = vec4(position, 1);
}

void OBJObject::setPositionDirection(vec3 position, vec3 direction) {
	setPositionDirection(position, direction, vec3(0, 0, 1));
}

void OBJObject::load(Renderer* renderer, RenderTarget* target, RenderTarget* depthBuffer, Shader* program) {
	// Depth testing
	DepthStateDesc depthStateDesc = {};
	depthStateDesc.mDepthTest = true;
	depthStateDesc.mDepthWrite = true;
	depthStateDesc.mDepthFunc = CMP_GEQUAL;

	// Attribute layout
	VertexLayout vertexLayout = {};
	vertexLayout.mAttribCount = 3;

	vertexLayout.mAttribs[0].mSemantic = SEMANTIC_POSITION;
	vertexLayout.mAttribs[0].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	vertexLayout.mAttribs[0].mBinding = 0;
	vertexLayout.mAttribs[0].mLocation = 0;
	vertexLayout.mAttribs[0].mOffset = 0;

	vertexLayout.mAttribs[1].mSemantic = SEMANTIC_NORMAL;
	vertexLayout.mAttribs[1].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	vertexLayout.mAttribs[1].mBinding = 0;
	vertexLayout.mAttribs[1].mLocation = 1;
	vertexLayout.mAttribs[1].mOffset = 3 * sizeof(float);

	vertexLayout.mAttribs[2].mSemantic = SEMANTIC_TEXCOORD0;
	vertexLayout.mAttribs[2].mFormat = TinyImageFormat_R32G32_SFLOAT;
	vertexLayout.mAttribs[2].mBinding = 0;
	vertexLayout.mAttribs[2].mLocation = 2;
	vertexLayout.mAttribs[2].mOffset = 6 * sizeof(float);

	// Rasterizer settings
	RasterizerStateDesc rasterizerStateDesc = {};
	rasterizerStateDesc.mCullMode = CULL_MODE_FRONT;

	// Pipeline construction
	PipelineDesc desc = {};
	desc.mType = PIPELINE_TYPE_GRAPHICS;
	GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
	pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
	pipelineSettings.mRenderTargetCount = 1;
	pipelineSettings.pDepthState = &depthStateDesc;
	pipelineSettings.pColorFormats = &target->mFormat;
	pipelineSettings.mSampleCount = target->mSampleCount;
	pipelineSettings.mSampleQuality = target->mSampleQuality;
	pipelineSettings.mDepthStencilFormat = depthBuffer->mFormat;
	pipelineSettings.pRootSignature = pRootSignature;
	pipelineSettings.pShaderProgram = program;
	pipelineSettings.pVertexLayout = &vertexLayout;
	pipelineSettings.pRasterizerState = &rasterizerStateDesc;
	addPipeline(renderer, &desc, &pPipeline);
}

void OBJObject::profile(ProfileToken profileToken) {
	this->profileToken = profileToken;
}

void OBJObject::unload(Renderer* renderer) {
	removePipeline(renderer, pPipeline);
}

void OBJObject::update(float deltaTime) {
	uniformData.mColor = vec4(color, 1.0);
	uniformData.mToWorld = model;
	uniformData.mProjectView = Application::projMat * Application::viewMat;

	return;
}

void OBJObject::draw(Cmd* commands) {
	// Update uniforms
	BufferUpdateDesc cbv = { pUniformBuffer[Application::gFrameIndex] };
	beginUpdateResource(&cbv);
	*(UniformBlock*)cbv.pMappedData = uniformData;
	endUpdateResource(&cbv, NULL);

	// Draw
	const uint32_t vbStride = sizeof(VertexData);
	if (profileToken != -1) cmdBeginGpuTimestampQuery(commands, profileToken, "Draw OBJ");
	cmdBindPipeline(commands, pPipeline);
	cmdBindDescriptorSet(commands, Application::gFrameIndex, pDescUniforms);
	cmdBindVertexBuffer(commands, 1, &pVertexBuffer, &vbStride, NULL);
	cmdDraw(commands, size, 0);
	if (profileToken != -1) cmdEndGpuTimestampQuery(commands, profileToken);
}

