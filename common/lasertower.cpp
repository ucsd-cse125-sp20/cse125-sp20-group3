#include "lasertower.h"
#include "../server/SceneManager_Server.h"

LaserTower::LaserTower(int id, Team* t, SceneManager_Server* sm_server) : Tower(id, LASER_TOWER_HEALTH, LASER_TOWER_ATTACK, t, sm_server) {
	actionState = LASER_ACTION_IDLE;
	
	attackRange = LASER_FIRE_RANGE;
	attackInterval = LASER_FIRE_INTERVAL; //interval between firing at attack target
}

void LaserTower::update(float deltaTime) { //should they be able to switch attack targets instantaneously?

	if (attackTarget != nullptr &&																//first, if targeting something
			(!manager->checkEntityAlive(attackTargetID) ||										//but either target is dead
			length(attackTarget->getPosition() - this->getPosition()) > this->attackRange)) {	//or target is out of range, null out ptr
		std::cout << "laser " << id << " nulling out attackTarget\n";
		attackTarget = nullptr; //do this check here instead of after attacking in the case of multiple entities targeting one entity
	}

	if (this->attackTarget == nullptr) { //next, if not currently targeting something, check if there is a valid enemy in range
		int flags = DETECTION_FLAG_LASER_TARGET;
		if (this->team->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_BLUE_TEAM;
		else flags = flags | DETECTION_FLAG_RED_TEAM;
		attackTarget = (Entity*)ObjectDetection::getNearestObject(this, flags, attackRange);

		if (attackTarget != nullptr && length(attackTarget->getPosition() - this->getPosition()) > this->attackRange) {
			attackTarget = nullptr; //if target isn't actually in range, ignore it
			//std::cout << "laser " << id_str << " ignoring target out of range\n";
		} //object detection doesn't strictly follow distance, based on hashed block sections

		if (attackTarget != nullptr) {
			attackTargetID = attackTarget->getID();
			timeElapsed = 0; //reset attack timer on acquiring new target
			std::cout << "laser " << id << " found target " << attackTarget->getID() << "\n";
		}
	}

	if (attackTarget != nullptr) {
		timeElapsed += deltaTime; //increase timeElapsed
		actionState = LASER_ACTION_ATTACK;

		if (timeElapsed >= attackInterval) { //only attack on an interval
			std::cout << "laser " << id << " attacking " << attackTargetID << "\n";
			this->attack();
			timeElapsed = 0;
		}
	}
	else actionState = LASER_ACTION_IDLE;

	ObjectDetection::updateObject(this);
}

void LaserTower::attack() {
	attackTarget->takeDamage(this->attackDamage);
	actionState = LASER_ACTION_FIRE;
	//TODO manipulate necessary data to spawn particle systems
}

void LaserTower::setEntData(EntityData data) {
	Entity::setEntData(data);
	//std::cout << "laser " << id << " targetID: " << attackTargetID << "\n";
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void LaserTower::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getID(); }