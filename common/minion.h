#ifndef _MINION_H_
#define _MINION_H_

#include "macros.h"
#include "entity.h"
#include "team.h"
#include "ObjectDetection.h"
#include "mapNode.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
using namespace std;

class Minion : public Entity {
private:
	mapNode* curNode;
protected:
	float velocity_x;
	float velocity_z;
	float timeElapsed;
	Entity* attackTarget;
	float attackRange;
	float attackInterval;

	Minion(string id, int health, int attack, float range, SceneManager_Server* sm); //accept values for other types of minions
public:
	Minion(string id, SceneManager_Server* sm); //basic minion spawned by claw tower
	
	void update(float deltaTime) override;
	void setHealth(int new_health) override;
	void move(float deltaTime);
	void attack();
};

#endif