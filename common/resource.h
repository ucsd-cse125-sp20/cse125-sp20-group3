#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "entity.h"
#include <iostream>
#include <chrono>
#include <ctime>

#define RESOURCE_REACTIVATION_TIME 30

class Resource : public Entity {
protected:
	char type;
	bool active;

public:
	Resource(char resourceType, GameObjectData data, int id, SceneManager_Server* sm_server);
	void update(float deltaTime) override;
	bool isActive();
	std::pair<char, int> harvest();
};

#endif
