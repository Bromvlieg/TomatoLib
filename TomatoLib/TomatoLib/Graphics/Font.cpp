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
		if(this->FontHandle == 0) {
			return;
		}

#ifdef TL_ENABLE_FTGL
		glDeleteTextures(1, &this->TexID);
		texture_font_delete(this->FontHandle);
#endif

		this->FontHandle = nullptr;
	}

	Font::Font(ftgl::texture_font_t& fontdata) {
		this->FontHandle = &fontdata;

#ifdef TL_ENABLE_FTGL
		unsigned char* data = (unsigned char*)this->FontHandle->atlas->data;
		Texture newtex(this->FontHandle->atlas->width, this->FontHandle->atlas->height);
#else
		unsigned char* data = (unsigned char*)this->FontHandle->tex_data;
		Texture newtex(this->FontHandle->tex_width, this->FontHandle->tex_height);
#endif

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
			texture_font_load_glyphs(this->FontHandle, L" ~!@#$%^&*()_+`1234567890-=QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm|\\<>?,./:;\"'}{][”“’\n");
		}

		this->ReuploadTexture();
#else
		throw "TomatoLib was build without FTGL support";
#endif

		return true;
	}

	void Font::ReuploadTexture() {
#ifdef TL_ENABLE_FTGL
		unsigned char* data = (unsigned char*)this->FontHandle->atlas->data;
		Texture newtex(this->FontHandle->atlas->width, this->FontHandle->atlas->height);
#else
		unsigned char* data = (unsigned char*)this->FontHandle->tex_data;
		Texture newtex(this->FontHandle->tex_width, this->FontHandle->tex_height);
#endif

		for (size_t i = 0; i < newtex.Width * newtex.Height; i++) {
			newtex.PixelData[i * 4 + 0] = 255;
			newtex.PixelData[i * 4 + 1] = 255;
			newtex.PixelData[i * 4 + 2] = 255;
			newtex.PixelData[i * 4 + 3] = data[i];
		}

		if (this->TexID != 0) {
			newtex.GLHandle = this->TexID;
			newtex.RegisteredInGL = true;
		} else {
			newtex.BindGL();
		}

		newtex.Upload();

		this->TexID = newtex.GLHandle;

		newtex.RegisteredInGL = false;
		newtex.GLHandle = 0;
	}

	void Font::AddChars(const std::wstring& chars) {
#ifdef TL_ENABLE_FTGL
		std::wstring notfound;
		for (auto letter : chars) {
			bool found = false;

			const ftgl::texture_glyph_t** glyphs = reinterpret_cast<const ftgl::texture_glyph_t**>(this->FontHandle->glyphs->items);
			for (size_t i = 0; i < this->FontHandle->glyphs->size; i++) {
				const ftgl::texture_glyph_t& glyph = *glyphs[i];

				if (glyph.charcode == letter) {
					found = true;
					break;
				}
			}

			if (!found) {
				notfound += letter;
			}
		}

		if (notfound.empty()) return;
		texture_font_load_glyphs(this->FontHandle, notfound.c_str());

		this->ReuploadTexture();
#else
		throw "TomatoLib was build without FTGL support";
#endif
	}
}
