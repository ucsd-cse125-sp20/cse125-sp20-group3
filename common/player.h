#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include "entity.h"
#include "client2server.h"
#include "macros.h"

#define MOVE_SPEED 1

class Player : public Entity {
private:
	enum BUILD_MODE { NEUTRAL, LASER, CLAW, SUPER_MINION };

    float velocity_x, velocity_z, acceleration_x, acceleration_z;
    float rotation_y;
	BUILD_MODE buildMode;
public:
	/*struct PlayerData {
		std::string id;
		float velocity_x;
		float velocity_z;
	};*/

    Player(std::string id, Team* t, SceneManager_Server* sm);
	
    void update(float deltaTime) override; //server-side state management
	void processInput(PlayerInput in);
	void setVelocity(float vel_x, float vel_z);
	std::pair<float, float> getVelocities();
	void setMoveAndDir(int move_x, int move_z, float view_y_rot);

    void buildTower();
    void spawnUnit();
};
#endif