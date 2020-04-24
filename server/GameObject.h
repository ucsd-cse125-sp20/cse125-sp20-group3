#pragma once

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <chrono>

class GameObject 
{
public:
	struct GameObjectData {
		float x, z, rot;
	};

	std::chrono::steady_clock::time_point lastTime;
	float deltaTime = 0.f;
	mat4 model;

	GameObject();
	GameObject(mat4 m) : model(m) {}
	
//	~GameObject();

	void update();

	void setPosRot(float pos_x, float pos_z, float rot_y);
	void setMatrix(mat4 m);
	mat4 getMatrix();

	int setData(char buf[], int index);
};