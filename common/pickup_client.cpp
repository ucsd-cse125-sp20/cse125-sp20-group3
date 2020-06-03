#include "pickup_client.h"
#include "../client/src/SceneManager_Client.h"
#include "../client/src/AudioManager.h"

namespace {
	const char* pickupActions[1] = { "Spin?" };
}

Pickup_Client::Pickup_Client(char resourceType, GameObjectData data, int id, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent) : Pickup(resourceType, data, id, nullptr), Entity_Client(sm_c)
{
	type = resourceType;
	//animator = conf_new(Animator, geode);
	//animator->SetClip(pickupActions[0]);
	//parent->addChild(animator);

	//printf("%d %s\n", animator->clipControllers[animator->currClip]->GetLoop(), animator->currClip.c_str());

	//particleTransform = conf_new(Transform);
	//particleTransform->addChild(particles);
	//parent->addChild(particleTransform);
	//particleTransform->active = false;

	parent->addChild(geode);
}

Pickup_Client::~Pickup_Client()
{
	//conf_delete(animator);
	//conf_delete(particleTransform);
}

void Pickup_Client::updateAnimParticles() {
	this->processAction(this->actionState);
}

void Pickup_Client::idleAction() {
	//AudioManager::playAudioSource(this->getPosition(), "collect");
}

void Pickup_Client::moveAction() {

}

void Pickup_Client::attackAction() {

}

void Pickup_Client::fireAction() {

}