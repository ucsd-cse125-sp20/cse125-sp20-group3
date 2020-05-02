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
	Entity() : GameObject() {};
	Entity(mat4 m) : GameObject(m) {};
	void update() override { GameObject::update(); }
    bool isEnemyTeam(Team* checkTeam);
	int getHealth() { return health; }
	void updateHealth(int attack);
};

#endif