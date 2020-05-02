#include "unit.h"

Unit::Unit(mat4 model_mat, std::string objFilename) : GameObject(model_mat) {
	Unit::Unit(model_mat);
}

Unit::Unit(mat4 model_mat) : GameObject(model_mat) {
	//TODO: Initialize attackTarget?
	health = 100;
	lastTime = std::chrono::steady_clock::now();
}

void Unit::update() {
	//TODO: update attackTarget
	//TODO: update health
}
