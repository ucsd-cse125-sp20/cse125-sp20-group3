#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(string id, SceneManager_Server* sm) : Entity(id, MINION_HEALTH, MINION_ATTACK, sm) {
	//init stuff
	timeElapsed = 0;
	attackTarget = nullptr;
	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY); //TODO do team flag stuff
	destNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, 50); //TODO unbounded radius check
}

Minion::Minion(string id, int health, int attack, int range, SceneManager_Server* sm) : Entity(id, health, attack, sm) {
	timeElapsed = 0;
	attackTarget = nullptr;
	attackRange = range;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
	destNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, MINION_MV_RANGE);
}

void Minion::update(float deltaTime) { //should they be able to switch attack targets instantaneously?
	timeElapsed += deltaTime; //increase elapsedTime

	if (attackTarget != nullptr && !manager->checkEntityAlive(attackTarget->getIDstr())) //first, if there was an attack target, check if it's still alive
			attackTarget = nullptr; //do this check here instead of after attacking in the case of multiple entities targeting one entity

	if (this->attackTarget == nullptr) //next, if not currently targeting something, check if there is a valid enemy in range
			attackTarget = (Entity*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_TOWER, attackRange);

	if (attackTarget != nullptr) { //if this minion should be attacking something, don't move
		if (timeElapsed >= attackInterval) { //only attack on an interval
			this->attack();
			timeElapsed = 0;
		}
	}
	else { //no attack target after all checks, move
		this->move(deltaTime);
	}
}

void Minion::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decUnit(); //TODO move this into an overriden takeDamage
}

void Minion::attack() {
	attackTarget->takeDamage(this->attackDamage);
	//TODO manipulate necessary data to spawn particle systems
}

void Minion::move(float deltaTime) {
	float remaining_move_dist = MINION_VELOCITY * deltaTime; //full movement distance this minion travels this tick

	while (remaining_move_dist > 0) { //while this minion still has distance to move
		vec3 dest_vec = destNode->getPosition() - this->getPosition(); //vector to reach destNode
		float dest_dist = length(dest_vec); //distance between this and destNode
		float move_dist = std::min(remaining_move_dist, dest_dist); //this iteration, move full move_dist, or only to destNode if closer

		vec3 move_vec = normalize(dest_vec) * move_dist; //move calculated distance along vector to destination

		float lastXPos = model[3][0];
		float lastZPos = model[3][2];
		model[3][0] += move_vec[0];
		model[3][2] += move_vec[2];
		remaining_move_dist -= move_dist; //decrease remaining movement distance
		vec3 forward = normalize(vec3(model[3][0] - lastXPos, 0, model[3][2] - lastZPos)); //TODO check vectors?
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);

		if (this->getPosition() == destNode->getPosition()) { //reached destNode with this iteration
			mapNode* nextNode; //continue moving to the next node
			if (true/*this->team->teamColor == RED_TEAM*/) nextNode = this->destNode->next_red; //TODO do team stuff
			else nextNode = this->destNode->next_blue;

			if (nextNode != nullptr) this->destNode = nextNode; 
			else {
				std::cout << "minion reached the end of the path!\n";
				break;
			}
		}
	}
}
