#include "minion.h"
#include "../server/SceneManager_Server.h"

map<tuple<float, float>, vector<tuple<float, float>>> Minion::pathMap;

Minion::Minion(string id, SceneManager_Server* sm) : Entity(id, MINION_HEALTH, MINION_ATTACK, sm) {
	//init stuff
	pathPtr = 0;
	//path initialization
	tuple<float, float> bornLoc = make_tuple(model[3][0], model[3][2]);
	path = pathMap[bornLoc];
	timeElapsed = 0;
	attackTarget = NULL;
	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
}

Minion::Minion(string id, int health, int attack, float range, SceneManager_Server* sm) : Entity(id, health, attack, sm) {
	timeElapsed = 0;
	pathPtr = 0;
	//path initialization
	tuple<float, float> bornLoc = make_tuple(model[3][0], model[3][2]);
	path = pathMap[bornLoc];
	attackTarget = NULL;
	attackRange = range;
	attackInterval = MINION_ATK_INTERVAL;
	ObjectDetection::addObject(this, DETECTION_FLAG_MINION | DETECTION_FLAG_ENTITY);
}

void Minion::update(float deltaTime) { //should they be able to switch attack targets instantaneously?
	timeElapsed += deltaTime;
	if (timeElapsed >= attackInterval) {
		this->attack();
		timeElapsed = 0;
	}
	else {
		this->move();
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

void Minion::move() {
	/*if (pathPtr < path.size) {
		tuple<float, float> nextPos = path[pathPtr];
		float lastXPos = model[3][0];
		float lastZPos = model[3][2];
		model[3][0] += (get<0>(nextPos) - lastXPos) * deltaTime;
		model[3][2] += (get<1>(nextPos) - lastZPos) * deltaTime;
		vec3 forward = normalize(vec3(model[3][0]-lastXPos, 0, model[3][2]-lastZPos));
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);
		pathPtr++;
	}*/
}
