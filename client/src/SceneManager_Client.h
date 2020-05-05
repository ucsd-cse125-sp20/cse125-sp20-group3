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

#define ENV_GEODE "env_geometry"
#define PLAYER_GEODE "player_geometry"
#define BASE_GEODE "base_geometry"
#define MINION_GEODE "minion_geometry"
#define TOWER_GEODE "tower_geometry"
#define RESOURCE_GEODE "resource_geometry"

class SceneManager_Client : public Transform
{
private:
	std::map<std::string, Entity*> idMap;
	std::map<std::string, Transform*> transforms;
	std::map<std::string, GLTFGeode*> gltfGeodes;
	std::string trackedPlayer_ID;

public:
	// This vector should eventually be split between tracked objects and client only objects
	//std::vector<Transform*> transforms;

	// Will likely try to convert to a dictionary
	//std::vector<GLTFGeode*> gltfGeodes;

	SceneManager_Client(Renderer* renderer);
	~SceneManager_Client();

	void createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateFromClientBuf(char buf[], int bufsize);
	void updateFromInputBuf(float deltaTime);

	void setProgram(Geode::GeodeShaderDesc program);

	void trackPlayer(std::string player_id);
	mat4 getPlayerTransformMat();
};