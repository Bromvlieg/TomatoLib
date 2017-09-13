#ifndef _TOMATOLIB_CONTENT
#define _TOMATOLIB_CONTENT

#include <string>
#include "../Utilities/Utilities.h"

namespace TomatoLib {
	struct RawModelData;
	class Font;
	class Texture;
	class Shader;

	namespace Content {
		typedef void(*ShaderReloadCallbackFunc)(TomatoLib::Shader& s);

		struct ShaderReloadCallbackData {
			ShaderReloadCallbackFunc Func;
			bool ShouldCall;
		};

		// use asset names, so without the extention
		TomatoLib::Texture& GetTexture(const std::string& path);
		TomatoLib::RawModelData& GetModel(const std::string& path);
		TomatoLib::Font& GetFont(const std::string& path, float size);
		TomatoLib::Shader& GetShader(const std::string& path);

		void Unload();
		void Init();

		void AddShaderReloadFunc(const std::string& path, ShaderReloadCallbackFunc func);

		// note: files starting with a period will not be loaded as content.
		// You can use this to optionaly load them before or afterwards with Preload to prioritize content
		// content files won't be loaded twice.
		void LoadFolder(const std::string& path, int& forasyncprogressreading, char* forasyncprogresscurfile);
		int CountFolder(const std::string& path);

		// asset name, not filename
		bool HasLoaded(const std::string& path);

		bool Preload(const std::string& path);
	}
}

#endif