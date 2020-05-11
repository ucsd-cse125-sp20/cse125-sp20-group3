#include "minion.h"
#include "../server/SceneManager_Server.h"

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
