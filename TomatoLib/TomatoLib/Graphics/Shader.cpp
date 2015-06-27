#include "Shader.h"
#include "../Defines.h"

namespace TomatoLib {
	Shader::Shader() {
		this->ProgramHandle = glCreateProgram();
	}

	Shader::~Shader() {
		checkGL;
		for (int i = 0; i < this->Attached.Count; i++) {
			glDeleteShader(this->Attached[i]);
			checkGL;
		}

		checkGL;
		glDeleteProgram(this->ProgramHandle);
		checkGL;
	}

	void Shader::Attach(std::string file, GLint mode) {
		checkGL;

		//Handle to the file to read from
		FILE* filePointer;
		filePointer = fopen(file.c_str(), "rb");

		//Find file size
		fseek(filePointer, 0L, SEEK_END);
		long fileSize = ftell(filePointer);
		rewind(filePointer);

		//Allocate space for file data
		char* fileData = new char[fileSize + 1];
		fread(fileData, 1, (size_t)fileSize, filePointer);
		fileData[fileSize] = 0;

		//Cast to a const char for the gl function
		const char* fileDataConst = (const char*)fileData;

		//Create new shader, set the source, and compile it
		GLuint handle = glCreateShader(mode);
		glShaderSource(handle, 1, &fileDataConst, 0);
		glCompileShader(handle);

		fclose(filePointer);

		delete[] fileData;

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

			throw log;
		}

		glAttachShader(this->ProgramHandle, handle);

		this->Attached.Add(handle);

		checkGL;
	}

	void Shader::AttachRaw(std::string text, GLint mode) {
		checkGL;

		//Create new shader, set the source, and compile it
		GLuint handle = glCreateShader(mode);
		const char* str = text.c_str();
		glShaderSource(handle, 1, &(const GLchar*)str, 0);
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

			throw log.c_str();
		}

		glAttachShader(this->ProgramHandle, handle);

		this->Attached.Add(handle);

		checkGL;
	}

	void Shader::Use() {
		glUseProgram(this->ProgramHandle);
	}

	void Shader::Link() {
		glLinkProgram(this->ProgramHandle);
	}
}