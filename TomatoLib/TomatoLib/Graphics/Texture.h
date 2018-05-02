#pragma once
#ifndef __TEXTURELOADER_H__
#define __TEXTURELOADER_H__
 
#include <vector>
#include "Color.h"

namespace TomatoLib {
	class Texture {
	public:
		Texture();
		Texture(const Texture& t);
		Texture(unsigned int w, unsigned int h);
		Texture(const std::string& fileName);
		~Texture();

		bool RegisteredInGL;
		unsigned int GLHandle;
		unsigned int Width, Height;
		std::vector<unsigned char> PixelData;

		std::string Filename;

		inline Color GetPixel(int x, int y) {
			return Color(
				this->PixelData[(y * this->Width + x) * 4 + 0],
				this->PixelData[(y * this->Width + x) * 4 + 1],
				this->PixelData[(y * this->Width + x) * 4 + 2],
				this->PixelData[(y * this->Width + x) * 4 + 3]
				);
		}

		inline void SetPixel(int x, int y, const Color& col) {
			this->PixelData[(y * this->Width + x) * 4 + 0] = col.R;
			this->PixelData[(y * this->Width + x) * 4 + 1] = col.G;
			this->PixelData[(y * this->Width + x) * 4 + 2] = col.B;
			this->PixelData[(y * this->Width + x) * 4 + 3] = col.A;
		}

		void Copy(const Texture& t);
		void BindGL();
		void Upload();
		void Use();
		void Clear();
		void Resize(int neww, int newh);
		bool FromFile(const std::string& filename);
	};
}

#endif
