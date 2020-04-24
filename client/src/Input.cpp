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
	
	actionDesc =
	{
		InputBindings::BUTTON_ANY, [](InputActionContext* ctx)
		{
			static uint8_t virtualKeyboard = 0;
			bool capture = Input::appUI->OnButton(ctx->mBinding, ctx->mBool, ctx->pPosition);
			setEnableCaptureInput(capture && INPUT_ACTION_PHASE_CANCELED != ctx->mPhase);
			if (Input::appUI->WantTextInput() != virtualKeyboard)
			{
				virtualKeyboard = Input::appUI->WantTextInput();
				setVirtualKeyboard(virtualKeyboard);
			}
			return true;
		}, Input::app
	};
	addInputAction(&actionDesc);

	actionDesc = { InputBindings::TEXT, [](InputActionContext* ctx) { return Input::appUI->OnText(ctx->pText); } };
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

int Input::EncodeToBuf(char buf[])
{
	int move_x = (inputs[INPUT_RIGHT] ? 1 : 0) + (inputs[INPUT_LEFT] ? -1 : 0);
	int move_z = (inputs[INPUT_UP] ? 1 : 0) + (inputs[INPUT_DOWN] ? -1 : 0);
	float view_y_rot = 0.0f;

	((PlayerInput*)buf)[0] = { move_x, move_z, view_y_rot };

	return sizeof(PlayerInput);
}

void Input::Exit()
{
	exitInputSystem();
}
