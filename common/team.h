#ifndef _TEAM_H_
#define _TEAM_H_

#include "macros.h"
#include <iostream>

class Team {
private:
	int winStatus;
	int metalCount;
	int plasticCount;
    int baseHealth;
    int minionCount;
    int towerCount;
public:
    char teamColor;
    Team(char color);
	void incMinion();
	void decMinion();
    void incTower();
	void decTower();
    bool checkResources(char entityType);
};

#endif