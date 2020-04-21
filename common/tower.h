#include "OBJObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Tower : public Entity {
public:
        Tower(std::string objFilename);
        Tower();
        ~Tower();
        void update();
};
