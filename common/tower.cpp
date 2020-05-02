#include "tower.h"

Tower::Tower(mat4 model_mat, std::string objFilename) : GameObject(model_mat) {
	Tower::Tower(model_mat);
}

Tower::Tower(mat4 model_mat) : GameObject(model_mat) {
	health = 100;
	lastTime = std::chrono::steady_clock::now();
}

void Tower::update() {
	//TODO: Update health
}