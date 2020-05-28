#ifndef _PATH_NODE_H_
#define _PATH_NODE_H_

#include "GameObject.h"
#include "ObjectDetection.h"

class PathNode : public GameObject {
    public:
        PathNode* next_red = nullptr;
        PathNode* next_blue = nullptr;

        PathNode(float x_index, float z_index);
        void setNextRed(PathNode *next_red_node);
        void setNextBlue(PathNode *next_blue_node);
};
#endif