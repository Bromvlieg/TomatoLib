#pragma once
#ifndef __UIADVANCEDTEXTENTRY_H__
#define __UIADVANCEDTEXTENTRY_H__

#include "UIBase.h"
#include "../Utilities/Dictonary.h"
#include "../Utilities/List.h"
#include "../Graphics/Font.h"
#include <glfw/glfw3.h>

namespace TomatoLib {
	class UIScrollPanel;
	class UIPanel;

	class UIAdvancedTextEntryLinePart {
	public:
		float Width;
		std::string Text;
		std::string Popup;
		Color ForeColor;
		Color BackColor;
		int CursorIcon;
		std::function<void()> OnClick;

		UIAdvancedTextEntryLinePart();
		UIAdvancedTextEntryLinePart(std::string text, std::string popup, Color textcolor, Color backcolor, int cursoricon = GLFW_ARROW_CURSOR, std::function<void()> callback = []() {});
	};

	class UIAdvancedTextEntryLine {
	public:
		List<UIAdvancedTextEntryLinePart*> Parts;
		UIAdvancedTextEntryLine* CurrentLine;
		Dictonary<std::string, int> Settings;
		Color BackgroundColor;
		int Number;

		~UIAdvancedTextEntryLine();
		bool HasSameSettings(UIAdvancedTextEntryLine* other);
		void AddPart(std::string text, std::string popup, Color textcolor, Color backcolor, int cursoricon = GLFW_ARROW_CURSOR, std::function<void()> callback = []() {});
	};

	class UIAdvancedTextEntry : public UIBase {
	public:
		UIAdvancedTextEntry(UIBase* parent);
		~UIAdvancedTextEntry();

		std::function<void(UIAdvancedTextEntryLine* line, std::string text, Dictonary<std::string, int>& settings)> OnParseLine;

		Color LineNumbersColor;
		Color LineNumbersBorderColor;
		Color LineNumbersBackColor;
		Color BorderColor;
		Color BackColor;
		Color TextColor;
		Font* Font;
		int Padding;
		bool LineNumbers;
		int LineNumbersOffset;
		bool WordWrap;
		float HoverTimeBeforePopup;
		List<UIAdvancedTextEntryLine*> Lines;

		float HoverStart;
		int CurrentHoverLine;
		int CurrentHoverPart;
		UIScrollPanel* ScrollPanel;
		UIPanel* MainPanel;
		UIPanel* HoverPanel;

		void AddLine(UIAdvancedTextEntryLine* line);
		void AddLine(std::string line);
		void AddLine(std::string text, Dictonary<std::string, int>& settings);

		void SetText(std::string text);
		void Clear();
		void ResizeToContents();

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;
	};
}


#endif