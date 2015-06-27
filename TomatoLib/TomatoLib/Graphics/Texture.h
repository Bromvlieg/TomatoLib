#pragma once
#ifndef __TEXTURELOADER_H__
#define __TEXTURELOADER_H__

#include <vector>
#include "Color.h"

namespace TomatoLib {
	class Texture {
	public:
		Texture();
		Texture(unsigned int w, unsigned int h);
		Texture(const char* fileName);
		~Texture();

		bool RegisteredInGL;
		unsigned int GLHandle;
		unsigned int Width, Height;
		std::vector<unsigned char> PixelData;

		Color GetPixel(int x, int y);
		void SetPixel(int x, int y, Color col);

		void Copy(Texture* t);
		void BindGL();
		void Upload();
		void Use();
		void Clear();
	};
}

#endif
