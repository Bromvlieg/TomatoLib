#include "Vector4.h"

namespace TomatoLib {
	Vector4::Vector4(Color col) {
		this->X = static_cast<float>(col.R) / 255;
		this->Y = static_cast<float>(col.G) / 255;
		this->Z = static_cast<float>(col.B) / 255;
		this->W = static_cast<float>(col.A) / 255;
	}

	void Vector4::operator/=(float denom) {
		this->X /= denom;
		this->Y /= denom;
		this->Z /= denom;
		this->W /= denom;
	}
}