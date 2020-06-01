#pragma once

#include "player.h"
#include "entity_client.h"
#include "MathUtils.h"
#include "../client/src/OzzGeode.h"
#include "../client/src/Animator.h"

#include <map>
#include <algorithm>

#define PLAYER_IDLE_THRESHOLD 0.1f

class Player_Client : public Player, public Entity_Client {
public:
	Animator* animator;
	Transform* rotator;

	Transform* previewTransform;
	std::map<BUILD_MODE, Node*> previews;

	Player_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm, OzzGeode* geode, Transform* parent);
	~Player_Client();

	void setPreview(BUILD_MODE mode, Node* obj);

	void updateAnimParticles() override {}
	void idleAction() override {}
	void moveAction() override {}
	void attackAction() override {}
	void fireAction() override {}

	void setEntData(EntityData data) override;
};