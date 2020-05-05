#include "SceneManager_Server.h"

SceneManager_Server::SceneManager_Server() {
	next_player_id = ID_PLAYER_MIN;
	next_base_id = ID_BASE_MIN;
	next_minion_id = ID_MINION_MIN;
	next_tower_id = ID_TOWER_MIN;
	next_resource_id = ID_RESOURCE_MIN;

	//team1 = new Team();
	//team2 = new Team();
}

void SceneManager_Server::processInput(std::string player, PlayerInput in) {
	((Player*)idMap[player])->setMoveAndDir(in);
	//std::cout << "processing input for player " << player << ": " << in.move_x << " " << in.move_z << " " << in.view_y_rot << "\n";
}

bool SceneManager_Server::addPlayer(std::string player_id) {
	std::cout << "inside addPlayer\n";
	if (idMap.find(player_id) == idMap.end()) { //player_id not in map, create a new player
		//if (idMap.find(player_id) == idMap.end()) std::cout << "this worked?\n";
		idMap[player_id] = new Player();
		std::cout << "created new player id: " << player_id << " at " << idMap[player_id] << "\n";
		//if (idMap.find(player_id) != idMap.end()) std::cout << "and this worked too???\n";

		this->populateScene();
		return true; //return true that a player was added
	}
	else {
		std::cout << player_id << " found in idMap, not adding\n";
		return false; //return false, player was not added
	}
}

void SceneManager_Server::spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y) {
	/*Entity* ent;
	int id_int;

	switch (spawnType) { //WARN: possible id wrap issues
	case BASE_TYPE:
		ent = new Base();
		id_int = next_base_id;
		next_base_id++;
		if (next_base_id == ID_BASE_MAX) next_base_id = ID_BASE_MIN;
		break;
	case MINION_TYPE:
		ent = new Minion();
		id_int = next_minion_id;
		next_minion_id++;
		if (next_minion_id == ID_MINION_MAX) next_minion_id = ID_MINION_MIN;
		break;
	case TOWER_TYPE:
		ent = new Tower();
		id_int = next_tower_id;
		next_tower_id++;
		if (next_tower_id == ID_TOWER_MAX) next_tower_id = ID_TOWER_MIN;
		break;
	case RESOURCE_TYPE:
		ent = new Resource();
		id_int = next_resource_id;
		next_resource_id++;
		if (next_resource_id == ID_RESOURCE_MAX) next_resource_id = ID_RESOURCE_MIN;
		break;
	default:
		ent = new Minion();
		id_int = -1;
		std::cout << "spawnEntity encountered unknown entity type\n";
	}

	GameObject::GameObjectData data = { pos_x, pos_z, rot_y };
	ent->setData(data);
	idMap[std::to_string(id_int)] = ent;*/
}

void SceneManager_Server::resetClocks() {
	std::cout << "resetting clocks\n";
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
	std::cout << "reset clocks ok\n";
}

void SceneManager_Server::update() {
	std::cout << "update\n";
	for (std::pair<std::string, Entity*> idEntPair: idMap) {
		idEntPair.second->update();
	}
	std::cout << "update ok\n";
}

int SceneManager_Server::encodeScene(char buf[]) {
	/* buf structure
	 * ID,{GameObjectData},health,
	 * ID,{GameObjectData},health,
	 * ...
	 * ID,{GameObjectData},health,,
	 */
	//std::cout << "encodeScene 1\n";
	//this->addPlayer("0");

	int i = 0;
	for (std::pair<std::string, Entity*> idEntPair : idMap) { //iterate through all entities in scene
		//std::cout << "id: " << idEntPair.first << "\n";

		//std::cout << "writing id at i: " << i << "\n";
		strncpy(buf + i, idEntPair.first.c_str(), idEntPair.first.length()); //write id bytes without null term
		i += idEntPair.first.length();

		//std::cout << "delimiter 1 at i: " << i << "\n";
		buf[i] = DELIMITER; //write delimiter
		i++;

		//std::cout << "writing data at i: " << i << "\n";
		int bytes = idEntPair.second->writeData(buf, i); //write GameObjectData at i, increase i by number of bytes written
		i += bytes;
		//std::cout << " x: " << idEntPair.second->model[3][0] << " z: " << idEntPair.second->model[3][2];
		//std::cout << " y: " << atan2(-idEntPair.second->model[2][2], -idEntPair.second->model[2][0]);

		//std::cout << "delimiter 2 at i: " << i << "\n";
		buf[i] = DELIMITER; //write delimiter
		i++;

		//std::cout << "writing health at i: " << i << "\n";
		((int*)buf)[i] = idEntPair.second->getHealth(); //write entity's health
		//std::cout << " health: " << idEntPair.second->getHealth() << "\n";
		i += sizeof(int);

		//std::cout << "delimiter 3 at i: " << i << "\n";
		buf[i] = DELIMITER; //write delimiter
		i++;
		//std::cout << "encodeScene 2\n";
		//this->addPlayer("0");
	}
	//std::cout << "closing delimiter at i: " << i << "\n";
	buf[i] = DELIMITER; //append an extra delimiter to indicate the end of the data
	i++;
	std::cout << "encodeScene 3\n";
	this->addPlayer("0");
	//std::cout << "returning i: " << i << "\n";
	return i;
}

void SceneManager_Server::populateScene() {
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 12; i++) {
		float x = (rand() % 41) - 20;//-100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = (rand() % 41) - 20;//-100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = (rand() * PI) / RAND_MAX;//static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 1.0f + ((rand() * 1.0f) / RAND_MAX);//static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Minion* m = new Minion(MINION_HEALTH, MINION_ATTACK);
		m->setMatrix(transform);
		idMap[std::to_string(next_minion_id)] = m;
		next_minion_id++;
		if (next_minion_id == ID_MINION_MAX) next_minion_id = ID_MINION_MIN;
	}
	for (int i = 0; i < 0; i++) {
		float x = (rand() % 41) - 20;//-100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = (rand() % 41) - 20;//-100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = (rand() * PI) / RAND_MAX;//static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = 1.0f + ((rand() * 1.0f) / RAND_MAX);//static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Tower* t = new Tower(TOWER_HEALTH, TOWER_ATTACK);
		t->setMatrix(transform);
		idMap[std::to_string(next_tower_id)] = t;
		next_tower_id++;
		if (next_tower_id == ID_TOWER_MAX) next_tower_id = ID_TOWER_MIN;
	}
	/*GameObject::GameObjectData data = { 10.f, 10.f, 5.f };
	GameObject::GameObjectData data2 = { -10.f, -10.f, 10.f };
	std::cout << "creating minion at " << std::to_string(ID_MINION_MIN) << "\n";
	Minion* m = new Minion(MINION_HEALTH, MINION_ATTACK);
	m->setData(data);
	idMap[std::to_string(ID_MINION_MIN)] = m;
	std::cout << "creating tower at " << std::to_string(ID_TOWER_MIN) << "\n";
	Tower* t = new Tower(TOWER_HEALTH, TOWER_ATTACK);
	t->setData(data2);
	idMap[std::to_string(ID_TOWER_MIN)] = t;
	std::cout << "creation ok\n";*/
}