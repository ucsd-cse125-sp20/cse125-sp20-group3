#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(SceneManager_Server* sm) : Entity(MINION_HEALTH, MINION_ATTACK, sm) {
	//init stuff
}

Minion::Minion(SceneManager_Server* sm, mat4 model_mat) : Entity(MINION_HEALTH, MINION_ATTACK, sm, model_mat) {
	//init stuff
}

Minion::Minion(int health, int attack, SceneManager_Server* sm) : Entity(health, attack, sm) {
	//init stuff
}

Minion::Minion(int health, int attack, SceneManager_Server* sm, mat4 model_mat) : Entity(health, attack, sm, model_mat) {
	//TODO: Initialize attackTarget?
}

void Minion::update(float deltaTime) {
	//TODO: update attackTarget
	//TODO: update health
}

void Minion::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decUnit();
}
