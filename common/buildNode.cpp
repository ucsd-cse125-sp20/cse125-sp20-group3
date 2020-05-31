#include "BuildNode.h"

BuildNode::BuildNode(char team, float x_index, float z_index) : GameObject(){
	GameObjectData data = { x_index * 5.0f + 2.5f, -(z_index * 5.0f + 2.5f), 0.0f };
    this->setGOData(data);
    this->teamColor = team;

	int flags = DETECTION_FLAG_BUILD_NODE;
	if (team == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
	else flags = flags | DETECTION_FLAG_BLUE_TEAM;
	//std::cout << "buildNode at " << getPosition().getX() << " " << getPosition().getZ() << "\n";
	ObjectDetection::addObject(this, flags);
}