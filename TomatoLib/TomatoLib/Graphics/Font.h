#pragma once
#ifndef __FONT_H__
#define __FONT_H__

#include "../Config.h"
#include <string>

#ifdef TL_ENABLE_FTGL
#include <freetype-gl.h>
#include <vertex-buffer.h>
#include <markup.h>
#include <mat4.h>
#endif

namespace TomatoLib {
#ifndef TL_ENABLE_FTGL
	class ftgl_texture_font_t_mirror {
	public:
		float height;
	};
#endif

	class Font {
	public:
#ifdef TL_ENABLE_FTGL
		ftgl::texture_font_t* FontHandle;
#else
		ftgl_texture_font_t_mirror* FontHandle;
#endif

		Font(const std::string &file, float size);
		Font();
		~Font();

		 unsigned int TexID;

		bool Load(const std::string &file, float size);
		void AddChars(const wchar_t* chars);

#ifdef TL_ENABLE_FTGL
		static texture_atlas_t* Atlas;
#endif
	};
}
#endif
