#ifndef _SCENE_MANAGER_S_H_
#define _SCENE_MANAGER_S_H_

#include <map>
#include <string>
#include "../common/entity.h"
#include "../common/client2server.h"
#include "../common/macros.h"
#include "../common/player.h"
#include "../common/team.h"

class SceneManager_Server {
private:
		std::map<std::string, Entity*> idMap;
		int next_player_id, next_base_id, next_minion_id, next_tower_id, next_resource_id;
		Team *team1, *team2;

		//hardcode
		Player* p;

public:
	SceneManager_Server();
	void processInput(std::string player, PlayerInput in);
	void spawnEntity(char spawnType, float pos_x, float pos_z, float y_rot);
	void update();
	int encodeScene(char buf[]);
};

#endif