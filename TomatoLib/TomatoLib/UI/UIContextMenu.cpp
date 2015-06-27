#include "UIContextMenu.h"
#include "UIManager.h"

#include <math.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UIContextMenu::UIContextMenu() : UIBase(null) {
		this->CanAcceptInput = true;
		this->BorderColor = Color(0, 0, 0, 255);
		this->BackColor = Color(60, 60, 60, 255);
		this->SplitLineColor = Color(0, 0, 0, 150);

		this->SetPos(0, 0);
		this->TopMost = true;
		this->Hide();
		this->AlwaysRedraw = false;

		this->OnClick = [this](int x, int y, int mouse) {
			float currentY = 0;

			for (int i = 0; i < this->Items.Count; i++) {
				float th = this->DefaultUImanager->Drawer.GetTextSize(this->Items[i].Label).Y;
				if (y >= currentY && y < currentY + th) {
					this->Items[i].Callback();
					this->Kill();
					return;
				}

				currentY += th;
			}
		};

		this->OnShow = [this]() {
			this->SetPos((int)this->UIMan->LastMousePosition.X, (int)this->UIMan->LastMousePosition.Y);
			this->OnTop();
		};
	}

	void UIContextMenu::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(1, 1, this->W - 2, this->H - 2, this->BackColor);

		Vector2 mousepos = this->UIMan->LastMousePosition;

		float currentY = 0;

		mousepos.Y -= this->Y;

		for (int i = 0; i < this->Items.Count; i++) {
			Vector2 ts = this->DefaultUImanager->Drawer.GetTextSize(this->Items[i].Label).Y;

			UIContextMenuItem& itm = this->Items[i];

			if (i != 0) p.Box(1.0f, currentY, this->W - 2.0f, 1.0f, this->SplitLineColor);
			p.Text(itm.Label, 2.0f, currentY + ts.Y / 4, itm.TextColor);

			currentY += ts.Y;
		}
	}

	void UIContextMenu::Update() {
		if (!this->ShouldRender) return;
		if (this->HasFocus()) return;

		this->Kill();
	}

	void UIContextMenu::AddItem(std::string name, std::function<void()> callback, Color textcol) {
		this->Items.Add({name, textcol, callback});
		Vector2 tsize = this->UIMan->Drawer.GetTextSize(name);

		int tw = (int)tsize.X + 4;
		if (tw < this->W) tw = this->W;

		float th = 0;
		for (int i = 0; i < this->Items.Count; i++) {
			th += this->DefaultUImanager->Drawer.GetTextSize(this->Items[i].Label).Y;
		}

		this->SetSize(tw, (int)th);
		this->MarkForFullRedraw();
	}

	void UIContextMenu::InvalidateLayout() {
	}
}
