#include "Texture.h"

#include "../Libaries/LodePNG/lodepng.h"
#include "../Defines.h"

#ifdef _MSC_VER
#include <gl/glew.h>
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#endif

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
		ASSERT(error == 0);
		if (error != 0) return;

		this->Filename = fileName;
		this->PixelData = image;
	}

	Texture::~Texture() {
		if (!this->RegisteredInGL) return;
		glDeleteTextures(1, &this->GLHandle);
	}

	void Texture::BindGL() {
		if (this->RegisteredInGL) return;

		unsigned int wpow = this->Width;
		unsigned int hpow = this->Height;

		if (wpow != 0 && (wpow & (wpow - 1)) != 0) {
			wpow--;
			wpow |= wpow >> 1;
			wpow |= wpow >> 2;
			wpow |= wpow >> 4;
			wpow |= wpow >> 8;
			wpow |= wpow >> 16;
			wpow++;
		}

		if (hpow != 0 && (hpow & (hpow - 1)) != 0) {
			hpow--;
			hpow |= hpow >> 1;
			hpow |= hpow >> 2;
			hpow |= hpow >> 4;
			hpow |= hpow >> 8;
			hpow |= hpow >> 16;
			hpow++;
		}


		if (wpow != this->Width || hpow != this->Height) {
			this->Resize(wpow, hpow);
		}

		glGenTextures(1, &this->GLHandle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->GLHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		this->RegisteredInGL = true;
	}

	void Texture::Resize(int neww, int newh) {
		auto newdata = std::vector<unsigned char>(neww * newh * 4);

		float wperc = (float)neww / (float)this->Width;
		float hperc = (float)newh / (float)this->Height;

		for (int cy = 0; cy < newh; cy++) {
			for (int cx = 0; cx < neww; cx++) {
				int pixel = (cy * (neww * 4)) + (cx * 4);
				int nearestMatch = (((int)(cy / hperc) * (this->Width * 4)) + ((int)(cx / wperc) * 4));

				*(int*)&newdata[pixel] = *(int*)&this->PixelData[nearestMatch];
			}
		}

		this->PixelData = newdata;
		this->Width = neww;
		this->Height = newh;
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
		glBindTexture(GL_TEXTURE_2D, this->GLHandle);
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
