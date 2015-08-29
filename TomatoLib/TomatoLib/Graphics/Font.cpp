#include "Font.h"
#include "../Defines.h"

namespace TomatoLib {
	texture_atlas_t* Font::Atlas = null;

	Font::Font(const std::string &file, float size) {
		Load(file, size);
	}

	Font::Font() {
		this->FontHandle = 0;
	}

	Font::~Font() {
		if(this->FontHandle == 0) {
			return;
		}
		texture_font_delete(this->FontHandle);
	}

	bool Font::Load(const std::string &file, float size) {
		if (Font::Atlas == null) {
			Font::Atlas = texture_atlas_new(1024, 1024, 1);
		}

		this->FontHandle = texture_font_new_from_file(Font::Atlas, size, file.c_str());
		if (this->FontHandle == nullptr) return false;

		texture_font_load_glyphs(this->FontHandle, L"~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|\\<>?,./:;\"'}{][”“’\n");
		return true;
	}
}
