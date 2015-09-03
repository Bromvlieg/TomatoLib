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

		// TODO: fix this projection thingy
		Matrix proj = Matrix::CreatePerspective(float(M_PI_2), 1920.0f / 1080.0f, 0.001f, 100000.0f);

		// Create Vertex Array Object
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);
		checkGL;

		// Create a Vertex Buffer Object and copy the vertex data to it
		glGenBuffers(1, &this->vbo);
		glGenBuffers(1, &this->ebo);
		checkGL;

		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		//glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
		checkGL;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
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

		glUniformMatrix4fv(s_projMatrixLocation, 1, GL_FALSE, proj.values);
		glUniform1i(glGetUniformLocation(s_Shader->ProgramHandle, "tex"), 0);
		checkGL;
	}

	void Model::SetTexture(Texture& tex) {
		if (!tex.RegisteredInGL) {
			tex.BindGL();
			tex.Upload();
		}

		this->m_TextureHandle = tex.GLHandle;
	}

	void Model::SetShader(Shader& s) {
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

	void Model::Draw(const Camera& cam) {
		if (this->m_IndiceCount == 0) return;

		checkGL;
		s_Shader->Use();

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

	Model* Model::FromSquare(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d) {
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

		Model* mdl = new Model();
		mdl->SetVertices(buff, 4);
		mdl->SetIndices(const_cast<GLushort*>(inds), 6);

		return mdl;
	}
}