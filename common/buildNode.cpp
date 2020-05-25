#include "buildNode.h"

buildNode::buildNode(char team, float x_index, float z_index) : GameObject(){
	GameObjectData data = { x_index * 5.0f + 2.5f, z_index * 5.0f + 2.5f, 0.0f };
    this->setGOData(data);
    this->teamColor = team;

	int flags = DETECTION_FLAG_BUILD_NODE;
	if (team == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
	else flags = flags | DETECTION_FLAG_BLUE_TEAM;
	ObjectDetection::addObject(this, flags);
}