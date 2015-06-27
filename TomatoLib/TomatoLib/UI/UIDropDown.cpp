#include "UIDropDown.h"
#include "UIManager.h"

#include <math.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UIDropDown::UIDropDown(UIBase* parent) : UILabel(parent) {
		this->CanAcceptInput = true;
		this->BorderColor = Color(0, 0, 0, 255);
		this->BackColor = Color(60, 60, 60, 255);

		this->SelectedIndex = -1;
		this->MaxShownOptions = 5;
		this->Align = TextAlign::MidLeft;

		this->MainPanel = new UIPanel(null);
		this->MainPanel->SetPos(0, 0);
		this->MainPanel->TopMost = true;
		this->MainPanel->Hide();

		this->ScrollPanel = new UIScrollPanel(this->MainPanel, false);
		this->ScrollPanel->SetPos(0, 0);
		this->ScrollPanel->SetSize(this->MainPanel->W, 0);
		this->AlwaysRedraw = false;
		this->PrevHoverIndex = -1;

		this->DrawPanel = new UIPanel(0);
		this->DrawPanel->OnDraw = [this](Render& p) {
			Vector2 mousepos = this->UIMan->LastMousePosition;

			float currentY = this->ScrollPanel->Back->Y * -1.0f;
			mousepos.Y -= this->MainPanel->Y;

			p.Box(0, 0, this->DrawPanel->W, this->DrawPanel->H, Color(0, 0, 0, 150));
			p.Box(1, 1, this->DrawPanel->W - 2, this->DrawPanel->H - 2, Color(60, 60, 60, 150));

			float fh = this->Font->FontHandle->height;
			int linecount = this->Options.Count;
			int startline = (int)(currentY / fh);
			for (int line = startline, i = 0; line < startline + this->MaxShownOptions && line < this->Options.Count; line++, i++) {
				bool hovering = mousepos.Y >= i * fh && mousepos.Y < i * fh + fh;

				if (line != 0) p.Box(0.0f, currentY, (float)this->DrawPanel->W, 1.0f, Color(0, 0, 0, 150));
				Vector2 tsize = p.GetTextSize(this->Font, this->Options.Keys[line]);
				p.Text(this->Font, this->Options.Keys[line], 2.0f, currentY + fh / 2 - tsize.Y / 2, hovering ? Color::Green : Color::White);

				currentY += this->Font->FontHandle->height;
			}
		};

		this->DrawPanel->OnUpdate = [this]() {
			Vector2 mousepos = this->UIMan->LastMousePosition;

			float currentY = this->ScrollPanel->Back->Y * -1.0f;
			mousepos.Y -= this->MainPanel->Y;

			float fh = this->Font->FontHandle->height;
			int linecount = this->Options.Count;
			int startline = (int)(currentY / fh);
			for (int line = startline, i = 0; line < startline + this->MaxShownOptions && line < this->Options.Count; line++, i++) {
				if (mousepos.Y >= i * fh && mousepos.Y < i * fh + fh) {
					if (this->PrevHoverIndex != line) {
						this->PrevHoverIndex = line;
						this->MarkForFullRedraw();
					}
					return;
				}

				currentY += this->Font->FontHandle->height;
			}
		};

		this->DrawPanel->OnClick = [this](int x, int y, int mouse) {
			Vector2 mousepos = this->UIMan->LastMousePosition;

			float currentY = this->ScrollPanel->Back->Y * -1.0f;
			mousepos.Y -= this->MainPanel->Y;

			float fh = this->Font->FontHandle->height;
			int linecount = this->Options.Count;
			int startline = (int)(currentY / fh);
			for (int line = startline, i = 0; line < startline + this->MaxShownOptions && line < this->Options.Count; line++, i++) {
				if (mousepos.Y >= i * fh && mousepos.Y < i * fh + fh) {
					this->SelectedIndex = line;
					this->SetText(this->Options.Keys[line]);
					this->MainPanel->Hide();
					this->SetFocus();

					if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
					return;
				}
			}
		};

		this->ScrollPanel->AddItem(this->DrawPanel);

		this->MainPanel->OnScroll = [this](int x, int y) { this->ScrollPanel->Scroll(x, y); };

		this->OnClick = [this](int x, int y, int mouse) {
			if (this->MainPanel->ShouldRender) {
				this->MainPanel->Hide();
			} else {
				Vector2 apos = this->GetAbsoluteLocation();
				this->MainPanel->SetPos((int)apos.X, (int)apos.Y + this->H);
				this->MainPanel->OnTop();
				this->MainPanel->Show();
			}
		};

		this->OnInput = [this](int key, int mods) {
			if (key == GLFW_KEY_UP) {
				if (this->SelectedIndex > 0) {
					this->SelectedIndex--;
					this->SetText(this->Options.Keys[this->SelectedIndex]);
					if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
				}
			} else if (key == GLFW_KEY_DOWN) {
				if (this->SelectedIndex < this->Options.Count - 1) {
					this->SelectedIndex++;
					this->SetText(this->Options.Keys[this->SelectedIndex]);
					if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
				}
			}
		};

		this->OnScroll = [this](int x, int y) {
			if (y > 0) {
				if (this->SelectedIndex > 0) {
					this->SelectedIndex--;
					this->SetText(this->Options.Keys[this->SelectedIndex]);
					if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
				}
			} else if (y < 0) {
				if (this->SelectedIndex < this->Options.Count - 1) {
					this->SelectedIndex++;
					this->SetText(this->Options.Keys[this->SelectedIndex]);
					if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
				}
			}
		};

		this->ExtendButton = new UIButton(this);
		this->ExtendButton->SetText(">");

		this->ScrollPanel->OnFocus = this->MainPanel->OnFocus;
		this->DrawPanel->OnFocus = this->MainPanel->OnFocus;

		this->ExtendButton->OnClick = this->OnClick;

		this->OnKill = [this]() {
			this->MainPanel->Kill();
		};
	}

	void UIDropDown::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(1, 1, this->W - 2, this->H - 2, this->BackColor);


		Vector2 apos = this->GetAbsoluteLocation();
		p.SetDrawingOffset((int)apos.X + 2, (int)apos.Y);
		UILabel::Draw(p);
	}

	void UIDropDown::Update() {
		if (!this->MainPanel->ShouldRender) return;
		if (this->MainPanel->HasFocus()) return;

		this->MainPanel->Hide();
	}

	void UIDropDown::ClearOptions() {
		this->Options.Clear();
		int oldi = this->SelectedIndex;
		this->SelectedIndex = -1;

		this->SetText("");
		this->MainPanel->SetSize(0, 0);

		this->ScrollPanel->SetSize(this->MainPanel->W, this->MainPanel->H);
		this->DrawPanel->SetSize(this->ScrollPanel->W, this->MainPanel->H);
		this->ScrollPanel->InvalidateLayout();

		if (oldi != -1 && this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
	}

	void* UIDropDown::GetSelectedValue() {
		if (this->SelectedIndex == -1) return null;
		return this->Options.Values[this->SelectedIndex];
	}

	void UIDropDown::AddOption(std::string key, void* value) {
		this->Options[key] = value;

		Vector2 tsize = this->UIMan->Drawer.GetTextSize(*this->Font, key);

		int tw = (int)tsize.X + (this->ScrollPanel->GrabY->ShouldRender ? this->ScrollPanel->GrabY->W : 0);
		if (tw < this->W) tw = this->W;
		this->MainPanel->SetSize(tw, this->Options.Count > this->MaxShownOptions ? this->MaxShownOptions * 16 : this->Options.Count * 16);

		this->DrawPanel->SetSize(this->MainPanel->W, this->Options.Count * 16);
		this->ScrollPanel->SetSize(this->MainPanel->W, this->MainPanel->H);
		this->ScrollPanel->InvalidateLayout();

		if (this->Options.Count == 1) {
			this->SetText(key);
			this->SelectedIndex = 0;
			if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
		}
	}

	void UIDropDown::SetSelectedIndex(int i) {
		this->SelectedIndex = i;
		this->SetText(this->Options.Keys[this->SelectedIndex]);
		if (this->OnSelectedIndexChanged != null) this->OnSelectedIndexChanged();
	}

	void UIDropDown::InvalidateLayout() {
		this->ExtendButton->SetSize(this->H, this->H);
		this->ExtendButton->SetPos(this->W - this->H, 0);
	}
}
