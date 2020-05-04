#include "SceneManager_Client.h"

namespace {
	const char* playerFile = "female-char.gltf";
	const char* groundFile = "Ground.gltf";
	const char* minionFile = "minion-retry.gltf";
	const char* towerFile = "tower-1-laser.gltf";
}

SceneManager_Client::SceneManager_Client(Renderer* renderer)
{
	// It'd be nice if I could put this in a loop later
	gltfGeodes[ENV_GEODE] = conf_new(GLTFGeode, renderer, groundFile);
	gltfGeodes[PLAYER_GEODE] = conf_new(GLTFGeode, renderer, playerFile);
	//gltfGeodes[BASE_GEODE] = conf_new(GLTFGeode, renderer, baseFile);
	gltfGeodes[MINION_GEODE] = conf_new(GLTFGeode, renderer, minionFile);
	gltfGeodes[TOWER_GEODE] = conf_new(GLTFGeode, renderer, towerFile);
	//gltfGeodes[RESOURCE_GEODE] = conf_new(GLTFGeode, renderer, resourceFile);

	trackedPlayer_ID = "";

	/*
	Transform* t = conf_new(Transform, mat4::identity());
	Transform* t2 = conf_new(Transform, mat4::rotationY(-PI/2));
	t->addChild(t2);
	t2->addChild(gltfGeodes[0]);
	this->addChild(t);
	transforms.push_back(t);
	transforms.push_back(t2);

	t = conf_new(Transform, mat4::identity());
	t->addChild(gltfGeodes[1]);
	this->addChild(t);
	transforms.push_back(t);

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 500; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		t->addChild(gltfGeodes[3]);
		this->addChild(t);
		transforms.push_back(t);
	}
	for (int i = 0; i < 100; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 0.75f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		t = conf_new(Transform, transform);
		t->addChild(gltfGeodes[2]);
		this->addChild(t);
		transforms.push_back(t);
	}
	*/
}

SceneManager_Client::~SceneManager_Client()
{
	for (std::pair<std::string, Transform*> t : transforms) conf_delete(t.second);
	for (std::pair<std::string, GLTFGeode*> g : gltfGeodes) conf_delete(g.second);
}

void SceneManager_Client::createMaterialResources(RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	for (std::pair<std::string, GLTFGeode*> g : gltfGeodes) {
		g.second->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
	}
}

void SceneManager_Client::updateFromClientBuf(char buf[], int bufsize)
{
	if (trackedPlayer_ID == "" && bufsize == 1) {	//first message from server should be 1 byte message
		trackPlayer(std::string(1, buf[0]));			//of the player's connection id
	}
	else {
		std::string id_str = "";
		GameObject::GameObjectData data;
		int health;
		int state = 0; // 0 for reading id, 1 for reading gameobjectdata, 2 for reading health
		for (int i = 0; i < DEFAULT_BUFLEN; i++) {
			if (buf[i] == DELIMITER) { //delimiter encountered, process bytes according to state
				if (state == 2) {
					std::cout << "state 2 delimiter | i: " << i << "\n";
					std::cout << "id: " << id_str << " x: " << data.x << " z: " << data.z << " y: " << data.rot << " health: " << health << "\n";

					if (idMap.find(id_str) == idMap.end()) { //new id encountered, spawn new object

						if (ID_PLAYER_MIN < stoi(id_str) || stoi(id_str) < ID_PLAYER_MAX) {
							idMap[id_str] = conf_new(Player); //TODO use conf_new
							transforms[id_str] = conf_new(Transform, mat4::identity());
							transforms[id_str]->addChild(gltfGeodes[PLAYER_GEODE]);
						}
						else if (ID_BASE_MIN < stoi(id_str) || stoi(id_str) < ID_BASE_MAX) {
							//idMap[id_str] = new Base();
						}
						else if (ID_MINION_MIN < stoi(id_str) || stoi(id_str) < ID_MINION_MAX) {
							//idMap[id_str] = new Minion();
						}
						else if (ID_TOWER_MIN < stoi(id_str) || stoi(id_str) < ID_TOWER_MAX) {
							//idMap[id_str] = new Tower();
						}
						else if (ID_RESOURCE_MIN < stoi(id_str) || stoi(id_str) < ID_RESOURCE_MAX) {
							//idMap[id_str] = new Resource();
						}
					}
					idMap[id_str]->setData(data);
					transforms[id_str]->setMatrix(idMap[id_str]->getMatrix());
					idMap[id_str]->setHealth(health);
					state = 0;
					id_str = "";
				}
				else {
					std::cout << "state " << state << " delimiter | i: " << i << "\n";
					state++;
				}
			}
			else { //non delimiter byte encountered
				if (state == 0) { //read id bytes one by one, appending to id_str
					std::cout << "state: 0 i: " << i << "\n";
					id_str += buf[i];
				}
				else if (state == 1) { //grab all of the gameobjectdata at once, move to one before delimiter and allow i to inc
					std::cout << "state: 1 i: " << i << "\n";
					data = ((GameObject::GameObjectData*)(buf + i))[0];
					i += (sizeof GameObject::GameObjectData) - 1; //-1 to account for i++
				}
				else if (state == 2) { //grab all of the health at once, move to one before delimiter and allow i to inc
					std::cout << "state: 2 i: " << i << "\n";
					health = ((int*)(buf + i))[0];
					i += sizeof(int) - 1; //-1 to account for i++
				}
				else {
					std::cout << "SceneManager_Client updateFromClientBuf state out of sync";
				}
			}

		}
	}
	//GameObject::GameObjectData data = ((GameObject::GameObjectData*)buf)[0];
	//player.setData(data);
	//transforms[0]->setMatrix(player.getMatrix());
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

void SceneManager_Client::setProgram(Geode::GeodeShaderDesc program)
{
	for (std::pair<std::string, GLTFGeode*> g : gltfGeodes) {
		g.second->setProgram(program);
	}
}

void SceneManager_Client::trackPlayer(std::string player_id) {
	std::cout << "tracking player " + player_id + "\n";
	trackedPlayer_ID = player_id;
}

mat4 SceneManager_Client::getPlayerTransformMat() {
	
	if (trackedPlayer_ID != "" && transforms.find(trackedPlayer_ID) != transforms.end()) {
		return transforms[trackedPlayer_ID]->M;
	}
	else return mat4::identity();
}