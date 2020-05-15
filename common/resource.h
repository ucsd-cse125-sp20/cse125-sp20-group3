#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "entity.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Resource : public Entity {
protected:
	char type;
public:
	Resource(char resourceType, SceneManager_Server* sm_server);
	Resource(char resourceType, SceneManager_Server* sm_server, mat4 model_mat);
	std::pair<char, int> harvest();
};

#endif
