#include "Texture.h"

#include "../Libaries/LodePNG/lodepng.h"
#include "../Defines.h"

#ifdef _MSC_VER
#include <gl/glew.h>
#else
#include <glew.h>
#endif
#include <GLFW/glfw3.h>

namespace TomatoLib {
	Texture::Texture() {
		this->RegisteredInGL = false;
		this->GLHandle = -1;
		this->Width = 0;
		this->Height = 0;
	}

	Texture::Texture(unsigned int w, unsigned int h) {
		this->RegisteredInGL = false;
		this->GLHandle = -1;
		this->Width = w;
		this->Height = h;
		this->PixelData = std::vector<unsigned char>(w * h * 4);
	}

	Texture::Texture(const char* fileName) {
		this->RegisteredInGL = false;
		this->GLHandle = -1;
		this->Filename = fileName;

		std::vector<unsigned char> image;
		unsigned error = lodepng::decode(image, this->Width, this->Height, fileName);
		if (error != 0) return;

		this->Filename = fileName;
		this->PixelData = image;
	}

	Texture::~Texture() {
		if (!this->RegisteredInGL) return;
		glDeleteTextures(1, &this->GLHandle);
	}

	void Texture::BindGL() {
		glGenTextures(1, &this->GLHandle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->GLHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		this->RegisteredInGL = true;
	}

	Texture::Texture(const Texture& o) {
		this->Copy(o);
	}

	void Texture::Copy(const Texture& o) {
		this->Width = o.Width;
		this->Height = o.Height;
		this->PixelData = o.PixelData;
		this->Filename = o.Filename;
		
		this->RegisteredInGL = false;
		this->GLHandle = -1;
	}

	void Texture::Upload() {
		TL_ASSERT(this->RegisteredInGL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->Width, this->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &this->PixelData[0]);
	}

	void Texture::Use() {
		TL_ASSERT(this->RegisteredInGL);
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
		this->GLHandle = -1;
	}
}
