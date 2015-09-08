#include "Shader.h"
#include "../Defines.h"

#include <iostream>
#include <fstream>

namespace TomatoLib {
	Shader::Shader() {
		this->ProgramHandle = -1;
	}

	Shader::~Shader() {
		this->Cleanup();
	}

	void Shader::Cleanup() {
		checkGL;
		for (int i = 0; i < this->Attached.Count; i++) {
			glDeleteShader(this->Attached[i]);
			checkGL;
		}

		glDeleteProgram(this->ProgramHandle);
		checkGL;

		this->ProgramHandle = -1;
		this->Attached.Clear();
	}

	void Shader::Attach(std::string file, GLint mode) {
		if (this->ProgramHandle == -1) {
			this->ProgramHandle = glCreateProgram();
		}

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
	}

	void Shader::AttachRaw(std::string text, GLint mode) {
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
