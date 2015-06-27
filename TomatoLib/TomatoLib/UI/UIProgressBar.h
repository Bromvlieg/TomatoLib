#pragma once
#ifndef __UIPROGRESSBAR_H__
#define __UIPROGRESSBAR_H__

#include "UIBase.h"

namespace TomatoLib {
	class UIProgressBar : public UIBase {
		float Progress;

	public:
		UIProgressBar(UIBase* parent);

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		int Border;
		Color BorderColor;
		Color Foreground;
		Color Background;

		void SetProgress(float progress);
		float GetProgress();
	};
}


#endif