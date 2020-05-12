#include "ObjectDetection.h"

std::unordered_map<std::pair<int, int>, SpatialCell> ObjectDetection::spatialHash = std::unordered_map<std::pair<int, int>, SpatialCell>();

std::pair<int, int> ObjectDetection::keyOf(GameObject* obj)
{
	GameObject::GameObjectData data = obj->getData();
	return std::make_pair(int(floor(data.x / SPATIAL_HASH_SIZE)), int(floor(data.z / SPATIAL_HASH_SIZE)));
}

std::pair<int, int> ObjectDetection::keyOf(vec2 position)
{
	return std::make_pair(int(floor(position[0] / SPATIAL_HASH_SIZE)), int(floor(position[1] / SPATIAL_HASH_SIZE)));
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

	auto key = keyOf(position);
	int x = key.first;
	int z = key.second;
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = std::make_pair(i, j);
			if (spatialHash.find(key) != spatialHash.end) {
				for (auto ce : spatialHash[key].objects) {
					if (flags == DETECTION_FLAG_NONE || flags & ce.first == flags) {
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

	auto key = keyOf(ref);
	int x = key.first;
	int z = key.second;
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = std::make_pair(i, j);
			if (spatialHash.find(key) != spatialHash.end) {
				for (auto ce : spatialHash[key].objects) {
					if (ref != ce.second && (flags == DETECTION_FLAG_NONE || flags & ce.first == flags)) {
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
