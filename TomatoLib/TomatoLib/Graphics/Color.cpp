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

	bool Color::operator== (const Color& o) const {
		return this->R == o.R && this->G == o.G && this->B == o.B && this->A == o.A;
	}

	bool Color::operator!= (const Color& other) const {
		return !operator==(other);
	}
}