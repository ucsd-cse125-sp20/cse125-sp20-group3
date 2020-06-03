#include "pickup.h"
#include "../server/SceneManager_Server.h"

Pickup::Pickup(char resourceType, GameObjectData data, int id, SceneManager_Server* sm_server) : Entity(data, id, PICKUP_HEALTH, PICKUP_ATTACK, nullptr, sm_server) {
	type = resourceType;
	active = true;

	if (sm_server != nullptr) { //only execute on server
		int flags = DETECTION_FLAG_PICKUP;
		ObjectDetection::addObject(this, flags, -PICKUP_WIDTH, PICKUP_WIDTH, -PICKUP_LENGTH, PICKUP_LENGTH);
	}
}

std::pair<char, int> Pickup::pickup() {
	if (!active) return std::make_pair(METAL_RES_TYPE, 0); //if not active, return nothing
	int value;
	if (type == METAL_RES_TYPE)	value = PICKUP_METAL;
	else if (type == PLASTIC_RES_TYPE) value = PICKUP_PLASTIC;
	else std::cout << "invalid pickup type picked up\n";
		
	std::pair<char, int> drop = std::make_pair(type, value);

	active = false;
	this->takeDamage(PICKUP_HEALTH);
	std::cout << "pickup " << id << " of type " << type << " picked up\n";

	return drop;
}