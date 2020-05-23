#include "base.h"
#include "../server/SceneManager_Server.h"

Base::Base(int id, Team* t, SceneManager_Server* sm) : Entity(id, BASE_HEALTH, BASE_ATTACK, t, sm) {
    //init stuff
}

void Base::update(float deltaTime) {
        //TODO send data about health
}
