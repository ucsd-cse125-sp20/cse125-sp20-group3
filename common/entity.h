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
	//std::string id_str;
	int id;
	char actionState;
	int health;
	int attackDamage;
	Team* team;
	SceneManager_Server* manager;

	vec3 lastPosition;

public:
	struct EntityData {
		GameObjectData GO_data;
		char actionState;
		char teamColor;
		int health;
	};

	Entity(int i, int h, int a, Team* t, SceneManager_Server* sm) : GameObject() { 
		id = i; health = h; attackDamage = a; team = t; manager = sm; 
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
	}

	int writeData(char buf[], int index) override {
		EntityData entData;
		entData.GO_data = this->GameObject::getData();
		entData.actionState = this->actionState;
		entData.teamColor = team != nullptr ? team->teamColor : NO_TEAM;
		entData.health = this->health;
		((EntityData*)(buf + index))[0] = entData;
		return sizeof(EntityData);
	}
};

#endif