#include "ObjectDetection.h"

std::unordered_map<uint64_t, SpatialCell> ObjectDetection::spatialHash = std::unordered_map<uint64_t, SpatialCell>();

uint64_t ObjectDetection::keyOf(GameObject* obj)
{
	GameObject::GameObjectData data = obj->getData();
	return keyOf(vec2(data.x, data.z));
}

uint64_t ObjectDetection::keyOf(vec2 position)
{
	return keyOf(uint64_t(floor(position[0] / SPATIAL_HASH_SIZE)), uint64_t(floor(position[1] / SPATIAL_HASH_SIZE)));
}

uint64_t ObjectDetection::keyOf(uint64_t x, uint64_t z)
{
	//printf("%" PRIx64 " %" PRIx64 " %" PRIx64 "\n", x<<32, z, (x << 32) | (z & 0xFFFFFFFF));
	return  (x << 32) | (z & 0xFFFFFFFF);
}

std::pair<int, int> ObjectDetection::unkey(uint64_t key)
{
	int x = key >> 32;
	int z = key & 0xFFFFFFFF;
	return std::make_pair(x, z);
}



// =======================================================================[ Public static methods ]================


void ObjectDetection::addObject(GameObject* obj, int flag, float minX, float maxX, float minZ, float maxZ)
{
	auto key = keyOf(obj);
	if (spatialHash.find(key) == spatialHash.end()) {
		// If cell not in data structure, initialize
		SpatialCell cell;
		cell.objects = std::vector<GameObject*>();
		cell.key = key;
		spatialHash[key] = cell;
	}
	// Add object to cell
	spatialHash[key].objects.push_back(obj);

	//printf("add to cell %" PRIx64 " size %d =====================================================\n", key, spatialHash[key].objects.size());
	
	// Create detection data for object
	ObjDectData data = {};
	data.cell = &spatialHash[key];
	data.flags = flag;
	data.minX = minX;
	data.maxX = maxX;
	data.minZ = minZ;
	data.maxZ = maxZ;
	obj->objDectData = data;
}

void ObjectDetection::updateObject(GameObject* obj, int flags)
{
	SpatialCell oldCell = *obj->objDectData.cell;
	uint64_t currKey = keyOf(obj);
	// Check if the object is in a new cell
	if (currKey != oldCell.key) {
		//printf("new cell %" PRIx64 " =====================================================\n", currKey);
		// Remove object from previous cell
		removeObject(obj);

		// Get new cell (initialize if needed)
		if (spatialHash.find(currKey) == spatialHash.end()) {
			SpatialCell newCell;
			newCell.objects = std::vector<GameObject*>();
			newCell.key = currKey;
			spatialHash[currKey] = newCell;
		}

		// Add object to new cell
		spatialHash[currKey].objects.push_back(obj);

		// Update pointer to occupied cell
		obj->objDectData.cell = &spatialHash[currKey];
	}
}

void ObjectDetection::removeObject(GameObject* obj)
{
	SpatialCell cell = *obj->objDectData.cell;
	cell.objects.erase(std::remove(cell.objects.begin(), cell.objects.end(), obj), cell.objects.end());
}

GameObject* ObjectDetection::getNearestObject(vec2 position, int flags, int radius)
{
	// Get how many cells out of reference cell to search
	int checkRadius = radius / SPATIAL_HASH_SIZE + 1;

	// Get the coords of the reference cell
	uint64_t key = keyOf(position);
	auto coords = unkey(key);
	int x = coords.first;
	int z = coords.second;
	
	// Iterate over nearby cells to find the closest object
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = keyOf(i, j);
			// Check if cell is initialized (if not, skip)
			if (spatialHash.find(key) != spatialHash.end()) {
				// Iterate over all objects in cell
				for (auto otherObj : spatialHash[key].objects) {
					// Verify filters
					if (flags == DETECTION_FLAG_NONE || (flags & otherObj->objDectData.flags) == flags) {
						// Get distance
						GameObject::GameObjectData data = otherObj->getData();
						vec2 objPosition = vec2(data.x, data.z);
						if (length(position - objPosition) < shortestDistance) {
							closestObject = otherObj;
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

	vec3 pos3 = ref->getPosition();
	vec2 position = vec2(pos3[0], pos3[2]);

	uint64_t key = keyOf(ref);
	auto coords = unkey(key);
	int x = coords.first;
	int z = coords.second;
	float shortestDistance = FLT_MAX;
	GameObject* closestObject = NULL;
	for (int i = x - checkRadius; i <= x + checkRadius; i++) {
		for (int j = z - checkRadius; j <= z + checkRadius; j++) {
			key = keyOf(i, j);
			if (spatialHash.find(key) != spatialHash.end()) {
				//printf("%d\n", spatialHash[key].objects.size());
				for (auto otherObj : spatialHash[key].objects) {
					//printf("obj ");
					// Also verify other object is not the current object
					if (ref != otherObj && (flags == DETECTION_FLAG_NONE || (flags & otherObj->objDectData.flags) == flags)) {
						GameObject::GameObjectData data = otherObj->getData();
						vec2 objPosition = vec2(data.x, data.z);
						if (length(position - objPosition) < shortestDistance) {
							closestObject = otherObj;
							shortestDistance = length(position - objPosition);
						}
					}
				}
			}
			//printf("%d %d\n", i, j);
		}
	}

	return closestObject;
}

std::vector<GameObject*> ObjectDetection::getCollisions(GameObject* ref, int flags)
{
	std::vector<GameObject*> collisions = std::vector<GameObject*>();

	vec3 pos3 = ref->getPosition();
	vec2 position = vec2(pos3[0], pos3[2]);
	auto bounds = ref->objDectData;

	uint64_t key = keyOf(ref);
	auto coords = unkey(key);
	int x = coords.first;
	int z = coords.second;

	for (int i = x - 1; i <= x + 1; i++) {
		for (int j = z - 1; j <= z + 1; j++) {
			key = keyOf(i, j);
			if (spatialHash.find(key) != spatialHash.end()) {
				for (auto otherObj : spatialHash[key].objects) {
					auto otherBounds = otherObj->objDectData;
					if (ref != otherObj && (flags == DETECTION_FLAG_NONE || (flags & otherBounds.flags) == flags)) {
						GameObject::GameObjectData data = otherObj->getData();
						vec2 objPosition = vec2(data.x, data.z);
						// AABB collision detection
						if ((position[0] + bounds.minX >= objPosition[0] + otherBounds.maxX ||
							position[0] + bounds.maxX <= objPosition[0] + otherBounds.minX) &&
							(position[2] + bounds.minZ >= objPosition[2] + otherBounds.maxZ ||
							position[2] + bounds.maxZ <= objPosition[2] + otherBounds.minZ)) {
							collisions.push_back(otherObj);
						}
					}
				}
			}
		}
	}

	return std::vector<GameObject*>();
}
