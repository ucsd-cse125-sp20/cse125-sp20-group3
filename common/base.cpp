#include "base.h"
#include "../server/SceneManager_Server.h"

Base::Base(SceneManager_Server* sm) : Entity(BASE_HEALTH, BASE_ATTACK, sm) {
    //init stuff
}

//Not entirely sure why we need this constructor but put it in
//for consistency
Base::Base(SceneManager_Server* sm, mat4 model_mat) : Entity(BASE_HEALTH, BASE_ATTACK, sm, model_mat) {
	//init stuff
}

void Base::update(float deltaTime) {
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
