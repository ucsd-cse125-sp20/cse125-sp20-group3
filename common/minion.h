#include "entity.h"
#include "team.h"
#include <iostream>
#include <chrono>
#include <ctime>

class Minion : public Entity {
private:
        Entity attackTarget;
public:
        Minion(mat4 model_mat, std::string objFilename);
        Minion(mat4 model_mat);
        ~Minion();
        void update() override;
};
