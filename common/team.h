#ifndef _TEAM_H_
#define _TEAM_H_

#include "macros.h"
#include <iostream>

class Team {
private:
	int metalCount;
	int plasticCount;
    int baseHealth;
    int minionCount;
    int towerCount;

public:

#if defined(USE_SMALL_DATA)
	struct TeamData {
		char teamColor;
		uint16_t metalCount;
		uint16_t plasticCount;
		uint16_t baseHealth;
		uint16_t minionCount;
		uint16_t towerCount;
	};
#else
	struct TeamData {
		char teamColor;
		int metalCount;
		int plasticCount;
		int baseHealth;
		int minionCount;
		int towerCount;
	};
#endif

    char teamColor;
    Team(char color);
	void incMinion();
	void decMinion();
    void incTower();
	void decTower();
	int getPlasticCount();
	int getMetalCount();
	void setBaseHealth(int health);
	int getBaseHealth();
    bool checkResources(char entityType);
	void buildEntity(char entityType);
	void addResource(char type, int amount);

	void setData(TeamData data);
	int writeData(char buf[], int index);
	void print();
};

#endif