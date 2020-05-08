#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include <ctime>
#include <map>

#include "Transform.h"
#include "GLTFGeode.h"
#include "OzzGeode.h"
#include "Animator.h"

#include "Input.h"

#include "../../common/macros.h"
#include "../../common/GameObject.h"
#include "../../common/entity.h"
#include "../../common/player.h"
#include "../../common/base.h"
#include "../../common/minion.h"
#include "../../common/tower.h"
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
	std::map<std::string, Animator*> animators;
	std::map<std::string, GLTFGeode*> gltfGeodes;
	std::map<std::string, OzzGeode*> ozzGeodes;
	std::string trackedPlayer_ID;

	std::vector<Transform*> player_adjustments;
    
    Buffer** instanceBuffer = NULL;
	Buffer** boneBuffer = NULL;

public:
	enum class GeodeType {
		MESH, ANIMATED_MESH
	};

	enum class SceneBuffer {
		INSTANCE, BONE
	};

	static bool enableCulling;

	// This vector should eventually be split between tracked objects and client only objects
	//std::vector<Transform*> transforms;

	// Will likely try to convert to a dictionary
	//std::vector<GLTFGeode*> gltfGeodes;

	SceneManager_Client(Renderer* renderer);
	~SceneManager_Client();

	void createMaterialResources(SceneManager_Client::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateFromClientBuf(char buf[], int bufsize);
	void updateFromInputBuf(float deltaTime);

	void setBuffer(SceneManager_Client::SceneBuffer type, Buffer** buffers); // TODO Could probably mange instance buffers within class, rather than app
	void setProgram(SceneManager_Client::GeodeType type, Geode::GeodeShaderDesc program);

	void trackPlayer(std::string player_id);
	mat4 getPlayerTransformMat();

    void randomStaticInstantiation(Geode* g, int num, float range, float minSize, float maxSize);
	void randomAnimatedInstantiation(OzzGeode* g, int num, float range, float minSize, float maxSize, const char* actions[], int numActions);

	void update(float deltaTime) override;
	void draw(Cmd* cmd) override;
};