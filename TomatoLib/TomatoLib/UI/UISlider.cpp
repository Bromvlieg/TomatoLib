#include "UISlider.h"
#include "UIManager.h"

#include <math.h>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UISlider::UISlider(UIBase* parent) : UIBase(parent) {
		this->BorderColor = Color::Black;
		this->BackColor = Color(100, 100, 100, 255);
		this->SliderColor = Color(80, 80, 80, 255);
		this->SliderBorderColor = Color::Black;
		this->LineColor = Color::Black;

		this->BorderSize = 1;
		this->Padding = 4;
		this->LineSize = 2;
		this->SliderSize = 6;
		this->SliderBorderSize = 2;
		this->CanAcceptInput = true;

		this->MinValue = 0;
		this->MaxValue = 100;
		this->Value = 50;

		this->BasePos = -1;
		this->GrabPos = -1;

		this->AlwaysRedraw = false;

		this->OnClick = [this](int x, int y, int but) {
			this->GrabPos = x;
		};

		this->OnRelease = [this](int x, int y, int but) {
			this->GrabPos = -1;
		};


		std::function<void(int, int)> oldinput = this->OnInput;
		this->OnInput = [this, oldinput](int key, int mods) {
			if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
				this->Value += key == GLFW_KEY_LEFT ? -1 : 1;

				if (this->Value < this->MinValue) {
					this->Value = this->MinValue;
					return;
				}

				if (this->Value > this->MaxValue) {
					this->Value = this->MaxValue;
					return;
				}

				if (this->OnValueChanged != null) this->OnValueChanged();
				return;
			}

			oldinput(key, mods);
		};
	}

	void UISlider::Draw(Render& p) {
		this->BasePos = (int)p.GetDrawingOffset().X;

		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(this->BorderSize, this->BorderSize, this->W - this->BorderSize * 2, this->H - this->BorderSize * 2, this->BackColor);

		p.Box(this->BorderSize + this->Padding, this->H / 2 - this->LineSize / 2, this->W - this->BorderSize * 2 - this->Padding * 2, this->LineSize, this->LineColor);

		int sliderX = this->BorderSize + this->Padding + (int)(((double)(this->Value - this->MinValue) / (double)this->MaxValue) * (this->W - this->Padding * 2 - this->BorderSize * 2 - this->SliderSize - this->SliderBorderSize));
		int sliderY = this->H / 2 - this->SliderSize / 2;

		p.Box(sliderX - this->SliderBorderSize, sliderY - this->SliderBorderSize, this->SliderSize + this->SliderBorderSize * 2, this->SliderSize + this->SliderBorderSize * 2, this->SliderBorderColor);
		p.Box(sliderX, sliderY, this->SliderSize, this->SliderSize, this->SliderColor);
	}

	void UISlider::Update() {
		if (!this->HasFocus() || this->GrabPos == -1 || this->BasePos == -1) return;

		Vector2 mousepos = this->UIMan->LastMousePosition;
		if (mousepos.X < 0) mousepos.X = 0;

		double offset = mousepos.X - this->BasePos;
		offset -= this->BorderSize + this->Padding + this->SliderBorderSize;

		int oldval = this->Value;
		this->Value = (int)round(offset / (this->W - this->BorderSize * 2 - this->Padding * 2 - this->SliderSize) * this->MaxValue + this->MinValue);
		if (this->Value < this->MinValue) this->Value = this->MinValue;
		if (this->Value > this->MaxValue) this->Value = this->MaxValue;

		if (this->Value != oldval) {
			if (this->OnValueChanged != null) this->OnValueChanged();
			this->MarkForFullRedraw();
		}
	}

	void UISlider::InvalidateLayout() {

	}

}
