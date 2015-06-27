#pragma once
#ifndef __UIFRAME_H__
#define __UIFRAME_H__

#include "UIBase.h"
#include "UIButton.h"
#include "UITextEntry.h"
#include "UIScrollPanel.h"

namespace TomatoLib {
	class UIFrame : public UIBase {
		std::string Text;
	public:
		UIFrame(UIBase* parent);

		UIButton* CloseButton;

		virtual void Draw(Render& p) override;
		virtual void Update() override;

		virtual void InvalidateLayout() override;

		void SetText(const std::string& text);
		std::string GetText();
	};
}


#endif