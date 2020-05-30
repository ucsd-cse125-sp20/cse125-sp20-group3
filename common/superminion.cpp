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