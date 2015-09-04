#include "UIBase.h"
#include "UIManager.h"
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UIManager* UIBase::DefaultUImanager = nullptr;

	UIBase::UIBase(UIBase* parent) {
		this->DrawColor = Color::White;
		this->Parent = nullptr;
		this->UIMan = UIBase::DefaultUImanager;
		this->X = 0;
		this->Y = 0;
		this->W = 0;
		this->H = 0;
		this->ShouldRender = parent != nullptr;
		this->CanClick = true;
		this->Dock = 0;
		this->TabIndex = -1;
		this->CanAcceptInput = false;
		this->TopMost = false;
		this->PassTrough = false;
		this->PassTroughSelfOnly = false;
		this->CanCatchScroll = false;
		this->Frozen = false;
		this->AlwaysRedraw = true;
		this->ShouldRedraw = true;
		this->Buffer = nullptr;

		this->SetParent(parent);

		this->OnKill = [this]() { };
		this->OnFocus = [this](bool focus) { };
		this->OnKeyUp = [this](int key) { };
		this->OnKeyDown = [this](int key) { };
		this->OnScroll = [this](int x, int y) { };
		this->OnCharInput = [this](int key) { };
		this->OnClick = [this](int x, int y, int button) { };
		this->OnPress = [this](int x, int y, int button) {};
		this->OnRelease = [this](int x, int y, int button) {};
		this->OnInvalidateLayout = [this]() { };
		this->OnShow = [this]() { };
		this->OnHide = [this]() {};

		this->OnDraw = [this](Render& drawer) {
			this->Draw(drawer);
		};

		this->OnUpdate = [this]() { };

		this->OnInput = [this](int key, int mods) {
			if (key == GLFW_KEY_TAB) {
				if (this->TabIndex == -1 || this->Parent == nullptr) return;

				int lowesttab = this->TabIndex + 1;
				int lowestID = -1;

				for (unsigned int i = 0; i < this->Parent->Children.size(); i++) {
					UIBase* c = this->Parent->Children[i];
					if (c->TabIndex > -1) {
						if (c->TabIndex == this->TabIndex + 1) {
							c->SetFocus();
							return;
						}

						if (c->TabIndex < lowesttab) {
							lowesttab = c->TabIndex;
							lowestID = i;
						}
					}
				}

				if (lowestID > -1) {
					this->Parent->Children[lowestID]->SetFocus();
				}
			}
		};
	}

	void UIBase::Show() {
		this->GenerateTabIndexes();

		this->ShouldRender = true;
		this->SetFocus();
		this->OnShow();
	}

	UIBase::~UIBase() { }

	void UIBase::Hide() {
		this->ShouldRender = false;
		this->UnFocus();
		this->OnHide();
	}

	void UIBase::SetFocus() {
		if (this->TopMost) {
			this->OnTop();
		}

		if (this->UIMan != nullptr) this->UIMan->FocusPanel = this;
		this->OnFocus(true);
	}

	void UIBase::UnFocus() {
		List<UIBase*> childs;
		childs.Add(this);

		bool found = false;

		UIBase* c = nullptr;
		while (childs.Count > 0) {
			c = childs[childs.Count - 1];
			childs.RemoveAt(childs.Count - 1);

			if (this->UIMan != nullptr && c == this->UIMan->FocusPanel) {
				found = true;
				break;
			} else {
				for (unsigned int i = 0; i < c->Children.size(); i++) {
					childs.Add(c->Children[i]);
				}
			}
		}

		if (!found) return;

		if (this->UIMan != nullptr) this->UIMan->FocusPanel = nullptr;
		c->OnFocus(false);
	}

	bool UIBase::HasFocus() {
		if (this->UIMan == nullptr) return false;

		if (this->UIMan->FocusPanel == this) {
			return true;
		}

		for (unsigned int i = 0; i < this->Children.size(); i++) {
			if (this->Children[i]->HasFocus()) {
				return true;
			}
		}

		return false;
	}

	void UIBase::GenerateTabIndexes() {
		int curtab = 0;
		for (unsigned int i = 0; i < this->Children.size(); i++) {
			if (this->Children[i]->CanAcceptInput) {
				this->Children[i]->TabIndex = curtab++;
			}

			this->Children[i]->GenerateTabIndexes();
		}
	}

	inline void UIBase::_GetAbsoluteLocation(Vector2& pos) {
		pos.X += this->X;
		pos.Y += this->Y;

		if (this->Parent != nullptr) this->Parent->_GetAbsoluteLocation(pos);
	}

	Vector2 UIBase::GetAbsoluteLocation() {
		Vector2 pos;
		this->_GetAbsoluteLocation(pos);
		return pos;
	}

	UIBase* UIBase::GetAbsoluteParent() {
		return this->Parent == nullptr ? this : this->Parent->GetAbsoluteParent();
	}

	void UIBase::_InternalDraw(Render& drawer) {
		if (!this->ShouldRender) return;

		Vector2 oldoffset = drawer.GetDrawingOffset();
		Vector2 pos = this->GetAbsoluteLocation();

		drawer.SetDrawingOffset((int)pos.X, (int)pos.Y);
		drawer.EnableClipping((int)pos.X, (int)pos.Y, this->W, this->H);

		bool recorderstarted = false;
		if (this->ShouldRedraw) {
			drawer.RecorderStart();
			recorderstarted = true;
		}
		
		if (!this->Frozen && (this->ShouldRedraw || this->AlwaysRedraw)) {
			this->OnDraw(drawer);
		} else if(this->Buffer != nullptr) {
			drawer.Buffer(this->Buffer);
		}

		if (recorderstarted) {
			if (this->Buffer != nullptr) delete this->Buffer;
			this->Buffer = drawer.RecorderStop();
			this->ShouldRedraw = false;
		}

		for (unsigned int i = 0; i < this->Children.size(); i++) {
			UIBase* p = this->Children[i];
			Vector2 pos2 = p->GetAbsoluteLocation();
			if (pos2.X + p->W < drawer._ClippingPos.X) continue;
			if (pos2.Y + p->H < drawer._ClippingPos.Y) continue;

			if (pos2.X > drawer._ClippingPos.X + drawer._ClippingSize.X) continue;
			if (pos2.Y > drawer._ClippingPos.Y + drawer._ClippingSize.Y) continue;

			p->_InternalDraw(drawer);
		}

		drawer.SetDrawingOffset((int)oldoffset.X, (int)oldoffset.Y);
		drawer.DisableClipping();
	}

	void UIBase::_InternalUpdate() {
		if (this->Frozen) return;

		this->Update();
		this->OnUpdate();

		for (unsigned int i = 0; i < this->Children.size(); i++) {
			this->Children[i]->_InternalUpdate();
		}
	}

	void UIBase::AddChild(UIBase* child) {
		this->Children.push_back(child);
		child->UIMan = this->UIMan;
	}

	void UIBase::RemoveChild(UIBase* child) {
		for (unsigned int i = 0; i < this->Children.size(); i++) {
			if (this->Children[i] == child) {
				this->Children.erase(this->Children.begin() + i);
				break;
			}
		}
	}

	void UIBase::SetParent(UIBase* parent) {
		if (this->Parent != nullptr) {
			this->Parent->RemoveChild(this);
		} else if (this->UIMan != nullptr) {
			this->UIMan->RemoveChild(this);
		}

		if (parent != nullptr) {
			this->Parent = parent;
			parent->AddChild(this);
		} else if (this->UIMan != nullptr) {
			this->UIMan->AddChild(this);
		}
	}

	UIBase* UIBase::GetParent() {
		return this->Parent;
	}

	void UIBase::InvalidateLayout() {
	}

	void UIBase::MarkForFullRedraw() {
		if (!this->AlwaysRedraw) this->ShouldRedraw = true;

		for (unsigned int i = 0; i < this->Children.size(); i++) {
			this->Children[i]->MarkForFullRedraw();
		}
	}

	void UIBase::SetPos(int x, int y) {
		this->X = x;
		this->Y = y;

		this->MarkForFullRedraw();
	}

	void UIBase::GetPos(int& x, int& y) {
		x = this->X;
		y = this->Y;
	}

	Vector2 UIBase::GetPos() {
		return Vector2(this->X, this->Y);
	}

	void UIBase::SetSize(int w, int h) {
		this->W = w;
		this->H = h;

		this->MarkForFullRedraw();
		this->InvalidateLayout();
		this->OnInvalidateLayout();
	}

	void UIBase::GetSize(int& w, int& h) {
		w = this->W;
		h = this->H;
	}

	Vector2 UIBase::GetSize() {
		return Vector2(this->W, this->H);
	}

	void UIBase::SetWidth(int w) {
		this->W = w;
		this->MarkForFullRedraw();
		this->InvalidateLayout();
		this->OnInvalidateLayout();
	}

	int UIBase::GetWidth() {
		return this->W;
	}

	bool UIBase::IsHovering() {
		if (this->UIMan == nullptr) return false;

		Vector2 pos = this->GetAbsoluteLocation();

		Vector2 mousepos = this->UIMan->LastMousePosition;

		return pos.X <= mousepos.X && pos.X + this->W >= mousepos.X && pos.Y <= mousepos.Y && pos.Y + this->H >= mousepos.Y;
	}

	void UIBase::SetHeight(int h) {
		this->H = h;
		this->MarkForFullRedraw();
		this->InvalidateLayout();
		this->OnInvalidateLayout();
	}

	int UIBase::GetHeight() {
		return this->H;
	}

	void UIBase::OnTop() {
		if (this->UIMan == nullptr) return;

		if (this->Parent == nullptr) {
			for (unsigned int i = 0; i < this->UIMan->Children.size(); i++) {
				if (this->UIMan->Children[i] == this) {
					this->UIMan->Children.erase(this->UIMan->Children.begin() + i);
					this->UIMan->Children.push_back(this);
					break;
				}
			}
		} else {
			for (unsigned int i = 0; i < this->UIMan->Children.size(); i++) {
				if (this->Parent->Children[i] == this) {
					this->Parent->Children.erase(this->UIMan->Children.begin() + i);
					this->Parent->Children.push_back(this);
					break;
				}
			}
		}
	}

	void UIBase::Kill() {
		this->OnKill();

		if (this->Parent != nullptr) {
			this->Parent->RemoveChild(this);
		} else if(this->UIMan != nullptr) {
			this->UIMan->RemoveChild(this);
		}
		
		if (this->UIMan != nullptr) {
			if (this->UIMan->HoldPanel == this) {
				this->UIMan->HoldPanel = nullptr;
			}

			if (this->UIMan->FocusPanel == this) {
				this->UIMan->FocusPanel = nullptr;
			}
		}

		while (this->Children.size() > 0) {
			this->Children[0]->Kill();
		}

		delete this;
	}

}