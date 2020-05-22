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
	red_team = new Team(RED_TEAM);
	blue_team = new Team(BLUE_TEAM);

	this->populateMap();

	//this->populateScene();
	this->testAttacking();
}

void SceneManager_Server::processInput(std::string player, PlayerInput in) {
	((Player*)idMap[player])->processInput(in);
}

bool SceneManager_Server::addPlayer(std::string player_id) {
	if (idMap.find(player_id) == idMap.end()) { //player_id not in map, create a new player
		idMap[player_id] = new Player(player_id, red_team, this); //TODO assign players teams based on lobby choices
		std::cout << "created new player id: " << player_id << " at " << idMap[player_id] << "\n";

		return true; //return true that a player was added
	}
	else {
		return false; //return false, player was not added
	}
}

void SceneManager_Server::spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y, Team* t) {
	Entity* ent;
	int id_int;
	std::string id_str;

	switch (spawnType) {
	case BASE_TYPE:
		id_int = next_base_id;
		id_str = std::to_string(id_int);
		ent = new Base(id_str, t, this);

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
		ent = new Minion(id_str, t, this);
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
		ent = new SuperMinion(id_str, t, this);

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
		ent = new LaserTower(id_str, t, this);

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
		ent = new ClawTower(id_str, t, this);

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
		ent = new Minion(id_str, t, this);
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
	std::vector<std::string> deadIDs;
	std::vector<Entity*> deadEntities;
	for (std::pair<std::string, Entity*> idEntPair : idMap) { //first pass, check for anything that died last cycle
		if (idEntPair.second->getHealth() <= 0) {	//entity reached 0 health last cycle, mark it for deletion
			std::cout << "marking entity id " << idEntPair.first << " addr " << idEntPair.second << " to be deleted\n";
			deadIDs.push_back(idEntPair.first);
			deadEntities.push_back(idEntPair.second);
		}
	}

	for (int i = 0; i < deadIDs.size(); i++) {
		delete deadEntities[i];			//delete the object
		idMap.erase(deadIDs[i]);		//clear the key out of idMap
	}

	for (std::pair<std::string, Entity*> idEntPair : idMap) { //second pass, update as usual
		idEntPair.second->update(deltaTime);
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

void SceneManager_Server::populateMap() {
	std::vector<mapNode*> nodeMap;
	nodeMap.push_back(new mapNode(7.5, 17.5)); //0
	nodeMap.push_back(new mapNode(22.5, 17.5)); //1
	nodeMap[0]->setNextRed(nodeMap[1]);
	nodeMap.push_back(new mapNode(22.5, 42.5)); //2
	nodeMap[1]->setNextRed(nodeMap[2]);
	nodeMap.push_back(new mapNode(22.5, 57.5)); //3
	nodeMap[2]->setNextRed(nodeMap[3]);
	nodeMap.push_back(new mapNode(22.5, 72.5)); //4
	nodeMap[3]->setNextRed(nodeMap[4]);
	nodeMap.push_back(new mapNode(57.5, 72.5)); //5
	nodeMap[4]->setNextRed(nodeMap[5]);
	nodeMap.push_back(new mapNode(57.5, 52.5)); //6
	nodeMap[5]->setNextRed(nodeMap[6]);
	nodeMap.push_back(new mapNode(67.5, 52.5)); //7
	nodeMap[6]->setNextRed(nodeMap[7]);
	nodeMap.push_back(new mapNode(82.5, 52.5)); //8
	nodeMap[7]->setNextRed(nodeMap[8]);
	nodeMap.push_back(new mapNode(107.5, 52.5)); //9
	nodeMap[8]->setNextRed(nodeMap[9]);
	nodeMap.push_back(new mapNode(107.5, 72.5)); //10
	nodeMap[9]->setNextRed(nodeMap[10]);
	nodeMap.push_back(new mapNode(117.5, 72.5)); //11
	nodeMap[10]->setNextRed(nodeMap[11]);
	nodeMap.push_back(new mapNode(32.5, 17.5)); //12
	nodeMap[12]->setNextRed(nodeMap[1]);
	nodeMap.push_back(new mapNode(47.5, 32.5)); //13
	nodeMap.push_back(new mapNode(67.5, 32.5)); //14
	nodeMap[13]->setNextRed(nodeMap[14]);
	nodeMap[14]->setNextRed(nodeMap[7]);
	nodeMap.push_back(new mapNode(67.5, 17.5)); //15
	nodeMap[15]->setNextRed(nodeMap[14]);
	nodeMap.push_back(new mapNode(107.5, 17.5)); //16
	nodeMap[16]->setNextRed(nodeMap[9]);
	nodeMap.push_back(new mapNode(67.5, 7.5)); //17
	nodeMap[17]->setNextRed(nodeMap[15]);
	nodeMap.push_back(new mapNode(17.5, 72.5)); //18
	nodeMap[18]->setNextRed(nodeMap[4]);
	nodeMap.push_back(new mapNode(82.5, 82.5)); //19
	nodeMap.push_back(new mapNode(102.5, 82.5)); //20
	nodeMap[19]->setNextRed(nodeMap[20]);
	nodeMap.push_back(new mapNode(102.5, 72.5)); //21
	nodeMap[20]->setNextRed(nodeMap[21]);
	nodeMap.push_back(new mapNode(47.5, 57.5)); //22
	nodeMap[22]->setNextRed(nodeMap[13]);
	nodeMap[11]->setNextBlue(nodeMap[10]);
	nodeMap[10]->setNextBlue(nodeMap[9]);
	nodeMap[9]->setNextBlue(nodeMap[8]);
	nodeMap[8]->setNextBlue(nodeMap[7]);
	nodeMap[7]->setNextBlue(nodeMap[6]);
	nodeMap[6]->setNextBlue(nodeMap[5]);
	nodeMap[5]->setNextBlue(nodeMap[4]);
	nodeMap[4]->setNextBlue(nodeMap[3]);
	nodeMap[3]->setNextBlue(nodeMap[2]);
	nodeMap[2]->setNextBlue(nodeMap[1]);
	nodeMap[1]->setNextBlue(nodeMap[0]);
	nodeMap[12]->setNextBlue(nodeMap[1]);
	nodeMap.push_back(new mapNode(12.5, 92.5)); //23
	nodeMap[23]->setNextBlue(nodeMap[18]);
	nodeMap[18]->setNextBlue(nodeMap[4]);
	nodeMap.push_back(new mapNode(82.5, 67.5)); //24
	nodeMap[24]->setNextBlue(nodeMap[8]);
	nodeMap[19]->setNextBlue(nodeMap[24]);
	nodeMap[20]->setNextBlue(nodeMap[19]);
	nodeMap[16]->setNextBlue(nodeMap[9]);
	nodeMap.push_back(new mapNode(92.5, 2.5)); //25
	nodeMap.push_back(new mapNode(92.5, 17.5)); //26
	nodeMap[25]->setNextBlue(nodeMap[26]);
	nodeMap[26]->setNextBlue(nodeMap[16]);
	nodeMap.push_back(new mapNode(122.5, 27.5)); //27
	nodeMap.push_back(new mapNode(107.5, 27.5)); //28
	nodeMap[27]->setNextBlue(nodeMap[28]);
	nodeMap[28]->setNextBlue(nodeMap[9]);
	nodeMap.push_back(new mapNode(82.5, 32.5)); //29
	nodeMap[29]->setNextBlue(nodeMap[8]);
	nodeMap.push_back(new mapNode(32.5, 42.5)); //30
	nodeMap[30]->setNextBlue(nodeMap[2]);
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
		Minion* m = new Minion(id_str, red_team, this);
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
		SuperMinion* sm = new SuperMinion(id_str, red_team, this);
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
		LaserTower* l = new LaserTower(id_str, blue_team, this);
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
		ClawTower* c = new ClawTower(id_str, blue_team, this);
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

void SceneManager_Server::testAttacking() {
	std::string id_str;
	mat4 transform;

	/*id_str = std::to_string(next_minion_id);
	next_minion_id++;
	transform = mat4::translation(vec3(10, 0, 5));
	Minion* m1 = new Minion(id_str, red_team, this);
	m1->setMatrix(transform);
	idMap[id_str] = m1;
	std::cout << "created minion at id " << id_str << "\n";

	id_str = std::to_string(next_super_minion_id);
	next_super_minion_id++;
	transform = mat4::translation(vec3(14, 0, 5));
	SuperMinion* sm1 = new SuperMinion(id_str, blue_team, this);
	sm1->setMatrix(transform);
	idMap[id_str] = sm1;
	std::cout << "created super minion at id " << id_str << "\n";*/

	/*id_str = std::to_string(next_super_minion_id);
	next_super_minion_id++;
	transform = mat4::translation(vec3(-5, 0, 0));
	SuperMinion* sm2 = new SuperMinion(id_str, blue_team, this);
	sm2->setMatrix(transform);
	idMap[id_str] = sm2;

	id_str = std::to_string(next_claw_id);
	next_claw_id++;
	transform = mat4::translation(vec3(15, 0, 0));
	ClawTower* c1 = new ClawTower(id_str, red_team, this);
	c1->setMatrix(transform);
	idMap[id_str] = c1;*/

	id_str = std::to_string(next_laser_id);
	next_laser_id++;
	transform = mat4::translation(vec3(30, 0, 42));
	LaserTower* l1 = new LaserTower(id_str, red_team, this);
	l1->setMatrix(transform);
	idMap[id_str] = l1;

	id_str = std::to_string(next_laser_id);
	next_laser_id++;
	transform = mat4::translation(vec3(40, 0, 46));
	LaserTower* l2 = new LaserTower(id_str, blue_team, this);
	l2->setMatrix(transform);
	idMap[id_str] = l2;

	id_str = std::to_string(next_minion_id);
	next_minion_id++;
	transform = mat4::translation(vec3(20, 0, 24));
	Minion* m2 = new Minion(id_str, blue_team, this);
	m2->setMatrix(transform);
	idMap[id_str] = m2;
}

/***** legacy code *****/
/*void SceneManager_Server::resetClocks() {
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
}*/