#include "UIAdvancedTextEntry.h"
#include "UIScrollPanel.h"
#include "UIPanel.h"
#include "UIManager.h"
#include "../Utilities/Utilities.h"
#include "../Defines.h"
#include "../Config.h"

#ifdef TL_ENABLE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace TomatoLib {
	UIAdvancedTextEntryLinePart::UIAdvancedTextEntryLinePart() {};
	UIAdvancedTextEntryLinePart::UIAdvancedTextEntryLinePart(std::string text, std::string popup, Color textcolor, Color backcolor, int cursoricon, std::function<void()> callback) :Text(text), Popup(popup), ForeColor(textcolor), BackColor(backcolor), OnClick(callback), CursorIcon(cursoricon) {};

	void UIAdvancedTextEntryLine::AddPart(std::string text, std::string popup, Color textcolor, Color backcolor, int cursoricon, std::function<void()> callback) {
		this->Parts.Add(new UIAdvancedTextEntryLinePart(text, popup, textcolor, backcolor, cursoricon, callback));
	};

	UIAdvancedTextEntryLine::~UIAdvancedTextEntryLine() {
		for (int i = this->Parts.Count - 1; i >= 0; i--) {
			delete this->Parts[i];
		}
	};

	UIAdvancedTextEntry::~UIAdvancedTextEntry() {
		for (int i = this->Lines.Count - 1; i >= 0; i--) {
			delete this->Lines[i];
		}

		this->HoverPanel->Kill();
	}

	UIAdvancedTextEntry::UIAdvancedTextEntry(UIBase* parent) : UIBase(parent) {
		this->TextColor = Color::White;
		this->BackColor = Color::White;
		this->BorderColor = Color::Black;
		this->Padding = 2;
		this->HoverTimeBeforePopup = 500;
		this->Font = this->UIMan->Drawer.DefaultFont;
		this->WordWrap = false;
		this->CanAcceptInput = true;
		this->AlwaysRedraw = false;

		this->CurrentHoverPart = null;
		this->LineNumbersOffset = 0;

		this->LineNumbers = false;

		this->HoverPanel = new UIPanel(null);
		this->HoverPanel->TopMost = true;
		this->HoverPanel->OnDraw = [this](Render& p) {
			p.Box(0, 0, this->HoverPanel->W, this->HoverPanel->H, this->HoverPanel->BorderColor);
			p.Box(1, 1, this->HoverPanel->W - 2, this->HoverPanel->H - 2, this->HoverPanel->BackgroundColor);

			p.Text(this->Lines[this->CurrentHoverLine]->Parts[this->CurrentHoverPart]->Popup, 4, -2, Color::White);
		};
		this->HoverPanel->OnUpdate = [this]() {
			if (!this->HoverPanel->ShouldRender) return;

			float curtime = TL_GET_TIME_MS;
			if (curtime - this->HoverStart > 100 && !this->HoverPanel->IsHovering() && !this->MainPanel->IsHovering()) {
				this->HoverPanel->Hide();
			}
		};

		this->ScrollPanel = new UIScrollPanel(this, true);
		this->ScrollPanel->SetPos(0, 0);
		this->ScrollPanel->ScrollAmplifier = 4.8f;

		this->MainPanel = new UIPanel(null);
		this->MainPanel->OnDraw = [this](Render& p) {
			p.Box(0, 0, this->ScrollPanel->Back->W, this->ScrollPanel->Back->H, this->BorderColor);
			p.Box(1, 1, this->ScrollPanel->Back->W - 2, this->ScrollPanel->Back->H - 2, this->BackColor);

			int linepaddingcount = (int)Utilities::GetNumberPadded(this->Lines.Count, 1, '0').size();

			float cy = this->Padding + this->ScrollPanel->Back->Y * -1.0f;
			float lineh = this->Font->FontHandle->height;

			int startline = this->ScrollPanel->Back->Y / -16;
			cy -= this->ScrollPanel->Back->Y * -1 - startline * 16;
			int endline = startline + this->H / 16 + 1;
			if (endline >= this->Lines.Count) endline = this->Lines.Count;

			Vector2 apos = this->GetAbsoluteLocation();
			p.EnableClipping((int)apos.X + this->Padding, (int)apos.Y + this->Padding, (int)apos.X + this->ScrollPanel->Back->W - this->Padding * 2, (int)apos.Y + this->ScrollPanel->Back->H - this->Padding * 2);

			Vector2 mpos = this->UIMan->LastMousePosition - apos;
			mpos.Y += this->ScrollPanel->Back->Y * -1;

			float curtime = TL_GET_TIME_MS;
			int hoverline = this->CurrentHoverLine;
			int hoverpart = this->CurrentHoverPart;

			if (this->LineNumbers) {
				p.Box(1, 1, this->LineNumbersOffset, this->ScrollPanel->Back->H - 2, this->LineNumbersBackColor);
				p.Box(this->LineNumbersOffset, 1, 1, this->ScrollPanel->Back->H - 2, this->LineNumbersBorderColor);
			}

			for (int line = startline; line < endline; line++) {
				float cx = (float)this->Padding + this->LineNumbersOffset;

				UIAdvancedTextEntryLine* ld = this->Lines[line];
				if (ld->BackgroundColor != this->BackColor) {
					p.Box(this->LineNumbersOffset + 1.0f, cy, (float)this->ScrollPanel->Back->W, 16.0f, ld->BackgroundColor);
				}

				if (this->LineNumbers) {
					p.Text(this->Font, Utilities::GetNumberPadded(ld->Number, linepaddingcount, '0'), (float)this->Padding, cy, this->LineNumbersColor);
				}

				for (int i = 0; i < ld->Parts.Count; i++) {
					UIAdvancedTextEntryLinePart* part = ld->Parts[i];

					if (part->BackColor != ld->BackgroundColor) {
						p.Box(cx, cy, part->Width, lineh, part->BackColor);
					}

					p.Text(this->Font, part->Text, cx, cy, part->ForeColor);

					if (cx <= mpos.X && cx + part->Width >= mpos.X && cy <= mpos.Y && cy + lineh >= mpos.Y) {
						hoverpart = i;
						hoverline = line;
						this->UIMan->CurrentCursorShape = part->CursorIcon;

						if (hoverpart != this->CurrentHoverPart || hoverline != this->CurrentHoverLine) {
							this->CurrentHoverPart = hoverpart;
							this->CurrentHoverLine = hoverline;
							this->HoverStart = curtime;
							this->HoverPanel->Hide();
						} else {
							if (part->Popup.size() > 0 && !this->HoverPanel->ShouldRender && curtime - this->HoverStart > this->HoverTimeBeforePopup) {
								Vector2 tsize = p.GetTextSize(this->Font, part->Popup);

								this->HoverPanel->SetPos((int)(apos.X + mpos.X) - (int)tsize.X / 2, (int)(apos.Y + mpos.Y - tsize.Y - 5 - this->ScrollPanel->Back->Y * -1));
								this->HoverPanel->SetSize((int)tsize.X + 6, (int)tsize.Y);
								this->HoverPanel->Show();
								this->HoverPanel->OnTop();
							}

							if (this->HoverPanel->ShouldRender) {
								this->HoverStart = curtime;
							}
						}
					}

					cx += part->Width;
				}

				cy += lineh;
			}

			p.DisableClipping();
		};

		this->MainPanel->OnClick = [this](int x, int y, int button) {
			int linepaddingcount = (int)Utilities::GetNumberPadded(this->Lines.Count, 1, '0').size();

			float cy = this->Padding + this->ScrollPanel->Back->Y * -1.0f;
			float lineh = this->Font->FontHandle->height;

			int startline = this->ScrollPanel->Back->Y / -16;
			cy -= this->ScrollPanel->Back->Y * -1 - startline * 16;
			int endline = startline + this->H / 16 + 1;
			if (endline >= this->Lines.Count) endline = this->Lines.Count;

			Vector2 apos = this->GetAbsoluteLocation();
			Vector2 mpos = this->UIMan->LastMousePosition - apos;
			mpos.Y += this->ScrollPanel->Back->Y * -1;

			float curtime = TL_GET_TIME_MS;
			int hoverline = this->CurrentHoverLine;
			int hoverpart = this->CurrentHoverPart;

			for (int line = startline; line < endline; line++) {
				float cx = (float)this->Padding + this->LineNumbersOffset;

				UIAdvancedTextEntryLine* ld = this->Lines[line];

				if (cy <= mpos.Y && cy + lineh >= mpos.Y) {
					for (int i = 0; i < ld->Parts.Count; i++) {
						UIAdvancedTextEntryLinePart* part = ld->Parts[i];

						if (cx <= mpos.X && cx + part->Width >= mpos.X) {
							part->OnClick();
							return;
						}

						cx += part->Width;
					}
				}

				cy += lineh;
			}
		};

		this->ScrollPanel->AddItem(this->MainPanel);

		this->OnScroll = [this](int x, int y) {
			this->ScrollPanel->OnScroll(x, y);
		};

		this->OnParseLine = [this](UIAdvancedTextEntryLine* line, std::string text, Dictonary<std::string, int>& settings) {
			line->AddPart(text, "", this->TextColor, this->BackColor);
		};
	}

	void UIAdvancedTextEntry::Draw(Render& p) {
	}

	void UIAdvancedTextEntry::Clear() {
		for (int i = this->Lines.Count - 1; i >= 0; i--) {
			delete this->Lines[i];
		}

		this->Lines.Clear();
		this->MainPanel->SetSize(0, 0);
		this->MarkForFullRedraw();
	}

	void UIAdvancedTextEntry::Update() {

	}

	void UIAdvancedTextEntry::InvalidateLayout() {
		this->ScrollPanel->SetSize(this->W, this->H);
	}

	void UIAdvancedTextEntry::SetText(std::string line) {
		for (int i = this->Lines.Count - 1; i >= 0; i--) {
			delete this->Lines[i];
		}
		this->Lines.Clear();

		this->MainPanel->SetSize(0, 0);
		this->ScrollPanel->Back->X = 0;
		this->ScrollPanel->Back->Y = 0;
		this->ScrollPanel->InvalidateLayout();

		Dictonary<std::string, int> settings;

		int lastpos = 0;
		int linesize = (int)line.size();
		for (int i = 0; i < linesize; i++) {
			if (line[i] == '\n') {
				this->AddLine(line.substr(lastpos, i - lastpos), settings);

				lastpos = i + 1;
			} else if (line[i] == '\r') {
				this->AddLine(line.substr(lastpos, i - lastpos), settings);

				lastpos = i + 2;
				i++;
			}
		}

		if (lastpos != linesize) {
			this->AddLine(line.substr(lastpos), settings);
		}
	}

	void UIAdvancedTextEntry::AddLine(std::string line, Dictonary<std::string, int>& settings) {
		UIAdvancedTextEntryLine* ld = new UIAdvancedTextEntryLine();
		ld->BackgroundColor = this->BackColor;
		ld->Number = this->Lines.Count + 1;

		this->OnParseLine(ld, line, settings);
		this->AddLine(ld);
	}

	void UIAdvancedTextEntry::AddLine(std::string line) {
		UIAdvancedTextEntryLine* ld = new UIAdvancedTextEntryLine();
		ld->BackgroundColor = this->BackColor;
		ld->Number = this->Lines.Count + 1;

		Dictonary<std::string, int> dict;
		this->OnParseLine(ld, line, dict);

		this->AddLine(ld);
	}

	void UIAdvancedTextEntry::ResizeToContents() {
		this->SetSize(this->W, (int)(this->Lines.Count * this->Font->FontHandle->height) + this->Padding * 2 + 2);
	}

	void UIAdvancedTextEntry::AddLine(UIAdvancedTextEntryLine* line) {
		float totalw = (float)this->Padding;

		for (int i = 0; i < line->Parts.Count; i++) {
			UIAdvancedTextEntryLinePart* part = line->Parts[i];

			Vector2 tsize = this->UIMan->Drawer.GetTextSize(this->Font, part->Text);
			part->Width = tsize.X;
			
			if (this->WordWrap && totalw + part->Width > this->W && part->Text.find(" ") != std::string::npos) {
				UIAdvancedTextEntryLine* preline = new UIAdvancedTextEntryLine();
				preline->BackgroundColor = line->BackgroundColor;
				preline->Number = line->Number;

				for (int i2 = 0; i2 < i; i2++) {
					preline->Parts.Add(line->Parts.RemoveAt(0));
				}

				for (int i2 = (int)part->Text.size() - 1; i2 > 0; i2--) {
					if (part->Text[i2] != ' ') continue;

					std::string tmp = part->Text.substr(0, i2);

					float w = this->UIMan->Drawer.GetTextSize(this->Font, tmp).X;
					if (totalw + w < this->W) {
						preline->AddPart(tmp, part->Popup, part->ForeColor, part->BackColor);

						part->Text = part->Text.substr(i2 + 1);

						tsize = this->UIMan->Drawer.GetTextSize(this->Font, part->Text);
						part->Width = tsize.X;
						break;
					}
				}

				if (preline->Parts.Count > 0) {
					this->AddLine(preline);
					line->Number++;
					i = -1;
				} else {
					delete preline;
				}

				totalw = (float)this->Padding;
				continue;
			}

			totalw += part->Width;
		}

		totalw += this->Padding;

		if (line->Parts.Count == 0) return;
		this->Lines.Add(line);

		if (this->LineNumbers) {
			Vector2 tsize = this->UIMan->Drawer.GetTextSize(this->Font, Utilities::GetNumberPadded(this->Lines.Count, 1, '0'));

			this->LineNumbersOffset = (int)tsize.X + 8;
			totalw += this->LineNumbersOffset;
		}

		if (totalw < this->MainPanel->W) totalw = (float)this->MainPanel->W;
		if (totalw < this->W) totalw = (float)this->W;

		float totalh = this->Lines.Count * this->Font->FontHandle->height + this->Padding * 2.0f;
		if (totalh < this->H) totalh = (float)this->H;

		this->MainPanel->SetSize((int)totalw, (int)totalh);
		this->ScrollPanel->InvalidateLayout();
		this->ScrollPanel->W = this->W;
		this->ScrollPanel->H = this->H;
		this->MarkForFullRedraw();
	}
}
