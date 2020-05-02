#include "player.h"

Player::Player() : Entity() {
	velocity_x = 0.f;
	velocity_z = 0.f;
	rotation_y = 0.f;
	acceleration_x = 0.f;
	acceleration_z = 0.f;
}

Player::Player(mat4 model_mat) : Entity(model_mat) {
	velocity_x = 0.f;
	velocity_z = 0.f;
	rotation_y = 0.f;
	acceleration_x = 0.f;
	acceleration_z = 0.f;
	lastTime = std::chrono::steady_clock::now();
}

void Player::update() {
	//should only execute on the server
	Entity::update();
	
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
	std::cout << "player x: " << xpos << " z: " << zpos << " y: " << yrot << "\n";
}

void Player::setVelocity(float vel_x, float vel_z) {
	velocity_x = vel_x;
	velocity_z = vel_z;
}

void Player::setMoveAndDir(PlayerInput in) {
	if (in.move_x < 0) acceleration_x = -1 * MOVE_SPEED;
	else if (in.move_x > 0) acceleration_x = MOVE_SPEED;
	else acceleration_x = 0;

	if (in.move_z < 0) acceleration_z = -1 * MOVE_SPEED;
	else if (in.move_z > 0) acceleration_z = MOVE_SPEED;
	else acceleration_z = 0;

	rotation_y = in.view_y_rot;
}