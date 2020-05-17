#define _CRT_SECURE_NO_WARNINGS

#include "SceneManager_Server.h"
#include <vector>

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

void SceneManager_Server::populateMap() {
	map = new std::vector<mapNode*>();
	(*map)[0] = new mapNode(7.5,17.5);
	(*map)[1] = new mapNode(22.5, 17.5);
	(*map)[0]->setNextRed((*map)[1]);
	(*map)[2] = new mapNode(22.5, 42.5);
	(*map)[1]->setNextRed((*map)[2]);
	(*map)[3] = new mapNode(22.5, 57.5);
	(*map)[2]->setNextRed((*map)[3]);
	(*map)[4] = new mapNode(22.5, 72.5);
	(*map)[3]->setNextRed((*map)[4]);
	(*map)[5] = new mapNode(57.5, 72.5);
	(*map)[4]->setNextRed((*map)[5]);
	(*map)[6] = new mapNode(57.5, 52.5);
	(*map)[5]->setNextRed((*map)[6]);
	(*map)[7] = new mapNode(67.5, 52.5);
	(*map)[6]->setNextRed((*map)[7]);
	(*map)[8] = new mapNode(82.5, 52.5);
	(*map)[7]->setNextRed((*map)[8]);
	(*map)[9] = new mapNode(107.5, 52.5);
	(*map)[8]->setNextRed((*map)[9]);
	(*map)[10] = new mapNode(107.5, 72.5);
	(*map)[9]->setNextRed((*map)[10]);
	(*map)[11] = new mapNode(117.5, 72.5);
	(*map)[10]->setNextRed((*map)[11]);
	(*map)[12] = new mapNode(32.5, 17.5);
	(*map)[12]->setNextRed((*map)[1]);
	(*map)[13] = new mapNode(47.5, 32.5);
	(*map)[14] = new mapNode(67.5, 32.5);
	(*map)[13]->setNextRed((*map)[14]);
	(*map)[14]->setNextRed((*map)[7]);
	(*map)[15] = new mapNode(67.5, 17.5);
	(*map)[15]->setNextRed((*map)[14]);
	(*map)[16] = new mapNode(107.5, 17.5);
	(*map)[16]->setNextRed((*map)[9]);
	(*map)[17] = new mapNode(67.5, 7.5);
	(*map)[17]->setNextRed((*map)[15]);
	(*map)[18] = new mapNode(17.5, 72.5);
	(*map)[18]->setNextRed((*map)[4]);
	(*map)[19] = new mapNode(82.5, 82.5);
	(*map)[20] = new mapNode(102.5, 82.5);
	(*map)[19]->setNextRed((*map)[20]);
	(*map)[21] = new mapNode(102.5, 72.5);
	(*map)[20]->setNextRed((*map)[21]);
	(*map)[22] = new mapNode(47.5, 57.5);
	(*map)[22]->setNextRed((*map)[13]);
	(*map)[11]->setNextBlue((*map)[10]);
	(*map)[10]->setNextBlue((*map)[9]);
	(*map)[9]->setNextBlue((*map)[8]);
	(*map)[8]->setNextBlue((*map)[7]);
	(*map)[7]->setNextBlue((*map)[6]);
	(*map)[6]->setNextBlue((*map)[5]);
	(*map)[5]->setNextBlue((*map)[4]);
	(*map)[4]->setNextBlue((*map)[3]);
	(*map)[3]->setNextBlue((*map)[2]);
	(*map)[2]->setNextBlue((*map)[1]);
	(*map)[1]->setNextBlue((*map)[0]);
	(*map)[12]->setNextBlue((*map)[1]);
	(*map)[23] = new mapNode(12.5, 92.5);
	(*map)[23]->setNextBlue((*map)[18]);
	(*map)[18]->setNextBlue((*map)[4]);
	(*map)[24] = new mapNode(82.5, 67.5);
	(*map)[24]->setNextBlue((*map)[8]);
	(*map)[19]->setNextBlue((*map)[24]);
	(*map)[20]->setNextBlue((*map)[19]);
	(*map)[16]->setNextBlue((*map)[9]);
	(*map)[25] = new mapNode(92.5, 2.5);
	(*map)[26] = new mapNode(92.5, 17.5);
	(*map)[25]->setNextBlue((*map)[26]);
	(*map)[26]->setNextBlue((*map)[16]);
	(*map)[27] = new mapNode(122.5, 27.5);
	(*map)[28] = new mapNode(107.5, 27.5);
	(*map)[27]->setNextBlue((*map)[28]);
	(*map)[28]->setNextBlue((*map)[9]);
	(*map)[29] = new mapNode(82.5, 32.5);
	(*map)[29]->setNextBlue((*map)[8]);
	(*map)[30] = new mapNode(32.5, 42.5);
	(*map)[30]->setNextBlue((*map)[2]);
}