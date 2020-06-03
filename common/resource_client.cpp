#include "resource_client.h"
#include "../client/src/SceneManager_Client.h"
#include "../client/src/AudioManager.h"

namespace {
	const char* dumpsterActions[2] = { "Open", "Close" };
	const char* recyclingBinActions[1] = { "Harvested" };

}

Resource_Client::Resource_Client(char resourceType, GameObjectData data, int id, SceneManager_Client* sm_c, OzzGeode* geode, ParticleSystemGeode* particles, Transform* parent) : Resource(resourceType, data, id, nullptr), Entity_Client(sm_c)
{
	type = resourceType;
	animator = conf_new(Animator, geode);
	animator->SetClip(resourceType == DUMPSTER_TYPE ? dumpsterActions[0] : recyclingBinActions[0]);
	if (type == DUMPSTER_TYPE) animator->SetLoop(false);
	parent->addChild(animator);

	printf("%d %s\n", animator->clipControllers[animator->currClip]->GetLoop(), animator->currClip.c_str());
	active = false;

	particleTransform = conf_new(Transform);
	particleTransform->addChild(particles);
	parent->addChild(particleTransform);
	particleTransform->active = false;
}

Resource_Client::~Resource_Client()
{
	conf_delete(animator);
	conf_delete(particleTransform);
}

void Resource_Client::updateAnimParticles() {
	this->processAction(this->actionState);
}

void Resource_Client::idleAction() {
	if (active) {
		AudioManager::playAudioSource(this->getPosition(), "collect");
	}
	if (type == DUMPSTER_TYPE && active) {
		animator->SetClip(dumpsterActions[1]);
		animator->SetLoop(false);
		animator->SetTime(0);
	}
	else if (type == RECYCLING_BIN_TYPE) {
		animator->SetPlay(false);
		animator->SetTime(0);
	}
	active = false;
	particleTransform->active = false;
}

void Resource_Client::moveAction() {
	
}

void Resource_Client::attackAction() {
	if (type == DUMPSTER_TYPE && !active) {
		animator->SetClip(dumpsterActions[0]);
		animator->SetLoop(false);
		animator->SetTime(0);
	}
	else if (type == RECYCLING_BIN_TYPE) {
		animator->SetClip(recyclingBinActions[0]);
		animator->SetPlay(true);
	}
	active = true;
	particleTransform->active = true;
}

void Resource_Client::fireAction() {
	
}