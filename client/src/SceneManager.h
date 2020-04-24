#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include <ctime>

#include "Transform.h"
#include "GLTFGeode.h"

#include "Input.h"

#include "../../server/GameObject.h"
#include "../../common/player.h"
#include "../../common/client2server.h"

class SceneManager : public Transform
{
public:
	// This vector should eventually be split between tracked objects and client only objects
	std::vector<Transform*> transforms;

	// Will likely try to convert to a dictionary
	std::vector<GLTFGeode*> gltfGeodes;

	// Would likely like to create a single Game object that contains all game components
	Player player = Player(mat4::identity());

	SceneManager(Renderer* renderer);
	~SceneManager();

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateFromClientBuf(char buf[]);
	void updateFromInputBuf(float deltaTime);

	void setProgram(Geode::GeodeShaderDesc program);
};