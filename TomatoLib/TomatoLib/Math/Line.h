#pragma once

#include "Vector2.h"

namespace TomatoLib {

	class Line {
	public:
		Vector2 A;
		Vector2 B;

		bool intersects(const Line& other);
	};
}