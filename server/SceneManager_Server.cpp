#include "SceneManager_Server.h"

SceneManager_Server::SceneManager_Server() {
	next_player_id = ID_PLAYER_MIN;
	next_base_id = ID_BASE_MIN;
	next_minion_id = ID_MINION_MIN;
	next_tower_id = ID_TOWER_MIN;
	next_resource_id = ID_RESOURCE_MIN;

	//team1 = new Team();
	//team2 = new Team();

	this->populateScene();
}

void SceneManager_Server::processInput(std::string player, PlayerInput in) {
	((Player*)idMap[player])->setMoveAndDir(in);
	//std::cout << "processing input for player " << player << ": " << in.move_x << " " << in.move_z << " " << in.view_y_rot << "\n";
}

bool SceneManager_Server::addPlayer(std::string player_id) {
	if (idMap.find(player_id) == idMap.end()) { //player_id not in map, create a new player
		idMap[player_id] = new Player();
		std::cout << "created new player id: " << player_id << " at " << idMap[player_id] << "\n";

		return true; //return true that a player was added
	}
	else {
		return false; //return false, player was not added
	}
}

void SceneManager_Server::spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y) {
	Entity* ent;
	int id_int;

	switch (spawnType) {
	case BASE_TYPE:
		ent = new Base();
		id_int = next_base_id;
		do {
			next_base_id = next_base_id + 1 > ID_BASE_MAX ? ID_BASE_MIN : next_base_id + 1;
			if (next_base_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of bases reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_base_id)) != idMap.end());
		break;
	case MINION_TYPE:
		ent = new Minion(MINION_HEALTH, MINION_ATTACK);
		id_int = next_minion_id;
		do {
			next_minion_id = next_minion_id + 1 > ID_MINION_MAX ? ID_MINION_MIN : next_minion_id + 1;
			if (next_minion_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of minions reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_minion_id)) != idMap.end());
		break;
	case TOWER_TYPE:
		ent = new Tower(TOWER_HEALTH, TOWER_ATTACK);
		id_int = next_tower_id;
		do {
			next_tower_id = next_tower_id + 1 > ID_TOWER_MAX ? ID_TOWER_MIN : next_tower_id + 1;
			if (next_tower_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of towers reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_tower_id)) != idMap.end());
		break;
	/*case RESOURCE_TYPE:
		ent = new Minion(MINION_HEALTH, MINION_ATTACK); // Resource();
		id_int = next_resource_id;
		do {
			next_resource_id = next_resource_id + 1 > ID_RESOURCE_MAX ? ID_RESOURCE_MIN : next_resource_id + 1;
			if (next_resource_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of resources reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_resource_id)) != idMap.end());
		break;*/
	default:
		ent = new Minion(MINION_HEALTH, MINION_ATTACK);
		id_int = -1;
		std::cout << "spawnEntity encountered unknown entity type\n";
	}

	GameObject::GameObjectData data = { pos_x, pos_z, rot_y };
	ent->setGOData(data);
	idMap[std::to_string(id_int)] = ent;
}

bool SceneManager_Server::checkEntityAlive(std::string id) {
	return idMap.find(id) != idMap.end();
}

void SceneManager_Server::update(float deltaTime) {
	for (std::pair<std::string, Entity*> idEntPair: idMap) {
		if (idEntPair.second->getHealth() <= 0) {	//entity was marked as dead last cycle, 
			delete idEntPair.second;				//delete the object
			idMap.erase(idEntPair.first);			//clear the key out of idMap
		}
		else { //otherwise, update normally
			idEntPair.second->update(deltaTime);
		}
	}
}

int SceneManager_Server::encodeScene(char buf[]) {
	/* buf structure
	 * ID,{EntityData},
	 * ID,{EntityData},
	 * ...
	 * ID,{EntityData},,
	 */

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
		int bytes = idEntPair.second->writeData(buf, i); //write EntityData at i, increase i by number of bytes written
		i += bytes;

		//std::cout << "delimiter 2 at i: " << i << "\n";
		buf[i] = DELIMITER; //write delimiter
		i++;
	}
	
	//std::cout << "closing delimiter at i: " << i << "\n";
	buf[i] = DELIMITER; //append an extra delimiter to indicate the end of the data
	i++;
	return i;
}

void SceneManager_Server::populateScene() { //testing only
	//NOTE: number of objects should not exceed sendbuf capacity
	//sendbufsize >= (NUM_PLAYERS * 20) + (# of minions and towers * 23) + 1
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 12; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Minion* m = new Minion(MINION_HEALTH, MINION_ATTACK);
		m->setMatrix(transform);
		idMap[std::to_string(next_minion_id)] = m;

		std::cout << "created new minion: " << std::to_string(next_minion_id) << " at " << idMap[std::to_string(next_minion_id)] << "\n";

		next_minion_id++;
		if (next_minion_id == ID_MINION_MAX) next_minion_id = ID_MINION_MIN;
	}
	for (int i = 0; i < 5; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Tower* t = new Tower(TOWER_HEALTH, TOWER_ATTACK);
		t->setMatrix(transform);
		idMap[std::to_string(next_tower_id)] = t;
		next_tower_id++;
		if (next_tower_id == ID_TOWER_MAX) next_tower_id = ID_TOWER_MIN;
	}
}

/***** legacy code *****/
/*void SceneManager_Server::resetClocks() {
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
}*/