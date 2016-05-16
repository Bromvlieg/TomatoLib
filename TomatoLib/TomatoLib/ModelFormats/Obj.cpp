#include "Obj.h"

#include "../Math/Matrix.h"
#include "../Graphics/Shader.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Dictonary.h"
#include "../Utilities/zip_uncompressed.h"
#include "../Defines.h"

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include <cstring>
#include <iostream>
#include <fstream>

namespace TomatoLib {
	namespace ModelFormats {
		Obj::Obj() {}
		Obj::Obj(const char* fname) { this->FromFile(fname); }

		void Obj::FromFile(const char* fname) {
			std::ifstream infile;
			infile.open(fname, std::ifstream::binary);

			if (!infile.is_open()) {
				std::cout << "Obj::FromFile() failed! Could not open file named: " << fname << std::endl;
				infile.close();
				return;
			}

			infile.seekg(0, std::ios::end);
			size_t file_size_in_byte = (size_t)infile.tellg();

			std::vector<char> data;
			data.resize(file_size_in_byte);
			infile.seekg(0, std::ios::beg);

			infile.read(&data[0], file_size_in_byte);

			const char* fileDataConst = (const char*)&data[0];

			this->FromBuffer(fileDataConst);

			infile.close();
		}

		void _add_triangle(ObjMesh& m, const List<Vector3>& vertices, const List<Vector3>& normals, const List<Vector2>& texposs, const std::string& a, const std::string& b, const std::string& c) {
			auto data_a = Utilities::Split(a, '/');
			auto data_b = Utilities::Split(b, '/');
			auto data_c = Utilities::Split(c, '/');

			vertex_t v_a;
			vertex_t v_b;
			vertex_t v_c;

			// vertice ID is always filled in
			// -1 due not zero-indexed array but one-indexed
			int vertID_a = atoi(data_a[0].c_str()) - 1;
			int vertID_b = atoi(data_b[0].c_str()) - 1;
			int vertID_c = atoi(data_c[0].c_str()) - 1;

			// fix negative offset
			if (vertID_a < 0) vertID_a += vertices.Count + 1;
			if (vertID_b < 0) vertID_b += vertices.Count + 1;
			if (vertID_c < 0) vertID_c += vertices.Count + 1;

			v_a.Pos = vertices[vertID_a];
			v_b.Pos = vertices[vertID_b];
			v_c.Pos = vertices[vertID_c];

			// UV
			if (data_a.size() > 1 && data_a[1].size() > 0) {
				int texID_a = atoi(data_a[1].c_str()) - 1;
				int texID_b = atoi(data_b[1].c_str()) - 1;
				int texID_c = atoi(data_c[1].c_str()) - 1;

				if (texID_a < 0) texID_a += texposs.Count + 1;
				if (texID_b < 0) texID_b += texposs.Count + 1;
				if (texID_c < 0) texID_c += texposs.Count + 1;

				v_a.UV = texposs[texID_a];
				v_b.UV = texposs[texID_b];
				v_c.UV = texposs[texID_c];
			}

			// normals
			if (data_a.size() > 2 && data_a[2].size() > 0) {
				int normID_a = atoi(data_a[2].c_str()) - 1;
				int normID_b = atoi(data_b[2].c_str()) - 1;
				int normID_c = atoi(data_c[2].c_str()) - 1;

				if (normID_a < 0) normID_a += normals.Count + 1;
				if (normID_b < 0) normID_b += normals.Count + 1;
				if (normID_c < 0) normID_c += normals.Count + 1;

				/* TODO: add normals
				v_a.Normal = normals[normID_a];
				v_b.Normal = normals[normID_b];
				v_c.Normal = normals[normID_c];
				*/
			}


			m.Vertices.Add(v_a);
			m.Indices.Add(m.Vertices.Count - 1);

			m.Vertices.Add(v_b);
			m.Indices.Add(m.Vertices.Count - 1);

			m.Vertices.Add(v_c);
			m.Indices.Add(m.Vertices.Count - 1);
		}

		void Obj::FromBuffer(const char* buffer) {
			ObjMesh m;

			std::string curline;
			int i = -1;
			int texid = 0;
			int normalid = 0;

			List<Vector3> vertices;
			List<Vector3> normals;
			List<Vector2> texposs;

			while (buffer[++i] != 0) {
				char l = buffer[i];

				if ((l == '\n' || l == '\r') && curline.size() > 0) {
					auto parts = Utilities::Split(curline, ' ');

					if (parts[0] == "v") {
						vertices.Add(Vector3((float)atof(parts[1].c_str()), (float)atof(parts[2].c_str()), (float)atof(parts[3].c_str())));
					} else if (parts[0] == "vt") {
						texposs.Add(Vector2((float)atof(parts[1].c_str()), (float)atof(parts[2].c_str())));
					} else if (parts[0] == "vn") {
						normals.Add(Vector3((float)atof(parts[1].c_str()), (float)atof(parts[2].c_str()), (float)atof(parts[3].c_str())));
					} else if (parts[0] == "f") {
						if (parts.size() == 4) { // triangle
							_add_triangle(m, vertices, normals, texposs, parts[1], parts[2], parts[3]);
						} else if (parts.size() == 5) { // quad
							_add_triangle(m, vertices, normals, texposs, parts[1], parts[2], parts[3]);
							_add_triangle(m, vertices, normals, texposs, parts[1], parts[3], parts[4]);
						} else {
							// throw "Obj::FromBuffer: Invalid face count";
							Utilities::Print("Obj::FromBuffer: Invalid face count while loading model");
						}
					} else if (parts[0] == "usemtl") {
						m.Mtl = parts[1];
					} else if (parts[0] == "g") {
						if (m.Group != "") {
							this->Meshes.Add(m);
							m = ObjMesh();
						}

						m.Group = parts[1];
					}

					curline = "";
				}

				if (l != '\n' && l != '\r') curline += l;
			}

			if (m.Vertices.Count > 0) this->Meshes.Add(m);
		}

		RawModelData Obj::GetRawData() {
			RawModelData ret;

			int vc = 0;
			int ic = 0;

			for (auto m : this->Meshes) {
				vc += m.Vertices.Count;
				ic += m.Indices.Count;
			}

			ret.VerticePtr = new vertex_t[vc];
			ret.IndicePtr = new GLushort[ic];

			int curv = 0;
			int curi = 0;
			for (auto m : this->Meshes) {
				memcpy(ret.VerticePtr + curv, m.Vertices.Buffer(), m.Vertices.Count * sizeof(vertex_t));

				for (int i = 0; i < m.Indices.Count; i++) {
					ret.IndicePtr[curi + i] = m.Indices[i] + curv;
				}
				
				curv += m.Vertices.Count;
				curi += m.Indices.Count;
			}

			ret.VerticeCount = vc;
			ret.IndiceCount = ic;

			// TODO: merge texture or just don't support more than one texture per model? dunno how to solve this yet
			ret.Texture = Texture(64, 64);
			for (unsigned int x = 0; x < ret.Texture.Width; x++) {
				for (unsigned int y = 0; y < ret.Texture.Height; y++) {
					ret.Texture.SetPixel(x, y, Color(Utilities::GetRandom(0, 256), Utilities::GetRandom(0, 256), Utilities::GetRandom(0, 256)));
				}
			}

			ret.Texture.Filename = "none";

			return ret;
		}
	}
}