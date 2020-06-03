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

void Pickup::update(float deltaTime) {
	timeElapsed += deltaTime;

	if (timeElapsed >= PICKUP_TIMEOUT_INTERVAL) {
		active = false;
		this->takeDamage(PICKUP_HEALTH);
		//std::cout << "pickup " << id << " timed out, despawning\n";
	}
}

std::pair<char, int> Pickup::pickup() {
	//in the case of multiple players colliding with this, first come first served
	if (!active) return std::make_pair(METAL_RES_TYPE, 0); //if not active, return nothing

	std::pair<char, int> drop;
	if (type == IRON_TYPE) drop = std::make_pair(METAL_RES_TYPE, IRON_METAL);
	else if (type == BOTTLE_TYPE) drop = std::make_pair(PLASTIC_RES_TYPE, BOTTLE_PLASTIC);
	else std::cout << "invalid pickup type picked up\n";

	active = false;
	this->takeDamage(PICKUP_HEALTH);
	//std::cout << "pickup " << id << " of type " << type << " picked up\n";

	return drop;
}