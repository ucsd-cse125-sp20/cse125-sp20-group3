#pragma once

#include <unordered_map>
#include <vector>
#include <math.h>

#include "GameObject.h"



// This number must be larger than the largest object/bounding box in the scene
#define SPATIAL_HASH_SIZE 10


#define DETECTION_FLAG_NONE 1 << 0
#define DETECTION_FLAG_TEAM0 1 << 1
#define DETECTION_FLAG_TEAM1 1 << 2
#define DETECTION_FLAG_ENTITY 1 << 3
#define DETECTION_FLAG_PLAYER 1 << 4
#define DETECTION_FLAG_MINION 1 << 5
#define DETECTION_FLAG_TOWER 1 << 6
#define DETECTION_FLAG_COLLIDABLE 1 << 7




struct SpatialCell {
	std::vector<std::pair<int, GameObject*>> objects;
};

class ObjectDetection {
public:
	static std::unordered_map<uint64_t, SpatialCell> spatialHash;

	static uint64_t keyOf(GameObject* obj);
	static uint64_t keyOf(vec2 position);

	static void addObject(GameObject* obj, int flags=DETECTION_FLAG_NONE);
	static void updateObject(GameObject* obj, int flags=0);
	static void removeObject(GameObject* obj);

	static GameObject* getNearestObject(vec2 position, int flags=DETECTION_FLAG_NONE, int radius=SPATIAL_HASH_SIZE);
	static GameObject* getNearestObject(GameObject* ref, int flags=DETECTION_FLAG_NONE, int radius=SPATIAL_HASH_SIZE);

	static std::vector<GameObject*> getCollisions(GameObject* ref);
};