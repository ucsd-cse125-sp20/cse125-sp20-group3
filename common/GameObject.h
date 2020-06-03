#pragma once

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <chrono>
#include <vector>
#include <iostream>

#define MIN_MAP_X -5.0f
#define MAX_MAP_X 140.0f
#define MIN_MAP_Z 5.0f
#define MAX_MAP_Z -105.0f

struct SpatialCell;

struct ObjDectData {
	float minX, maxX, minZ, maxZ;
	int flags;
	SpatialCell* cell;
};

class GameObject
{
protected:
	mat4 model;

public:
	struct GameObjectData {
		float x, z, rot;
	};

	struct GameObjectData_Small {
		uint16_t x, z;
		uint8_t rot;
	};

	ObjDectData objDectData = {};

	GameObject();
	GameObject(GameObjectData data);

	void setGOData(GameObjectData data);
	void setMatrix(mat4 m);
	mat4 getMatrix();
	vec3 getPosition();

	GameObjectData getData();
	virtual int writeData(char buf[], int index);

	static GameObjectData_Small compressData(GameObjectData data);
	static GameObjectData decompressData(GameObjectData_Small data);
};

struct SpatialCell {
	uint64_t key;
	std::vector<GameObject*> objects;
};