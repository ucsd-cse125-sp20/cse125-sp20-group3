#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(std::string id, SceneManager_Server* sm) : Minion(id, SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, sm) {}