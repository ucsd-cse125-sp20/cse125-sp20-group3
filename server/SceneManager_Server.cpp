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

	this->populatePaths();
	this->populateWalls();
	this->populateBuilds();

	//this->populateScene();
	//this->testAttacking();
	//this->testBuilding();
	//this->testWalls();
}

SceneManager_Server::~SceneManager_Server() {
	for (auto idEntPair : idMap) delete(idEntPair.second);
	for (auto path : pathNodes) delete(path);
	for (auto build : buildNodes) delete(build);
	for (auto wall : wallNodes) delete(wall);
}

void SceneManager_Server::processInput(int player_id, PlayerInput in) {
	((Player*)idMap[player_id])->processInput(in);
}

bool SceneManager_Server::addPlayer(int player_id) {
	//TODO figure out player spawn locations
	GameObject::GameObjectData data = { 7.5f, -7.5f, 0.0f };
	if (idMap.find(player_id) == idMap.end()) { //player_id not in map, create a new player
		idMap[player_id] = new Player(data, player_id, red_team, this); //TODO assign players teams based on lobby choices
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

	GameObject::GameObjectData data = { pos_x, pos_z, rot_y };

	switch (spawnType) {
	case BASE_TYPE:
		id = next_base_id;
		ent = new Base(data, id, t, this);

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
		ent = new Minion(data, id, t, this);

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
		ent = new SuperMinion(data, id, t, this);

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
		ent = new LaserTower(data, id, t, this);

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
		ent = new ClawTower(data, id, t, this);

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
		ent = new Resource(DUMPSTER_TYPE, data, id, this);

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
		ent = new Resource(RECYCLING_BIN_TYPE, data, id, this);

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
		ent = new Minion(data, id, t, this);
		std::cout << "spawnEntity encountered unknown entity type " << spawnType << "\n";
	}
	
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

	i += red_team->writeData(buf, i);
	buf[i] = DELIMITER;
	i++;
	i += blue_team->writeData(buf, i);
	buf[i] = DELIMITER;
	i++;

	buf[i] = DELIMITER; //closing delimiter
	i++;

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

void SceneManager_Server::populatePaths() {
	pathNodes.push_back(new PathNode(1, 4)); //0
	pathNodes.push_back(new PathNode(5, 4)); //1
	pathNodes[0]->setNextRed(pathNodes[1]);
	pathNodes.push_back(new PathNode(5, 9)); //2
	pathNodes[1]->setNextRed(pathNodes[2]);
	pathNodes.push_back(new PathNode(5, 12)); //3
	pathNodes[2]->setNextRed(pathNodes[3]);
	pathNodes.push_back(new PathNode(5, 15)); //4
	pathNodes[3]->setNextRed(pathNodes[4]);
	pathNodes.push_back(new PathNode(12, 15)); //5
	pathNodes[4]->setNextRed(pathNodes[5]);
	pathNodes.push_back(new PathNode(12, 11)); //6
	pathNodes[5]->setNextRed(pathNodes[6]);
	pathNodes.push_back(new PathNode(14, 11)); //7
	pathNodes[6]->setNextRed(pathNodes[7]);
	pathNodes.push_back(new PathNode(17, 11)); //8
	pathNodes[7]->setNextRed(pathNodes[8]);
	pathNodes.push_back(new PathNode(22, 11)); //9
	pathNodes[8]->setNextRed(pathNodes[9]);
	pathNodes.push_back(new PathNode(22, 15)); //10
	pathNodes[9]->setNextRed(pathNodes[10]);
	pathNodes.push_back(new PathNode(24, 15)); //11
	pathNodes[10]->setNextRed(pathNodes[11]);
	pathNodes.push_back(new PathNode(7, 4)); //12
	pathNodes[12]->setNextRed(pathNodes[1]);
	pathNodes.push_back(new PathNode(10, 7)); //13
	pathNodes.push_back(new PathNode(14, 7)); //14
	pathNodes[13]->setNextRed(pathNodes[14]);
	pathNodes[14]->setNextRed(pathNodes[7]);
	pathNodes.push_back(new PathNode(14, 4)); //15
	pathNodes[15]->setNextRed(pathNodes[14]);
	pathNodes.push_back(new PathNode(22, 4)); //16
	pathNodes[16]->setNextRed(pathNodes[9]);
	pathNodes.push_back(new PathNode(14, 1)); //17
	pathNodes[17]->setNextRed(pathNodes[15]);
	pathNodes.push_back(new PathNode(3, 15)); //18
	pathNodes[18]->setNextRed(pathNodes[4]);
	pathNodes.push_back(new PathNode(17, 17)); //19
	pathNodes.push_back(new PathNode(21, 17)); //20
	pathNodes[19]->setNextRed(pathNodes[20]);
	pathNodes.push_back(new PathNode(21, 15)); //21
	pathNodes[20]->setNextRed(pathNodes[21]);
	pathNodes.push_back(new PathNode(10, 12)); //22
	pathNodes[22]->setNextRed(pathNodes[13]);
	pathNodes[11]->setNextBlue(pathNodes[10]);
	pathNodes[10]->setNextBlue(pathNodes[9]);
	pathNodes[9]->setNextBlue(pathNodes[8]);
	pathNodes[8]->setNextBlue(pathNodes[7]);
	pathNodes[7]->setNextBlue(pathNodes[6]);
	pathNodes[6]->setNextBlue(pathNodes[5]);
	pathNodes[5]->setNextBlue(pathNodes[4]);
	pathNodes[4]->setNextBlue(pathNodes[3]);
	pathNodes[3]->setNextBlue(pathNodes[2]);
	pathNodes[2]->setNextBlue(pathNodes[1]);
	pathNodes[1]->setNextBlue(pathNodes[0]);
	pathNodes[12]->setNextBlue(pathNodes[1]);
	pathNodes.push_back(new PathNode(3, 19)); //23
	pathNodes[23]->setNextBlue(pathNodes[18]);
	pathNodes[18]->setNextBlue(pathNodes[4]);
	pathNodes.push_back(new PathNode(17, 14)); //24
	pathNodes[24]->setNextBlue(pathNodes[8]);
	pathNodes[19]->setNextBlue(pathNodes[24]);
	pathNodes[20]->setNextBlue(pathNodes[19]);
	pathNodes[16]->setNextBlue(pathNodes[9]);
	pathNodes.push_back(new PathNode(19, 0)); //25
	pathNodes.push_back(new PathNode(19, 4)); //26
	pathNodes[25]->setNextBlue(pathNodes[26]);
	pathNodes[26]->setNextBlue(pathNodes[16]);
	pathNodes.push_back(new PathNode(25, 6)); //27
	pathNodes.push_back(new PathNode(22, 6)); //28
	pathNodes[27]->setNextBlue(pathNodes[28]);
	pathNodes[28]->setNextBlue(pathNodes[9]);
	pathNodes.push_back(new PathNode(17, 7)); //29
	pathNodes[29]->setNextBlue(pathNodes[8]);
	pathNodes.push_back(new PathNode(7, 9)); //30
	pathNodes[30]->setNextBlue(pathNodes[2]);

	// Add map nodes next to the towers for smooth animation
	pathNodes.push_back(new PathNode(2, 4)); // 31
	pathNodes.push_back(new PathNode(3, 4)); // 32
	pathNodes.push_back(new PathNode(4, 4)); // 33
	pathNodes.push_back(new PathNode(6, 4)); // 34
	pathNodes.push_back(new PathNode(5, 5)); // 35
	pathNodes.push_back(new PathNode(7, 5)); // 36
	pathNodes.push_back(new PathNode(5, 8)); // 37
	pathNodes.push_back(new PathNode(3, 9)); // 38
	pathNodes.push_back(new PathNode(4, 9)); // 39
	pathNodes.push_back(new PathNode(5, 10)); // 40
	pathNodes.push_back(new PathNode(5, 11)); // 41
	pathNodes.push_back(new PathNode(5, 13)); // 42
	pathNodes.push_back(new PathNode(5, 14)); // 43
	pathNodes.push_back(new PathNode(3, 13)); // 44
	pathNodes.push_back(new PathNode(3, 16)); // 45
	pathNodes.push_back(new PathNode(3, 17)); // 46
	pathNodes.push_back(new PathNode(3, 18)); // 47
	pathNodes.push_back(new PathNode(6, 15)); // 48
	pathNodes.push_back(new PathNode(8, 12)); // 49
	pathNodes.push_back(new PathNode(9, 12)); // 50
	pathNodes.push_back(new PathNode(6, 19)); // 51
	pathNodes.push_back(new PathNode(7, 19)); // 52
	pathNodes.push_back(new PathNode(9, 9)); // 53
	pathNodes.push_back(new PathNode(10, 8)); // 54
	pathNodes.push_back(new PathNode(10, 9)); // 55
	pathNodes.push_back(new PathNode(11, 15)); // 56
	pathNodes.push_back(new PathNode(12, 17)); // 57
	pathNodes.push_back(new PathNode(12, 18)); // 58
	pathNodes.push_back(new PathNode(14, 2)); // 59
	pathNodes.push_back(new PathNode(14, 3)); // 60
	pathNodes.push_back(new PathNode(14, 6)); // 61
	pathNodes.push_back(new PathNode(14, 10)); // 62
	pathNodes.push_back(new PathNode(16, 1)); // 63
	pathNodes.push_back(new PathNode(17, 8)); // 64
	pathNodes.push_back(new PathNode(17, 12)); // 65
	pathNodes.push_back(new PathNode(17, 13)); // 66
	pathNodes.push_back(new PathNode(17, 15)); // 67
	pathNodes.push_back(new PathNode(17, 16)); // 68
	pathNodes.push_back(new PathNode(18, 17)); // 69
	pathNodes.push_back(new PathNode(19, 1)); // 70
	pathNodes.push_back(new PathNode(19, 2)); // 71
	pathNodes.push_back(new PathNode(19, 3)); // 72
	pathNodes.push_back(new PathNode(19, 17)); // 73
	pathNodes.push_back(new PathNode(20, 17)); // 74
	pathNodes.push_back(new PathNode(21, 0)); // 75
	pathNodes.push_back(new PathNode(21, 16)); // 76
	pathNodes.push_back(new PathNode(22, 5)); // 77
	pathNodes.push_back(new PathNode(22, 8)); // 78
	pathNodes.push_back(new PathNode(22, 9)); // 79
	pathNodes.push_back(new PathNode(22, 10)); // 80
	pathNodes.push_back(new PathNode(22, 13)); // 81
	pathNodes.push_back(new PathNode(22, 14)); // 82
	pathNodes.push_back(new PathNode(25, 3)); // 83
	pathNodes.push_back(new PathNode(25, 4)); // 84

	// Connect red the nodes
	pathNodes[31]->setNextRed(pathNodes[32]);
	pathNodes[32]->setNextRed(pathNodes[33]);
	pathNodes[33]->setNextRed(pathNodes[1]);
	pathNodes[34]->setNextRed(pathNodes[1]);
	pathNodes[36]->setNextRed(pathNodes[12]);
	pathNodes[37]->setNextRed(pathNodes[2]);
	pathNodes[38]->setNextRed(pathNodes[39]);
	pathNodes[39]->setNextRed(pathNodes[2]);
	pathNodes[40]->setNextRed(pathNodes[41]);
	pathNodes[41]->setNextRed(pathNodes[3]);
	pathNodes[42]->setNextRed(pathNodes[2]);
	pathNodes[43]->setNextRed(pathNodes[44]);
	pathNodes[44]->setNextRed(pathNodes[4]);
	pathNodes[45]->setNextRed(pathNodes[18]);
	pathNodes[46]->setNextRed(pathNodes[45]);
	pathNodes[47]->setNextRed(pathNodes[46]);
	pathNodes[48]->setNextRed(pathNodes[4]);
	pathNodes[49]->setNextRed(pathNodes[50]);
	pathNodes[50]->setNextRed(pathNodes[22]);
	pathNodes[54]->setNextRed(pathNodes[13]);
	pathNodes[55]->setNextRed(pathNodes[54]);
	pathNodes[59]->setNextRed(pathNodes[60]);
	pathNodes[60]->setNextRed(pathNodes[15]);
	pathNodes[61]->setNextRed(pathNodes[14]);
	pathNodes[63]->setNextRed(pathNodes[17]);
	pathNodes[68]->setNextRed(pathNodes[19]);
	pathNodes[26]->setNextRed(pathNodes[16]);
	pathNodes[78]->setNextRed(pathNodes[9]);

	// Connect blue nodes
	pathNodes[35]->setNextBlue(pathNodes[1]);
	pathNodes[44]->setNextBlue(pathNodes[18]);
	pathNodes[52]->setNextBlue(pathNodes[51]);
	pathNodes[51]->setNextBlue(pathNodes[23]);
	pathNodes[53]->setNextBlue(pathNodes[30]);
	pathNodes[56]->setNextBlue(pathNodes[4]);
	pathNodes[58]->setNextBlue(pathNodes[57]);
	pathNodes[57]->setNextBlue(pathNodes[5]);
	pathNodes[62]->setNextBlue(pathNodes[7]);
	pathNodes[64]->setNextBlue(pathNodes[8]);
	pathNodes[65]->setNextBlue(pathNodes[8]);
	pathNodes[66]->setNextBlue(pathNodes[65]);
	pathNodes[67]->setNextBlue(pathNodes[24]);
	pathNodes[70]->setNextBlue(pathNodes[71]);
	pathNodes[71]->setNextBlue(pathNodes[72]);
	pathNodes[72]->setNextBlue(pathNodes[26]);
	pathNodes[73]->setNextBlue(pathNodes[69]);
	pathNodes[69]->setNextBlue(pathNodes[19]);
	pathNodes[74]->setNextBlue(pathNodes[73]);
	pathNodes[75]->setNextBlue(pathNodes[25]);
	pathNodes[76]->setNextBlue(pathNodes[20]);
	pathNodes[77]->setNextBlue(pathNodes[28]);
	pathNodes[79]->setNextBlue(pathNodes[80]);
	pathNodes[81]->setNextBlue(pathNodes[82]);
	pathNodes[82]->setNextBlue(pathNodes[10]);
	pathNodes[83]->setNextBlue(pathNodes[84]);
	pathNodes[84]->setNextBlue(pathNodes[27]);

}

void SceneManager_Server::populateWalls(){
	// boundaries
	for (float i = -1; i < 27; i++) {
		wallNodes.push_back(new WallNode(i, -1.0f));
		wallNodes.push_back(new WallNode(i, 20.0f));
	}
	for (float i = 0; i < 20; i++) {
		wallNodes.push_back(new WallNode(-1.0f, i));
		wallNodes.push_back(new WallNode(26.0f, i));
	}

	wallNodes.push_back(new WallNode(0,3)); 
	wallNodes.push_back(new WallNode(0,4)); 
	wallNodes.push_back(new WallNode(0,5));
	wallNodes.push_back(new WallNode(2,3)); 
	wallNodes.push_back(new WallNode(3,3));
	wallNodes.push_back(new WallNode(4,5));
	wallNodes.push_back(new WallNode(6,2));
	wallNodes.push_back(new WallNode(7,1));
	wallNodes.push_back(new WallNode(8,1));
	wallNodes.push_back(new WallNode(9,1));
	wallNodes.push_back(new WallNode(10,1));
	wallNodes.push_back(new WallNode(11,1));
	wallNodes.push_back(new WallNode(12,1));
	wallNodes.push_back(new WallNode(13,1));
	wallNodes.push_back(new WallNode(14,0));
	wallNodes.push_back(new WallNode(15,0));
	wallNodes.push_back(new WallNode(16,0));
	wallNodes.push_back(new WallNode(17,0));
	wallNodes.push_back(new WallNode(18,0));
	wallNodes.push_back(new WallNode(9,3));
	wallNodes.push_back(new WallNode(10,3));
	wallNodes.push_back(new WallNode(11,3));
	wallNodes.push_back(new WallNode(12,3));
	wallNodes.push_back(new WallNode(17,2));
	wallNodes.push_back(new WallNode(18,2));
	wallNodes.push_back(new WallNode(22,1));
	wallNodes.push_back(new WallNode(23,1));
	wallNodes.push_back(new WallNode(24,1));
	wallNodes.push_back(new WallNode(23,5));
	wallNodes.push_back(new WallNode(24,5));
	wallNodes.push_back(new WallNode(21,3));
	wallNodes.push_back(new WallNode(22,3));
	wallNodes.push_back(new WallNode(23,4));
	wallNodes.push_back(new WallNode(24,2));
	wallNodes.push_back(new WallNode(16,3));
	wallNodes.push_back(new WallNode(17,3));
	wallNodes.push_back(new WallNode(4,6));
	wallNodes.push_back(new WallNode(4,7));
	wallNodes.push_back(new WallNode(4,8));
	wallNodes.push_back(new WallNode(3,8));
	wallNodes.push_back(new WallNode(2,8));
	wallNodes.push_back(new WallNode(1,9));
	wallNodes.push_back(new WallNode(6,6));
	wallNodes.push_back(new WallNode(6,7));
	wallNodes.push_back(new WallNode(8,4));
	wallNodes.push_back(new WallNode(8,6));
	wallNodes.push_back(new WallNode(8,7));
	wallNodes.push_back(new WallNode(8,8));
	wallNodes.push_back(new WallNode(9,7));
	wallNodes.push_back(new WallNode(9,8));
	wallNodes.push_back(new WallNode(11,6));
	wallNodes.push_back(new WallNode(12,6));
	wallNodes.push_back(new WallNode(13,4));
	wallNodes.push_back(new WallNode(13,5));
	wallNodes.push_back(new WallNode(15,5));
	wallNodes.push_back(new WallNode(16,5));
	wallNodes.push_back(new WallNode(17,5));
	wallNodes.push_back(new WallNode(18,5));
	wallNodes.push_back(new WallNode(20,5));
	wallNodes.push_back(new WallNode(15,6));
	wallNodes.push_back(new WallNode(16,6));
	wallNodes.push_back(new WallNode(17,6));
	wallNodes.push_back(new WallNode(18,8));
	wallNodes.push_back(new WallNode(18,9));
	wallNodes.push_back(new WallNode(18,10));
	wallNodes.push_back(new WallNode(19,10));
	wallNodes.push_back(new WallNode(21,10));
	wallNodes.push_back(new WallNode(18,7));
	wallNodes.push_back(new WallNode(21,6));
	wallNodes.push_back(new WallNode(21,7));
	wallNodes.push_back(new WallNode(21,8));
	wallNodes.push_back(new WallNode(21,9));
	wallNodes.push_back(new WallNode(23,7));
	wallNodes.push_back(new WallNode(24,7));
	wallNodes.push_back(new WallNode(23,11));
	wallNodes.push_back(new WallNode(23,12));
	wallNodes.push_back(new WallNode(25,15));
	wallNodes.push_back(new WallNode(25,16));
	wallNodes.push_back(new WallNode(0,10)); // New
	wallNodes.push_back(new WallNode(0,13)); // New
	wallNodes.push_back(new WallNode(1,5)); // New
	wallNodes.push_back(new WallNode(1,6)); // New
	wallNodes.push_back(new WallNode(1,11));
	wallNodes.push_back(new WallNode(1,12));
	wallNodes.push_back(new WallNode(1,13));
	wallNodes.push_back(new WallNode(1,14)); // New
	wallNodes.push_back(new WallNode(1,17)); // New
	wallNodes.push_back(new WallNode(1,18)); // New
	wallNodes.push_back(new WallNode(2,5)); // New
	wallNodes.push_back(new WallNode(2,6)); // New
	wallNodes.push_back(new WallNode(2,14));
	wallNodes.push_back(new WallNode(2,15));
	wallNodes.push_back(new WallNode(2,17)); // New
	wallNodes.push_back(new WallNode(2,18)); // New
	wallNodes.push_back(new WallNode(2,16));
	wallNodes.push_back(new WallNode(2,19));
	wallNodes.push_back(new WallNode(3,0)); // New
	wallNodes.push_back(new WallNode(3,1)); // New
	wallNodes.push_back(new WallNode(3,2)); // New
	wallNodes.push_back(new WallNode(3,5)); // New
	wallNodes.push_back(new WallNode(3,6)); // New
	wallNodes.push_back(new WallNode(3,10)); // New
	wallNodes.push_back(new WallNode(3,11));
	wallNodes.push_back(new WallNode(4,2)); // New
	wallNodes.push_back(new WallNode(4,3)); // New
	wallNodes.push_back(new WallNode(4,10)); // New
	wallNodes.push_back(new WallNode(4,11));
	wallNodes.push_back(new WallNode(4,12));
	wallNodes.push_back(new WallNode(4,13));
	wallNodes.push_back(new WallNode(4,14));
	wallNodes.push_back(new WallNode(4,16)); // New
	wallNodes.push_back(new WallNode(4,17)); // New
	wallNodes.push_back(new WallNode(4,18)); // New
	wallNodes.push_back(new WallNode(5,2)); // New
	wallNodes.push_back(new WallNode(5,3)); // New
	wallNodes.push_back(new WallNode(5,16)); // New
	wallNodes.push_back(new WallNode(5,17)); // New
	wallNodes.push_back(new WallNode(5,18));
	wallNodes.push_back(new WallNode(6,3)); // New
	wallNodes.push_back(new WallNode(6,5)); // New
	wallNodes.push_back(new WallNode(6,8)); // New
	wallNodes.push_back(new WallNode(6,10)); // New
	wallNodes.push_back(new WallNode(6,11)); // New
	wallNodes.push_back(new WallNode(6,13)); // New
	wallNodes.push_back(new WallNode(6,14)); // New
	wallNodes.push_back(new WallNode(6,16)); // New
	wallNodes.push_back(new WallNode(6,17)); // New
	wallNodes.push_back(new WallNode(6,18)); // New
	//wallNodes.push_back(new WallNode(7,2)); // New
	//wallNodes.push_back(new WallNode(7,3)); // New
	//wallNodes.push_back(new WallNode(7,5)); // New
	//wallNodes.push_back(new WallNode(7,6)); // New
	//wallNodes.push_back(new WallNode(7,7)); // New
	//wallNodes.push_back(new WallNode(7,8)); // New
	wallNodes.push_back(new WallNode(7,10));
	wallNodes.push_back(new WallNode(7,11));
	wallNodes.push_back(new WallNode(7,13));
	wallNodes.push_back(new WallNode(7,14));
	wallNodes.push_back(new WallNode(7,16));
	wallNodes.push_back(new WallNode(7,18)); // New
	wallNodes.push_back(new WallNode(8,5)); // New
	wallNodes.push_back(new WallNode(8,10));
	wallNodes.push_back(new WallNode(8,11));
	wallNodes.push_back(new WallNode(8,13)); // New
	wallNodes.push_back(new WallNode(8,14));
	wallNodes.push_back(new WallNode(8,16));
	wallNodes.push_back(new WallNode(8,18));
	wallNodes.push_back(new WallNode(9,4)); // New
	wallNodes.push_back(new WallNode(9,6)); // New
	wallNodes.push_back(new WallNode(9,10)); // New
	wallNodes.push_back(new WallNode(9,11));
	wallNodes.push_back(new WallNode(9,13)); // New
	wallNodes.push_back(new WallNode(9,14));
	wallNodes.push_back(new WallNode(9,16));
	wallNodes.push_back(new WallNode(9,18));
	wallNodes.push_back(new WallNode(10,5)); // New
	wallNodes.push_back(new WallNode(10,6)); // New
	wallNodes.push_back(new WallNode(10,13));
	wallNodes.push_back(new WallNode(10,14));
	wallNodes.push_back(new WallNode(10,16));
	wallNodes.push_back(new WallNode(10,18));
	wallNodes.push_back(new WallNode(11,8)); // New
	wallNodes.push_back(new WallNode(11,9)); // New
	wallNodes.push_back(new WallNode(11,10));
	wallNodes.push_back(new WallNode(11,11));
	wallNodes.push_back(new WallNode(11,12));
	wallNodes.push_back(new WallNode(11,13));
	wallNodes.push_back(new WallNode(11,14)); // New
	wallNodes.push_back(new WallNode(11,16));
	wallNodes.push_back(new WallNode(11,17));
	wallNodes.push_back(new WallNode(11,18));
	wallNodes.push_back(new WallNode(12,8));
	wallNodes.push_back(new WallNode(12,9)); // New
	wallNodes.push_back(new WallNode(12,10)); // New
	wallNodes.push_back(new WallNode(13,3)); // New
	wallNodes.push_back(new WallNode(13,8));
	wallNodes.push_back(new WallNode(13,9));
	wallNodes.push_back(new WallNode(13,10));
	wallNodes.push_back(new WallNode(13,12));
	wallNodes.push_back(new WallNode(13,13));
	wallNodes.push_back(new WallNode(13,14));
	wallNodes.push_back(new WallNode(13,15));
	wallNodes.push_back(new WallNode(13,16));
	wallNodes.push_back(new WallNode(13,17)); // New
	wallNodes.push_back(new WallNode(13,18)); // New
	wallNodes.push_back(new WallNode(14,12)); // New
	wallNodes.push_back(new WallNode(14,13)); // New
	wallNodes.push_back(new WallNode(14,14));
	wallNodes.push_back(new WallNode(14,15));
	wallNodes.push_back(new WallNode(14,16));
	wallNodes.push_back(new WallNode(14,17));
	wallNodes.push_back(new WallNode(14,18));
	wallNodes.push_back(new WallNode(15,2)); // New
	wallNodes.push_back(new WallNode(15,3)); // New
	wallNodes.push_back(new WallNode(15,8));
	wallNodes.push_back(new WallNode(15,9));
	wallNodes.push_back(new WallNode(15,10)); // New
	wallNodes.push_back(new WallNode(15,12));
	wallNodes.push_back(new WallNode(15,13));
	wallNodes.push_back(new WallNode(16,2)); // New
	wallNodes.push_back(new WallNode(16,8)); // New
	wallNodes.push_back(new WallNode(16,9));
	wallNodes.push_back(new WallNode(16,10));
	wallNodes.push_back(new WallNode(16,12)); // New
	wallNodes.push_back(new WallNode(16,13));
	wallNodes.push_back(new WallNode(16,15)); // New
	wallNodes.push_back(new WallNode(16,16));
	wallNodes.push_back(new WallNode(16,18));
	wallNodes.push_back(new WallNode(16,19));
	wallNodes.push_back(new WallNode(17,18));
	wallNodes.push_back(new WallNode(18,3)); // New
	wallNodes.push_back(new WallNode(18,12)); // New
	wallNodes.push_back(new WallNode(18,13)); // New
	wallNodes.push_back(new WallNode(18,14));
	wallNodes.push_back(new WallNode(18,15));
	wallNodes.push_back(new WallNode(18,16)); // New
	wallNodes.push_back(new WallNode(18,18)); // New
	wallNodes.push_back(new WallNode(18,19)); // New
	wallNodes.push_back(new WallNode(19,14));
	wallNodes.push_back(new WallNode(19,12));
	wallNodes.push_back(new WallNode(19,18)); // New
	wallNodes.push_back(new WallNode(19,19)); // New
	wallNodes.push_back(new WallNode(20,1)); // New
	wallNodes.push_back(new WallNode(20,2)); // New
	wallNodes.push_back(new WallNode(20,3)); // New
	wallNodes.push_back(new WallNode(20,23)); // New
	wallNodes.push_back(new WallNode(20,12));
	wallNodes.push_back(new WallNode(20,15)); // New
	wallNodes.push_back(new WallNode(20,16)); // New
	wallNodes.push_back(new WallNode(20,18)); // New
	wallNodes.push_back(new WallNode(20,19)); // New
	wallNodes.push_back(new WallNode(21,1)); // New
	wallNodes.push_back(new WallNode(21,2)); // New
	wallNodes.push_back(new WallNode(21,12));
	wallNodes.push_back(new WallNode(21,18)); // New
	wallNodes.push_back(new WallNode(21,19)); // New
	wallNodes.push_back(new WallNode(21,13));
	wallNodes.push_back(new WallNode(21,14));
	wallNodes.push_back(new WallNode(22,16));
	wallNodes.push_back(new WallNode(22,17));
	wallNodes.push_back(new WallNode(22,18)); // New
	wallNodes.push_back(new WallNode(22,19)); // New
	wallNodes.push_back(new WallNode(23,3)); // New
	wallNodes.push_back(new WallNode(23,8)); // New
	wallNodes.push_back(new WallNode(23,9)); // New
	wallNodes.push_back(new WallNode(23,10)); // New
	wallNodes.push_back(new WallNode(23,13)); // New
	wallNodes.push_back(new WallNode(23,14)); // New
	wallNodes.push_back(new WallNode(23,16));
	wallNodes.push_back(new WallNode(24,3)); // New
	wallNodes.push_back(new WallNode(24,4)); // New
	wallNodes.push_back(new WallNode(24,8)); // New
	wallNodes.push_back(new WallNode(24,9)); // New
	wallNodes.push_back(new WallNode(24,10)); // New
	wallNodes.push_back(new WallNode(24,13)); // New
	wallNodes.push_back(new WallNode(24,14)); // New
	wallNodes.push_back(new WallNode(25,7)); // New
	wallNodes.push_back(new WallNode(25,8)); // New
	wallNodes.push_back(new WallNode(25,14)); // New
}

void SceneManager_Server::populateBuilds(){
	buildNodes.push_back(new BuildNode(RED_TEAM, 1, 5)); //1
	buildNodes.push_back(new BuildNode(RED_TEAM, 2, 5)); //2
	buildNodes.push_back(new BuildNode(RED_TEAM, 3, 5)); //3
	buildNodes.push_back(new BuildNode(RED_TEAM, 4, 3)); //4
	buildNodes.push_back(new BuildNode(RED_TEAM, 5, 3)); //5
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 3)); //6
	buildNodes.push_back(new BuildNode(RED_TEAM, 8, 5)); //7
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 8)); //8
	buildNodes.push_back(new BuildNode(RED_TEAM, 3, 10)); //9
	buildNodes.push_back(new BuildNode(RED_TEAM, 4, 10)); //10
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 10)); //11
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 11)); //12
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 13)); //13
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 14)); //14
	buildNodes.push_back(new BuildNode(RED_TEAM, 2, 17)); //15
	buildNodes.push_back(new BuildNode(RED_TEAM, 2, 18)); //16
	buildNodes.push_back(new BuildNode(RED_TEAM, 4, 16)); //17
	buildNodes.push_back(new BuildNode(RED_TEAM, 5, 16)); //18
	buildNodes.push_back(new BuildNode(RED_TEAM, 6, 16)); //19
	buildNodes.push_back(new BuildNode(RED_TEAM, 4, 17)); //20
	buildNodes.push_back(new BuildNode(RED_TEAM, 4, 18)); //21
	buildNodes.push_back(new BuildNode(RED_TEAM, 8, 13)); //22
	buildNodes.push_back(new BuildNode(RED_TEAM, 9, 13)); //23
	buildNodes.push_back(new BuildNode(RED_TEAM, 10, 6)); //24
	buildNodes.push_back(new BuildNode(RED_TEAM, 11, 8)); //25
	buildNodes.push_back(new BuildNode(RED_TEAM, 11, 9)); //26
	buildNodes.push_back(new BuildNode(RED_TEAM, 13, 3)); //27
	buildNodes.push_back(new BuildNode(RED_TEAM, 13, 6)); //28
	buildNodes.push_back(new BuildNode(RED_TEAM, 14, 12)); //29
	buildNodes.push_back(new BuildNode(RED_TEAM, 15, 2)); //30
	buildNodes.push_back(new BuildNode(RED_TEAM, 16, 2)); //31
	buildNodes.push_back(new BuildNode(RED_TEAM, 15, 3)); //32
	buildNodes.push_back(new BuildNode(RED_TEAM, 19, 5)); //33
	buildNodes.push_back(new BuildNode(RED_TEAM, 18, 16)); //34
	buildNodes.push_back(new BuildNode(RED_TEAM, 23, 8)); //35

	buildNodes.push_back(new BuildNode(BLUE_TEAM, 24, 14)); //36
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 23, 14)); //37
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 23, 13)); //38
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 23, 10)); //39
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 23, 9)); //40
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 25, 7)); //41
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 24, 4)); //42
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 24, 3)); //43
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 21, 18)); //44
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 18)); //45
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 19, 18)); //46
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 18, 18)); //47
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 16)); //48
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 15)); //49
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 21, 5)); //50
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 3)); //51
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 2)); //52
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 20, 1)); //53
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 21, 1)); //54
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 18, 3)); //55
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 18, 12)); //56
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 18, 13)); //57
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 16, 15)); //58
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 16, 12)); //59
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 15, 10)); //60
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 16, 8)); //61
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 13, 18)); //62
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 13, 17)); //63
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 12, 10)); //64
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 11, 14)); //65
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 9, 10)); //66
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 7, 18)); //67
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 6, 18)); //68
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 6, 5)); //69
	buildNodes.push_back(new BuildNode(BLUE_TEAM, 2, 13)); //70
	
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

		int id = next_minion_id;
		GameObject::GameObjectData data = { x, z, rot };
		Minion* m = new Minion(data, id, red_team, this);
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

		int id = next_super_minion_id;
		GameObject::GameObjectData data = { x, z, rot };
		SuperMinion* sm = new SuperMinion(data, id, red_team, this);
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

		int id = next_laser_id;
		GameObject::GameObjectData data = { x, z, rot };
		LaserTower* l = new LaserTower(data, id, blue_team, this);
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

		int id = next_claw_id;
		GameObject::GameObjectData data = { x, z, rot };
		ClawTower* c = new ClawTower(data, id, blue_team, this);
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

		int id = next_dumpster_id;
		GameObject::GameObjectData data = { x, z, rot };
		Resource* d = new Resource(DUMPSTER_TYPE, data, id, this);
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

		int id = next_recycling_bin_id;
		GameObject::GameObjectData data = { x, z, rot };
		Resource* r = new Resource(RECYCLING_BIN_TYPE, data, id, this);
		idMap[id] = r;

		std::cout << "created new recycling bin: " << id << " at " << idMap[id] << "\n";

		next_recycling_bin_id++;
		if (next_recycling_bin_id == ID_RECYCLING_BIN_MAX) next_recycling_bin_id = ID_RECYCLING_BIN_MIN;
	}
}

void SceneManager_Server::testAttacking() {
	int id;
	GameObject::GameObjectData data;

	/*id = next_minion_id;
	next_minion_id++;
	data = { 10, 5, 0 };
	Minion* m1 = new Minion(data, id, red_team, this);
	idMap[id] = m1;
	std::cout << "created minion at id " << id << "\n";

	id = next_super_minion_id;
	next_super_minion_id++;
	data = { 14, 5, 0 };
	SuperMinion* sm1 = new SuperMinion(data, id, blue_team, this);
	idMap[id] = sm1;
	std::cout << "created super minion at id " << id << "\n";*/

	id = next_super_minion_id;
	next_super_minion_id++;
	data = { -10, 10, 0 };
	SuperMinion* sm2 = new SuperMinion(data, id, red_team, this);
	idMap[id] = sm2;

	id = next_claw_id;
	next_claw_id++;
	data = { 7.5, 27.5, 0 };
	ClawTower* c1 = new ClawTower(data, id, red_team, this);
	idMap[id] = c1;

	id = next_laser_id;
	next_laser_id++;
	data = { 0, 10, 0 };
	LaserTower* l1 = new LaserTower(data, id, blue_team, this);
	idMap[id] = l1;

	id = next_laser_id;
	next_laser_id++;
	data = { 0, 20, 0 };
	LaserTower* l2 = new LaserTower(data, id, red_team, this);
	idMap[id] = l2;

	/*id = next_minion_id;
	next_minion_id++;
	data = { -10, 10, 0 };
	Minion* m2 = new Minion(data, id, red_team, this);
	idMap[id] = m2;*/

	id = next_dumpster_id;
	next_dumpster_id++;
	data = { 10, 10, 0 };
	Resource* d1 = new Resource(DUMPSTER_TYPE, data, id, this);
	idMap[id] = d1;

	id = next_recycling_bin_id;
	next_recycling_bin_id++;
	data = { 0, -10, 0 };
	Resource* r1 = new Resource(RECYCLING_BIN_TYPE, data, id, this);
	idMap[id] = r1;
}

void SceneManager_Server::testBuilding() {
	new BuildNode(RED_TEAM, 0, 0);
}

void SceneManager_Server::testWalls() {
	new WallNode(-0.5f, -0.5f);
}

/***** legacy code *****/
/*void SceneManager_Server::resetClocks() {
	for (std::pair<std::string, Entity*> idEntPair : idMap) {
		idEntPair.second->resetClock();
	}
}*/