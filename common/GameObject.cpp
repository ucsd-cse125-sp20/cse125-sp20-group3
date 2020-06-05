#include "GameObject.h"

GameObject::GameObject() {
	model = mat4::identity();
}

GameObject::GameObject(GameObjectData data) {
	model = mat4::identity();
	this->setGOData(data);
}

void GameObject::setGOData(GameObjectData data){
	//std::cout << "setting data x: " << data.x << " z: " << data.z << " y: " << data.rot << "\n";
	vec3 forward = normalize(vec3(cos(data.rot), 0, sin(data.rot)));
	vec3 right = cross(forward, vec3(0, 1, 0));

	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);
	model[3] = vec4(data.x, 0, data.z, 1);
}

void GameObject::setMatrix(mat4 m){
	model = m;
}
mat4 GameObject::getMatrix(){
	return model;
}

vec3 GameObject::getPosition() {
	return getMatrix()[3].getXYZ();
}

int GameObject::writeData(char buf[], int index) {
	((GameObjectData*)(buf + index))[0] = { model[3][0], model[3][2], atan2(-model[2][2], -model[2][0]) };
	return sizeof(GameObjectData);
}

uint64_t GameObject::compressData(GameObject::GameObjectData data)
{
	uint64_t d = 0;
	d |= (uint64_t)(UINT16_MAX * (data.x - MIN_MAP_X) / (MAX_MAP_X - MIN_MAP_X)) << 24;
	d |= (uint64_t)(UINT16_MAX * (data.z - MIN_MAP_Z) / (MAX_MAP_Z - MIN_MAP_Z)) << 8;
	d |= (uint64_t)(UINT8_MAX * (data.rot + PI) / (2 * PI));
	return d;
}

GameObject::GameObjectData GameObject::decompressData(uint64_t data)
{
	GameObject::GameObjectData d = {};
	d.x = ((float)(data >> 24) / UINT16_MAX) * (MAX_MAP_X - MIN_MAP_X) + MIN_MAP_X;
	d.z = ((float)((data >> 8) & 0xFFFF) / UINT16_MAX) * (MAX_MAP_Z - MIN_MAP_Z) + MIN_MAP_Z;
	d.rot = ((float)(data & 0xFF) / UINT8_MAX) * (2 * PI) - PI;
	return d;
}

GameObject::GameObjectData GameObject::getData() {
	GameObjectData data = { model[3][0], model[3][2], atan2(-model[2][2], -model[2][0]) };
	return data;
}

/***** legacy code *****/
/*void GameObject::resetClock()
{
	lastTime = std::chrono::steady_clock::now();
}

void GameObject::update() {
	auto currTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaDuration = currTime - lastTime;
	deltaTime = deltaDuration.count();
	lastTime = std::chrono::steady_clock::now();
}*/