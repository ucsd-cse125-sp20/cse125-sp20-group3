#ifndef _CLAW_TOWER_H_
#define _CLAW_TOWER_H_

#include "tower.h"

class ClawTower : public Tower {
private:
	vec3 spawnPoint;
	float spawnInterval;
    
public:
	ClawTower(GameObjectData data, int id, Team* t, SceneManager_Server* sm_server);

	void update(float deltaTime) override;

	void setEntData(EntityData data) override;
};

#endif
