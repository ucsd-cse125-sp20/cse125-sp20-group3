#include "superminion_client.h"
#include "../client/src/SceneManager_Client.h"
#include "../client/src/AudioManager.h"

namespace {
	const char* superMinionActions[2] = { "Walking", "Fighting" };
}

SuperMinion_Client::SuperMinion_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent) : SuperMinion(data, id, t, nullptr), Entity_Client(sm_c)
{
	animator = conf_new(Animator, geode);
	animator->SetClip(superMinionActions[0]);
	parent->addChild(animator);
}

SuperMinion_Client::~SuperMinion_Client()
{
	conf_delete(animator);
}

void SuperMinion_Client::updateAnimParticles() {
	this->processAction(this->actionState);
}

void SuperMinion_Client::idleAction() {
	audioFrame = true;
}

void SuperMinion_Client::moveAction() {
	animator->SetClip(superMinionActions[0]);
	audioFrame = true;
}

void SuperMinion_Client::attackAction() {
	animator->SetClip(superMinionActions[1]);
	audioFrame = true;
}

void SuperMinion_Client::fireAction() {
	if (audioFrame) AudioManager::playAudioSource(this->getPosition(), "thud", 0.75f);
}
