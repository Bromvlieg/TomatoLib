#include "UIPanel.h"

namespace TomatoLib {
	UIPanel::UIPanel(UIBase* parent) : UIBase(parent) {
		this->BackgroundColor = Color(100, 100, 100, 255);
		this->BorderColor = Color::Black;
		this->AlwaysRedraw = false;

		this->OnScroll = [this](int x, int y) {
			if (this->Parent != nullptr) {
				this->Parent->OnScroll(x, y);
			}
		};
	}

	void UIPanel::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(1, 1, this->W - 2, this->H - 2, this->BackgroundColor);
	}

	void UIPanel::Update() {

	}

	void UIPanel::InvalidateLayout() {

	}

}
