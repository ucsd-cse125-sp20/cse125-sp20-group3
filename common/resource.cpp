#include "resource.h"
#include "../server/SceneManager_Server.h"

Resource::Resource(char resourceType, SceneManager_Server* sm_server) : Entity(RESOURCE_HEALTH, RESOURCE_ATTACK, sm_server) {
	type = resourceType;
}

Resource::Resource(char resourceType, SceneManager_Server* sm_server, mat4 model_mat) : Entity(RESOURCE_HEALTH, RESOURCE_ATTACK, sm_server, model_mat) {
	type = resourceType;
}

std::pair<char, int> Resource::harvest() {
	//TODO figure out how much to return, kill this once harvested
	std::pair<char, int> res = std::make_pair(type, 1);
	return res;
}
