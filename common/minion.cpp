#include "minion.h"

Minion::Minion(mat4 model_mat, std::string objFilename) : GameObject(model_mat) {
	Minion::Minion(model_mat);
}

Minion::Minion(mat4 model_mat) : GameObject(model_mat) {
	//TODO: Initialize attackTarget?
	health = 100;
	lastTime = std::chrono::steady_clock::now();
}

void Unit::update() {
	//TODO: update attackTarget
	//TODO: update health
}
