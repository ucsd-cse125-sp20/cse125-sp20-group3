#pragma once

#include "minion.h"
#include "entity_client.h"
#include "../client/src/ParticleSystemGeode.h"
#include "../client/src/TimedTransform.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

#define MINION_BULLET_OFFSET1 vec3(-0.4f, 0.f, 0.f)
#define MINION_BULLET_OFFSET2 vec3(0.4f, 0.f, 0.f)
#define MINION_BULLET_SPRITE "LaserParticle.png"
#define MINION_BULLET_TIMEOUT 0.33f
#define MINION_BULLET_SPEED 5.0f
#define MINION_BULLET_LIFE 0.5f

class Minion_Client : public Minion, public Entity_Client {
public:
	ParticleSystemGeode* bullets;
	TimedTransform* bulletTransform;
	Transform *bulletTransform1, *bulletTransform2;

	bool audioFrame = true;
	Animator* animator;

	Minion_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, ParticleSystemGeode* bullets, Transform* parent);
	~Minion_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};