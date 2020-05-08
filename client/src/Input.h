#pragma once

#include <ctime>

#include "../../common/client2server.h"
#include "../../common/macros.h"

#include "../The-Forge/Common_3/OS/Interfaces/ICameraController.h"
#include "../The-Forge/Common_3/OS/Interfaces/IOperatingSystem.h"
#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/OS/Interfaces/IInput.h"
#include "../The-Forge/Common_3/OS/Interfaces/IApp.h"

enum InputEnum {
	INPUT_UP,		// W
	INPUT_DOWN,		// S
	INPUT_LEFT,		// A
	INPUT_RIGHT,	// D
	INPUT_SELECT_1, // 1
	INPUT_SELECT_2,	// 2
	INPUT_SELECT_3,	// 3
	INPUT_ACTION_1,	// E
	INPUT_ACTION_2,	// R
	INPUT_ACTION_3,	// F
	INPUT_ACTION_4,	// Space
	INPUT_LAST
};

class Input {
public:

	static UIApp* appUI;
	static IApp* app;
	static ICameraController* camera;

	static float inputs[InputEnum::INPUT_LAST];

	static bool Init(WindowsDesc* window, UIApp* appUI, IApp* app, ICameraController* camera);

	static void Update(int32_t width, int32_t height);

	static int EncodeToBuf(char buf[]);

	static void Exit();
};