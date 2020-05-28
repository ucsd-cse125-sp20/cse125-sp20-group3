#ifndef _WALL_NODE_H_
#define _WALL_NODE_H_

#include "GameObject.h"
#include "ObjectDetection.h"

class WallNode : public GameObject {
    public: 
        WallNode(float x_index, float z_index);
};
#endif