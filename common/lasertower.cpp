#include "lasertower.h"
#include "../server/SceneManager_Server.h"

LaserTower::LaserTower(std::string id, SceneManager_Server* sm_server) : Tower(id, LASER_TOWER_HEALTH, LASER_TOWER_ATTACK, sm_server) {
	//init stuff
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = LASER_FIRE_RANGE;
	attackInterval = LASER_FIRE_INTERVAL; //interval between firing at attack target
}

void LaserTower::update(float deltaTime) { //should they be able to switch attack targets instantaneously?
	if (attackTarget == NULL || !manager->checkEntityAlive(attackTarget->getIDstr()) /*|| attackTarget->getHealth() > 0*/) {
		//look for new attack target within attackRange
		//if none found in range, set attackTarget = NULL
	}

	if (attackTarget != NULL) {
		timeElapsed += deltaTime;
		if (timeElapsed >= attackInterval) {
			//attack attackTarget
			timeElapsed = 0;
		}
	}
}
