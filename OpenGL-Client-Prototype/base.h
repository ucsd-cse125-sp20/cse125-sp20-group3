#include "OBJObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Base : public Entity {
public:
        Base(std::string objFilename);
        Base();
        ~Base();
        void update();
};
