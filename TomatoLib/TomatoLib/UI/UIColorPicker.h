#pragma once
#ifndef UIColorPicker
#define __UICOLORPICKER_H__

#include "UIBase.h"

namespace TomatoLib {
	class UIColorPicker : public UIBase {
		bool pm_bIsBragging;
		bool pm_bIsBraggingAlpha;
		Vector2 pm_vPointR;
		Vector2 pm_vPointG;
		Vector2 pm_vPointB;
		Vector2 pm_vCenter;
		float pm_fHue;
		Vector2 pm_vsat;

	public:
		UIColorPicker(UIBase* parent);

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		int CirclePoints;
		bool CanChangeAlpha;
		Color BorderColor;

		Color MinValue;
		Color MaxValue;
		Color Value;

		int Padding;
		int BorderSize;
		int DotSize;

		std::function<void()> OnValueChanged;
	};
}


#endif