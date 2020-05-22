#ifndef _SPAWNNODE_H_
#define _SPAWNNODE_H_
#include "GameObject.h"
#include "ObjectDetection.h"

class spawnNode : public GameObject {
    public:
        char teamColor;
        spawnNode(char team, float xpos, float zpos);
};
#endif