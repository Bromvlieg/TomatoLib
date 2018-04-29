#pragma once
#ifndef __UITEXTENTRY_H__
#define __UITEXTENTRY_H__

#include "UIBase.h"

namespace TomatoLib {
	class UITextEntry : public UIBase {
	public:
		UITextEntry(UIBase* parent);

		TomatoLib::Font* Font = nullptr;
		float WideScroll = 0;
		int Caret = 0;
		float CaretX = 0;
		int MaxInput = 0;
		char MaskChar = 0;
		bool OnlyNumbers = false;
		bool OnlyLetters = false;
		bool MultiLine = false;
		bool m_blink = false;

		std::string Text;
		int prevtime = 0;

		Color BackgroundColor;
		Color BorderColor;
		Color CaretColor;
		Color TextColor;

		void Recalc();

		std::function<void()> OnEnter;
		std::function<void()> OnTextChange;

		void Clear();

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		void SetText(std::string text);
		std::string GetText();
	};
}

#endif
