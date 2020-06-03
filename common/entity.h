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

	struct EntityData_Small {
		GameObjectData_Small GO_data;
		uint8_t health;
		uint16_t state;
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
	Team* getTeam() { return this->team; }
	int getID() { return id; }
	char getActionState() { return actionState; }
	int getAttackTargetID() { return attackTargetID; }
	int getHealth() { return health; }
	void setHealth(int new_health) { health = new_health; }
	virtual void takeDamage(int damage) { health = std::max(health - damage, 0);	}

	static EntityData_Small compressData(EntityData data) {
		EntityData_Small d = {};
		d.GO_data = GameObject::compressData(data.GO_data);
		d.health = (uint8_t)(data.health / 10);
		d.state = (uint16_t)((data.targetID << 4) + (data.actionState << 2));
		d.state += data.teamColor == RED_TEAM ? RED_TEAM_SMALL : (data.teamColor == BLUE_TEAM ? BLUE_TEAM_SMALL : NO_TEAM_SMALL);
		return d;
	}

	static EntityData decompressData(EntityData_Small data) {
		EntityData d = {};
		d.GO_data = GameObject::decompressData(data.GO_data);
		d.health = (int)data.health * 10;
		d.targetID = (int)data.state >> 4;
		d.actionState = (int)data.state >> 2 & 0x3;
		int team = (int)data.state & 0x3;
		d.teamColor = team == RED_TEAM_SMALL ? RED_TEAM : (team == BLUE_TEAM_SMALL ? BLUE_TEAM : NO_TEAM);
		return d;
	}

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
		if (USE_SMALL_DATA) {
			((EntityData_Small*)(buf + index))[0] = Entity::compressData(entData);
			return sizeof(EntityData_Small);
		}
		((EntityData*)(buf + index))[0] = entData;
		return sizeof(EntityData);
	}
};

#endif