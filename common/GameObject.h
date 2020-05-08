#pragma once

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <chrono>
#include <iostream>

class GameObject 
{
public:
	struct GameObjectData {
		float x, z, rot;
	};

	//std::chrono::steady_clock::time_point lastTime;
	//float deltaTime = 0.f;
	mat4 model;

	GameObject();
	GameObject(mat4 m) : model(m) {}
	
//	~GameObject();

	//virtual void update();
	//void resetClock();

	void setGOData(GameObjectData data);
	void setMatrix(mat4 m);
	mat4 getMatrix();

	GameObjectData getData();
	virtual int writeData(char buf[], int index);
};