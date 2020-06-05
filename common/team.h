#ifndef _TEAM_H_
#define _TEAM_H_

#include "macros.h"
#include <iostream>

class Team {
private:
	int metalCount;
	int plasticCount;
    int baseHealth;

public:

#if defined(USE_SMALL_DATA)
	struct TeamData {
		char teamColor;
		uint16_t metalCount;
		uint16_t plasticCount;
		uint16_t baseHealth;
	};
#else
	struct TeamData {
		char teamColor;
		int metalCount;
		int plasticCount;
		int baseHealth;
	};
#endif

    char teamColor;
    Team(char color);
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