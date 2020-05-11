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
        Base(SceneManager_Server* sm);
        Base(SceneManager_Server* sm, mat4 model_mat);
        void update(float deltaTime) override;
};

#endif