#include "PathNode.h"

PathNode::PathNode(float x_index, float z_index) : GameObject(){
	GameObjectData data = { x_index * 5.0f + 2.5f, -(z_index * 5.0f + 2.5f), 0.0f };
    this->setGOData(data);

    ObjectDetection::addObject(this, DETECTION_FLAG_PATH_NODE);
}

void PathNode::setNextRed(PathNode *next_red_node){
    this->next_red = next_red_node;
}

void PathNode::setNextBlue(PathNode *next_blue_node){
    this->next_blue = next_blue_node;
}