#include "clawtower.h"
#include "../server/SceneManager_Server.h"

ClawTower::ClawTower(SceneManager_Server* sm_server) : Tower(CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, sm_server) {
    //init stuff
	spawnOffset = vec3(2.f, 0, 2.f);
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns
	timeElapsed = 0;
}

ClawTower::ClawTower(SceneManager_Server* sm_server, mat4 model_mat) : Tower(CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, sm_server, model_mat) {
	//init stuff
	spawnOffset = vec3(2.f, 0, 2.f);
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns
	timeElapsed = 0;
}

void ClawTower::update(float deltaTime) {
	/*timeElapsed += deltaTime; commented out for testing
	if (timeElapsed >= spawnInterval) {
		manager->spawnEntity(MINION_TYPE, model[3][0] + spawnOffset.getX, model[3][2] + spawnOffset.getZ, 0);
		timeElapsed = 0;
	}*/
}
