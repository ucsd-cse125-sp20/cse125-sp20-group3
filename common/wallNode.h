#ifndef _WALL_NODE_H_
#define _WALL_NODE_H_

#include "GameObject.h"
#include "ObjectDetection.h"

class wallNode : public GameObject {
    public: 
        wallNode(float x_index, float z_index);
};
#endif