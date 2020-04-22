#ifndef _Application_H_
#define _Application_H_

#include "../The-Forge/Common_3/OS/Interfaces/IApp.h"

#include "Input.h"

#include "GLTFGeode.h"
#include "Transform.h"

#define IMAGE_COUNT 3
#define LIGHT_COUNT 3
#define MAX_INSTANCES 50

class Application : public IApp
{
public:

	static Texture* pTextureBlack;

	struct UniformBlock
	{
		mat4 mProjectView;
		mat4 mModel;
		mat4 mShadowLightViewProj;
		vec4 mCameraPosition;
		vec4 mLightColor[LIGHT_COUNT + 1];
		vec4 mLightDirection[LIGHT_COUNT];
	};

	struct UniformBlock_Instance
	{
		mat4 mModel;
		//vec3 baseColor;
	};

	struct UniformBlock_Shadow
	{
		mat4 ViewProj;
		mat4 mModel;
	};

	struct UniformBlock_Floor
	{
		mat4	worldMat;
		mat4	projViewMat;
		vec4	screenSize;
	};

	struct FXAAINFO
	{
		vec2 ScreenSize;
		uint Use;
		uint padding00;
	};

    static uint32_t gImageCount, gFrameIndex;
    static mat4 projMat, viewMat;

	Application();
    static bool InitShaderResources();
    static bool InitSceneResources();
    static void setRenderTarget(Cmd* cmd, uint32_t count, RenderTarget** pDestinationRenderTargets, RenderTarget* pDepthStencilTarget, LoadActionsDesc* loadActions);
    static void drawShadowMap(Cmd* cmd);
    bool Init();
	static bool AddDescriptorSets();
	static void RemoveDescriptorSets();
	static void PrepareDescriptorSets();
	static void RemoveShaderResources();
	static void RemoveSceneResources();
	void Exit();
	static void LoadPipelines();
	bool Load();
	static void RemovePipelines();
	void Unload();
	void Update(float deltaTime);
	void PostDrawUpdate();
	static void SelectModelFunc(const Path* path, void* pathPtr);
	static void LoadNewModel();
	static void LoadLOD();
	void Draw();
	const char* GetName();
	bool addSwapChain();
	bool addRenderTargets();
	bool addDepthBuffer();
	void RecenterCameraView(float maxDistance, vec3 lookAt);
};

#endif