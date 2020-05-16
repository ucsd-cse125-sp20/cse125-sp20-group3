#ifndef _TOWER_H_
#define _TOWER_H_

#include "entity.h"
#include "team.h"
#include "ObjectDetection.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
protected:
	Tower(std::string id, int health, int attack, SceneManager_Server* sm);
public:
	void setHealth(int new_health) override;
};

#endif