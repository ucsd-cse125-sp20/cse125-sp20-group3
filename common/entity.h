#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "GameObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Entity : public GameObject {
protected:
    int health;
    int attack;
    Team* team;
public:
	struct EntityData {
		GameObjectData GO_data;
		int health;
	};

	Entity(int h, int a) : GameObject() { health = h; attack = a; };
	Entity(int h, int a, mat4 m) : GameObject(m) { health = h; attack = a; };
	virtual void update(float deltaTime) {}
    bool isEnemyTeam(Team* checkTeam);
	int getHealth() { return health; }
	void setHealth(int new_health) { health = new_health; }
	void takeDamage(int attack);

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