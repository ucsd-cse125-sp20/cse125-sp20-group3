#include "tower.h"
#include "../server/SceneManager_Server.h"

Tower::Tower(int health, int attack, SceneManager_Server* sm) : Entity(health, attack, sm) {
	//init stuff
}

Tower::Tower(int health, int attack, SceneManager_Server* sm, mat4 model_mat) : Entity(health, attack, sm, model_mat) {
	//init stuff
}

void Tower::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decTower();
}