#include "resource_client.h"
#include "../client/src/SceneManager_Client.h"

namespace {
	const char* resourceActions[2] = { "Open", "Close" };
}

Resource_Client::Resource_Client(char resourceType, GameObjectData data, int id, SceneManager_Client* sm_c, GLTFGeode* geode, Transform* parent) : Resource(resourceType, data, id, nullptr), Entity_Client(sm_c)
{
	//animator = conf_new(Animator, geode);
	//animator->SetClip(resourceActions[0]);
	//parent->addChild(animator);
	parent->addChild(geode);
}

Resource_Client::~Resource_Client()
{
	//conf_delete(animator);
}

void Resource_Client::updateAnimParticles() {
	this->processAction(this->actionState);
}

void Resource_Client::idleAction() {

}

void Resource_Client::moveAction() {
	
}

void Resource_Client::attackAction() {
	
}

void Resource_Client::fireAction() {
	
}