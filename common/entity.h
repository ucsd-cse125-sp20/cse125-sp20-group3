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
	Entity(int h, int a) : GameObject() { health = h; attack = a; };
	Entity(int h, int a, mat4 m) : GameObject(m) { health = h; attack = a; };
	void update() override { GameObject::update(); }
    bool isEnemyTeam(Team* checkTeam);
	int getHealth() { return health; }
	void setHealth(int new_health) { health = new_health; }
	void takeDamage(int attack);
};

#endif