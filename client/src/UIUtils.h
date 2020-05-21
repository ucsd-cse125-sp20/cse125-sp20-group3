#pragma once

#include "Object.h"

#include "../The-Forge/Middleware_3/UI/AppUI.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/imgui/imgui.h"
#include <map>
#include "../The-Forge/Common_3/Renderer/IResourceLoader.h"
#include <string>

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

	struct TextDrawData {
		std::string text;
		float2 position;
		TextDrawDesc desc;
	};

	static std::map<std::string, Texture*> textures;
	static std::map<std::string, int> fonts;
	static std::map<std::string, GuiComponent*> guis;
	static std::map<std::string, TextureButtonWidget*> images;
	static std::map<std::string, TextDrawData> texts;
	
	// Create an image at a specified position
	static void createImage(std::string label, std::string filename, float x, float y, UIApp app, float scale=1.0f);

	// Adds a callback for when the image is clicked
	static void addCallbackToImage(std::string label, WidgetCallback cb);

	// Change the image in an existing component
	static void changeImage(std::string label, std::string filename, float scale=1.0f);

	// Remove an image container so it isn't drawn
	static void removeImage(std::string label);

	// Create screen space text
	static void createText(std::string label, std::string text, float x, float y, std::string font, float size = 30, uint32_t color=0xffffffff);

	// Change text content
	static void editText(std::string label, std::string text);

	// Remove a text element
	static void removeText(std::string label);


	// Load a texture for the UI
	static void loadTexture(std::string filename);

	// Load a font file
	static void loadFont(std::string label, std::string filename, UIApp app);


	// Create an empty widget container
	static void createGuiComponent(std::string label, GuiDesc desc, UIApp app);

	// Deallocate resources
	static void unload();

	// Set styling parameter
	static void setStyleColor(ImGuiCol_ component, float4 color);

	// Set components to be drawn
	static void drawImages(Cmd* cmd, UIApp app);
	static void drawText(Cmd* cmd, UIApp app);
};