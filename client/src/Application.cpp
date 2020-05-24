/*
 * Copyright (c) 2018-2020 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "Application.h"

#include "../The-Forge/Common_3/ThirdParty/OpenSource/EASTL/vector.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/EASTL/string.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/EASTL/unordered_map.h"

 //Interfaces
#include "../The-Forge/Common_3/OS/Interfaces/ICameraController.h"
#include "../The-Forge/Common_3/OS/Interfaces/ILog.h"
#include "../The-Forge/Common_3/OS/Interfaces/IFileSystem.h"
#include "../The-Forge/Common_3/OS/Interfaces/ITime.h"
#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/Renderer/IRenderer.h"
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include "../The-Forge/Common_3/OS/Interfaces/IProfiler.h"

//Math
#include "../The-Forge/Common_3/OS/Math/MathTypes.h"
#include "../The-Forge/Common_3/OS/Interfaces/IMemory.h"    // Must be last include in cpp file


//--------------------------------------------------------------------------------------------
// GLOBAL DEFINTIONS
//--------------------------------------------------------------------------------------------

uint32_t Application::gFrameIndex = 0;
uint32_t Application::gImageCount = 3;

const uint32_t		gLightCount = 3;
const uint32_t		gTotalLightCount = gLightCount + 1;

// Model Quantization Settings
int					gCurrentLod = 0;
int					gMaxLod = 5;

bool				bToggleCull = true;
bool				bToggleFXAA = true;
bool				bVignetting = true;
bool				bToggleVSync = false;

ProfileToken   gGpuProfileToken;
//--------------------------------------------------------------------------------------------
// PRE PROCESSORS
//--------------------------------------------------------------------------------------------

#define SHADOWMAP_MSAA_SAMPLES 1

#if defined(TARGET_IOS) || defined(__ANDROID__)
#define SHADOWMAP_RES 1024u
#else
#define SHADOWMAP_RES 2048u
#endif

#if !defined(TARGET_IOS) && !defined(__ANDROID__) && !defined(ORBIS)
#define USE_BASIS 1
#endif

//--------------------------------------------------------------------------------------------
// RENDERING PIPELINE DATA
//--------------------------------------------------------------------------------------------

Renderer* pRenderer = NULL;

Queue* pGraphicsQueue = NULL;

CmdPool* pCmdPool = NULL;
Cmd** ppCmds = NULL;

SwapChain* pSwapChain = NULL;

RenderTarget* pForwardRT = NULL;
RenderTarget* pPostProcessRT = NULL;
RenderTarget* pDepthBuffer = NULL;
RenderTarget* pShadowRT = NULL;

Fence* pRenderCompleteFences[IMAGE_COUNT] = { NULL };

Semaphore* pImageAcquiredSemaphore = NULL;
Semaphore* pRenderCompleteSemaphores[IMAGE_COUNT] = { NULL };

Shader* pShaderZPass = NULL;
Shader* pMeshOptDemoShader = NULL;
Shader* pShaderSkinning = NULL;
Shader* pVignetteShader = NULL;
Shader* pFXAAShader = NULL;

Pipeline* pPipelineShadowPass = NULL;
Pipeline* pMeshOptDemoPipeline = NULL;
Pipeline* pPipelineSkinning = NULL;
Pipeline* pVignettePipeline = NULL;
Pipeline* pFXAAPipeline = NULL;

RootSignature* pRootSignatureShadow = NULL;
RootSignature* pRootSignatureShaded = NULL;
RootSignature* pRootSignatureSkinning = NULL; // TODO merge into shaded RS
RootSignature* pRootSignaturePostEffects = NULL;

DescriptorSet* pDescriptorSetVignette;
DescriptorSet* pDescriptorSetFXAA;
DescriptorSet* pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_COUNT];
DescriptorSet* pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_COUNT];
DescriptorSet* pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_COUNT];

VirtualJoystickUI   gVirtualJoystick = {};

Buffer* pUniformBuffer[IMAGE_COUNT] = { NULL };
Buffer* pInstanceBuffer[IMAGE_COUNT] = { NULL };
Buffer* pUniformBufferBones[IMAGE_COUNT] = { NULL };
Buffer* pShadowUniformBuffer[IMAGE_COUNT] = { NULL };
Buffer* pShadowInstanceBuffer[IMAGE_COUNT] = { NULL };

Buffer* TriangularVB = NULL;

Sampler* pDefaultSampler = NULL;
Sampler* pBilinearClampSampler = NULL;

Application::UniformBlock		gUniformData;
Application::UniformBlock_Shadow gShadowUniformData;
std::vector<mat4> instanceData;
std::vector<mat4> shadowInstanceData;

//--------------------------------------------------------------------------------------------
// THE FORGE OBJECTS
//--------------------------------------------------------------------------------------------

ICameraController* pCameraController = NULL;
ICameraController* pLightView = NULL;

GuiComponent* pDebugGui;
GuiComponent* pTestGui;

Texture* testImage;

UIApp gAppUI;

Input inputHandler;

TextDrawDesc gFrameTimeDraw = TextDrawDesc(0, 0xff00ffff, 18);

#if defined(__ANDROID__) || defined(__LINUX__)
uint32_t			modelToLoadIndex = 0;
uint32_t			guiModelToLoadIndex = 0;
#endif

const wchar_t* gMissingTextureString = L"MissingTexture";

const uint			gBackroundColor = { 0xb2b2b2ff };
static uint			gLightColor[gTotalLightCount] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffff66 };
static float		gLightColorIntensity[gTotalLightCount] = { 1.0f, 0.2f, 0.2f, 0.25f };
static float2		gLightDirection = { -122.0f, 222.0f };

mat4 Application::viewMat = mat4::identity();
mat4 Application::projMat = mat4::identity();

vec3 cameraOffset(0, 5, -5);

float rot = 0.f;

Client* client;
SceneManager_Client* scene;

bool connected = false;
const int serverNameSize = 32;
char serverName[serverNameSize] = "localhost";
char sendbuf[DEFAULT_BUFLEN];
//char recvbuf[SERVER_SENDBUFLEN];
Client::UpData updateData;

// ======================================================================================================
// ==============================================[ CODE STARTS HERE ]====================================
// ======================================================================================================

Application::Application()
{
#ifdef TARGET_IOS
	mSettings.mContentScaleFactor = 1.f;
#endif
}

// ======================================================================================================
// ==============================================[ SCENE INITIALIZATION ]================================
// ======================================================================================================

bool Application::InitSceneResources()
{
	scene = conf_new(SceneManager_Client, pRenderer);

	waitForAllResourceLoads();

	scene->createMaterialResources(SceneManager_Client::GeodeType::MESH, pRootSignatureShaded, NULL, pDefaultSampler);
	scene->createMaterialResources(SceneManager_Client::GeodeType::ANIMATED_MESH, pRootSignatureSkinning, NULL, pDefaultSampler);

	scene->setBuffer(SceneManager_Client::SceneBuffer::INSTANCE, pInstanceBuffer);
	scene->setBuffer(SceneManager_Client::SceneBuffer::BONE, pUniformBufferBones);

	return true;
}

void Application::RemoveSceneResources()
{
	conf_delete(scene);
}

// ======================================================================================================
// ==============================================[ SHADER MANAGEMENT ]===================================
// ======================================================================================================

bool Application::InitShaderResources()
{
	// Create shaders
	ShaderLoadDesc MeshOptDemoShader = {};
	MeshOptDemoShader.mStages[0] = { "basic.vert", NULL, 0, RD_SHADER_SOURCES };
	addShader(pRenderer, &MeshOptDemoShader, &pShaderZPass);

	MeshOptDemoShader.mStages[1] = { "basic.frag", NULL, 0, RD_SHADER_SOURCES };
	addShader(pRenderer, &MeshOptDemoShader, &pMeshOptDemoShader);

	ShaderLoadDesc skinningShader = {};
	skinningShader.mStages[0] = { "skinning.vert", NULL, 0, RD_SHADER_SOURCES };
	skinningShader.mStages[1] = { "basic.frag", NULL, 0, RD_SHADER_SOURCES };
	addShader(pRenderer, &skinningShader, &pShaderSkinning);

	ShaderLoadDesc VignetteShader = {};
	VignetteShader.mStages[0] = { "Triangular.vert", NULL, 0, RD_SHADER_SOURCES };
	VignetteShader.mStages[1] = { "vignette.frag", NULL, 0, RD_SHADER_SOURCES };
	addShader(pRenderer, &VignetteShader, &pVignetteShader);

	ShaderLoadDesc FXAAShader = {};
	FXAAShader.mStages[0] = { "Triangular.vert", NULL, 0, RD_SHADER_SOURCES };
	FXAAShader.mStages[1] = { "FXAA.frag", NULL, 0, RD_SHADER_SOURCES };
	addShader(pRenderer, &FXAAShader, &pFXAAShader);

	// Create root signatures
	const char* pStaticSamplerNames[] = { "clampMiplessLinearSampler" };
	Sampler* pStaticSamplers[] = { pBilinearClampSampler };
	Shader* shaders[] = { pShaderZPass };
	RootSignatureDesc rootDesc = {};
	rootDesc.mStaticSamplerCount = 1;
	rootDesc.ppStaticSamplerNames = pStaticSamplerNames;
	rootDesc.ppStaticSamplers = pStaticSamplers;
	rootDesc.mShaderCount = 1;
	rootDesc.ppShaders = shaders;
	addRootSignature(pRenderer, &rootDesc, &pRootSignatureShadow);

	Shader* demoShaders[] = { pMeshOptDemoShader };
	rootDesc.mShaderCount = 1;
	rootDesc.ppShaders = demoShaders;
	addRootSignature(pRenderer, &rootDesc, &pRootSignatureShaded);

	//Shader* demoShaders[] = { pMeshOptDemoShader, pShaderSkinning };
	//rootDesc.mShaderCount = 2;
	//rootDesc.ppShaders = demoShaders;
	//addRootSignature(pRenderer, &rootDesc, &pRootSignatureShaded);

	rootDesc.mShaderCount = 1;
	rootDesc.ppShaders = &pShaderSkinning;
	addRootSignature(pRenderer, &rootDesc, &pRootSignatureSkinning);

	Shader* postShaders[] = { pVignetteShader, pFXAAShader };
	rootDesc.mShaderCount = 2;
	rootDesc.ppShaders = postShaders;
	addRootSignature(pRenderer, &rootDesc, &pRootSignaturePostEffects);

	// Create descriptor sets
	DescriptorSetDesc setDesc = { pRootSignaturePostEffects, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetVignette);
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetFXAA);

	setDesc = { pRootSignatureShadow, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE]);
	setDesc = { pRootSignatureShadow, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	setDesc = { pRootSignatureShadow, DESCRIPTOR_UPDATE_FREQ_PER_BATCH, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	setDesc = { pRootSignatureShaded, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
	setDesc = { pRootSignatureShaded, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	setDesc = { pRootSignatureShaded, DESCRIPTOR_UPDATE_FREQ_PER_BATCH, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	setDesc = { pRootSignatureSkinning, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_NONE]);
	setDesc = { pRootSignatureSkinning, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	setDesc = { pRootSignatureSkinning, DESCRIPTOR_UPDATE_FREQ_PER_BATCH, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	return true;
}



void Application::RemoveShaderResources()
{
	removeDescriptorSet(pRenderer, pDescriptorSetVignette);
	removeDescriptorSet(pRenderer, pDescriptorSetFXAA);

	removeDescriptorSet(pRenderer, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	removeDescriptorSet(pRenderer, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	removeDescriptorSet(pRenderer, pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_NONE]);
	removeDescriptorSet(pRenderer, pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	removeDescriptorSet(pRenderer, pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_BATCH]);

	removeShader(pRenderer, pShaderZPass);
	removeShader(pRenderer, pVignetteShader);
	removeShader(pRenderer, pMeshOptDemoShader);
	removeShader(pRenderer, pShaderSkinning);
	removeShader(pRenderer, pFXAAShader);

	removeRootSignature(pRenderer, pRootSignatureShadow);
	removeRootSignature(pRenderer, pRootSignatureShaded);
	removeRootSignature(pRenderer, pRootSignatureSkinning);
	removeRootSignature(pRenderer, pRootSignaturePostEffects);
}

// ======================================================================================================
// ==============================================[ USER INTERFACE ]======================================
// ======================================================================================================

void Application::InitDebugGui()
{
	GuiDesc guiDesc = {};
	guiDesc.mStartSize = vec2(400.0f, 20.0f);
	guiDesc.mStartPosition = vec2(100, 0);
	pDebugGui = gAppUI.AddGuiComponent("Client Settings", &guiDesc);

	CollapsingHeaderWidget NetworkWidgets("Network Settings", false, false);
	NetworkWidgets.AddSubWidget(TextboxWidget("Server Name", serverName, serverNameSize));
	CheckboxWidget toggleServerButtonWidget("Toggle Server Connection", &connected);
	toggleServerButtonWidget.pOnEdited = Application::ToggleClient;
	NetworkWidgets.AddSubWidget(toggleServerButtonWidget);
	pDebugGui->AddWidget(NetworkWidgets);

	CollapsingHeaderWidget PerformanceWidgets("Performance Settings");
	PerformanceWidgets.AddSubWidget(CheckboxWidget("Toggle Culling", &bToggleCull));
	PerformanceWidgets.AddSubWidget(CheckboxWidget("Toggle VSync", &bToggleVSync));
	pDebugGui->AddWidget(PerformanceWidgets);

	CollapsingHeaderWidget RenderWidgets("Render Settings");
	RenderWidgets.AddSubWidget(CheckboxWidget("Enable FXAA", &bToggleFXAA));
	RenderWidgets.AddSubWidget(CheckboxWidget("Enable Vignetting", &bVignetting));
	pDebugGui->AddWidget(RenderWidgets);

	CollapsingHeaderWidget LightWidgets("Lighting Settings");
	LightWidgets.AddSubWidget(SliderFloatWidget("Light Azimuth", &gLightDirection.x, float(-180.0f), float(180.0f), float(0.001f)));
	LightWidgets.AddSubWidget(SliderFloatWidget("Light Elevation", &gLightDirection.y, float(210.0f), float(330.0f), float(0.001f)));

	LightWidgets.AddSubWidget(SeparatorWidget());

	CollapsingHeaderWidget LightColor1Picker("Main Light Color");
	LightColor1Picker.AddSubWidget(ColorPickerWidget("Main Light Color", &gLightColor[0]));
	LightWidgets.AddSubWidget(LightColor1Picker);

	CollapsingHeaderWidget LightColor1Intensity("Main Light Intensity");
	LightColor1Intensity.AddSubWidget(SliderFloatWidget("Main Light Intensity", &gLightColorIntensity[0], 0.0f, 5.0f, 0.001f));
	LightWidgets.AddSubWidget(LightColor1Intensity);

	LightWidgets.AddSubWidget(SeparatorWidget());

	CollapsingHeaderWidget AmbientLightColorPicker("Ambient Light Color");
	AmbientLightColorPicker.AddSubWidget(ColorPickerWidget("Ambient Light Color", &gLightColor[3]));
	LightWidgets.AddSubWidget(AmbientLightColorPicker);

	CollapsingHeaderWidget LightColor4Intensity("Ambient Light Intensity");
	LightColor4Intensity.AddSubWidget(SliderFloatWidget("Light Intensity", &gLightColorIntensity[3], 0.0f, 5.0f, 0.001f));
	LightWidgets.AddSubWidget(LightColor4Intensity);

	pDebugGui->AddWidget(LightWidgets);



	/*
	GuiDesc testGuiDesc = {};
	testGuiDesc.mStartSize = vec2(100, 100);
	testGuiDesc.mStartPosition = vec2((float)mSettings.mWidth - 1000, (float)mSettings.mHeight - 1000);
	pTestGui = gAppUI.AddGuiComponent("asdf", &testGuiDesc);
	pTestGui->mFlags |= GUI_COMPONENT_FLAGS_NO_TITLE_BAR;
	pTestGui->mFlags |= GUI_COMPONENT_FLAGS_ALWAYS_USE_WINDOW_PADDING;
	pTestGui->mFlags |= GUI_COMPONENT_FLAGS_NO_RESIZE;
	pTestGui->mFlags |= GUI_COMPONENT_FLAGS_NO_COLLAPSE;
	pTestGui->mFlags |= GUI_COMPONENT_FLAGS_NO_SCROLLBAR;
	pTestGui->mAlpha = 0.9f;

	PathHandle testImagePath = fsCopyPathInResourceDirectory(RD_TEXTURES, "bot.png");
	TextureLoadDesc texDesc = {};
	texDesc.pFilePath = testImagePath;
	texDesc.ppTexture = &testImage;
	addResource(&texDesc, NULL, LOAD_PRIORITY_LOW);
	waitForAllResourceLoads();
	
	TextureButtonWidget texWidget("");
	texWidget.SetTexture(testImage, float2(200, 200));
	texWidget.pOnDeactivatedAfterEdit = []() { printf("Boop\n"); };
	pTestGui->AddWidget(texWidget);
	*/
}

void Application::ToggleClient()
{
	if (!connected) {
		client->closeConnection(SD_SEND);
		conf_delete(client);
	}
	else {
		client = conf_new(Client, serverName);
		int myPlayerID = client->recvPlayerID();
		std::cout << "myPlayerID: " << myPlayerID << "\n";
		scene->trackPlayer(myPlayerID);
	}
}

// ======================================================================================================
// ==============================================[ APPLICATION MANAGEMENT ]==============================
// ======================================================================================================

bool Application::Init()
{
#if defined(TARGET_IOS)
	fsRegisterUTIForExtension("dyn.ah62d4rv4ge80s5dyq2", "gltf");
#endif

	// FILE PATHS
	PathHandle programDirectory = fsCopyProgramDirectoryPath();
	if (!fsPlatformUsesBundledResources())
	{
		PathHandle resourceDirRoot = fsAppendPathComponent(programDirectory, "../../../../../../src");
		fsSetResourceDirectoryRootPath(resourceDirRoot);

		fsSetRelativePathForResourceDirectory(RD_TEXTURES, "../Assets/Textures");
		fsSetRelativePathForResourceDirectory(RD_MESHES, "../Assets/Meshes");
		fsSetRelativePathForResourceDirectory(RD_BUILTIN_FONTS, "../Assets/Fonts");
		fsSetRelativePathForResourceDirectory(RD_ANIMATIONS, "../Assets/Animation");
		fsSetRelativePathForResourceDirectory(RD_MIDDLEWARE_TEXT, "../The-Forge/Middleware_3/Text");
		fsSetRelativePathForResourceDirectory(RD_MIDDLEWARE_UI, "../The-Forge/Middleware_3/UI");
	}

	// window and renderer setup
	RendererDesc settings = { 0 };
	initRenderer(GetName(), &settings, &pRenderer);
	//check for init success
	if (!pRenderer)
		return false;

	QueueDesc queueDesc = {};
	queueDesc.mType = QUEUE_TYPE_GRAPHICS;
	queueDesc.mFlag = QUEUE_FLAG_INIT_MICROPROFILE;
	addQueue(pRenderer, &queueDesc, &pGraphicsQueue);

	CmdPoolDesc cmdPoolDesc = {};
	cmdPoolDesc.pQueue = pGraphicsQueue;
	addCmdPool(pRenderer, &cmdPoolDesc, &pCmdPool);
	CmdDesc cmdDesc = {};
	cmdDesc.pPool = pCmdPool;
	addCmd_n(pRenderer, &cmdDesc, Application::gImageCount, &ppCmds);

	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		addFence(pRenderer, &pRenderCompleteFences[i]);
		addSemaphore(pRenderer, &pRenderCompleteSemaphores[i]);
	}
	addSemaphore(pRenderer, &pImageAcquiredSemaphore);

	initResourceLoaderInterface(pRenderer);

	if (!gAppUI.Init(pRenderer))
		return false;

	gAppUI.LoadFont("TitilliumText/TitilliumText-Bold.otf", RD_BUILTIN_FONTS);

	initProfiler();

	gGpuProfileToken = addGpuProfiler(pRenderer, pGraphicsQueue, "GpuProfiler");

#if defined(__ANDROID__) || defined(TARGET_IOS)
	if (!gVirtualJoystick.Init(pRenderer, "circlepad", RD_TEXTURES))
	{
		LOGF(LogLevel::eERROR, "Could not initialize Virtual Joystick.");
		return false;
	}
#endif

	SamplerDesc defaultSamplerDesc = {
		FILTER_LINEAR, FILTER_LINEAR, MIPMAP_MODE_LINEAR,
		ADDRESS_MODE_REPEAT, ADDRESS_MODE_REPEAT, ADDRESS_MODE_REPEAT
	};
	addSampler(pRenderer, &defaultSamplerDesc, &pDefaultSampler);

	SamplerDesc samplerClampDesc = {
		FILTER_LINEAR, FILTER_LINEAR, MIPMAP_MODE_LINEAR,
		ADDRESS_MODE_CLAMP_TO_EDGE, ADDRESS_MODE_CLAMP_TO_EDGE, ADDRESS_MODE_CLAMP_TO_EDGE
	};
	addSampler(pRenderer, &samplerClampDesc, &pBilinearClampSampler);


	float screenTriangularPoints[] =
	{
		-1.0f,  3.0f, 0.5f, 0.0f, -1.0f,
		-1.0f, -1.0f, 0.5f, 0.0f, 1.0f,
		3.0f, -1.0f, 0.5f, 2.0f, 1.0f,
	};

	BufferLoadDesc screenQuadVbDesc = {};
	screenQuadVbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
	screenQuadVbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	screenQuadVbDesc.mDesc.mSize = sizeof(float) * 5 * 3;
	screenQuadVbDesc.pData = screenTriangularPoints;
	screenQuadVbDesc.ppBuffer = &TriangularVB;
	addResource(&screenQuadVbDesc, NULL, LOAD_PRIORITY_NORMAL);


	BufferLoadDesc ubDesc = {};
	ubDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	ubDesc.mDesc.mSize = sizeof(UniformBlock);
	ubDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
	ubDesc.pData = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		ubDesc.ppBuffer = &pUniformBuffer[i];
		addResource(&ubDesc, NULL, LOAD_PRIORITY_NORMAL);
	}

	BufferLoadDesc ibDesc = {};
	ibDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_BUFFER;
	ibDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	ibDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_NONE;
	ibDesc.mDesc.mSize = sizeof(mat4) * MAX_GEOMETRY_INSTANCES;
	ibDesc.mDesc.mFirstElement = 0;
	ibDesc.mDesc.mElementCount = MAX_GEOMETRY_INSTANCES;
	ibDesc.mDesc.mStructStride = sizeof(mat4);
	ibDesc.mDesc.pCounterBuffer = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		ibDesc.pData = instanceData.data();
		ibDesc.ppBuffer = &pInstanceBuffer[i];
		addResource(&ibDesc, NULL, LOAD_PRIORITY_NORMAL);

		ibDesc.pData = shadowInstanceData.data();
		ibDesc.ppBuffer = &pShadowInstanceBuffer[i];
		addResource(&ibDesc, NULL, LOAD_PRIORITY_NORMAL);
	}

	BufferLoadDesc subDesc = {};
	subDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	subDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	subDesc.mDesc.mSize = sizeof(UniformBlock_Shadow);
	subDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
	subDesc.pData = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		subDesc.ppBuffer = &pShadowUniformBuffer[i];
		addResource(&subDesc, NULL, LOAD_PRIORITY_NORMAL);
	}

	BufferLoadDesc boneBufferDesc = {};
	boneBufferDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_BUFFER;
	boneBufferDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	boneBufferDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_NONE;
	boneBufferDesc.mDesc.mSize = sizeof(mat4) * MAX_GEOMETRY_INSTANCES * MAX_NUM_BONES;
	boneBufferDesc.mDesc.mElementCount = MAX_GEOMETRY_INSTANCES * MAX_NUM_BONES;
	boneBufferDesc.mDesc.mStructStride = sizeof(mat4);
	boneBufferDesc.pData = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		boneBufferDesc.ppBuffer = &pUniformBufferBones[i];
		addResource(&boneBufferDesc, NULL, LOAD_PRIORITY_NORMAL);
	}

	InitDebugGui();

	// Initialize camera
	CameraMotionParameters cmp{ 1.0f, 120.0f, 40.0f };
	vec3                   camPos{ 0.0f, 0.0f, 0.0f };
	vec3                   lookAt{ 0.0f, 0.0f, 1.0f };

	pLightView = createGuiCameraController(camPos, lookAt);
	pCameraController = createFpsCameraController(camPos, lookAt);
	pCameraController->setMotionParameters(cmp);

	// Initialize input system
	if (!Input::Init(pWindow, &gAppUI, this, pCameraController)) 
		return false;

	// Initialize shaders
	if (!InitShaderResources())
		return false;

	// Initialize scene
	if (!InitSceneResources())
		return false;

	return true;
}

void Application::Exit()
{
	waitQueueIdle(pGraphicsQueue);

	if (connected) {
		connected = false;
		ToggleClient();
	}

	Input::Exit();

	exitProfiler();

	RemoveShaderResources();

	RemoveSceneResources();

	destroyCameraController(pCameraController);
	destroyCameraController(pLightView);

#if defined(TARGET_IOS) || defined(__ANDROID__)
	gVirtualJoystick.Exit();
#endif

	gAppUI.Exit();
	//removeResource(testImage);

	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		removeResource(pShadowUniformBuffer[i]);
		removeResource(pUniformBuffer[i]);
		removeResource(pInstanceBuffer[i]);
		removeResource(pUniformBufferBones[i]);
		removeResource(pShadowInstanceBuffer[i]);
	}

	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		removeFence(pRenderer, pRenderCompleteFences[i]);
		removeSemaphore(pRenderer, pRenderCompleteSemaphores[i]);
	}
	removeSemaphore(pRenderer, pImageAcquiredSemaphore);

	removeCmd_n(pRenderer, Application::gImageCount, ppCmds);
	removeCmdPool(pRenderer, pCmdPool);

	removeSampler(pRenderer, pDefaultSampler);
	removeSampler(pRenderer, pBilinearClampSampler);

	removeResource(TriangularVB);

	exitResourceLoaderInterface(pRenderer);
	removeQueue(pRenderer, pGraphicsQueue);
	removeRenderer(pRenderer);

#if defined(__linux__) || defined(__ANDROID__)
	gModelFiles.set_capacity(0);
	gDropDownWidgetData.set_capacity(0);
#endif
}



void Application::PrepareDescriptorSets()
{
	// Shadow
	{
		DescriptorData params[2] = {};
		if (GLTFObject::pNodeTransformsBuffer)
		{
			params[0].pName = "modelToSceneMatrices";
			params[0].ppBuffers = &GLTFObject::pNodeTransformsBuffer;
			updateDescriptorSet(pRenderer, 0, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE], 1, params);
		}

		params[0].pName = "sceneTexture";
		params[0].ppTextures = &pForwardRT->pTexture;
		updateDescriptorSet(pRenderer, 0, pDescriptorSetVignette, 1, params);

		params[0].pName = "sceneTexture";
		params[0].ppTextures = &pPostProcessRT->pTexture;
		updateDescriptorSet(pRenderer, 0, pDescriptorSetFXAA, 1, params);

		for (uint32_t i = 0; i < Application::gImageCount; ++i)
		{
			params[0].pName = "cbPerPass";
			params[0].ppBuffers = &pShadowUniformBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME], 1, params);

			params[0].pName = "instanceBuffer";
			params[0].ppBuffers = &pShadowInstanceBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_BATCH], 1, params);
		}
	}
	// Shading
	{
		DescriptorData params[3] = {};
		params[0].pName = "ShadowTexture";
		params[0].ppTextures = &pShadowRT->pTexture;
		if (GLTFObject::pNodeTransformsBuffer)
		{
			params[1].pName = "modelToSceneMatrices";
			params[1].ppBuffers = &GLTFObject::pNodeTransformsBuffer;
		}
		updateDescriptorSet(pRenderer, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE], GLTFObject::pNodeTransformsBuffer ? 2 : 1, params);

		for (uint32_t i = 0; i < Application::gImageCount; ++i)
		{
			params[0].pName = "cbPerPass";
			params[0].ppBuffers = &pUniformBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME], 1, params);

			params[0].pName = "instanceBuffer";
			params[0].ppBuffers = &pInstanceBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_BATCH], 1, params);
		}
	}

	{
		DescriptorData params[2] = {};

		for (uint32_t i = 0; i < gImageCount; ++i)
		{
			params[0].pName = "cbPerPass";
			params[0].ppBuffers = &pUniformBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_FRAME], 1, params);

			params[0].pName = "boneBuffer";
			params[0].ppBuffers = &pUniformBufferBones[i];
			params[1].pName = "instanceBuffer";
			params[1].ppBuffers = &pInstanceBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetSkinning[DESCRIPTOR_UPDATE_FREQ_PER_BATCH], 2, params);
		}
	}
}

// ======================================================================================================
// ==============================================[ PIPELINE DEFINITION ]=================================
// ======================================================================================================

void Application::LoadPipelines()
{

	/************************************************************************/
	// Setup the resources needed for shadow map
	/************************************************************************/
	RasterizerStateDesc rasterizerStateDesc = {};
	rasterizerStateDesc.mCullMode = CULL_MODE_BACK;

	DepthStateDesc depthStateDesc = {};
	depthStateDesc.mDepthTest = true;
	depthStateDesc.mDepthWrite = true;
	depthStateDesc.mDepthFunc = CMP_GEQUAL;

	BlendStateDesc blendStateAlphaDesc = {};
	blendStateAlphaDesc.mSrcFactors[0] = BC_SRC_ALPHA;
	blendStateAlphaDesc.mDstFactors[0] = BC_ONE_MINUS_SRC_ALPHA;
	blendStateAlphaDesc.mBlendModes[0] = BM_ADD;
	blendStateAlphaDesc.mSrcAlphaFactors[0] = BC_ONE;
	blendStateAlphaDesc.mDstAlphaFactors[0] = BC_ZERO;
	blendStateAlphaDesc.mBlendAlphaModes[0] = BM_ADD;
	blendStateAlphaDesc.mMasks[0] = ALL;
	blendStateAlphaDesc.mRenderTargetMask = BLEND_STATE_TARGET_0;
	blendStateAlphaDesc.mIndependentBlend = false;

	{
		PipelineDesc desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& shadowMapPipelineSettings = desc.mGraphicsDesc;
		shadowMapPipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		shadowMapPipelineSettings.mRenderTargetCount = 0;
		shadowMapPipelineSettings.pDepthState = &depthStateDesc;
		shadowMapPipelineSettings.mDepthStencilFormat = pShadowRT->mFormat;
		shadowMapPipelineSettings.mSampleCount = pShadowRT->mSampleCount;
		shadowMapPipelineSettings.mSampleQuality = pShadowRT->mSampleQuality;
		shadowMapPipelineSettings.pRootSignature = pRootSignatureShadow;
		shadowMapPipelineSettings.pRasterizerState = &rasterizerStateDesc;
		shadowMapPipelineSettings.pShaderProgram = pShaderZPass;
		shadowMapPipelineSettings.pVertexLayout = &GLTFObject::pVertexLayoutModel;
		addPipeline(pRenderer, &desc, &pPipelineShadowPass);
	}

	{
		PipelineDesc desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
		pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		pipelineSettings.mRenderTargetCount = 1;
		pipelineSettings.pDepthState = &depthStateDesc;
		pipelineSettings.mDepthStencilFormat = pDepthBuffer->mFormat;
		pipelineSettings.pBlendState = &blendStateAlphaDesc;
		pipelineSettings.pColorFormats = &pForwardRT->mFormat;
		pipelineSettings.mSampleCount = pForwardRT->mSampleCount;
		pipelineSettings.mSampleQuality = pForwardRT->mSampleQuality;
		pipelineSettings.pRootSignature = pRootSignatureShaded;
		pipelineSettings.pVertexLayout = &GLTFObject::pVertexLayoutModel;
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		pipelineSettings.pShaderProgram = pMeshOptDemoShader;
		addPipeline(pRenderer, &desc, &pMeshOptDemoPipeline);
	}

	VertexLayout screenTriangle_VertexLayout = {};

	screenTriangle_VertexLayout.mAttribCount = 2;
	screenTriangle_VertexLayout.mAttribs[0].mSemantic = SEMANTIC_POSITION;
	screenTriangle_VertexLayout.mAttribs[0].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
	screenTriangle_VertexLayout.mAttribs[0].mBinding = 0;
	screenTriangle_VertexLayout.mAttribs[0].mLocation = 0;
	screenTriangle_VertexLayout.mAttribs[0].mOffset = 0;
	screenTriangle_VertexLayout.mAttribs[1].mSemantic = SEMANTIC_TEXCOORD0;
	screenTriangle_VertexLayout.mAttribs[1].mFormat = TinyImageFormat_R32G32_SFLOAT;
	screenTriangle_VertexLayout.mAttribs[1].mBinding = 0;
	screenTriangle_VertexLayout.mAttribs[1].mLocation = 1;
	screenTriangle_VertexLayout.mAttribs[1].mOffset = 3 * sizeof(float);

	{
		PipelineDesc desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
		pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		pipelineSettings.mRenderTargetCount = 1;
		pipelineSettings.pDepthState = NULL;
		pipelineSettings.pBlendState = &blendStateAlphaDesc;
		pipelineSettings.pColorFormats = &pPostProcessRT->mFormat;
		pipelineSettings.mSampleCount = pPostProcessRT->mSampleCount;
		pipelineSettings.mSampleQuality = pPostProcessRT->mSampleQuality;
		pipelineSettings.pRootSignature = pRootSignaturePostEffects;
		pipelineSettings.pVertexLayout = &screenTriangle_VertexLayout;
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		pipelineSettings.pShaderProgram = pVignetteShader;
		addPipeline(pRenderer, &desc, &pVignettePipeline);
	}

	{
		PipelineDesc desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
		pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		pipelineSettings.mRenderTargetCount = 1;
		pipelineSettings.pDepthState = NULL;
		pipelineSettings.pBlendState = NULL;
		pipelineSettings.pColorFormats = &pSwapChain->ppRenderTargets[0]->mFormat;
		pipelineSettings.mSampleCount = pSwapChain->ppRenderTargets[0]->mSampleCount;
		pipelineSettings.mSampleQuality = pSwapChain->ppRenderTargets[0]->mSampleQuality;
		pipelineSettings.pRootSignature = pRootSignaturePostEffects;
		pipelineSettings.pVertexLayout = &screenTriangle_VertexLayout;
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		pipelineSettings.pShaderProgram = pFXAAShader;
		addPipeline(pRenderer, &desc, &pFXAAPipeline);
	}

	{
		PipelineDesc desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
		pipelineSettings = {};
		pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		pipelineSettings.mRenderTargetCount = 1;
		pipelineSettings.pDepthState = &depthStateDesc;
		pipelineSettings.mDepthStencilFormat = pDepthBuffer->mFormat;
		pipelineSettings.pBlendState = &blendStateAlphaDesc;
		pipelineSettings.pColorFormats = &pForwardRT->mFormat;
		pipelineSettings.mSampleCount = pForwardRT->mSampleCount;
		pipelineSettings.mSampleQuality = pForwardRT->mSampleQuality;
		pipelineSettings.mDepthStencilFormat = pDepthBuffer->mFormat;
		pipelineSettings.pRootSignature = pRootSignatureSkinning;
		pipelineSettings.pShaderProgram = pShaderSkinning;
		pipelineSettings.pVertexLayout = &OzzObject::getVertexLayout();
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		addPipeline(pRenderer, &desc, &pPipelineSkinning);
	}
}

void Application::RemovePipelines()
{
	removePipeline(pRenderer, pPipelineShadowPass);
	removePipeline(pRenderer, pVignettePipeline);
	removePipeline(pRenderer, pMeshOptDemoPipeline);
	removePipeline(pRenderer, pPipelineSkinning);
	removePipeline(pRenderer, pFXAAPipeline);
}

// ======================================================================================================
// ==============================================[ LOAD/UNLAODING ]======================================
// ======================================================================================================

bool Application::Load()
{
	if (!addSwapChain())
		return false;

	if (!addRenderTargets())
		return false;

	if (!addDepthBuffer())
		return false;

	if (!gAppUI.Load(pSwapChain->ppRenderTargets))
		return false;

#if defined(TARGET_IOS) || defined(__ANDROID__)
	if (!gVirtualJoystick.Load(pSwapChain->ppRenderTargets[0]))
		return false;
#endif

	loadProfilerUI(&gAppUI, mSettings.mWidth, mSettings.mHeight);

	PrepareDescriptorSets();

	LoadPipelines();

	return true;
}

void Application::Unload()
{
	waitQueueIdle(pGraphicsQueue);
	waitForFences(pRenderer, Application::gImageCount, pRenderCompleteFences);

	unloadProfilerUI();
	gAppUI.Unload();

#if defined(TARGET_IOS) || defined(__ANDROID__)
	gVirtualJoystick.Unload();
#endif

	RemovePipelines();

	removeSwapChain(pRenderer, pSwapChain);

	removeRenderTarget(pRenderer, pPostProcessRT);
	removeRenderTarget(pRenderer, pForwardRT);
	removeRenderTarget(pRenderer, pDepthBuffer);
	removeRenderTarget(pRenderer, pShadowRT);
}

// ======================================================================================================
// ==============================================[ CORE UPDATE/DRAW LOOP ]===============================
// ======================================================================================================

void Application::Update(float deltaTime)
{
	// TODO clean up update call

	Input::Update(mSettings.mWidth, mSettings.mHeight);

#if !defined(__ANDROID__) && !defined(TARGET_IOS) && !defined(_DURANGO)
	if (pSwapChain->mEnableVsync != bToggleVSync)
	{
		waitQueueIdle(pGraphicsQueue);
		::toggleVSync(pRenderer, &pSwapChain);
	}
#endif
	/************************************************************************/
	// Server Contact
	/************************************************************************/
	if (connected) {
		int size = Input::EncodeToBuf(sendbuf);
		client->sendData(sendbuf, size, 0);
		updateData = client->recvAndFormatData();
	}

	/************************************************************************/
	// Scene Update
	/************************************************************************/

	if (connected) {
		scene->updateStateAndScene(updateData);
	}
	else {
		scene->updateFromInputBuf(deltaTime);
	}
	scene->update(deltaTime);

	/************************************************************************/
	// Uniform Data Updates
	/************************************************************************/

	vec3 playerPos = scene->getPlayerTransformMat()[3].getXYZ();
	pCameraController->moveTo(playerPos);

	pCameraController->update(deltaTime);
	Application::viewMat = mat4::translation(vec3(0,-2,10)) * pCameraController->getViewMatrix();
	const float aspectInverse = (float)mSettings.mHeight / (float)mSettings.mWidth;
	const float horizontal_fov = PI / 3.0f;
	Application::projMat = mat4::perspectiveReverseZ(horizontal_fov, aspectInverse, 0.001f, 1000.0f);
	gUniformData.mProjectView = projMat * viewMat;
	gUniformData.mModel = mat4::identity();
	gUniformData.mCameraPosition = vec4(pCameraController->getViewPosition() + cameraOffset, 1.0f);

	mat4 viewProj = Application::projMat * Application::viewMat;

	for (uint i = 0; i < gTotalLightCount; ++i)
	{
		gUniformData.mLightColor[i] = vec4(float((gLightColor[i] >> 24) & 0xff),
			float((gLightColor[i] >> 16) & 0xff),
			float((gLightColor[i] >> 8) & 0xff),
			float((gLightColor[i] >> 0) & 0xff)) / 255.0f;

		gUniformData.mLightColor[i].setW(gLightColorIntensity[i]);
	}

	float Azimuth = (PI / 180.0f) * gLightDirection.x;
	float Elevation = (PI / 180.0f) * (gLightDirection.y - 180.0f);

	vec3 sunDirection = normalize(vec3(cosf(Azimuth) * cosf(Elevation), sinf(Elevation), sinf(Azimuth) * cosf(Elevation)));

	gUniformData.mLightDirection[0] = vec4(sunDirection, 0.0f);
	// generate 2nd, 3rd light from the main light
	gUniformData.mLightDirection[1] = vec4(-sunDirection.getX(), sunDirection.getY(), -sunDirection.getZ(), 0.0f);
	gUniformData.mLightDirection[2] = vec4(-sunDirection.getX(), -sunDirection.getY(), -sunDirection.getZ(), 0.0f);



	/************************************************************************/
	// Light Matrix Update - for shadow map
	/************************************************************************/

	vec3 lightPos = sunDirection;
	pLightView->moveTo(lightPos + playerPos);
	pLightView->lookAt(vec3(0.0f) + playerPos);

	mat4 lightView = pLightView->getViewMatrix();
	//perspective as spotlight, for future use. TODO: Use a frustum fitting algorithm to maximise effective resolution!
	vec3 corners[8];
	getFrustumCorners(viewProj, corners);
	Point3 bounds[2] = { Point3(FLT_MAX), Point3(-FLT_MAX) };
	for (int i = 0; i < 8; i++) {
		vec4 lightSpace = lightView * corners[i];
		bounds[0] = minPerElem(bounds[0], Point3(lightSpace.getXYZ()));
		bounds[1] = maxPerElem(bounds[1], Point3(lightSpace.getXYZ()));
	}
	//mat4 lightProjMat = mat4::orthographicReverseZ(bounds[0][0], bounds[1][0], bounds[0][1], bounds[1][1], 0, 20);


	const float shadowRange = 10.0f;
	mat4 lightProjMat = mat4::orthographicReverseZ(-shadowRange, shadowRange, -shadowRange, shadowRange, 0, shadowRange * 8.0f);

	gShadowUniformData.ViewProj = lightProjMat * lightView;
	gUniformData.mShadowLightViewProj = gShadowUniformData.ViewProj;

	/************************************************************************/
	/************************************************************************/

	gAppUI.Update(deltaTime);
}

void Application::Draw()
{
	acquireNextImage(pRenderer, pSwapChain, pImageAcquiredSemaphore, NULL, &Application::gFrameIndex);

	// Stall if CPU is running "Swap Chain Buffer Count" frames ahead of GPU
	Fence* pNextFence = pRenderCompleteFences[Application::gFrameIndex];
	FenceStatus fenceStatus;
	getFenceStatus(pRenderer, pNextFence, &fenceStatus);
	if (fenceStatus == FENCE_STATUS_INCOMPLETE)
		waitForFences(pRenderer, 1, &pNextFence);

	RenderTarget* pRenderTarget = NULL;

	Semaphore* pRenderCompleteSemaphore = pRenderCompleteSemaphores[Application::gFrameIndex];
	Fence* pRenderCompleteFence = pRenderCompleteFences[Application::gFrameIndex];

	vec4 bgColor = vec4(float((gBackroundColor >> 24) & 0xff),
		float((gBackroundColor >> 16) & 0xff),
		float((gBackroundColor >> 8) & 0xff),
		float((gBackroundColor >> 0) & 0xff)) / 255.0f;


	Cmd* cmd = ppCmds[Application::gFrameIndex];
	beginCmd(cmd);

	cmdBeginGpuFrameProfile(cmd, gGpuProfileToken);

	// Draw depth map for shadows
	//drawShadowMap(cmd);

	// draw scene
	{
		// Prepare render target for drawing scene
		LoadActionsDesc loadActions = {};
		loadActions.mLoadActionsColor[0] = LOAD_ACTION_CLEAR;
		loadActions.mClearColorValues[0].r = bgColor.getX();
		loadActions.mClearColorValues[0].g = bgColor.getY();
		loadActions.mClearColorValues[0].b = bgColor.getZ();
		loadActions.mClearColorValues[0].a = bgColor.getW();
		loadActions.mLoadActionDepth = LOAD_ACTION_CLEAR;
		loadActions.mClearDepth.depth = 0.0f;
		loadActions.mClearDepth.stencil = 0;

		pRenderTarget = pForwardRT;

		RenderTargetBarrier barriers[] =
		{
			{ pRenderTarget, RESOURCE_STATE_RENDER_TARGET },
			{ pDepthBuffer, RESOURCE_STATE_DEPTH_WRITE },
			{ pShadowRT, RESOURCE_STATE_SHADER_RESOURCE }
		};
		cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 3, barriers);

		cmdBindRenderTargets(cmd, 1, &pRenderTarget, pDepthBuffer, &loadActions, NULL, NULL, -1, -1);

		// Start Drawing scene
		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Scene");

		// Collect rendering details
		Geode::GeodeShaderDesc meshShaderDesc;
		meshShaderDesc.rootSignature = pRootSignatureShaded;
		meshShaderDesc.pipeline = pMeshOptDemoPipeline;
		for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
			meshShaderDesc.descriptorSets[i] = pDescriptorSetsShaded[i];
		}

		Geode::GeodeShaderDesc skinShaderDesc;
		skinShaderDesc.rootSignature = pRootSignatureSkinning;
		skinShaderDesc.pipeline = pPipelineSkinning;
		for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
			skinShaderDesc.descriptorSets[i] = pDescriptorSetSkinning[i];
		}
		
		// Set render target
		cmdBindRenderTargets(cmd, 1, &pRenderTarget, pDepthBuffer, NULL, NULL, NULL, -1, -1);
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

		// Update general uniform buffers
		BufferUpdateDesc shaderCbv = { pUniformBuffer[Application::gFrameIndex] };
		beginUpdateResource(&shaderCbv);
		*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
		endUpdateResource(&shaderCbv, NULL);

		scene->setProgram(SceneManager_Client::GeodeType::MESH, meshShaderDesc);
		scene->setProgram(SceneManager_Client::GeodeType::ANIMATED_MESH, skinShaderDesc);
		GLTFGeode::useMaterials = true;
		SceneManager_Client::enableCulling = bToggleCull;
		scene->draw(cmd);

		// Unbind render targets
		cmdBindRenderTargets(cmd, 0, NULL, 0, NULL, NULL, NULL, -1, -1);

		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	pRenderTarget = pPostProcessRT;
	RenderTargetBarrier barriers[] = {
		{ pRenderTarget, RESOURCE_STATE_RENDER_TARGET },
		{ pForwardRT, RESOURCE_STATE_SHADER_RESOURCE }
	};

	cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 2, barriers);

	if (bVignetting)
	{
		LoadActionsDesc loadActions = {};
		loadActions.mLoadActionsColor[0] = LOAD_ACTION_LOAD;
		loadActions.mLoadActionDepth = LOAD_ACTION_DONTCARE;
		loadActions.mLoadActionStencil = LOAD_ACTION_LOAD;

		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Vignetting");

		cmdBindRenderTargets(cmd, 1, &pRenderTarget, NULL, &loadActions, NULL, NULL, -1, -1);
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

		cmdBindPipeline(cmd, pVignettePipeline);

		cmdBindDescriptorSet(cmd, 0, pDescriptorSetVignette);

		const uint32_t stride = sizeof(float) * 5;
		cmdBindVertexBuffer(cmd, 1, &TriangularVB, &stride, NULL);
		cmdDraw(cmd, 3, 0);

		cmdBindRenderTargets(cmd, 0, NULL, 0, NULL, NULL, NULL, -1, -1);

		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	pRenderTarget = pSwapChain->ppRenderTargets[Application::gFrameIndex];
	{
		RenderTargetBarrier barriers[] =
		{
			{ pRenderTarget, RESOURCE_STATE_RENDER_TARGET },
			{ pPostProcessRT, RESOURCE_STATE_SHADER_RESOURCE }
		};
		cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 2, barriers);

		LoadActionsDesc loadActions = {};
		loadActions.mLoadActionsColor[0] = LOAD_ACTION_CLEAR;
		loadActions.mClearColorValues[0].r = 0.0f;
		loadActions.mClearColorValues[0].g = 0.0f;
		loadActions.mClearColorValues[0].b = 0.0f;
		loadActions.mClearColorValues[0].a = 0.0f;
		loadActions.mLoadActionDepth = LOAD_ACTION_DONTCARE;

		cmdBindRenderTargets(cmd, 1, &pRenderTarget, NULL, &loadActions, NULL, NULL, -1, -1);
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "FXAA");

		cmdBindPipeline(cmd, pFXAAPipeline);

		FXAAINFO FXAAinfo;
		FXAAinfo.ScreenSize = vec2((float)mSettings.mWidth, (float)mSettings.mHeight);
		FXAAinfo.Use = bToggleFXAA ? 1 : 0;

		cmdBindDescriptorSet(cmd, 0, bVignetting ? pDescriptorSetFXAA : pDescriptorSetVignette);
		cmdBindPushConstants(cmd, pRootSignaturePostEffects, "FXAARootConstant", &FXAAinfo);

		const uint32_t stride = sizeof(float) * 5;
		cmdBindVertexBuffer(cmd, 1, &TriangularVB, &stride, NULL);
		cmdDraw(cmd, 3, 0);

		cmdBindRenderTargets(cmd, 0, NULL, 0, NULL, NULL, NULL, -1, -1);

		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	{
		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw UI");
		static HiresTimer gTimer;
		gTimer.GetUSec(true);

		LoadActionsDesc loadActions = {};
		loadActions.mLoadActionsColor[0] = LOAD_ACTION_LOAD;

		cmdBindRenderTargets(cmd, 1, &pRenderTarget, NULL, &loadActions, NULL, NULL, -1, -1);
#if defined(TARGET_IOS) || defined(__ANDROID__)
		//gVirtualJoystick.Draw(cmd, { 1.0f, 1.0f, 1.0f, 1.0f });
#endif

		cmdDrawCpuProfile(cmd, float2(8, mSettings.mHeight - 200), &gFrameTimeDraw);
#if !defined(__ANDROID__)
		cmdDrawGpuProfile(cmd, float2(8, mSettings.mHeight - 160), gGpuProfileToken);
#endif

		//cmdDrawProfilerUI();

		//ImGui::SetNextWindowBgAlpha(0);
		gAppUI.Gui(pDebugGui);
		//gAppUI.Gui(pTestGui);

		gAppUI.Draw(cmd);

		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	cmdBindRenderTargets(cmd, 0, NULL, 0, NULL, NULL, NULL, -1, -1);

	RenderTargetBarrier finalBarriers = { pRenderTarget, RESOURCE_STATE_PRESENT };
	cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, &finalBarriers);
	cmdEndGpuFrameProfile(cmd, gGpuProfileToken);
	endCmd(cmd);

	QueueSubmitDesc submitDesc = {};
	submitDesc.mCmdCount = 1;
	submitDesc.mSignalSemaphoreCount = 1;
	submitDesc.mWaitSemaphoreCount = 1;
	submitDesc.ppCmds = &cmd;
	submitDesc.ppSignalSemaphores = &pRenderCompleteSemaphore;
	submitDesc.ppWaitSemaphores = &pImageAcquiredSemaphore;
	submitDesc.pSignalFence = pRenderCompleteFence;
	queueSubmit(pGraphicsQueue, &submitDesc);
	QueuePresentDesc presentDesc = {};
	presentDesc.mIndex = Application::gFrameIndex;
	presentDesc.mWaitSemaphoreCount = 1;
	presentDesc.ppWaitSemaphores = &pRenderCompleteSemaphore;
	presentDesc.pSwapChain = pSwapChain;
	presentDesc.mSubmitDone = true;
	queuePresent(pGraphicsQueue, &presentDesc);

	flipProfiler();
}

// ======================================================================================================
// ==============================================[ MULTIPASS RENDERING ]=================================
// ======================================================================================================

void Application::setRenderTarget(Cmd* cmd, uint32_t count, RenderTarget** pDestinationRenderTargets, RenderTarget* pDepthStencilTarget, LoadActionsDesc* loadActions)
{
	if (count == 0 && pDestinationRenderTargets == NULL && pDepthStencilTarget == NULL)
		cmdBindRenderTargets(cmd, 0, NULL, NULL, NULL, NULL, NULL, -1, -1);
	else
	{
		cmdBindRenderTargets(cmd, count, pDestinationRenderTargets, pDepthStencilTarget, loadActions, NULL, NULL, -1, -1);
		// sets the rectangles to match with first attachment, I know that it's not very portable.
		RenderTarget* pSizeTarget = pDepthStencilTarget ? pDepthStencilTarget : pDestinationRenderTargets[0];
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pSizeTarget->mWidth, (float)pSizeTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pSizeTarget->mWidth, pSizeTarget->mHeight);
	}
}

void Application::drawShadowMap(Cmd* cmd)
{
	// Update uniform buffers
	BufferUpdateDesc shaderCbv = { pShadowUniformBuffer[gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	*(UniformBlock_Shadow*)shaderCbv.pMappedData = gShadowUniformData;
	endUpdateResource(&shaderCbv, NULL);

	RenderTargetBarrier barriers[] =
	{
		{ pShadowRT, RESOURCE_STATE_DEPTH_WRITE },
	};
	cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);

	LoadActionsDesc loadActions = {};
	loadActions.mLoadActionDepth = LOAD_ACTION_CLEAR;
	loadActions.mClearDepth = pShadowRT->mClearValue;
	cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Shadow Map");
	// Start render pass and apply load actions
	setRenderTarget(cmd, 0, NULL, pShadowRT, &loadActions);

	cmdBindPipeline(cmd, pPipelineShadowPass);

	



	Geode::GeodeShaderDesc meshShaderDesc;
	meshShaderDesc.rootSignature = pRootSignatureShadow;
	meshShaderDesc.pipeline = pPipelineShadowPass;
	for (int i = 0; i < DESCRIPTOR_UPDATE_FREQ_COUNT; i++) {
		meshShaderDesc.descriptorSets[i] = pDescriptorSetsShadow[i];
	}

	// Update per-instance uniforms
	shaderCbv = { pShadowInstanceBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	scene->updateTransformBuffer(shaderCbv, mat4::identity());
	endUpdateResource(&shaderCbv, NULL);

	vec4 frustumPlanes[6];
	mat4::extractFrustumClipPlanes(gShadowUniformData.ViewProj, frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], frustumPlanes[3], frustumPlanes[4], frustumPlanes[5], true);
	scene->cull(frustumPlanes, bToggleCull);

	scene->setProgram(SceneManager_Client::GeodeType::MESH, meshShaderDesc);
	GLTFGeode::useMaterials = false;
	scene->draw(cmd);





	setRenderTarget(cmd, 0, NULL, NULL, NULL);

	cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
}

// ======================================================================================================
// ==============================================[ RENDER SETUP ]========================================
// ======================================================================================================

bool Application::addSwapChain()
{
	SwapChainDesc swapChainDesc = {};
	swapChainDesc.mWindowHandle = pWindow->handle;
	swapChainDesc.mPresentQueueCount = 1;
	swapChainDesc.ppPresentQueues = &pGraphicsQueue;
	swapChainDesc.mWidth = mSettings.mWidth;
	swapChainDesc.mHeight = mSettings.mHeight;
	swapChainDesc.mImageCount = Application::gImageCount;

	swapChainDesc.mColorFormat = getRecommendedSwapchainFormat(true);
	swapChainDesc.mEnableVsync = false;

	::addSwapChain(pRenderer, &swapChainDesc, &pSwapChain);

	return pSwapChain != NULL;
}

bool Application::addRenderTargets()
{
	RenderTargetDesc RT = {};
	RT.mArraySize = 1;
	RT.mDepth = 1;
	RT.mFormat = TinyImageFormat_R8G8B8A8_UNORM;

	vec4 bgColor = vec4(float((gBackroundColor >> 24) & 0xff),
		float((gBackroundColor >> 16) & 0xff),
		float((gBackroundColor >> 8) & 0xff),
		float((gBackroundColor >> 0) & 0xff)) / 255.0f;

	RT.mClearValue.r = bgColor.getX();
	RT.mClearValue.g = bgColor.getY();
	RT.mClearValue.b = bgColor.getZ();
	RT.mClearValue.a = bgColor.getW();

	RT.mWidth = mSettings.mWidth;
	RT.mHeight = mSettings.mHeight;

	RT.mSampleCount = SAMPLE_COUNT_1;
	RT.mSampleQuality = 0;
	RT.pDebugName = L"Render Target";
	addRenderTarget(pRenderer, &RT, &pForwardRT);

	RT = {};
	RT.mArraySize = 1;
	RT.mDepth = 1;
	RT.mFormat = TinyImageFormat_R8G8B8A8_UNORM;
	RT.mWidth = mSettings.mWidth;
	RT.mHeight = mSettings.mHeight;
	RT.mSampleCount = SAMPLE_COUNT_1;
	RT.mSampleQuality = 0;
	RT.pDebugName = L"Post Process Render Target";
	addRenderTarget(pRenderer, &RT, &pPostProcessRT);

	return pForwardRT != NULL && pPostProcessRT != NULL;
}

bool Application::addDepthBuffer()
{
	// Add depth buffer
	RenderTargetDesc depthRT = {};
	depthRT.mArraySize = 1;
	depthRT.mClearValue.depth = 0.0f;
	depthRT.mClearValue.stencil = 0;
	depthRT.mDepth = 1;
	depthRT.mFormat = TinyImageFormat_D32_SFLOAT;
	depthRT.mHeight = mSettings.mHeight;
	depthRT.mSampleCount = SAMPLE_COUNT_1;
	depthRT.mSampleQuality = 0;
	depthRT.mWidth = mSettings.mWidth;
	depthRT.mFlags = TEXTURE_CREATION_FLAG_ON_TILE;
	addRenderTarget(pRenderer, &depthRT, &pDepthBuffer);

	/************************************************************************/
	// Shadow Map Render Target
	/************************************************************************/

	RenderTargetDesc shadowRTDesc = {};
	shadowRTDesc.mArraySize = 1;
	shadowRTDesc.mClearValue.depth = 0.0f;
	shadowRTDesc.mClearValue.stencil = 0;
	shadowRTDesc.mDepth = 1;
	shadowRTDesc.mFormat = TinyImageFormat_D32_SFLOAT;
	shadowRTDesc.mWidth = SHADOWMAP_RES;
	shadowRTDesc.mHeight = SHADOWMAP_RES;
	shadowRTDesc.mSampleCount = (SampleCount)SHADOWMAP_MSAA_SAMPLES;
	shadowRTDesc.mSampleQuality = 0;    // don't need higher quality sample patterns as the texture will be blurred heavily
	shadowRTDesc.pDebugName = L"Shadow Map RT";

	addRenderTarget(pRenderer, &shadowRTDesc, &pShadowRT);

	return pDepthBuffer != NULL && pShadowRT != NULL;
}

void Application::getFrustumCorners(mat4 frustum, vec3 corners[8])
{
	vec4 ndcCorners[8];
	ndcCorners[0] = vec4(1, 1, 1, 1);
	ndcCorners[1] = vec4(-1, 1, 1, 1);
	ndcCorners[2] = vec4(-1, -1, 1, 1);
	ndcCorners[3] = vec4(1, -1, 1, 1);
	ndcCorners[4] = vec4(1, 1, 0, 1);
	ndcCorners[5] = vec4(-1, 1, 0, 1);
	ndcCorners[6] = vec4(-1, -1, 0, 1);
	ndcCorners[7] = vec4(1, -1, 0, 1);

	mat4 invFrustum = inverse(frustum);
	//print(frustum);
	//print(invFrustum);
	for (int i = 0; i < 8; i++) {
		//print(ndcCorners[i]);
		ndcCorners[i] = invFrustum  * ndcCorners[i];
		corners[i] = (ndcCorners[i] / ndcCorners[i][3]).getXYZ();
		//print(ndcCorners[i]);
		//print(corners[i]);
	}
}


DEFINE_APPLICATION_MAIN(Application)
