#include "Shader.h"
#include "../Defines.h"

#include <iostream>
#include <fstream>

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

		std::ifstream infile;
		infile.open( file, std::ifstream::binary );

		if( !infile.is_open() )
		{
			std::cout << "Shader::Attach() failed! Could not open file named: " << file << std::endl;
			infile.close();
			return;
		}

		infile.seekg( 0, std::ios::end );
		size_t file_size_in_byte = (size_t)infile.tellg();

		std::vector<char> data; // used to store text data
		data.resize( file_size_in_byte );
		infile.seekg( 0, std::ios::beg );

		infile.read( &data[ 0 ], file_size_in_byte );

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
