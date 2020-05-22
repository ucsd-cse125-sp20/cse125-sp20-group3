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

	if (attackTarget != nullptr &&																//first, if targeting something
			(!manager->checkEntityAlive(attackTargetID) ||										//but either target is dead
			length(attackTarget->getPosition() - this->getPosition()) > this->attackRange)) {	//or target is out of range, null out ptr
		std::cout << "laser " << id_str << " nulling out attackTarget\n";
		attackTarget = nullptr; //do this check here instead of after attacking in the case of multiple entities targeting one entity
	}

	if (this->attackTarget == nullptr) { //next, if not currently targeting something, check if there is a valid enemy in range
		int flags = DETECTION_FLAG_LASER_TARGET;
		if (this->team->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_BLUE_TEAM;
		else flags = flags | DETECTION_FLAG_RED_TEAM;
		attackTarget = (Entity*)ObjectDetection::getNearestObject(this, flags, attackRange);

		if (attackTarget != nullptr && length(attackTarget->getPosition() - this->getPosition()) > this->attackRange) {
			attackTarget = nullptr; //if target isn't actually in range, ignore it
		} //object detection doesn't strictly follow distance, based on hashed block sections

		if (attackTarget != nullptr) {
			attackTargetID = attackTarget->getIDstr();
			timeElapsed = 0; //reset attack timer on acquiring new target
			std::cout << "laser " << id_str << " found target " << attackTarget->getIDstr() << "\n";
		}
	}

	if (attackTarget != nullptr) {
		timeElapsed += deltaTime; //increase timeElapsed

		if (timeElapsed >= attackInterval) { //only attack on an interval
			std::cout << "laser " << id_str << " attacking " << attackTargetID << "\n";
			this->attack();
			timeElapsed = 0;
		}
	}
}

void LaserTower::attack() {
	attackTarget->takeDamage(this->attackDamage);
	//TODO manipulate necessary data to spawn particle systems
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void LaserTower::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getIDstr(); }