#include "SceneManager.h"
#include "Application.h"

namespace {
	const char* playerFile1 = "char-1-female.gltf";
	const char* player2File = "char-2-male.gltf";
	const char* groundFile = "Ground.gltf";
	const char* towerFile1 = "tower-1-laser.gltf";
	const char* other2File = "tower-2-aoe.gltf";
	const char* minionFile1 = "minion-retry.gltf";
	const char* thingBIGFile = "minion-2-super.gltf";
	const char* clearFile = "ClearBox.gltf";
	const char* trashFile = "resource-1-dumpster.gltf";
	const char* trasherFile = "resource-2-recycling-bin.gltf";

	const char* blarfDir = "Gou";
	const char* blarfActions[4] = { "Idle", "Walk", "Run", "Punch_Full" };

	int counter = 0;
	int animCounter = 0;
}

bool SceneManager::enableCulling = true;

SceneManager::SceneManager(Renderer* renderer)
{
	// It'd be nice if I could put this in a loop later
	meshes.push_back(conf_new(GLTFGeode, renderer, playerFile1));//0
	meshes.push_back(conf_new(GLTFGeode, renderer, groundFile));//1
	meshes.push_back(conf_new(GLTFGeode, renderer, minionFile1));//2
	meshes.push_back(conf_new(GLTFGeode, renderer, towerFile1));//3
	//meshes.push_back(conf_new(GLTFGeode, renderer, player2File));//4
	//meshes.push_back(conf_new(GLTFGeode, renderer, other2File));//5
	//meshes.push_back(conf_new(GLTFGeode, renderer, thingBIGFile));//6
	//meshes.push_back(conf_new(GLTFGeode, renderer, trashFile));//7
	//meshes.push_back(conf_new(GLTFGeode, renderer, trasherFile));//8
	
	animatedMeshes.push_back(conf_new(OzzGeode, renderer, blarfDir));
	((OzzObject*)animatedMeshes.back()->obj)->SetClip(blarfActions[0]);

	Transform* t = conf_new(Transform, mat4::identity());
	Transform* t2 = conf_new(Transform, mat4::rotationY(-PI/2));
	t->addChild(t2);
	t2->addChild(meshes[0]);
	this->addChild(t);
	transforms.push_back(t);
	transforms.push_back(t2);

	t = conf_new(Transform, mat4::identity());
	t->addChild(meshes[1]);
	this->addChild(t);
	transforms.push_back(t);

	srand((unsigned int)time(NULL));
	randomStaticInstantiation(meshes[2], 150, 100, 1.0f, 1.5f);
	randomStaticInstantiation(meshes[3], 50, 100, 1.0f, 1.5f);
	randomAnimatedInstantiation(animatedMeshes[0], 1, 5, 0.15f, 0.2f, blarfActions, 4);
}

SceneManager::~SceneManager()
{
	for (auto t : transforms) conf_delete(t);
	for (auto a : animators) conf_delete(a);
	for (auto g : meshes) conf_delete(g);
	for (auto g : animatedMeshes) conf_delete(g);
}

void SceneManager::randomStaticInstantiation(Geode* g, int num, float range, float minSize, float maxSize) {
	for (int i = 0; i < num; i++) {
		float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * PI)));
		float s = minSize + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxSize - minSize)));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Transform* t = conf_new(Transform, transform);
		t->addChild(g);
		this->addChild(t);
		transforms.push_back(t);
	}
}

void SceneManager::randomAnimatedInstantiation(OzzGeode* g, int num, float range, float minSize, float maxSize, const char* actions[], int numActions) {
	for (int i = 0; i < num; i++) {
		float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * PI)));
		float s = minSize + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxSize - minSize)));
		int r = rand() % numActions;

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Transform* t = conf_new(Transform, transform);
		Animator* a = conf_new(Animator, g);
		a->SetClip(actions[r]);
		t->addChild(a);
		this->addChild(t);
		transforms.push_back(t);
		animators.push_back(a);
	}
}

void SceneManager::createMaterialResources(SceneManager::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	switch (type) {
	case SceneManager::GeodeType::MESH:
		for (auto g : meshes) {
			g->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	case SceneManager::GeodeType::ANIMATED_MESH:
		for (auto g : animatedMeshes) {
			g->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	}
}

void SceneManager::updateFromClientBuf(char buf[])
{
	GameObject::GameObjectData data = ((GameObject::GameObjectData*)buf)[0];
	player.setPosRot(data.x, data.z, data.rot);
	transforms[0]->setMatrix(player.getMatrix());
}

void SceneManager::updateFromInputBuf(float deltaTime)
{
	char recvbuf[DEFAULT_BUFLEN];
	Input::EncodeToBuf(recvbuf);
	PlayerInput input = ((PlayerInput*)recvbuf)[0];
	//printf("%d %d %f\n", input.move_x, input.move_z, input.view_y_rot);
	player.setMoveAndDir(input);
	player.update();
	transforms[0]->setMatrix(player.getMatrix());
}

void SceneManager::setBuffer(SceneManager::SceneBuffer type, Buffer** buffer)
{
	switch (type) {
	case SceneManager::SceneBuffer::INSTANCE:
		instanceBuffer = buffer;
		break;
	case SceneManager::SceneBuffer::BONE:
		boneBuffer = buffer;
		break;
	}
}

void SceneManager::setProgram(SceneManager::GeodeType type, Geode::GeodeShaderDesc program)
{
	switch (type) {
	case SceneManager::GeodeType::MESH:
		for (auto g : meshes) {
			g->setProgram(program);
		}
		break;
	case SceneManager::GeodeType::ANIMATED_MESH:
		for (auto g : animatedMeshes) {
			g->setProgram(program);
		}
		break;
	}
}

void SceneManager::update(float deltaTime)
{
	if (counter++ % 50 == 0) animators[0]->SetClip(blarfActions[animCounter = (animCounter + 1) % 4]);
	Transform::update(deltaTime);
}

void SceneManager::draw(Cmd* cmd)
{
	// Update per-instance uniforms
	BufferUpdateDesc shaderCbv = { instanceBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateTransformBuffer(shaderCbv, mat4::identity());
	endUpdateResource(&shaderCbv, NULL);
	
	shaderCbv = { boneBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateBoneBuffer(shaderCbv, NULL);
	endUpdateResource(&shaderCbv, NULL);

	// Do culling check
	vec4 frustumPlanes[6];
	mat4::extractFrustumClipPlanes(Application::projMat * Application::viewMat, frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], frustumPlanes[3], frustumPlanes[4], frustumPlanes[5], true);
	cull(frustumPlanes, SceneManager::enableCulling);
	
	// Draw Graph
	Transform::draw(cmd);
}
