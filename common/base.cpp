#include "base.h"
#include "../server/SceneManager_Server.h"

Base::Base(SceneManager_Server* sm) : Entity(BASE_HEALTH, BASE_ATTACK, sm) {
    //init stuff
}

void Base::update(float deltaTime) {
        //TODO send data about health
}

/*void Base::updateHealth(int attack) {
        health = health-attack;
}

bool Base::isEnemyTeam(Team checkTeam) {
        //need to overload the equals operator for Team
        if ( checkTeam != team) {
                return true;
        }
}*/
