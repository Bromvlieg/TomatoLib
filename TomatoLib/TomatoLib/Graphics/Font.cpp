#include "Font.h"
#include "../Defines.h"

namespace TomatoLib {
	texture_atlas_t* Font::Atlas = null;

	Font::Font(std::string file, float size) {
		if (Font::Atlas == null) {
			Font::Atlas = texture_atlas_new(1024, 1024, 1);
		}

		this->FontHandle = texture_font_new_from_file(Font::Atlas, size, file.c_str());
		texture_font_load_glyphs(this->FontHandle, L"~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|\\<>?,./:;\"'}{][”“’\n");
	}

	Font::~Font() {
		texture_font_delete(this->FontHandle);
	}
}