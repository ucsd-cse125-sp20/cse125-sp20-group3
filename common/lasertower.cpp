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
	timeElapsed += deltaTime;
	if (timeElapsed >= attackInterval) {
		this->attack();
		timeElapsed = 0;
	}
}

void LaserTower::attack() {
	if (this->attackTarget == nullptr) {
		attackTarget = (Entity*)ObjectDetection::getNearestObject(this, 1, attackRange);
		if (this->isEnemyTeam(attackTarget->team) == false || !manager->checkEntityAlive(attackTarget->getIDstr())) attackTarget = nullptr; 
	}
	if (attackTarget != nullptr) {
		attackTarget->takeDamage(this->attackDamage);
		int enemyHealth = attackTarget->getHealth();
		if (!manager->checkEntityAlive(attackTarget->getIDstr())) attackTarget = nullptr;
	}	
}
