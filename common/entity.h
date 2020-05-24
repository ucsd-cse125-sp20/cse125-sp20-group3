#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "GameObject.h"
#include "team.h"
#include "ObjectDetection.h"
#include <iostream>
#include <chrono>
#include <ctime>

class SceneManager_Server;

class Entity : public GameObject {
protected:
	float timeElapsed;

	int id;
	char actionState;
	int health;
	int attackDamage;

	Entity* attackTarget; //shouldn't do anything in classes that don't use it
	int attackTargetID;

	Team* team;
	SceneManager_Server* manager;

	vec3 lastPosition;

public:
	struct EntityData {
		GameObjectData GO_data;
		char actionState;
		char teamColor;
		int health;
		int targetID;
	};

	Entity(GameObjectData startData, int i, int h, int a, Team* t, SceneManager_Server* sm) : GameObject(startData) {
		id = i; health = h; attackDamage = a; team = t; manager = sm;
		timeElapsed = 0;
		actionState = ACTION_STATE_IDLE;
		attackTarget = nullptr;
		attackTargetID = NO_TARGET_ID;
	};
	virtual void update(float deltaTime) {} //server only function
	bool isEnemyTeam(Team* checkTeam) { return this->team != checkTeam; }
	int getID() { return id; }
	int getHealth() { return health; }
	void setHealth(int new_health) { health = new_health; }
	virtual void takeDamage(int damage) { health = std::max(health - damage, 0);	}

	virtual void setEntData(EntityData data) {
		lastPosition = this->getPosition(); //save old position for velocity inference purposes
		this->GameObject::setGOData(data.GO_data);
		this->actionState = data.actionState;
		//teamColor is never updated, only used for initial instantiation of objects
		this->health = data.health;
		this->attackTargetID = data.targetID;
	}

	int writeData(char buf[], int index) override {
		EntityData entData;
		entData.GO_data = this->GameObject::getData();
		entData.actionState = this->actionState;
		entData.teamColor = team != nullptr ? team->teamColor : NO_TEAM;
		entData.health = this->health;
		entData.targetID = attackTarget != nullptr ? attackTargetID : NO_TARGET_ID;
		((EntityData*)(buf + index))[0] = entData;
		return sizeof(EntityData);
	}
};

#endif