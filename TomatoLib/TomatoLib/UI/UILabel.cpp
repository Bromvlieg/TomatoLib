#include "UILabel.h"
#include "UIManager.h"
#include "../Math/Vector2.h"

namespace TomatoLib {
	UILabel::UILabel(UIBase* parent) : UIBase(parent) {
		this->Text = "Label";
		this->TextColor = Color::White;
		this->Align = TextAlign::MidCenter;
		this->Underline = false;
		this->Font = Render::DefaultFont;
		this->AlwaysRedraw = false;
	}

	void UILabel::SetText(const std::string& str) {
		if (this->Text == str) return;

		this->Text = str;
		this->MarkForFullRedraw();
	}

	std::string UILabel::GetText() {
		return this->Text;
	}

	void UILabel::Draw(Render& p) {
		Vector2 size = p.GetTextSize(this->Font, this->Text);

		int x = 0;
		int y = 0;
		if (this->Align == TextAlign::MidLeft || this->Align == TextAlign::MidCenter || this->Align == TextAlign::MidRight) y = this->H / 2 - (int)(this->Font->FontHandle->height / 2);
		if (this->Align == TextAlign::BotLeft || this->Align == TextAlign::BotCenter || this->Align == TextAlign::BotRight) y = this->H - (int)this->Font->FontHandle->height;

		if (this->Align == TextAlign::TopCenter || this->Align == TextAlign::MidCenter || this->Align == TextAlign::BotCenter) x = this->W / 2 - (int)size.X / 2;
		if (this->Align == TextAlign::TopRight || this->Align == TextAlign::MidRight || this->Align == TextAlign::BotRight) x = this->W - (int)size.X;

		p.Text(this->Font, this->Text, x, y, this->TextColor);

		if (this->Underline) {
			if (this->Text.find('\n') != std::string::npos) {
				int lastindex = 0;
				for (unsigned int i = 0; i < this->Text.size(); i++) {
					if (this->Text[i] == '\n') {
						size = p.GetTextSize(this->Font, this->Text.substr(lastindex, i - lastindex));
						p.Box(x, y + (int)size.Y - 1, (int)size.X, 1, this->TextColor);
						lastindex = i + 1;

						y += (int)size.Y;
					}
				}

				if (lastindex < (int)this->Text.size()) {
					size = p.GetTextSize(this->Font, this->Text.substr(lastindex, (int)this->Text.size() - lastindex));
					p.Box(x, y + (int)size.Y - 1, (int)size.X, 1, this->TextColor);
				}
			} else {
				p.Box(x, y + (int)size.Y - 1, (int)size.X, 1, this->TextColor);
			}
		}
	}

	void UILabel::ResizeToContents() {
		Vector2 size = this->UIMan->Drawer.GetTextSize(this->Font, this->Text);
		this->SetSize((int)size.X, (int)size.Y);
	}

	void UILabel::Update() {

	}

	void UILabel::InvalidateLayout() {

	}
}
