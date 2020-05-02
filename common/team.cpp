#include "team.h"

Team::update() {
        //send the base health, unitCount,
        //and towerCount
}

void Team::incTower(int amount) {
        towerCount+=amount;
}

void Team::incUnit(int amount) {
        unitCounter+=amount;
}

void Team::checkResources(int attack);
