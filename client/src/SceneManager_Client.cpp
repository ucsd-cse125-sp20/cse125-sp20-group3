#include "SceneManager_Client.h"
#include "Application.h"

namespace {
	const char* playerFile ="char-1-female.gltf";
	const char* groundFile = "Ground.gltf";
	const char* minionFile = "minion-retry.gltf";
	const char* towerFile = "tower-1-laser.gltf";

	const char* smallMinionDir = "small-minion";
	const char* smallMinionActions[2] = { "Walk Animation", "Fight Animation" };
	const char* superMinionDir = "super-minion";
	const char* superMinionActions[2] = { "Walking Action", "Fight Animation" };

	int counter = 0;
	int animCounter = 1;
}

bool SceneManager_Client::enableCulling = true;

SceneManager_Client::SceneManager_Client(Renderer* renderer)
{
	// It'd be nice if I could put this in a loop later
	gltfGeodes[ENV_GEODE] = conf_new(GLTFGeode, renderer, groundFile);
	gltfGeodes[PLAYER_GEODE] = conf_new(GLTFGeode, renderer, playerFile);
	//gltfGeodes[BASE_GEODE] = conf_new(GLTFGeode, renderer, baseFile);
	gltfGeodes[MINION_GEODE] = conf_new(GLTFGeode, renderer, minionFile);
	gltfGeodes[TOWER_GEODE] = conf_new(GLTFGeode, renderer, towerFile);
	//gltfGeodes[RESOURCE_GEODE] = conf_new(GLTFGeode, renderer, resourceFile);

    // TODO This is a hard coded animation example. Remove this later
    ozzGeodes["blarf"] = conf_new(OzzGeode, renderer, smallMinionDir);
	((OzzObject*)ozzGeodes["blarf"]->obj)->SetClip(smallMinionActions[0]);
    Transform* t = conf_new(Transform, mat4::translation(vec3(-1, 0, 2)));
    Animator* a = conf_new(Animator, ozzGeodes["blarf"]);
    a->SetClip(smallMinionActions[0]);
    t->addChild(a);
    this->addChild(t);
    transforms["blarf"] = t;
    animators["blarf"] = a;

	// TODO This is a hard coded animation example. Remove this later
	ozzGeodes["blarf2"] = conf_new(OzzGeode, renderer, superMinionDir);
	((OzzObject*)ozzGeodes["blarf2"]->obj)->SetClip(superMinionActions[0]);
	t = conf_new(Transform, mat4::translation(vec3(1, 0, 2)));
	a = conf_new(Animator, ozzGeodes["blarf2"]);
	a->SetClip(superMinionActions[0]);
	t->addChild(a);
	this->addChild(t);
	transforms["blarf2"] = t;
	animators["blarf2"] = a;

	trackedPlayer_ID = "";

	transforms["ground"] = conf_new(Transform, mat4::identity());
	transforms["ground"]->addChild(gltfGeodes[ENV_GEODE]);
	this->addChild(transforms["ground"]);
}

SceneManager_Client::~SceneManager_Client()
{
	for (std::pair<std::string, Entity*> e : idMap) conf_delete(e.second);
	for (std::pair<std::string, Transform*> t : transforms) conf_delete(t.second);
	for (std::pair<std::string, Animator*> t : animators) conf_delete(t.second);
	for (std::pair<std::string, GLTFGeode*> g : gltfGeodes) conf_delete(g.second);
	for (std::pair<std::string, OzzGeode*> g : ozzGeodes) conf_delete(g.second);

	for (Transform* a : player_adjustments) conf_delete(a);
}

void SceneManager_Client::createMaterialResources(SceneManager_Client::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	switch (type) {
	case SceneManager_Client::GeodeType::MESH:
		for (auto ge : gltfGeodes) {
			ge.second->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	case SceneManager_Client::GeodeType::ANIMATED_MESH:
		for (auto ge : ozzGeodes) {
			ge.second->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	}
}

void SceneManager_Client::updateFromClientBuf(std::vector<Client::UpdateData> updateBuf)
{
	//std::cout << "updating from client buf of size " << updateBuf.size() << "\n";
	for (Client::UpdateData data : updateBuf) {
		/*if (data.id_str == "0") {
			std::cout << "x: " << data.ent_data.GO_data.x << " z: " << data.ent_data.GO_data.z << " y: " << data.ent_data.GO_data.rot << "\n";
		}*/
		if (idMap.find(data.id_str) == idMap.end()) { //new id encountered, spawn new object
			int id_int = stoi(data.id_str);
			//std::cout << "id_int: " << id_int << "\n";
			if (ID_PLAYER_MIN <= id_int && id_int <= ID_PLAYER_MAX) {
				std::cout << "creating new player, id: " << data.id_str << "\n";

				idMap[data.id_str] = conf_new(Player);
				transforms[data.id_str] = conf_new(Transform, mat4::identity());
				Transform* adjustment = conf_new(Transform, mat4::rotationY(-PI / 2));

				adjustment->addChild(gltfGeodes[PLAYER_GEODE]);
				transforms[data.id_str]->addChild(adjustment);

				player_adjustments.push_back(adjustment); //save to be deleted upon closing
			}
			else if (ID_BASE_MIN <= id_int && id_int <= ID_BASE_MAX) {
				//idMap[id_str] = new Base();
			}
			else if (ID_MINION_MIN <= id_int && id_int <= ID_MINION_MAX) {
				std::cout << "creating new minion, id: " << data.id_str << "\n";
				idMap[data.id_str] = conf_new(Minion, MINION_HEALTH, MINION_ATTACK);
				transforms[data.id_str] = conf_new(Transform, mat4::identity());
				transforms[data.id_str]->addChild(gltfGeodes[MINION_GEODE]);
			}
			else if (ID_TOWER_MIN <= id_int && id_int <= ID_TOWER_MAX) {
				std::cout << "creating new tower, id: " << data.id_str << "\n";
				idMap[data.id_str] = conf_new(Tower, TOWER_HEALTH, TOWER_ATTACK);
				transforms[data.id_str] = conf_new(Transform, mat4::identity());
				transforms[data.id_str]->addChild(gltfGeodes[TOWER_GEODE]);
			}
			else if (ID_RESOURCE_MIN <= id_int && id_int <= ID_RESOURCE_MAX) {
				//idMap[id_str] = new Resource();
			}
			this->addChild(transforms[data.id_str]);
		}
		
		if (data.ent_data.health <= 0) { //updated health marks entity as dead
			//play death animation
			conf_delete(idMap[data.id_str]);
			idMap[data.id_str] = NULL;
			conf_delete(transforms[data.id_str]);
			transforms[data.id_str] = NULL;
		}
		else { //otherwise, update the entity's data and transform
			idMap[data.id_str]->setEntData(data.ent_data);
			transforms[data.id_str]->setMatrix(idMap[data.id_str]->getMatrix());
		}
	}
}

void SceneManager_Client::updateFromInputBuf(float deltaTime)
{
	char recvbuf[DEFAULT_BUFLEN];
	Input::EncodeToBuf(recvbuf);
	PlayerInput input = ((PlayerInput*)recvbuf)[0];
	//printf("%d %d %f\n", input.move_x, input.move_z, input.view_y_rot);
	//player.setMoveAndDir(input);
	//player.update();
	//transforms[0]->setMatrix(player.getMatrix());
}

void SceneManager_Client::setBuffer(SceneManager_Client::SceneBuffer type, Buffer** buffer)
{
	switch (type) {
	case SceneManager_Client::SceneBuffer::INSTANCE:
		instanceBuffer = buffer;
		break;
	case SceneManager_Client::SceneBuffer::BONE:
		boneBuffer = buffer;
		break;
	}
}

void SceneManager_Client::setProgram(SceneManager_Client::GeodeType type, Geode::GeodeShaderDesc program)
{
	switch (type) {
	case SceneManager_Client::GeodeType::MESH:
		for (auto ge : gltfGeodes) {
			ge.second->setProgram(program);
		}
		break;
	case SceneManager_Client::GeodeType::ANIMATED_MESH:
		for (auto ge : ozzGeodes) {
			ge.second->setProgram(program);
		}
		break;
	}
}

void SceneManager_Client::trackPlayer(std::string player_id) {
	std::cout << "tracking player " + player_id + "\n";
	trackedPlayer_ID = player_id;
}

mat4 SceneManager_Client::getPlayerTransformMat() {
	
	if (trackedPlayer_ID != "" && transforms.find(trackedPlayer_ID) != transforms.end()) {
		return transforms[trackedPlayer_ID]->getMatrix();
	}
	else return mat4::identity();
}

void SceneManager_Client::update(float deltaTime)
{
	if (counter % 100 == 0) animators["blarf"]->SetClip(smallMinionActions[animCounter]);
	if (counter++ % 100 == 0) animators["blarf2"]->SetClip(superMinionActions[animCounter = (animCounter + 1) % 2]);
	Transform::update(deltaTime);
}

void SceneManager_Client::draw(Cmd* cmd)
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
	cull(frustumPlanes, SceneManager_Client::enableCulling);
	
	// Draw Graph
	Transform::draw(cmd);
}
/***** legacy code *****/
/*if (trackedPlayer_ID == "" && bufsize == 1) {	//first message from server should be 1 byte message
	trackPlayer(std::string(1, buf[0]));		//of the player's connection id
}
else {
	std::string id_str = "";
	GameObject::GameObjectData data;
	int health;
	int state = 0; // 0 for reading id, 1 for reading gameobjectdata, 2 for reading health, 3 for checking closing delimiter
	for (int i = 0; i < DEFAULT_BUFLEN; i++) {
		//std::cout << "i: " << i << "\n";
		if (state == 0) {
			if (buf[i] == DELIMITER) {
				state++; //end of state found, advance to next state
				//std::cout << "id_str: " + id_str + "\n";
			}
			else { //read id bytes one by one, appending to id_str
				id_str += buf[i];
			}

		}
		else if (state == 1) {
			int move_x = ((int*)(buf + i))[0];
			int move_z = ((int*)(buf + i + 4))[0];
			float rot_y = ((float*)(buf + i + 8))[0];
			//std::cout << "move_x: " << move_x << " move_z: " << move_z << " rot_y: " << rot_y << "\n";
			data = ((GameObject::GameObjectData*)(buf + i))[0];
			i += (sizeof GameObject::GameObjectData); //advance i to where delimiter should be

			if (buf[i] == DELIMITER) {
				//std::cout << "state 1 delimiter at i: " << i << "\n";
				state++; //end of state found, advance to next state
			}
			else {
				//std::cout << "state 1 delimiter expected but not found, i: " << i << "\n";
			}
		}
		else if (state == 2) {
			health = ((int*)(buf + i))[0];
			i += sizeof(int); //advance i to where delimiter should be

			if (buf[i] == DELIMITER) { //end of data line found, write to map, advance to possible final state
				//std::cout << "id: " << id_str << " x: " << data.x << " z: " << data.z << " y: " << data.rot << " health: " << health << "\n";
				//std::cout << "state 2 delimiter at i: " << i << "\n";


			}
			else {
				std::cout << "state 2 delimiter not found when expected, i: " << i << "\n";
			}
		}
		else if (state == 3) {
			if (buf[i] == DELIMITER) {
				std::cout << "closing delimiter found at i: " << i << "\n";
				break;
			}
			else {
				std::cout << "non-closing byte in state 3 at i: " << i << ", rechecking\n";
				state = 0; //reset state and read this byte again
				i--;
			}
		}
		else {
			std::cout << "SceneManager_Client updateFromClientBuf state out of sync";
		}
	}
}*/