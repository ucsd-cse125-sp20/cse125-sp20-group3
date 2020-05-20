#include "tower.h"
#include "../server/SceneManager_Server.h"

Tower::Tower(std::string id, int health, int attack, Team* t, SceneManager_Server* sm) : Entity(id, health, attack, t, sm) {
	if (sm != nullptr) { //only execute on server
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_TOWER |
					DETECTION_FLAG_MINION_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags);
	}
}

void Tower::takeDamage(int damage) {
	Entity::takeDamage(damage);
	std::cout << "tower: " << id_str << " took " << damage << " damage | remaining health: " << health << "\n";
	if (health <= 0) { team->decTower(); std::cout << "i die\n"; }
}