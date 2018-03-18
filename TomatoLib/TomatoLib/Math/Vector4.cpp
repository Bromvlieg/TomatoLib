#include "Vector4.h"

void TomatoLib::Vector4::operator/=(float denom) {
	this->X /= denom;
	this->Y /= denom;
	this->Z /= denom;
	this->W /= denom;
}
