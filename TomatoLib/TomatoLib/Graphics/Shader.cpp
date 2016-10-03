#include "Shader.h"
#include "../Defines.h"

#include <iostream>
#include <fstream>

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix.h"

namespace TomatoLib {
	Shader::Shader() {
		this->ProgramHandle = -1;
	}

	Shader::~Shader() {
		this->Cleanup();
	}

	void Shader::Cleanup() {
#ifndef TL_OPENGL_OLD
		checkGL;
		if (this->ProgramHandle == -1) return;

		for (int i = 0; i < this->Attached.Count; i++) {
			glDeleteShader(this->Attached[i]);
			checkGL;
		}

		glDeleteProgram(this->ProgramHandle);
		checkGL;

		this->ProgramHandle = -1;
		this->Attached.Clear();
#endif
	}

	bool Shader::Attach(std::string file, GLint mode) {
#ifndef TL_OPENGL_OLD
		checkGL;

		std::ifstream infile;
		infile.open( file, std::ifstream::binary );

		if( !infile.is_open() )
		{
			infile.close();
			return false;
		}

		if (this->ProgramHandle == -1) {
			this->ProgramHandle = glCreateProgram();
		}

		infile.seekg( 0, std::ios::end );
		size_t file_size_in_byte = (size_t)infile.tellg();

		std::vector<char> data; // used to store text data
		data.resize( file_size_in_byte + 1 );
		infile.seekg( 0, std::ios::beg );

		infile.read( &data[ 0 ], file_size_in_byte );
		data[file_size_in_byte] = 0;

		//Cast to a const char for the gl function
		const char* fileDataConst = (const char*) &data[0];

		//Create new shader, set the source, and compile it
		GLuint handle = glCreateShader(mode);

		glShaderSource(handle, 1, &fileDataConst, 0);
		glCompileShader(handle);

		infile.close();
		
		//Check if compile is succesfull
		GLint isSuccesfullyCompiled = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &isSuccesfullyCompiled);
		if (isSuccesfullyCompiled == GL_FALSE) {
			//Get info log length
			GLint maxLength = 0;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

			char* buff = new char[maxLength + 1];
			buff[maxLength] = 0;

			//Allocate data for log
			glGetShaderInfoLog(handle, maxLength, &maxLength, (GLchar*)buff);

			glDeleteShader(handle);
			
			std::string log = buff;
			delete[] buff;

			throw log;
		}

		glAttachShader(this->ProgramHandle, handle);

		this->Attached.Add(handle);

		checkGL;
#endif
		return true;
	}

	void Shader::AttachRaw(std::string text, GLint mode) {
#ifndef TL_OPENGL_OLD
		checkGL;

		if (this->ProgramHandle == -1) {
			this->ProgramHandle = glCreateProgram();
		}

		checkGL;

		//Create new shader, set the source, and compile it
		GLuint handle = glCreateShader(mode);
		const char* str = text.c_str();
		glShaderSource(handle, 1, (const GLchar**)&str, 0);
		glCompileShader(handle);

		//Check if compile is succesfull
		GLint isSuccesfullyCompiled = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &isSuccesfullyCompiled);
		if (isSuccesfullyCompiled == GL_FALSE) {
			//Get info log length
			GLint maxLength = 0;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &maxLength);

			//Allocate data for log
			std::string log;
			log.reserve(maxLength);
			glGetShaderInfoLog(handle, maxLength, &maxLength, (GLchar*)log.c_str());

			glDeleteShader(handle);

			printf("Shader compile error: %s\n", log.c_str());
			throw log.c_str();
		}

		glAttachShader(this->ProgramHandle, handle);

		this->Attached.Add(handle);

		checkGL;
#endif
	}

	void Shader::Use() {
#ifndef TL_OPENGL_OLD
		glUseProgram(this->ProgramHandle);
#endif
	}

	void Shader::Link() {
#ifndef TL_OPENGL_OLD
		glLinkProgram(this->ProgramHandle);
#endif
	}// Setting floats

	void Shader::SetUniform(std::string sName, float* fValues, int iCount) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform1fv(iLoc, iCount, fValues);
#endif
	}

	void Shader::SetUniform(std::string sName, const float fValue) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform1fv(iLoc, 1, &fValue);
#endif
	}

	// Setting vectors

	void Shader::SetUniform(std::string sName, TomatoLib::Vector2* vVectors, int iCount) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
#endif
	}

	void Shader::SetUniform(std::string sName, const TomatoLib::Vector2& vVector) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
#endif
	}

	void Shader::SetUniform(std::string sName, TomatoLib::Vector3* vVectors, int iCount) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
#endif
	}

	void Shader::SetUniform(std::string sName, const TomatoLib::Vector3& vVector) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
#endif
	}

	void Shader::SetUniform(std::string sName, const TomatoLib::Matrix& mMatrix) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniformMatrix4fv(iLoc, 1, true, mMatrix.values);
#endif
	}

	// Setting integers

	void Shader::SetUniform(std::string sName, int* iValues, int iCount) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform1iv(iLoc, iCount, iValues);
#endif
	}

	void Shader::SetUniform(std::string sName, const int iValue) {
#ifndef TL_OPENGL_OLD
		int iLoc = glGetUniformLocation(this->ProgramHandle, sName.c_str());
		glUniform1i(iLoc, iValue);
#endif
	}

	// Uniforms!
	template<> void Uniform<int>::Set(const int& value) { glUniform1i(this->m_GLiIndex, value); }
	template<> void Uniform<float>::Set(const float& value) { glUniform1fv(this->m_GLiIndex, 1, (GLfloat*)&value); }
	template<> void Uniform<Vector2>::Set(const Vector2& value) { glUniform2fv(this->m_GLiIndex, 1, (GLfloat*)&value); }
	template<> void Uniform<Vector3>::Set(const Vector3& value) { glUniform3fv(this->m_GLiIndex, 1, (GLfloat*)&value); }
}
