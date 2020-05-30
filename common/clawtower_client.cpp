#include "clawtower_client.h"
#include "../client/src/SceneManager_Client.h"

namespace {
	const char* clawTowerActions[1] = { "Spawning" };
}

ClawTower_Client::ClawTower_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, GLTFGeode* geode, Transform* parent) : ClawTower(data, id, t, nullptr), Entity_Client(sm_c)
{
	//animator = conf_new(Animator, geode);
	//animator->SetClip(clawTowerActions[0]);
	//parent->addChild(animator);
	parent->addChild(geode); //only if GLTFGeode
}

ClawTower_Client::~ClawTower_Client()
{
	//conf_delete(animator);
}

void ClawTower_Client::updateAnimParticles() {
	//claw towers don't change anything with actionState
}

void ClawTower_Client::idleAction() {

}

void ClawTower_Client::moveAction() {
	
}

void ClawTower_Client::attackAction() {
	
}

void ClawTower_Client::fireAction() {
	
}