#include "Bsp.h"

#include "../Math/Matrix.h"
#include "../Graphics/Shader.h"
#include "../Utilities/Dictonary.h"
#include "../Utilities/zip_uncompressed.h"
#include "../Defines.h"

#include <vector>
#include <VTFLib.h>

#define _USE_MATH_DEFINES
#include <math.h>

//  3904856
// 23250864

namespace TomatoLib {
	void _readFromFile(FILE* f, char* dest, int len, int pos) {
		fseek(f, pos, SEEK_SET);

		int rem = len;
		while (rem > 0) {
			int toread = rem > 1024 ? 1024 : rem;
			int r = fread(dest, 1, toread, f);

			rem -= r;
			dest += toread;
		}
	}

	void _addtriangle(GLuint a, GLuint b, GLuint c, List<GLushort>& indices) {
		indices.Add(a);
		indices.Add(b);
		indices.Add(c);
	}

	template<class Type>
	void _filllist(FILE* fptr, List<Type>& lst, int size, bsp_lump_t& l) {
		int c = l.filelen / size;
		lst.Reserve(c);
		_readFromFile(fptr, (char*)lst.Buffer(), l.filelen, l.fileofs);
		lst.Count = c;
	}

	void _addIndice(List<GLushort>& indices, GLushort a, GLushort b, GLushort c) {
		indices.Add(a);
		indices.Add(b);
		indices.Add(c);
	}

	//materials/maps/de_dust2/cs_italy/hpe_stone_wall_02_top_640_480_112.vmt
	//materials/maps/de_dust2/cs_italy/hpe_stone_wall_02_top_640_480_112
	bool startsWith(const char *pre, const char *str) {
		size_t lenpre = strlen(pre), lenstr = strlen(str);
		return lenstr > lenpre ? false : strncmp(pre, str, lenstr) == 0;
	}


	void _rec_vmt_handler(VTFLib::CVTFFile& texfile, List<bsp_pak_file_t>& pakkedfiles, char* texname) {
		VTFLib::CVMTFile mfile;
		mfile.Load((std::string("E:/Tmp/csgomatstuff/materials/") + texname + ".vmt").c_str());

		if (!mfile.IsLoaded()) {
			for (int paki = 0; paki < pakkedfiles.Count; paki++) {
				bsp_pak_file_t& file = pakkedfiles[paki];
				if (startsWith(file.fileName, (std::string("materials/") + texname + ".vmt").c_str())) {
					mfile.Load(file.data, file.length);
					break;
				}
			}
		}

		if (!mfile.IsLoaded()) return;
		texname = null;
		VTFLib::Nodes::CVMTStringNode* basetex = (VTFLib::Nodes::CVMTStringNode*)mfile.GetRoot()->GetNode("$baseTexture");
		if (basetex != null) {
			texname = (char*)basetex->GetValue();
		} else {
			VTFLib::Nodes::CVMTGroupNode* g_rep = (VTFLib::Nodes::CVMTGroupNode*)mfile.GetRoot()->GetNode("replace");

			if (g_rep != null) {
				VTFLib::Nodes::CVMTStringNode* envmap = (VTFLib::Nodes::CVMTStringNode*)g_rep->GetNode("$envmap");
				if (envmap != null) {
					texname = (char*)envmap->GetValue();
				}
			}
		}

		if (texname == null) {
			return;
		}

		char* nameptr = texname;
		while (*nameptr != 0) {
			char c = *nameptr;
			if (c >= 'A' && c <= 'Z') *nameptr += 'a' - 'A';
			nameptr++;
		}

		texfile.Load((std::string("E:/Tmp/csgomatstuff/materials/") + texname + ".vtf").c_str());

		if (!texfile.IsLoaded()) {
			for (int paki = 0; paki < pakkedfiles.Count; paki++) {
				bsp_pak_file_t& file = pakkedfiles[paki];
				if (startsWith(file.fileName, (std::string("materials/") + texname + ".vtf").c_str())) {
					texfile.Load(file.data, file.length, false);
					return;
				}
			}
		}
	}

	bsp_mesh_t& _getMesh(List<bsp_mesh_t*>& lst, int texid, char* texname, List<bsp_pak_file_t>& pakkedfiles) {
		for (int i = 0; i < lst.Count; i++) {
			if (lst[i]->TextureID == texid) {
				return *lst[i];
			}
		}

		bsp_mesh_t* mesh = new bsp_mesh_t();
		lst.Add(mesh);

		mesh->TextureID = texid;

		VTFLib::CVTFFile texfile;
		texfile.Load((std::string("E:/Tmp/csgomatstuff/materials/") + texname + ".vtf").c_str());

		if (!texfile.IsLoaded()) {
			for (int paki = 0; paki < pakkedfiles.Count; paki++) {
				bsp_pak_file_t& file = pakkedfiles[paki];
				if (startsWith(file.fileName, (std::string("materials/") + texname + ".vtf").c_str())) {
					texfile.Load(file.data, file.length, false);
					break;
				}
			}
		}

		if (!texfile.IsLoaded()) {
			_rec_vmt_handler(texfile, pakkedfiles, texname);
		}

		if (texfile.IsLoaded()) {
			VTFImageFormat mat = texfile.GetFormat();
			mesh->Texture = Texture(texfile.GetWidth(), texfile.GetHeight());
			if (!VTFLib::CVTFFile::ConvertToRGBA8888(texfile.GetData(0, 0, 0, 0), &mesh->Texture.PixelData[0], mesh->Texture.Width, mesh->Texture.Height, texfile.GetFormat())) {
				mesh->Texture = Texture(1, 1);
				mesh->Texture.SetPixel(0, 0, Color::Red);
			}
		} else {
			mesh->Texture = Texture(1, 1);
			mesh->Texture.SetPixel(0, 0, Color::Red);
		}

		mesh->Texture.BindGL();
		mesh->Texture.Upload();

		return *mesh;
	}

	GLushort _getVerticeIndex(List<Vector3>& master, List<bsp_mesh_vertex_t>& slave, int masterkey, texinfo_t& ti, Vector2& tsize) {
		Vector3& v = master[masterkey];

		float _u = (ti.textureVecs[0][0] * v.X + ti.textureVecs[0][1] * v.Z + ti.textureVecs[0][2] * v.Y + ti.textureVecs[0][3]) / tsize.X;
		float _v = (ti.textureVecs[1][0] * v.X + ti.textureVecs[1][1] * v.Z + ti.textureVecs[1][2] * v.Y + ti.textureVecs[1][3]) / tsize.Y;

		bsp_mesh_vertex_t vert;
		vert.pos = v;
		vert.u = _u;
		vert.v = _v;

		return (GLushort)slave.Add(vert);
	}

	Bsp::Bsp(std::string file) {
		FILE* fptr = fopen(file.c_str(), "rb");
		
		fread(&this->Header, 1, sizeof(bsp_header_t), fptr);

		if (this->Header.ident != IDBSPHEADER) {
			throw "Invalid file";
		}


		List<Vector3> vertices;

		bsp_lump_t l_texdatastrings = this->Header.lumps[MapFormats::Lumps::TexDataStringData];
		bsp_lump_t l_texdatatable = this->Header.lumps[MapFormats::Lumps::TexDataStringTable];
		bsp_lump_t l_texdata = this->Header.lumps[MapFormats::Lumps::TexData];
		bsp_lump_t l_texinfo = this->Header.lumps[MapFormats::Lumps::TexInfo];
		bsp_lump_t l_faces = this->Header.lumps[MapFormats::Lumps::Faces];
		bsp_lump_t l_sedge = this->Header.lumps[MapFormats::Lumps::Surfedges];
		bsp_lump_t l_edge = this->Header.lumps[MapFormats::Lumps::Edges];
		bsp_lump_t l_brushes = this->Header.lumps[MapFormats::Lumps::Brushes];
		bsp_lump_t l_brushsides = this->Header.lumps[MapFormats::Lumps::BrushSides];
		bsp_lump_t l_verts = this->Header.lumps[MapFormats::Lumps::Vertices];
		bsp_lump_t l_planes = this->Header.lumps[MapFormats::Lumps::Planes];
		bsp_lump_t l_pak = this->Header.lumps[MapFormats::Lumps::PakFile];
		bsp_lump_t l_overlays = this->Header.lumps[MapFormats::Lumps::Overlays];

		List<doverlay_t> overlays;
		List<dplane_t> planes;
		List<dbrushside_t> brushsides;
		List<dbrush_t> brushes;
		List<dedge_t> edges;
		List<GLint> surfedges;
		List<dface_t> faces;
		List<texinfo_t> texinto;
		List<dtexdata_t> texdata;
		List<int> texdataStringTable;
		List<char*> texdataStrings;

		_filllist(fptr, vertices, sizeof(Vector3), l_verts);
		_filllist(fptr, surfedges, 4, l_sedge);
		_filllist(fptr, edges, sizeof(dedge_t), l_edge);
		_filllist(fptr, faces, sizeof(dface_t), l_faces);
		_filllist(fptr, brushes, sizeof(dbrush_t), l_brushes);
		_filllist(fptr, brushsides, sizeof(dbrushside_t), l_brushsides);
		_filllist(fptr, texdata, sizeof(dtexdata_t), l_texdata);
		_filllist(fptr, texinto, sizeof(texinfo_t), l_texinfo);
		_filllist(fptr, planes, sizeof(dplane_t), l_planes);
		_filllist(fptr, overlays, sizeof(doverlay_t), l_overlays);
		_filllist(fptr, texdataStringTable, sizeof(int), l_texdatatable);

		for (int i = 0; i < texdataStringTable.Count; i++) {
			char* buff = new char[128];
			_readFromFile(fptr, (char*)buff, 128, texdataStringTable[i] + l_texdatastrings.fileofs);

			char* nameptr = buff;
			while (*nameptr != 0) {
				char c = *nameptr;
				if (c >= 'A' && c <= 'Z') *nameptr += 'a' - 'A';
				nameptr++;
			}

			texdataStrings.Add(buff);
		}

		for (int i = 0; i < vertices.Count; i++) {
			vertices[i].X *= -1;
			std::swap(vertices[i].Y, vertices[i].Z);
		}


		ZIP_EndOfCentralDirRecord rec = {0};
		fseek(fptr, l_pak.fileofs, SEEK_SET);

		bool bFoundEndOfCentralDirRecord = false;
		unsigned int offset = l_pak.fileofs + l_pak.filelen - sizeof(ZIP_EndOfCentralDirRecord);
		for (unsigned int startOffset = offset; offset <= startOffset; offset--) {
			fseek(fptr, offset, SEEK_SET);

			fread(&rec, 1, sizeof(ZIP_EndOfCentralDirRecord), fptr);

			if (rec.signature == PKID(5, 6)) {
				bFoundEndOfCentralDirRecord = true;

				// Set any xzip configuration
				if (rec.commentLength) {
					char commentString[128];
					int commentLength = min(rec.commentLength, sizeof(commentString));
					fread(commentString, 1, commentLength, fptr);

					if (commentLength == sizeof(commentString))
						--commentLength;
					commentString[commentLength] = '\0';
					//ParseXZipCommentString(commentString);
				}
				break;
			} else {
				// wrong record
				rec.nCentralDirectoryEntries_Total = 0;
			}
		}

		List<bsp_pak_file_t> pakkedfiles;
		int numZipFiles = rec.nCentralDirectoryEntries_Total;
		fseek(fptr, rec.startOfCentralDirOffset + l_pak.fileofs, SEEK_SET);
		long curpos = ftell(fptr);

		for (int i = 0; i < numZipFiles; i++) {
			ZIP_FileHeader zipFileHeader;
			fread(&zipFileHeader, 1, sizeof(ZIP_FileHeader), fptr);

			bsp_pak_file_t e;
			fread(e.fileName, 1, zipFileHeader.fileNameLength, fptr);
			e.fileName[zipFileHeader.fileNameLength] = 0;
			e.length = zipFileHeader.uncompressedSize;
			e.data = new unsigned char[e.length];

			curpos = ftell(fptr);
			fseek(fptr, zipFileHeader.relativeOffsetOfLocalHeader + sizeof(ZIP_LocalFileHeader) + zipFileHeader.fileNameLength + zipFileHeader.extraFieldLength + l_pak.fileofs, SEEK_SET);

			unsigned int rem = e.length;
			unsigned char* dptr = e.data;
			while (rem > 0) {
				int toread = rem > 1024 ? 1024 : rem;
				int r = fread(dptr, 1, toread, fptr);

				rem -= r;
				dptr += toread;
			}

			pakkedfiles.Add(e);
			fseek(fptr, curpos, SEEK_SET);
		}

		
		for (int faceindex = 0; faceindex < faces.Count; faceindex++) {
			dface_t& f = faces[faceindex];
			if (f.texinfo == -1) continue; // no texture

			texinfo_t& ti = texinto[f.texinfo];

			char* texname = texdataStrings[ti.texdata];
			if (startsWith(texname, "tools/")) continue;

			if ((ti.flags & 0x2) > 0) continue; // sky2D
			if ((ti.flags & 0x4) > 0) continue; // sky
			if ((ti.flags & 0x40) > 0) continue; // trigger
			if ((ti.flags & 0x80) > 0) continue; // nodraw
			if ((ti.flags & 0x100) > 0) continue; // hint
			if ((ti.flags & 0x200) > 0) continue; // skip

			bsp_mesh_t& mesh = _getMesh(this->Meshes, ti.texdata, texname, pakkedfiles);

			Vector2 tsize((float)mesh.Texture.Width, (float)mesh.Texture.Height);
			GLint begin = surfedges[f.firstedge];
			GLushort top = begin < 0 ? edges[-begin].end : edges[begin].start;
			//skip first and last because when we have the 'top' of a polygon we only need to make triangles from there to every other, the lines from the top are implied by the others
			for (int i = 1; i < f.numedges - 1; i++) {
				dedge_t& e = edges[abs(surfedges[f.firstedge + i])];
				_addIndice(mesh.Indices, _getVerticeIndex(vertices, mesh.Vertices, e.start, ti, tsize), _getVerticeIndex(vertices, mesh.Vertices, e.end, ti, tsize), _getVerticeIndex(vertices, mesh.Vertices, top, ti, tsize));
			}
		}

		for (int i = 0; i < overlays.Count; i++) {
			doverlay_t& o = overlays[i];
			texinfo_t& ti = texinto[o.TexInfo];
			char* texname = texdataStrings[ti.texdata];

			if (startsWith(texname, "tools/")) continue;

			if ((ti.flags & 0x2) > 0) continue; // sky2D
			if ((ti.flags & 0x4) > 0) continue; // sky
			if ((ti.flags & 0x40) > 0) continue; // trigger
			if ((ti.flags & 0x80) > 0) continue; // nodraw
			if ((ti.flags & 0x100) > 0) continue; // hint
			if ((ti.flags & 0x200) > 0) continue; // skip

			bsp_mesh_t& mesh = _getMesh(this->Meshes, ti.texdata, texname, pakkedfiles);


			Vector2 tsize((float)mesh.Texture.Width, (float)mesh.Texture.Height);

			o.Origin.X *= -1;
			std::swap(o.Origin.Y, o.Origin.Z);
			std::swap(o.BasisNormal.Y, o.BasisNormal.Z);

			for (int i2 = 0; i2 < 4; i2++) {
				o.UVPoints[i2].X *= -1;
				std::swap(o.UVPoints[i2].Y, o.UVPoints[i2].Z);
			}

			float pi = ((float)M_PI / 2);
			// TODO: finetune this
			Matrix m = Matrix::CreateRotationZ((o.BasisNormal.Z + 2) * pi) * Matrix::CreateRotationY((o.BasisNormal.Y - 1) * pi) * Matrix::CreateRotationX((o.BasisNormal.X + 2) * pi);

			mesh.Vertices.Add({o.Origin + m.Translate(o.UVPoints[3]), o.U[0], o.V[1]}); // A
			mesh.Vertices.Add({o.Origin + m.Translate(o.UVPoints[0]), o.U[1], o.V[1]}); // B
			mesh.Vertices.Add({o.Origin + m.Translate(o.UVPoints[2]), o.U[0], o.V[0]}); // C
			mesh.Vertices.Add({o.Origin + m.Translate(o.UVPoints[1]), o.U[1], o.V[0]}); // D

			mesh.Indices.Add(mesh.Vertices.Count - 3);
			mesh.Indices.Add(mesh.Vertices.Count - 2);
			mesh.Indices.Add(mesh.Vertices.Count - 4);

			mesh.Indices.Add(mesh.Vertices.Count - 3);
			mesh.Indices.Add(mesh.Vertices.Count - 1);
			mesh.Indices.Add(mesh.Vertices.Count - 2);
		}

		fclose(fptr);
	}

	Bsp::~Bsp() {}

	GLint viewMatrixLocation;
	GLint projMatrixLocation;
	void Bsp::DoGLStuff() {
		checkGL;

		shader.AttachRaw("#version 150\
						 \n	\
						 \n	in vec3 position;\
						 \n	in vec2 texpos;\
						 \n out vec3 pos;\
						 \n out vec2 tpos;\
						 \n	uniform mat4 proj;\
						 \n	uniform mat4 view;\
						 \n	\
						 \n	void main() {\
						 \n		gl_Position = proj * view * vec4(position, 1.0);\
						 \n		tpos = texpos;\
						 \n	}\
						 \n	", GL_VERTEX_SHADER);

		shader.AttachRaw("#version 150\
						 \n	\
						 \n	out vec4 outColor;\
						 \n in vec2 tpos;\
						 \n uniform sampler2D tex;\
						 \n	\
						 \n	void main() {\
						 \n		outColor = texture(tex, tpos);\
						 \n		if (outColor.a == 0.0) discard;\
						 \n	}\
		", GL_FRAGMENT_SHADER);

		shader.Link();
		shader.Use();

		glBindFragDataLocation(shader.ProgramHandle, 0, "outColor");
		checkGL;

		viewMatrixLocation = glGetUniformLocation(shader.ProgramHandle, "view");
		projMatrixLocation = glGetUniformLocation(shader.ProgramHandle, "proj");
		Matrix proj = Matrix::CreatePerspective(float(M_PI_2), 1920.0f / 1080.0f, 0.1f, 10000.0f);

		for (int i = 0; i < this->Meshes.Count; i++) {
			bsp_mesh_t& m = *this->Meshes[i];

			// Create Vertex Array Object
			glGenVertexArrays(1, &m.vao);
			glBindVertexArray(m.vao);

			// Create a Vertex Buffer Object and copy the vertex data to it
			glGenBuffers(1, &m.vbo);
			glGenBuffers(1, &m.ebo);

			glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
			glBufferData(GL_ARRAY_BUFFER, m.Vertices.Count * sizeof(float) * 5, m.Vertices.Buffer(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.Indices.Count * sizeof(short), m.Indices.Buffer(), GL_STATIC_DRAW);
			checkGL;

			// Specify the layout of the vertex data
			GLint posAttrib = glGetAttribLocation(shader.ProgramHandle, "position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
			checkGL;

			// Specify the layout of the vertex data
			GLint texposAttrib = glGetAttribLocation(shader.ProgramHandle, "texpos");
			glEnableVertexAttribArray(texposAttrib);
			glVertexAttribPointer(texposAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			checkGL;

			glUniformMatrix4fv(projMatrixLocation, 1, GL_FALSE, proj.values);
			glUniform1i(glGetUniformLocation(shader.ProgramHandle, "tex"), 0);
		}
	}

	void Bsp::Draw(Render& r, Camera& cam) {
		checkGL;
		shader.Use();
		glActiveTexture(GL_TEXTURE0);
		checkGL;

		for (int i = 0; i < this->Meshes.Count; i++) {
			bsp_mesh_t& m = *this->Meshes[i];

			m.Texture.Use();

			glBindVertexArray(m.vao);
			glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);

			cam.InsertViewMatrix(viewMatrixLocation);

			glDrawElements(GL_TRIANGLES, m.Indices.Count, GL_UNSIGNED_SHORT, 0);
		}

		checkGL;
	}
}