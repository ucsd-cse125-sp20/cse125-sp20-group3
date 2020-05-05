#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Base : public Entity {
public:
        Base(mat4 model_mat, std::string objFilename);
        Base(mat4 model_mat);
        ~Base();
        void update() override;
};
