#include "UIUtils.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/imgui/imgui_internal.h"
#include "Application.h"

std::map<std::string, Texture*> UIUtils::textures = std::map<std::string, Texture*>();
std::map<std::string, ImFont*> UIUtils::fonts = std::map<std::string, ImFont*>();
std::map<std::string, UIUtils::WindowDrawData> UIUtils::windows = std::map<std::string, UIUtils::WindowDrawData>();
std::map<std::string, TextureButtonWidget*> UIUtils::images = std::map<std::string, TextureButtonWidget*>();
std::map<std::string, TextWidget*> UIUtils::texts = std::map<std::string, TextWidget*>();

void UIUtils::createGuiComponent(std::string label, GuiDesc desc, int priority)
{
	windows[label] = {};
	windows[label].gui = Application::gAppUI.AddGuiComponent(label.c_str(), &desc);
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_TITLE_BAR;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_ALWAYS_USE_WINDOW_PADDING;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_RESIZE;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_COLLAPSE;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_SCROLLBAR;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_MOVE;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_BRING_TO_FRONT_ON_FOCUS;
	windows[label].gui->mFlags |= GUI_COMPONENT_FLAGS_NO_NAV_FOCUS;
	windows[label].priority = priority;
}

void UIUtils::createImage(std::string label, std::string filename, float x, float y, float2 scale, int priority)
{
	GuiDesc desc = {};
	desc.mStartPosition = vec2(x, y);
	UIUtils::createGuiComponent(label, desc, priority);
	images[label] = conf_new(TextureButtonWidget, label.c_str());

	changeImage(label, filename, scale);
	UIUtils::windows[label].gui->AddWidget(images[label]);
}

void UIUtils::addCallbackToImage(std::string label, WidgetCallback cb)
{
	images[label]->pOnActive = cb;
}

void UIUtils::changeImage(std::string label, std::string filename, float2 scale)
{
	if (textures.find(filename) == textures.end()) UIUtils::loadTexture(filename);
	float width = UIUtils::textures[filename]->mWidth * scale.x;
	float height = UIUtils::textures[filename]->mHeight * scale.y;
	images[label]->SetTexture(UIUtils::textures[filename], float2(width, height));
}

void UIUtils::removeImage(std::string label)
{
	// There is probably a better way to do this. Might change
	windows.erase(label);
}

void UIUtils::createText(std::string label, std::string text, float x, float y, std::string font, uint32_t color, int priority)
{
	GuiDesc desc = {};
	desc.mStartPosition = vec2(x, y);
	UIUtils::createGuiComponent(label, desc, priority);
	texts[label] = conf_new(TextWidget, label.c_str());

	editText(label, text, font, color);
	UIUtils::windows[label].gui->AddWidget(texts[label]);
}

void UIUtils::editText(std::string label, std::string text, std::string font, uint32_t color)
{
	texts[label]->text = text;
	texts[label]->font = font == "" ? texts[label]->font : fonts[font];
	texts[label]->color = color == 0xabcdef00 ? texts[label]->color : color;
}

void UIUtils::removeText(std::string label)
{
	windows.erase(label);
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

void UIUtils::loadFont(std::string label, std::string filename, float size)
{
	ImGuiIO& io = ImGui::GetIO();
	auto path = fsCopyPathInResourceDirectory(ResourceDirectory::RD_BUILTIN_FONTS, filename.c_str());
	std::string pathstr = fsGetPathAsNativeString(path);
	printf("%s\n", pathstr.c_str());
	fsFreePath(path);
	//if (fonts.find(filename) == fonts.end()) fonts[label] = Application::gAppUI.LoadFont(filename.c_str(), ResourceDirectory::RD_BUILTIN_FONTS);
	if (fonts.find(label) == fonts.end()) fonts[label] = io.Fonts->AddFontFromFileTTF(pathstr.c_str(), size);
}

void UIUtils::unload()
{
	for (auto texe : textures) {
		removeResource(texe.second);
	}

	for (auto img : images) {
		conf_delete(img.second);
	}

	for (auto t : texts) {
		conf_delete(t.second);
	}
}

void UIUtils::setStyleColor(ImGuiCol_ component, float4 color)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[component] = color;
}

void UIUtils::drawImages(Cmd* cmd)
{
	auto ctx = ImGui::GetCurrentContext();
	std::stable_sort(ctx->Windows.begin(), ctx->Windows.end(),
		[](const ImGuiWindow* a, const ImGuiWindow* b) {
			//printf("comparing %s and %s\n", a->Name, b->Name);
			if (UIUtils::windows.find(a->Name) == UIUtils::windows.end() || UIUtils::windows.find(b->Name) == UIUtils::windows.end()) return false;
			//printf("priorities %d and %d\n", UIUtils::windows[a->Name].priority, UIUtils::windows[b->Name].priority);
			return UIUtils::windows[a->Name].priority < UIUtils::windows[b->Name].priority;
		}
	);

	for (auto w : ctx->Windows) {
		//printf("%s ", w->Name);
	}

	for (auto w : windows) {
		//printf("->%s ", w.first.c_str());
		Application::gAppUI.Gui(w.second.gui);
	}
	//printf("\n");
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

IWidget* TextWidget::Clone() const
{
	TextWidget* pWidget = conf_placement_new<TextWidget>(conf_calloc(1, sizeof(*pWidget)), this->mLabel);
	pWidget->SetText(this->text, this->font, this->color);

	// Clone the callbacks
	CloneCallbacks((IWidget*)this, pWidget);

	return pWidget;
}

void TextWidget::Draw()
{
	ImGui::PushFont(this->font);
	ImGui::PushStyleColor(ImGuiCol_Text, this->color);
	ImGui::Text(this->text.c_str());
	ImGui::PopStyleColor();
	ImGui::PopFont();
	
	ImGui::SameLine();
}
