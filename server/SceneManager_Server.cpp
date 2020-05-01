#include "SceneManager_Server.h"

SceneManager_Server::SceneManager_Server() {
	next_player_id = ID_PLAYER_MIN;
	next_base_id = ID_BASE_MIN;
	next_minion_id = ID_MINION_MIN;
	next_tower_id = ID_TOWER_MIN;
	next_resource_id = ID_RESOURCE_MIN;

	//team1 = new Team();
	//team2 = new Team();

	//hardcode
	p = new Player();
}

void SceneManager_Server::processInput(std::string player, PlayerInput in) {
	//((Player*)idMap[player])->setMoveAndDir(in);
	std::cout << "processing input for player " << player << ": " << in.move_x << " " << in.move_z << " " << in.view_y_rot << "\n";

	//hardcode
	p->setMoveAndDir(in);
}

void SceneManager_Server::spawnEntity(char spawnType, float pos_x, float pos_z, float y_rot) {

}

void SceneManager_Server::update() {
	for (std::pair<std::string, Entity*> idEntPair: idMap) {
		idEntPair.second->update();
	}
}

int SceneManager_Server::encodeScene(char buf[]) {
	/* buf structure
	 * ID,{GameObjectData},health,
	 * ID,{GameObjectData},health,
	 * ...
	 */

	/*int i = 0;
	for (std::pair<std::string, Entity*> idEntPair : idMap) { //iterate through all entities in scene
		strncpy(buf + i, idEntPair.first.c_str(), idEntPair.first.length); //write id bytes without null term
		i += idEntPair.first.length;

		buf[i] = DELIMITER; //write delimiter
		i++;

		i += idEntPair.second->setData(buf, i); //write GameObjectData at i, increase i by number of bytes written

		buf[i] = DELIMITER; //write delimiter
		i++;

		((int*)buf)[i] = idEntPair.second->getHealth(); //write entity's health
		i += sizeof(int);

		buf[i] = DELIMITER; //write delimiter
		i++;
	}*/

	p->setData(buf, 0);
	return sizeof(GameObject::GameObjectData);
}