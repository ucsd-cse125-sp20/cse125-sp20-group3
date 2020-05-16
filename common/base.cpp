#include "base.h"
#include "../server/SceneManager_Server.h"

Base::Base(std::string id, SceneManager_Server* sm) : Entity(id, BASE_HEALTH, BASE_ATTACK, sm) {
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
