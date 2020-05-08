#include "minion.h"

Minion::Minion(int health, int attack) : Entity(health, attack) {
	//init stuff
}

Minion::Minion(int health, int attack, mat4 model_mat) : Entity(health, attack, model_mat) {
	//TODO: Initialize attackTarget?
}

void Minion::update(float deltaTime) {
	//TODO: update attackTarget
	//TODO: update health
}
