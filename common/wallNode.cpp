#include "wallNode.h"
wallNode::wallNode(float x_index, float z_index): GameObject(){
    GameObjectData data = {x_index*5.0 + 2.5, z_index*5.0 + 2.5, 0.0};
    this->setGOData(data);
    ObjectDetection::addObject(this, DETECTION_FLAG_WALL_NODE);
}