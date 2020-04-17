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

Player::updateVelocity(float vel_x, float vel_z) {
        velocity_x = vel_x;
        velocity_z = vel_z;
}

Player::update() {
        //std::chrono::duration<float, curTime_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(curTime.time_since_epoch()).count();
        float deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
}
