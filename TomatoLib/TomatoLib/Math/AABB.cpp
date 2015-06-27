#include "AABB.h"
#include <math.h>

namespace TomatoLib {
	AABB::AABB() : X(0), Y(0), W(0), H(0) {}
	AABB::AABB(int x, int y, int width, int height) : X((float)x), Y((float)y), W((float)width), H((float)height) {}
	AABB::AABB(float x, float y, float width, float height) : X(x), Y(y), W(width), H(height) {}
	AABB::AABB(const Vector2& pos, const Vector2& size) : X(pos.X), Y(pos.Y), W(size.X), H(size.Y) {}
	AABB::AABB(const AABB& copy) : X(copy.X), Y(copy.Y), W(copy.W), H(copy.H) {}

	inline float min(float a, float b) { return a < b ? a : b; }
	inline float max(float a, float b) { return a < b ? b : a; }

	Vector2 AABB::GetMin() const {
		return Vector2(min(X, X + W), min(Y, Y + H));
	}
	Vector2 AABB::GetMax() const {
		return Vector2(max(X, X + W), max(Y, Y + H));
	}

	bool AABB::Intersects(const AABB& other) const {
		return  this->X + this->W > other.X &&
			this->X < other.X + other.W &&
			this->Y + this->H > other.Y &&
			this->Y < other.Y + other.H;
	}

	AABB AABB::Mask(const AABB& other) const {
		AABB newclip(*this);

		if (newclip.X + newclip.W < other.X) newclip = AABB(other.X, other.Y, 0.0f, 0.0f);
		if (newclip.Y + newclip.H < other.Y) newclip = AABB(other.X, other.Y, 0.0f, 0.0f);
		if (newclip.X > other.X + other.W) newclip = AABB(other.X, other.Y, 0.0f, 0.0f);
		if (newclip.Y > other.Y + other.H) newclip = AABB(other.X, other.Y, 0.0f, 0.0f);

		if (newclip.X < other.X) { newclip.W -= other.X - abs(newclip.X); newclip.X = other.X; }
		if (newclip.Y < other.Y) { newclip.H -= other.Y - abs(newclip.Y); newclip.Y = other.Y; }

		if (newclip.X + newclip.W > other.X + other.W) newclip.W = other.X + other.W - newclip.X;
		if (newclip.Y + newclip.H > other.Y + other.H) newclip.H = other.Y + other.H - newclip.Y;

		return newclip;
	}
}
