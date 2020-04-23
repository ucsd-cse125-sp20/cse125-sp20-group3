#pragma once

#include <ctime>

#include "../../common/macros.h"

#include "../The-Forge/Common_3/OS/Interfaces/IOperatingSystem.h"
#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/OS/Interfaces/IInput.h"
#include "../The-Forge/Common_3/OS/Interfaces/IApp.h"

enum InputEnum {
	INPUT_UP,
	INPUT_DOWN,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_LAST
};

class Input {
public:

	static UIApp* appUI;
	static IApp* app;

	static float inputs[InputEnum::INPUT_LAST];

	static bool Init(WindowsDesc* window, UIApp* appUI, IApp* app);

	static void Update(int32_t width, int32_t height);

	static int EncodeToBuf(char buf[]);

	static void Exit();
};