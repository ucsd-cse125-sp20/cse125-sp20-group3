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
#include "../common/pathNode.h"
#include "../common/buildNode.h"
#include "../common/wallNode.h"


class SceneManager_Server {
private:
		
		int next_player_id, next_base_id, next_minion_id, next_super_minion_id, next_laser_id, next_claw_id, next_dumpster_id, next_recycling_bin_id;
		Team *red_team, *blue_team;

public:
	std::map<int, Entity*> idMap;
	std::vector<pathNode*> pathNodes;
	std::vector<wallNode*> wallNodes;
	std::vector<buildNode*> buildNodes;

	SceneManager_Server();
	~SceneManager_Server();
	void processInput(int player_id, PlayerInput in);
	bool addPlayer(int player_id);
	void spawnEntity(char spawnType, float pos_x, float pos_z, float rot_y, Team* t);
	bool checkEntityAlive(int id);
	void update(float deltaTime);
	int encodeState(char buf[], int start_index);
	int encodeScene(char buf[], int start_index);

	void populatePaths();
	void populateWalls();
	void populateBuilds();

	void populateScene();
	void testAttacking();
};

#endif