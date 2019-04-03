	#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include "Vector2.h"
#include "list"
#include <vector>

namespace TomatoLib {
	class Rectangle {
	public:
		float x;
		float y;
		float w;
		float h;

		float rotation;
		bool anchorInCenter;

	public:
		Rectangle();
		Rectangle(Vector2 vPos, Vector2 vSize);
		Rectangle(Vector2 vPos, Vector2 vSize, float fRot);
		Rectangle(float fX, float fY, float fW, float fH);
		Rectangle(float fX, float fY, float fW, float fH, float fRot);
		~Rectangle();

		void Translate(const Vector2 &v);
		void Rotate(float fDegrees);

		Vector2 GetPos() const;
		Vector2 GetSize() const;

		void GetRotatedPosArray(Vector2* pArray) const; // requires array size of at least (typeof(Vector2)*4)

		bool Contains(const Vector2 &point);

		bool Intersects(const Rectangle &other);

		static bool PointInPolygon(const Vector2 &p, const std::vector<Vector2>& poly);
	};

	/*
	 * TODO: Implement this!
	inline Rectangle ParseRectangle(const Scratch::String &str)
	{
		Scratch::StackArray<Scratch::String> parse;
		str.Split(",", parse, TRUE);

		Rectangle ret;

		if(parse.Count() >= 2) {
			ret.x = (float)atof(parse[0]);
			ret.y = (float)atof(parse[1]);
		}

		if(parse.Count() >= 4) {
			ret.w = (float)atof(parse[2]);
			ret.h = (float)atof(parse[3]);
		}

		return ret;
	}
	*/
}

#endif
