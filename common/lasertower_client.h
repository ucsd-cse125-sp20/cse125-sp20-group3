#pragma once

#include "lasertower.h"
#include "entity_client.h"
#include "../client/src/ParticleSystemGeode.h"
#include "../client/src/GLTFGeode.h"
#include "../client/src/TimedTransform.h"
#include "MathUtils.h"

#include <algorithm>

#define LASER_TOWER_BEAM_OFFSET vec3(0.f, 6.f, 0.f)
#define LASER_TOWER_BEAM_SPRITE "LaserParticle.png"
#define LASER_TOWER_BEAM_TIMEOUT 1.0f
#define LASER_TOWER_BEAM_SPEED 1.0f

class LaserTower_Client : public LaserTower, public Entity_Client {
public:
	ParticleSystemGeode* laser;
	TimedTransform* laserTransform;

	LaserTower_Client(GameObjectData data, int id, Team* t, SceneManager_Client* sm_c, GLTFGeode* geode, ParticleSystemGeode* laser, Transform* parent);
	~LaserTower_Client();

	void updateAnimParticles() override;
	void idleAction() override;
	void moveAction() override;
	void attackAction() override;
	void fireAction() override;
};