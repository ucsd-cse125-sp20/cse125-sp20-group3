#include "OBJObject.h"
#include "Player.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Team {
private:
        Team enemyTeam;
        Player player1;
        Player player2;
        int baseHealth;
        int unitCount;
        int towerCount;
        //TODO: Add resources
public:
        Team(std::string objFilename);
        Team();
        ~Team();
        void update();
        void incTower(int amount);
        void incUnit(int amount);
        void checkResources(int attack);
};
