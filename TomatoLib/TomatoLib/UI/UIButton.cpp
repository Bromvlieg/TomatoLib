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

	void UIButton::SetFont(TomatoLib::Font& font) {
		if (this->Font == &font) return;

		this->Font = &font;
		this->MarkForFullRedraw();
	}

	TomatoLib::Font& UIButton::GetFont() {
		return *this->Font;
	}

	void UIButton::ResizeToContents() {
		TomatoLib::Font* fontused = this->Font == nullptr ? Render::DefaultFont : this->Font;

		Vector2 size = this->UIMan->Drawer.GetTextSize(fontused, this->Text);
		this->SetSize((int)size.X + this->BorderSize * 2, (int)size.Y + this->BorderSize * 2);
	}

	void UIButton::Draw(Render& p) {
		TomatoLib::Font* fontused = this->Font == nullptr ? Render::DefaultFont : this->Font;

		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(this->BorderSize, this->BorderSize, this->W - this->BorderSize * 2, this->H - this->BorderSize * 2, this->CanClick ? this->IsHovering() ? this->HoverColor : this->BackColor : this->DisabledColor);

		std::string strrem(this->GetText());
		Vector2 size = p.GetTextSize(fontused, strrem);

		// the easy way out
		if (size.X < this->W - this->BorderSize) {
			p.Text(fontused, strrem, this->W / 2 - size.X / 2, this->H / 2 - size.Y / 2, this->CanClick ? this->TextColor : this->DisabledTextColor);
			return;
		}
		
		List<std::string> parts;
		List<Vector2> sizes;
		while (size.X >= this->W - this->BorderSize) {
			int found = -1;

			for (size_t i = strrem.size() - 1; i > 0; i--) {
				if (strrem[i] == ' ') {
					std::string part = strrem.substr(0, i);
					Vector2 partsize = p.GetTextSize(fontused, part);

					if (partsize.X < this->W - this->BorderSize) {
						found = i;
						parts.Add(part);
						sizes.Add(partsize);
						break;
					}
				}
			}

			// just cut it off when there's no space found.... gotta do something right?
			bool iscut = false;
			if (found == -1) {
				for (size_t i = strrem.size() - 1; i > 0; i--) {
					std::string part = strrem.substr(0, i);
					Vector2 partsize = p.GetTextSize(fontused, part);

					if (partsize.X < this->W - this->BorderSize) {
						found = i;
						parts.Add(part);
						sizes.Add(partsize);
						break;
					}
				}
			}

			// FINE, HAVE IT YOUR WAY.
			if (found == -1) {
				parts.Add(strrem);
				sizes.Add(size);
				break;
			}

			strrem = strrem.substr(found + 1);
			size = p.GetTextSize(fontused, strrem);
		}

		// add final part
		if (strrem.size() > 0) {
			parts.Add(strrem);
			sizes.Add(size);
		}

		float startx = this->W / 2.0f;
		float starty = this->H / 2.0f;
		starty -= parts.Count * fontused->FontHandle->height / 2;

		for (int i = 0; i < parts.Count; i++) {
			const std::string& curpart = parts[i];
			const Vector2& cursize = sizes[i];

			p.Text(fontused, curpart, startx - cursize.X / 2.0f, starty, this->CanClick ? this->TextColor : this->DisabledTextColor);
			starty += fontused->FontHandle->height + fontused->FontHandle->linegap;
		}
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