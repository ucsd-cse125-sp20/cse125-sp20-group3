#include "wallNode.h"
wallNode::wallNode(float x_index, float z_index): GameObject(){
    GameObjectData data = {x_index * 5.0f + 2.5f, z_index * 5.0f + 2.5f, 0.0f};
    this->setGOData(data);

    ObjectDetection::addObject(this, DETECTION_FLAG_WALL_NODE);
}