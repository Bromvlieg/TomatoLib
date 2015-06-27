#include "UICheckBox.h"

namespace TomatoLib {
	UICheckBox::UICheckBox(UIBase* parent) : UILabel(parent) {
		this->IsChecked = false;
		this->Align = TextAlign::MidRight;
		this->BoxOnleftSide = true;

		this->LineColor = Color::White;
		this->LineSize = 2;
		this->Padding = 2;

		this->OnClick = [this](int x, int y, int mouse) {
			this->IsChecked = !this->IsChecked;
		};
	}

	void UICheckBox::Draw(Render& p) {
		int x = 0;
		int y = -2;

		Vector2 size = p.GetTextSize(this->GetText());
		if (this->Align == TextAlign::MidLeft || this->Align == TextAlign::MidCenter || this->Align == TextAlign::MidRight) y = this->H / 2 - (int)size.Y / 2;
		if (this->Align == TextAlign::BotLeft || this->Align == TextAlign::BotCenter || this->Align == TextAlign::BotRight) y = this->H - (int)size.Y / 2 + 2;

		if (this->Align == TextAlign::TopCenter || this->Align == TextAlign::MidCenter || this->Align == TextAlign::BotCenter) x = this->W / 2 - (int)size.X / 2;
		if (this->Align == TextAlign::TopRight || this->Align == TextAlign::MidRight || this->Align == TextAlign::BotRight) x = this->W - (int)size.X;


		if (this->BoxOnleftSide) {
			p.Box(0, 0, this->H, this->H, Color::Black);
			p.Box(1, 1, this->H - 2, this->H - 2, Color(80, 80, 80, 255));

			if (this->IsChecked) {
				p.Line(this->Padding, this->Padding, this->H - this->Padding, this->H - this->Padding, this->LineSize, this->LineColor);
				p.Line(this->H - this->Padding, this->Padding, this->Padding, this->H - this->Padding, this->LineSize, this->LineColor);
			}

			if (x == 0) x += this->H + (int)this->Padding;
		} else {
			int addx = (int)size.X;

			p.Box(addx, 0, this->H, this->H, Color::Black);
			p.Box(addx + 1, 1, this->H - 2, this->H - 2, Color(80, 80, 80, 255));

			if (this->IsChecked) {
				p.Line(addx + this->Padding, this->Padding, addx + this->H - this->Padding, this->H - this->Padding, this->LineSize, this->LineColor);
				p.Line(addx + this->H - this->Padding, this->Padding, addx + this->Padding, this->H - this->Padding, this->LineSize, this->LineColor);
			}
		}

		std::string text = this->GetText();
		p.Text(text, x, y, this->TextColor);
		if (this->Underline) {
			if (text.find('\n')) {
				int lastindex = 0;
				for (unsigned int i = 0; i < text.size(); i++) {
					if (text[i] == '\n') {
						size = p.GetTextSize(text.substr(lastindex, i - lastindex));
						p.Box(x, y, (int)size.X, 1, this->TextColor);
						lastindex = i + 1;
					}
				}

				if (lastindex < (int)text.size()) {
					size = p.GetTextSize(text.substr(lastindex, (int)text.size() - lastindex));
					p.Box(x, y + (int)size.Y, (int)size.X, 1, this->TextColor);
				}
			} else {
				p.Box(x, y, (int)size.X, 1, Color::Black);
			}
		}
	}

	void UICheckBox::Update() {

	}

	void UICheckBox::InvalidateLayout() {

	}

	void UICheckBox::SetChecked(bool checked) {
		this->IsChecked = checked;
	}

	bool UICheckBox::GetChecked() {
		return this->IsChecked;
	}
}
