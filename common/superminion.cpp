#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(int id, Team* t, SceneManager_Server* sm) : Minion(id, SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, SUPER_MINION_ATK_INTERVAL, SUPER_MINION_VELOCITY, t, sm) {}