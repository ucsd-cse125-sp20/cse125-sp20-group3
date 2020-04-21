#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "OBJObject.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Player : public OBJObject {
private:
        float lastTime;
        float velocity_x, velocity_z;
		float pos_x, pos_z;
public:
        Player(std::string objFilename);
        Player();
		void update();
        void updateVelocity(float vel_x, float vel_z);
        void buildTower();
        void spawnUnit();
};
#endif