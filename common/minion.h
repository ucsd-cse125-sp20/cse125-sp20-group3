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
	int pathPtr;
	std::vector<std::tuple<float, float>> path;
protected:
	Minion(int health, int attack, SceneManager_Server* sm); //accept values for super minion spawned by player
	Minion(int health, int attack, SceneManager_Server* sm, mat4 model_mat);
public:
	Minion(SceneManager_Server* sm); //basic minion spawned by claw tower
	Minion(SceneManager_Server* sm, mat4 model_mat);
	
	void update(float deltaTime) override;
	void setHealth(int new_health) override;
	void move();
};

#endif