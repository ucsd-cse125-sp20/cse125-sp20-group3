#pragma once

#include "superminion.h"
#include "entity_client.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

class SuperMinion_Client : public SuperMinion, public Entity_Client {
public:
	Animator* animator;

	bool audioFrame = true;

	SuperMinion_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm, OzzGeode* geode, Transform* parent);
	~SuperMinion_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};