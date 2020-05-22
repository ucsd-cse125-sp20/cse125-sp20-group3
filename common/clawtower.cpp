#include "clawtower.h"
#include "../server/SceneManager_Server.h"

ClawTower::ClawTower(std::string id, Team* t, SceneManager_Server* sm_server) : Tower(id, CLAW_TOWER_HEALTH, CLAW_TOWER_ATTACK, t, sm_server) {
    //init stuff
	timeElapsed = 0;
	spawnPoint = this->getPosition() + vec3(2.f, 0, 2.f); //TODO find nearest path location and use as spawn point
	spawnInterval = CLAW_SPAWN_INTERVAL; //interval between minion spawns
}

void ClawTower::update(float deltaTime) {
	timeElapsed += deltaTime;
	if (timeElapsed >= spawnInterval) {
		std::cout << "claw " << id_str << " spawning at x: " << spawnPoint.getX() << " z: " << spawnPoint.getZ() << "\n";
		manager->spawnEntity(MINION_TYPE, spawnPoint[0], spawnPoint[2], 0, this->team);
		timeElapsed = 0;
	}

	ObjectDetection::updateObject(this);
}
