#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Unit : public Entity {
private:
        Entity attackTarget;
public:
        Unit(std::string objFilename);
        Unit();
        ~Unit();
        void update();
};
