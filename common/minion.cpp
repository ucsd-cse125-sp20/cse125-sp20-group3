#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(SceneManager_Server* sm) : Entity(MINION_HEALTH, MINION_ATTACK, sm) {
	pathPtr = 0;
}

Minion::Minion(SceneManager_Server* sm, mat4 model_mat) : Entity(MINION_HEALTH, MINION_ATTACK, sm, model_mat) {
	pathPtr = 0;
}

/*

Minion::Minion(int health, int attack, SceneManager_Server* sm) : Entity(health, attack, sm) {
	//init stuff
}

Minion::Minion(int health, int attack, SceneManager_Server* sm, mat4 model_mat) : Entity(health, attack, sm, model_mat) {
	//TODO: Initialize attackTarget?
}

*/

void Minion::update(float deltaTime) {
	timeElapsed += deltaTime; 
	if (timeElapsed >= actionInterval) {
		this->attack(MINION_ATTACK_RANGE);
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

void Minion::move() {
	/*
	if (pathPtr < path.size) {
		tuple<float, float> nextPos = path[pathPtr];
		lastXPos = model[3][0];
		lastZPos = model[3][2];
		model[3][0] = get<0>(nextPos);
		model[3][2] = get<1>(nextPos);
		vec3 forward = normalize(vec3(model[3][0]-lastXPos, 0, model[3][2]-lastZPos));
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);
		pathPtr++;
	}
	*/
}
