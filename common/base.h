#ifndef _BASE_H_
#define _BASE_H_

#include "macros.h"
#include "entity.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Base : public Entity {
public:
        Base(GameObjectData data, int id, Team* t, SceneManager_Server* sm);
};

#endif