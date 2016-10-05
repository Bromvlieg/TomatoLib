#pragma once
#ifndef __UISCROLLPANEL_H__
#define __UISCROLLPANEL_H__

#include "UIBase.h"
#include "UIButton.h"
#include "UIPanel.h"

namespace TomatoLib {
	class UIScrollPanel : public UIBase {
	public:
		UIScrollPanel(UIBase* parent, bool enablehorizontal);

		UIPanel* Back;
		UIButton* UpX, *UpY;
		UIButton* DownX, *DownY;
		UIButton* GrabX, *GrabY;

		Color BackgroundColor;
		Color BorderColor;

		bool AlwaysShowBar;

		bool _IsGrabbingX, _IsGrabbingY;
		int _LastX, _LastY;
		int _GrabX, _GrabY;
		int _ScrollX, _ScrollY;
		float ScrollAmplifier;

		void Scroll(int deltaY);
		void Scroll(int deltaX, int deltaY);

		void DeltaX(int deltaX);
		void DeltaY(int deltaY);

		void AddItem(UIBase* item);
		void RemoveItem(UIBase* item);

		void DisableVerticalScrollbar();
		void DisableHorizontalScrollbar();

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;
	};
}


#endif