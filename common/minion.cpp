#include "minion.h"

Minion::Minion(int health, int attack) : Entity(health, attack) {
	//init stuff
}

Minion::Minion(int health, int attack, mat4 model_mat) : Entity(health, attack, model_mat) {
	//TODO: Initialize attackTarget?
	lastTime = std::chrono::steady_clock::now();
}

void Minion::update() {
	//TODO: update attackTarget
	//TODO: update health
}
