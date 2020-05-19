#ifndef _LASER_TOWER_H_
#define _LASER_TOWER_H_

#include "tower.h"

class LaserTower : public Tower {
private:
	float timeElapsed;
	Entity* attackTarget;
	int attackRange;
	float attackInterval;
public:
	LaserTower(std::string id, Team* t, SceneManager_Server* sm_server);

	void update(float deltaTime) override;
	void attack();

	/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
	void setAttackTarget(Entity* e);
};

#endif
