#define _CRT_SECURE_NO_WARNINGS

#include "SceneManager_Server.h"

SceneManager_Server::SceneManager_Server() :
	next_player_id(ID_PLAYER_MIN),
	next_base_id(ID_BASE_MIN),
	next_minion_id(ID_MINION_MIN),
	next_super_minion_id(ID_SUPER_MINION_MIN),
	next_laser_id(ID_LASER_MIN),
	next_claw_id(ID_CLAW_MIN),
	next_dumpster_id(ID_DUMPSTER_MIN),
	next_recycling_bin_id(ID_RECYCLING_BIN_MIN)
{

	//team1 = new Team();
	//team2 = new Team();

	this->populateScene();
}

void SceneManager_Server::processInput(std::string player, PlayerInput in) {
	((Player*)idMap[player])->processInput(in);
}

bool SceneManager_Server::addPlayer(std::string player_id) {
	if (idMap.find(player_id) == idMap.end()) { //player_id not in map, create a new player
		idMap[player_id] = new Player(player_id, this);
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
	std::string id_str;

	switch (spawnType) {
	case BASE_TYPE:
		id_int = next_base_id;
		id_str = std::to_string(id_int);
		ent = new Base(id_str, this);

		do {
			next_base_id = next_base_id + 1 > ID_BASE_MAX ? ID_BASE_MIN : next_base_id + 1;
			if (next_base_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of bases reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_base_id)) != idMap.end());
		break;
	case MINION_TYPE:
		
		id_int = next_minion_id;
		id_str = std::to_string(id_int);
		ent = new Minion(id_str, this);
		do {
			next_minion_id = next_minion_id + 1 > ID_MINION_MAX ? ID_MINION_MIN : next_minion_id + 1;
			if (next_minion_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of minions reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_minion_id)) != idMap.end());
		break;
	case SUPER_MINION_TYPE:
		id_int = next_super_minion_id;
		id_str = std::to_string(id_int);
		ent = new SuperMinion(id_str, this);

		do {
			next_super_minion_id = next_super_minion_id + 1 > ID_SUPER_MINION_MAX ? ID_SUPER_MINION_MIN : next_super_minion_id + 1;
			if (next_super_minion_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of super minions reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_super_minion_id)) != idMap.end());
		break;
	case LASER_TYPE:
		id_int = next_laser_id;
		id_str = std::to_string(id_int);
		ent = new LaserTower(id_str, this);

		do {
			next_laser_id = next_laser_id + 1 > ID_LASER_MAX ? ID_LASER_MIN : next_laser_id + 1;
			if (next_laser_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of laser towers reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_laser_id)) != idMap.end());
		break;
	case CLAW_TYPE:
		id_int = next_claw_id;
		id_str = std::to_string(id_int);
		ent = new ClawTower(id_str, this);

		do {
			next_claw_id = next_claw_id + 1 > ID_CLAW_MAX ? ID_CLAW_MIN : next_claw_id + 1;
			if (next_claw_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of claw machines reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_claw_id)) != idMap.end());
		break;
	case DUMPSTER_TYPE:
		id_int = next_dumpster_id;
		id_str = std::to_string(id_int);
		ent = new Resource(DUMPSTER_TYPE, id_str, this);

		do {
			next_dumpster_id = next_dumpster_id + 1 > ID_DUMPSTER_MAX ? ID_DUMPSTER_MIN : next_dumpster_id + 1;
			if (next_dumpster_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of dumpsters reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_dumpster_id)) != idMap.end());
		break;
	case RECYCLING_BIN_TYPE:
		id_int = next_recycling_bin_id;
		id_str = std::to_string(id_int);
		ent = new Resource(RECYCLING_BIN_TYPE, id_str, this);

		do {
			next_recycling_bin_id = next_recycling_bin_id + 1 > ID_RECYCLING_BIN_MAX ? ID_RECYCLING_BIN_MIN : next_recycling_bin_id + 1;
			if (next_recycling_bin_id == id_int) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of recycling bins reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(std::to_string(next_recycling_bin_id)) != idMap.end());
		break;
	default:
		id_int = -1;
		id_str = std::to_string(id_int);
		ent = new Minion(id_str, this);
		std::cout << "spawnEntity encountered unknown entity type " << spawnType << "\n";
	}

	GameObject::GameObjectData data = { pos_x, pos_z, rot_y };
	ent->setGOData(data);
	idMap[id_str] = ent;
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

			// THIS IS FOR OBJECT DETECTION DEBUGGING. REMOVE LATER
			//ObjectDetection::updateObject(idMap["0"]);
			//GameObject* nearest = ObjectDetection::getNearestObject(idMap["0"], DETECTION_FLAG_TOWER, 10);
			//if (nearest) {
			//	printf("nearest tower at (%f,%f)\n", nearest->getData().x, nearest->getData().z);
			//}
		}
	}
}

int SceneManager_Server::encodeState(char buf[], int start_index) {
	int i = start_index;

	/*((int*)(buf + i))[0] = team1.getPlasticCount();
	((int*)(buf + i))[1] = team1.getMetalCount();
	((int*)(buf + i))[2] = team2.getPlasticCount();
	((int*)(buf + i))[3] = team2.getMetalCount();
	i += 4 * sizeof(int);

	buf[i] = DELIMITER;
	i++;*/

	/*for (int p = ID_PLAYER_MIN; p < ID_PLAYER_MIN + NUM_PLAYERS; p++) {
		std::string id_str = std::to_string(p);

		strncpy(buf + i, id_str.c_str(), id_str.length()); //copy player id into buf
		i += id_str.length();

		buf[i] = DELIMITER;
		i++;

		std::pair<float, float> p_velocities = ((Player*)idMap[std::to_string(p)])->getVelocities();
		((float*)(buf + i))[0] = p_velocities.first; //write velocity_x
		((float*)(buf + i))[1] = p_velocities.second; //write velocity_z
		i += 2 * sizeof(float);

		buf[i] = DELIMITER;
		i++;
	}

	buf[i] = DELIMITER;
	i++;*/

	return i;
}

int SceneManager_Server::encodeScene(char buf[], int start_index) {
	/* buf structure
	 * ID,{EntityData},
	 * ID,{EntityData},
	 * ...
	 * ID,{EntityData},,
	 */

	int i = start_index;
	for (std::pair<std::string, Entity*> idEntPair : idMap) { //iterate through all entities in scene
		//std::cout << "id: " << idEntPair.first << "\n";

		//std::cout << "writing id at i: " << i << "\n";
		strncpy(buf + i, idEntPair.first.c_str(), idEntPair.first.length()); //write id bytes without null term
		i += (int)idEntPair.first.length();

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
	//spawn standard minions
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_minion_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Minion* m = new Minion(id_str, this);
		m->setMatrix(transform);
		idMap[id_str] = m;

		std::cout << "created new minion: " << id_str << " at " << idMap[id_str] << "\n";

		next_minion_id++;
		if (next_minion_id == ID_MINION_MAX) next_minion_id = ID_MINION_MIN;
	}
	//spawn super minions
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_super_minion_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		SuperMinion* sm = new SuperMinion(id_str, this);
		sm->setMatrix(transform);
		idMap[id_str] = sm;

		std::cout << "created new super minion: " << id_str << " at " << idMap[id_str] << "\n";

		next_super_minion_id++;
		if (next_super_minion_id == ID_SUPER_MINION_MAX) next_super_minion_id = ID_SUPER_MINION_MIN;
	}
	//spawn laser towers
	for (int i = 0; i < 4; i++) { 
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_laser_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		LaserTower* l = new LaserTower(id_str, this);
		l->setMatrix(transform);
		idMap[id_str] = l;

		std::cout << "created new laser tower: " << id_str << " at " << idMap[id_str] << "\n";

		next_laser_id++;
		if (next_laser_id == ID_LASER_MAX) next_laser_id = ID_LASER_MIN;
	}
	//spawn claw towers
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_claw_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		ClawTower* c = new ClawTower(id_str, this);
		c->setMatrix(transform);
		idMap[id_str] = c;

		std::cout << "created new claw tower: " << id_str << " at " << idMap[id_str] << "\n";

		next_claw_id++;
		if (next_claw_id == ID_CLAW_MAX) next_claw_id = ID_CLAW_MIN;
	}
	//spawn dumpsters
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_dumpster_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Resource* d = new Resource(DUMPSTER_TYPE, id_str, this);
		d->setMatrix(transform);
		idMap[id_str] = d;

		std::cout << "created new dumpster: " << id_str << " at " << idMap[id_str] << "\n";

		next_dumpster_id++;
		if (next_dumpster_id == ID_DUMPSTER_MAX) next_dumpster_id = ID_DUMPSTER_MIN;
	}
	//spawn recycling bins
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		std::string id_str = std::to_string(next_recycling_bin_id);
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Resource* r = new Resource(RECYCLING_BIN_TYPE, id_str, this);
		r->setMatrix(transform);
		idMap[id_str] = r;

		std::cout << "created new recycling bin: " << id_str << " at " << idMap[id_str] << "\n";

		next_recycling_bin_id++;
		if (next_recycling_bin_id == ID_RECYCLING_BIN_MAX) next_recycling_bin_id = ID_RECYCLING_BIN_MIN;
	}
}

/***** legacy code *****/
/*void SceneManager_Server::resetClocks() {
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
}*/