#pragma once
#ifndef UISlider
#define __UISLIDER_H__

#include "UIBase.h"

namespace TomatoLib {
	class UISlider : public UIBase {
		int GrabPos;
		int BasePos;

	public:
		UISlider(UIBase* parent);

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		Color BorderColor;
		Color BackColor;
		Color SliderColor;
		Color SliderBorderColor;
		Color LineColor;

		int Value;
		int MaxValue;
		int MinValue;

		int BorderSize;
		int Padding;
		int LineSize;
		int SliderSize;
		int SliderBorderSize;

		std::function<void()> OnValueChanged;
	};
}


#endif