#include "WallNode.h"
WallNode::WallNode(float x_index, float z_index): GameObject(){
    GameObjectData data = {x_index * 5.0f + 2.5f, z_index * 5.0f + 2.5f, 0.0f};
    this->setGOData(data);

	int flags = DETECTION_FLAG_WALL_NODE | DETECTION_FLAG_COLLIDABLE;
    ObjectDetection::addObject(this, flags, -2.5f, 2.5f, -2.5f, 2.5f);

	std::cout << "WallNode at x: " << data.x << " z: " << data.z << "\n";
}