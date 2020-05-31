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
	struct TeamData {
		char teamColor;
		int metalCount;
		int plasticCount;
		int baseHealth;
		int minionCount;
		int towerCount;
	};

    char teamColor;
    Team(char color);
	void incMinion();
	void decMinion();
    void incTower();
	void decTower();
	int getPlasticCount();
	int getMetalCount();
	int getBaseHealth();
    bool checkResources(char entityType);
	void buildEntity(char entityType);
	void addResource(char type, int amount);

	void setData(TeamData data);
	int writeData(char buf[], int index);
	void print();
};

#endif