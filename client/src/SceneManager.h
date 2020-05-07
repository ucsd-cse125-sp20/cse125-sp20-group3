#pragma once

#include "../The-Forge/Common_3/Renderer/IRenderer.h"

#include <ctime>

#include "Transform.h"
#include "GLTFGeode.h"
#include "OzzGeode.h"
#include "Animator.h"

#include "Input.h"

#include "../../common/GameObject.h"
#include "../../common/player.h"
#include "../../common/client2server.h"

class SceneManager : public Transform
{
public:
	enum class GeodeType {
		MESH, ANIMATED_MESH
	};

	enum class SceneBuffer {
		INSTANCE, BONE
	};
	
	// This vector should eventually be split between tracked objects and client only objects
	std::vector<Transform*> transforms;
	std::vector<Animator*> animators;

	// Will likely try to convert to a dictionary
	std::vector<GLTFGeode*> meshes;
	std::vector<OzzGeode*> animatedMeshes;

	// Would likely like to create a single Game object that contains all game components
	Player player = Player(mat4::identity());

	Buffer** instanceBuffer = NULL;
	Buffer** boneBuffer = NULL;

	static bool enableCulling;

	SceneManager(Renderer* renderer);
	~SceneManager();

	void randomStaticInstantiation(Geode* g, int num, float range, float minSize, float maxSize);
	void randomAnimatedInstantiation(OzzGeode* g, int num, float range, float minSize, float maxSize, const char* actions[], int numActions);

	void createMaterialResources(SceneManager::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler);

	void updateFromClientBuf(char buf[]);
	void updateFromInputBuf(float deltaTime);

	void setBuffer(SceneManager::SceneBuffer type, Buffer** buffers); // TODO Could probably mange instance buffers within class, rather than app
	void setProgram(SceneManager::GeodeType type, Geode::GeodeShaderDesc program);

	void draw(Cmd* cmd) override;
};