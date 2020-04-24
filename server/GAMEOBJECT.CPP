#include "GameObject.h"

GameObject::GameObject() {
	model = mat4::identity();
}

void GameObject::update() {
	auto currTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaDuration = currTime - lastTime;
	deltaTime = deltaDuration.count();
	lastTime = std::chrono::steady_clock::now();
}

void GameObject::setPosRot(float pos_x, float pos_z, float rot_y){
	vec3 forward = normalize(vec3(cos(rot_y), 0, sin(rot_y)));
	vec3 right = cross(forward, vec3(0, 1, 0));

	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);
	model[3] = vec4(pos_x, 0, pos_z, 1);
}

void GameObject::setMatrix(mat4 m){
	model = m;
}
mat4 GameObject::getMatrix(){
	return model;
}

int GameObject::setData(char buf[], int index){
	((GameObjectData*)buf)[index] = { model[3][0], model[3][2], atan2(-model[2][2], -model[2][0]) };
	return sizeof(GameObjectData);
}
