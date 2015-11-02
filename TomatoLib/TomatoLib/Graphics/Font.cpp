#include "Font.h"
#include "Texture.h"
#include "../Defines.h"

namespace TomatoLib {
	texture_atlas_t* Font::Atlas = null;

	Font::Font(const std::string &file, float size) {
		Load(file, size);
	}

	Font::Font() {
		this->FontHandle = 0;
		this->TexID = 0;
	}

	Font::~Font() {
		if(this->FontHandle == 0) {
			return;
		}
		texture_font_delete(this->FontHandle);

		glDeleteTextures(1, &this->TexID);
	}

	bool Font::Load(const std::string &file, float size) {
		if (Font::Atlas == null) {
			Font::Atlas = texture_atlas_new(1024, 1024, 1);
		}

		this->FontHandle = texture_font_new_from_file(Font::Atlas, size, file.c_str());
		if (this->FontHandle == nullptr) return false;
		texture_font_load_glyphs(this->FontHandle, L"~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|\\<>?,./:;\"'}{][”“’\n");

		unsigned char* data = Font::Atlas->data;
		Texture newtex(1024, 1024);
		for (int i = 0; i < 1024 * 1024; i++) {
			newtex.PixelData[i * 4 + 0] = 255;
			newtex.PixelData[i * 4 + 1] = 255;
			newtex.PixelData[i * 4 + 2] = 255;
			newtex.PixelData[i * 4 + 3] = data[i];
		}

		newtex.BindGL();
		newtex.Upload();

		this->TexID = newtex.GLHandle;

		newtex.RegisteredInGL = false;
		newtex.GLHandle = 0;

		return true;
	}

	void Font::AddChars(const wchar_t* chars) {
		texture_font_load_glyphs(this->FontHandle, chars);
	}
}
