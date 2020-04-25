#ifndef _PLAYER_H_
#define _PLAYER_H_

//#include "../client/src/Transform.h" //only include if client-side?
#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include "GameObject.h"
#include "client2server.h"

#define MOVE_SPEED 1

class Player : public GameObject {
private:
    std::chrono::steady_clock::time_point lastTime;
    float velocity_x, velocity_z, acceleration_x, acceleration_z;
    float rotation_y;
public:

    Player(mat4 model_mat, std::string objFilename); //client-side, create geometry to load model
    Player(mat4 model_mat); //server-side, only init state management logic
	
    void update(); //server-side state management
	void setVelocity(float vel_x, float vel_z);
	void setMoveAndDir(PlayerInput in);

    void buildTower();
    void spawnUnit();
};
#endif