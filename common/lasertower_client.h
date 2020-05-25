#pragma once

#include "lasertower.h"
#include "../client/src/ParticleSystemGeode.h"
#include "../client/src/TimedTransform.h"
#include "MathUtils.h"

#include <algorithm>

#define LASER_TOWER_BEAM_OFFSET vec3(0.f, 6.f, 0.f)
#define LASER_TOWER_BEAM_SPRITE "LaserParticle.png"
#define LASER_TOWER_BEAM_TIMEOUT 1.0f
#define LASER_TOWER_BEAM_SPEED 1.0f

class LaserTower_Client : public LaserTower {
public:
	static std::vector<ParticleSystemGeode*> geodes;
	ParticleSystemGeode* laser;
	TimedTransform* laserTransform;

	LaserTower_Client(int id, Team* t, SceneManager_Server* sm_server, Renderer* renderer);
	~LaserTower_Client();

	static void setProgram(Geode::GeodeShaderDesc shader);

	void activate(vec3 target);

	int getTargetID() { return attackTargetID; }
	int getActionState() { return actionState; }
};