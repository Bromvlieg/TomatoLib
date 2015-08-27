#pragma once
#ifndef __TL__MDLOBJ_H_
#define __TL__MDLOBJ_H_

#include "../Graphics/Render.h"
#include "../Graphics/Model.h"
#include "../Math/Vector3.h"
#include "../Utilities/List.h"
#include "../Game/Camera.h"
#include <string>

namespace TomatoLib {
	namespace ModelFormats {
		struct mtl_t {
			Vector3 Ka;
			Vector3 Kd;
			Vector3 Ks;
			float Km;
			float Ni;
			float Ns;
			float d;

			std::string Name;
			std::string TexturePath;
		};

		class ObjMesh {
		public:
			List<vertex_t> Vertices;
			List<GLushort> Indices;

			std::string Mtl;
			std::string Group;
			TomatoLib::Texture Texture;
		};

		class Obj {
		public:
			List<ObjMesh> Meshes;

			Obj();
			Obj(const char* file);
			void FromFile(const char* file);
			void FromBuffer(const char* buff);
			RawModelData GetRawData();
		};
	}
}

#endif