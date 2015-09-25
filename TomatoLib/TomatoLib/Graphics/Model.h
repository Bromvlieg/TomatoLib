#pragma once
#ifndef __TL_MODEL__
#define __TL_MODEL__

#include <vector>
#include "Color.h"
#include "../Math/Matrix.h"
#include "../Math/Vector3.h"
#include "../Math/Vector2.h"
#include "Texture.h"
#include "Shader.h"
#include <GL/glew.h>

namespace TomatoLib {
	class Camera;

	struct vertex_t {
		Vector3 Pos;
		Vector2 UV;
	};

	struct RawModelData {
		vertex_t* VerticePtr;
		GLushort* IndicePtr;

		int VerticeCount;
		int IndiceCount;

		TomatoLib::Texture Texture;
	};

	class Model {
		Matrix m_Matrix;
		GLuint m_TextureHandle;
		GLint m_ShaderHandle;

		GLuint vao;
		GLuint vbo;
		GLuint ebo;

		int m_IndiceCount;

		static GLint s_viewMatrixLocation;
		static GLint s_projMatrixLocation;
		static GLint s_worldMatrixLocation;
		static Shader* s_Shader;
	public:
		static Matrix s_mProj;

		Model();

		void SetMatrix(const Matrix& m);
		Matrix GetMatrix();

		void SetShader(Shader& s);
		void SetTexture(Texture& tex);
		void SetTexture(GLuint tex);

		GLuint GetTexture();

		void SetVertices(vertex_t* verts, int count);
		void SetIndices(GLushort* inds, int count);

		void Draw(const Camera& cam);

		void BindBuffers();

		void FromSquare(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);
		void FromCircle(int points, float radius);
		void FromTube(int points, const Vector3& size);
	};
}

#endif
