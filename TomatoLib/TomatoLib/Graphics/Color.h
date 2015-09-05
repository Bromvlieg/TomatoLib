#pragma once
#ifndef __COLOR_H__
#define __COLOR_H__

#include <string>

namespace TomatoLib {
	class Color {
	public:
		unsigned char R, G, B, A;

		Color();
		Color(unsigned char R, unsigned char G, unsigned char B);
		Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
		Color(unsigned int hex);

		std::string ToString() const;

		const static Color Black; // Color(0, 0, 0)
		const static Color White; // Color(255, 255, 255)
		const static Color Red; // Color(255, 0, 0)
		const static Color Green; // Color(0, 255, 0)
		const static Color Blue; // Color(0, 0, 255)
		const static Color Orange; // Color(255, 153, 0)
		const static Color Yellow; // Color(255, 255, 0)
		const static Color Transparent; // Color(0, 0, 0, 0)

		Color Lerp(const Color& other, float timestep);

		bool operator== (const Color& other) const;
		bool operator!= (const Color& other) const;
		Color operator* (int mult) const;
		Color operator* (const Color& other) const;
	};
}
#endif