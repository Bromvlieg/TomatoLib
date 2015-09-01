#include "Rectangle.h"
#include "Defines.h"
#include "Matrix.h"
#include <math.h>

namespace TomatoLib {
	Rectangle::Rectangle() {
		x = y = w = h = 0;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(Vector2 vPos, Vector2 vSize)
	{
		x = vPos.X; y = vPos.Y;
		w = vSize.X; h = vSize.Y;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(Vector2 vPos, Vector2 vSize, float fRot)
	{
		x = vPos.X; y = vPos.Y;
		w = vSize.X; h = vSize.Y;
		rotation = fRot;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(float fX, float fY, float fW, float fH)
	{
		x = fX; y = fY;
		w = fW; h = fH;
		rotation = 0;
		anchorInCenter = false;
	}

	Rectangle::Rectangle(float fX, float fY, float fW, float fH, float fRot)
	{
		x = fX; y = fY;
		w = fW; h = fH;
		rotation = fRot;
		anchorInCenter = false;
	}

	Rectangle::~Rectangle()
	{
	}

	void Rectangle::Translate(const Vector2 &v)
	{
		x += v.X;
		y += v.Y;
	}

	void Rectangle::Rotate(float fDegrees)
	{
		rotation += fDegrees;
	}

	Vector2 Rectangle::GetPos() const
	{
		return Vector2(x, y);
	}

	Vector2 Rectangle::GetSize() const
	{
		return Vector2(w, h);
	}

	void Rectangle::GetRotatedPosArray(Vector2* pArray) const
	{
		TL_ASSERT(pArray != NULL);

		Matrix mat = Matrix::CreateRotationZ(mthToRadians(rotation));

		Vector2 &topLeft = pArray[0];
		Vector2 &topRight = pArray[1];
		Vector2 &botRight = pArray[2];
		Vector2 &botLeft = pArray[3];

		Vector2 pos = GetPos();

		if(anchorInCenter) {
			topLeft = pos + mat.Translate(Vector2(-w/2.0f, -h/2.0f));
			topRight = pos + mat.Translate(Vector2(w/2.0f, -h/2.0f));
			botRight = pos + mat.Translate(Vector2(w/2.0f, h/2.0f));
			botLeft = pos + mat.Translate(Vector2(-w/2.0f, h/2.0f));
		} else {
			topLeft = pos;
			topRight = pos + mat.Translate(Vector2(w, 0.0f));
			botRight = pos + mat.Translate(Vector2(w, h));
			botLeft = pos + mat.Translate(Vector2(0.0f, h));
		}
	}

	bool Rectangle::Contains(const Vector2 &point)
	{
		Vector2 verts[4];
		GetRotatedPosArray(verts);
		return PointInPolygon(point, verts, 4);
	}

	bool Rectangle::Intersects(const Rectangle &other)
	{
		Vector2 points[4]; GetRotatedPosArray(points);
		Vector2 otherPoints[4]; other.GetRotatedPosArray(otherPoints);

		if(PointInPolygon(points[0], otherPoints, 4)) { return true; }
		if(PointInPolygon(points[1], otherPoints, 4)) { return true; }
		if(PointInPolygon(points[2], otherPoints, 4)) { return true; }
		if(PointInPolygon(points[3], otherPoints, 4)) { return true; }

		if(PointInPolygon(otherPoints[0], points, 4)) { return true; }
		if(PointInPolygon(otherPoints[1], points, 4)) { return true; }
		if(PointInPolygon(otherPoints[2], points, 4)) { return true; }
		if(PointInPolygon(otherPoints[3], points, 4)) { return true; }

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

	bool pointInPolygonOnSegment(const Vector2 &p, const Vector2 &q, const Vector2 &r)
	{
		return q.X <= std::max(p.X, r.X) && q.X >= std::min(p.X, r.X) &&
					 q.Y <= std::max(p.Y, r.Y) && q.Y >= std::min(p.Y, r.Y);
	}

	int pointInPolygonOrientation(const Vector2 &p, const Vector2 &q, const Vector2 &r)
	{
		float val = (q.Y - p.Y) * (r.X - q.X) - (q.X - p.X) * (r.Y - q.Y);
		if(val == 0) {
			return 0;
		} else if(val > 0) {
			return 1;
		}
		return 2;
	}

	bool pointInPolygonDoIntersect(const Vector2 &p1, const Vector2 &q1, const Vector2 &p2, const Vector2 &q2)
	{
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
	 * Return whether the given point `p` is in the given polygon `poly`.
	 * Uses above pointInPolygon-prefixed functions as well.
	 * Source: http://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
	 * This appears to work a lot better than what we were using before: http://alienryderflex.com/polygon/
	 */
	bool Rectangle::PointInPolygon(const Vector2 &p, const Vector2* poly, int vertices)
	{
		if(vertices < 3) {
			return false;
		}

		Vector2 extreme(10000.0f, p.Y);
		int count = 0;
		int i = 0;
		do {
			int next = (i + 1) % vertices;
			if(pointInPolygonDoIntersect(poly[i], poly[next], p, extreme)) {
				if(pointInPolygonOrientation(poly[i], p, poly[next]) == 0) {
					return pointInPolygonOnSegment(poly[i], p, poly[next]);
				}
				count++;
			}
			i = next;
		} while(i != 0);
		return count & 1;
	}
}
