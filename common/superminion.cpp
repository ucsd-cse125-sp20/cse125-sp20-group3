#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Minion(data, id, SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, SUPER_MINION_ATK_INTERVAL, SUPER_MINION_VELOCITY, t, sm) {
	if (sm != nullptr) {
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION |
			DETECTION_FLAG_MINION_TARGET | DETECTION_FLAG_LASER_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, -SUPER_MINION_WIDTH, SUPER_MINION_WIDTH, -SUPER_MINION_LENGTH, SUPER_MINION_LENGTH);
	}
}

void SuperMinion::dropPickups() {
	srand((unsigned int)time(NULL));
	vec3 pos = this->getPosition();
	if (rand() % SUPER_MINION_IRON_DROP_CHANCE == 0) {
		float x = pos.getX() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		float z = pos.getZ() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		manager->spawnEntity(IRON_TYPE, x, z, 0, nullptr);
	}
	if (rand() % SUPER_MINION_BOTTLE_DROP_CHANCE == 0) {
		float x = pos.getX() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		float z = pos.getZ() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		manager->spawnEntity(BOTTLE_TYPE, x, z, 0, nullptr);
	}
}