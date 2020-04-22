#include "OBJObject.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Entity : public OBJObject {
private:
        int health;
        int attack;
        Team team;
public:
        Entity(std::string objFilename);
        Entity();
        ~Entity();
        void update();
        void updateHealth(int attack);
        bool isEnemyTeam(Team checkTeam);
};
