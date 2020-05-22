#ifndef _CLAW_TOWER_H_
#define _CLAW_TOWER_H_

#include "tower.h"

#define CLAW_ACTION_IDLE 0
#define CLAW_ACTION_SPAWN 1

class ClawTower : public Tower {
private:
	float timeElapsed;
	vec3 spawnPoint;
	float spawnInterval;
    
public:
	ClawTower(std::string id, Team* t, SceneManager_Server* sm_server);

	void update(float deltaTime) override;

	void setEntData(EntityData data) override;
};

#endif
