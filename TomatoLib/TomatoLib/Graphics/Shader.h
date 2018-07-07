#pragma once
#ifndef __SAHDER_H__
#define __SAHDER_H__

#include "../Utilities/List.h"

#include <string>
#include <GL/glew.h>

namespace TomatoLib {
	class Vector2;
	class Vector3;
	class Matrix;

	template<typename T>
	class Uniform {
	private:
		GLint m_GLiIndex;

	public:
		void Set(const T& value);

		Uniform(const GLint& index) {
			this->m_GLiIndex = index;
		}

		Uniform() {
			this->m_GLiIndex = -1;
		}
	};

	class Shader {
	public:
		template<typename T>
		Uniform<T> GetUniform(const std::string& sName) {
			return Uniform<T>(glGetUniformLocation(this->ProgramHandle, sName.c_str()));
		}

		GLint ProgramHandle;
		List<GLint> Attached;

		Shader();
		~Shader();

		void Use();
		void Link();
		bool AttachRaw(std::string text, GLint mode);
		bool Attach(std::string file, GLint mode);
		void Cleanup();

		// Setting vectors
		void SetUniform(std::string sName, TomatoLib::Vector2* vVectors, int iCount = 1);
		void SetUniform(std::string sName, const TomatoLib::Vector2& vVector);
		void SetUniform(std::string sName, TomatoLib::Vector3* vVectors, int iCount = 1);
		void SetUniform(std::string sName, const TomatoLib::Vector3& vVector);

		// Setting floats
		void SetUniform(std::string sName, float* fValues, int iCount = 1);
		void SetUniform(std::string sName, const float fValue);

		// Setting 4x4 matrices
		void SetUniform(std::string sName, const TomatoLib::Matrix& mMatrix);

		// Setting integers
		void SetUniform(std::string sName, int* iValues, int iCount = 1);
		void SetUniform(std::string sName, const int iValue);
	};
}
#endif