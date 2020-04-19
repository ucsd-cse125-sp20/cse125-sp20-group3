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

bool				bToggleFXAA = true;
bool				bVignetting = true;
bool				bToggleVSync = false;
bool				bScreenShotMode = false;

ProfileToken   gGpuProfileToken;
Texture* Application::pTextureBlack = NULL;
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
// STRUCT DEFINTIONS
//--------------------------------------------------------------------------------------------

// MOVED

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
Shader* pShaderZPass_NonOptimized = NULL;
Shader* pMeshOptDemoShader = NULL;
Shader* pFloorShader = NULL;
Shader* pVignetteShader = NULL;
Shader* pFXAAShader = NULL;
Shader* pWaterMarkShader = NULL;

Pipeline* pPipelineShadowPass = NULL;
Pipeline* pPipelineShadowPass_NonOPtimized = NULL;
Pipeline* pMeshOptDemoPipeline = NULL;
Pipeline* pFloorPipeline = NULL;
Pipeline* pVignettePipeline = NULL;
Pipeline* pFXAAPipeline = NULL;
Pipeline* pWaterMarkPipeline = NULL;

RootSignature* pRootSignatureShadow = NULL;
RootSignature* pRootSignatureShaded = NULL;
RootSignature* pRootSignaturePostEffects = NULL;

DescriptorSet* pDescriptorSetVignette;
DescriptorSet* pDescriptorSetFXAA;
DescriptorSet* pDescriptorSetWatermark;
DescriptorSet* pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_COUNT];
DescriptorSet* pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_COUNT];

VirtualJoystickUI   gVirtualJoystick = {};

Buffer* pUniformBuffer[IMAGE_COUNT] = { NULL };
Buffer* pShadowUniformBuffer[IMAGE_COUNT] = { NULL };
Buffer* pFloorUniformBuffer[IMAGE_COUNT] = { NULL };

Buffer* TriangularVB = NULL;
Buffer* pFloorVB = NULL;
Buffer* pFloorIB = NULL;
Buffer* WaterMarkVB = NULL;

Sampler* pDefaultSampler = NULL;
Sampler* pBilinearClampSampler = NULL;

Application::UniformBlock		gUniformData;
Application::UniformBlock_Floor	gFloorUniformBlock;
Application::UniformBlock_Shadow gShadowUniformData;

//--------------------------------------------------------------------------------------------
// THE FORGE OBJECTS
//--------------------------------------------------------------------------------------------

ICameraController* pCameraController = NULL;
ICameraController* pLightView = NULL;

GuiComponent* pGuiWindow;
GuiComponent* pGuiGraphics;

IWidget* pSelectLodWidget = NULL;

UIApp				gAppUI;
eastl::vector<uint32_t>	gDropDownWidgetData;
eastl::vector<PathHandle> gModelFiles;

Input inputHandler;

#if defined(__ANDROID__) || defined(__LINUX__)
uint32_t			modelToLoadIndex = 0;
uint32_t			guiModelToLoadIndex = 0;
#endif

const wchar_t* gMissingTextureString = L"MissingTexture";

const char* gDefaultModelFile = "SpinningBox.gltf";
PathHandle					    gModelFile;
PathHandle					    gGuiModelToLoad;

const uint			gBackroundColor = { 0xb2b2b2ff };
static uint			gLightColor[gTotalLightCount] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffff66 };
static float		gLightColorIntensity[gTotalLightCount] = { 2.0f, 0.2f, 0.2f, 0.25f };
static float2		gLightDirection = { -122.0f, 222.0f };

vec3 cameraOffset(0, 10, -5);

GLTFObject* player;
float rot = 0;

float currPosX = 0;
float currPosY = 0;
float currVelX = 0;
float currVelY = 0;
float acceleration = 1;
float drag = 0.1f;

TextDrawDesc gFrameTimeDraw = TextDrawDesc(0, 0xff00ffff, 18);

std::vector<GLTFObject*> others;


Application::Application()
{
#ifdef TARGET_IOS
	mSettings.mContentScaleFactor = 1.f;
#endif
}

bool Application::InitShaderResources()
{
	// shader

	ShaderLoadDesc FloorShader = {};

	FloorShader.mStages[0] = { "floor.vert", NULL, 0, RD_SHADER_SOURCES };

	addShader(pRenderer, &FloorShader, &pShaderZPass_NonOptimized);

#if defined(__ANDROID__)
	FloorShader.mStages[1] = { "floorMOBILE.frag", NULL, 0, RD_SHADER_SOURCES };
#else
	FloorShader.mStages[1] = { "floor.frag", NULL, 0, RD_SHADER_SOURCES };
#endif

	addShader(pRenderer, &FloorShader, &pFloorShader);

	ShaderLoadDesc MeshOptDemoShader = {};

	MeshOptDemoShader.mStages[0] = { "basic.vert", NULL, 0, RD_SHADER_SOURCES };

	addShader(pRenderer, &MeshOptDemoShader, &pShaderZPass);

#if defined(__ANDROID__)
	MeshOptDemoShader.mStages[1] = { "basicMOBILE.frag", NULL, 0, RD_SHADER_SOURCES };
#else
	MeshOptDemoShader.mStages[1] = { "basic.frag", NULL, 0, RD_SHADER_SOURCES };
#endif

	addShader(pRenderer, &MeshOptDemoShader, &pMeshOptDemoShader);

	ShaderLoadDesc VignetteShader = {};

	VignetteShader.mStages[0] = { "Triangular.vert", NULL, 0, RD_SHADER_SOURCES };
	VignetteShader.mStages[1] = { "vignette.frag", NULL, 0, RD_SHADER_SOURCES };

	addShader(pRenderer, &VignetteShader, &pVignetteShader);

	ShaderLoadDesc FXAAShader = {};

	FXAAShader.mStages[0] = { "Triangular.vert", NULL, 0, RD_SHADER_SOURCES };
	FXAAShader.mStages[1] = { "FXAA.frag", NULL, 0, RD_SHADER_SOURCES };

	addShader(pRenderer, &FXAAShader, &pFXAAShader);

	ShaderLoadDesc WaterMarkShader = {};

	WaterMarkShader.mStages[0] = { "watermark.vert", NULL, 0, RD_SHADER_SOURCES };
	WaterMarkShader.mStages[1] = { "watermark.frag", NULL, 0, RD_SHADER_SOURCES };

	addShader(pRenderer, &WaterMarkShader, &pWaterMarkShader);

	const char* pStaticSamplerNames[] = { "clampMiplessLinearSampler" };
	Sampler* pStaticSamplers[] = { pBilinearClampSampler };
	Shader* shaders[] = { pShaderZPass, pShaderZPass_NonOptimized };
	RootSignatureDesc rootDesc = {};
	rootDesc.mStaticSamplerCount = 1;
	rootDesc.ppStaticSamplerNames = pStaticSamplerNames;
	rootDesc.ppStaticSamplers = pStaticSamplers;
	rootDesc.mShaderCount = 2;
	rootDesc.ppShaders = shaders;

	addRootSignature(pRenderer, &rootDesc, &pRootSignatureShadow);

	Shader* demoShaders[] = { pMeshOptDemoShader, pFloorShader };

	rootDesc.mShaderCount = 2;
	rootDesc.ppShaders = demoShaders;

	addRootSignature(pRenderer, &rootDesc, &pRootSignatureShaded);

	Shader* postShaders[] = { pVignetteShader, pFXAAShader, pWaterMarkShader };
	rootDesc.mShaderCount = 3;
	rootDesc.ppShaders = postShaders;

	addRootSignature(pRenderer, &rootDesc, &pRootSignaturePostEffects);

	if (!AddDescriptorSets())
		return false;

	return true;
}

bool Application::InitModelDependentResources()
{
	if (!GLTFObject::LoadModel(player, pRenderer, pDefaultSampler, gModelFile))
		return false;

	for (auto other : others) {
		if (!GLTFObject::LoadModel(other, pRenderer, pDefaultSampler, gModelFile))
			return false;
	}

	if (!InitShaderResources())
		return false;

	waitForAllResourceLoads();

	PrepareDescriptorSets();

	return true;
}

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

	cmdBindPipeline(cmd, pPipelineShadowPass_NonOPtimized);
	cmdBindDescriptorSet(cmd, 0, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE]);
	cmdBindDescriptorSet(cmd, Application::gFrameIndex, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);

	const uint32_t stride = sizeof(float) * 5;
	cmdBindVertexBuffer(cmd, 1, &pFloorVB, &stride, NULL);
	cmdBindIndexBuffer(cmd, pFloorIB, INDEX_TYPE_UINT16, 0);

	cmdDrawIndexed(cmd, 6, 0, 0);

	cmdBindPipeline(cmd, pPipelineShadowPass);
	
	/*/ Update uniform buffers
	gUniformData.mModel = gCurrentAsset->model;
	shaderCbv = { pShadowUniformBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
	endUpdateResource(&shaderCbv, NULL);
	*/

	// Update uniform buffers
	gShadowUniformData.mModel = player->model;
	BufferUpdateDesc shaderCbv = { pShadowUniformBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	*(UniformBlock_Shadow*)shaderCbv.pMappedData = gShadowUniformData;
	endUpdateResource(&shaderCbv, NULL);

	player->draw(cmd, pRootSignatureShadow, false);

	setRenderTarget(cmd, 0, NULL, NULL, NULL);

	cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
}

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

	float floorPoints[] = {
		-1.0f, 0.0f, 1.0f, -1.0f, -1.0f,
		-1.0f, 0.0f, -1.0f, -1.0f, 1.0f,
		1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, -1.0f,
	};

	BufferLoadDesc floorVbDesc = {};
	floorVbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
	floorVbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	floorVbDesc.mDesc.mSize = sizeof(float) * 5 * 4;
	floorVbDesc.pData = floorPoints;
	floorVbDesc.ppBuffer = &pFloorVB;
	addResource(&floorVbDesc, NULL, LOAD_PRIORITY_NORMAL);

	uint16_t floorIndices[] =
	{
		0, 1, 3,
		3, 1, 2
	};

	BufferLoadDesc indexBufferDesc = {};
	indexBufferDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_INDEX_BUFFER;
	indexBufferDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	indexBufferDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_OWN_MEMORY_BIT;
	indexBufferDesc.mDesc.mSize = sizeof(uint16_t) * 6;
	indexBufferDesc.pData = floorIndices;
	indexBufferDesc.ppBuffer = &pFloorIB;
	addResource(&indexBufferDesc, NULL, LOAD_PRIORITY_NORMAL);

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
	defaultTextureDesc.pDebugName = gMissingTextureString;
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

#if defined(__ANDROID__) || defined(__LINUX__)
	// Get list of Models
	eastl::vector<eastl::string> filesToLoad;
	eastl::vector<PathHandle> filesToLoadFullPath;
	PathHandle meshDirectory = fsCopyPathForResourceDirectory(RD_MESHES);
	eastl::vector<PathHandle> filesInDirectory = fsGetFilesWithExtension(meshDirectory, "gltf");

	//reduce duplicated filenames
	for (size_t i = 0; i < filesInDirectory.size(); ++i)
	{
		const PathHandle& file = filesInDirectory[i];

		eastl::string tempfile(fsGetPathAsNativeString(file));

		const char* first = strstr(tempfile.c_str(), "PQPM");

		bool bAlreadyLoaded = false;

		if (first != NULL)
		{
			for (size_t j = 0; j < filesToLoad.size(); ++j)
			{
				if (strstr(tempfile.c_str(), filesToLoad[j].c_str()) != NULL)
				{
					bAlreadyLoaded = true;
					break;
				}
			}

			if (!bAlreadyLoaded)
			{
				int gap = first - tempfile.c_str();
				tempfile.resize(gap);
				filesToLoad.push_back(tempfile);
				filesToLoadFullPath.push_back(file);
			}
		}
		else
		{
			filesToLoad.push_back(tempfile);
			filesToLoadFullPath.push_back(file);
		}
	}

	size_t modelFileCount = filesToLoadFullPath.size();

	eastl::vector<const char*> modelFileNames(modelFileCount);
	gModelFiles.resize(modelFileCount);
	gDropDownWidgetData.resize(modelFileCount);

	for (size_t i = 0; i < modelFileCount; ++i)
	{
		const PathHandle& file = filesToLoadFullPath[i];

		gModelFiles[i] = file;
		modelFileNames[i] = fsGetPathFileName(gModelFiles[i]).buffer;
		gDropDownWidgetData[i] = (uint32_t)i;
	}

	gModelFile = gModelFiles[0];
	gGuiModelToLoad = gModelFiles[0];

#else
	PathHandle fullModelPath = fsCopyPathInResourceDirectory(RD_MESHES, gDefaultModelFile);
	gModelFile = fullModelPath;
	gGuiModelToLoad = fullModelPath;
#endif

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

	ubDesc = {};
	ubDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
	ubDesc.mDesc.mSize = sizeof(UniformBlock);
	ubDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
	ubDesc.pData = NULL;
	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		ubDesc.ppBuffer = &pFloorUniformBuffer[i];
		addResource(&ubDesc, NULL, LOAD_PRIORITY_NORMAL);
	}
	/************************************************************************/
	// GUI
	/************************************************************************/
	/*
	GuiDesc guiDesc = {};
	guiDesc.mStartSize = vec2(300.0f, 250.0f);
	guiDesc.mStartPosition = vec2(100.0f, guiDesc.mStartSize.getY());
	pGuiWindow = gAppUI.AddGuiComponent(GetName(), &guiDesc);

#if !defined(TARGET_IOS) && !defined(_DURANGO)
	pGuiWindow->AddWidget(CheckboxWidget("Toggle VSync", &bToggleVSync));
#endif

#if defined(__ANDROID__) || defined(__LINUX__)
	pGuiWindow->AddWidget(DropdownWidget("Models", &guiModelToLoadIndex, modelFileNames.data(), gDropDownWidgetData.data(), (uint32_t)gModelFiles.size()));
#else
	pGuiWindow->AddWidget(SeparatorWidget());

	ButtonWidget loadModelButtonWidget("Load Model                                      ");
	loadModelButtonWidget.pOnEdited = Application::LoadNewModel;
	pGuiWindow->AddWidget(loadModelButtonWidget);

	pGuiWindow->AddWidget(SeparatorWidget());

	//ButtonWidget loadLODButtonWidget("Load Model LOD");
	//loadLODButtonWidget.pOnEdited = GLTFViewer::LoadLOD;
	//pGuiWindow->AddWidget(loadLODButtonWidget);
#endif

	pSelectLodWidget = pGuiWindow->AddWidget(SliderIntWidget("LOD", &gCurrentLod, 0, gMaxLod));
	*/
	////////////////////////////////////////////////////////////////////////////////////////////
	/*
	guiDesc = {};
	guiDesc.mStartSize = vec2(400.0f, 250.0f);
	guiDesc.mStartPosition = vec2(mSettings.mWidth - guiDesc.mStartSize.getX(), guiDesc.mStartSize.getY());
	pGuiGraphics = gAppUI.AddGuiComponent("Graphics Options", &guiDesc);

	pGuiGraphics->AddWidget(CheckboxWidget("Enable FXAA", &bToggleFXAA));
	pGuiGraphics->AddWidget(CheckboxWidget("Enable Vignetting", &bVignetting));

	pGuiGraphics->AddWidget(SeparatorWidget());

	CollapsingHeaderWidget LightWidgets("Light Options", false, false);
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

	CollapsingHeaderWidget LightColor2Picker("Light2 Color");
	LightColor2Picker.AddSubWidget(ColorPickerWidget("Light2 Color", &gLightColor[1]));
	LightWidgets.AddSubWidget(LightColor2Picker);

	CollapsingHeaderWidget LightColor2Intensity("Light2 Intensity");
	LightColor2Intensity.AddSubWidget(SliderFloatWidget("Light2 Intensity", &gLightColorIntensity[1], 0.0f, 5.0f, 0.001f));
	LightWidgets.AddSubWidget(LightColor2Intensity);

	LightWidgets.AddSubWidget(SeparatorWidget());

	CollapsingHeaderWidget LightColor3Picker("Light3 Color");
	LightColor3Picker.AddSubWidget(ColorPickerWidget("Light3 Color", &gLightColor[2]));
	LightWidgets.AddSubWidget(LightColor3Picker);

	CollapsingHeaderWidget LightColor3Intensity("Light3 Intensity");
	LightColor3Intensity.AddSubWidget(SliderFloatWidget("Light3 Intensity", &gLightColorIntensity[2], 0.0f, 5.0f, 0.001f));
	LightWidgets.AddSubWidget(LightColor3Intensity);

	LightWidgets.AddSubWidget(SeparatorWidget());

	CollapsingHeaderWidget AmbientLightColorPicker("Ambient Light Color");
	AmbientLightColorPicker.AddSubWidget(ColorPickerWidget("Ambient Light Color", &gLightColor[3]));
	LightWidgets.AddSubWidget(AmbientLightColorPicker);

	CollapsingHeaderWidget LightColor4Intensity("Ambient Light Intensity");
	LightColor4Intensity.AddSubWidget(SliderFloatWidget("Light Intensity", &gLightColorIntensity[3], 0.0f, 5.0f, 0.001f));
	LightWidgets.AddSubWidget(LightColor4Intensity);

	LightWidgets.AddSubWidget(SeparatorWidget());

	pGuiGraphics->AddWidget(LightWidgets);
	*/

	// Scene Initialization Initialization


	CameraMotionParameters cmp{ 1.0f, 120.0f, 40.0f };
	vec3                   camPos{ 3.0f, 2.5f, -4.0f };
	vec3                   lookAt{ 0.0f, 0.4f, 0.0f };

	pLightView = createGuiCameraController(camPos, lookAt);
	pCameraController = createFpsCameraController(normalize(camPos) * 3.0f, lookAt);
	pCameraController->setMotionParameters(cmp);

	if (!Input::Init(pWindow, &gAppUI, this)) return false;

	player = conf_new(GLTFObject);

	float randoRange = 5;
	for (int i = 0; i < 1; i++) {
		float x = -randoRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * randoRange)));
		float y = -randoRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * randoRange)));
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.5f));

		vec3 position = vec3(x, 0, y);
		if (sqrt(x * x + y * y) > 1.5f) {
			auto other = conf_new(GLTFObject);
			other->setTranslate(position);
			other->setScaleRot(vec3(s), rot, vec3(0, 1, 0));
			//other->setColor(glm::vec3(r, g, b));
			others.push_back(other);
		}
	}

	return true;
}

bool Application::AddDescriptorSets()
{
	DescriptorSetDesc setDesc = { pRootSignaturePostEffects, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetVignette);
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetFXAA);
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetWatermark);

	setDesc = { pRootSignatureShadow, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE]);
	setDesc = { pRootSignatureShadow, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);

	setDesc = { pRootSignatureShaded, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
	setDesc = { pRootSignatureShaded, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, Application::gImageCount };
	addDescriptorSet(pRenderer, &setDesc, &pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);

	return true;
}

void Application::RemoveDescriptorSets()
{
	removeDescriptorSet(pRenderer, pDescriptorSetVignette);
	removeDescriptorSet(pRenderer, pDescriptorSetFXAA);
	removeDescriptorSet(pRenderer, pDescriptorSetWatermark);
	removeDescriptorSet(pRenderer, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
	removeDescriptorSet(pRenderer, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
}

void Application::PrepareDescriptorSets()
{
	// Shadow
	{
		DescriptorData params[2] = {};
		if (player->pNodeTransformsBuffer)
		{
			params[0].pName = "modelToWorldMatrices";
			params[0].ppBuffers = &player->pNodeTransformsBuffer;
			updateDescriptorSet(pRenderer, 0, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_NONE], 1, params);
		}

		params[0].pName = "sceneTexture";
		params[0].ppTextures = &pForwardRT->pTexture;
		updateDescriptorSet(pRenderer, 0, pDescriptorSetVignette, 1, params);

		params[0].pName = "sceneTexture";
		params[0].ppTextures = &pPostProcessRT->pTexture;
		updateDescriptorSet(pRenderer, 0, pDescriptorSetFXAA, 1, params);

		params[0].pName = "sceneTexture";
		params[0].ppTextures = &pTextureBlack;
		updateDescriptorSet(pRenderer, 0, pDescriptorSetWatermark, 1, params);

		for (uint32_t i = 0; i < Application::gImageCount; ++i)
		{
			DescriptorData params[2] = {};
			params[0].pName = "cbPerPass";
			params[0].ppBuffers = &pShadowUniformBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShadow[DESCRIPTOR_UPDATE_FREQ_PER_FRAME], 1, params);
		}
	}
	// Shading
	{
		DescriptorData params[3] = {};
		params[0].pName = "ShadowTexture";
		params[0].ppTextures = &pShadowRT->pTexture;
		if (player->pNodeTransformsBuffer)
		{
			params[1].pName = "modelToWorldMatrices";
			params[1].ppBuffers = &player->pNodeTransformsBuffer;
		}
		updateDescriptorSet(pRenderer, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE], player->pNodeTransformsBuffer ? 2 : 1, params);

		for (uint32_t i = 0; i < Application::gImageCount; ++i)
		{
			params[0].pName = "cbPerPass";
			params[0].ppBuffers = &pUniformBuffer[i];
			updateDescriptorSet(pRenderer, i, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME], 1, params);
		}
	}

	player->createMaterialResources(pRootSignatureShaded, /* bindlessTexturesSamplersSet = */ NULL);

	for (auto other : others) {
		other->createMaterialResources(pRootSignatureShaded, /* bindlessTexturesSamplersSet = */ NULL);
	}
}

void Application::RemoveShaderResources()
{
	RemoveDescriptorSets();

	removeShader(pRenderer, pShaderZPass);
	removeShader(pRenderer, pShaderZPass_NonOptimized);
	removeShader(pRenderer, pVignetteShader);
	removeShader(pRenderer, pFloorShader);
	removeShader(pRenderer, pMeshOptDemoShader);
	removeShader(pRenderer, pFXAAShader);
	removeShader(pRenderer, pWaterMarkShader);

	removeRootSignature(pRenderer, pRootSignatureShadow);
	removeRootSignature(pRenderer, pRootSignatureShaded);
	removeRootSignature(pRenderer, pRootSignaturePostEffects);
}

void Application::RemoveModelDependentResources()
{
	RemoveShaderResources();

	player->removeResources();
	conf_free(player);
	player = conf_new(GLTFObject);

	for (auto other : others) {
		other->removeResources();
		conf_free(other);
		other = conf_new(GLTFObject);
	}
}

void Application::Exit()
{
	waitQueueIdle(pGraphicsQueue);

	Input::Exit();

	exitProfiler();

	destroyCameraController(pCameraController);
	destroyCameraController(pLightView);

#if defined(TARGET_IOS) || defined(__ANDROID__)
	gVirtualJoystick.Exit();
#endif

	gAppUI.Exit();

	for (uint32_t i = 0; i < Application::gImageCount; ++i)
	{
		removeResource(pShadowUniformBuffer[i]);
		removeResource(pUniformBuffer[i]);
		removeResource(pFloorUniformBuffer[i]);
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

	removeResource(pFloorVB);
	removeResource(pFloorIB);

	removeResource(pTextureBlack);

	exitResourceLoaderInterface(pRenderer);
	removeQueue(pRenderer, pGraphicsQueue);
	removeRenderer(pRenderer);

#if defined(__linux__) || defined(__ANDROID__)
	gModelFiles.set_capacity(0);
	gDropDownWidgetData.set_capacity(0);
#endif

	gModelFile = NULL;
	gGuiModelToLoad = NULL;

	conf_free(player);

	for (auto other : others) {
		conf_free(other);
	}
}

void Application::LoadPipelines()
{
	PipelineDesc desc = {};

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
		desc = {};
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
		desc = {};
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
		desc = {};
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
		shadowMapPipelineSettings.pShaderProgram = pShaderZPass_NonOptimized;
		shadowMapPipelineSettings.pVertexLayout = &screenTriangle_VertexLayout;
		addPipeline(pRenderer, &desc, &pPipelineShadowPass_NonOPtimized);
	}

	{
		desc = {};
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
		pipelineSettings.pVertexLayout = &screenTriangle_VertexLayout;
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		pipelineSettings.pShaderProgram = pFloorShader;
		addPipeline(pRenderer, &desc, &pFloorPipeline);
	}

	{
		desc = {};
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
		desc = {};
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
		desc = {};
		desc.mType = PIPELINE_TYPE_GRAPHICS;
		GraphicsPipelineDesc& pipelineSettings = desc.mGraphicsDesc;
		pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
		pipelineSettings.mRenderTargetCount = 1;
		pipelineSettings.pDepthState = NULL;
		pipelineSettings.pBlendState = &blendStateAlphaDesc;
		pipelineSettings.pColorFormats = &pSwapChain->ppRenderTargets[0]->mFormat;
		pipelineSettings.mSampleCount = pSwapChain->ppRenderTargets[0]->mSampleCount;
		pipelineSettings.mSampleQuality = pSwapChain->ppRenderTargets[0]->mSampleQuality;
		pipelineSettings.pRootSignature = pRootSignaturePostEffects;
		pipelineSettings.pVertexLayout = &screenTriangle_VertexLayout;
		pipelineSettings.pRasterizerState = &rasterizerStateDesc;
		pipelineSettings.pShaderProgram = pWaterMarkShader;
		addPipeline(pRenderer, &desc, &pWaterMarkPipeline);
	}
}

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

	InitModelDependentResources();

	LoadPipelines();

	float wmHeight = min(mSettings.mWidth, mSettings.mHeight) * 0.09f;
	float wmWidth = wmHeight * 2.8077f;

	float widthGap = wmWidth * 2.0f / (float)mSettings.mWidth;
	float heightGap = wmHeight * 2.0f / (float)mSettings.mHeight;

	float pixelGap = 80.0f;
	float widthRight = 1.0f - pixelGap / (float)mSettings.mWidth;
	float heightDown = -1.0f + pixelGap / (float)mSettings.mHeight;

	float screenWaterMarkPoints[] = {
		widthRight - widthGap,	heightDown + heightGap, 0.5f, 0.0f, 0.0f,
		widthRight - widthGap,	heightDown,				0.5f, 0.0f, 1.0f,
		widthRight,				heightDown + heightGap, 0.5f, 1.0f, 0.0f,

		widthRight,				heightDown + heightGap, 0.5f, 1.0f, 0.0f,
		widthRight - widthGap,	heightDown,				0.5f, 0.0f, 1.0f,
		widthRight,				heightDown,				0.5f, 1.0f, 1.0f
	};

	BufferLoadDesc screenQuadVbDesc = {};
	screenQuadVbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
	screenQuadVbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
	screenQuadVbDesc.mDesc.mSize = sizeof(float) * 5 * 6;
	screenQuadVbDesc.pData = screenWaterMarkPoints;
	screenQuadVbDesc.ppBuffer = &WaterMarkVB;
	addResource(&screenQuadVbDesc, NULL, LOAD_PRIORITY_NORMAL);

	return true;
}

void Application::RemovePipelines()
{
	removePipeline(pRenderer, pPipelineShadowPass_NonOPtimized);
	removePipeline(pRenderer, pPipelineShadowPass);
	removePipeline(pRenderer, pVignettePipeline);
	removePipeline(pRenderer, pFloorPipeline);
	removePipeline(pRenderer, pMeshOptDemoPipeline);
	removePipeline(pRenderer, pFXAAPipeline);
	removePipeline(pRenderer, pWaterMarkPipeline);
}

void Application::Unload()
{
	waitQueueIdle(pGraphicsQueue);
	waitForFences(pRenderer, Application::gImageCount, pRenderCompleteFences);

	RemoveModelDependentResources();

	unloadProfilerUI();
	gAppUI.Unload();

#if defined(TARGET_IOS) || defined(__ANDROID__)
	gVirtualJoystick.Unload();
#endif

	removeResource(WaterMarkVB);

	RemovePipelines();

	removeSwapChain(pRenderer, pSwapChain);

	removeRenderTarget(pRenderer, pPostProcessRT);
	removeRenderTarget(pRenderer, pForwardRT);
	removeRenderTarget(pRenderer, pDepthBuffer);
	removeRenderTarget(pRenderer, pShadowRT);
}

void Application::Update(float deltaTime)
{
	Input::Update(mSettings.mWidth, mSettings.mHeight);

#if !defined(__ANDROID__) && !defined(TARGET_IOS) && !defined(_DURANGO)
	if (pSwapChain->mEnableVsync != bToggleVSync)
	{
		waitQueueIdle(pGraphicsQueue);
		::toggleVSync(pRenderer, &pSwapChain);
	}
#endif

	pCameraController->update(deltaTime);
	/************************************************************************/
	// Scene Update
	/************************************************************************/
	mat4 viewMat = pCameraController->getViewMatrix();
	const float aspectInverse = (float)mSettings.mHeight / (float)mSettings.mWidth;
	const float horizontal_fov = PI / 3.0f;
	mat4 projMat = mat4::perspectiveReverseZ(horizontal_fov, aspectInverse, 0.001f, 1000.0f);
	gUniformData.mProjectView = projMat * viewMat;
	gUniformData.mModel = mat4::identity();
	gUniformData.mCameraPosition = vec4(pCameraController->getViewPosition(), 1.0f);

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

	gFloorUniformBlock.projViewMat = gUniformData.mProjectView;
	gFloorUniformBlock.worldMat = mat4::scale(vec3(50.0f));
	gFloorUniformBlock.screenSize = vec4((float)mSettings.mWidth, (float)mSettings.mHeight, 1.0f / mSettings.mWidth, bVignetting ? 1.0f : 0.0f);

	//rot += 0.001f;
	//player->setScaleRot(vec3(0.2f), rot, vec3(0.0f, 1.0f, 0.0f));

	currVelX -= drag * currVelX;
	currVelY -= drag * currVelY;
	if (Input::inputs[InputEnum::INPUT_UP] > 0.0f) {
		currVelY += acceleration * deltaTime;
	}
	if (Input::inputs[InputEnum::INPUT_LEFT] > 0.0f) {
		currVelX -= acceleration * deltaTime;
	}
	if (Input::inputs[InputEnum::INPUT_DOWN] > 0.0f) {
		currVelY -= acceleration * deltaTime;
	}
	if (Input::inputs[InputEnum::INPUT_RIGHT] > 0.0f) {
		currVelX += acceleration * deltaTime;
	}

	if (sqrt(currVelX * currVelX + currVelY * currVelY) > 0.01) {
		currPosX += currVelX;
		currPosY += currVelY;

		player->setPositionDirection(vec3(currPosX, 0, currPosY), vec3(currVelX, 0, currVelY));
	}

	pCameraController->moveTo(player->getPosition() + cameraOffset);
	pCameraController->lookAt(player->getPosition() + vec3(0, 0.4f, 0));

	/************************************************************************/
	// Light Matrix Update - for shadow map
	/************************************************************************/

	vec3 lightPos = sunDirection * 4.0f;
	pLightView->moveTo(lightPos);
	pLightView->lookAt(vec3(0.0f));

	mat4 lightView = pLightView->getViewMatrix();
	//perspective as spotlight, for future use. TODO: Use a frustum fitting algorithm to maximise effective resolution!
	const float shadowRange = 2.7f;
	//const float shadowHalfRange = shadowRange * 0.5f;
	mat4 lightProjMat = mat4::orthographicReverseZ(-shadowRange, shadowRange, -shadowRange, shadowRange, shadowRange * 0.5f, shadowRange * 4.0f);

	gShadowUniformData.ViewProj = lightProjMat * lightView;
	gUniformData.mShadowLightViewProj = gShadowUniformData.ViewProj;

	/************************************************************************/
	/************************************************************************/

	//gAppUI.Update(deltaTime);
}

void Application::PostDrawUpdate()
{

#if defined(__ANDROID__) || defined(__LINUX__)
	if (guiModelToLoadIndex != modelToLoadIndex)
	{
		modelToLoadIndex = guiModelToLoadIndex;
		gGuiModelToLoad = gModelFiles[modelToLoadIndex];
	}
#endif
	if (!fsPathsEqual(gGuiModelToLoad, gModelFile))
	{
		if (fsFileExists(gGuiModelToLoad))
		{
			gModelFile = gGuiModelToLoad;

			Unload();
			Load();
		}
		else
		{
			gGuiModelToLoad = gModelFile;
		}
	}
	gCurrentLod = min(gCurrentLod, gMaxLod);
}

void Application::SelectModelFunc(const Path* path, void* pathPtr) {
	PathHandle* outputPath = (PathHandle*)pathPtr;

	if (path) {
		*outputPath = fsCopyPath(path);
	}
}

void Application::LoadNewModel()
{
	eastl::vector<const char*> extFilter;
	extFilter.push_back("gltf");
	extFilter.push_back("glb");

	PathHandle meshDir = fsCopyPathForResourceDirectory(RD_MESHES);

	fsShowOpenFileDialog("Select model to load", meshDir, SelectModelFunc, &gGuiModelToLoad, "Model File", &extFilter[0], extFilter.size());
}

void Application::LoadLOD()
{
	waitQueueIdle(pGraphicsQueue);

	eastl::vector<const char*> extFilter;
	extFilter.push_back("gltf");
	extFilter.push_back("glb");

	PathHandle meshDir = fsCopyPathForResourceDirectory(RD_MESHES);

	fsShowOpenFileDialog("Select model to load", meshDir, SelectModelFunc, &gGuiModelToLoad, "Model File", &extFilter[0], extFilter.size());
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

	// Update uniform buffers
	BufferUpdateDesc shaderCbv = { pUniformBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
	endUpdateResource(&shaderCbv, NULL);

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

	drawShadowMap(cmd);

	{
		LoadActionsDesc loadActions = {};
		loadActions.mLoadActionsColor[0] = LOAD_ACTION_CLEAR;
		loadActions.mClearColorValues[0].r = bgColor.getX();
		loadActions.mClearColorValues[0].g = bgColor.getY();
		loadActions.mClearColorValues[0].b = bgColor.getZ();
		loadActions.mClearColorValues[0].a = bgColor.getW();
		loadActions.mLoadActionDepth = LOAD_ACTION_CLEAR;
		loadActions.mClearDepth.depth = 0.0f;
		loadActions.mClearDepth.stencil = 0;

		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Floor");

		pRenderTarget = pForwardRT;

		RenderTargetBarrier barriers[] =
		{
			{ pRenderTarget, RESOURCE_STATE_RENDER_TARGET },
			{ pDepthBuffer, RESOURCE_STATE_DEPTH_WRITE },
			{ pShadowRT, RESOURCE_STATE_SHADER_RESOURCE }
		};
		cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 3, barriers);

		cmdBindRenderTargets(cmd, 1, &pRenderTarget, pDepthBuffer, &loadActions, NULL, NULL, -1, -1);
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

		cmdBindPipeline(cmd, pFloorPipeline);

		// Update uniform buffers
		BufferUpdateDesc Cb = { pFloorUniformBuffer[Application::gFrameIndex] };
		beginUpdateResource(&Cb);
		*(UniformBlock_Floor*)Cb.pMappedData = gFloorUniformBlock;
		endUpdateResource(&Cb, NULL);

		cmdBindDescriptorSet(cmd, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
		cmdBindDescriptorSet(cmd, Application::gFrameIndex, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);

		const uint32_t stride = sizeof(float) * 5;
		cmdBindVertexBuffer(cmd, 1, &pFloorVB, &stride, NULL);
		cmdBindIndexBuffer(cmd, pFloorIB, INDEX_TYPE_UINT16, 0);

		cmdDrawIndexed(cmd, 6, 0, 0);

		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	//// draw scene

	{
		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Scene");

		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);



		for (auto other : others) {
			cmdBindPipeline(cmd, pMeshOptDemoPipeline);

			gUniformData.mModel = other->model;
			BufferUpdateDesc shaderCbv = { pUniformBuffer[Application::gFrameIndex] };
			beginUpdateResource(&shaderCbv);
			*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
			endUpdateResource(&shaderCbv, NULL);

			cmdBindDescriptorSet(cmd, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
			cmdBindDescriptorSet(cmd, Application::gFrameIndex, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
			other->draw(cmd, pRootSignatureShaded, true);
		}

		cmdBindPipeline(cmd, pMeshOptDemoPipeline);





		// Update uniform buffers
		gUniformData.mModel = player->model;
		BufferUpdateDesc shaderCbv = { pUniformBuffer[Application::gFrameIndex] };
		beginUpdateResource(&shaderCbv);
		*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
		endUpdateResource(&shaderCbv, NULL);

		cmdBindDescriptorSet(cmd, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
		cmdBindDescriptorSet(cmd, Application::gFrameIndex, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
		player->draw(cmd, pRootSignatureShaded, true);



		// Update uniform buffers
		//gUniformData.mModel = player->model * mat4::translation(vec3(1, 0, 0));
		shaderCbv = { pUniformBuffer[Application::gFrameIndex] };
		beginUpdateResource(&shaderCbv);
		*(UniformBlock*)shaderCbv.pMappedData = gUniformData;
		endUpdateResource(&shaderCbv, NULL);

		cmdBindPipeline(cmd, pMeshOptDemoPipeline);
		cmdBindDescriptorSet(cmd, 0, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_NONE]);
		cmdBindDescriptorSet(cmd, Application::gFrameIndex, pDescriptorSetsShaded[DESCRIPTOR_UPDATE_FREQ_PER_FRAME]);
		//player->draw(cmd, pRootSignatureShaded, true);



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


	if (bScreenShotMode)
	{
		cmdBindRenderTargets(cmd, 1, &pRenderTarget, NULL, NULL, NULL, NULL, -1, -1);
		cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
		cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);

		cmdBeginGpuTimestampQuery(cmd, gGpuProfileToken, "Draw Water Mark");

		cmdBindPipeline(cmd, pWaterMarkPipeline);

		cmdBindDescriptorSet(cmd, 0, pDescriptorSetVignette);

		const uint32_t stride = sizeof(float) * 5;
		cmdBindVertexBuffer(cmd, 1, &WaterMarkVB, &stride, NULL);
		cmdDraw(cmd, 6, 0);

		cmdBindRenderTargets(cmd, 0, NULL, 0, NULL, NULL, NULL, -1, -1);
		cmdEndGpuTimestampQuery(cmd, gGpuProfileToken);
	}

	if (!bScreenShotMode)
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

		cmdDrawCpuProfile(cmd, float2(8.0f, 15.0f), &gFrameTimeDraw);
#if !defined(__ANDROID__)
		cmdDrawGpuProfile(cmd, float2(8, 40), gGpuProfileToken);
#endif

		cmdDrawProfilerUI();

		gAppUI.Gui(pGuiWindow);
		gAppUI.Gui(pGuiGraphics);

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

	PostDrawUpdate();
}

const char* Application::GetName() { return "08_GltfViewer"; }

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

void Application::RecenterCameraView(float maxDistance, vec3 lookAt = vec3(0))
{
	vec3 p = pCameraController->getViewPosition();
	vec3 d = p - lookAt;

	float lenSqr = lengthSqr(d);
	if (lenSqr > (maxDistance * maxDistance))
	{
		d *= (maxDistance / sqrtf(lenSqr));
	}

	p = d + lookAt;
	pCameraController->moveTo(p);
	pCameraController->lookAt(lookAt);
}


DEFINE_APPLICATION_MAIN(Application)
