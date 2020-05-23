#ifndef _LASER_TOWER_H_
#define _LASER_TOWER_H_

#include "tower.h"

#define LASER_ACTION_IDLE 0
#define LASER_ACTION_ATTACK 1
#define LASER_ACTION_FIRE 2

class LaserTower : public Tower {
private:
	float timeElapsed;
	Entity* attackTarget;
	int attackTargetID;
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
