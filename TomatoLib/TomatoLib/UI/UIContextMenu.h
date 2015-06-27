#pragma once
#ifndef __UICM_H__
#define __UICM_H__

#include "UILabel.h"
#include "UIButton.h"
#include "UIPanel.h"
#include "UIScrollPanel.h"
#include "UITextEntry.h"
#include "../Utilities/Dictonary.h"
#include "../Utilities/List.h"
#include <string>
#include <vector>
#include <mutex>

namespace TomatoLib {
	struct UIContextMenuItem {
		std::string Label;
		Color TextColor;
		std::function<void()> Callback;
	};

	class UIContextMenu : public UIBase {

	public:
		List<UIContextMenuItem> Items;
		UIContextMenu();

		Color BorderColor;
		Color BackColor;
		Color SplitLineColor;

		UIPanel* DrawPanel;
		UIScrollPanel* ScrollPanel;

		int MaxShownOptions;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		void AddItem(std::string name, std::function<void()> callback, Color textcol);
	};
}


#endif