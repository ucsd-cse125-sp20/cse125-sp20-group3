#include "player.h"

Player::Player(mat4 model_mat, std::string objFilename) : Transform(model_mat) {
    //TODO create a geometry
	velocity_x = 0.f;
	velocity_z = 0.f;
	lastTime = std::chrono::steady_clock::now();
}

Player::Player(mat4 model_mat) : Transform(model_mat) {
	velocity_x = 0.f;
	velocity_z = 0.f;
	lastTime = std::chrono::steady_clock::now();
}

void Player::update() {
	//should only execute on the server
	auto currTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaDuration = currTime - lastTime;
	float deltaTime = deltaDuration.count();
	lastTime = std::chrono::steady_clock::now();

	if (velocity_x > 0.01f || velocity_x < -0.01f) {
		float x_translate = velocity_x * deltaTime;
	}
	if (velocity_z > 0.01f || velocity_z < -0.01f) {
		float z_translate = velocity_z * deltaTime;
	}

	//TODO translate model by (x_translate, 0, z_translate)

	//TODO handle rotation stuff
}

void Player::setVelocity(float vel_x, float vel_z) {
	velocity_x = vel_x;
	velocity_z = vel_z;
}

void Player::setMove(int move_x, int move_z) {
	if (move_x < 0) velocity_x = -1 * MOVE_SPEED;
	else if (move_x > 0) velocity_x = MOVE_SPEED;
	else velocity_x = 0;

	if (move_z < 0) velocity_z = -1 * MOVE_SPEED;
	else if (move_z > 0) velocity_z = MOVE_SPEED;
	else velocity_z = 0;
}

void Player::setPosRot(float pos_x, float pos_z, float rot_y) {
	//should only execute on the client
	//TODO update matrix accordingly
}