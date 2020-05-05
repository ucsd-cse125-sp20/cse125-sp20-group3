#ifndef _SCENE_MANAGER_S_H_
#define _SCENE_MANAGER_S_H_

#include <map>
#include <string>
#include "../common/entity.h"
#include "../common/client2server.h"
#include "../common/macros.h"
#include "../common/player.h"
#include "../common/team.h"
#include "../common/base.h"
#include "../common/minion.h"
#include "../common/tower.h"
//#include "../common/resource.h"

class SceneManager_Server {
private:
		std::map<std::string, Entity*> idMap;
		int next_player_id, next_base_id, next_minion_id, next_tower_id, next_resource_id;
		//Team *team1, *team2;

public:
	SceneManager_Server();
	void processInput(std::string player, PlayerInput in);
	bool addPlayer(std::string player_id);
	void spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y);
	void resetClocks();
	void update();
	int encodeScene(char buf[]);

	void populateScene();
};

#endif