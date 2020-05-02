#include "OBJObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
public:
        Tower(mat4 model_mat, std::string objFilename);
        Tower(mat4 model_mat);
        ~Tower();
        void update();
};
