#ifndef _PATH_NODE_H_
#define _PATH_NODE_H_

#include "GameObject.h"
#include "ObjectDetection.h"

class pathNode : public GameObject {
    public:
        pathNode* next_red = nullptr;
        pathNode* next_blue = nullptr;

        pathNode(float x_index, float z_index);
        void setNextRed(pathNode *next_red_node);
        void setNextBlue(pathNode *next_blue_node);
};
#endif