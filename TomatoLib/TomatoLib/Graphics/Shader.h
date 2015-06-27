#pragma once
#ifndef __SAHDER_H__
#define __SAHDER_H__

#include "../Utilities/List.h"

#include <string>
#include <GL/glew.h>

namespace TomatoLib {
	class Shader {
	public:
		GLint ProgramHandle;
		List<GLint> Attached;

		Shader();
		~Shader();

		void Use();
		void Link();
		void AttachRaw(std::string text, GLint mode);
		void Attach(std::string file, GLint mode);
	};
}
#endif