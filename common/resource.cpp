#include "resource.h"
#include "../server/SceneManager_Server.h"

Resource::Resource(char resourceType, std::string id, SceneManager_Server* sm_server) : Entity(id, RESOURCE_HEALTH, RESOURCE_ATTACK, nullptr, sm_server) {
	type = resourceType;

	if (sm_server != nullptr) { //only execute on server
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_RESOURCE;
		ObjectDetection::addObject(this, flags);
	}
}

std::pair<char, int> Resource::harvest() {
	std::pair<char, int> res;
	if (type == DUMPSTER_TYPE) {
		res = std::make_pair(PLASTIC_RES_TYPE, DUMPSTER_PLASTIC);
	}
	else if (type == RECYCLING_BIN_TYPE) {
		res = std::make_pair(METAL_RES_TYPE, RECYCLING_BIN_METAL);
	}

	this->takeDamage(RESOURCE_HEALTH); //mark as "dead" to indicate used

	return res;
}
