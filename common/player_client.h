#pragma once

#include "player.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <algorithm>

#define PLAYER_IDLE_THRESHOLD 0.1f

class Player_Client : public Player {
public:
	Animator* animator;
	Transform* rotator;

	Player_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm, OzzGeode* geode, Transform* parent);
	~Player_Client();

	void setEntData(EntityData data) override;
};