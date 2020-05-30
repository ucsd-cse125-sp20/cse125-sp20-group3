#pragma once

#include "clawtower.h"
#include "entity_client.h"
#include "../client/src/TimedTransform.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

class ClawTower_Client : public ClawTower, public Entity_Client {
public:
	Animator* animator;

	ClawTower_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, GLTFGeode* geode, Transform* parent);
	~ClawTower_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};