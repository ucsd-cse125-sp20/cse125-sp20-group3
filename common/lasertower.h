#ifndef _LASER_TOWER_H_
#define _LASER_TOWER_H_

#include "tower.h"

class LaserTower : public Tower {
private:
	float timeElapsed;
	Entity* attackTarget;
	float attackRange;
	float attackInterval;
public:
	LaserTower(SceneManager_Server* sm_server);
	//LaserTower(SceneManager_Server* sm_server, mat4 model_mat);
	void update(float deltaTime) override;
};

#endif
