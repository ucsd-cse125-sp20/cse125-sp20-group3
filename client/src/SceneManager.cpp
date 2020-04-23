#include "SceneManager.h"

namespace {
	const char* playerFile = "WeirdBox.gltf";
	const char* groundFile = "Ground.gltf";
	const char* otherFile = "Kyubey.gltf";

	float currPosX = 0;
	float currPosY = 0;
	float currVelX = 0;
	float currVelY = 0;
	float acceleration = 1;
	float drag = 0.1f;
}

SceneManager::SceneManager(Renderer* renderer)
{
	// It'd be nice if I could put this in a loop later
	gltfGeodes.push_back(conf_new(GLTFGeode, renderer, playerFile));
	gltfGeodes.push_back(conf_new(GLTFGeode, renderer, groundFile));
	gltfGeodes.push_back(conf_new(GLTFGeode, renderer, otherFile));

	Transform* t = conf_new(Transform, mat4::identity());
	t->addChild(gltfGeodes[0]);
	this->addChild(t);
	transforms.push_back(t);

	t = conf_new(Transform, mat4::identity());
	t->addChild(gltfGeodes[1]);
	this->addChild(t);
	transforms.push_back(t);

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 50; i++) {
		float x = -50 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (100)));
		float z = -50 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (100)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		t->addChild(gltfGeodes[2]);
		this->addChild(t);
		transforms.push_back(t);
	}
}

SceneManager::~SceneManager()
{
	for (auto t : transforms) conf_delete(t);
	for (auto g : gltfGeodes) conf_delete(g);
}

void SceneManager::createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	for (auto g : gltfGeodes) {
		g->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
	}
}

void SceneManager::updateFromClientBuf(char buf[])
{
	Player::PlayerData data = ((Player::PlayerData*)buf)[0];
	player.setPosRot(data.x, data.z, data.rot);
	transforms[0]->setMatrix(player.getMatrix());
}

void SceneManager::updateFromInputBuf(float deltaTime)
{
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

		transforms[0]->setPositionDirection(vec3(currPosX, 0, currPosY), vec3(currVelX, 0, currVelY));
	}
}

void SceneManager::setProgram(Geode::GeodeShaderDesc program)
{
	for (auto g : gltfGeodes) {
		g->setProgram(program);
	}
}
