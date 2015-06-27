#include "UITexture.h"

namespace TomatoLib {
	UITexture::UITexture(UIBase* parent) : UIBase(parent) {
		this->BackgroundColor = Color::Transparent;
		this->DrawColor = Color::White;
		this->AlwaysRedraw = false;
		this->TextureHandle = 0;
	}

	void UITexture::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BackgroundColor);

		if (this->TextureHandle == 0) return;
		p.SetTexture(this->TextureHandle);
		p.Texture(0, 0, (float)this->W, (float)this->H, this->TextureStart.X, this->TextureStart.Y, this->TextureEnd.X, this->TextureEnd.Y, this->DrawColor);
	}

	void UITexture::Update() {

	}

	void UITexture::InvalidateLayout() {

	}

	void UITexture::SetTexture(const Texture& tex, const Vector2& glcoordstart, const Vector2& glcoordend) {
		this->TextureHandle = tex.GLHandle;
		this->ShouldRedraw = true;
		this->TextureStart = glcoordstart;
		this->TextureEnd = glcoordend;
	}

	void UITexture::SetTexture(GLuint handle, const Vector2& glcoordstart, const Vector2& glcoordend) {
		this->TextureHandle = handle;
		this->ShouldRedraw = true;
		this->TextureStart = glcoordstart;
		this->TextureEnd = glcoordend;
	}

	void UITexture::ClearTexture() {
		this->TextureHandle = 0;
		this->ShouldRedraw = true;
	}
}
