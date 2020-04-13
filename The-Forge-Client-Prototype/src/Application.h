#ifndef _Application_H_
#define _Application_H_

#include "../The-Forge/Common_3/OS/Interfaces/IApp.h"
#include "OBJObject.h"

class Application : public IApp
{
public:
    static uint32_t gImageCount, gFrameIndex;
    static mat4 projMat, viewMat;

    bool Init();
	bool InitializeShaderPrograms();
	bool InitializeObjects();
    void Exit();

    bool Load();
    void Unload();
    void Update(float deltaTime);
    void Draw();

    const char* GetName() { return "Application"; }

    bool addSwapChain();
    bool addDepthBuffer();
};

#endif