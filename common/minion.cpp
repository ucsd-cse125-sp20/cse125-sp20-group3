#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(SceneManager_Server* sm) : Entity(MINION_HEALTH, MINION_ATTACK, sm) {
	//init stuff
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;

	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
}

Minion::Minion(int health, int attack, float range, SceneManager_Server* sm) : Entity(health, attack, sm) {
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = range;
	attackInterval = MINION_ATK_INTERVAL;

	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
}

/*Minion::Minion(int health, int attack, float range, SceneManager_Server* sm, mat4 model_mat) : Entity(health, attack, sm, model_mat) {
	attackTarget = NULL;
	attackRange = range;
	timeElapsed = 0;
	actionInterval = MINION_ATK_INTERVAL;

	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
}*/

void Minion::update(float deltaTime) {
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

void Minion::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decUnit();
}
