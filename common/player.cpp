#include "player.h"

Player::Player() : Entity(PLAYER_HEALTH, PLAYER_ATTACK) {
	velocity_x = 0.f;
	velocity_z = 0.f;
	rotation_y = 0.f;
	acceleration_x = 0.f;
	acceleration_z = 0.f;
}

Player::Player(mat4 model_mat) : Entity(PLAYER_HEALTH, PLAYER_ATTACK, model_mat) {
	velocity_x = 0.f;
	velocity_z = 0.f;
	rotation_y = 0.f;
	acceleration_x = 0.f;
	acceleration_z = 0.f;
}

void Player::update(float deltaTime) {
	//should only execute on the server
	
	velocity_x *= 0.9f;
	velocity_z *= 0.9f;
	velocity_x += (cos(rotation_y) * acceleration_x - sin(rotation_y) * acceleration_z) * deltaTime;
	velocity_z += (cos(rotation_y) * acceleration_z + sin(rotation_y) * acceleration_x) * deltaTime;
	//printf("%f %f %f %f %f\n", velocity_x, velocity_z, acceleration_x, acceleration_z, deltaTime);

	//TODO handle rotation stuff
	if (sqrt(velocity_x * velocity_x + velocity_z * velocity_z) > 0.001) {
		model[3] += vec4(velocity_x, 0, velocity_z, 0);
	}

	vec3 forward = vec3(cos(rotation_y), 0, sin(rotation_y));
	vec3 right = cross(forward, vec3(0, 1, 0));
	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);

	float xpos = model[3][0];
	float zpos = model[3][2];
	float yrot = atan2(-model[2][2], -model[2][0]);
	//std::cout << "player x: " << xpos << " z: " << zpos << " y: " << yrot << "\n";
}

void Player::processInput(PlayerInput in) {
	this->setMoveAndDir(in.move_x, in.move_z, in.view_y_rot);

	std::cout << "intent: " << in.buildIntent << " buildType: " << in.buildType << " harvestResource: " << in.harvestResource << "\n";

	if (in.buildIntent == BUILD_CANCEL) {
		buildMode = NEUTRAL;
		std::cout << "cancelling\n";
	}
	else {
		if (in.buildIntent == BUILD_CONFIRM && buildMode != NEUTRAL) {
			//build something based on buildMode
			//TODO: check plastic and metal cost against amount in team inventory
			std::cout << "building\n";
			//TODO: do math to figure out where to build
		}
		else {
			buildMode = in.buildType == LASER_TYPE ? LASER : 
						in.buildType == CLAW_TYPE ? CLAW : 
						in.buildType == SUPER_MINION_TYPE ? SUPER_MINION : 
						buildMode;
			if (in.buildType != NO_BUILD_TYPE) {
				std::cout << "buildMode: " << buildMode << "\n";
			}
		}
	}

	if (in.harvestResource) {
		//TODO: check front of player based on position and rotation and harvest resource if present
		std::cout << "harvesting\n";
	}
}

void Player::setVelocity(float vel_x, float vel_z) {
	velocity_x = vel_x;
	velocity_z = vel_z;
}

std::pair<float, float> Player::getVelocities() {
	return std::make_pair(this->velocity_x, this->velocity_z);
}

//read the move_x, move_z, and view_y_rot from PlayerInput
//ignore the other values
void Player::setMoveAndDir(int move_x, int move_z, float view_y_rot) {
	if (move_x < 0) acceleration_x = -1 * MOVE_SPEED;
	else if (move_x > 0) acceleration_x = MOVE_SPEED;
	else acceleration_x = 0;

	if (move_z < 0) acceleration_z = -1 * MOVE_SPEED;
	else if (move_z > 0) acceleration_z = MOVE_SPEED;
	else acceleration_z = 0;

	rotation_y = view_y_rot;
}