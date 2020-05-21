#ifndef _BASE_H_
#define _BASE_H_

#include "macros.h"
#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Base : public Entity {
public:
        Base(std::string id, Team* t, SceneManager_Server* sm);
        void update(float deltaTime) override;
};

#endif