#ifndef _TEAM_H_
#define _TEAM_H_

//#include "GameObject.h"
//#include "Player.h"
#include "macros.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Team {
private:
    /*Player* player1;
    Player* player2;
	Team* enemyTeam;*/
	int winStatus;
	int metalCount;
	int plasticCount;
    int baseHealth;
    int unitCount;
    int towerCount;
    //TODO: Add resources
public:
    Team(std::string objFilename);
    Team();
    void update();
    void incTower();
	void decTower();
    void incUnit();
	void decUnit();
    bool checkResources(char entityType);
};

#endif