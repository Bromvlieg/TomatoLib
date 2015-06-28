#include "UIButton.h"
#include <GLFW/glfw3.h>
#include "UIManager.h"

namespace TomatoLib {
	UIButton::UIButton(UIBase* parent) : UIBase(parent) {
		this->BorderSize = 1;
		this->TextColor = Color::White;
		this->BorderColor = Color::Black;
		this->BackColor = Color(80, 80, 80, 255);
		this->DisabledColor = Color(40, 40, 40, 255);
		this->HoverColor = Color(120, 120, 120, 255);

		this->SetText("Button");
		this->CanAcceptInput = true;
		this->AlwaysRedraw = false;
		this->WasHovering = false;

		std::function<void(int, int)> oldinput = this->OnInput;
		this->OnInput = [this, oldinput](int key, int mods) {
			if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
				this->OnClick(this->W / 2, this->H / 2, GLFW_MOUSE_BUTTON_LEFT);
				return;
			}

			oldinput(key, mods);
		};
	}

	void UIButton::SetText(const std::string& str) {
		if (this->Text == str) return;

		this->Text = str;
		this->MarkForFullRedraw();
	}

	std::string UIButton::GetText() {
		return this->Text;
	}

	void UIButton::ResizeToContents() {
		Vector2 size = this->UIMan->Drawer.GetTextSize(this->Text);
		this->SetSize((int)size.X + this->BorderSize * 2, (int)size.Y + this->BorderSize * 2);
	}

	void UIButton::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(this->BorderSize, this->BorderSize, this->W - this->BorderSize * 2, this->H - this->BorderSize * 2, this->CanClick ? this->IsHovering() ? this->HoverColor : this->BackColor : this->DisabledColor);

		std::string str(this->GetText());
		Vector2 size = p.GetTextSize(str);
		p.Text(str, this->W / 2 + (int)size.X / -2, this->H / 2 + (int)size.Y / -2, this->CanClick ? this->TextColor : this->DisabledTextColor);
	}

	void UIButton::Update() {
		bool curhover = this->IsHovering();
		if (curhover != this->WasHovering) {
			this->WasHovering = curhover;
			this->MarkForFullRedraw();
		}
	}

	void UIButton::InvalidateLayout() {

	}
}