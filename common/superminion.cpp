#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Minion(data, id, SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, SUPER_MINION_ATK_INTERVAL, SUPER_MINION_VELOCITY, t, sm) {}