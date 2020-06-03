#ifndef _MINION_H_
#define _MINION_H_

#include "macros.h"
#include "entity.h"
#include "team.h"
#include "ObjectDetection.h"
#include "pathNode.h"
#include "MathUtils.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <map>

#define MINION_MOVE_ATTEMPTS 7

class Minion : public Entity {
protected:
	PathNode* destNode;
	int attackRange;
	float attackInterval;
	float velocity;
	bool doneMoving;

	Minion(GameObjectData data, int id, int health, int attack, int range, float interval, float vel, Team* t, SceneManager_Server* sm); //accept values for other types of minions
public:
	Minion(GameObjectData data, int id, Team* t, SceneManager_Server* sm); //basic minion spawned by claw tower
	
	void update(float deltaTime) override;
	void takeDamage(int damage) override;
	virtual void dropPickups();
	void move(float deltaTime);
	void attack();

	void setEntData(EntityData data) override;

	//TESTING SPECIFIC FUNCTIONALITY - DO NOT USE
	void setAttackTarget(Entity* e);
};

#endif