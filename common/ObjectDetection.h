#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <math.h>

#include "GameObject.h"



// This number must be larger than the largest object/bounding box in the scene
#define SPATIAL_HASH_SIZE 5

#define DETECTION_FLAG_NONE 1 << 0
#define DETECTION_FLAG_RED_TEAM 1 << 1
#define DETECTION_FLAG_BLUE_TEAM 1 << 2
#define DETECTION_FLAG_ENTITY 1 << 3
#define DETECTION_FLAG_PLAYER 1 << 4
#define DETECTION_FLAG_MINION 1 << 5
#define DETECTION_FLAG_TOWER 1 << 6
#define DETECTION_FLAG_RESOURCE 1 << 7
#define DETECTION_FLAG_COLLIDABLE 1 << 8
#define DETECTION_FLAG_MINION_TARGET 1 << 9
#define DETECTION_FLAG_LASER_TARGET 1 << 10
#define DETECTION_FLAG_PATH_NODE 1 << 11
#define DETECTION_FLAG_BUILD_NODE 1 << 12
#define DETECTION_FLAG_WALL_NODE 1 << 13

class ObjectDetection {
private:
	static std::unordered_map<uint64_t, SpatialCell> spatialHash;

	static uint64_t keyOf(GameObject* obj);
	static uint64_t keyOf(vec2 position);
	static uint64_t keyOf(uint64_t x, uint64_t z);
	static std::pair<int, int> unkey(uint64_t key);

public:
	static void addObject(GameObject* obj, int flags=DETECTION_FLAG_NONE, float minX = -0.5f, float maxX = 0.5f, float minZ = -0.5f, float maxZ = 0.5f);
	static void updateObject(GameObject* obj, int flags=0);
	static void removeObject(GameObject* obj);

	static GameObject* getNearestObject(vec2 position, int flags=DETECTION_FLAG_NONE, int radius=SPATIAL_HASH_SIZE);
	static GameObject* getNearestObject(GameObject* ref, int flags=DETECTION_FLAG_NONE, int radius=SPATIAL_HASH_SIZE);

	static std::vector<GameObject*> getCollisions(GameObject* ref, int flags = DETECTION_FLAG_NONE);
};