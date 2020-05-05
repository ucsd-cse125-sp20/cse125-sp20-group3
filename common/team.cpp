#include "team.h"

Team::Team(std::string objFilename) {
    Team::Team();
}

Team::Team() {
    baseHealth = 100;
    unitCount = 0;
    towerCount = 0;
}

void Team::update() {
    //send the base health, unitCount and towerCount
}

void Team::incTower(int amount) {
    towerCount+=amount;
}

void Team::incUnit(int amount) {
    unitCount+=amount;
}

bool Team::checkResources(char entityType) {
    //find a better way to determine type of entity
    //perhaps enums would be a good way? 
    if (entityType == MINION_TYPE) {
        //check if there are enough resources to spawn unit
		return true;
    }
    else if (entityType == TOWER_TYPE) {
        //check if there are enough resources to spawn tower
		return true;
    }
    else {
        //error, string not recognized
		return false;
    }
}
