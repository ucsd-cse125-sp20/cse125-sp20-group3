#include "resource.h"
#include "../server/SceneManager_Server.h"

Resource::Resource(char resourceType, std::string id, SceneManager_Server* sm_server) : Entity(id, RESOURCE_HEALTH, RESOURCE_ATTACK, sm_server) {
	type = resourceType;
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
