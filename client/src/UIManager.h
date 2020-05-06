#pragma once

#include "Object.h"

#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/imgui/imgui.h"

class UIManager : public Object {
public:
	GuiComponent* pDebugGui;
	GuiComponent* pTestGui;

	Texture* testImage;

	UIApp gAppUI;

	virtual void update(float deltaTime);
	virtual void draw(Cmd* commands);
};

class TextureButtonWidget : public IWidget
{
public:
	TextureButtonWidget(const eastl::string& _label) :
		IWidget(_label),
		mTextureDisplaySize(float2(512.f, 512.f)),
		mTexture(NULL) {}

	IWidget* Clone() const;
	void     Draw();

	void SetTexture(Texture* const& texture, float2 const& displaySize)
	{
		mTexture = texture;
		mTextureDisplaySize = displaySize;
	}

private:
	Texture* mTexture;
	float2 mTextureDisplaySize;
};