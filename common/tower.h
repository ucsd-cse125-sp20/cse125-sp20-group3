#ifndef _TOWER_H_
#define _TOWER_H_

#include "entity.h"
#include "team.h"
#include "ObjectDetection.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
protected:
	Tower(GameObjectData data, int id, int health, int attack, Team* t, SceneManager_Server* sm);
public:
	void takeDamage(int damage) override;
};

#endif