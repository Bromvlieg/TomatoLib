#pragma once
#ifndef __UIDD_H__
#define __UIDD_H__

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
	class UIDropDown : public UILabel {
		int PrevHoverIndex;

	public:
		Dictonary<std::string, void*> Options;
		int SelectedIndex;

		UIDropDown(UIBase* parent);

		Color BorderColor;
		Color BackColor;
		Color HoverColor;
		int Spacing;

		UIButton* ExtendButton;
		UIPanel* MainPanel;
		UIPanel* DrawPanel;
		UIScrollPanel* ScrollPanel;

		int MaxShownOptions;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		void* GetSelectedValue();
		void SetSelectedIndex(int i);
		void AddOption(std::string name, void* value);
		void ClearOptions();

		std::function<void()> OnSelectedIndexChanged;
	};
}


#endif