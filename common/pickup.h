#pragma once

#include "macros.h"
#include "entity.h"
#include <iostream>

class Pickup : public Entity {
protected:
	char type;
	bool active;

public:
	Pickup(char resourceType, GameObjectData data, int id, SceneManager_Server* sm);

	std::pair<char, int> pickup();
};