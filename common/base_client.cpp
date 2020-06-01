#include "base_client.h"
#include "../client/src/SceneManager_Client.h"

/*namespace {
	const char* baseActions[1] = { "Something" };
}*/

Base_Client::Base_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent) : Base(data, id, t, nullptr), Entity_Client(sm_c)
{
	//animator = conf_new(Animator, geode);
	//animator->SetClip(baseActions[0]);
	//parent->addChild(animator);
	parent->addChild(geode); //only if GLTFGeode
}

Base_Client::~Base_Client()
{
	//conf_delete(animator);
}

void Base_Client::updateAnimParticles() {
	//maybe play explosion animation or something on destruction?
}

void Base_Client::idleAction() {

}

void Base_Client::moveAction() {

}

void Base_Client::attackAction() {

}

void Base_Client::fireAction() {

}