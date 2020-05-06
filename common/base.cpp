#include "base.h"

Base::Base() : Entity(BASE_HEALTH, BASE_ATTACK) {
    //init stuff
}

//Not entirely sure why we need this constructor but put it in
//for consistency
Base::Base(mat4 model_mat) : Entity(BASE_HEALTH, BASE_ATTACK, model_mat) {
	lastTime = std::chrono::steady_clock::now();
}

void Base::update() {
        //TODO send data about health
}

/*void Base::updateHealth(int attack) {
        health = health-attack;
        if (health == 0) {
                //lose the game
        }
}

bool Base::isEnemyTeam(Team checkTeam) {
        //need to overload the equals operator for Team
        if ( checkTeam != team) {
                return true;
        }
}*/
