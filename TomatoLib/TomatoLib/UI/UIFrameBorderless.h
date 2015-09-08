#pragma once
#ifndef __UIFRAMEBORDERLESS_H__
#define __UIFRAMEBORDERLESS_H__

#include "UIBase.h"
#include "UIButton.h"
#include "UITextEntry.h"
#include "UIScrollPanel.h"

namespace TomatoLib {
	class UIFrameBorderless : public UIBase {
		int ResizeBordersize;
		bool MovingEnabled;
	public:
		Vector2 OldSize;
		Vector2 OldPos;
		int _GrabX;
		int _GrabY;
		int _OldW;
		int _OldH;
		int _OldX;
		int _OldY;
		int _ResizeMode;

		UIFrameBorderless(UIBase* parent);

		UIButton* CloseButton;
		UIButton* MaxButton;
		UIButton* MinButton;

		Color BorderColor;
		Color BackColor;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;
		void EnableMoving();
		void EnableControls();
		void SetResizeBorderSize(int size);

		inline int UIFrameBorderless::GetResizeBorderSize() {
			return this->ResizeBordersize;
		}
	};
}


#endif