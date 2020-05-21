#include "UIUtils.h"

std::map<std::string, Texture*> UIUtils::textures = std::map<std::string, Texture*>();
std::map<std::string, int> UIUtils::fonts = std::map<std::string, int>();
std::map<std::string, GuiComponent*> UIUtils::guis = std::map<std::string, GuiComponent*>();
std::map<std::string, TextureButtonWidget*> UIUtils::images = std::map<std::string, TextureButtonWidget*>();
std::map<std::string, UIUtils::TextDrawData> UIUtils::texts = std::map<std::string, UIUtils::TextDrawData>();

void UIUtils::createGuiComponent(std::string label, GuiDesc desc, UIApp app)
{
	guis[label] = app.AddGuiComponent(label.c_str(), &desc);
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_TITLE_BAR;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_ALWAYS_USE_WINDOW_PADDING;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_RESIZE;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_COLLAPSE;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_SCROLLBAR;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_MOVE;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_BRING_TO_FRONT_ON_FOCUS;
	guis[label]->mFlags |= GUI_COMPONENT_FLAGS_NO_NAV_FOCUS;
}

void UIUtils::createImage(std::string label, std::string filename, float x, float y, UIApp app, float scale)
{
	GuiDesc desc = {};
	desc.mStartPosition = vec2(x, y);
	UIUtils::createGuiComponent(label, desc, app);
	images[label] = conf_new(TextureButtonWidget, label.c_str());

	changeImage(label, filename, scale);
	UIUtils::guis[label]->AddWidget(images[label]);
}

void UIUtils::addCallbackToImage(std::string label, WidgetCallback cb)
{
	images[label]->pOnActive = cb;
}

void UIUtils::changeImage(std::string label, std::string filename, float scale)
{
	if (textures.find(filename) == textures.end()) UIUtils::loadTexture(filename);
	float width = UIUtils::textures[filename]->mWidth * scale;
	float height = UIUtils::textures[filename]->mHeight * scale;
	images[label]->SetTexture(UIUtils::textures[filename], float2(width, height));
}

void UIUtils::removeImage(std::string label)
{
	// There is probably a better way to do this. Might change
	guis.erase(label);
}

void UIUtils::createText(std::string label, std::string text, float x, float y, std::string font, float size, uint32_t color)
{
	texts[label] = {};
	texts[label].text = text;
	texts[label].position = float2(x, y);
	texts[label].desc.mFontID = fonts[font];
	texts[label].desc.mFontSize = size;
	texts[label].desc.mFontColor = color;
}

void UIUtils::loadTexture(std::string filename)
{
	Texture* tex;
	PathHandle testImagePath = fsCopyPathInResourceDirectory(RD_TEXTURES, filename.c_str());
	TextureLoadDesc texDesc = {};
	texDesc.pFilePath = testImagePath;
	texDesc.ppTexture = &tex;
	addResource(&texDesc, NULL, LOAD_PRIORITY_LOW);
	waitForAllResourceLoads();
	textures[filename] = tex;
}

void UIUtils::loadFont(std::string label, std::string filename, UIApp app)
{
	if (fonts.find(filename) == fonts.end()) fonts[label] = app.LoadFont(filename.c_str(), ResourceDirectory::RD_BUILTIN_FONTS);
}

void UIUtils::unload()
{
	for (auto texe : textures) {
		removeResource(texe.second);
	}

	for (auto img : images) {
		conf_delete(img.second);
	}
}

void UIUtils::setStyleColor(ImGuiCol_ component, float4 color)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[component] = color;
}

void UIUtils::drawImages(Cmd* cmd, UIApp app)
{
	for (auto gui : guis) {
		app.Gui(gui.second);
	}
}

void UIUtils::drawText(Cmd* cmd, UIApp app)
{
	for (auto text : texts) {
		app.DrawText(cmd, text.second.position, text.second.text.c_str(), &text.second.desc);
	}
}

static void CloneCallbacks(IWidget* pSrc, IWidget* pDst)
{
	// Clone the callbacks
	pDst->pOnActive = pSrc->pOnActive;
	pDst->pOnHover = pSrc->pOnHover;
	pDst->pOnFocus = pSrc->pOnFocus;
	pDst->pOnEdited = pSrc->pOnEdited;
	pDst->pOnDeactivated = pSrc->pOnDeactivated;
	pDst->pOnDeactivatedAfterEdit = pSrc->pOnDeactivatedAfterEdit;
}

IWidget* TextureButtonWidget::Clone() const
{
	TextureButtonWidget* pWidget = conf_placement_new<TextureButtonWidget>(conf_calloc(1, sizeof(*pWidget)), this->mLabel);
	pWidget->SetTexture(this->mTexture, mTextureDisplaySize);

	// Clone the callbacks
	CloneCallbacks((IWidget*)this, pWidget);

	return pWidget;
}

void TextureButtonWidget::Draw()
{
	if (ImGui::ImageButton(mTexture, mTextureDisplaySize, float2(0, 0), float2(1, 1), 0)) {
		if (pOnActive) pOnActive();
	}
	ImGui::SameLine();

	//ProcessCallbacks();
}