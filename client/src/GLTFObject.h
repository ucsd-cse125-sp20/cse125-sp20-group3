#pragma once

#include <string>
#include <iostream>

#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"

#include "../The-Forge/Common_3/ThirdParty/OpenSource/cgltf/GLTFLoader.h"

#include "Object.h"

#define MAX_GLTF_NODES 100
#define MAX_GLTF_MODELS 50

class GLTFObject : public Object
{
public:
	Texture* pTextureBlack;

	struct MeshPushConstants
	{
		uint32_t nodeIndex;
		uint32_t instanceIndex;
		uint32_t modelIndex;
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
	static Buffer* pNodeTransformsBuffer;
	Buffer* pMaterialBuffer = NULL;
	Sampler* pDefaultSampler = NULL;

	RootSignature* defaultRootSignature = NULL;

	mat4 model = mat4::identity();
	vec3 baseColor = vec3(1);

	float radius = 0;

	mat4* initialNodeTransforms, * nodeTransforms;

	static VertexLayout pVertexLayoutModel;


	int instanceID;
	static int instanceCount;
	static bool countingInstances;
	static int modelCount;
	int modelID;



	static bool LoadModel(GLTFObject* asset, Renderer* renderer, const Path* modelFilePath);

	void Init(const Path* path, Renderer* renderer);

	static VertexLayout getVertexLayout() { return pVertexLayoutModel; }

	void updateTransform(size_t nodeIndex, mat4* nodeTransforms, bool* nodeTransformsInited);
	void createNodeTransformsBuffer();

	void updateTextureProperties(const GLTFTextureView& textureView, GLTFTextureProperties& textureProperties);
	void updateParam(DescriptorData* params, const GLTFTextureView& textureView, const char* textureName, const char* samplerName, uint32_t& nextParamIdx, Sampler* defaultSampler);

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);
	void bindBuffers(Cmd* cmd);
	void removeResources();

	void update(float deltaTime);
	void draw(Cmd* cmd, RootSignature* rootSignature, bool useMaterial);
	void draw(Cmd* cmd);

	void setTranslate(vec3 position);
	void setScaleRot(vec3 scale, float deg, vec3 axis);

	void setPositionDirection(vec3 position, vec3 direction, vec3 up);
	void setPositionDirection(vec3 position, vec3 direction);
	void setPositionDirection(vec2 position, float angle);

	void applyTransform(mat4 transform);

	vec3 getPosition();
};