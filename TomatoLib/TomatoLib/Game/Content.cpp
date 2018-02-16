#include "Content.h"
#include "../Graphics/Font.h"
#include "../Utilities/List.h"
#include "../Utilities/Dictonary.h"
#include "../Graphics/Model.h"
#include "../Graphics/Shader.h"
#include "../ModelFormats/Obj.h"
#include "../Async/Async.h"
#include "../Defines.h"


#if WINDOWS
#include "../Utilities/dirent.h"
#else
// For memcpy()
#include <string.h>
// On linux, dirent is a little different, where d_reclen is used instead of d_namlen.
#include <dirent.h>
#include <sys/stat.h>
#define d_namlen d_reclen
#endif

namespace TomatoLib {
	namespace Content {
		Dictonary<std::string, time_t> ScannedFiles;
		Dictonary<std::string, Texture*> TextureDatas;
		Dictonary<std::string, RawModelData> ModelDatas;
		Dictonary<std::string, Font*> FontDatas;
		Dictonary<std::string, Shader*> ShaderDatas;

		Dictonary<std::string, ShaderReloadCallbackData> ShaderCallbacks;

		void AddShaderReloadFunc(const std::string& path, ShaderReloadCallbackFunc func) {
			ShaderReloadCallbackData sd;
			sd.ShouldCall = true;
			sd.Func = func;

			ShaderCallbacks.Add(path, sd);
		}


		RawModelData& GetModel(const std::string& path) {
			if (!ModelDatas.ContainsKey(path)) {
				throw std::runtime_error("Model not found");
			}

			return ModelDatas[path];
		}

		Shader& GetShader(const std::string& path) {
			if (!ShaderDatas.ContainsKey(path)) {
				throw std::runtime_error("Shader not found");
			}

			Shader& ptr = *ShaderDatas[path];
			if (ShaderCallbacks.ContainsKey(path)) {
				ShaderReloadCallbackData& sd = ShaderCallbacks[path];
				if (sd.ShouldCall) {
					sd.Func(ptr);
					sd.ShouldCall = false;
				}
			}

			return ptr;
		}

		Texture& GetTexture(const std::string& path) {
			if (!TextureDatas.ContainsKey(path)) {
				throw std::runtime_error("Texture not found");
			}

			return *TextureDatas[path];
		}

		Font& GetFont(const std::string& path, float size) {
			if (!FontDatas.ContainsKey(path)) {
				throw std::runtime_error("Font not found");
			}

			char buff[256];
			sprintf(buff, "%s:%f", path.c_str(), size);

			if (!FontDatas.ContainsKey(buff)) {
				Font* f = FontDatas.Values[FontDatas.Add(buff, new Font())];
				bool ret = f->Load(path + ".ttf", size);

				return *f;
			}

			return *FontDatas[buff];
		}

		bool LoadFile(const std::string& path, bool forcenow = false) {
			if (path.find(".") == std::string::npos) return false;

			size_t i = path.find_last_of('.');
			std::string ext = path.c_str() + i + 1;

			char buff[256];
			memcpy(buff, path.c_str(), i);
			buff[i] = 0;

			std::string internalname = buff;

			FILE* htest = fopen(path.c_str(), "rb");
			if (htest == nullptr) {
				Utilities::Print("CONTENT: Could not open '%s' for reading", path.c_str());
				return false;
			}
			fclose(htest);

			if (ext == "obj") {
				FILE* objbinf = fopen((internalname + ".objbin").c_str(), "rb");
				if (objbinf != nullptr) {
					// parse read our more efficient data format here, rather then obj
					fclose(objbinf);
					return false;
				}

				ModelFormats::Obj m;
				m.FromFile(path.c_str());

				RawModelData rmd = m.GetRawData();

				ModelDatas.Add(internalname, rmd);
				Utilities::Print("CONTENT: Loaded '%s' as model", internalname.c_str());

				Utilities::Print("CONTENT: Compiling '%s'...", internalname.c_str());

				FILE* tmpf = fopen((internalname + ".objbin").c_str(), "wb");
				fwrite(&rmd.VerticeCount, 1, 4, tmpf);
				fwrite(&rmd.IndiceCount, 1, 4, tmpf);

				fwrite(rmd.VerticePtr, 1, rmd.VerticeCount * sizeof(vertex_t), tmpf);
				fwrite(rmd.IndicePtr, 1, rmd.IndiceCount * sizeof(GLshort), tmpf);

				fwrite(&rmd.Texture.Width, 1, 4, tmpf);
				fwrite(&rmd.Texture.Height, 1, 4, tmpf);

				fwrite(&rmd.Texture.PixelData[0], 1, rmd.Texture.PixelData.size(), tmpf);
				fclose(tmpf);
			} else if (ext == "ttf") {
				Font* f = new Font();
				Async::RunOnMainThread([internalname, f]() {
					FontDatas.Add(internalname, f);
				}, forcenow);

				Utilities::Print("CONTENT: Loaded '%s' as font", internalname.c_str());
			} else if (ext == "png") {
				Texture* t = new Texture(path.c_str());
				Async::RunOnMainThread([internalname, t]() {
					TextureDatas.Add(internalname, t);
				}, forcenow);

				Utilities::Print("CONTENT: Loaded '%s' as texture", internalname.c_str());
			} else if (ext == "sv") {
				FILE* objbinf = fopen((internalname + ".sf").c_str(), "rb");
				if (objbinf == nullptr) {
					Utilities::Print("CONTENT: Failed to load'%s' no matching .sf (fragment shader)", internalname.c_str());
					return false;
				}
				fclose(objbinf);

#ifndef TL_OPENGL_OLD
				Async::RunOnMainThread([internalname]() {
					Shader* sptr = new Shader();
					if (ShaderDatas.ContainsKey(internalname)) {
						delete ShaderDatas.Remove(internalname);
					}

					ShaderDatas.Add(internalname, sptr);

					try {
						sptr->Attach(internalname + ".sv", GL_VERTEX_SHADER);
						sptr->Attach(internalname + ".sf", GL_FRAGMENT_SHADER);
						sptr->Attach(internalname + ".sg", GL_GEOMETRY_SHADER);
					} catch (std::string err) {
						Utilities::Print("CONTENT: Failed to load '%s':\n%s", internalname.c_str(), err.c_str());

						return;
					}
					checkGL;

					if (ShaderCallbacks.ContainsKey(internalname)) {
						ShaderReloadCallbackData& sd = ShaderCallbacks[internalname];
						sd.Func(*sptr);
						sd.ShouldCall = false;
					}

					checkGL;

					ScannedFiles.Add(internalname, 0);
					Utilities::Print("CONTENT: Loaded '%s' as shader", internalname.c_str());
				}, forcenow);
#endif
				return true;
			} else if (ext == "objbin") {
				RawModelData rmd;

				FILE* tmpf = fopen(path.c_str(), "rb");
				fread(&rmd.VerticeCount, 1, 4, tmpf);
				fread(&rmd.IndiceCount, 1, 4, tmpf);

				rmd.VerticePtr = new vertex_t[rmd.VerticeCount];
				rmd.IndicePtr = new GLushort[rmd.IndiceCount];

				fread(rmd.VerticePtr, 1, rmd.VerticeCount * sizeof(vertex_t), tmpf);
				fread(rmd.IndicePtr, 1, rmd.IndiceCount * sizeof(GLushort), tmpf);

				int w, h;
				fread(&w, 1, 4, tmpf);
				fread(&h, 1, 4, tmpf);

				rmd.Texture = Texture(w, h);
				fread(&rmd.Texture.PixelData[0], 1, rmd.Texture.PixelData.size(), tmpf);

				fclose(tmpf);

				Async::RunOnMainThread([internalname, rmd]() {
					ModelDatas.Add(internalname, rmd);
				}, forcenow);
				Utilities::Print("CONTENT: Loaded '%s' as model", internalname.c_str());
			} else {
				Utilities::Print("CONTENT: No handler for type '%s'", internalname.c_str());
				return false;
			}

			Async::RunOnMainThread([internalname]() {
				ScannedFiles.Add(internalname, 0);
			}, forcenow);

			return true;
		}

		bool Preload(const std::string& path) {
			if (!LoadFile(path, true)) {
				Utilities::Print("ERROR: could not preload %s\n", path.c_str());
				return false;
			}

			return true;
		}

		void Unload() {
			// who cares
		}

		bool HasLoaded(const std::string& path) {
			return ScannedFiles.ContainsKey(path);
		}

		void Init() {
			static bool s_bInited = false;
			if (s_bInited) return;
			s_bInited = true;
		}

		void LoadFolder(const std::string& path, int& asyncprogress, char* forasyncprogresscurfile) {
			DIR* d = opendir(path.c_str());
			if (d == nullptr) throw std::runtime_error("Invalid path");

			struct dirent *ent;

			while ((ent = readdir(d)) != NULL) {
				if (ent->d_name[0] == '.') continue;

				if (ent->d_type == DT_DIR) {
					LoadFolder(path + "/" + ent->d_name, asyncprogress, forasyncprogresscurfile);
				} else {
					memcpy(forasyncprogresscurfile, ent->d_name, ent->d_namlen + 1);

					LoadFile(path + "/" + ent->d_name);
					asyncprogress++;
				}
			}

			closedir(d);
		}

		int CountFolder(const std::string& path) {
			int ret = 0;

			DIR* d = opendir(path.c_str());
			if (d == nullptr) throw std::runtime_error("Invalid path");

			struct dirent *ent;

			while ((ent = readdir(d)) != NULL) {
				if (ent->d_name[0] == '.') continue;

				if (ent->d_type == DT_DIR) {
					ret += CountFolder(path + "/" + ent->d_name);
				} else {
					ret++;
				}
			}

			closedir(d);

			return ret;
		}
	}
}