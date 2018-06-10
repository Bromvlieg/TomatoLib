#pragma once
#ifndef _TOMATOLIB_VECTOR4_H_
#define _TOMATOLIB_VECTOR4_H_

#include "../Graphics/Color.h"

namespace TomatoLib {
	struct Vector4 {
		float X, Y, Z, W;

		Vector4() = default;
		Vector4(float x, float y = 0, float z = 0, float w = 0) : X(x), Y(y), Z(z), W(w) {}
		Vector4(Color col);

		void operator /= (float denom);
	};
}

#endif