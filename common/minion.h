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

#define MINION_ACTION_IDLE 0
#define MINION_ACTION_MOVE 1
#define MINION_ACTION_ATTACK 2
#define MINION_ACTION_FIRE 3

class Minion : public Entity {
protected:
	mapNode* destNode;
	int attackRange;
	float attackInterval;
	float velocity;
	bool doneMoving;

	Minion(int id, int health, int attack, int range, float interval, float vel, Team* t, SceneManager_Server* sm); //accept values for other types of minions
public:
	Minion(int id, Team* t, SceneManager_Server* sm); //basic minion spawned by claw tower
	
	void update(float deltaTime) override;
	void takeDamage(int damage) override;
	void move(float deltaTime);
	void attack();

	void setEntData(EntityData data) override;

	//TESTING SPECIFIC FUNCTIONALITY - DO NOT USE
	void setAttackTarget(Entity* e);
};

#endif