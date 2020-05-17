#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(string id, SceneManager_Server* sm) : Entity(id, MINION_HEALTH, MINION_ATTACK, sm) {
	//init stuff
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
	curNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, MINION_MV_RANGE);
}

Minion::Minion(string id, int health, int attack, float range, SceneManager_Server* sm) : Entity(id, health, attack, sm) {
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = range;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
	curNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, MINION_MV_RANGE);
}

void Minion::update(float deltaTime) { //should they be able to switch attack targets instantaneously?
	timeElapsed += deltaTime;
	if (timeElapsed >= attackInterval) {
		this->attack();
		timeElapsed = 0;
	}
	else {
		this->move(deltaTime);
	}
}

void Minion::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decUnit();
}

void Minion::attack() {
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

void Minion::move(float deltaTime) {
	float lastXPos = model[3][0];
	float lastZPos = model[3][2];
	GameObjectData curNodeData = curNode->getData();
	float curXPos = curNodeData.x;
	float curZPos = curNodeData.z;
	if (curXPos == lastXPos && curZPos == lastZPos) {
		mapNode* nextPtr;
		if (this->team->teamColor == RED_TEAM) nextPtr = this->curNode->next_red;
		else nextPtr = this->curNode->next_blue;
		if (nextPtr != nullptr) {
			//recalculating velocity
			GameObjectData nextNodeData = nextPtr->getData();
			float nextXPos = nextNodeData.x;
			float nextZPos = nextNodeData.z;
			velocity_x = nextXPos - curXPos;
			velocity_z = nextZPos - curZPos;
			if (velocity_x != 0) {
				if (velocity_x > 0) velocity_x = MINION_VELOCITY;
				else velocity_x = -1 * MINION_VELOCITY;
			}
			else {
				if (velocity_z > 0) velocity_z = MINION_VELOCITY;
				else velocity_z = -1 * MINION_VELOCITY;
			}
		}
		else {
			velocity_x = 0;
			velocity_z = 0;
		}
		curNode = nextPtr;
	}
	model[3][0] += velocity_x*deltaTime + model[3][0];
	model[3][2] += velocity_z*deltaTime + model[3][2];
	vec3 forward = normalize(vec3(model[3][0]-lastXPos, 0, model[3][2]-lastZPos));
	vec3 right = cross(forward, vec3(0, 1, 0));
	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);
}
