#ifndef _CLAW_TOWER_H_
#define _CLAW_TOWER_H_

#include "tower.h"

class ClawTower : public Tower {
private:
	vec3 spawnOffset;
	float timeElapsed;
	float spawnInterval;
    
public:
	ClawTower(SceneManager_Server* sm_server);
	ClawTower(SceneManager_Server* sm_server, mat4 model_mat);
	void update(float deltaTime) override;
};

#endif
