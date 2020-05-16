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
	std::string id_str;
	int health;
	int attack;
	Team* team;
	SceneManager_Server* manager;

public:
	struct EntityData {
		GameObjectData GO_data;
		int health;
	};

	Entity(std::string id, int h, int a, SceneManager_Server* sm) : GameObject() { id_str = id; health = h; attack = a; manager = sm; };
	virtual void update(float deltaTime) {}
	bool isEnemyTeam(Team* checkTeam) { return this->team != checkTeam; }
	std::string getIDstr() { return id_str; }
	int getHealth() { return health; }
	virtual void setHealth(int new_health) { health = new_health; }
	void takeDamage(int attack) { health = max(health - attack, 0);	}

	void setEntData(EntityData data) {
		this->GameObject::setGOData(data.GO_data);
		this->health = data.health;
	}

	int writeData(char buf[], int index) override {
		EntityData entData;
		entData.GO_data = this->GameObject::getData();
		entData.health = this->health;
		((EntityData*)(buf + index))[0] = entData;
		return sizeof(EntityData);
	}
};

#endif