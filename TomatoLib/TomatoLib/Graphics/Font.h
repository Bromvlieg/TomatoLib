#pragma once
#ifndef __FONT_H__
#define __FONT_H__

#include <string>
#include <freetype-gl.h>
#include <vertex-buffer.h>
#include <markup.h>
#include <mat4.h>

namespace TomatoLib {
	class Font {
	public:
		ftgl::texture_font_t* FontHandle;

		Font(const std::string &file, float size);
		Font();
		~Font();

		bool Load(const std::string &file, float size);
		void AddChars(const wchar_t* chars);

		static texture_atlas_t* Atlas;
	};
}
#endif
