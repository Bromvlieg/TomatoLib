#include "UIProgressBar.h"

namespace TomatoLib {
	UIProgressBar::UIProgressBar(UIBase* parent) : UIBase(parent) {
		this->Border = 1;
		this->Progress = 0;
		this->BorderColor = Color(0, 0, 0);
		this->Foreground = Color(0, 200, 0);
		this->Background = Color(0, 100, 0);
		this->AlwaysRedraw = false;
	}

	void UIProgressBar::Draw(Render& p) {
		if (this->Border > 0) {
			p.Box(0, 0, this->W, this->H, this->BorderColor);
		}
		p.Box(this->Border, this->Border, this->W - this->Border * 2, this->H - this->Border * 2, this->Background);
		p.Box((float)this->Border, (float)this->Border, (this->W - this->Border * 2) * this->Progress, this->H - this->Border * 2.0f, this->Foreground);
	}

	void UIProgressBar::SetProgress(float progress) {
		this->Progress = progress;
	}

	float UIProgressBar::GetProgress() {
		return this->Progress;
	}

	void UIProgressBar::Update() {

	}

	void UIProgressBar::InvalidateLayout() {

	}

}
