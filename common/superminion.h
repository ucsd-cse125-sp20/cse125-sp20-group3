#ifndef _SUPER_MINION_H_
#define _SUPER_MINION_H_

#include "minion.h"

class SuperMinion : public Minion {
public:
	SuperMinion(int id, Team* t, SceneManager_Server* sm_server);
};

#endif
