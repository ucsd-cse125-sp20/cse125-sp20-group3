#pragma once

#include "base.h"
#include "entity_client.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

class Base_Client : public Base, public Entity_Client {
public:
	//Animator* animator;

	Base_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent);
	~Base_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};