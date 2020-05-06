#include "UIManager.h"

void UIManager::update(float deltaTime)
{
}

void UIManager::draw(Cmd* commands)
{
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
	ImGui::ImageButton(mTexture, mTextureDisplaySize, float2(0, 0), float2(1, 1), 0);
	ImGui::SameLine();

	ProcessCallbacks();
}
