#include "Rectangle.h"
#include "Matrix.h"
#include "Line.h"
#include <math.h>
#include <algorithm>

namespace TomatoLib {
	Rectangle::Rectangle() {
		x = y = w = h = 0;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(Vector2 vPos, Vector2 vSize) {
		x = vPos.X; y = vPos.Y;
		w = vSize.X; h = vSize.Y;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(Vector2 vPos, Vector2 vSize, float fRot) {
		x = vPos.X; y = vPos.Y;
		w = vSize.X; h = vSize.Y;
		rotation = fRot;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(float fX, float fY, float fW, float fH) {
		x = fX; y = fY;
		w = fW; h = fH;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(float fX, float fY, float fW, float fH, float fRot) {
		x = fX; y = fY;
		w = fW; h = fH;
		rotation = fRot;
		anchorInCenter = false;
	}

	Rectangle::~Rectangle() {
	}

	void Rectangle::Translate(const Vector2 &v) {
		x += v.X;
		y += v.Y;
	}

	void Rectangle::Rotate(float fDegrees) {
		rotation += fDegrees;
	}

	Vector2 Rectangle::GetPos() const {
		return Vector2(x, y);
	}

	Vector2 Rectangle::GetSize() const {
		return Vector2(w, h);
	}

	void Rectangle::GetRotatedPosArray(Vector2* pArray) const {
		Matrix mat = Matrix::CreateRotationZ(rotation * 3.14159265f / 180);

		Vector2 &topLeft = pArray[0];
		Vector2 &topRight = pArray[1];
		Vector2 &botRight = pArray[2];
		Vector2 &botLeft = pArray[3];

		Vector2 pos = GetPos();

		if (anchorInCenter) {
			topLeft = pos + mat.Translate(Vector2(-w / 2.0f, -h / 2.0f));
			topRight = pos + mat.Translate(Vector2(w / 2.0f, -h / 2.0f));
			botRight = pos + mat.Translate(Vector2(w / 2.0f, h / 2.0f));
			botLeft = pos + mat.Translate(Vector2(-w / 2.0f, h / 2.0f));
		} else {
			topLeft = pos;
			topRight = pos + mat.Translate(Vector2(w, 0.0f));
			botRight = pos + mat.Translate(Vector2(w, h));
			botLeft = pos + mat.Translate(Vector2(0.0f, h));
		}
	}

	bool Rectangle::Contains(const Vector2 &point) {
		Vector2 verts[4];
		GetRotatedPosArray(verts);
		return PointInPolygon(point, {std::begin(verts), std::end(verts)});
	}

	bool Rectangle::Intersects(const Rectangle &other) {
		Vector2 points[4]; GetRotatedPosArray(points);
		Vector2 otherPoints[4]; other.GetRotatedPosArray(otherPoints);

		if (PointInPolygon(points[0], {std::begin(otherPoints), std::end(otherPoints)})) { return true; }
		if (PointInPolygon(points[1], {std::begin(otherPoints), std::end(otherPoints)})) { return true; }
		if (PointInPolygon(points[2], {std::begin(otherPoints), std::end(otherPoints)})) { return true; }
		if (PointInPolygon(points[3], {std::begin(otherPoints), std::end(otherPoints)})) { return true; }

		if (PointInPolygon(otherPoints[0], {std::begin(points), std::end(points)})) { return true; }
		if (PointInPolygon(otherPoints[1], {std::begin(points), std::end(points)})) { return true; }
		if (PointInPolygon(otherPoints[2], {std::begin(points), std::end(points)})) { return true; }
		if (PointInPolygon(otherPoints[3], {std::begin(points), std::end(points)})) { return true; }

		/**
		 * NOTE: This should be fixed later, due to a special case where two rectangles of the same size are on the same
		 *       centered position, one of which is rotated at 45 degrees. Right now, this doesn't seem to work as it
		 *       should, so we need to figure out the bug in lineSegmentsIntersect() or re-implement the function entirely.
		 */
		 /*
		 if (mthLineSegmentsIntersect(points[0], points[1], otherPoints[0], otherPoints[1]) ||
				 mthLineSegmentsIntersect(points[1], points[2], otherPoints[0], otherPoints[1]) ||
				 mthLineSegmentsIntersect(points[2], points[3], otherPoints[0], otherPoints[1]) ||
				 mthLineSegmentsIntersect(points[3], points[0], otherPoints[0], otherPoints[1]) ||
				 mthLineSegmentsIntersect(points[0], points[1], otherPoints[1], otherPoints[2]) ||
				 mthLineSegmentsIntersect(points[1], points[2], otherPoints[1], otherPoints[2]) ||
				 mthLineSegmentsIntersect(points[2], points[3], otherPoints[1], otherPoints[2]) ||
				 mthLineSegmentsIntersect(points[3], points[0], otherPoints[1], otherPoints[2]) ||
				 mthLineSegmentsIntersect(points[0], points[1], otherPoints[2], otherPoints[3]) ||
				 mthLineSegmentsIntersect(points[1], points[2], otherPoints[2], otherPoints[3]) ||
				 mthLineSegmentsIntersect(points[2], points[3], otherPoints[2], otherPoints[3]) ||
				 mthLineSegmentsIntersect(points[3], points[0], otherPoints[2], otherPoints[3]) ||
				 mthLineSegmentsIntersect(points[0], points[1], otherPoints[3], otherPoints[0]) ||
				 mthLineSegmentsIntersect(points[1], points[2], otherPoints[3], otherPoints[0]) ||
				 mthLineSegmentsIntersect(points[2], points[3], otherPoints[3], otherPoints[0]) ||
				 mthLineSegmentsIntersect(points[3], points[0], otherPoints[3], otherPoints[0])
		 ) {
			 return true;
		 }
		 */

		return false;
	}

	bool pointInPolygonOnSegment(const Vector2 &p, const Vector2 &q, const Vector2 &r) {
		return q.X <= std::max(p.X, r.X) && q.X >= std::min(p.X, r.X) &&
			q.Y <= std::max(p.Y, r.Y) && q.Y >= std::min(p.Y, r.Y);
	}

	int pointInPolygonOrientation(const Vector2 &p, const Vector2 &q, const Vector2 &r) {
		float val = (q.Y - p.Y) * (r.X - q.X) - (q.X - p.X) * (r.Y - q.Y);
		if (val == 0) {
			return 0;
		} else if (val > 0) {
			return 1;
		}
		return 2;
	}

	bool pointInPolygonDoIntersect(const Vector2 &p1, const Vector2 &q1, const Vector2 &p2, const Vector2 &q2) {
		int o1 = pointInPolygonOrientation(p1, q1, p2);
		int o2 = pointInPolygonOrientation(p1, q1, q2);
		int o3 = pointInPolygonOrientation(p2, q2, p1);
		int o4 = pointInPolygonOrientation(p2, q2, q1);
		return
			(o1 != o2 && o3 != o4) ||
			(o1 == 0 && pointInPolygonOnSegment(p1, p2, q1)) ||
			(o2 == 0 && pointInPolygonOnSegment(p1, q2, q1)) ||
			(o3 == 0 && pointInPolygonOnSegment(p2, p1, q2)) ||
			(o4 == 0 && pointInPolygonOnSegment(p2, q1, q2));
	}

	/**
	 * I'm sorry, for any complains you can go to brom, he'll tell you to stick it up where the light does not shine
	 */
	bool Rectangle::PointInPolygon(const Vector2 &p, const std::vector<Vector2>& points) {
		if (points.size() < 3) {
			return false;
		}

		size_t i, j, c = 0;

		for (i = 0, j = points.size() - 1; i < points.size(); j = i++) {
			if (((points[i].Y > p.Y) != (points[j].Y > p.Y)) && (p.X < (points[j].X - points[i].X) * (p.Y - points[i].Y) / (points[j].Y - points[i].Y) + points[i].X)) {
				c = !c;
			}
		}

		return c != 0;
	}
}
