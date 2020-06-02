#include "BuildNode.h"
#include "../server/SceneManager_Server.h"

BuildNode::BuildNode(char team, float x_index, float z_index, SceneManager_Server *sceneManager_server) : GameObject(){
	GameObjectData data = { x_index * 5.0f + 2.5f, -(z_index * 5.0f + 2.5f), 0.0f };
    this->setGOData(data);
    this->teamColor = team;
	this->server = sceneManager_server;
	this->entity_id = -1;

	int flags = DETECTION_FLAG_BUILD_NODE;
	if (team == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
	else flags = flags | DETECTION_FLAG_BLUE_TEAM;
	//std::cout << "buildNode at " << getPosition().getX() << " " << getPosition().getZ() << "\n";
	ObjectDetection::addObject(this, flags);
}

bool BuildNode::isOccupied(){
	// check if the entity is dead
	if (entity_id == -1){
		return false;
	}
	if (server->checkEntityAlive(entity_id)){
		return true;
	}else{
		entity_id = -1;
		return false;
	}
}

void BuildNode::build(int build_id){
	this->entity_id = build_id;
}