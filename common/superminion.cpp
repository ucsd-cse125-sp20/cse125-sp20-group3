#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(SceneManager_Server* sm) : Minion(SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, sm) {}