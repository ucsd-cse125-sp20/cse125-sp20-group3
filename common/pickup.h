#pragma once

#include "macros.h"
#include "entity.h"
#include <iostream>

class Pickup : public Entity {
protected:
	char type;
	bool active;
	float timeoutInterval;

public:
	Pickup(char resourceType, GameObjectData data, int id, SceneManager_Server* sm);

	void update(float deltaTime) override;
	std::pair<char, int> pickup();
};