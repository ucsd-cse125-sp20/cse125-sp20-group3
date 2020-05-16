#include "clawtower.h"
#include "../server/SceneManager_Server.h"

ClawTower::ClawTower(std::string id, SceneManager_Server* sm_server) : Tower(id, CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, sm_server) {
    //init stuff
	timeElapsed = 0;
	spawnOffset = vec3(2.f, 0, 2.f);
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns
}

void ClawTower::update(float deltaTime) {
	/*timeElapsed += deltaTime;
	if (timeElapsed >= actionInterval) {
		manager->spawnEntity(MINION_TYPE, model[3][0] + spawnOffset.getX(), model[3][2] + spawnOffset.getZ(), 0);
		timeElapsed = 0;
	}*/
}
