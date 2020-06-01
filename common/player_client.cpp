#include "player_client.h"
#include "../client/src/SceneManager_Client.h"

namespace {
	const char* playerActions[2] = { "Idle", "Walking" };
}

Player_Client::Player_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, OzzGeode* geode, Transform* parent) : Player(data, id, t, nullptr), Entity_Client(sm_c)
{
	rotator = conf_new(Transform);
	previewTransform = conf_new(Transform, mat4::translation(vec3(-INTERACT_DISTANCE, 1, 0)) * mat4::scale(vec3(0.5f)));
	previewTransform->setColor(vec4(5.0f, 10.0f, 10.0f, 0.75f));

	animator = conf_new(Animator, geode);
	animator->SetClip(playerActions[0]);
	parent->addChild(rotator);
	rotator->addChild(animator);
	parent->addChild(previewTransform);
}

Player_Client::~Player_Client()
{
	conf_delete(animator);
	conf_delete(rotator);
	conf_delete(previewTransform);
}

void Player_Client::setPreview(BUILD_MODE mode, Node* obj)
{
	previews[mode] = obj;
}

void Player_Client::setEntData(EntityData data)
{
	auto lastBuildMode = getBuildMode();

	Player::setEntData(data);

	vec3 position = getPosition();
	//setMatrix(mat4::translation(position));

	vec3 diff = this->getPosition() - lastPosition;
	if (length(diff) > 0.001f) {
		rotator->setMatrix(inverse(getMatrix()) * mat4::translation(position) * mat4::rotationY(atan2f(-diff[2], diff[0])));
		if (length(diff) > PLAYER_IDLE_THRESHOLD) {
			animator->SetClip(playerActions[1]);
		}
		else {
			animator->SetClip(playerActions[0]);
		}
	}

	auto buildMode = getBuildMode();
	if (buildMode != lastBuildMode) {
		previewTransform->removeChild(previews[lastBuildMode]);
		if (buildMode != BUILD_MODE::NEUTRAL) {
			previewTransform->addChild(previews[buildMode]);
		}
	}
}
