#ifndef _CLAW_TOWER_H_
#define _CLAW_TOWER_H_

#include "tower.h"

class ClawTower : public Tower {
private:
	float timeElapsed;
	vec3 spawnOffset;
	float spawnInterval;
    
public:
	ClawTower(std::string id, SceneManager_Server* sm_server);

	void update(float deltaTime) override;
};

#endif
