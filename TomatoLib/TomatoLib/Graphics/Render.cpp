#include "Render.h"

#include "../Defines.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <cstring>

#define INDICE_PUSH(x) TL_ASSERT(this->IndiceDataCount != this->IndiceDataSize); this->IndiceData[this->IndiceDataCount++] = x
#define VERTICE_PUSH(x) TL_ASSERT(this->VerticeDataCount != this->VerticeDataSize); this->VerticeData[this->VerticeDataCount++] = x

namespace TomatoLib {
	void __shaderStuff(Shader& s) {
		s.Link();
		s.Use();

		glBindFragDataLocation(s.ProgramHandle, 0, "outColor");
		glUniform1i(glGetUniformLocation(s.ProgramHandle, "tex"), 0);

		GLint posAttrib = glGetAttribLocation(s.ProgramHandle, "position");
		glEnableVertexAttribArray(posAttrib);
		checkGL;
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
		checkGL;

		GLint tposAttrib = glGetAttribLocation(s.ProgramHandle, "texpos");
		checkGL;
		glEnableVertexAttribArray(tposAttrib);
		checkGL;
		glVertexAttribPointer(tposAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 3));
		checkGL;

		GLint colAttrib = glGetAttribLocation(s.ProgramHandle, "color");
		checkGL;
		glEnableVertexAttribArray(colAttrib);
		checkGL;
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(sizeof(float) * 5));
	}
	
	bool firstcreate = true;
	Texture Render::DefaultTexture(1, 1);
	Font* Render::DefaultFont = null;

	Render::Render() {
		this->VerticeDataCount = 0;
		this->IndiceDataCount = 0;

		this->VerticeData = null;
		this->IndiceData = null;

		this->VerticeDataSize = 0;
		this->IndiceDataSize = 0;

		this->ClippingEnabled = false;
		this->CaptureBuffer = null;

		checkGL;
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);
		glGenBuffers(1, &this->vbo);
		glGenBuffers(1, &this->ebo);
		checkGL;

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

		if (firstcreate) {
			firstcreate = false;

			this->DefaultTexture.SetPixel(0, 0, Color::White);
			this->DefaultTexture.BindGL();
			this->DefaultTexture.Upload();

			this->DefaultShaderText.AttachRaw("#version 150\n\nout vec4 outColor;\n\nin vec4 output_color;\nin vec2 output_texpos;\n\nuniform sampler2D tex;\n\nvoid main(){outColor = output_color; outColor.a *= texture(tex, output_texpos).r;\nif (outColor.a == 0) discard;\n}\n", GL_FRAGMENT_SHADER);
			this->DefaultShaderText.AttachRaw("#version 150\nin vec3 position;\nin vec2 texpos;\nin vec4 color;\n\nout vec2 output_texpos;\nout vec4 output_color;\nvoid main() {\ngl_Position = vec4(position, 1);\noutput_color = color;\noutput_texpos = texpos;\n}\n", GL_VERTEX_SHADER);
			__shaderStuff(this->DefaultShaderText);

			this->DefaultShaderTexture.AttachRaw("#version 150\n\nout vec4 outColor;\n\nin vec4 output_color;\nin vec2 output_texpos;\n\nuniform sampler2D tex;\n\nvoid main(){\noutColor = texture(tex, output_texpos) * output_color;\n\nif (outColor.a == 0) discard;\n}\n", GL_FRAGMENT_SHADER);
			this->DefaultShaderTexture.AttachRaw("#version 150\nin vec3 position;\nin vec2 texpos;\nin vec4 color;\n\nout vec2 output_texpos;\nout vec4 output_color;\nvoid main() {\ngl_Position = vec4(position, 1);\noutput_color = color;\noutput_texpos = texpos;\n}\n", GL_VERTEX_SHADER);
			__shaderStuff(this->DefaultShaderTexture);
		}

		this->CurrentShader = this->DefaultShaderText.ProgramHandle;
		this->CurrentTexture = this->DefaultTexture.GLHandle;

		checkGL;
	}

	Render::~Render() {
		if (this->VerticeData != null) delete[] this->VerticeData;
		if (this->IndiceData != null) delete[] this->IndiceData;

		glDeleteBuffers(1, &this->ebo);
		glDeleteBuffers(1, &this->vbo);
		glDeleteVertexArrays(1, &this->vao);
	}

	void Render::Line(float x1, float y1, float x2, float y2, float width, const Color& color) {
		if (color.A == 0) return;
		this->SetTexture(this->DefaultTexture.GLHandle);
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);

		float ang = atan2(x1 - x2, y1 - y2) - (float)(M_PI / 2);
		float dist = sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));

		float rad90 = (float)(M_PI / 4);

		x1 += this->_DrawOffset.X;
		y1 += this->_DrawOffset.Y;
		x2 += this->_DrawOffset.X;
		y2 += this->_DrawOffset.Y;

		width /= 2;

		// left side, pull back + up/down
		Vector2 apos = Vector2(x1 + cos(ang) * -width, y1 + sin(ang - rad90) * width);
		Vector2 cpos = Vector2(x1 + cos(ang) * -width, y1 + sin(ang + rad90) * width);

		// right side, push away + up/down
		Vector2 bpos = Vector2(x2 + cos(ang) * width, y2 + sin(ang - rad90) * width);
		Vector2 dpos = Vector2(x2 + cos(ang) * width, y2 + sin(ang + rad90) * width);


		_vertexData a, b, c, d;

		a.Location = apos;
		b.Location = bpos;
		c.Location = cpos;
		d.Location = dpos;

		a.Color = color;
		b.Color = color;
		c.Color = color;
		d.Color = color;

		unsigned int curVertices = this->VerticeDataCount;
		this->CheckSpace(4, 6); // per square, 4 vertices and 6 indices

		VERTICE_PUSH(a);
		VERTICE_PUSH(b);
		VERTICE_PUSH(c);
		VERTICE_PUSH(d);

		INDICE_PUSH(curVertices + 0);
		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 2);

		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 3);
		INDICE_PUSH(curVertices + 2);
	}

	void Render::BoxOutlined(int x, int y, int w, int h, int bordersize, const Color& color) {
		if (color.A == 0) return;

		this->BoxOutlined((float)x, (float)y, (float)w, (float)h, (float)bordersize, color);
	}

	void Render::BoxOutlined(float x, float y, float w, float h, float bordersize, const Color& color) {
		if (color.A == 0) return;

		this->CheckSpace(16, 24); // per square, 4 vertices and 6 indices

		// up, left, right, down
		this->Box(x + bordersize, y, w - bordersize * 2, bordersize, color);
		this->Box(x, y, bordersize, h, color);
		this->Box(x + w - bordersize, y, bordersize, h, color);
		this->Box(x + bordersize, y + h - bordersize, w - bordersize * 2, bordersize, color);
	}

	void Render::Buffer(RenderBuffer* buff) {
		for (int i = 0; i < buff->Chunks.Count; i++) {
			RenderBufferChunk& b = buff->Chunks[i];
			this->SetShader(b.ShaderHandle);
			this->SetTexture(b.TextureHandle);

			this->CheckSpace(b.VerticeDataCount, b.IndiceDataCount);

			memcpy(this->VerticeData + this->VerticeDataCount, b.VerticeData, b.VerticeDataCount * sizeof(_vertexData));
			memcpy(this->IndiceData + this->IndiceDataCount, b.IndiceData, b.IndiceDataCount * sizeof(GLuint));

			for (unsigned int i2 = 0; i2 < b.IndiceDataCount; i2++) {
				this->IndiceData[i2 + this->IndiceDataCount] += this->VerticeDataCount;
			}

			this->VerticeDataCount += b.VerticeDataCount;
			this->IndiceDataCount += b.IndiceDataCount;
		}
	}

	void Render::Box(int x, int y, int w, int h, const Color& color) {
		if (color.A == 0) return;

		this->Box((float)x, (float)y, (float)w, (float)h, color);
	}

	void Render::Box(float x, float y, float w, float h, const Color& color) {
		if (color.A == 0) return;
		this->SetTexture(this->DefaultTexture.GLHandle);
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);

		x += this->_DrawOffset.X;
		y += this->_DrawOffset.Y;

		if (this->ClippingEnabled) {
			if (x > this->_ClippingPos.X + this->_ClippingSize.X) return;
			if (y > this->_ClippingPos.Y + this->_ClippingSize.Y) return;
			if (x + w < this->_ClippingPos.X) return;
			if (y + h < this->_ClippingPos.Y) return;

			if (x < this->_ClippingPos.X) {
				w -= this->_ClippingPos.X - x;
				x = this->_ClippingPos.X;
			}

			if (y < this->_ClippingPos.Y) {
				h -= this->_ClippingPos.Y - y;
				y = this->_ClippingPos.Y;
			}

			float maxX = (this->_ClippingPos.X + this->_ClippingSize.X);
			float maxY = (this->_ClippingPos.Y + this->_ClippingSize.Y);

			if (x + w > maxX) w = maxX - x;
			if (y + h > maxY) {
				h = maxY - y;
			}
		}

		_vertexData a, b, c, d;

		a.Location = Vector2(x, y);
		b.Location = Vector2(x + w, y);
		c.Location = Vector2(x, y + h);
		d.Location = Vector2(x + w, y + h);

		a.Color = color;
		b.Color = color;
		c.Color = color;
		d.Color = color;

		a.TextureLocation = Vector2(0, 0);
		b.TextureLocation = Vector2(1, 0);
		c.TextureLocation = Vector2(0, 1);
		d.TextureLocation = Vector2(1, 1);

		unsigned int curVertices = this->VerticeDataCount;
		this->CheckSpace(4, 6); // per square, 4 vertices and 6 indices

		VERTICE_PUSH(a);
		VERTICE_PUSH(b);
		VERTICE_PUSH(c);
		VERTICE_PUSH(d);

		INDICE_PUSH(curVertices + 0);
		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 2);

		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 3);
		INDICE_PUSH(curVertices + 2);
	}

	void Render::Triangle(const Vector2& a, const Vector2& b, const Vector2& c, const Color& color) {
		// TODO: clipping
		if (color.A == 0) return;
		this->SetTexture(this->DefaultTexture.GLHandle);
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);


		_vertexData vda, vdb, vdc;

		vda.Location = a + this->_DrawOffset;
		vdb.Location = b + this->_DrawOffset;
		vdc.Location = c + this->_DrawOffset;
		
		if (this->ClippingEnabled) {
			Vector2& min = this->_ClippingPos;
			Vector2 max = min + this->_ClippingSize;

			if (vda.Location.X < min.X && vdb.Location.X < min.X && vdc.Location.X < min.X) return;
			if (vda.Location.X > max.X && vdb.Location.X > max.X && vdc.Location.X > max.X) return;
			if (vda.Location.Y < min.Y && vdb.Location.Y < min.Y && vdc.Location.Y < min.Y) return;
			if (vda.Location.Y > max.Y && vdb.Location.Y > max.Y && vdc.Location.Y > max.Y) return;
		}

		vda.Color = color;
		vdb.Color = color;
		vdc.Color = color;

		unsigned int curVertices = this->VerticeDataCount;
		this->CheckSpace(3, 3); // per triangle, 3 vertices and 3 indices

		VERTICE_PUSH(vda);
		VERTICE_PUSH(vdb);
		VERTICE_PUSH(vdc);

		INDICE_PUSH(curVertices + 0);
		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 2);
	}


	void Render::Circle(int x, int y, int w, int h, int roundness, const Color& color) {
		this->Circle((float)x, (float)y, (float)w, (float)h, roundness, color);
	}

	void Render::Circle(float x, float y, float w, float h, int roundness, const Color& color) {
		w /= 2;
		h /= 2;

		x += w;
		y += h;

		float space = (float)M_PI / roundness * 2;

		this->CheckSpace(roundness * 6, roundness * 6);
		Vector2 a(x, y);
		for (int i = 0; i < roundness;) {
			Vector2 b(x + sin(space * i) * w, y + cos(space * i) * h);

			i++;
			Vector2 c(x + sin(space * i) * w, y + cos(space * i) * h);

			this->Triangle(a, b, c, color);
		}
	}

	void Render::CircleOutlined(int x, int y, int w, int h, int bordersize, int roundness, const Color& color) {
		this->CircleOutlined((float)x, (float)y, (float)w, (float)h, (float)bordersize, roundness, color);
	}

	void Render::CircleOutlined(float x, float y, float w, float h, float bordersize, int roundness, const Color& color) {
		w /= 2;
		h /= 2;

		x += w;
		y += h;

		float space = (float)M_PI / roundness * 2;

		this->CheckSpace(roundness * 6, roundness * 6);
		for (int i = 0; i < roundness;) {
			Vector2 a(x + (sin(space * i) * (w - bordersize)), y + (cos(space * i) * (h - bordersize)));
			Vector2 b(x + sin(space * i) * w, y + cos(space * i) * h);

			i++;
			Vector2 c(x + (sin(space * i) * (w - bordersize)), y + (cos(space * i) * (h - bordersize)));
			Vector2 d(x + sin(space * i) * w, y + cos(space * i) * h);

			this->Triangle(a, b, c, color);
			this->Triangle(b, d, c, color);
		}
	}

	void Render::PutTexture(float x, float y, float w, float h, float tex_x_start, float tex_y_start, float tex_x_end, float tex_y_end, const Color& color) {
		if (color.A == 0) return;
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);

		x += this->_DrawOffset.X;
		y += this->_DrawOffset.Y;

		_vertexData a, b, c, d;

		a.Location = Vector2(x, y);
		b.Location = Vector2(x + w, y);
		c.Location = Vector2(x, y + h);
		d.Location = Vector2(x + w, y + h);

		a.Color = color;
		b.Color = color;
		c.Color = color;
		d.Color = color;

		a.TextureLocation = Vector2(tex_x_start, tex_y_start);
		b.TextureLocation = Vector2(tex_x_end, tex_y_start);
		c.TextureLocation = Vector2(tex_x_start, tex_y_end);
		d.TextureLocation = Vector2(tex_x_end, tex_y_end);

		if (this->ClippingEnabled) {
			float pixelWidth = (tex_x_end - tex_x_start) / w;
			float pixelHeight = (tex_y_end - tex_y_start) / w;

			if (a.Location.X < this->_ClippingPos.X) {
				// if the sign is fully outside of the clipping, then just skip it.
				if (a.Location.X + w < this->_ClippingPos.X) {
					return;
				} else {
					// calculate the new screen and texture location
					float offset = this->_ClippingPos.X - a.Location.X;

					a.Location.X += offset;
					c.Location.X += offset;

					a.TextureLocation.X += offset * pixelWidth;
					c.TextureLocation.X += offset * pixelWidth;
				}
			}

			if (a.Location.X + w > this->_ClippingPos.X + this->_ClippingSize.X) {
				if (a.Location.X > this->_ClippingPos.X + this->_ClippingSize.X) {
					return;
				} else {
					float offset = a.Location.X + w - this->_ClippingPos.X - this->_ClippingSize.X;

					b.Location.X -= offset;
					d.Location.X -= offset;

					b.TextureLocation.X -= offset * pixelWidth;
					d.TextureLocation.X -= offset * pixelWidth;
				}
			}

			if (a.Location.Y < this->_ClippingPos.Y) {
				if (d.Location.Y < this->_ClippingPos.Y) {
					return;
				} else {
					float offset = this->_ClippingPos.Y - a.Location.Y;

					a.Location.Y += offset;
					b.Location.Y += offset;

					a.TextureLocation.Y += offset * pixelHeight;
					b.TextureLocation.Y += offset * pixelHeight;
				}
			}

			if (d.Location.Y > this->_ClippingPos.Y + this->_ClippingSize.Y) {
				if (b.Location.Y > this->_ClippingPos.Y + this->_ClippingSize.Y) {
					return;
				} else {
					float offset = d.Location.Y - this->_ClippingPos.Y - this->_ClippingSize.Y;

					c.Location.Y -= offset;
					d.Location.Y -= offset;

					c.TextureLocation.Y -= offset * pixelHeight;
					d.TextureLocation.Y -= offset * pixelHeight;
				}
			}
		}

		unsigned int curVertices = this->VerticeDataCount;
		this->CheckSpace(4, 6); // per square, 4 vertices and 6 indices

		VERTICE_PUSH(a);
		VERTICE_PUSH(b);
		VERTICE_PUSH(c);
		VERTICE_PUSH(d);

		INDICE_PUSH(curVertices + 0);
		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 2);

		INDICE_PUSH(curVertices + 1);
		INDICE_PUSH(curVertices + 3);
		INDICE_PUSH(curVertices + 2);
	}

	void Render::Poly(Vector2* points, int len, const Color& color) {
		if (color.A == 0) return;
		this->SetTexture(this->DefaultTexture.GLHandle);
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);

		// TODO: find a way to draw this with GL_TRIANGLES_STRIP, cus this ain't working for wierd ass poly things.
		this->CheckSpace(3 * len, 3 * len); // per triangle, 3 vertices and 3 indices
		for (int i = 0; i < len - 1; i++) {
			this->Triangle(points[0], points[i], points[i + 1], color);
		}
	}

	void Render::PolyOutlined(Vector2* points, int len, float w, const Color& color) {
		if (color.A == 0) return;
		this->SetTexture(this->DefaultTexture.GLHandle);
		this->SetShader(this->DefaultShaderTexture.ProgramHandle);

		// TODO: find a way to draw this with GL_TRIANGLES_STRIP, cus this ain't working for wierd ass poly things.
		this->CheckSpace(3 * len, 3 * len); // per triangle, 3 vertices and 3 indices
		for (int i = 0; i < len - 1; i++) {
			this->Line(points[i].X, points[i].Y, points[i + 1].X, points[i + 1].Y, w, color);
		}
		this->Line(points[0].X, points[0].Y, points[len - 1].X, points[len - 1].Y, w, color);
	}

	Vector2 Render::GetTextSize(char letter) {
		texture_glyph_t* glyph = texture_font_get_glyph(this->DefaultFont->FontHandle, (wchar_t)letter);
		if (glyph == null) {
			TL_ASSERT(false);
			return Vector2::Zero;
		}

		return Vector2((float)glyph->width, (float)glyph->height);
	}

	Vector2 Render::GetTextSize(const std::string& text) {
		return this->GetTextSize(*Render::DefaultFont, text);
	}

	Vector2 Render::GetTextSize(Font* font, const std::string& text) {
		return this->GetTextSize(*font, text);
	}

	Vector2 Render::GetTextSize(const Font& font, const std::string& text) {
		float totalW = 0;
		float totalH = 0;

		float cx = 0;
		float cy = 0;
		unsigned long len = text.size();

		for (unsigned long i = 0; i < len; i++) {
			char l = text[i];

			texture_glyph_t* glyph = texture_font_get_glyph(font.FontHandle, (wchar_t)text[i]);
			if (glyph == null) {
				TL_ASSERT(false);
				return Vector2::Zero;
			}

			if (i > 0) {
				cx += texture_glyph_get_kerning(glyph, text[i - 1]);
			}

			if (l == '\n') {
				cy += font.FontHandle->height;
				cx = 0;
				continue;
			}

			cx += glyph->advance_x;
			if (cx > totalW) totalW = cx;
			if (cy + font.FontHandle->height > totalH) totalH = cy + font.FontHandle->height;
		}

		return Vector2(totalW, totalH);
	}

	void Render::Text(const std::string& text, int x, int y, const Color& color) {
		this->Text(text, (float)x, (float)y, color);
	}

	void Render::Text(const std::string& text, float x, float y, const Color& color) {
		if (this->DefaultFont == null) return;
		if (color.A == 0) return;

		this->Text(this->DefaultFont, text, x, y, color);
	}

	void Render::SetTexture(GLint handle) {
		if (this->CurrentTexture != handle) this->DrawOnScreen();
		this->CurrentTexture = handle;
	}

	void Render::SetTexture(Texture& tex) {
		if (!tex.RegisteredInGL) {
			tex.BindGL();
			tex.Upload();
		}

		if (this->CurrentTexture != tex.GLHandle) this->DrawOnScreen();
		this->CurrentTexture = tex.GLHandle;
	}

	void Render::SetShader(GLuint handle) {
		if (this->CurrentShader != handle) this->DrawOnScreen();
		this->CurrentShader = handle;
	}

	void Render::SetShader(const Shader& shader) {
		if (this->CurrentShader != shader.ProgramHandle) this->DrawOnScreen();
		this->CurrentShader = shader.ProgramHandle;
	}

	void Render::Text(Font* font, const std::string& text, int x, int y, const Color& color) {
		this->Text(font, text, (float)x, (float)y, color);
	}

	void Render::Text(Font* font, const std::string& text, float x, float y, const Color& color) {
		if (color.A == 0) return;
		this->SetTexture(font->Atlas->id);
		this->SetShader(this->DefaultShaderText.ProgramHandle);

		x += this->_DrawOffset.X;
		y += this->_DrawOffset.Y;

		y -= font->FontHandle->height / 4;

		float cx = x;
		float cy = y;
		int len = (int)text.size();

		this->CheckSpace(len * 4, len * 6); // per square, 4 vertices and 6 indices

		float currentMaxLineHeight = 0;
		for (int i = 0; i < len; i++) {
			char l = text[i];

			texture_glyph_t* glyph = texture_font_get_glyph(font->FontHandle, (wchar_t)text[i]);
			if (glyph == null) {
				TL_ASSERT(false);
				return;
			}

			if (i > 0) {
				cx += texture_glyph_get_kerning(glyph, text[i - 1]);
			}

			if (l == '\n') {
				cy += font->FontHandle->height;
				cx = x;
				continue;
			}

			float w = (float)glyph->width;
			float h = (float)font->FontHandle->height;

			_vertexData a, b, c, d;

			float lw = cx + glyph->offset_x;
			float lh = cy + h - glyph->offset_y;

			float x1 = glyph->s0;
			float x2 = glyph->s1;
			float y1 = glyph->t0;
			float y2 = glyph->t1;

			float pixelWidth = 1.0f / this->DefaultFont->Atlas->width;
			float pixelHeight = 1.0f / this->DefaultFont->Atlas->height;

			a.Location.X = lw;
			a.Location.Y = lh;

			b.Location.X = lw + w;
			b.Location.Y = lh;

			c.Location.X = lw;
			c.Location.Y = lh + glyph->height;

			d.Location.X = lw + w;
			d.Location.Y = lh + glyph->height;

			a.TextureLocation.X = x1;
			a.TextureLocation.Y = y1;

			b.TextureLocation.X = x2;
			b.TextureLocation.Y = y1;

			c.TextureLocation.X = x1;
			c.TextureLocation.Y = y2;

			d.TextureLocation.X = x2;
			d.TextureLocation.Y = y2;


			if (this->ClippingEnabled) {
				if (lw < this->_ClippingPos.X) {
					// if the sign is fully outside of the clipping, then just skip it.
					if (lw + w < this->_ClippingPos.X) {
						cx += glyph->advance_x;
						continue;
					} else {
						// calculate the new screen and texture location
						float offset = this->_ClippingPos.X - lw;

						a.Location.X += offset;
						c.Location.X += offset;

						a.TextureLocation.X += offset * pixelWidth;
						c.TextureLocation.X += offset * pixelWidth;
					}
				}

				if (lw + w > this->_ClippingPos.X + this->_ClippingSize.X) {
					if (cx > this->_ClippingPos.X + this->_ClippingSize.X) {
						cx += glyph->advance_x;
						continue;
					} else {
						float offset = lw + w - this->_ClippingPos.X - this->_ClippingSize.X;

						b.Location.X -= offset;
						d.Location.X -= offset;

						b.TextureLocation.X -= offset * pixelWidth;
						d.TextureLocation.X -= offset * pixelWidth;
					}
				}

				if (a.Location.Y < this->_ClippingPos.Y) {
					if (d.Location.Y < this->_ClippingPos.Y) {
						cx += glyph->advance_x;
						continue;
					} else {
						float offset = this->_ClippingPos.Y - a.Location.Y;

						a.Location.Y += offset;
						b.Location.Y += offset;

						a.TextureLocation.Y += offset * pixelHeight;
						b.TextureLocation.Y += offset * pixelHeight;
					}
				}

				if (d.Location.Y > this->_ClippingPos.Y + this->_ClippingSize.Y) {
					if (b.Location.Y > this->_ClippingPos.Y + this->_ClippingSize.Y) {
						cx += glyph->advance_x;
						continue;
					} else {
						float offset = d.Location.Y - this->_ClippingPos.Y - this->_ClippingSize.Y;

						c.Location.Y -= offset;
						d.Location.Y -= offset;

						c.TextureLocation.Y -= offset * pixelHeight;
						d.TextureLocation.Y -= offset * pixelHeight;
					}
				}
			}

			a.Color = color;
			b.Color = color;
			c.Color = color;
			d.Color = color;

			unsigned int curVertices = this->VerticeDataCount;

			VERTICE_PUSH(a);
			VERTICE_PUSH(b);
			VERTICE_PUSH(c);
			VERTICE_PUSH(d);

			INDICE_PUSH(curVertices + 0);
			INDICE_PUSH(curVertices + 1);
			INDICE_PUSH(curVertices + 2);

			INDICE_PUSH(curVertices + 1);
			INDICE_PUSH(curVertices + 3);
			INDICE_PUSH(curVertices + 2);

			cx += glyph->advance_x;
		}
	}

	void Render::EnableClipping(int x, int y, int w, int h) {
		AABB newclip((float)x, (float)y, (float)w, (float)h);

		if (this->ClippingEnabled) {
			if (newclip.X + newclip.W < this->_ClippingPos.X) newclip = AABB(this->_ClippingPos.X, this->_ClippingPos.Y, 0.0f, 0.0f);
			if (newclip.Y + newclip.H < this->_ClippingPos.Y) newclip = AABB(this->_ClippingPos.X, this->_ClippingPos.Y, 0.0f, 0.0f);
			if (newclip.X > this->_ClippingPos.X + this->_ClippingSize.X) newclip = AABB(this->_ClippingPos.X, this->_ClippingPos.Y, 0.0f, 0.0f);
			if (newclip.Y > this->_ClippingPos.Y + this->_ClippingSize.Y) newclip = AABB(this->_ClippingPos.X, this->_ClippingPos.Y, 0.0f, 0.0f);

			if (newclip.X < this->_ClippingPos.X) { newclip.W -= this->_ClippingPos.X - (float)fabs(newclip.X); newclip.X = this->_ClippingPos.X; }
			if (newclip.Y < this->_ClippingPos.Y) { newclip.H -= this->_ClippingPos.Y - (float)fabs(newclip.Y); newclip.Y = this->_ClippingPos.Y; }

			if (newclip.X + newclip.W > this->_ClippingPos.X + this->_ClippingSize.X) newclip.W = this->_ClippingPos.X + this->_ClippingSize.X - newclip.X;
			if (newclip.Y + newclip.H > this->_ClippingPos.Y + this->_ClippingSize.Y) newclip.H = this->_ClippingPos.Y + this->_ClippingSize.Y - newclip.Y;
		}

		this->_ClippingPos = Vector2(newclip.X, newclip.Y);
		this->_ClippingSize = Vector2(newclip.W, newclip.H);
		this->_ClippingList.Add(newclip);
		this->ClippingEnabled = true;
	}

	void Render::DisableClipping() {
		if (this->_ClippingList.Count == 0) return;

		this->_ClippingList.RemoveAt(this->_ClippingList.Count - 1);
		this->ClippingEnabled = this->_ClippingList.Count > 0;

		if (this->ClippingEnabled) {
			AABB newclip = this->_ClippingList[this->_ClippingList.Count - 1];

			this->_ClippingPos = Vector2(newclip.X, newclip.Y);
			this->_ClippingSize = Vector2(newclip.W, newclip.H);
		}
	}

	void Render::RecorderStart() {
		if (this->CaptureBuffer != null) throw("Already capturing!");

		this->DrawOnScreen();
		this->CaptureBuffer = new RenderBuffer();
	}

	RenderBuffer* Render::RecorderStop() {
		if (this->CaptureBuffer == null) throw("Not capturing!");

		this->DrawOnScreen();
		RenderBuffer* tmp = this->CaptureBuffer;
		this->CaptureBuffer = null;

		return tmp;
	}

	void Render::EnableFlipping(FlipMode mode, const Vector2 &center) {
		this->FlippingMode = mode;
		this->FlippingCenter = center;
		this->FlippingStart = this->VerticeDataCount;
	}

	void Render::DisableFlipping() {
		for (unsigned int i = this->FlippingStart; i < this->VerticeDataCount; i++) {
			_vertexData& vd = this->VerticeData[i];

			if (this->FlippingMode == FlipMode::Vertical || this->FlippingMode == FlipMode::Both) vd.Location.X = (this->FlippingCenter.X + this->_DrawOffset.X) + ((this->FlippingCenter.X + this->_DrawOffset.X) - vd.Location.X);
			if (this->FlippingMode == FlipMode::Horizontal || this->FlippingMode == FlipMode::Both) vd.Location.Y = (this->FlippingCenter.Y + this->_DrawOffset.Y) + ((this->FlippingCenter.Y + this->_DrawOffset.Y) - vd.Location.Y);
		}

		this->FlippingMode = FlipMode::None;
	}

	void Render::SetDrawingOffset(int x, int y) {
		this->_DrawOffset = Vector2(x, y);
	}

	Vector2 Render::GetDrawingOffset() {
		return this->_DrawOffset;
	}

	void Render::CheckSpace(int vcount, int icount) {
		int bufferincreese = 512; // don't waste CPU on ading 4 more vertices every draw call
		if (bufferincreese < vcount) bufferincreese = vcount;
		if (bufferincreese < icount) bufferincreese = icount;

		if (this->VerticeDataCount + vcount >= this->VerticeDataSize) {
			_vertexData* tmp = new _vertexData[this->VerticeDataSize + bufferincreese];

			if (this->VerticeData != null) {
				memcpy(tmp, this->VerticeData, this->VerticeDataCount * sizeof(_vertexData));
				delete[] this->VerticeData;
			}

			this->VerticeData = tmp;
			this->VerticeDataSize += bufferincreese;
		}

		if (this->IndiceDataCount + icount >= this->IndiceDataSize) {
			GLuint* tmp = new GLuint[this->IndiceDataSize + bufferincreese];

			if (this->IndiceData != null) {
				memcpy(tmp, this->IndiceData, this->IndiceDataCount * sizeof(GLuint));
				delete[] this->IndiceData;
			}

			this->IndiceData = tmp;
			this->IndiceDataSize += bufferincreese;
		}
	}

	void Render::DrawOnScreen() {
		if (this->VerticeDataCount == 0) return;

		glDisable(GL_DEPTH_TEST);

		glUseProgram(this->CurrentShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->CurrentTexture);

		if (this->CaptureBuffer != null) {
			RenderBufferChunk c;
			c.ShaderHandle = this->CurrentShader;
			c.TextureHandle = this->CurrentTexture;

			c.VerticeDataCount = this->VerticeDataCount;
			c.VerticeData = new _vertexData[c.VerticeDataCount];
			memcpy(c.VerticeData, this->VerticeData, c.VerticeDataCount * sizeof(_vertexData));

			c.IndiceDataCount = this->IndiceDataCount;
			c.IndiceData = new GLuint[c.IndiceDataCount];
			memcpy(c.IndiceData, this->IndiceData, c.IndiceDataCount * sizeof(GLuint));

			this->CaptureBuffer->Chunks.Add(c);
		}

		unsigned long count = this->VerticeDataCount;
		float* vertices = new float[count * 9];

		int offset = 0;
		for (unsigned long i = 0; i < count; i++) {
			// X and Y
			vertices[offset++] = this->VerticeData[i].Location.X / this->ScreenSize.X * 2 - 1;
			vertices[offset++] = (this->VerticeData[i].Location.Y / this->ScreenSize.Y * 2 - 1) * -1;
			vertices[offset++] = 0; // z

			// UI texture location (for text/texture rendering)
			vertices[offset++] = this->VerticeData[i].TextureLocation.X;
			vertices[offset++] = this->VerticeData[i].TextureLocation.Y;

			// colors, R,G,B,A, convert 256 to 1 because that's how openGL rolls.
			vertices[offset++] = this->VerticeData[i].Color.R / 255.0f;
			vertices[offset++] = this->VerticeData[i].Color.G / 255.0f;
			vertices[offset++] = this->VerticeData[i].Color.B / 255.0f;
			vertices[offset++] = this->VerticeData[i].Color.A / 255.0f;
		}

		glBindVertexArray(this->vao);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 9, vertices, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->IndiceDataCount * sizeof(GLuint), this->IndiceData, GL_STREAM_DRAW);

		glDrawElements(GL_TRIANGLES, this->IndiceDataCount, GL_UNSIGNED_INT, 0);

		delete[] vertices;

		glEnable(GL_DEPTH_TEST);

		this->VerticeDataCount = 0;
		this->IndiceDataCount = 0;
	}
}
