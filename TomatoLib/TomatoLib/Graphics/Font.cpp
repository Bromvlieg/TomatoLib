#include "Font.h"
#include "Texture.h"
#include "../Config.h"
#include "../Defines.h"
#include "../Utilities/Utilities.h"

namespace TomatoLib {
#ifdef TL_ENABLE_FTGL
	texture_atlas_t* Font::Atlas = null;
#endif

	Font::Font(const std::string &file, float size) {
		Load(file, size);
	}

	Font::Font() {
		this->FontHandle = nullptr;
		this->TexID = 0;
	}

	Font::~Font() {
#ifdef TL_ENABLE_FTGL
		if(this->FontHandle == 0) {
			return;
		}
		texture_font_delete(this->FontHandle);

		glDeleteTextures(1, &this->TexID);
#endif
	}

	Font::Font(const ftgl::texture_font_t& fontdata) {
		this->FontHandle = &fontdata;

		unsigned char* data = (unsigned char*)this->FontHandle->tex_data;
		Texture newtex(this->FontHandle->tex_width, this->FontHandle->tex_height);
		for (size_t i = 0; i < newtex.Width * newtex.Height; i++) {
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
	}

	bool Font::Load(const std::string &file, float size) {
#ifdef TL_ENABLE_FTGL
		if (Font::Atlas == nullptr) {
			Font::Atlas = texture_atlas_new(1024, 1024, 1);
		}

		this->FontHandle = texture_font_new_from_file(Font::Atlas, size, file.c_str());
		if (this->FontHandle == nullptr) {
			std::string fallback = Utilities::GetFormatted("%s_%f.bin", file.c_str(), size);

			FILE* f = fopen(fallback.c_str(), "rb");
			if (!f) return false;

			fseek(f, 0, SEEK_END);
			size_t size = ftell(f);

			unsigned char* data = new unsigned char[size];

			rewind(f);
			fread(data, sizeof(char), size, f);
			fclose(f);

			this->FontHandle = (texture_font_t*)data;
		} else {
			texture_font_load_glyphs(this->FontHandle, L"~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|\\<>?,./:;\"'}{][”“’\n");
		}

		unsigned char* data = Font::Atlas->data;
		Texture newtex(Font::Atlas->width, Font::Atlas->height);
		for (size_t i = 0; i < newtex.Width * newtex.Height; i++) {
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
#else
		throw "TomatoLib was build without FTGL support";
#endif

		return true;
	}

	void Font::AddChars(const wchar_t* chars) {
#ifdef TL_ENABLE_FTGL
		texture_font_load_glyphs(this->FontHandle, chars);
#else
		throw "TomatoLib was build without FTGL support";
#endif
	}
}
