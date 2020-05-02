#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Minion : public Entity {
private:
        Entity attackTarget;
public:
        Minion(std::string objFilename);
        Minion();
        ~Minion();
        void update() override;
};
