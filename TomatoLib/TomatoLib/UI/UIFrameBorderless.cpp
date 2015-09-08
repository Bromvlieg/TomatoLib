#include "UIFrameBorderless.h"
#include "UIManager.h"
#include <GLFW/glfw3.h>
#include "../Game/Window.h"

namespace TomatoLib {
	UIFrameBorderless::UIFrameBorderless(UIBase* parent) : UIBase(parent) {
		this->AlwaysRedraw = false;

		this->BackColor = Color(60, 60, 60);
		this->BorderColor = Color::Black;

		this->ResizeBordersize = 0;
		this->CloseButton = nullptr;
		this->MinButton = nullptr;
		this->MaxButton = nullptr;
		this->_ResizeMode = 0;
		this->MovingEnabled = false;

		this->_GrabX = -1;
		this->_GrabY = -1;

		int& ox = this->_OldX;
		int& oy = this->_OldY;
		int& ow = this->_OldW;
		int& oh = this->_OldH;
		int& gx = this->_GrabX;
		int& gy = this->_GrabY;
		int& rm = this->_ResizeMode;

		this->OnPress = [this, &gx, &gy, &ow, &oh, &ox, &oy, &rm](int x, int y, int button) {
			if (button != GLFW_MOUSE_BUTTON_LEFT) return;

			gx = x;
			gy = y;
			ox = this->X;
			oy = this->Y;
			ow = this->W;
			oh = this->H;
			rm = 0;


			int bs = this->GetResizeBorderSize();
			if (bs != 0) {
				if (x < bs) rm |= 1;
				if (x > this->W - bs) rm |= 2;

				if (y < bs) rm |= 4;
				if (y > this->H - bs) rm |= 8;
			}

			if (this->MovingEnabled && rm == 0) rm = 16;
		};

		this->OnRelease = [this, &gx, &gy, &rm](int x, int y, int button) {
			rm = 0;
		};
	}

	void UIFrameBorderless::Draw(Render& p) {
		p.Box(0, 0, this->W, this->H, this->BorderColor);
		p.Box(this->ResizeBordersize, this->ResizeBordersize, this->W - this->ResizeBordersize * 2, this->H - this->ResizeBordersize * 2, this->BackColor);
	}

	void UIFrameBorderless::Update() {
		int dothethingwiththeupdate = 0;

		Vector2 apos = this->GetAbsoluteLocation();
		int x = (int)this->UIMan->LastMousePosition.X;
		int y = (int)this->UIMan->LastMousePosition.Y;

		int bs = this->GetResizeBorderSize();
		if (bs > 0) {
			if (x - apos.X < bs) this->UIMan->CursorShape = GLFW_HRESIZE_CURSOR;
			if (x - apos.X > this->W - bs) this->UIMan->CursorShape = GLFW_HRESIZE_CURSOR;

			if (this->UIMan->CursorShape == GLFW_HRESIZE_CURSOR) {
				if (y - apos.Y < bs) this->UIMan->CursorShape = GLFW_HAND_CURSOR;
				if (y - apos.Y > this->H - bs) this->UIMan->CursorShape = GLFW_HAND_CURSOR;
			} else {
				if (y - apos.Y < bs) this->UIMan->CursorShape = GLFW_VRESIZE_CURSOR;
				if (y - apos.Y > this->H - bs) this->UIMan->CursorShape = GLFW_VRESIZE_CURSOR;
			}
		}

		if (this->_ResizeMode == 0) return;
		if ((this->_ResizeMode & 1) > 0) {
			int oldx = this->X;
			this->X = x - this->_GrabX;
			if (oldx != this->X) dothethingwiththeupdate++;
			this->UIMan->CursorShape = GLFW_HRESIZE_CURSOR;
		}

		if ((this->_ResizeMode & 2) > 0) {
			int oldw = this->W;
			this->W = x + (this->_OldW - this->_GrabX) - (int)apos.X;
			if (oldw != this->W) dothethingwiththeupdate++;
			this->UIMan->CursorShape = GLFW_HRESIZE_CURSOR;
		}

		if ((this->_ResizeMode & 4) > 0) {
			int oldy = this->Y;
			this->Y = y - this->_GrabY;
			if (oldy != this->Y) dothethingwiththeupdate++;
			this->UIMan->CursorShape = GLFW_VRESIZE_CURSOR;
		}

		if ((this->_ResizeMode & 8) > 0) {
			int oldh = this->H;
			this->H = y + (this->_OldH - this->_GrabY) - (int)apos.Y;
			if (oldh != this->H) dothethingwiththeupdate++;
			this->UIMan->CursorShape = GLFW_VRESIZE_CURSOR;
		}

		if ((this->_ResizeMode & 16) > 0) {
			int oldx = this->X;
			int oldy = this->Y;
			this->X = x - this->_GrabX;
			this->Y = y - this->_GrabY;
			this->UIMan->CursorShape = GLFW_HAND_CURSOR;
			if (oldx != this->X || oldy != this->Y) dothethingwiththeupdate++;
		}

		if (dothethingwiththeupdate > 1) {
			this->UIMan->CursorShape = GLFW_HAND_CURSOR;
		}

		if (dothethingwiththeupdate) {
			this->MarkForFullRedraw();

			this->InvalidateLayout();
			this->OnInvalidateLayout();
		}
	}

	void UIFrameBorderless::EnableControls() {
		this->CloseButton = new UIButton(this);
		this->CloseButton->SetSize(22, 22);
		this->CloseButton->SetText("X");
		this->CloseButton->BorderColor = Color::Black;
		this->CloseButton->BackColor = Color(255 / 2, 0, 0);
		this->CloseButton->TextColor = Color::Black;
		this->CloseButton->BorderSize = 1;
		this->CloseButton->OnClick = [this](int x, int y, int button) {
			this->Kill();
		};

		this->MinButton = new UIButton(this);
		this->MinButton->SetSize(22, 22);
		this->MinButton->SetText("_");
		this->MinButton->BorderColor = Color::Black;
		this->MinButton->BackColor = Color(255, 153, 0);
		this->MinButton->BorderSize = 1;
		this->MinButton->OnClick = [this](int x, int y, int button) {
			this->SetSize(0, 0);
		};

		this->MaxButton = new UIButton(this);
		this->MaxButton->SetSize(22, 22);
		this->MaxButton->SetText("\r"); // \r is square in font generator, :D
		this->MaxButton->BorderColor = Color::Black;
		this->MaxButton->BackColor = Color(255, 153, 0);
		this->MaxButton->BorderSize = 1;
		this->MaxButton->OnClick = [this](int x, int y, int button) {
			if (this->OldSize.X > 0) {
				this->SetPos((int)this->OldPos.X, (int)this->OldPos.Y);
				this->SetSize((int)this->OldSize.X, (int)this->OldSize.Y);

				this->OldSize.X = 0;
			} else {
				this->OldPos.X = (float)this->X;
				this->OldPos.Y = (float)this->Y;

				this->OldSize.X = (float)this->W;
				this->OldSize.Y = (float)this->H;

				Vector2 wsize = Window::CurrentWindow->GetSize();
				this->SetPos(0, 0);
				this->SetSize((int)wsize.X, (int)wsize.Y);
			}
		};

		this->InvalidateLayout();
		this->OnInvalidateLayout();
	}

	void UIFrameBorderless::EnableMoving() {
		this->MovingEnabled = true;
	}

	void UIFrameBorderless::SetResizeBorderSize(int size) {
		this->ResizeBordersize = size;
		this->ShouldRedraw = true;

		this->InvalidateLayout();
		this->OnInvalidateLayout();
	}

	void UIFrameBorderless::InvalidateLayout() {
		if (this->CloseButton == nullptr) return;

		this->CloseButton->SetPos(this->W - 21 - this->ResizeBordersize, this->ResizeBordersize - 1);
		this->MaxButton->SetPos(this->W - 43 - this->ResizeBordersize, this->ResizeBordersize - 1);
		this->MinButton->SetPos(this->W - 65 - this->ResizeBordersize, this->ResizeBordersize - 1);
	}

}
