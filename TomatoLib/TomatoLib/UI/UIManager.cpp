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

		int oldshape = this->CursorShape;
		this->CursorShape = GLFW_ARROW_CURSOR;
		for (unsigned int i = 0; i < this->Children.size(); i++) {
			this->Children[i]->_InternalUpdate();
		}

		if (oldshape != this->CursorShape) {
#ifdef TL_ENABLE_GLFW
			if (this->CurrentCursor != null) glfwDestroyCursor(this->CurrentCursor);
			this->CurrentCursor = glfwCreateStandardCursor(this->CursorShape);
#endif
		}
	}

	void UIManager::_CheckClick(UIBase* panel, int x, int y) {
		for (int i = (int)panel->Children.size() - 1; i > -1; i--) {
			if (this->HoldPanel != null) break;

			UIBase* child = panel->Children[i];

			if (!child->ShouldRender) continue;
			if (child->PassTrough) continue;

			if (child->X <= x && child->Y <= y && (child->X + child->W) > x && (child->Y + child->H) > y) {
				if (!child->CanClick) continue;
				this->_CheckClick(child, x - child->X, y - child->Y);
			}
		}

		if (this->HoldPanel == null && !panel->PassTroughSelfOnly) {
			this->HoldPanel = panel;
		}
	}

	bool UIManager::HandleMouseInteraction(int x, int y, int button, bool pressed, int mods) {
		if (this->ShowCursor) {
			if (pressed) {
				this->HoldPanel = null;
				for (int i = (int)this->Children.size() - 1; i > -1; i--) {
					UIBase* Pan = this->Children[i];

					if (this->HoldPanel != null) break;
					if (Pan->ShouldRender == false) continue;
					if (Pan->PassTrough) continue;

					if (Pan->X <= x && Pan->Y <= y && (Pan->X + Pan->W) > x && (Pan->Y + Pan->H) > y) {
						if (!Pan->CanClick) continue;
						this->_CheckClick(Pan, x - Pan->X, y - Pan->Y);
					}
				}
				if (this->HoldPanel != null) {
					if (this->FocusPanel != this->HoldPanel) {
						if (this->FocusPanel != null) {
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
					if (this->FocusPanel != null) {
						UIBase* tmp = this->FocusPanel;
						this->FocusPanel = null;
						tmp->OnFocus(false);
					}
				}
			} else {
				if (this->HoldPanel != null) {
					UIBase* tmp = this->HoldPanel;
					this->HoldPanel = null;
					Vector2 apos = tmp->GetAbsoluteLocation();
					tmp->OnRelease(x - (int)apos.X, y - (int)apos.Y, button);

					if (x >= apos.X && x <= apos.X + tmp->W && y >= apos.Y && y <= apos.Y + tmp->H) {
						tmp->OnClick(x - (int)apos.X, y - (int)apos.Y, button);
					}
				}
			}

		}

		return false;
	}

	bool UIManager::HandleKeyboardInteraction(int key, unsigned char pressed, int mods) {
		if (pressed) {
			if (this->FocusPanel != null) {
				if (!this->FocusPanel->CanAcceptInput) {
					if (key == GLFW_KEY_GRAVE_ACCENT && this->Console != null) {
						if (this->Console->ShouldRender == false) {
							this->Console->Show();
						} else {
							this->Console->Hide();
						}
					}

					return true;
				}

				if (key == GLFW_KEY_GRAVE_ACCENT && this->Console != null && this->Console->HasFocus()) {
					this->Console->Hide();
					return true;
				}

				this->FocusPanel->OnKeyDown(key);
				if (this->FocusPanel != null) this->FocusPanel->OnInput(key, mods);
				return true;
			}

			if (key == GLFW_KEY_GRAVE_ACCENT) {
				if (this->Console != null) {
					if (this->Console->ShouldRender == false) {
						this->Console->Show();
					} else {
						this->Console->Hide();
					}
					return true;
				}
			}
		} else {
			if (this->FocusPanel != null && this->FocusPanel->CanAcceptInput) {
				this->FocusPanel->OnKeyUp(key);
				return true;
			}
		}

		return false;
	}

	bool UIManager::HandleScrollInteraction(int scrollx, int scrolly) {
		if (this->ShowCursor) {
			Vector2 mpos = this->LastMousePosition;
			int x = (int)mpos.X;
			int y = (int)mpos.Y;

			this->HoldPanel = null;
			for (int i = (int)this->Children.size() - 1; i > -1; i--) {
				UIBase* Pan = this->Children[i];

				if (this->HoldPanel != null) break;
				if (Pan->ShouldRender == false) continue;
				if (Pan->PassTrough) continue;

				if (Pan->X <= x && Pan->Y <= y && (Pan->X + Pan->W) > x && (Pan->Y + Pan->H) > y) {
					if (!Pan->CanClick) break;
					this->_CheckClick(Pan, x - Pan->X, y - Pan->Y);
				}
			}

			if (this->HoldPanel != null) {
				UIBase* catcher = this->HoldPanel;
				while (!catcher->CanCatchScroll && catcher->Parent != null) {
					catcher = catcher->Parent;
				}

				catcher->OnScroll(scrollx, scrolly);
				return true;
			}
		}
		return false;
	}

	bool UIManager::HandleCharInteraction(int key) {
		if (this->FocusPanel != null) {
			if (!this->FocusPanel->CanAcceptInput) return true;
			if (key == GLFW_KEY_GRAVE_ACCENT && this->Console != null && this->Console->HasFocus()) return true;

			this->FocusPanel->OnCharInput(key);
			return true;
		}

		return false;
	}

	UIManager::UIManager(Render& drawer) : Drawer(drawer) {
		this->FocusPanel = null;
		this->HoldPanel = null;
		this->Console = null;
		this->ShowCursor = true;
		this->OverrideShowCursor = true;
		this->CurrentCursor = null;

		TomatoLib::UIBase::DefaultUImanager = this;

		this->Console = new UIConsole(null);
		this->Console->SetSize((int)drawer.ScreenSize.X, (int)drawer.ScreenSize.Y);
		this->Console->TopMost = true;
	}

	UIManager::~UIManager() {
	}
}
