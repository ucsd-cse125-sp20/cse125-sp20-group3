#include "Input.h"

float Input::inputs[] = {0.0, 0.0, 0.0, 0.0};
UIApp* Input::appUI = NULL;
IApp* Input::app = NULL;

bool Input::Init(WindowsDesc* window, UIApp* appUI, IApp* app)
{
	Input::appUI = appUI;
	Input::app = app;

	if (!initInputSystem(window))
		return false;

	// App Actions
	InputActionDesc actionDesc = { InputBindings::BUTTON_FULLSCREEN, [](InputActionContext* ctx) { toggleFullscreen(((IApp*)ctx->pUserData)->pWindow); return true; }, app };
	addInputAction(&actionDesc);

	actionDesc = { InputBindings::BUTTON_EXIT, [](InputActionContext* ctx) { requestShutdown(); return true; } };
	addInputAction(&actionDesc);
	
	typedef bool(*StickInputHandler)(InputActionContext* ctx);
	static StickInputHandler onStickInput = [](InputActionContext* ctx)
	{
		if (!Input::appUI->IsFocused() && *ctx->pCaptured)
		{
			if (ctx->mFloat2.x > 0.0) {
				inputs[INPUT_RIGHT] = 1.0f;
			}
			else {
				inputs[INPUT_RIGHT] = 0.0f;
			}
			if (ctx->mFloat2.x < 0.0) {
				inputs[INPUT_LEFT] = 1.0f;
			}
			else {
				inputs[INPUT_LEFT] = 0.0f;
			}
			if (ctx->mFloat2.y > 0.0) {
				inputs[INPUT_UP] = 1.0f;
			}
			else {
				inputs[INPUT_UP] = 0.0f;
			}

			if (ctx->mFloat2.y < 0.0) {
				inputs[INPUT_DOWN] = 1.0f;
			}
			else {
				inputs[INPUT_DOWN] = 0.0f;
			}
			//printf("%f %f\n", ctx->mFloat2.x, ctx->mFloat2.y);
		}
		return true;
	};
	actionDesc = { InputBindings::FLOAT_LEFTSTICK, [](InputActionContext* ctx) { return onStickInput(ctx); }, NULL, 20.0f, 200.0f, 0.25f };
	addInputAction(&actionDesc);

	return true;
}

void Input::Update(int32_t width, int32_t height)
{
	updateInputSystem(width, height);
}

void Input::EncodeToBuf(char buf[])
{
	// TODO redefine when action encoding is in place
	// Possibly might be able to store encoding buffer in state, and only update
	buf[0] = inputs[INPUT_UP] ? '1' : '0';
	buf[1] = inputs[INPUT_LEFT] ? '1' : '0';
	buf[2] = inputs[INPUT_DOWN] ? '1' : '0';
	buf[3] = inputs[INPUT_RIGHT] ? '1' : '0';
}

void Input::Exit()
{
	exitInputSystem();
}
