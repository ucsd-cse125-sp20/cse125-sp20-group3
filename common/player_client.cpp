#include "player_client.h"

namespace {
	const char* playerActions[2] = { "Idle", "Walking" };
}

Player_Client::Player_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm, OzzGeode* geode, Transform* parent) : Player(data, id, t, sm)
{
	rotator = conf_new(Transform);

	animator = conf_new(Animator, geode);
	animator->SetClip(playerActions[0]);
	parent->addChild(rotator);
	rotator->addChild(animator);
}

Player_Client::~Player_Client()
{
	conf_delete(animator);
	conf_delete(rotator);
}

void Player_Client::setEntData(EntityData data)
{
	Player::setEntData(data);

	vec3 position = getPosition();
	setMatrix(mat4::translation(position));

	vec3 diff = this->getPosition() - lastPosition;
	if (length(diff) > 0.001f) {
		rotator->setMatrix(mat4::rotationY(atan2f(-diff[2], diff[0])));
		if (length(diff) > PLAYER_IDLE_THRESHOLD) {
			animator->SetClip(playerActions[1]);
		}
		else {
			animator->SetClip(playerActions[0]);
		}
	}
}
