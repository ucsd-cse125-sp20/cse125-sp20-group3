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

class Minion : public Entity {
protected:
	mapNode* destNode;
	float timeElapsed;
	Entity* attackTarget;
	std::string attackTargetID;
	int attackRange;
	float attackInterval;
	float velocity;

	Minion(std::string id, int health, int attack, int range, float interval, float vel, Team* t, SceneManager_Server* sm); //accept values for other types of minions
public:
	Minion(std::string id, Team* t, SceneManager_Server* sm); //basic minion spawned by claw tower
	
	void update(float deltaTime) override;
	void takeDamage(int damage) override;
	void move(float deltaTime);
	void attack();

	//TESTING SPECIFIC FUNCTIONALITY - DO NOT USE
	void setAttackTarget(Entity* e);
};

#endif