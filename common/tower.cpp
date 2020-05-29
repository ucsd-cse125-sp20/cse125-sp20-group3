#include "tower.h"
#include "../server/SceneManager_Server.h"

Tower::Tower(GameObjectData data, int id, int health, int attack, Team* t, SceneManager_Server* sm) : Entity(data, id, health, attack, t, sm) {
	if (sm != nullptr) { //only execute on server
		t->incTower();
	}
}

void Tower::takeDamage(int damage) {
	Entity::takeDamage(damage);
	std::cout << "tower: " << id << " took " << damage << " damage | remaining health: " << health << "\n";
	if (health <= 0) {
		std::cout << "tower " << id << " dying\n";
		team->decTower(); 
		ObjectDetection::removeObject(this);
	}
}