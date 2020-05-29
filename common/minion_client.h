#pragma once

#include "minion.h"
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
#define MINION_BULLET_SPEED 10.0f

class Minion_Client : public Minion {
public:
	ParticleSystemGeode* bullets;
	TimedTransform* bulletTransform;
	Transform *bulletTransform1, *bulletTransform2;

	Animator* animator;
	bool alive = true;
	int deathCounter = 5;

	Minion_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm, OzzGeode* geode, ParticleSystemGeode* bullets, Transform* parent);
	~Minion_Client();

	void shoot();
	void kill();
};