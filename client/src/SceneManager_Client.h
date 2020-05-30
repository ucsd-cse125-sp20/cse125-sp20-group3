#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include <ctime>
#include <map>

#include "Transform.h"
#include "GLTFGeode.h"
#include "OzzGeode.h"
#include "ParticleSystemGeode.h"
#include "Animator.h"

#include "Input.h"

#include "Client.h"
#include "../../common/macros.h"
#include "../../common/GameObject.h"
#include "../../common/entity.h"
#include "../../common/team.h"
#include "../../common/player_client.h"
#include "../../common/base.h"
#include "../../common/minion_client.h"
#include "../../common/superminion_client.h"
#include "../../common/tower.h"
#include "../../common/lasertower_client.h"
#include "../../common/clawtower_client.h"
#include "../../common/resource_client.h"
#include "../../common/client2server.h"

#define GROUND_KEY 111111
#define NO_TRACKED_PLAYER -1

#define ENV_GEODE "env_geometry"
#define PLAYER_GEODE "player_geometry"
#define BASE_GEODE "base_geometry"
#define MINION_GEODE "minion_geometry"
#define SUPER_MINION_GEODE "super_minion_geometry"
#define LASER_TOWER_GEODE "laser_tower_geometry"
#define CLAW_TOWER_GEODE "claw_tower_geometry"
#define DUMPSTER_GEODE "dumpster_geometry"
#define RECYCLING_BIN_GEODE "recycling_bin_geometry"

class SceneManager_Client : public Transform
{
private:
	std::map<int, Entity*> idMap;
	std::map<int, Entity_Client*> wrapperMap;
	std::map<int, Transform*> transforms;
	std::map<int, Animator*> animators;
	std::map<std::string, GLTFGeode*> gltfGeodes;
	std::map<std::string, OzzGeode*> ozzGeodes;
	std::map<std::string, ParticleSystemGeode*> particleGeodes;
	int trackedPlayer_ID = -1;

	std::vector<Transform*> otherTransforms;
    
    Buffer** instanceBuffer = NULL;
	Buffer** boneBuffer = NULL;
	Buffer** particleBuffer = NULL;

	RootSignature* particleRootSignature = NULL;

	Team *red_team, *blue_team;

	Renderer* renderer;

public:
	enum class GeodeType {
		MESH, ANIMATED_MESH, PARTICLES
	};

	enum class SceneBuffer {
		INSTANCE, BONE, PARTICLES
	};

	static bool enableCulling;

	SceneManager_Client(Renderer* renderer);
	~SceneManager_Client();

	void createMaterialResources(SceneManager_Client::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateStateAndScene(Client::UpData data);
	void updateState(Client::StateUpdateData updateData);
	void updateScene(Client::SceneUpdateData updateData);
	void updateFromInputBuf(float deltaTime);

	vec3 getTargetPosition(int targetID);

	void setBuffer(SceneManager_Client::SceneBuffer type, Buffer** buffers); // TODO Could probably mange instance buffers within class, rather than app
	void setProgram(SceneManager_Client::GeodeType type, Geode::GeodeShaderDesc program);

	void trackPlayer(int player_id);
	mat4 getPlayerTransformMat();

	void update(float deltaTime) override;
	void draw(Cmd* cmd) override;

	void updateUI();
};