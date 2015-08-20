#include "Texture.h"

#include "../Libaries/LodePNG/lodepng.h"

#ifdef _MSC_VER
#include <gl/glew.h>
#else
#include <glew.h>
#endif
#include <GLFW/glfw3.h>

namespace TomatoLib {
	Texture::Texture() {
		this->Width = 0;
		this->Height = 0;
		this->RegisteredInGL = false;
	}

	Texture::Texture(unsigned int w, unsigned int h) {
		this->RegisteredInGL = false;
		this->PixelData = std::vector<unsigned char>(w * h * 4);
		this->Width = w;
		this->Height = h;
		this->RegisteredInGL = false;
	}

	Texture::Texture(const char* fileName) {
		this->RegisteredInGL = false;
		std::vector<unsigned char> image;
		unsigned error = lodepng::decode(image, this->Width, this->Height, fileName);
		if (error != 0) return;

		this->PixelData = image;
		this->GLHandle = 0;
	}

	Color Texture::GetPixel(int x, int y) {
		return Color(
			this->PixelData[(y * this->Width + x) * 4 + 0],
			this->PixelData[(y * this->Width + x) * 4 + 1],
			this->PixelData[(y * this->Width + x) * 4 + 2],
			this->PixelData[(y * this->Width + x) * 4 + 3]
			);
	}

	void Texture::SetPixel(int x, int y, Color col) {
		this->PixelData[(y * this->Width + x) * 4 + 0] = col.R;
		this->PixelData[(y * this->Width + x) * 4 + 1] = col.G;
		this->PixelData[(y * this->Width + x) * 4 + 2] = col.B;
		this->PixelData[(y * this->Width + x) * 4 + 3] = col.A;
	}

	Texture::~Texture() {
		if (!this->RegisteredInGL) return;
		glDeleteTextures(1, &this->GLHandle);
	}

	void Texture::BindGL() {
		this->RegisteredInGL = true;

		glGenTextures(1, &this->GLHandle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->GLHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void Texture::Copy(Texture* o) {
		this->Width = o->Width;
		this->Height = o->Height;
		this->PixelData = o->PixelData;
	}

	void Texture::Upload() {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->Width, this->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &this->PixelData[0]);
	}

	void Texture::Use() {
		glBindTexture(GL_TEXTURE_2D, this->GLHandle);
	}

	void Texture::Clear() {
		if (this->RegisteredInGL) {
			glDeleteTextures(1, &this->GLHandle);
			this->RegisteredInGL = false;
		}
	
		this->PixelData.clear();
		this->Width = 0;
		this->Height = 0;
		this->GLHandle = 0;
	}
}
