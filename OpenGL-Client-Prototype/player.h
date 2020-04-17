#include "OBJObject.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Player : public OBJObject {
private:
        float lastTime;
        float velocity_x;
        float velocity_z;
        //TODO: Incorporate camera
public:
        Player(std::string objFilename);
        Player();
        ~Player();
        void update();
        void updateVelocity(float vel_x, float vel_z);
        void buildTower();
        void spawnUnit();
};
