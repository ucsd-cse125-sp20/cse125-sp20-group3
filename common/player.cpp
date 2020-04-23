#include "player.h"

Player::Player(mat4 model_mat, std::string objFilename) : model(model_mat) {
    //TODO create a geometry
	velocity_x = 0.f;
	velocity_z = 0.f;
	lastTime = std::chrono::steady_clock::now();
}

Player::Player(mat4 model_mat) : model(model_mat) {
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

	float x_translate = 0;
	float z_translate = 0;
	if (velocity_x > 0.01f || velocity_x < -0.01f) {
		x_translate = velocity_x * deltaTime;
	}
	if (velocity_z > 0.01f || velocity_z < -0.01f) {
		z_translate = velocity_z * deltaTime;
	}

	//TODO translate model by (x_translate, 0, z_translate)
	model[3] += vec4(x_translate, 0, z_translate, 0);

	//TODO handle rotation stuff
	if (sqrt(velocity_x * velocity_x + velocity_z * velocity_z) > 0.001) {
		vec3 forward = normalize(vec3(velocity_x, 0, velocity_z));
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);
	}
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
	vec3 forward = normalize(vec3(cos(rot_y), 0, sin(rot_y)));
	vec3 right = cross(forward, vec3(0, 1, 0));

	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);
	model[3] = vec4(pos_x, 0, pos_z, 1);
}

void Player::setData(char buf[], int index)
{
	((PlayerData*)buf)[index] = { model[3][0], model[3][2], acos(model[0][0]) };
}

mat4 Player::getMatrix() {
	return model;
}
