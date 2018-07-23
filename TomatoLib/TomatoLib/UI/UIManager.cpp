#include "UIManager.h"
#include <vector>
#include "UIBase.h"
#include "../Game/Window.h"
#include "Menus/UIConsole.h"
#include "Menus/UIGraph.h"
#include "../Defines.h"
#include "../Utilities/Utilities.h"

#include "../Config.h"

#ifdef TL_ENABLE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace TomatoLib {
	UIBase* UIManager::AddChild(UIBase* obj) {
		obj->UIMan = this;

		bool added = false;
		for (int i2 = (int)this->Children.size() - 1; i2 >= 0; i2--) {
			if (!this->Children[i2]->TopMost) {
				this->Children.insert(this->Children.begin() + i2 + 1, obj);
				added = true;
				break;
			}
		}

		if (!added) this->Children.insert(this->Children.begin(), obj);

		return obj;
	}

	void UIManager::KillChildren(){
		while (!this->Children.empty()){
			this->Children[0]->Kill();
		}
	}

	void UIManager::RemoveChild(UIBase* child) {
		for (unsigned int i = 0; i < this->Children.size(); i++) {
			if (this->Children[i] == child) {
				this->Children.erase(this->Children.begin() + i);
				break;
			}
		}
	}

	void UIManager::Draw(Render& r) {
		this->ShowCursor = this->OverrideShowCursor;

		r.SetDrawingOffset(0, 0);

		for (unsigned int i = 0; i < this->Children.size(); i++) {
			this->Children[i]->_InternalDraw(r);
		}
	}

	void UIManager::Update() {
		Vector2 mpos = Window::CurrentWindow->GetMouse();

		this->LastMousePosition = mpos;

		int oldshape = this->CurrentCursorShape;
		this->CurrentCursorShape = GLFW_ARROW_CURSOR;
		for (unsigned int i = 0; i < this->Children.size(); i++) {
			this->Children[i]->_InternalUpdate();
		}

		if (oldshape != this->CurrentCursorShape) {
#ifdef TL_ENABLE_GLFW
			if (this->CurrentCursor != nullptr) glfwDestroyCursor(this->CurrentCursor);
			this->CurrentCursor = glfwCreateStandardCursor(this->CurrentCursorShape);
#endif
		}
	}

	void UIManager::_CheckClick(UIBase* panel, int x, int y) {
		for (int i = (int)panel->Children.size() - 1; i > -1; i--) {
			if (this->HoldPanel != nullptr) break;

			UIBase* child = panel->Children[i];

			if (!child->ShouldRender) continue;
			if (child->PassTrough) continue;

			if (child->X <= x && child->Y <= y && (child->X + child->W) > x && (child->Y + child->H) > y) {
				if (!child->CanClick) continue;
				this->_CheckClick(child, x - child->X, y - child->Y);
			}
		}

		if (this->HoldPanel == nullptr && !panel->PassTroughSelfOnly) {
			this->HoldPanel = panel;
		}
	}

	bool UIManager::HandleMouseInteraction(int x, int y, int button, bool pressed, int mods) {
		if (this->OnBeforeMouseInteracton(x, y, button, pressed, mods)) return true;

		if (this->ShowCursor) {
			if (pressed) {
				this->HoldPanel = nullptr;
				for (int i = (int)this->Children.size() - 1; i > -1; i--) {
					UIBase* Pan = this->Children[i];

					if (this->HoldPanel != nullptr) break;
					if (Pan->ShouldRender == false) continue;
					if (Pan->PassTrough) continue;


					if (Pan->X <= x && Pan->Y <= y && (Pan->X + Pan->W) > x && (Pan->Y + Pan->H) > y) {
						if (!Pan->CanClick) continue;

						Vector2 apos = Pan->GetAbsoluteLocation();
						if (!Pan->OnCanClick(x - (int)apos.X, y - (int)apos.Y, button)) continue;
						this->_CheckClick(Pan, x - Pan->X, y - Pan->Y);
					}
				}

				if (this->HoldPanel != nullptr) {
					if (this->FocusPanel != this->HoldPanel) {
						if (this->FocusPanel != nullptr) {
							UIBase* tmp = this->FocusPanel;
							this->FocusPanel = this->HoldPanel; // override already so that it knows.
							tmp->OnFocus(false);
						}
						this->HoldPanel->SetFocus();
					}
					Vector2 apos = this->HoldPanel->GetAbsoluteLocation();
					this->HoldPanel->OnPress(x - (int)apos.X, y - (int)apos.Y, button);
					return true;
				} else {
					if (this->FocusPanel != nullptr) {
						UIBase* tmp = this->FocusPanel;
						this->FocusPanel = nullptr;
						tmp->OnFocus(false);
					}
				}
			} else {
				if (this->HoldPanel != nullptr) {
					UIBase* tmp = this->HoldPanel;
					this->HoldPanel = nullptr;
					Vector2 apos = tmp->GetAbsoluteLocation();
					tmp->OnRelease(x - (int)apos.X, y - (int)apos.Y, button);

					if (x >= apos.X && x <= apos.X + tmp->W && y >= apos.Y && y <= apos.Y + tmp->H) {
						tmp->OnClick(x - (int)apos.X, y - (int)apos.Y, button);
					}
				}
			}

		}

		return this->OnAfterMouseInteracton(x, y, button, pressed, mods);
	}

	bool UIManager::HandleKeyboardInteraction(int key, bool pressed, int mods) {
		if (this->OnBeforeKeyboardInteracton(key, pressed, mods)) return true;

		if (this->ShowCursor) {
			if (pressed) {
				if (this->FocusPanel != nullptr) {
					if (!this->FocusPanel->OnCanPress(key)) return false;

					if (!this->FocusPanel->CanAcceptInput) {
						if ((key == GLFW_KEY_GRAVE_ACCENT || key == GLFW_KEY_F1) && this->Console != nullptr) {
							if (this->Console->ShouldRender == false) {
								this->Console->Show();
							} else {
								this->Console->Hide();
							}
						}

						return true;
					}

					if ((key == GLFW_KEY_GRAVE_ACCENT || key == GLFW_KEY_F1) && this->Console != nullptr && this->Console->HasFocus()) {
						this->Console->Hide();
						return true;
					}

					this->FocusPanel->OnKeyDown(key);
					if (this->FocusPanel != nullptr) this->FocusPanel->OnInput(key, mods);
					return true;
				}

				if ((key == GLFW_KEY_GRAVE_ACCENT || key == GLFW_KEY_F1)) {
					if (this->Console != nullptr) {
						if (this->Console->ShouldRender == false) {
							this->Console->Show();
						} else {
							this->Console->Hide();
						}
						return true;
					}
				}
			} else {
				if (this->FocusPanel != nullptr && this->FocusPanel->CanAcceptInput) {
					this->FocusPanel->OnKeyUp(key);
					return true;
				}
			}
		}

		return this->OnAfterKeyboardInteracton(key, pressed, mods);
	}

	bool UIManager::HandleScrollInteraction(int scrollx, int scrolly) {
		if (this->OnBeforeScrollInteracton(scrollx, scrolly)) return true;

		if (this->ShowCursor) {
			Vector2 mpos = this->LastMousePosition;
			int x = (int)mpos.X;
			int y = (int)mpos.Y;

			this->HoldPanel = nullptr;
			for (int i = (int)this->Children.size() - 1; i > -1; i--) {
				UIBase* Pan = this->Children[i];

				if (this->HoldPanel != nullptr) break;
				if (Pan->ShouldRender == false) continue;
				if (Pan->PassTrough) continue;

				if (Pan->X <= x && Pan->Y <= y && (Pan->X + Pan->W) > x && (Pan->Y + Pan->H) > y) {
					if (!Pan->CanClick) break;

					Vector2 apos = Pan->GetAbsoluteLocation();
					if (!Pan->OnCanClick(x - (int)apos.X, y - (int)apos.Y, 0)) continue;
					this->_CheckClick(Pan, x - Pan->X, y - Pan->Y);
				}
			}

			if (this->HoldPanel != nullptr) {
				UIBase* catcher = this->HoldPanel;
				while (!catcher->CanCatchScroll && catcher->Parent != nullptr) {
					catcher = catcher->Parent;
				}

				catcher->OnScroll(scrollx, scrolly);
				return true;
			}
		}

		return this->OnAfterScrollInteracton(scrollx, scrolly);
	}

	bool UIManager::HandleCharInteraction(int key) {
		if (this->OnBeforeCharInteraction(key)) return true;

		if (this->FocusPanel != nullptr) {
			if (!this->FocusPanel->CanAcceptInput) return true;
			if (key == GLFW_KEY_GRAVE_ACCENT && this->Console != nullptr && this->Console->HasFocus()) return true;

			if (!this->FocusPanel->OnCanPress(key)) return false;

			this->FocusPanel->OnCharInput(key);
			return true;
		}

		return this->OnAfterCharInteraction(key);
	}

	UIManager::UIManager(Render& drawer) : Drawer(drawer) {
		this->FocusPanel = nullptr;
		this->HoldPanel = nullptr;
		this->Console = nullptr;
		this->ShowCursor = true;
		this->OverrideShowCursor = true;
		this->CurrentCursor = nullptr;

		TomatoLib::UIBase::DefaultUImanager = this;

		this->Console = new UIConsole(nullptr);
		this->Console->SetSize((int)drawer.ScreenSize.X, (int)drawer.ScreenSize.Y);
		this->Console->TopMost = true;
	}

	UIManager::~UIManager() {
		if (TomatoLib::UIBase::DefaultUImanager == this) {
			TomatoLib::UIBase::DefaultUImanager = nullptr;
		}
	}
}
