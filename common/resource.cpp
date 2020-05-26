#include "resource.h"
#include "../server/SceneManager_Server.h"

Resource::Resource(char resourceType, GameObjectData data, int id, SceneManager_Server* sm_server) : Entity(data, id, RESOURCE_HEALTH, RESOURCE_ATTACK, nullptr, sm_server) {
	type = resourceType;
	active = true;

	if (sm_server != nullptr) { //only execute on server
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_RESOURCE;
		ObjectDetection::addObject(this, flags);
	}
}

void Resource::update(float deltaTime) {
	if (!active) timeElapsed += deltaTime;
	if (timeElapsed >= RESOURCE_REACTIVATION_TIME) {
		active = true;
		timeElapsed = 0;
		std::cout << "resource " << id << " going active\n";
	}
}

bool Resource::isActive() {
	return active;
}

std::pair<char, int> Resource::harvest() {
	if (!active) std::cout << "resource " << id << " harvested when inactive\n";
	else std::cout << "resource " << id << " harvested\n";
	std::pair<char, int> res;
	if (type == DUMPSTER_TYPE) {
		res = std::make_pair(PLASTIC_RES_TYPE, DUMPSTER_PLASTIC);
	}
	else if (type == RECYCLING_BIN_TYPE) {
		res = std::make_pair(METAL_RES_TYPE, RECYCLING_BIN_METAL);
	}

	//this->takeDamage(RESOURCE_HEALTH); //mark as "dead" to indicate used
	active = false;

	return res;
}
