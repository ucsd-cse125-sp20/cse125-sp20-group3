#include "tower.h"

Tower::Tower(int health, int attack) : Entity(health, attack) {
	//init stuff
}

Tower::Tower(int health, int attack, mat4 model_mat) : Entity(health, attack, model_mat) {
	//init stuff
}

void Tower::update(float deltaTime) {
	//TODO: Update health
}