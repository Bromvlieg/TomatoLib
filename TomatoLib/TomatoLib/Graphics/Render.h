#pragma once
#ifndef __TL__RENDER_H_
#define __TL__RENDER_H_

#include "../Math/AABB.h"
#include "../Math/Vector2.h"
#include "Texture.h"
#include "Color.h"
#include "Shader.h"
#include "Font.h"
#include "../Utilities/List.h"
#include <vector>
#include <string>

#ifndef TL_ENABLE_EGL
#include <GL/glew.h>
#else
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

// Small fix for Linux builds where 'None' is defined to 0
#ifdef None
#undef None
#endif

namespace TomatoLib {
	enum class FlipMode {
		Vertical,
		Horizontal,
		Both,
		None
	};

	enum class RenderAlignment {
		Left,
		Center,
		Right
	};

	struct _vertexData {
		Vector2 Location;
		Vector2 TextureLocation;
		TomatoLib::Color Color;

		_vertexData() :TextureLocation(-1.0f, -1.0f) {};
	};

	struct _GlyphData {
		Vector2 Start;
		Vector2 End;

		int Width;
		int Height;
		int OffsetY;
		int OffsetX;
		int MonospaceX;

		char Letter;

		_GlyphData() :Width(0), Height(0), OffsetX(0), OffsetY(0), MonospaceX(0), Letter('\0') {}
	};

	struct RenderBufferChunk {
		GLuint TextureHandle;
		GLuint ShaderHandle;

		unsigned int VerticeDataCount;
		_vertexData* VerticeData;
		unsigned int IndiceDataCount;
		GLuint* IndiceData;
	};

	struct RenderBuffer {
		List<RenderBufferChunk> Chunks;
	};

	class Render {
	private:
		unsigned int VerticeDataCount;
		_vertexData* VerticeData;
		unsigned int IndiceDataCount;
		GLuint* IndiceData;
		unsigned int IndiceDataSize;
		unsigned int VerticeDataSize;
		unsigned int FlippingStart;
		FlipMode FlippingMode;
		Vector2 FlippingCenter;

		int MonospaceWidth;

		GLuint vao;
		GLuint vbo;
		GLuint ebo;

		RenderBuffer* CaptureBuffer;

		size_t m_uiDrawnVertices;
		size_t m_uiDrawnIndices;

	public:
		Shader DefaultShaderText;
		Shader DefaultShaderTexture;

		Vector2 ScreenSize;

		GLuint CurrentTexture;
		GLuint CurrentShader;

		static Texture DefaultTexture;
		static Font* DefaultFont;

		bool ClippingEnabled;
		Vector2 _ClippingPos;
		Vector2 _ClippingSize;
		Vector2 _DrawOffset;
		List<AABB> _ClippingList;

		bool DisableDeptTest;

		Render();
		~Render();

		void Buffer(const RenderBuffer& buffer);
		void Box(int x, int y, int w, int h, const Color& color);
		void Box(float x, float y, float w, float h, const Color& color);
		void BoxOutlined(int x, int y, int w, int h, int bordersize, const Color& color);
		void BoxOutlined(float x, float y, float w, float h, float bordersize, const Color& color);
		void Circle(int x, int y, int w, int h, int roundness, const Color& color);
		void Circle(float x, float y, float w, float h, int roundness, const Color& color);
		void CircleOutlined(int x, int y, int w, int h, int bordersize, int roundness, const Color& color);
		void CircleOutlined(float x, float y, float w, float h, float bordersize, int roundness, const Color& color);
		void Line(float x1, float y1, float x2, float y2, float width, const Color& color);
		void Triangle(const Vector2& a, const Vector2& b, const Vector2& c, const Color& color);
		void Triangle(const Vector2& a, const Vector2& b, const Vector2& c, const Color& colora, const Color& colorb, const Color& colorc);
		void Poly(Vector2* points, int len, const Color& color);
		void PolyOutlined(Vector2* points, int len, float linew, const Color& color);

		void Text(const std::string& text, int x, int y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const std::string& text, float x, float y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const Font* font, const std::string& text, int x, int y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const Font* font, const std::string& text, float x, float y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);

		void Text(const std::wstring& text, int x, int y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const std::wstring& text, float x, float y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const Font* font, const std::wstring& text, int x, int y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);
		void Text(const Font* font, const std::wstring& text, float x, float y, const Color& color, RenderAlignment alignx = RenderAlignment::Left, RenderAlignment aligny = RenderAlignment::Left);

		void PutTexture(float x, float y, float w, float h, float tex_x_start, float tex_y_start, float tex_x_end, float tex_y_end, const Color& color);
		void PutTexture(Texture& tex, float x, float y, float w, float h, float tex_x_start, float tex_y_start, float tex_x_end, float tex_y_end, const Color& color);
		void PutTexture(float x, float y, float w, float h, float tex_x_start, float tex_y_start, float tex_x_end, float tex_y_end, float rotation, const Color& color);
		void PutTexture(Texture& tex, float x, float y, float w, float h, float tex_x_start, float tex_y_start, float tex_x_end, float tex_y_end, float rotation, const Color& color);

		void SetTexture(GLint handle);
		void SetTexture(Texture& tex);
		void SetShader(GLuint handle);
		void SetShader(const Shader& shader);

		Vector2 GetTextSize(char text);
		Vector2 GetTextSize(const Font& font, char text);
		Vector2 GetTextSize(const Font* font, char text);
		Vector2 GetTextSize(const std::string& text);
		Vector2 GetTextSize(const Font& font, const std::string& text);
		Vector2 GetTextSize(const Font* font, const std::string& text);

		Vector2 GetTextSize(wchar_t text);
		Vector2 GetTextSize(const Font& font, wchar_t text);
		Vector2 GetTextSize(const Font* font, wchar_t text);
		Vector2 GetTextSize(const std::wstring& text);
		Vector2 GetTextSize(const Font& font, const std::wstring& text);
		Vector2 GetTextSize(const Font* font, const std::wstring& text);

		void SetDrawingOffset(int x, int y);
		Vector2 GetDrawingOffset();

		void EnableClipping(int x, int y, int w, int h);
		void DisableClipping();

		void EnableFlipping(FlipMode mode, const Vector2& center);
		void DisableFlipping();

		void RecorderStart(RenderBuffer& buff);
		void RecorderStop();

		void CheckSpace(int vcount, int icount);
		void DrawOnScreen();

		void GetDrawStats(size_t& vertices, size_t& indices);
		void ResetDrawStats();
	};
}

#endif