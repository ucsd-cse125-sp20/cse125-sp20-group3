#include "spawnNode.h"
spawnNode::spawnNode(char team, float xpos, float zpos) : GameObject(){
    GameObjectData data = {xpos, zpos, 0.0};
    this->setGOData(data);
    ObjectDetection::addObject(this, DETECTION_FLAG_SPAWN_NODE);
    this->teamColor = team;
}