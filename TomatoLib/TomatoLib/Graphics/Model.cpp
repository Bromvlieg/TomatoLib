#include "Model.h"
#include "../Game/Camera.h"
#include "../Defines.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace TomatoLib {
	GLint Model::s_projMatrixLocation = -1;
	GLint Model::s_viewMatrixLocation = -1;
	GLint Model::s_worldMatrixLocation = -1;
	Shader* Model::s_Shader = nullptr;
	Matrix Model::s_mProj;

	void RawModelData::GetBoundingBox(Vector3& min, Vector3& max) {
		min.X = 99999;
		min.Y = 99999;
		min.Z = 99999;
		max.X = -99999;
		max.Y = -99999;
		max.Z = -99999;

		for (int i = 0; i < this->VerticeCount; i++) {
			vertex_t& v = this->VerticePtr[i];

			if (v.Pos.X < min.X) min.X = v.Pos.X;
			if (v.Pos.Y < min.Y) min.Y = v.Pos.Y;
			if (v.Pos.Z < min.Z) min.Z = v.Pos.Z;

			if (v.Pos.X > max.X) max.X = v.Pos.X;
			if (v.Pos.Y > max.Y) max.Y = v.Pos.Y;
			if (v.Pos.Z > max.Z) max.Z = v.Pos.Z;
		}
	}

	Model::Model() : m_IndiceCount(0) {
		checkGL;

		if (s_Shader == nullptr) {
			s_Shader = new Shader();
			// init shader for first time use
			checkGL;

			s_Shader->AttachRaw("#version 150\
							\n	\
							\n	in vec3 position;\
							\n	in vec2 texpos;\
							\n	out vec3 pos;\
							\n	out vec2 tpos;\
							\n	uniform mat4 proj;\
							\n	uniform mat4 view;\
							\n	uniform mat4 world;\
							\n	\
							\n	void main() {\
							\n		vec4 translated = world * vec4(position, 1.0);\
							\n		gl_Position = proj * view * vec4(translated.xzy, 1.0);\
							\n		tpos = texpos;\
							\n	}\
							\n	", GL_VERTEX_SHADER);

			s_Shader->AttachRaw("#version 150\
							\n	\
							\n	out vec4 outColor;\
							\n in vec2 tpos;\
							\n uniform sampler2D tex;\
							\n	\
							\n	void main() {\
							\n		outColor = texture(tex, tpos);\
							\n		if (outColor.a == 0.0) discard;\
							\n	}\
			", GL_FRAGMENT_SHADER);

			s_Shader->Link();
			s_Shader->Use();

			glBindFragDataLocation(s_Shader->ProgramHandle, 0, "outColor");
			checkGL;

			s_viewMatrixLocation = glGetUniformLocation(s_Shader->ProgramHandle, "view");
			s_projMatrixLocation = glGetUniformLocation(s_Shader->ProgramHandle, "proj");
			s_worldMatrixLocation = glGetUniformLocation(s_Shader->ProgramHandle, "world");
			checkGL;
		}

		this->m_ShaderHandle = s_Shader->ProgramHandle;
		s_Shader->Use();

		// Create Vertex Array Object
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);
		checkGL;

		// Create a Vertex Buffer Object and copy the vertex data to it
		glGenBuffers(1, &this->vbo);
		glGenBuffers(1, &this->ebo);
		checkGL;

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		checkGL;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
		checkGL;

		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(s_Shader->ProgramHandle, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, Pos));
		checkGL;

		// Specify the layout of the vertex data
		GLint texposAttrib = glGetAttribLocation(s_Shader->ProgramHandle, "texpos");
		glEnableVertexAttribArray(texposAttrib);
		glVertexAttribPointer(texposAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, UV));
		checkGL;

		glUniformMatrix4fv(s_projMatrixLocation, 1, GL_TRUE, s_mProj.values);
		glUniform1i(glGetUniformLocation(s_Shader->ProgramHandle, "tex"), 0);
		checkGL;
	}

	void Model::BindBuffers() {
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	}

	void Model::SetTexture(Texture& tex) {
		if (!tex.RegisteredInGL) {
			tex.BindGL();
			tex.Upload();
		}

		this->m_TextureHandle = tex.GLHandle;
	}

	void Model::SetTexture(GLuint t) {
		this->m_TextureHandle = t;
	}

	void Model::SetShader(const Shader& s) {
		this->m_ShaderHandle = s.ProgramHandle;
	}

	void Model::SetVertices(vertex_t* verts, int count) {
		checkGL;
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(vertex_t), verts, GL_STATIC_DRAW);
		checkGL;
	}

	void Model::SetIndices(GLushort* inds, int count) {
		checkGL;
		glBindBuffer(GL_ARRAY_BUFFER, this->ebo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLushort), inds, GL_STATIC_DRAW);
		checkGL;

		this->m_IndiceCount = count;
	}

	void Model::SetMatrix(const Matrix& m) {
		this->m_Matrix = m;
	}

	Matrix Model::GetMatrix() {
		return this->m_Matrix;
	}

	GLuint Model::GetTexture() {
		return this->m_TextureHandle;
	}

	void Model::Draw(const Camera& cam) {
		if (this->m_IndiceCount == 0) return;

		checkGL;
		glUseProgram(this->m_ShaderHandle);

		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

		if (this->m_ShaderHandle == s_Shader->ProgramHandle) {
			glBindTexture(GL_TEXTURE_2D, this->m_TextureHandle);

			cam.InsertViewMatrix(s_viewMatrixLocation);
			glUniformMatrix4fv(s_worldMatrixLocation, 1, GL_TRUE, this->m_Matrix.values);
		}

		glDrawElements(GL_TRIANGLES, this->m_IndiceCount, GL_UNSIGNED_SHORT, 0);
		checkGL;
	}

	void Model::FromSquare(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d) {
		vertex_t buff[4];
		buff[0].Pos = a;
		buff[1].Pos = b;
		buff[2].Pos = c;
		buff[3].Pos = d;

		buff[0].UV = Vector2::Zero;
		buff[1].UV = Vector2(1, 0);
		buff[2].UV = Vector2(0, 1);
		buff[3].UV = Vector2::One;

		const static GLushort inds[6] = {0, 1, 2, 1, 2, 3};

		this->SetVertices(buff, 4);
		this->SetIndices(const_cast<GLushort*>(inds), 6);
	}

	inline Vector3 F(float u, float v) {
		return Vector3(cos(u)*sin(v), cos(v), sin(u)*sin(v));
	}

	void Model::FromSphere(int rings, int sectors, float radius) {
		float const R = 1 / (float)(rings - 1);
		float const S = 1 / (float)(sectors - 1);
		int r, s;

		List<vertex_t> vertices;
		List<GLfloat> normals;
		List<GLfloat> texcoords;
		List<GLushort> indices;

		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);

		List<vertex_t>::iterator v = vertices.begin();
		List<GLfloat>::iterator n = normals.begin();

		int i = 0;
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = (float)sin(-M_PI_2 + M_PI * r * R);
			float const x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
			float const z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

			vertex_t& v = vertices[i];
			v.Pos.X = x * radius;
			v.Pos.Y = y * radius;
			v.Pos.Z = z * radius;

			v.UV.X = s*S;
			v.UV.Y = r*R;

			*n++ = x;
			*n++ = y;
			*n++ = z;

			i++;
		}

		indices.resize(rings * sectors * 6);
		List<GLushort>::iterator ii = indices.begin();
		const static GLushort inds[6] = {0, 1, 2, 1, 2, 3};
		for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
			GLushort a = r * sectors + s;
			GLushort b = r * sectors + s + 1;
			GLushort c = (r + 1) * sectors + s;
			GLushort d = (r + 1) * sectors + s + 1;

			*ii++ = a;
			*ii++ = b;
			*ii++ = c;

			*ii++ = b;
			*ii++ = c;
			*ii++ = d;
		}

		this->SetVertices(&vertices[0], vertices.size());
		this->SetIndices(&indices[0], indices.size());
	}

	void Model::FromCircle(int points, float radius) {
		vertex_t* vbuff = new vertex_t[points];
		GLushort* ibuff = new GLushort[points * 3 - 3];

		float step = (float)(M_PI * 2 / points);
		float cur = 0;
		for (int i = 0; i < points; i++) {
			float x = sin(cur);
			float y = cos(cur);

			vbuff[i].UV = Vector2(x, y) / 2.0f + 0.5f;
			vbuff[i].Pos = Vector3(x * radius, y * radius, 0);


			cur += step;
		}

		for (int i = 0; i < points - 1; i++) {
			ibuff[i * 3 + 0] = 0;
			ibuff[i * 3 + 1] = i;
			ibuff[i * 3 + 2] = i + 1;
		}

		this->SetVertices(vbuff, points);
		this->SetIndices(ibuff, points * 3 - 3);

		delete[] vbuff;
		delete[] ibuff;
	}

	void Model::FromTube(int points, const Vector3& size) {
		vertex_t* vbuff = new vertex_t[points * 2];
		GLushort* ibuff = new GLushort[points * 12];

		float step = (float)(M_PI * 2 / (points - 1));
		float cur = 0;
		for (int i = 0; i < points; i++) {
			float x = sin(cur);
			float y = cos(cur);

			vbuff[i * 2].UV = Vector2(1.0f, cur / ((float)M_PI * 2));
			vbuff[i * 2].Pos = Vector3(x * size.X, y * size.Y, size.Z);

			vbuff[i * 2 + 1].UV = Vector2(0.0f, cur / ((float)M_PI * 2));
			vbuff[i * 2 + 1].Pos = Vector3(x * size.X, y * size.Y, -size.Z);

			cur += step;
		}

		for (int i = 0; i < points * 2 - 3; i++) {
			ibuff[i * 6 + 0] = i;
			ibuff[i * 6 + 1] = i + 1;
			ibuff[i * 6 + 2] = i + 2;

			ibuff[i * 6 + 3] = i + 1;
			ibuff[i * 6 + 4] = i + 2;
			ibuff[i * 6 + 5] = i + 3;
		}

		this->SetVertices(vbuff, points * 2);
		this->SetIndices(ibuff, points * 12);

		delete[] vbuff;
		delete[] ibuff;
	}
}