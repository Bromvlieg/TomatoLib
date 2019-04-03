#include "Line.h"

namespace TomatoLib {
	bool Line::intersects(const Line & other) {
		float s1_x, s1_y, s2_x, s2_y, sn, tn, sd, td, t;
		s1_x = this->B.X - this->A.X;     s1_y = this->B.Y - this->A.Y;
		s2_x = other.B.X - other.A.X;     s2_y = other.B.Y - other.A.Y;

		sn = -s1_y * (this->A.X - other.A.X) + s1_x * (this->A.Y - other.A.Y);
		sd = -s2_x * s1_y + s1_x * s2_y;
		tn = s2_x * (this->A.Y - other.A.Y) - s2_y * (this->A.X - other.A.X);
		td = -s2_x * s1_y + s1_x * s2_y;

		if (sn >= 0 && sn <= sd && tn >= 0 && tn <= td) {
			return true;
		}

		return false;
	}

}