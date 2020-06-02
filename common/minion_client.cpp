#include "minion_client.h"
#include "../client/src/SceneManager_Client.h"
#include "../client/src/AudioManager.h"

namespace {
	const char* smallMinionActions[2] = { "Walking", "Fighting" };
}

Minion_Client::Minion_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, ParticleSystemGeode* bullets, Transform* parent) : Minion(data, id, t, nullptr), Entity_Client(sm_c)
{
	this->bullets = bullets;

	bulletTransform = conf_new(TimedTransform, mat4::rotationY(PI) * mat4::translation(vec3(0.f, 0.25f, 0.05f)));
	bulletTransform1 = conf_new(Transform, mat4::translation(MINION_BULLET_OFFSET1));
	bulletTransform2 = conf_new(Transform, mat4::translation(MINION_BULLET_OFFSET2));
	bulletTransform->addChild(bulletTransform1);
	bulletTransform->addChild(bulletTransform2);
	bulletTransform1->addChild(bullets);
	bulletTransform2->addChild(bullets);

	animator = conf_new(Animator, geode);
	animator->SetClip(smallMinionActions[0]);
	parent->addChild(animator);
	parent->addChild(bulletTransform);
}

Minion_Client::~Minion_Client()
{
	conf_delete(bulletTransform);
	conf_delete(bulletTransform1);
	conf_delete(bulletTransform2);

	conf_delete(animator);
}

void Minion_Client::updateAnimParticles() {
	this->processAction(this->actionState);
}

void Minion_Client::idleAction() {

}

void Minion_Client::moveAction() { 
	animator->SetClip(smallMinionActions[0]);
}

void Minion_Client::attackAction() { 
	animator->SetClip(smallMinionActions[1]);
}

void Minion_Client::fireAction() {
	/*((ParticleSystem*)bullets->obj)->reset(0, 1);
	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	animator->SetClip(smallMinionActions[1]);*/
	((ParticleSystem*)bullets->obj)->reset(0, 1);

	//bulletTransform->setPositionDirection(getPosition(), sm_c->getTargetPosition(this->attackTargetID) - getPosition());
	//mat4 currMat = this->getMatrix();
	//mat4 inverseMat = inverse(currMat);
	//bulletTransform->setMatrix(inverseMat * mat4::translation(this->getPosition()) * bulletTransform->getMatrix());

	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	animator->SetClip(smallMinionActions[1]);
	AudioManager::playAudioSource(this->getPosition(), "bullets", 0.5f);
}

/* legacy code */
/*
void Minion_Client::shoot()
{
	((ParticleSystem*)bullets->obj)->reset(0, 1);
	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	animator->SetClip(smallMinionActions[1]);
}

void Minion_Client::shoot(vec3 target)
{
	((ParticleSystem*)bullets->obj)->reset(0, 1);
	animator->SetClip(smallMinionActions[1]);

	bulletTransform->setPositionDirection(getPosition(), target - getPosition());
	mat4 currMat = this->getMatrix();
	mat4 inverseMat = inverse(currMat);
	bulletTransform->setMatrix(inverseMat * mat4::translation(this->getPosition()) * bulletTransform->getMatrix());

	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	//AudioManager::playAudioSource(this->getPosition(), "minion fire");
}
*/
