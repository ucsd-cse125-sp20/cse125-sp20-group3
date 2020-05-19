#include "lasertower.h"
#include "../server/SceneManager_Server.h"

LaserTower::LaserTower(std::string id, Team* t, SceneManager_Server* sm_server) : Tower(id, LASER_TOWER_HEALTH, LASER_TOWER_ATTACK, t, sm_server) {
	//init stuff
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = LASER_FIRE_RANGE;
	attackInterval = LASER_FIRE_INTERVAL; //interval between firing at attack target
}

void LaserTower::update(float deltaTime) { //should they be able to switch attack targets instantaneously?
	timeElapsed += deltaTime; //increase elapsedTime

	if (attackTarget == nullptr ||																//first, if not targeting anything
			!manager->checkEntityAlive(attackTargetID) ||										//or target is dead
			length(attackTarget->getPosition() - this->getPosition()) > this->attackRange) {	//or target is out of range, null out ptr
		attackTarget = nullptr; //do this check here instead of after attacking in the case of multiple entities targeting one entity
	}

	if (this->attackTarget == nullptr) { //next, if not currently targeting something, check if there is a valid enemy in range
		int flags = DETECTION_FLAG_MINION; //TODO set flags according to team; LASER TOWERS CAN ONLY TARGET MINIONS
		attackTarget = (Entity*)ObjectDetection::getNearestObject(this, flags, attackRange);
		if (attackTarget != nullptr) attackTargetID = attackTarget->getIDstr();
	}

	if (attackTarget != nullptr && timeElapsed >= attackInterval) { //only attack on an interval
		std::cout << "tower: " << this << " attacking that " << attackTarget << "\n";
		this->attack();
		timeElapsed = 0;
	}
}

void LaserTower::attack() {
	attackTarget->takeDamage(this->attackDamage);
	//TODO manipulate necessary data to spawn particle systems
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void LaserTower::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getIDstr(); }