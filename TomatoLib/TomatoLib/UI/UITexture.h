#pragma once
#ifndef __UITPANEL_H__
#define __UITPANEL_H__

#include "UIBase.h"
#include "../Graphics/Texture.h"

namespace TomatoLib {
	class UITexture : public UIBase {
		GLuint TextureHandle;
		Vector2 TextureStart;
		Vector2 TextureEnd;

	public:
		UITexture(UIBase* parent);

		Color BackgroundColor;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;
		void SetTexture(GLuint handle, const Vector2& glcoordstart = Vector2::Zero, const Vector2& glcoordend = Vector2::One);
		void SetTexture(const Texture& tex, const Vector2& glcoordstart = Vector2::Zero, const Vector2& glcoordend = Vector2::One);
		void ClearTexture();
	};
}


#endif