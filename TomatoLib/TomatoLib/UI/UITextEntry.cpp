#include "UITextEntry.h"
#include "UIManager.h"

#include <GLFW/glfw3.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include <cstring>

namespace TomatoLib {

	bool CheckLet(int l, bool spc) {
		if (((l >= 48 && l <= 57) || (l >= 65 && l <= 90) || (l >= 97 && l <= 122) || (l == 32 && !spc))) {
			return true;
		}
		return false;
	}

	UITextEntry::UITextEntry(UIBase* parent) : UIBase(parent) {
		this->DrawColor = Color::Black;
		this->Text = "";
		this->Caret = 0;
		this->MaxInput = -1;
		this->MaskChar = 0;
		this->OnlyLetters = false;
		this->OnlyNumbers = false;
		this->MultiLine = false;
		this->CanAcceptInput = true;
		this->WideScroll = 0;
		this->Font = Render::DefaultFont;
		
		
		// TODO: fix AlwaysRedraw
		// this->AlwaysRedraw = false;

		this->TextColor = Color::Black;
		this->CaretColor = Color::Black;
		this->BackgroundColor = Color(200, 200, 200);
		this->BorderColor = Color::Black;

		this->OnFocus = [this](bool isfocused) {
			if (isfocused) this->Caret = (int)this->Text.size();
		};

		std::function<void(int, int)> oldinput = this->OnInput;
		this->OnInput = [this, oldinput](int key, int mods) {
			if (key == GLFW_KEY_BACKSPACE && this->Caret > 0) {
				if ((mods & 2) > 0 && (CheckLet(this->Text[this->Caret - 1], false))) {
					bool spc = false;
					for (int i = 1; i < this->Caret + 1; i++) {
						char l = this->Text[this->Caret - i];
						if (CheckLet(l, spc)) {
							if (l != 32) spc = true;
							if (i == this->Caret) {
								this->Text.erase(this->Caret - i, i);
								this->Caret = 0;
								if (this->OnTextChange != nullptr) this->OnTextChange();
								this->MarkForFullRedraw();
							}
							continue;
						} else {
							this->Text.erase(this->Caret - i + 1, i - 1);
							this->Caret -= (i - 1);
							if (this->OnTextChange != nullptr) this->OnTextChange();
							this->MarkForFullRedraw();
							break;
						}
					}
				} else {
					if (this->Caret == this->Text.size()) {
						this->Text.pop_back();
					} else {
						this->Text.erase(this->Caret - 1, 1);
					}
					this->Caret -= 1;
					if (this->OnTextChange != nullptr) this->OnTextChange();
					this->MarkForFullRedraw();
				}
				return;
			} else if (key == GLFW_KEY_DELETE && this->Text.size() > (unsigned int)this->Caret) {
				if ((mods & 2) > 0 && (CheckLet(this->Text[this->Caret], false))) {
					bool spc = false;
					for (unsigned int i = 0; i < (this->Text.size() - (unsigned int)this->Caret); i++) {
						char l = this->Text[this->Caret + i];
						if (CheckLet(l, spc)) {
							if (l != 32) spc = true;
							if (i == (this->Text.size() - this->Caret - 1)) {
								this->Text.erase(this->Caret, i + 1);
								if (this->OnTextChange != nullptr) this->OnTextChange();
								this->MarkForFullRedraw();
							}
							continue;
						} else {
							this->Text.erase(this->Caret, i);
							if (this->OnTextChange != nullptr) this->OnTextChange();
							this->MarkForFullRedraw();
							break;
						}
					}
				} else {
					this->Text.erase(this->Caret, 1);
					if (this->OnTextChange != nullptr) this->OnTextChange();
					this->MarkForFullRedraw();
				}
				return;
			} else if (key == GLFW_KEY_LEFT && this->Text.size() > 0 && (unsigned int)this->Caret > 0) {
				this->Caret -= 1;
				return;
			} else if (key == GLFW_KEY_RIGHT && this->Text.size() > 0) {
				if ((unsigned int)this->Caret < this->Text.size()) this->Caret += 1;
				return;
			} else if (key == GLFW_KEY_END && this->Text.size() > 0) {
				this->Caret = (int)this->Text.size();
				return;
			} else if (key == GLFW_KEY_HOME) {
				this->Caret = 0;
				return;
			} else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
				if (!this->MultiLine) this->OnEnter();
				return;
			}

#ifdef _MSC_VER
			else if (key == GLFW_KEY_V && (mods & 2) > 0) {
				OpenClipboard(NULL);
				HANDLE pText = GetClipboardData(CF_TEXT);
				CloseClipboard();
				char* text = (char*)GlobalLock(pText);
				size_t tlen = strlen(text);
				for (size_t i = 0; i < tlen; i++) {
					this->OnCharInput(text[i]);
				}
				return;
			}
#endif

			oldinput(key, mods);
		};

		this->OnClick = [this](int x, int y, int button) {
			float xoffset = this->WideScroll * -1 + 3;

			char* str = new char[this->Text.size() + 1];
			memset((void*)str, 0, this->Text.size() + 1);

			unsigned int i = 0;
			while (true) {
				Vector2 tsize = this->UIMan->Drawer.GetTextSize(str);
				if (x < xoffset + tsize.X) {
					if (i > 0) {
						Vector2 lettersize = this->UIMan->Drawer.GetTextSize(str[i - 1]);
						if (x < xoffset + tsize.X - lettersize.X / 2) {
							this->Caret = i - 1;
						} else {
							this->Caret = i;
						}
						break;
					}

					this->Caret = i;
					break;
				}

				if (i < this->Text.size()) {
					str[i++] = this->MaskChar == 0 ? this->Text[i] : this->MaskChar;
				} else {
					this->Caret = i;
					break;
				}
			}
			delete[] str;

		};

		this->OnEnter = [this]() {

		};

		this->OnCharInput = [this](int key) {
			if (key > 255) return;

			if (this->MaxInput != -1 && (int)this->Text.size() + 1 > this->MaxInput) {
				return;
			}

			char c = key;
			if (!this->MultiLine && (c == '\n' || c == '\r')) return;
			if (this->OnlyLetters && (c < 'a' || c > 'Z')) return;
			if (this->OnlyNumbers && (c < '0' || c > '9')) return;

			this->Text.insert(this->Caret, std::string("") + c);

			this->Recalc();
			this->Caret++;
			if (this->OnTextChange != nullptr) this->OnTextChange();
			this->MarkForFullRedraw();
		};
	}

	std::string UITextEntry::GetText() {
		return this->Text;
	}

	void UITextEntry::SetText(std::string text) {
		this->Text = text;
		this->WideScroll = 0;
		this->Caret = 0;
		this->Recalc();
		this->Caret = (int)text.size();

		if (this->OnTextChange != nullptr) this->OnTextChange();
		this->MarkForFullRedraw();
	}

	void UITextEntry::Clear() {
		this->Text.clear();
		this->Caret = 0;
		this->WideScroll = 0;
		if (this->OnTextChange != nullptr) this->OnTextChange();
		this->MarkForFullRedraw();

		this->Recalc();
	}

	void UITextEntry::Recalc() {
		Vector2 Size = this->UIMan->Drawer.GetTextSize(this->Text);

		//printf("%d %d", (int)Size.X, this->WideScroll);

		float pushback = 0;
		if (this->MaskChar != 0) {
			char* tmp = new char[this->Caret + 1];
			memset(tmp, this->MaskChar, this->Caret);
			tmp[this->Caret] = 0;

			pushback = this->UIMan->Drawer.GetTextSize(tmp).X;
			delete tmp;
		} else {
			pushback = this->UIMan->Drawer.GetTextSize(this->Text.substr(0, this->Caret)).X;
		}

		pushback -= this->WideScroll;

		if ((pushback) >(this->W - 6)) {
			this->WideScroll += ((pushback)-(this->W - 6));
		} else if (pushback < 0) {
			this->WideScroll += pushback;
		}

		if ((int)Size.X >= (this->W - 6) && (((int)Size.X - this->WideScroll) < (this->W - 6))) {
			this->WideScroll -= ((this->W - 6) - ((int)Size.X - this->WideScroll));
		}

		this->CaretX = pushback;
	}

	void UITextEntry::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(1, 1, this->W - 2, this->H - 2, this->BackgroundColor);

		this->Recalc();
		
		Vector2 Offset = p.GetDrawingOffset();

		float y = this->H / 2.0f - this->Font->FontHandle->height / 2;

		p.EnableClipping((int)Offset.X + 1, (int)Offset.Y + 1, this->W - 2, this->H - 2);
		if (this->prevtime == 1 && this->UIMan->FocusPanel == this) p.Box(this->CaretX + 2, this->H / 2.0f - this->Font->FontHandle->height / 3, 2.0f, this->Font->FontHandle->height / 3 * 2, this->CaretColor);

		if (this->MaskChar != 0) {
			char* str = new char[this->Text.size() + 1];
			memset((void*)str, this->MaskChar, this->Text.size());
			str[this->Text.size()] = 0;

			p.Text(str, this->WideScroll * -1 + 3.0f, y, this->TextColor);

			delete[] str;
		} else {
			p.Text(this->Text, this->WideScroll * -1 + 3.0f, y, this->TextColor);
		}
		p.DisableClipping();

	}

	void UITextEntry::Update() {
		int time = (int)(glfwGetTime() * 2);
		time = time % 2;

		if (this->prevtime != time) {
			this->prevtime = time;
			this->MarkForFullRedraw();
		}
	}

	void UITextEntry::InvalidateLayout() {
	}
}
