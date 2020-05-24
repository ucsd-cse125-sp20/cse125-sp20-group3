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

void SceneManager_Server::processInput(int player_id, PlayerInput in) {
	((Player*)idMap[player_id])->processInput(in);
}

bool SceneManager_Server::addPlayer(int player_id) {
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
	int id;

	switch (spawnType) {
	case BASE_TYPE:
		id = next_base_id;
		ent = new Base(id, t, this);

		do {
			next_base_id = next_base_id + 1 > ID_BASE_MAX ? ID_BASE_MIN : next_base_id + 1;
			if (next_base_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of bases reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_base_id) != idMap.end());
		break;
	case MINION_TYPE:
		id = next_minion_id;
		ent = new Minion(id, t, this);

		do {
			next_minion_id = next_minion_id + 1 > ID_MINION_MAX ? ID_MINION_MIN : next_minion_id + 1;
			if (next_minion_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of minions reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_minion_id) != idMap.end());
		break;
	case SUPER_MINION_TYPE:
		id = next_super_minion_id;
		ent = new SuperMinion(id, t, this);

		do {
			next_super_minion_id = next_super_minion_id + 1 > ID_SUPER_MINION_MAX ? ID_SUPER_MINION_MIN : next_super_minion_id + 1;
			if (next_super_minion_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of super minions reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_super_minion_id) != idMap.end());
		break;
	case LASER_TYPE:
		id = next_laser_id;
		ent = new LaserTower(id, t, this);

		do {
			next_laser_id = next_laser_id + 1 > ID_LASER_MAX ? ID_LASER_MIN : next_laser_id + 1;
			if (next_laser_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of laser towers reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_laser_id) != idMap.end());
		break;
	case CLAW_TYPE:
		id = next_claw_id;
		ent = new ClawTower(id, t, this);

		do {
			next_claw_id = next_claw_id + 1 > ID_CLAW_MAX ? ID_CLAW_MIN : next_claw_id + 1;
			if (next_claw_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of claw machines reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_claw_id) != idMap.end());
		break;
	case DUMPSTER_TYPE:
		id = next_dumpster_id;
		ent = new Resource(DUMPSTER_TYPE, id, this);

		do {
			next_dumpster_id = next_dumpster_id + 1 > ID_DUMPSTER_MAX ? ID_DUMPSTER_MIN : next_dumpster_id + 1;
			if (next_dumpster_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of dumpsters reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_dumpster_id) != idMap.end());
		break;
	case RECYCLING_BIN_TYPE:
		id = next_recycling_bin_id;
		ent = new Resource(RECYCLING_BIN_TYPE, id, this);

		do {
			next_recycling_bin_id = next_recycling_bin_id + 1 > ID_RECYCLING_BIN_MAX ? ID_RECYCLING_BIN_MIN : next_recycling_bin_id + 1;
			if (next_recycling_bin_id == id) { //wrapped all the way around, all id's taken
				std::cout << "maximum number of recycling bins reached, consider expanding id ranges\n";
				break;
			}
		} while (idMap.find(next_recycling_bin_id) != idMap.end());
		break;
	default:
		id = -1;
		ent = new Minion(id, t, this);
		std::cout << "spawnEntity encountered unknown entity type " << spawnType << "\n";
	}

	GameObject::GameObjectData data = { pos_x, pos_z, rot_y };
	ent->setGOData(data);
	idMap[id] = ent;
}

bool SceneManager_Server::checkEntityAlive(int id) {
	return idMap.find(id) != idMap.end();
}

void SceneManager_Server::update(float deltaTime) {
	std::vector<int> deadIDs;
	std::vector<Entity*> deadEntities;
	for (std::pair<int, Entity*> idEntPair : idMap) { //first pass, check for anything that died last cycle
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

	for (std::pair<int, Entity*> idEntPair : idMap) { //second pass, update as usual
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
		int id_str = std::to_string(p);

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
	for (std::pair<int, Entity*> idEntPair : idMap) { //iterate through all entities in scene
		//std::cout << "id: " << idEntPair.first << "\n";

		//std::cout << "writing id at i: " << i << "\n";
		((int*)(buf + i))[0] = idEntPair.first;
		i += sizeof(int);

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

void SceneManager_Server::populateWallLocation(){
	std::vector<wallNode*> wallNodeMap;
	this->wallNodeMap = &wallNodeMap;
	wallNodeMap.push_back(new wallNode(0,3)); 
	wallNodeMap.push_back(new wallNode(0,4)); 
	wallNodeMap.push_back(new wallNode(0,5));
	wallNodeMap.push_back(new wallNode(2,3)); 
	wallNodeMap.push_back(new wallNode(3,3));
	wallNodeMap.push_back(new wallNode(4,5));
	wallNodeMap.push_back(new wallNode(6,2));
	wallNodeMap.push_back(new wallNode(7,1));
	wallNodeMap.push_back(new wallNode(8,1));
	wallNodeMap.push_back(new wallNode(9,1));
	wallNodeMap.push_back(new wallNode(10,1));
	wallNodeMap.push_back(new wallNode(11,1));
	wallNodeMap.push_back(new wallNode(12,1));
	wallNodeMap.push_back(new wallNode(13,1));
	wallNodeMap.push_back(new wallNode(14,0));
	wallNodeMap.push_back(new wallNode(15,0));
	wallNodeMap.push_back(new wallNode(16,0));
	wallNodeMap.push_back(new wallNode(17,0));
	wallNodeMap.push_back(new wallNode(18,0));
	wallNodeMap.push_back(new wallNode(9,3));
	wallNodeMap.push_back(new wallNode(10,3));
	wallNodeMap.push_back(new wallNode(11,3));
	wallNodeMap.push_back(new wallNode(12,3));
	wallNodeMap.push_back(new wallNode(17,2));
	wallNodeMap.push_back(new wallNode(18,2));
	wallNodeMap.push_back(new wallNode(22,1));
	wallNodeMap.push_back(new wallNode(23,1));
	wallNodeMap.push_back(new wallNode(24,1));
	wallNodeMap.push_back(new wallNode(23,5));
	wallNodeMap.push_back(new wallNode(24,5));
	wallNodeMap.push_back(new wallNode(21,3));
	wallNodeMap.push_back(new wallNode(22,3));
	wallNodeMap.push_back(new wallNode(23,4));
	wallNodeMap.push_back(new wallNode(24,2));
	wallNodeMap.push_back(new wallNode(16,3));
	wallNodeMap.push_back(new wallNode(17,3));
	wallNodeMap.push_back(new wallNode(4,6));
	wallNodeMap.push_back(new wallNode(4,7));
	wallNodeMap.push_back(new wallNode(4,8));
	wallNodeMap.push_back(new wallNode(3,8));
	wallNodeMap.push_back(new wallNode(2,8));
	wallNodeMap.push_back(new wallNode(1,9));
	wallNodeMap.push_back(new wallNode(6,6));
	wallNodeMap.push_back(new wallNode(6,7));
	wallNodeMap.push_back(new wallNode(8,4));
	wallNodeMap.push_back(new wallNode(8,6));
	wallNodeMap.push_back(new wallNode(8,7));
	wallNodeMap.push_back(new wallNode(8,8));
	wallNodeMap.push_back(new wallNode(9,7));
	wallNodeMap.push_back(new wallNode(9,8));
	wallNodeMap.push_back(new wallNode(11,6));
	wallNodeMap.push_back(new wallNode(12,6));
	wallNodeMap.push_back(new wallNode(13,4));
	wallNodeMap.push_back(new wallNode(13,5));
	wallNodeMap.push_back(new wallNode(15,5));
	wallNodeMap.push_back(new wallNode(16,5));
	wallNodeMap.push_back(new wallNode(17,5));
	wallNodeMap.push_back(new wallNode(18,5));
	wallNodeMap.push_back(new wallNode(20,5));
	wallNodeMap.push_back(new wallNode(15,6));
	wallNodeMap.push_back(new wallNode(16,6));
	wallNodeMap.push_back(new wallNode(17,6));
	wallNodeMap.push_back(new wallNode(18,8));
	wallNodeMap.push_back(new wallNode(18,9));
	wallNodeMap.push_back(new wallNode(18,10));
	wallNodeMap.push_back(new wallNode(19,10));
	wallNodeMap.push_back(new wallNode(21,10));
	wallNodeMap.push_back(new wallNode(18,7));
	wallNodeMap.push_back(new wallNode(21,6));
	wallNodeMap.push_back(new wallNode(21,7));
	wallNodeMap.push_back(new wallNode(21,8));
	wallNodeMap.push_back(new wallNode(21,9));
	wallNodeMap.push_back(new wallNode(23,7));
	wallNodeMap.push_back(new wallNode(24,7));
	wallNodeMap.push_back(new wallNode(23,11));
	wallNodeMap.push_back(new wallNode(23,12));
	wallNodeMap.push_back(new wallNode(25,15));
	wallNodeMap.push_back(new wallNode(25,16));
	wallNodeMap.push_back(new wallNode(1,11));
	wallNodeMap.push_back(new wallNode(1,12));
	wallNodeMap.push_back(new wallNode(2,14));
	wallNodeMap.push_back(new wallNode(2,15));
	wallNodeMap.push_back(new wallNode(2,16));
	wallNodeMap.push_back(new wallNode(2,19));
	wallNodeMap.push_back(new wallNode(3,11));
	wallNodeMap.push_back(new wallNode(4,11));
	wallNodeMap.push_back(new wallNode(4,12));
	wallNodeMap.push_back(new wallNode(4,13));
	wallNodeMap.push_back(new wallNode(4,14));
	wallNodeMap.push_back(new wallNode(5,18));
	wallNodeMap.push_back(new wallNode(7,10));
	wallNodeMap.push_back(new wallNode(7,11));
	wallNodeMap.push_back(new wallNode(7,13));
	wallNodeMap.push_back(new wallNode(7,14));
	wallNodeMap.push_back(new wallNode(7,16));
	wallNodeMap.push_back(new wallNode(8,10));
	wallNodeMap.push_back(new wallNode(8,11));
	wallNodeMap.push_back(new wallNode(8,14));
	wallNodeMap.push_back(new wallNode(8,16));
	wallNodeMap.push_back(new wallNode(8,18));
	wallNodeMap.push_back(new wallNode(9,11));
	wallNodeMap.push_back(new wallNode(9,14));
	wallNodeMap.push_back(new wallNode(9,16));
	wallNodeMap.push_back(new wallNode(9,18));
	wallNodeMap.push_back(new wallNode(10,13));
	wallNodeMap.push_back(new wallNode(10,14));
	wallNodeMap.push_back(new wallNode(10,16));
	wallNodeMap.push_back(new wallNode(10,18));
	wallNodeMap.push_back(new wallNode(11,10));
	wallNodeMap.push_back(new wallNode(11,11));
	wallNodeMap.push_back(new wallNode(11,12));
	wallNodeMap.push_back(new wallNode(11,13));
	wallNodeMap.push_back(new wallNode(11,16));
	wallNodeMap.push_back(new wallNode(11,17));
	wallNodeMap.push_back(new wallNode(11,18));
	wallNodeMap.push_back(new wallNode(12,8));
	wallNodeMap.push_back(new wallNode(13,8));
	wallNodeMap.push_back(new wallNode(13,9));
	wallNodeMap.push_back(new wallNode(13,10));
	wallNodeMap.push_back(new wallNode(13,12));
	wallNodeMap.push_back(new wallNode(13,13));
	wallNodeMap.push_back(new wallNode(13,14));
	wallNodeMap.push_back(new wallNode(13,15));
	wallNodeMap.push_back(new wallNode(13,16));
	wallNodeMap.push_back(new wallNode(14,14));
	wallNodeMap.push_back(new wallNode(14,15));
	wallNodeMap.push_back(new wallNode(14,16));
	wallNodeMap.push_back(new wallNode(14,17));
	wallNodeMap.push_back(new wallNode(14,18));
	wallNodeMap.push_back(new wallNode(15,8));
	wallNodeMap.push_back(new wallNode(15,9));
	wallNodeMap.push_back(new wallNode(15,12));
	wallNodeMap.push_back(new wallNode(15,13));
	wallNodeMap.push_back(new wallNode(16,9));
	wallNodeMap.push_back(new wallNode(16,10));
	wallNodeMap.push_back(new wallNode(16,13));
	wallNodeMap.push_back(new wallNode(16,16));
	wallNodeMap.push_back(new wallNode(16,18));
	wallNodeMap.push_back(new wallNode(16,19));
	wallNodeMap.push_back(new wallNode(17,18));
	wallNodeMap.push_back(new wallNode(18,14));
	wallNodeMap.push_back(new wallNode(18,15));
	wallNodeMap.push_back(new wallNode(19,14));
	wallNodeMap.push_back(new wallNode(19,12));
	wallNodeMap.push_back(new wallNode(20,12));
	wallNodeMap.push_back(new wallNode(21,12));
	wallNodeMap.push_back(new wallNode(21,13));
	wallNodeMap.push_back(new wallNode(21,14));
	wallNodeMap.push_back(new wallNode(22,16));
	wallNodeMap.push_back(new wallNode(22,17));
	wallNodeMap.push_back(new wallNode(23,16));





}


void SceneManager_Server::populateSpawnLocation(){
	std::vector<spawnNode*> spawnNodeMap;
	this->spawnNodeMap = &spawnNodeMap;
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 7.5, 27.5)); //1
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 12.5, 27.5)); //2
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 17.5, 27.5)); //3
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 22.5, 17.5)); //4
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 27.5, 17.5)); //5
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 17.5)); //6
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 42.5, 27.5)); //7
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 42.5)); //8
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 17.5, 52.5)); //9
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 22.5, 52.5)); //10
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 52.5)); //11
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 57.5)); //12
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 67.5)); //13
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 72.5)); //14
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 12.5, 87.5)); //15
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 12.5, 92.5)); //16
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 22.5, 82.5)); //17
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 27.5, 82.5)); //18
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 32.5, 82.5)); //19
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 27.5, 87.5)); //20
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 27.5, 92.5)); //21
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 42.5, 67.5)); //22
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 47.5, 67.5)); //23
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 52.5, 32.5)); //24
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 57.5, 42.5)); //25
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 57.5, 47.5)); //26
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 67.5, 17.5)); //27
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 67.5, 32.5)); //28
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 72.5, 62.5)); //29
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 77.5, 12.5)); //30
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 82.5, 12.5)); //31
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 77.5, 17.5)); //32
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 97.5, 27.5)); //33
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 92.5, 82.5)); //34
	spawnNodeMap.push_back(new spawnNode(RED_TEAM, 117.5, 42.5)); //35

	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 122.5, 72.5)); //36
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 117.5, 72.5)); //37
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 117.5, 67.5)); //38
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 117.5, 52.5)); //39
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 117.5, 47.5)); //40
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 127.5, 37.5)); //41
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 122.5, 22.5)); //42
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 122.5, 17.5)); //43
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 107.5, 92.5)); //44
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 92.5)); //45
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 97.5, 92.5)); //46
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 92.5, 92.5)); //47
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 82.5)); //48
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 77.5)); //49
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 107.5, 22.5)); //50
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 17.5)); //51
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 12.5)); //52
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 102.5, 7.5)); //53
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 107.5, 7.5)); //54
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 92.5, 17.5)); //55
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 92.5, 62.5)); //56
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 92.5, 67.5)); //57
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 82.5, 77.5)); //58
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 82.5, 62.5)); //59
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 77.5, 52.5)); //60
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 82.5, 42.5)); //61
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 67.5, 92.5)); //62
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 67.5, 87.5)); //63
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 62.5, 52.5)); //64
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 57.5, 72.5)); //65
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 47.5, 52.5)); //66
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 37.5, 92.5)); //67
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 32.5, 92.5)); //68
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 32.5, 27.5)); //69
	spawnNodeMap.push_back(new spawnNode(BLUE_TEAM, 12.5, 67.5)); //70
	
}

void SceneManager_Server::populateMap() {
	std::vector<mapNode*> nodeMap;
	this->map = &nodeMap;
	nodeMap.push_back(new mapNode(1*5+2.5, 4*5+2.5)); //0
	nodeMap.push_back(new mapNode(5*5+2.5, 4*5+2.5)); //1
	nodeMap[0]->setNextRed(nodeMap[1]);
	nodeMap.push_back(new mapNode(5*5+2.5, 9*5+2.5)); //2
	nodeMap[1]->setNextRed(nodeMap[2]);
	nodeMap.push_back(new mapNode(5*5+2.5, 12*5+2.5)); //3
	nodeMap[2]->setNextRed(nodeMap[3]);
	nodeMap.push_back(new mapNode(5*5+2.5, 15*5+2.5)); //4
	nodeMap[3]->setNextRed(nodeMap[4]);
	nodeMap.push_back(new mapNode(12*5+2.5, 15*5+2.5)); //5
	nodeMap[4]->setNextRed(nodeMap[5]);
	nodeMap.push_back(new mapNode(12*5+2.5, 11*5+2.5)); //6
	nodeMap[5]->setNextRed(nodeMap[6]);
	nodeMap.push_back(new mapNode(14*5+2.5, 11*5+2.5)); //7
	nodeMap[6]->setNextRed(nodeMap[7]);
	nodeMap.push_back(new mapNode(17*5+2.5, 11*5+2.5)); //8
	nodeMap[7]->setNextRed(nodeMap[8]);
	nodeMap.push_back(new mapNode(22*5+2.5, 11*5+2.5)); //9
	nodeMap[8]->setNextRed(nodeMap[9]);
	nodeMap.push_back(new mapNode(22*5+2.5, 15*5+2.5)); //10
	nodeMap[9]->setNextRed(nodeMap[10]);
	nodeMap.push_back(new mapNode(24*5+2.5, 15*5+2.5)); //11
	nodeMap[10]->setNextRed(nodeMap[11]);
	nodeMap.push_back(new mapNode(7*5+2.5, 4*5+2.5)); //12
	nodeMap[12]->setNextRed(nodeMap[1]);
	nodeMap.push_back(new mapNode(10*5+2.5, 7*5+2.5)); //13
	nodeMap.push_back(new mapNode(14*5+2.5, 7*5+2.5)); //14
	nodeMap[13]->setNextRed(nodeMap[14]);
	nodeMap[14]->setNextRed(nodeMap[7]);
	nodeMap.push_back(new mapNode(14*5+2.5, 4*5+2.5)); //15
	nodeMap[15]->setNextRed(nodeMap[14]);
	nodeMap.push_back(new mapNode(22*5+2.5, 4*5+2.5)); //16
	nodeMap[16]->setNextRed(nodeMap[9]);
	nodeMap.push_back(new mapNode(14*5+2.5, 1*5+2.5)); //17
	nodeMap[17]->setNextRed(nodeMap[15]);
	nodeMap.push_back(new mapNode(3*5+2.5, 15*5+2.5)); //18
	nodeMap[18]->setNextRed(nodeMap[4]);
	nodeMap.push_back(new mapNode(17*5+2.5, 17*5+2.5)); //19
	nodeMap.push_back(new mapNode(21*5+2.5, 17*5+2.5)); //20
	nodeMap[19]->setNextRed(nodeMap[20]);
	nodeMap.push_back(new mapNode(21*5+2.5, 15*5+2.5)); //21
	nodeMap[20]->setNextRed(nodeMap[21]);
	nodeMap.push_back(new mapNode(10*5+2.5, 12*5+2.5)); //22
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
	nodeMap.push_back(new mapNode(3*5+2.5, 19*5+2.5)); //23
	nodeMap[23]->setNextBlue(nodeMap[18]);
	nodeMap[18]->setNextBlue(nodeMap[4]);
	nodeMap.push_back(new mapNode(17*5+2.5, 14*5+2.5)); //24
	nodeMap[24]->setNextBlue(nodeMap[8]);
	nodeMap[19]->setNextBlue(nodeMap[24]);
	nodeMap[20]->setNextBlue(nodeMap[19]);
	nodeMap[16]->setNextBlue(nodeMap[9]);
	nodeMap.push_back(new mapNode(19*5+2.5, 0*5+2.5)); //25
	nodeMap.push_back(new mapNode(19*5+2.5, 4*5+2.5)); //26
	nodeMap[25]->setNextBlue(nodeMap[26]);
	nodeMap[26]->setNextBlue(nodeMap[16]);
	nodeMap.push_back(new mapNode(25*5+2.5, 6*5+2.5)); //27
	nodeMap.push_back(new mapNode(22*5+2.5, 6*5+2.5)); //28
	nodeMap[27]->setNextBlue(nodeMap[28]);
	nodeMap[28]->setNextBlue(nodeMap[9]);
	nodeMap.push_back(new mapNode(17*5+2.5, 7*5+2.5)); //29
	nodeMap[29]->setNextBlue(nodeMap[8]);
	nodeMap.push_back(new mapNode(7*5+2.5, 9*5+2.5)); //30
	nodeMap[30]->setNextBlue(nodeMap[2]);


	// Add map nodes next to the towers for smooth animation
	nodeMap.push_back(new mapNode(12.5, 22.5)); // 31
	nodeMap.push_back(new mapNode(17.5, 22.5)); // 32
	nodeMap.push_back(new mapNode(4*5+2.5, 4*4+2.5)); // 33
	nodeMap.push_back(new mapNode(6*5+2.5, 4*5+2.5)); // 34
	nodeMap.push_back(new mapNode(5*5+2.5, 5*5+2.5)); // 35
	nodeMap.push_back(new mapNode(7*5+2.5, 5*5+2.5)); // 36
	nodeMap.push_back(new mapNode(5*5+2.5, 8*5+2.5)); // 37
	nodeMap.push_back(new mapNode(3*5+2.5, 9*5+2.5)); // 38
	nodeMap.push_back(new mapNode(4*5+2.5, 9*5+2.5)); // 39
	nodeMap.push_back(new mapNode(5*5+2.5, 10*5+2.5)); // 40
	nodeMap.push_back(new mapNode(5*5+2.5, 11*5+2.5)); // 41
	nodeMap.push_back(new mapNode(5*5+2.5, 13*5+2.5)); // 42
	nodeMap.push_back(new mapNode(5*5+2.5, 14*5+2.5)); // 43
	nodeMap.push_back(new mapNode(3*5+2.5, 13*5+2.5)); // 44
	nodeMap.push_back(new mapNode(3*5+2.5, 16*5+2.5)); // 45
	nodeMap.push_back(new mapNode(3*5+2.5, 17*5+2.5)); // 46
	nodeMap.push_back(new mapNode(3*5+2.5, 18*5+2.5)); // 47
	nodeMap.push_back(new mapNode(6*5+2.5, 15*5+2.5)); // 48
	nodeMap.push_back(new mapNode(8*5+2.5, 12*5+2.5)); // 49
	nodeMap.push_back(new mapNode(9*5+2.5, 12*5+2.5)); // 50
	nodeMap.push_back(new mapNode(6*5+2.5, 19*5+2.5)); // 51
	nodeMap.push_back(new mapNode(7*5+2.5, 19*5+2.5)); // 52
	nodeMap.push_back(new mapNode(9*5+2.5, 9*5+2.5)); // 53
	nodeMap.push_back(new mapNode(10*5+2.5, 8*5+2.5)); // 54
	nodeMap.push_back(new mapNode(10*5+2.5, 9*5+2.5)); // 55
	nodeMap.push_back(new mapNode(11*5+2.5, 15*5+2.5)); // 56
	nodeMap.push_back(new mapNode(12*5+2.5, 17*5+2.5)); // 57
	nodeMap.push_back(new mapNode(12*5+2.5, 18*5+2.5)); // 58
	nodeMap.push_back(new mapNode(14*5+2.5, 2*5+2.5)); // 59
	nodeMap.push_back(new mapNode(14*5+2.5, 3*5+2.5)); // 60
	nodeMap.push_back(new mapNode(14*5+2.5, 6*5+2.5)); // 61
	nodeMap.push_back(new mapNode(14*5+2.5, 10*5+2.5)); // 62
	nodeMap.push_back(new mapNode(16*5+2.5, 1*5+2.5)); // 63
	nodeMap.push_back(new mapNode(17*5+2.5, 8*5+2.5)); // 64
	nodeMap.push_back(new mapNode(17*5+2.5, 12*5+2.5)); // 65
	nodeMap.push_back(new mapNode(17*5+2.5, 13*5+2.5)); // 66
	nodeMap.push_back(new mapNode(17*5+2.5, 15*5+2.5)); // 67
	nodeMap.push_back(new mapNode(17*5+2.5, 16*5+2.5)); // 68
	nodeMap.push_back(new mapNode(18*5+2.5, 17*5+2.5)); // 69
	nodeMap.push_back(new mapNode(19*5+2.5, 1*5+2.5)); // 70
	nodeMap.push_back(new mapNode(19*5+2.5, 2*5+2.5)); // 71
	nodeMap.push_back(new mapNode(19*5+2.5, 3*5+2.5)); // 72
	nodeMap.push_back(new mapNode(19*5+2.5, 17*5+2.5)); // 73
	nodeMap.push_back(new mapNode(20*5+2.5, 17*5+2.5)); // 74
	nodeMap.push_back(new mapNode(21*5+2.5, 0*5+2.5)); // 75
	nodeMap.push_back(new mapNode(21*5+2.5, 16*5+2.5)); // 76
	nodeMap.push_back(new mapNode(22*5+2.5, 5*5+2.5)); // 77
	nodeMap.push_back(new mapNode(22*5+2.5, 8*5+2.5)); // 78
	nodeMap.push_back(new mapNode(22*5+2.5, 9*5+2.5)); // 79
	nodeMap.push_back(new mapNode(22*5+2.5, 10*5+2.5)); // 80
	nodeMap.push_back(new mapNode(22*5+2.5, 13*5+2.5)); // 81
	nodeMap.push_back(new mapNode(22*5+2.5, 14*5+2.5)); // 82
	nodeMap.push_back(new mapNode(25*5+2.5, 3*5+2.5)); // 83
	nodeMap.push_back(new mapNode(25*5+2.5, 4*5+2.5)); // 84

	// Connect red the nodes
	nodeMap[31]->setNextRed(nodeMap[32]);
	nodeMap[32]->setNextRed(nodeMap[33]);
	nodeMap[33]->setNextRed(nodeMap[1]);
	nodeMap[34]->setNextRed(nodeMap[1]);
	nodeMap[36]->setNextRed(nodeMap[12]);
	nodeMap[37]->setNextRed(nodeMap[2]);
	nodeMap[38]->setNextRed(nodeMap[39]);
	nodeMap[39]->setNextRed(nodeMap[2]);
	nodeMap[40]->setNextRed(nodeMap[41]);
	nodeMap[41]->setNextRed(nodeMap[3]);
	nodeMap[42]->setNextRed(nodeMap[2]);
	nodeMap[43]->setNextRed(nodeMap[44]);
	nodeMap[44]->setNextRed(nodeMap[4]);
	nodeMap[45]->setNextRed(nodeMap[18]);
	nodeMap[46]->setNextRed(nodeMap[45]);
	nodeMap[47]->setNextRed(nodeMap[46]);
	nodeMap[48]->setNextRed(nodeMap[4]);
	nodeMap[49]->setNextRed(nodeMap[50]);
	nodeMap[50]->setNextRed(nodeMap[22]);
	nodeMap[54]->setNextRed(nodeMap[13]);
	nodeMap[55]->setNextRed(nodeMap[54]);
	nodeMap[59]->setNextRed(nodeMap[60]);
	nodeMap[60]->setNextRed(nodeMap[15]);
	nodeMap[61]->setNextRed(nodeMap[14]);
	nodeMap[63]->setNextRed(nodeMap[17]);
	nodeMap[68]->setNextRed(nodeMap[19]);
	nodeMap[26]->setNextRed(nodeMap[16]);
	nodeMap[78]->setNextRed(nodeMap[9]);
	
	// Connect blue nodes
	nodeMap[35]->setNextBlue(nodeMap[1]);
	nodeMap[44]->setNextBlue(nodeMap[18]);
	nodeMap[52]->setNextBlue(nodeMap[51]);
	nodeMap[51]->setNextBlue(nodeMap[23]);
	nodeMap[53]->setNextBlue(nodeMap[30]);
	nodeMap[56]->setNextBlue(nodeMap[4]);
	nodeMap[58]->setNextBlue(nodeMap[57]);
	nodeMap[57]->setNextBlue(nodeMap[5]);
	nodeMap[62]->setNextBlue(nodeMap[7]);
	nodeMap[64]->setNextBlue(nodeMap[8]);
	nodeMap[65]->setNextBlue(nodeMap[8]);
	nodeMap[66]->setNextBlue(nodeMap[65]);
	nodeMap[67]->setNextBlue(nodeMap[24]);
	nodeMap[70]->setNextBlue(nodeMap[71]);
	nodeMap[71]->setNextBlue(nodeMap[72]);
	nodeMap[72]->setNextBlue(nodeMap[26]);
	nodeMap[73]->setNextBlue(nodeMap[69]);
	nodeMap[69]->setNextBlue(nodeMap[19]);
	nodeMap[74]->setNextBlue(nodeMap[73]);
	nodeMap[75]->setNextBlue(nodeMap[25]);
	nodeMap[76]->setNextBlue(nodeMap[20]);
	nodeMap[77]->setNextBlue(nodeMap[28]);
	nodeMap[79]->setNextBlue(nodeMap[80]);
	nodeMap[81]->setNextBlue(nodeMap[82]);
	nodeMap[82]->setNextBlue(nodeMap[10]);
	nodeMap[83]->setNextBlue(nodeMap[84]);
	nodeMap[84]->setNextBlue(nodeMap[27]);
	
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

		int id = next_minion_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Minion* m = new Minion(id, red_team, this);
		m->setMatrix(transform);
		idMap[id] = m;

		std::cout << "created new minion: " << id << " at " << idMap[id] << "\n";

		next_minion_id++;
		if (next_minion_id == ID_MINION_MAX) next_minion_id = ID_MINION_MIN;
	}
	//spawn super minions
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		int id = next_super_minion_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		SuperMinion* sm = new SuperMinion(id, red_team, this);
		sm->setMatrix(transform);
		idMap[id] = sm;

		std::cout << "created new super minion: " << id << " at " << idMap[id] << "\n";

		next_super_minion_id++;
		if (next_super_minion_id == ID_SUPER_MINION_MAX) next_super_minion_id = ID_SUPER_MINION_MIN;
	}
	//spawn laser towers
	for (int i = 0; i < 4; i++) { 
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		int id = next_laser_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		LaserTower* l = new LaserTower(id, blue_team, this);
		l->setMatrix(transform);
		idMap[id] = l;

		std::cout << "created new laser tower: " << id << " at " << idMap[id] << "\n";

		next_laser_id++;
		if (next_laser_id == ID_LASER_MAX) next_laser_id = ID_LASER_MIN;
	}
	//spawn claw towers
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		int id = next_claw_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		ClawTower* c = new ClawTower(id, blue_team, this);
		c->setMatrix(transform);
		idMap[id] = c;

		std::cout << "created new claw tower: " << id << " at " << idMap[id] << "\n";

		next_claw_id++;
		if (next_claw_id == ID_CLAW_MAX) next_claw_id = ID_CLAW_MIN;
	}
	//spawn dumpsters
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		int id = next_dumpster_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Resource* d = new Resource(DUMPSTER_TYPE, id, this);
		d->setMatrix(transform);
		idMap[id] = d;

		std::cout << "created new dumpster: " << id << " at " << idMap[id] << "\n";

		next_dumpster_id++;
		if (next_dumpster_id == ID_DUMPSTER_MAX) next_dumpster_id = ID_DUMPSTER_MIN;
	}
	//spawn recycling bins
	for (int i = 0; i < 4; i++) {
		float x = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float z = -100 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (200)));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / PI));
		float s = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));

		int id = next_recycling_bin_id;
		mat4 transform = mat4::translation(vec3(x, 0, z)) * mat4::rotationY(rot) * mat4::scale(vec3(s));
		Resource* r = new Resource(RECYCLING_BIN_TYPE, id, this);
		r->setMatrix(transform);
		idMap[id] = r;

		std::cout << "created new recycling bin: " << id << " at " << idMap[id] << "\n";

		next_recycling_bin_id++;
		if (next_recycling_bin_id == ID_RECYCLING_BIN_MAX) next_recycling_bin_id = ID_RECYCLING_BIN_MIN;
	}
}

void SceneManager_Server::testAttacking() {
	int id;
	mat4 transform;

	/*id = next_minion_id;
	next_minion_id++;
	transform = mat4::translation(vec3(10, 0, 5));
	Minion* m1 = new Minion(id, red_team, this);
	m1->setMatrix(transform);
	idMap[id] = m1;
	std::cout << "created minion at id " << id << "\n";

	id = next_super_minion_id;
	next_super_minion_id++;
	transform = mat4::translation(vec3(14, 0, 5));
	SuperMinion* sm1 = new SuperMinion(id, blue_team, this);
	sm1->setMatrix(transform);
	idMap[id] = sm1;
	std::cout << "created super minion at id " << id << "\n";*/

	/*id = next_super_minion_id;
	next_super_minion_id++;
	transform = mat4::translation(vec3(-5, 0, 0));
	SuperMinion* sm2 = new SuperMinion(id, blue_team, this);
	sm2->setMatrix(transform);
	idMap[id] = sm2;

	id = next_claw_id;
	next_claw_id++;
	transform = mat4::translation(vec3(15, 0, 0));
	ClawTower* c1 = new ClawTower(id, red_team, this);
	c1->setMatrix(transform);
	idMap[id] = c1;*/

	id = next_laser_id;
	next_laser_id++;
	transform = mat4::translation(vec3(30, 0, 42));
	LaserTower* l1 = new LaserTower(id, red_team, this);
	l1->setMatrix(transform);
	idMap[id] = l1;

	id = next_laser_id;
	next_laser_id++;
	transform = mat4::translation(vec3(40, 0, 46));
	LaserTower* l2 = new LaserTower(id, blue_team, this);
	l2->setMatrix(transform);
	idMap[id] = l2;

	id = next_minion_id;
	next_minion_id++;
	transform = mat4::translation(vec3(20, 0, 24));
	Minion* m2 = new Minion(id, blue_team, this);
	m2->setMatrix(transform);
	idMap[id] = m2;
}

/***** legacy code *****/
/*void SceneManager_Server::resetClocks() {
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
}*/