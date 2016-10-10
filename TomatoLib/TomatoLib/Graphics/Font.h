#pragma once
#ifndef __FONT_H__
#define __FONT_H__

#ifdef TL_FONT_STATIC_SIZE
#define TL_FONT_STATICSIZE_INTERNAL TL_FONT_STATIC_SIZE
#else
#define TL_FONT_STATICSIZE_INTERNAL 65536
#endif

#include "../Config.h"
#include "../Defines.h"
#include <string>

#ifdef TL_ENABLE_FTGL
#include <freetype-gl.h>
#include <vertex-buffer.h>
#include <markup.h>
#include <mat4.h>
#else
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
	namespace ftgl {
#endif
		typedef struct {
			wchar_t charcode;
			float kerning;
		} kerning_t;

		typedef struct {
			wchar_t charcode;
			int width, height;
			int offset_x, offset_y;
			float advance_x, advance_y;
			float s0, t0, s1, t1;
			size_t kerning_count;
			kerning_t kerning[10];
		} texture_glyph_t;

		typedef struct {
			size_t tex_width;
			size_t tex_height;
			size_t tex_depth;
			char tex_data[TL_FONT_STATICSIZE_INTERNAL];
			float size;
			float height;
			float linegap;
			float ascender;
			float descender;
			size_t glyphs_count;
			texture_glyph_t glyphs[96];
		} texture_font_t;
#ifdef __cplusplus
	}
}
#endif
#endif

namespace TomatoLib {
	class Font {
	public:
		const ftgl::texture_font_t* FontHandle;

		Font(const std::string &file, float size);
		Font(const ftgl::texture_font_t& persistant_data);
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
