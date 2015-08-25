#pragma once
#ifndef __UILABEL_H__
#define __UILABEL_H__

#include "UIBase.h"

namespace TomatoLib {
	class Font;

	enum class TextAlign {
		TopLeft, TopCenter, TopRight,
		MidLeft, MidCenter, MidRight,
		BotLeft, BotCenter, BotRight
	};

	class UILabel : public UIBase {
		std::string Text;

	public:
		UILabel(UIBase* parent);

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		TomatoLib::Font* Font;
		Color TextColor;
		TextAlign Align;
		bool Underline;

		void ResizeToContents();
		void SetText(const std::string& str);
		std::string GetText();
	};
}


#endif