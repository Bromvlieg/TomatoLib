#include "UIColorPicker.h"
#include "UIManager.h"
#include "../Utilities/Utilities.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <GLFW/glfw3.h>

namespace TomatoLib {
	UIColorPicker::UIColorPicker(UIBase* parent) : UIBase(parent) {
		this->BorderColor = Color::Black;
		this->MinValue = Color(0, 0, 0, 0);
		this->MaxValue = Color(255, 255, 255, 255);
		this->Value = Color(0, 0, 0, 255);

		this->Padding = 5;
		this->CirclePoints = 32;
		this->BorderSize = 10;
		this->CanAcceptInput = true;
		this->CanChangeAlpha = false;

		this->pm_bIsBragging = false;

		this->AlwaysRedraw = false;

		this->OnPress = [this](int x, int y, int but) {
			if (but == GLFW_MOUSE_BUTTON_LEFT) {
				this->pm_bIsBragging = true;
			}

			if (but == GLFW_MOUSE_BUTTON_RIGHT) {
				this->pm_bIsBraggingAlpha = true;
			}

			this->MarkForFullRedraw();
		};

		this->OnRelease = [this](int x, int y, int but) {
			if (but == GLFW_MOUSE_BUTTON_LEFT) {
				this->pm_bIsBragging = false;
			}

			if (but == GLFW_MOUSE_BUTTON_RIGHT) {
				this->pm_bIsBraggingAlpha = false;
			}

			this->MarkForFullRedraw();
		};
	}

	Color HSVtoRGB(float h, float s, float v) {
		float r = 0;
		float g = 0;
		float b = 0;

		int i;
		float f, p, q, t;
		if (s == 0) {
			// achromatic (grey)
			return Color((unsigned char)v * 255, (unsigned char)v * 255, (unsigned char)v * 255);
		}

		h /= 60;            // sector 0 to 5
		i = (int)floor(h);
		f = h - i;          // factorial part of h
		p = v * (1 - s);
		q = v * (1 - s * f);
		t = v * (1 - s * (1 - f));

		switch (i) {
			case 0:
				r = v;
				g = t;
				b = p;
				break;
			case 1:
				r = q;
				g = v;
				b = p;
				break;
			case 2:
				r = p;
				g = v;
				b = t;
				break;
			case 3:
				r = p;
				g = q;
				b = v;
				break;
			case 4:
				r = t;
				g = p;
				b = v;
				break;
			default:        // case 5:
				r = v;
				g = p;
				b = q;
				break;
		}

		return Color((unsigned char)r * 255, (unsigned char)g * 255, (unsigned char)b * 255);
	}

	void UIColorPicker::Draw(Render& p) {
		float w = (float)this->W / 2;
		float h = (float)this->H / 2;

		float x = w;
		float y = h;

		float space = (float)M_PI * 2 / this->CirclePoints;

		p.CheckSpace(this->CirclePoints * 6, this->CirclePoints * 6);

		for (int i = 0; i < this->CirclePoints;) {
			Vector2 a(x + (sin(space * i) * (w - this->BorderSize)), y + (cos(space * i) * (h - this->BorderSize)));
			Vector2 b(x + sin(space * i) * w, y + cos(space * i) * h);

			float hue = space * i * (float)(180.0 / M_PI);

			i++;
			Vector2 c(x + (sin(space * i) * (w - this->BorderSize)), y + (cos(space * i) * (h - this->BorderSize)));
			Vector2 d(x + sin(space * i) * w, y + cos(space * i) * h);

			float hue2 = space * i * (float)(180.0 / M_PI);

			Color ca = HSVtoRGB(hue, 1, 1);
			Color cb = HSVtoRGB(hue, 1, 1);
			Color cc = HSVtoRGB(hue2, 1, 1);
			Color cd = HSVtoRGB(hue2, 1, 1);

			p.Triangle(a, b, c, ca, cb, cc);
			p.Triangle(b, d, c, cb, cd, cc);
		}

		Vector2 a(this->BorderSize * 3 + this->Padding, this->BorderSize * 3 + this->Padding);
		Vector2 b(this->W - this->BorderSize * 3 - +this->Padding, this->BorderSize * 3 + +this->Padding);
		Vector2 c(this->BorderSize * 3 + +this->Padding, this->H - this->BorderSize * 3 - +this->Padding);
		Vector2 d(this->W - this->BorderSize * 3 - +this->Padding, this->H - this->BorderSize * 3 - +this->Padding);

		Color ca = HSVtoRGB(this->pm_fHue, 0, 1);
		Color cb = HSVtoRGB(this->pm_fHue, 1, 1);
		Color cc = HSVtoRGB(this->pm_fHue, 0, 0);
		Color cd = HSVtoRGB(this->pm_fHue, 1, 0);

		p.Triangle(a, b, c, ca, cb, cc);
		p.Triangle(b, d, c, cb, cd, cc);
	}

	void UIColorPicker::Update() {
		if (!this->HasFocus() || (!this->pm_bIsBragging && !this->pm_bIsBraggingAlpha)) return;

		Vector2 mp = this->UIMan->LastMousePosition - this->GetAbsoluteLocation();

		float mdist = mp.Distance(this->pm_vCenter);
		if (mdist >= this->W / 2 - BorderSize && mdist <= this->W / 2) {
			this->pm_fHue = (float)((int)(-atan2(mp.X - (float)this->W / 2, (float)this->H / 2 - mp.Y) * (float)(180.0 / M_PI) + 180) % 360);
		} else {
			if (mp.X >= this->BorderSize * 3 + +this->Padding && mp.Y >= this->BorderSize * 3 + this->Padding && mp.X <= this->W - this->BorderSize * 3 - this->Padding  && mp.Y <= this->H - this->BorderSize * 3 - this->Padding) {
				Vector2 tmp = mp;
				tmp -= (float)this->BorderSize * 3 + this->Padding;

				this->pm_vsat.X = Utilities::Clamp(tmp.X / (float)(this->W - this->BorderSize * 6 - this->Padding * 2), 0.0f, 1.0f);
				this->pm_vsat.Y = Utilities::Clamp(1 - (tmp.Y / (float)(this->H - this->BorderSize * 6 - this->Padding * 2)), 0.0f, 1.0f);
			}
		}

		Color oldval = this->Value;
		this->Value = HSVtoRGB(this->pm_fHue, this->pm_vsat.X, this->pm_vsat.Y);

		if (this->Value != oldval) {
			if (this->OnValueChanged != nullptr) this->OnValueChanged();
			this->MarkForFullRedraw();
		}
	}

	void UIColorPicker::InvalidateLayout() {
		float angpp = (float)(M_PI * 2.0 / 3.0);
		float wh = (float)this->W / 2.0f;
		float hh = (float)this->H / 2.0f;

		this->pm_vPointR = Vector2(sin(angpp) * wh / 2 + wh, cos(angpp) * hh + hh);
		this->pm_vPointG = Vector2(sin(angpp * 2) * wh / 2 + wh, cos(angpp * 2) * hh + hh);
		this->pm_vPointB = Vector2(sin(angpp * 3) * wh / 2 + wh, cos(angpp * 3) * hh + hh);

		this->pm_vCenter = Vector2(wh, hh);
	}

}
