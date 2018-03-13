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

#ifndef TL_ENABLE_EGL
#include <GL/glew.h>
#else
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

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

		void GetBoundingBox(Vector3& min, Vector3& max);
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
		Matrix& GetMatrix();

		void SetShader(const Shader& s);
		void SetTexture(Texture& tex);
		void SetTexture(GLuint tex);

		GLuint GetTexture();

		void SetVertices(vertex_t* verts, int count);
		void SetIndices(GLushort* inds, int count);

		void Draw(const Camera& cam);

		void BindBuffers();

		void FromSquare(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);
		void FromCircle(int points, float radius);
		void FromSphere(int upoints, int vpoints, float radius);
		void FromTube(int points, const Vector3& size);
	};
}

#endif
