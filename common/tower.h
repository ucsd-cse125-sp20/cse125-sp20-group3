#ifndef _TOWER_H_
#define _TOWER_H_

#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
private:
	Entity* attackTarget;
public:
	Tower(int health, int attack);
	Tower(int health, int attack, mat4 model_mat);
	void update(float deltaTime) override;
};

#endif