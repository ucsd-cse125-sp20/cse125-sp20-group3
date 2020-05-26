#ifndef _BUILD_NODE_H_
#define _BUILD_NODE_H_

#include "macros.h"
#include "GameObject.h"
#include "ObjectDetection.h"

class BuildNode : public GameObject {
    public:
        char teamColor;
        BuildNode(char team, float x_index, float z_index);
};
#endif