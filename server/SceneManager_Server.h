#ifndef _SCENE_MANAGER_S_H_
#define _SCENE_MANAGER_S_H_

#include <map>
#include <string>
#include <vector>
#include "../common/entity.h"
#include "../common/client2server.h"
#include "../common/macros.h"
#include "../common/player.h"
#include "../common/team.h"
#include "../common/base.h"
#include "../common/minion.h"
#include "../common/superminion.h"
#include "../common/tower.h"
#include "../common/lasertower.h"
#include "../common/clawtower.h"
#include "../common/resource.h"

#include "../common/ObjectDetection.h"
#include "../common/mapNode.h"

class SceneManager_Server {
private:
		
		int next_player_id, next_base_id, next_minion_id, next_super_minion_id, next_laser_id, next_claw_id, next_dumpster_id, next_recycling_bin_id;
		Team *red_team, *blue_team;

public:
	std::map<std::string, Entity*> idMap;
	std::vector<mapNode*> *map;

	SceneManager_Server();
	void processInput(std::string player, PlayerInput in);
	bool addPlayer(std::string player_id);
	void spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y, Team* t);
	bool checkEntityAlive(std::string id);
	void update(float deltaTime);
	int encodeState(char buf[], int start_index);
	int encodeScene(char buf[], int start_index);

	void populateScene();
	void testAttacking();

	void populateMap();
};

#endif