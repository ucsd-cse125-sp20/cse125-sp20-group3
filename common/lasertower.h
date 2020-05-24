#ifndef _LASER_TOWER_H_
#define _LASER_TOWER_H_

#include "tower.h"

class LaserTower : public Tower {
private:
	int attackRange;
	float attackInterval;

public:
	LaserTower(int id, Team* t, SceneManager_Server* sm_server);

	void update(float deltaTime) override;
	void attack();

	void setEntData(EntityData data) override;

	/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
	void setAttackTarget(Entity* e);
};

#endif
