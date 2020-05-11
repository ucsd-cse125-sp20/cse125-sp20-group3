#ifndef _MINION_H_
#define _MINION_H_

#include "macros.h"
#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Minion : public Entity {
private:
	Entity* attackTarget;
public:
	Minion(int health, int attack, SceneManager_Server* sm);
	Minion(int health, int attack, SceneManager_Server* sm, mat4 model_mat);
	void update(float deltaTime) override;
};

#endif