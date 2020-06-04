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
	Team* getTeam() { return this->team; }
	int getID() { return id; }
	char getActionState() { return actionState; }
	int getAttackTargetID() { return attackTargetID; }
	int getHealth() { return health; }
	void setHealth(int new_health) { health = new_health; }
	virtual void takeDamage(int damage) { health = std::max(health - damage, 0);	}

	static uint64_t compressData(EntityData data) {
		uint64_t d = 0;
		d |= GameObject::compressData(data.GO_data) << 24;
		d |= (uint64_t)(data.health / 10) << 16;
		d |= ((uint64_t)data.targetID & 0xFFF) << 4;
		d |= (uint64_t)data.actionState << 2;
		d |= data.teamColor == RED_TEAM ? RED_TEAM_SMALL : (data.teamColor == BLUE_TEAM ? BLUE_TEAM_SMALL : NO_TEAM_SMALL);
		//printf("%llx\n", d);
		return d;
	}

	static EntityData decompressData(uint64_t data) {
		EntityData d = {};
		d.GO_data = GameObject::decompressData(data >> 24);
		d.health = (int)((data >> 16) & 0xFFL) * 10;
		d.targetID = (int)((data >> 4) & 0xFFFL);
		d.actionState = (int)((data >> 2) & 0x3L);
		int team = (int)(data & 0x3L);
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
#if defined(USE_SMALL_DATA)
		//printf("%d ", index);
		((uint64_t*)(buf + index))[0] = Entity::compressData(entData);
		return sizeof(uint64_t);
#else
		//EntityData check = Entity::decompressData(Entity::compressData(entData));
		((EntityData*)(buf + index))[0] = entData;
		return sizeof(EntityData);
#endif
	}
};

#endif