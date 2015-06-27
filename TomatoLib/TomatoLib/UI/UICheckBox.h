#pragma once
#ifndef __UICHKBOX_H__
#define __UICHKBOX_H__

#include "UILabel.h"

namespace TomatoLib {
	class UICheckBox : public UILabel {
		bool IsChecked;

	public:
		UICheckBox(UIBase* parent);

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		bool BoxOnleftSide;

		float Padding;
		float LineSize;
		Color LineColor;

		void SetChecked(bool checked);
		bool GetChecked();
	};
}


#endif