#pragma once
#ifndef __UITEXTENTRY_H__
#define __UITEXTENTRY_H__

#include "UIBase.h"

namespace TomatoLib {
	class UITextEntry : public UIBase {
	public:
		UITextEntry(UIBase* parent);

		TomatoLib::Font* Font;
		float WideScroll;
		int Caret;
		float CaretX;
		int MaxInput;
		char MaskChar;
		bool OnlyNumbers;
		bool OnlyLetters;
		bool MultiLine;
		bool m_blink;

		std::string Text;
		int prevtime;

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