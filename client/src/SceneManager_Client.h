#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include <ctime>
#include <map>

#include "Transform.h"
#include "GLTFGeode.h"

#include "Input.h"

#include "../../common/macros.h"
#include "../../common/GameObject.h"
#include "../../common/entity.h"
#include "../../common/player.h"
#include "../../common/client2server.h"

class SceneManager_Client : public Transform
{
private:
	//std::map<std::string, Entity*> idMap;

public:
	// This vector should eventually be split between tracked objects and client only objects
	std::vector<Transform*> transforms;

	// Will likely try to convert to a dictionary
	std::vector<GLTFGeode*> gltfGeodes;

	// Would likely like to create a single Game object that contains all game components
	Player player = Player(mat4::identity());

	SceneManager_Client(Renderer* renderer);
	~SceneManager_Client();

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateFromClientBuf(char buf[]);
	void updateFromInputBuf(float deltaTime);

	void setProgram(Geode::GeodeShaderDesc program);
};