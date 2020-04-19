#pragma once

#include <string>
#include <iostream>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
//#include "../The-Forge/Common_3/OS/Interfaces/IProfiler.h"

#include "../The-Forge/Common_3/ThirdParty/OpenSource/cgltf/GLTFLoader.h"

#include "Object.h"

class GLTFObject
{
public:
	Texture* pTextureBlack = NULL;

	struct MeshPushConstants
	{
		uint32_t nodeIndex;
	};

	struct GLTFTextureProperties
	{
		int16_t mTextureIndex;
		int16_t mSamplerIndex;
		int32_t mUVStreamIndex;
		float mRotation;
		float mValueScale;
		float2 mOffset;
		float2 mScale;
	};

	struct GLTFMaterialData
	{
		uint32_t mAlphaMode;
		float mAlphaCutoff;
		float2 mEmissiveGBScale;

		float4 mBaseColorFactor;
		float4 mMetallicRoughnessFactors; // RG, or specular RGB + A glossiness

		GLTFTextureProperties mBaseColorProperties;
		GLTFTextureProperties mMetallicRoughnessProperties;

		GLTFTextureProperties mNormalTextureProperties;
		GLTFTextureProperties mOcclusionTextureProperties;
		GLTFTextureProperties mEmissiveTextureProperties;
	};

	Renderer* pRenderer = NULL;
	Geometry* pGeom = NULL;
	GLTFContainer* pData = NULL;
	eastl::vector<Texture*> mTextures;
	eastl::vector<Sampler*> mSamplers;
	DescriptorSet* pMaterialSet = NULL;
	Buffer* pNodeTransformsBuffer = NULL;
	Buffer* pMaterialBuffer = NULL;
	Sampler* pDefaultSampler = NULL;

	mat4 model = mat4::identity();
	mat4* initialNodeTransforms, * nodeTransforms;

	static VertexLayout pVertexLayoutModel;

	static bool LoadModel(GLTFObject* asset, Renderer* renderer, Sampler* sampler, const Path* modelFilePath);

	void Init(const Path* path, Renderer* renderer, Sampler* defaultSampler);

	static VertexLayout getVertexLayout() { return pVertexLayoutModel; }

	void updateTransform(size_t nodeIndex, mat4* nodeTransforms, bool* nodeTransformsInited);
	void createNodeTransformsBuffer();

	void updateTextureProperties(const GLTFTextureView& textureView, GLTFTextureProperties& textureProperties);
	void updateParam(DescriptorData* params, const GLTFTextureView& textureView, const char* textureName, const char* samplerName, uint32_t& nextParamIdx);

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet = NULL);
	void bindBuffers(Cmd* cmd);
	void removeResources();

	void update(float deltaTime);
	void draw(Cmd* cmd, RootSignature* rootSignature, bool useMaterial);

	void setTranslate(vec3 position);
	void setScaleRot(vec3 scale, float deg, vec3 axis);

	void setPositionDirection(vec3 position, vec3 direction, vec3 up);
	void setPositionDirection(vec3 position, vec3 direction);
	void setPositionDirection(vec2 position, float angle);

	vec3 getPosition();
};