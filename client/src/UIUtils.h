#pragma once

#include "Object.h"

#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/imgui/imgui.h"
#include <map>
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include <string>
#include <algorithm>

class TextWidget : public IWidget
{
public:
	ImFont* font;
	uint32_t color;
	std::string text;

	TextWidget(const eastl::string& _label) :
		IWidget(_label),
		font(NULL),
		color(0),
		text("") {}

	IWidget* Clone() const;
	void     Draw();

	void SetText(std::string const& text, ImFont* const font, uint32_t const& color) {
		this->font = font;
		this->text = text;
		this->color = color;
	}
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

class UIUtils : public Object {
public:

	struct WindowDrawData {
		GuiComponent* gui;
		int priority;
	};

	static std::map<std::string, Texture*> textures;
	static std::map<std::string, ImFont*> fonts;
	static std::map<std::string, WindowDrawData> windows;
	static std::map<std::string, TextureButtonWidget*> images;
	static std::map<std::string, TextWidget*> texts;
	
	// Create an image at a specified position
	static void createImage(std::string label, std::string filename, float x, float y, float2 scale=float2(1,1), int priority=0);

	// Adds a callback for when the image is clicked
	static void addCallbackToImage(std::string label, WidgetCallback cb);

	// Change the image in an existing component
	static void changeImage(std::string label, std::string filename, float2 scale = float2(1, 1));

	// Remove an image container so it isn't drawn
	static void removeImage(std::string label);

	// Create screen space text
	static void createText(std::string label, std::string text, float x, float y, std::string font, uint32_t color=0xffffffff, int priority=0);

	// Change text content
	static void editText(std::string label, std::string text, std::string font="", uint32_t color=0xabcdef00);

	// Remove a text element
	static void removeText(std::string label);

	// Load a texture for the UI
	static void loadTexture(std::string filename);

	// Load a font file
	static void loadFont(std::string label, std::string filename, float size);


	// Create an empty widget container
	static void createGuiComponent(std::string label, GuiDesc desc, int priority=0);

	// Deallocate resources
	static void unload();

	// Set styling parameter
	static void setStyleColor(ImGuiCol_ component, float4 color);

	// Set components to be drawn
	static void drawImages(Cmd* cmd);
};