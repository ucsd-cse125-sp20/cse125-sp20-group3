#ifndef _TOWER_H_
#define _TOWER_H_

#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
protected:
	Entity* attackTarget;
	Tower(int health, int attack, SceneManager_Server* sm);
	Tower(int health, int attack, SceneManager_Server* sm, mat4 model_mat);
public:
	void update(float deltaTime) override {};
	void setHealth(int new_health) override;
};

#endif