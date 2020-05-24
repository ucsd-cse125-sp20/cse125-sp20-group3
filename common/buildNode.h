#ifndef _BUILD_NODE_H_
#define _BUILD_NODE_H_

#include "macros.h"
#include "GameObject.h"
#include "ObjectDetection.h"

class buildNode : public GameObject {
    public:
        char teamColor;
        buildNode(char team, float x_index, float z_index);
};
#endif