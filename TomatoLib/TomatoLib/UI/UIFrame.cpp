#include "UIFrame.h"

namespace TomatoLib {
	UIFrame::UIFrame(UIBase* parent) : UIBase(parent) {
		this->Text = "New Window";
		this->AlwaysRedraw = false;

		this->CloseButton = new UIButton(this);
		this->CloseButton->SetSize(20, 20);
		this->CloseButton->SetText("X");

		this->CloseButton->OnRelease = [this](int x, int y, int button) {
			this->Kill();
		};

		this->CloseButton->OnDraw = [this](Render& p) {
			p.Box(0, 0, this->CloseButton->W, this->CloseButton->H, Color::Black);
			p.Box(1, 1, this->CloseButton->W - 2, this->CloseButton->H - 1, Color::Red);
			Vector2 size = p.GetTextSize(this->CloseButton->GetText());
			p.Text(this->CloseButton->GetText(), this->CloseButton->W / 2 - (int)size.X / 2, this->CloseButton->H / 2 - (int)size.Y / 2 - 2, Color::White);
		};
	}

	void UIFrame::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, Color(0, 0, 0, 255));
		p.Box(1, 1, this->W - 2, 19, Color(80, 80, 80, 255));
		p.Box(1, 21, this->W - 2, this->H - 23, Color(60, 60, 60, 255));
		p.Text(this->Text, 4, 0, Color(255, 255, 255, 255));
	}

	void UIFrame::Update() {

	}

	void UIFrame::SetText(const std::string& text) {
		this->Text = text;;
	}

	std::string UIFrame::GetText() {
		return this->Text;
	}

	void UIFrame::InvalidateLayout() {
		this->CloseButton->SetPos(this->W - 20, 0);
	}

}
