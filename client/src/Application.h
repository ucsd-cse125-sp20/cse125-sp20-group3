#ifndef _Application_H_
#define _Application_H_

#include <ctime>
#include <vector>

#include "../The-Forge/Common_3/OS/Interfaces/IApp.h"

#include "Client.h"

#include "Input.h"

#include "SceneManager_Client.h"
#include "AudioManager.h"

#include "UIUtils.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/imgui/imgui.h"

#include "../../common/client2server.h"


#define IMAGE_COUNT 3
#define LIGHT_COUNT 3

class Application : public IApp
{
public:

	struct UniformBlock
	{
		mat4 mProjectView;
		mat4 mProj;
		mat4 mView;
		mat4 mShadowLightViewProj;
		vec4 mCameraPosition;
		vec4 mLightColor[LIGHT_COUNT + 1];
		vec4 mLightDirection[LIGHT_COUNT];
	};

	struct UniformBlock_Shadow
	{
		mat4 ViewProj;
		mat4 mModel;
	};

	struct FXAAINFO
	{
		vec2 ScreenSize;
		uint Use;
		uint padding00;
	};

	static UIApp gAppUI;

    static uint32_t gImageCount, gFrameIndex;
    static mat4 projMat, viewMat;

	Application();

	static bool InitSceneResources();
	static void RemoveSceneResources();
    static bool InitShaderResources();
	static void RemoveShaderResources();

	void InitDebugGui();
	static void ToggleClient();

    bool Init() override;
	void Exit() override;

	static void PrepareDescriptorSets();
	static void LoadPipelines();
	static void RemovePipelines();
	bool Load() override;
	void Unload() override;

	void Update(float deltaTime) override;
	void Draw() override;

	static void setRenderTarget(Cmd* cmd, uint32_t count, RenderTarget** pDestinationRenderTargets, RenderTarget* pDepthStencilTarget, LoadActionsDesc* loadActions);
	static void drawShadowMap(Cmd* cmd);

	bool addSwapChain();
	bool addRenderTargets();
	bool addDepthBuffer();

	static void getFrustumCorners(mat4 frustum, vec3 corners[8]);

	const char* Application::GetName() { return "client"; }
};

#endif