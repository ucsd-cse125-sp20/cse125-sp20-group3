#include "clawtower.h"
#include "../server/SceneManager_Server.h"

ClawTower::ClawTower(int id, Team* t, SceneManager_Server* sm_server) : Tower(id, CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, t, sm_server) {
	actionState = CLAW_ACTION_IDLE;

	spawnPoint = this->getPosition() + vec3(2.f, 0, 2.f); //TODO find nearest path location and use as spawn point
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns
}

void ClawTower::update(float deltaTime) {
	timeElapsed += deltaTime;
	actionState = CLAW_ACTION_SPAWN;

	if (timeElapsed >= spawnInterval) {
		std::cout << "claw " << id << " spawning at x: " << spawnPoint.getX() << " z: " << spawnPoint.getZ() << "\n";
		manager->spawnEntity(MINION_TYPE, spawnPoint[0], spawnPoint[2], 0, this->team);
		timeElapsed = 0;
	}

	ObjectDetection::updateObject(this);
}

void ClawTower::setEntData(EntityData data) {
	Entity::setEntData(data);
}