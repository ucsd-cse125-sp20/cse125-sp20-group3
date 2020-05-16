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
    int health;
    int attackDamage;
    float timeElapsed;
    float actionInterval;
    Team* team;
    Entity* attackTarget;
	SceneManager_Server* manager;
public:
	struct EntityData {
		GameObjectData GO_data;
		int health;
	};

	Entity(int h, int a, SceneManager_Server* sm) : GameObject() { health = h; attackDamage = a; manager = sm; attackTarget = nullptr; timeElapsed = 0;
										actionInterval=ATTACK_INTERVAL;};
	Entity(int h, int a, SceneManager_Server* sm, mat4 m) : GameObject(m) { health = h; attackDamage = a; manager = sm; attackTarget = nullptr; timeElapsed=0;
										actionInterval=ATTACK_INTERVAL;};
	virtual void update(float deltaTime) {}
	bool isEnemyTeam(Team* checkTeam) { return this->team != checkTeam; }
	int getHealth() { return health; }
	virtual void setHealth(int new_health) { health = new_health; }
	void takeDamage(int attack) { health = max(health - attack, 0);	}
	void attack(int attackRange) {
		if (attackTarget == nullptr) {
			attackTarget = (Entity*)ObjectDetection::getNearestObject(this, 1, attackRange);
			if (this->isEnemyTeam(attackTarget->team) == false) attackTarget = nullptr; 
		}
		if (attackTarget != nullptr) {
			attackTarget->takeDamage(attackDamage);
			int enemyHealth = attackTarget->getHealth();
			if (enemyHealth <= 0) attackTarget = nullptr;
		}	
	}

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