#include "player.h"

Player::Player(std::string objFilename) : OBJObject(objFilename) {
        Player();
}

Player::Player() {
        velocity_x = 0.f;
        velocity_z = 0.f;
        lastTime = glfwGetTime();
        //lastTime = std::chrono::system_clock::now();
}

void Player::updateVelocity(float vel_x, float vel_z) {
        velocity_x = vel_x;
        velocity_z = vel_z;
}

void Player::update() {
        //std::chrono::duration<float, curTime_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(curTime.time_since_epoch()).count();
        float deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
		glm::vec3 p = this->model * glm::vec4(0, 0, 0, 1);
		std::cout << "player: " << p.x << " " << p.y << " " << p.z << std::endl;
		/*if (velocity_x > 0.01f || velocity_x < -0.01f) {
			pos_x = (velocity_x * deltaTime) + pos_x;
		}
		if (velocity_z > 0.01f || velocity_z < -0.01f) {
			pos_z = (velocity_z * deltaTime) + pos_z;
		}*/
}
