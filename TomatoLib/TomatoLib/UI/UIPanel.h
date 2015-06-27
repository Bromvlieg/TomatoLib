#pragma once
#ifndef __UIPANEL_H__
#define __UIPANEL_H__

#include "UIBase.h"

namespace TomatoLib {
	class UIPanel : public UIBase {
	public:
		UIPanel(UIBase* parent);

		Color BorderColor;
		Color BackgroundColor;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;
	};
}


#endif