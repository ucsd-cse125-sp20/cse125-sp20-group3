#include "lasertower.h"
#include "../server/SceneManager_Server.h"

LaserTower::LaserTower(SceneManager_Server* sm_server) : Tower(LASER_TOWER_HEALTH, LASER_TOWER_ATTACK, sm_server) {
	//init stuff
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = LASER_FIRE_RANGE;
	attackInterval = LASER_FIRE_INTERVAL; //interval between firing at attack target
}

void LaserTower::update(float deltaTime) {
	/*if (!manager->checkEntityAlive(attackTarget.getID())) {
		//look for new attack target within attackRange
		//if none found in range, set attackTarget = NULL
	}*/

	if (attackTarget != NULL) {
		timeElapsed += deltaTime;
		if (timeElapsed >= attackInterval) {
			//attack attackTarget
			timeElapsed = 0;
		}
	}
}
