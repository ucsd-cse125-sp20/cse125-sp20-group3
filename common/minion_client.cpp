#include "minion_client.h"

namespace {
	const char* smallMinionActions[2] = { "Walking", "Fighting" };
}

Minion_Client::Minion_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm, OzzGeode* geode, ParticleSystemGeode* bullets, Transform* parent) : Minion(data, id, t, sm)
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

void Minion_Client::shoot()
{
	((ParticleSystem*)bullets->obj)->reset(0, 1);
	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	animator->SetClip(smallMinionActions[1]);
}

void Minion_Client::kill()
{
}
