#include "SceneManager.h"
#include "Application.h"

namespace {
	const char* playerFile = "female-char.gltf";
	const char* groundFile = "Ground.gltf";
	const char* otherFile = "tower-1-laser.gltf";
	const char* thingFile = "minion-retry.gltf";

	const char* blarfDir = "kyubey";
	const char* blarfClip = "ArmatureAction";
}

bool SceneManager::enableCulling = true;

SceneManager::SceneManager(Renderer* renderer)
{
	// It'd be nice if I could put this in a loop later
	meshes.push_back(conf_new(GLTFGeode, renderer, playerFile));
	meshes.push_back(conf_new(GLTFGeode, renderer, groundFile));
	meshes.push_back(conf_new(GLTFGeode, renderer, otherFile));
	meshes.push_back(conf_new(GLTFGeode, renderer, thingFile));
	
	animatedMeshes.push_back(conf_new(OzzGeode, renderer, blarfDir));
	((OzzObject*)animatedMeshes.back()->obj)->AddClip(blarfClip); // TODO complete Animator Node that takes ozz geode.

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
	float range = 50;
	for (int i = 0; i < 50; i++) {
		float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		t->addChild(meshes[3]);
		this->addChild(t);
		transforms.push_back(t);
	}
	for (int i = 0; i < 10; i++) {
		float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 0.75f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		t->addChild(meshes[2]);
		this->addChild(t);
		transforms.push_back(t);
	}

	for (int i = 0; i < 1; i++) {
		float x = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float z = -range + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * range)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 0.75f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		// TODO add animator node in between
		t->addChild(animatedMeshes[0]);
		this->addChild(t);
		transforms.push_back(t);
	}
}

SceneManager::~SceneManager()
{
	for (auto t : transforms) conf_delete(t);
	for (auto g : meshes) conf_delete(g);
	for (auto g : animatedMeshes) conf_delete(g);
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

	vec4 frustumPlanes[6];
	mat4::extractFrustumClipPlanes(Application::projMat * Application::viewMat, frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], frustumPlanes[3], frustumPlanes[4], frustumPlanes[5], true);
	cull(frustumPlanes, SceneManager::enableCulling);
	
	Transform::draw(cmd);
}
