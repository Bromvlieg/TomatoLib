#include "UIScrollPanel.h"
#include "UIManager.h"
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UIScrollPanel::UIScrollPanel(UIBase* parent, bool enablehorizontal) : UIBase(parent) {
		this->BorderColor = Color::Black;
		this->BackgroundColor = Color(100, 100, 100, 255);

		this->AlwaysShowBar = false;
		this->Back = 0;
		this->_LastX = 0;
		this->_LastY = 0;
		this->_GrabX = -1;
		this->_GrabY = -1;
		this->_ScrollX = 5;
		this->_ScrollY = 5;
		this->ScrollAmplifier = 1.0f;
		this->CanAcceptInput = true;
		this->_IsGrabbingX = false;
		this->_IsGrabbingY = false;
		this->AlwaysRedraw = false;


		this->UpX = nullptr;
		this->UpY = nullptr;
		this->DownX = nullptr;
		this->DownY = nullptr;
		this->GrabX = nullptr;
		this->GrabY = nullptr;

		this->CanCatchScroll = true;

		this->Back = new UIPanel(this);
		this->Back->SetPos(0, 0);
		this->Back->SetSize(this->W, this->H);

		if (enablehorizontal) {
			this->UpX = new UIButton(this);
			this->UpX->SetSize(20, 20);
			this->UpX->SetPos(0, this->H - this->UpX->H);
			this->UpX->SetText("-");

			this->UpX->OnClick = [this](int x, int y, int but) {
				this->Scroll(this->_ScrollX, 0);
			};

			this->DownX = new UIButton(this);
			this->DownX->SetSize(20, 10);
			this->DownX->SetPos(this->W - this->DownX->W, this->H - this->DownX->H);
			this->DownX->SetText("+");

			this->DownX->OnClick = [this](int x, int y, int but) {
				this->Scroll(this->_ScrollX * -1, 0);
			};

			this->GrabX = new UIButton(this);
			this->GrabX->SetSize(this->W - (this->UpX->W + this->DownX->W), 20);
			this->GrabX->SetPos(this->UpX->W, this->H - this->GrabX->H);
			this->GrabX->SetText("");

			this->GrabX->OnPress = [this](int x, int y, int but) {
				this->_LastX = this->GrabX->X - this->UpX->W;
				this->_GrabX = (int)this->UIMan->LastMousePosition.X;
				this->_IsGrabbingX = true;
			};

			this->GrabX->OnUpdate = [this]() {
				if (!this->GrabX->HasFocus()) return;

				if (this->_IsGrabbingX) {
					Vector2 mousepos = this->UIMan->LastMousePosition;

					if (mousepos.X < 0) mousepos.X = 0;

					int deltaX = this->_LastX + this->UpX->H + (int)mousepos.X - this->_GrabX;

					if (deltaX < this->UpX->W) deltaX = this->UpX->W;
					if (deltaX > this->W - this->GrabX->W - this->UpX->W) deltaX = this->W - this->GrabX->W - this->UpX->W;

					this->GrabX->X = deltaX;

					float mul = (float)(deltaX - this->UpX->W) / (float)(this->W - this->GrabX->W - this->UpX->W - this->DownX->W);

					if ((this->Back->W - this->W) > 0) {
						this->DeltaX((int)((float)(this->Back->W - this->W) * mul) * -1);
					} else {
						this->DeltaX(0);
					}
				}
			};

			this->GrabX->OnRelease = [this](int x, int y, int but) { this->_GrabX = -1; this->_IsGrabbingX = false; };
			this->GrabX->OnScroll = [this](int x, int y) { this->Scroll(x, 0); };
			this->DownX->OnScroll = [this](int x, int y) { this->Scroll(x, 0); };
			this->UpX->OnScroll = [this](int x, int y) { this->Scroll(x, 0); };
		}

		this->UpY = new UIButton(this);
		this->UpY->SetSize(20, 20);
		this->UpY->SetPos(this->W - this->UpY->W, 0);
		this->UpY->SetText("-");

		this->UpY->OnClick = [this](int x, int y, int but) {
			this->Scroll(this->_ScrollY);
		};

		this->DownY = new UIButton(this);
		this->DownY->SetSize(20, enablehorizontal ? 10 : 20);
		this->DownY->SetPos(this->W - this->DownY->W, this->H - 20);
		this->DownY->SetText("+");

		this->DownY->OnClick = [this](int x, int y, int but) {
			this->Scroll(this->_ScrollY * -1);
		};

		this->GrabY = new UIButton(this);
		this->GrabY->SetSize(20, this->H - (this->UpY->H + this->DownY->W));
		this->GrabY->SetPos(this->W - this->GrabY->W, this->UpY->H);
		this->GrabY->SetText("");

		this->GrabY->OnPress = [this](int x, int y, int but) {
			this->_LastY = this->GrabY->Y - this->UpY->H;
			this->_GrabY = (int)this->UIMan->LastMousePosition.Y;
			this->_IsGrabbingY = true;
		};

		this->GrabY->OnUpdate = [this]() {
			if (!this->GrabY->HasFocus()) return;

			if (this->_GrabY != -1) {
				Vector2 mousepos = this->UIMan->LastMousePosition;

				if (mousepos.X < 0) mousepos.X = 0;
				if (mousepos.Y < 0) mousepos.Y = 0;

				int deltaY = this->_LastY + this->UpY->H + (int)mousepos.Y - this->_GrabY;

				if (deltaY < this->UpY->H) deltaY = this->UpY->H;
				if (deltaY > this->H - this->GrabY->H - this->UpY->H) deltaY = this->H - this->GrabY->H - this->UpY->H;

				this->GrabY->Y = deltaY;

				float mul = (float)(deltaY - this->UpY->H) / (float)(this->H - this->GrabY->H - this->UpY->H - this->DownY->W);

				if ((this->Back->H - this->H) > 0) {
					this->DeltaY((int)((float)(this->Back->H - this->H) * mul) * -1);
				} else {
					this->DeltaY(0);
				}
			}
		};

		this->GrabY->OnRelease = [this](int x, int y, int but) { this->_GrabY = -1; this->_IsGrabbingY = false; };

		this->OnScroll = [this](int x, int y) { this->Scroll(y); };
		this->Back->OnScroll = [this](int x, int y) { this->Scroll(y); };

		this->GrabY->OnScroll = [this](int x, int y) { this->Scroll(y); };
		this->DownY->OnScroll = [this](int x, int y) { this->Scroll(y); };
		this->UpY->OnScroll = [this](int x, int y) { this->Scroll(y); };
	}

	void UIScrollPanel::DisableVerticalScrollball() {
		if (this->GrabX == nullptr) return;

		this->UpX->Kill();
		this->DownX->Kill();
		this->GrabX->Kill();

		this->UpX = nullptr;
		this->DownX = nullptr;
		this->GrabX = nullptr;
	}

	void UIScrollPanel::DisableHorizontalScrollball() {
		if (this->GrabY == nullptr) return;

		this->UpY->Kill();
		this->DownY->Kill();
		this->GrabY->Kill();

		this->UpY = nullptr;
		this->DownY = nullptr;
		this->GrabY = nullptr;
	}

	void UIScrollPanel::Scroll(int scrolly) {
		this->DeltaY(this->Back->Y + (int)(scrolly * this->ScrollAmplifier));
	}

	void UIScrollPanel::Scroll(int scrollx, int scrolly) {
		this->Scroll(scrolly);
		if (this->GrabX == nullptr) return;

		this->DeltaX(this->Back->X + (int)(scrollx * this->ScrollAmplifier));
	}

	void UIScrollPanel::DeltaX(int delta) {
		if (this->GrabX == nullptr) return;

		if (delta < (this->W - this->Back->W)) delta = (this->W - this->Back->W);
		if (delta > 0) delta = 0;
		this->Back->X = delta;
		
		this->GrabX->X = this->UpX->W + (int)((this->W - this->UpX->W - this->DownX->W - this->GrabX->W) * (float)this->Back->X / (float)(this->W - this->Back->W));
		this->MarkForFullRedraw();
	}

	void UIScrollPanel::DeltaY(int delta) {
		if (this->GrabY == nullptr) return;

		if (delta < (this->H - this->Back->H)) delta = (this->H - this->Back->H);
		if (delta > 0) delta = 0;
		this->Back->Y = delta;

		this->GrabY->Y = this->UpY->H + (int)((this->H - this->UpY->H - this->DownY->W - this->GrabY->H) * (float)this->Back->Y / (float)(this->H - this->Back->H));
		this->MarkForFullRedraw();
	}

	void UIScrollPanel::Draw(Render& p) {
		if (this->GrabY != nullptr && this->GrabY->ShouldRender) p.Box(this->GrabY->X, this->UpY->Y + this->UpY->H, this->GrabY->W, this->DownY->Y - this->UpY->Y - this->UpY->H, this->BorderColor);
		if (this->GrabX != nullptr && this->GrabX->ShouldRender) p.Box(this->UpX->W, this->GrabX->Y, this->DownX->X - this->UpX->X - this->UpX->W, this->UpX->H, this->BorderColor);
	}

	void UIScrollPanel::Update() {
		this->Back->BorderColor = this->BorderColor;
		this->Back->BackgroundColor = this->BackgroundColor;
	}

	void UIScrollPanel::AddItem(UIBase* item) {
		item->ShouldRender = true;
		item->SetParent(this->Back);
		this->InvalidateLayout();
		this->MarkForFullRedraw();
	}

	void UIScrollPanel::RemoveItem(UIBase* item) {
		this->Back->RemoveChild(item);
		this->InvalidateLayout();
		this->MarkForFullRedraw();
	}

	void UIScrollPanel::InvalidateLayout() {
		int maxx = this->W;
		int maxy = this->H;
		int subx = 0;
		int suby = 0;

		if (this->GrabX != nullptr) {
			for (unsigned int i = 0; i < this->Back->Children.size(); i++) {
				if ((this->Back->Children[i]->X + this->Back->Children[i]->W) > maxx) maxx = (this->Back->Children[i]->X + this->Back->Children[i]->W) + 1;
			}

			float mulx = ((float)(maxx) / (float)this->W);

			this->UpX->SetPos(0, this->H - this->UpX->H);
			this->DownX->SetPos(this->W - this->DownX->W, this->H - this->DownX->H);

			int newgrabhx = (int)((this->W - (this->UpX->W + this->DownX->W)) / mulx);
			if (newgrabhx < this->GrabX->W / 2) newgrabhx = this->GrabX->W / 2;

			this->GrabX->SetSize(newgrabhx, 20);
			this->GrabX->SetPos(this->UpX->W, this->H - this->GrabX->H);

			this->DownX->SetSize(20, 10);
			this->DownY->SetSize(20, 10);

			if (mulx > 1.0f || this->AlwaysShowBar) {
				this->UpX->ShouldRender = true;
				this->DownX->ShouldRender = true;
				this->DownY->SetSize(20, 10);
				this->GrabX->ShouldRender = true;
				subx = this->GrabX->H;
			} else {
				if (this->AlwaysShowBar) {
					this->GrabX->SetSize((int)((this->W - (this->UpX->W + this->DownX->W))), 20);
					this->GrabX->SetPos(this->UpX->W, this->H - this->GrabX->H);
					this->Back->Y = 0;

					this->UpX->ShouldRender = true;
					this->DownX->ShouldRender = true;
					this->GrabX->ShouldRender = true;
					this->DownY->SetSize(20, 10);
					subx = this->GrabX->H;
				} else {
					this->UpX->ShouldRender = false;
					this->DownX->ShouldRender = false;
					this->GrabX->ShouldRender = false;
					this->DownY->SetSize(20, 20);
				}
			}
		}

		if (this->GrabY != nullptr) {
			for (unsigned int i = 0; i < this->Back->Children.size(); i++) {
				if ((this->Back->Children[i]->Y + this->Back->Children[i]->H) > maxy) maxy = (this->Back->Children[i]->Y + this->Back->Children[i]->H) + 1;
			}

			float muly = ((float)(maxy) / (float)this->H);

			this->UpY->SetPos(this->W - this->UpY->W, 0);
			this->DownY->SetPos(this->W - this->DownY->W, this->H - this->DownY->W);

			int newgrabhy = (int)((this->H - (this->UpY->H + this->DownY->W)) / muly);
			if (newgrabhy < this->GrabY->H / 2) newgrabhy = this->GrabY->H / 2;

			this->GrabY->SetSize(20, newgrabhy);
			this->GrabY->SetPos(this->W - this->GrabY->W, this->UpY->H);

			if (muly > 1.0f || this->AlwaysShowBar) {
				this->UpY->ShouldRender = true;
				this->DownY->ShouldRender = true;
				this->GrabY->ShouldRender = true;
				suby = this->GrabY->W;
			} else {
				if (this->AlwaysShowBar) {
					this->GrabY->SetSize(20, (int)((this->H - (this->UpY->H + this->DownY->W))));
					this->GrabY->SetPos(this->W - this->GrabY->W, this->UpY->H);
					this->Back->Y = 0;

					this->UpY->ShouldRender = true;
					this->DownY->ShouldRender = true;
					this->GrabY->ShouldRender = true;
					suby = this->GrabY->W;
				} else {
					this->UpY->ShouldRender = false;
					this->DownY->ShouldRender = false;
					this->GrabY->ShouldRender = false;
				}
			}
		}

		this->Back->SetSize(maxx + suby, maxy + subx);
	}
}
