#include "tower.h"
#include "../server/SceneManager_Server.h"

Tower::Tower(int health, int attack, SceneManager_Server* sm) : Entity(health, attack, sm) {
	ObjectDetection::addObject(this, DETECTION_FLAG_TOWER | DETECTION_FLAG_ENTITY);
}

void Tower::setHealth(int new_health) {
	Entity::setHealth(new_health);
	if (health <= 0) team->decTower();
}