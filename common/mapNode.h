#ifndef _MAPNODE_H_
#define _MAPNODE_H_
#include "GameObject.h"

class mapNode : public GameObject {
    public:
        mapNode* next_red = nullptr;
        mapNode* next_blue = nullptr;
        mapNode(float xpos, float zpos);
        void setNextRed(mapNode *next_red_node);
        void setNextBlue(mapNode *next_blude_node);
};
#endif