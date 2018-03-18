#pragma once
#ifndef _TOMATOLIB_VECTOR4_H_
#define _TOMATOLIB_VECTOR4_H_

namespace TomatoLib {
	struct Vector4 {
		float X, Y, Z, W;

		void operator /= (float denom);
	};
}

#endif