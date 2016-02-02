#include "Color.h"

#include "../Utilities/Utilities.h"

namespace TomatoLib {
	const Color Color::Black = Color(0, 0, 0);
	const Color Color::White = Color(255, 255, 255);
	const Color Color::Red = Color(255, 0, 0);
	const Color Color::Green = Color(0, 255, 0);
	const Color Color::Blue = Color(0, 0, 255);
	const Color Color::Orange = Color(255, 153, 0);
	const Color Color::Yellow = Color(255, 255, 0);
	const Color Color::Transparent = Color(0, 0, 0, 0);

	Color::Color() :R(0), G(0), B(0), A(255) {}
	Color::Color(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b), A(255) {}
	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : R(r), G(g), B(b), A(a) {}
	Color::Color(unsigned int hex) : R(((unsigned char*)&hex)[0]), G(((unsigned char*)&hex)[1]), B(((unsigned char*)&hex)[2]), A(((unsigned char*)&hex)[3]) {}

	std::string Color::ToString() const {
		return Utilities::GetNumberPadded(this->R, 1, '0') + "," + Utilities::GetNumberPadded(this->G, 1, '0') + "," + Utilities::GetNumberPadded(this->B, 1, '0') + "," + Utilities::GetNumberPadded(this->A, 1, '0');
	}

	Color Color::Lerp(const Color& o, float timestep) {
		int r = (int)(this->R + (o.R - this->R) * timestep);
		int g = (int)(this->G + (o.G - this->G) * timestep);
		int b = (int)(this->B + (o.B - this->B) * timestep);
		int a = (int)(this->A + (o.A - this->A) * timestep);

		return Color(r, g, b, a);
	}

	bool Color::operator== (const Color& o) const {
		return this->R == o.R && this->G == o.G && this->B == o.B && this->A == o.A;
	}

	bool Color::operator!= (const Color& other) const {
		return !operator==(other);
	}

	Color Color::operator* (const Color& other) const {
		return Color(this->R * other.R, this->G * other.G, this->B * other.B, this->A * other.A);
	}

	Color Color::operator+ (const Color& other) const {
		return Color(this->R + other.R, this->G + other.G, this->B + other.B, this->A + other.A);
	}

	Color Color::operator- (const Color& other) const {
		return Color(this->R - other.R, this->G - other.G, this->B - other.B, this->A - other.A);
	}

	Color Color::operator* (int mult) const {
		int r = this->R * mult;
		int g = this->G * mult;
		int b = this->B * mult;
		int a = this->A * mult;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;

		return Color(r, g, b, a);
	}

	Color Color::operator* (float mult) const {
		int r = (int)((float)this->R * mult);
		int g = (int)((float)this->G * mult);
		int b = (int)((float)this->B * mult);
		int a = (int)((float)this->A * mult);

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;

		return Color(r, g, b, a);
	}

	Color operator* (int mult, const Color& col) {
		int r = col.R * mult;
		int g = col.G * mult;
		int b = col.B * mult;
		int a = col.A * mult;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;

		return Color(r, g, b, a);
	}

	Color operator* (float mult, const Color& col) {
		int r = (int)((float)col.R * mult);
		int g = (int)((float)col.G * mult);
		int b = (int)((float)col.B * mult);
		int a = (int)((float)col.A * mult);

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;

		return Color(r, g, b, a);
	}
}