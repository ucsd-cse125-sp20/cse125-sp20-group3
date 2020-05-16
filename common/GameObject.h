#pragma once

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <chrono>
#include <vector>
#include <iostream>

struct SpatialCell;

struct ObjDectData {
	float minX, maxX, minZ, maxZ;
	int flags;
	SpatialCell* cell;
};

class GameObject 
{
public:
	struct GameObjectData {
		float x, z, rot;
	};

	ObjDectData objDectData = {};

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
	vec3 getPosition();

	GameObjectData getData();
	virtual int writeData(char buf[], int index);
};

struct SpatialCell {
	uint64_t key;
	std::vector<GameObject*> objects;
};