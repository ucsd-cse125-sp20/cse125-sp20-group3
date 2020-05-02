#include "OBJObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Unit : public Entity {
private:
        Entity attackTarget;
public:
        Unit(mat4 model_mat, std::string objFilename);
        Unit(mat4 model_mat);
        ~Unit();
        void update();
};
