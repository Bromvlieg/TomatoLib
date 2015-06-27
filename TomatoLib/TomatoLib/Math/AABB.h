#ifndef __AABB_H__
#define __AABB_H__

#include "Vector2.h"

namespace TomatoLib {
	class AABB {
	public:
		float X, Y, W, H;
		AABB();
		AABB(int x, int y, int width, int height);
		AABB(float x, float y, float width, float height);
		AABB(const Vector2& pos, const Vector2& size);
		AABB(const AABB& copy);

		Vector2 GetMin() const;
		Vector2 GetMax() const;

		bool Intersects(const AABB& other) const;
		AABB Mask(const AABB& other) const;
	};
}
#endif