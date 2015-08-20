#pragma once
#ifndef __UICONSOLE_H__
#define __UICONSOLE_H__

#include "../UIBase.h"
#include "../UIButton.h"
#include "../UIPanel.h"
#include "../UIScrollPanel.h"
#include "../UITextEntry.h"
#include "../../Utilities/Dictonary.h"
#include "../../Utilities/List.h"
#include <string>
#include <vector>
#include <mutex>

namespace TomatoLib {
	class Font;

	struct UIConsoleLineData {
		std::vector<Color> Colors;
		std::vector<std::string> Strings;
	};

	class UIConsole : public UIBase {
		Dictonary<std::string, std::function<std::string(std::string)>> RegisteredFunctions;
		Dictonary<std::string, std::string> RegisteredFunctionsDesc;
		Dictonary<std::string, std::string> RegisteredVariables;
		Dictonary<std::string, std::string> RegisteredVariablesDesc;

		List<std::string> InputHistory;
		int InputHistoryIndex;
		size_t PrevCharCount;

		List<std::string> AutoComplete;
		std::mutex PrintLock;

	public:
		UIConsole(UIBase* parent);

		UIPanel* MainPanel;
		UIPanel* ConsolePanel;
		UIScrollPanel* ScrollPanel;
		UITextEntry* TextEntry;
		TomatoLib::Font* Font;
		std::vector<UIConsoleLineData> Lines;

		int MaxHeight;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		void Print(std::string line, bool enablecolors = true);
		void HandleInput(std::string line);

		void RegisterConsoleVar(std::string name, std::string defaultvalue, std::string description = "");
		void RegisterConsoleFunc(std::string name, std::function<std::string(std::string)> func, std::string description = "");
		std::string GetConsoleVar(std::string name);
	};
}


#endif