#include "base.h"
#include "../server/SceneManager_Server.h"

Base::Base(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Entity(data, id, BASE_HEALTH, BASE_ATTACK, t, sm) {
	if (sm != nullptr) {
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, -BASE_WIDTH, BASE_WIDTH, -BASE_LENGTH, BASE_LENGTH);
	}
}

void Base::update(float deltaTime) {
	this->team->setBaseHealth(this->getHealth());
}
