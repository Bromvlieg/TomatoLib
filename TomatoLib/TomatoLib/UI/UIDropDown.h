#pragma once
#ifndef __UIDD_H__
#define __UIDD_H__

#include "UILabel.h"
#include "UIButton.h"
#include "UIPanel.h"
#include "UIScrollPanel.h"
#include "UITextEntry.h"
#include "../Utilities/Dictonary.h"
#include "../Utilities/List.h"
#include "../Defines.h"
#include <string>
#include <vector>
#include <mutex>

#ifdef TL_ENABLE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace TomatoLib {
	template<typename ValueType>
	class UIDropDown : public UILabel {
		int PrevHoverIndex;

	public:
		Dictonary<std::string, ValueType> Options;
		int SelectedIndex;

		UIDropDown(UIBase* parent) : UILabel(parent) {
			this->CanAcceptInput = true;
			this->BorderColor = Color(0, 0, 0, 255);
			this->BackColor = Color(60, 60, 60, 255);
			this->HoverColor = Color::Green;
			this->SetText("");

			this->Spacing = 2;
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
			this->PrevHoverIndex = -1;

			this->DrawPanel = new UIPanel(0);
			this->DrawPanel->OnDraw = [this](Render& p) {
				Vector2 mousepos = this->UIMan->LastMousePosition;

				float currentY = this->ScrollPanel->Back->Y * -1.0f;
				mousepos.Y -= this->MainPanel->Y;

				p.Box(0, 0, this->DrawPanel->W, this->DrawPanel->H, this->BorderColor);
				p.Box(1, 1, this->DrawPanel->W - 2, this->DrawPanel->H - 2, this->BackColor);

				float fh = this->Font->FontHandle->height;
				int linecount = this->Options.Count;
				int startline = (int)(currentY / fh);
				for (int line = startline, i = 0; line < startline + this->MaxShownOptions && line < this->Options.Count; line++, i++) {
					bool hovering = mousepos.Y >= i * fh && mousepos.Y < i * fh + fh;

					if (line != 0) p.Box(0.0f, currentY + 1.0f, (float)this->DrawPanel->W, 1.0f, this->BorderColor);
					Vector2 tsize = p.GetTextSize(this->Font, this->Options.Keys[line]);
					p.Text(this->Font, this->Options.Keys[line], 2.0f, currentY + (fh + this->Spacing) / 2 - tsize.Y / 2 + 3, hovering ? this->HoverColor : this->TextColor);

					currentY += this->Font->FontHandle->height + this->Spacing;
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
					if (mousepos.Y >= i * (fh + this->Spacing) && mousepos.Y < i * (fh + this->Spacing) + fh + this->Spacing) {
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
					if (mousepos.Y >= i * (fh + this->Spacing) && mousepos.Y < i * (fh + this->Spacing) + fh + this->Spacing) {
						this->SelectedIndex = line;
						this->SetText(this->Options.Keys[line]);
						this->MainPanel->Hide();
						this->SetFocus();

						if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
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
					this->MainPanel->ForceTop();
					this->MainPanel->Show();
				}
			};

			this->OnInput = [this](int key, int mods) {
				if (key == GLFW_KEY_UP) {
					if (this->SelectedIndex > 0) {
						this->SelectedIndex--;
						this->SetText(this->Options.Keys[this->SelectedIndex]);
						if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
					}
				} else if (key == GLFW_KEY_DOWN) {
					if (this->SelectedIndex < this->Options.Count - 1) {
						this->SelectedIndex++;
						this->SetText(this->Options.Keys[this->SelectedIndex]);
						if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
					}
				}
			};

			this->OnScroll = [this](int x, int y) {
				if (y > 0) {
					if (this->SelectedIndex > 0) {
						this->SelectedIndex--;
						this->SetText(this->Options.Keys[this->SelectedIndex]);
						if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
					}
				} else if (y < 0) {
					if (this->SelectedIndex < this->Options.Count - 1) {
						this->SelectedIndex++;
						this->SetText(this->Options.Keys[this->SelectedIndex]);
						if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
					}
				}
			};

			this->ExtendButton = new UIButton(this);
			this->ExtendButton->SetText(">");

			this->ScrollPanel->OnFocus = this->MainPanel->OnFocus;
			this->DrawPanel->OnFocus = this->MainPanel->OnFocus;

			this->ExtendButton->OnClick = this->OnClick;
		}

		Color BorderColor;
		Color BackColor;
		Color HoverColor;
		int Spacing;

		UIButton* ExtendButton;
		UIPanel* MainPanel;
		UIPanel* DrawPanel;
		UIScrollPanel* ScrollPanel;

		int MaxShownOptions;


		virtual void Kill() override {
			this->MainPanel->Kill();
			UIBase::Kill();
		}

		virtual void Draw(Render& p) override {
			p.Box(0, 0, this->W, this->H, this->BorderColor);
			p.Box(1, 1, this->W - 2, this->H - 2, this->BackColor);

			Vector2 apos = this->GetAbsoluteLocation();
			p.SetDrawingOffset((int)apos.X + 2, (int)apos.Y);
			UILabel::Draw(p);
		}

		virtual void Update() override {
			if (!this->MainPanel->ShouldRender) return;
			if (this->MainPanel->HasFocus() || this->HasFocus()) return;

			this->MainPanel->Hide();
		}

		void ClearOptions() {
			this->Options.Clear();
			int oldi = this->SelectedIndex;
			this->SelectedIndex = -1;

			this->SetText("");
			this->MainPanel->SetSize(0, 0);

			this->ScrollPanel->SetSize(this->MainPanel->W, this->MainPanel->H);
			this->DrawPanel->SetSize(this->ScrollPanel->W, this->MainPanel->H);
			this->ScrollPanel->InvalidateLayout();

			if (oldi != -1 && this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
		}

		ValueType GetSelectedValue() {
			if (this->SelectedIndex == -1) throw std::runtime_error("no item selected");
			return this->Options.Values[this->SelectedIndex];
		}

		void SetSelectedIndex(int i) {
			if (i >= this->Options.Keys.Count) return;

			this->SelectedIndex = i;
			this->SetText(this->Options.Keys[this->SelectedIndex]);
			if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
		}

		virtual void InvalidateLayout() override {
			this->ExtendButton->SetSize(this->H, this->H);
			this->ExtendButton->SetPos(this->W - this->H, 0);
		}

		void AddOption(std::string key, ValueType value) {
			this->Options[key] = value;

			Vector2 tsize = this->UIMan->Drawer.GetTextSize(*this->Font, key);

			int tw = (int)tsize.X + (this->ScrollPanel->GrabY->ShouldRender ? this->ScrollPanel->GrabY->W : 0);
			if (tw < this->W) tw = this->W;

			int itemh = (int)this->Font->FontHandle->height + this->Spacing;
			this->MainPanel->SetSize(tw, this->Options.Count > this->MaxShownOptions ? this->MaxShownOptions * itemh : this->Options.Count * itemh);

			this->DrawPanel->SetSize(this->MainPanel->W, this->Options.Count * itemh + this->Spacing);
			this->ScrollPanel->SetSize(this->MainPanel->W, this->MainPanel->H);
			this->ScrollPanel->InvalidateLayout();

			if (this->Options.Count == 1) {
				this->SetText(key);
				this->SelectedIndex = 0;
				if (this->OnSelectedIndexChanged != nullptr) this->OnSelectedIndexChanged();
			}
		}

		std::function<void()> OnSelectedIndexChanged;
	};
}


#endif