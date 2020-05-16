#ifndef _MINION_H_
#define _MINION_H_

#include "macros.h"
#include "entity.h"
#include "team.h"
#include "ObjectDetection.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Minion : public Entity {
protected:
	float timeElapsed;
	Entity* attackTarget;
	float attackRange;
	float attackInterval;

	Minion(std::string id, int health, int attack, float range, SceneManager_Server* sm); //accept values for other types of minions
public:
	Minion(std::string id, SceneManager_Server* sm); //basic minion spawned by claw tower
	
	void update(float deltaTime) override;
	void setHealth(int new_health) override;
};

#endif