#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include "entity.h"
#include "client2server.h"

#define MOVE_SPEED 1

class Player : public Entity {
private:
    std::chrono::steady_clock::time_point lastTime;
    float velocity_x, velocity_z, acceleration_x, acceleration_z;
    float rotation_y;
public:

    Player(); //client-side, init nothing
    Player(mat4 model_mat); //server-side, only init state management logic
	
    void update() override; //server-side state management
	void setVelocity(float vel_x, float vel_z);
	void setMoveAndDir(PlayerInput in);

    void buildTower();
    void spawnUnit();
};
#endif