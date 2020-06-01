#ifndef _BUILD_NODE_H_
#define _BUILD_NODE_H_

#include "macros.h"
#include "GameObject.h"
#include "ObjectDetection.h"
class SceneManager_Server;
class BuildNode : public GameObject {
    public:
        char teamColor;
        bool isOccupied();
        int entity_id;
        SceneManager_Server *server;
        BuildNode(char team, float x_index, float z_index, SceneManager_Server *sceneManager_server);
        void build(int build_id);
};
#endif