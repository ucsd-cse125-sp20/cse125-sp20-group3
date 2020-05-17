#include "mapNode.h"
mapNode::mapNode(float xpos, float zpos) : GameObject(){
    GameObjectData data = {xpos, zpos, 0.0};
    this->setGOData(data);
}

void mapNode::setNextRed(mapNode *next_red_node){
    this->next_red = next_red_node;
}

void mapNode::setNextBlue(mapNode *next_blue_node){
    this->next_blue = next_blue_node;
}