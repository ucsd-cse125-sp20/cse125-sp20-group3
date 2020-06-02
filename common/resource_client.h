#pragma once

#include "resource.h"
#include "entity_client.h"
//#include "../client/src/ParticleSystemGeode.h"
//#include "../client/src/TimedTransform.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

class Resource_Client : public Resource, public Entity_Client {
public:
	Animator* animator;

	char type;

	Resource_Client(char resourceType, GameObjectData data, int id, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent);
	~Resource_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};