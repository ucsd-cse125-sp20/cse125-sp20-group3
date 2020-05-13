#include "ObjectDetection.h"

std::unordered_map<uint64_t, SpatialCell> ObjectDetection::spatialHash = std::unordered_map<uint64_t, SpatialCell>();

uint64_t ObjectDetection::keyOf(GameObject* obj)
{
	GameObject::GameObjectData data = obj->getData();
	return keyOf(vec2(data.x, data.z));
}

uint64_t ObjectDetection::keyOf(vec2 position)
{
	return (uint64_t(floor(position[0] / SPATIAL_HASH_SIZE)) << 32) | (uint64_t(floor(position[1] / SPATIAL_HASH_SIZE)) & 0xFFFFFFFF);
}

void ObjectDetection::addObject(GameObject* obj, int flag)
{
	SpatialCell cell;
	auto key = keyOf(obj);
	if (spatialHash.find(key) == spatialHash.end()) {
		cell.objects = std::vector<std::pair<int, GameObject*>>();
		spatialHash[key] = cell;
	}
	else {
		cell = spatialHash[key];
	}
	cell.objects.push_back(std::make_pair(flag, obj));
}

void ObjectDetection::updateObject(GameObject* obj, int flags)
{
	// TODO
}

void ObjectDetection::removeObject(GameObject* obj)
{
	// TODO
}

GameObject* ObjectDetection::getNearestObject(vec2 position, int flags, int radius)
{
	int checkRadius = radius / SPATIAL_HASH_SIZE + 1;

	uint64_t key = keyOf(position);
	int x = key >> 32;
	int z = key & 0xFFFFFFFF;
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = (((uint64_t)i) << 32) | ((uint64_t)j & 0xFFFFFFFF);
			if (spatialHash.find(key) != spatialHash.end()) {
				for (auto ce : spatialHash[key].objects) {
					if (flags == DETECTION_FLAG_NONE || (flags & ce.first) == flags) {
						GameObject::GameObjectData data = ce.second->getData();
						vec2 objPosition = vec2(data.x, data.z);
						if (length(position - objPosition) < shortestDistance) {
							closestObject = ce.second;
						}
					}
				}
			}
		}
	}
	
	return closestObject;
}

GameObject* ObjectDetection::getNearestObject(GameObject* ref, int flags, int radius)
{
	int checkRadius = radius / SPATIAL_HASH_SIZE + 1;

	GameObject::GameObjectData data = ref->getData();
	vec2 position = vec2(data.x, data.z);

	uint64_t key = keyOf(ref);
	int x = key >> 32;
	int z = key & 0xFFFFFFFF;
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = (((uint64_t)i) << 32) | ((uint64_t)j & 0xFFFFFFFF);
			if (spatialHash.find(key) != spatialHash.end()) {
				for (auto ce : spatialHash[key].objects) {
					if (ref != ce.second && (flags == DETECTION_FLAG_NONE || (flags & ce.first) == flags)) {
						GameObject::GameObjectData data = ce.second->getData();
						vec2 objPosition = vec2(data.x, data.z);
						if (length(position - objPosition) < shortestDistance) {
							closestObject = ce.second;
						}
					}
				}
			}
		}
	}

	return closestObject;
}

std::vector<GameObject*> ObjectDetection::getCollisions(GameObject* ref)
{
	// TODO
	return std::vector<GameObject*>();
}
