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
        Base();
        Base(mat4 model_mat);
        void update(float deltaTime) override;
};

#endif