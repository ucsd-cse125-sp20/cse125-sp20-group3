#include "Camera.h"
#include <glm/gtx/string_cast.hpp>

const glm::vec3 player_offset(0, 3, 10);

Camera::Camera(Player* p) {
	this->model = glm::mat4(1);
	this->tracked_player = p;
	pitch = 0.f;
}

glm::mat4 Camera::getView() {
	this->updateView();
	/*glm::mat4 camMat = glm::lookAt(player_offset, glm::vec3(0, 3, 0), glm::vec3(0, 1, 0));
	glm::vec3 posFromCam = glm::inverse(please) * glm::vec4(0, 0, 0, 1);
	std::cout << "posFromCam: " << glm::to_string(posFromCam) << std::endl;
	std::cout << "ideal: " << glm::to_string(camMat) << std::endl;
	std::cout << "actual: " << glm::to_string(glm::inverse(this->model)) << std::endl;*/
	return glm::inverse(this->model);
}

void Camera::updateView() {
	//first apply offset translation and then rotation around x-axis so that camera orbits player
	this->model = glm::rotate(this->pitch, glm::vec3(1.f, 0, 0)) * glm::translate(player_offset) * glm::mat4(1);
	//then apply player object's rotation and translation
	this->model = tracked_player->getModel() * this->model;
	glm::vec3 p = this->model * glm::vec4(0, 0, 0, 1);
	std::cout << "camera: " << p.x << " " << p.y << " " << p.z << std::endl;
}

void Camera::lookUpDown(float yOffset) {
	pitch += yOffset;
	if (pitch > 89.f) pitch = 89.f;
	else if (pitch < -89.f) pitch = -89.f;
}