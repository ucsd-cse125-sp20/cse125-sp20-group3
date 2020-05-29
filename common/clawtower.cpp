#include "clawtower.h"
#include "../server/SceneManager_Server.h"

ClawTower::ClawTower(GameObjectData data, int id, Team* t, SceneManager_Server* sm_server) : Tower(data, id, CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, t, sm_server) {
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns

	if (sm_server != nullptr) {
		std::cout << "x: " << this->getPosition().getX() << " z: " << this->getPosition().getZ() << "\n";

		GameObject* nearPathNode = ObjectDetection::getNearestObject(this, DETECTION_FLAG_PATH_NODE, 0);
		if (nearPathNode == nullptr) std::cout << "claw tower " << id << " couldn't find a nearby path!\n";
		else std::cout << "claw tower " << id << " using x: " << nearPathNode->getPosition().getX() << " z: " << nearPathNode->getPosition().getZ() << " as spawnPoint\n";
		spawnPoint = nearPathNode->getPosition();

		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_TOWER |
			DETECTION_FLAG_MINION_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, -CLAW_WIDTH, CLAW_WIDTH, -CLAW_LENGTH, CLAW_LENGTH);
	}
}

void ClawTower::update(float deltaTime) {
	timeElapsed += deltaTime;

	if (timeElapsed >= spawnInterval) {
		std::cout << "claw " << id << " spawning at x: " << spawnPoint.getX() << " z: " << spawnPoint.getZ() << "\n";
		manager->spawnEntity(MINION_TYPE, spawnPoint[0], spawnPoint[2], 0, this->team);
		timeElapsed = 0;
	}

	//ObjectDetection::updateObject(this);
}

void ClawTower::setEntData(EntityData data) {
	Entity::setEntData(data);
}