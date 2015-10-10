#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS

#include "Packet.h"
#include <stdint.h>
#include <string>
using std::string;

#include <cstring>

namespace TomatoLib {
	bool Packet::InsertOutlenIntOnSend = true;

	void Packet::swap_2(void* source) {
		char ret[2];
		for (int i = 0; i < 2; i++) {
			ret[i] = ((char*)source)[1 - i];
		}

		memcpy(source, ret, 2);
	}

	void Packet::swap_4(void* source) {
		char ret[4];
		for (int i = 0; i < 4; i++) {
			ret[i] = ((char*)source)[3 - i];
		}

		memcpy(source, ret, 4);
	}

	void Packet::swap_8(void* source) {
		char ret[8];
		for (int i = 0; i < 8; i++) {
			ret[i] = ((char*)source)[7 - i];
		}

		memcpy(source, ret, 8);
	}

	EndianType Packet::get_local_endian(void) {
		// compiler magic, GCC makes the result of this a constant, moar speeds
		union {
			uint32_t i;
			char c[4];
		} bint = {0x01000002}; // outcome: 0x01 == big endian, 0x02 == little endian

		return (EndianType)bint.c[0];
	}

	Packet::Packet() {
		this->Valid = false;

		this->Sock = nullptr;
		this->InPos = 0;
		this->InSize = 0;

		this->OutPos = 0;
		this->OutSize = 0;
		this->OutBuffer = nullptr;
		this->InBuffer = nullptr;

		this->Endian = EndianType::Little;
	}

	Packet::Packet(EzSock* sock) {
		this->Valid = sock != nullptr;

		this->Sock = sock;
		this->InPos = 0;
		this->InSize = 0;

		this->OutPos = 0;
		this->OutSize = 0;
		this->OutBuffer = nullptr;
		this->InBuffer = nullptr;

		this->Endian = EndianType::Little;
	}

	Packet::~Packet() {
		this->Clear();
	}

	void Packet::Clear() {
		if (this->InBuffer != nullptr) delete[] this->InBuffer;
		if (this->OutBuffer != nullptr) delete[] this->OutBuffer;

		this->InPos = 0;
		this->InSize = 0;
		this->InBuffer = nullptr;

		this->OutPos = 0;
		this->OutSize = 0;
		this->OutBuffer = nullptr;
	}

	void Packet::CheckSpaceOut(int needed) {
		if (this->OutPos + needed >= this->OutSize) {
			this->AllocateMoreSpaceOut(needed < 128 ? 128 : needed);
		}
	}

	void Packet::AllocateMoreSpaceOut(int addsize) {
		this->OutSize += addsize;
		unsigned char* newbuff = new unsigned char[this->OutSize];

		if (this->OutBuffer != nullptr) {
			memcpy(newbuff, this->OutBuffer, this->OutSize - addsize);
			delete[] this->OutBuffer;
		}

		this->OutBuffer = newbuff;
	}

	void Packet::CheckSpaceIn(int needed) {
		if (this->InPos + needed >= this->InSize) {
			this->AllocateMoreSpaceIn(needed);
		}
	}

	void Packet::AllocateMoreSpaceIn(int addsize) {
		this->InSize += addsize;
		unsigned char* newbuff = new unsigned char[this->InSize];

		if (this->InBuffer != nullptr) {
			memcpy(newbuff, this->InBuffer, this->InSize - addsize);
			delete[] this->InBuffer;
		}

		this->InBuffer = newbuff;
	}

	unsigned char Packet::ReadByte() {
		if (!this->CanRead(1)) return 0;

		return this->InBuffer[this->InPos++];
	}

	bool Packet::ReadBool() {
		if (!this->CanRead(1)) return 0;

		return this->InBuffer[this->InPos++] == 1;
	}

	unsigned char* Packet::ReadBytes(int len) {
		if (!this->CanRead(len)) return nullptr;

		this->InPos += len;
		return this->InBuffer + (this->InPos - len);
	}

	short Packet::ReadShort() {
		if (!this->CanRead(2)) return 0;

		short ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 2);
		this->InPos += 2;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_2(&ret);
		return ret;
	}

	unsigned short Packet::ReadUShort() {
		if (!this->CanRead(2)) return 0;

		unsigned short ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 2);
		this->InPos += 2;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_2(&ret);
		return ret;
	}

	float Packet::ReadFloat() {
		if (!this->CanRead(4)) return 0;

		float ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 4);
		this->InPos += 4;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&ret);
		return ret;
	}

	double Packet::ReadDouble() {
		if (!this->CanRead(8)) return 0;

		double ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 8);
		this->InPos += 8;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&ret);
		return ret;
	}

	int Packet::ReadInt() {
		if (!this->CanRead(4)) return 0;

		int ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 4);
		this->InPos += 4;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&ret);
		return ret;
	}

	unsigned int Packet::ReadUInt() {
		if (!this->CanRead(4)) return 0;

		unsigned int ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 4);
		this->InPos += 4;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&ret);
		return ret;
	}

	long long Packet::ReadLong() {
		if (!this->CanRead(8)) return 0;

		long long ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 8);
		this->InPos += 8;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&ret);
		return ret;
	}

	unsigned long long Packet::ReadULong() {
		if (!this->CanRead(8)) return 0;

		unsigned long long ret = 0;
		memcpy(&ret, this->InBuffer + this->InPos, 8);
		this->InPos += 8;

		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&ret);
		return ret;
	}

	Vector2 Packet::ReadVector2() {
		if (!this->CanRead(8)) return Vector2::Zero;

		float x = this->ReadFloat();
		float y = this->ReadFloat();

		return Vector2(x, y);
	}

	Vector3 Packet::ReadVector3() {
		if (!this->CanRead(12)) return Vector3::Zero;

		float x = this->ReadFloat();
		float y = this->ReadFloat();
		float z = this->ReadFloat();

		return Vector3(x, y, z);
	}

	Color Packet::ReadColor() {
		if (!this->CanRead(4)) return Color::Black;

		Color c;
		c.R = this->InBuffer[this->InPos + 0];
		c.G = this->InBuffer[this->InPos + 1];
		c.B = this->InBuffer[this->InPos + 2];
		c.A = this->InBuffer[this->InPos + 3];

		this->InPos += 4;
		return c;
	}

	Matrix Packet::ReadMatrix() {
		if (!this->CanRead(sizeof(float) * 16)) return Matrix();

		Matrix ret;
		for (int i = 0; i < 16; i++) {
			ret.values[i] = this->ReadFloat();
		}

		return ret;
	}

	const char* Packet::ReadString(int len) {
		if (len == -1) len = this->ReadShort();
		if (!this->CanRead(len)) return "";

		if (len == 0) return "";

		char* buff = new char[len + 1];
		memcpy(buff, this->InBuffer + this->InPos, len);
		buff[len] = 0;

		this->InPos += len;
		return buff;
	}

	const char* Packet::ReadStringAll() {
		int len = this->InSize - this->InPos;
		if (len == 0) return "";

		char* buff = new char[len + 1];
		memcpy(buff, this->InBuffer + this->InPos, len);
		buff[len] = 0;

		this->InPos += len;
		return buff;
	}

	const char* Packet::ReadStringNT() {
		int startpos = this->InPos;

		while (this->CanRead(1)) {
			this->InPos++;

			if (this->InBuffer[this->InPos] == 0) {
				break;
			}
		}

		if (startpos == this->InPos) return "";

		char* buff = new char[this->InPos - startpos];
		memcpy(buff, this->InBuffer + startpos, this->InPos - startpos);
		buff[this->InPos - startpos] = 0;

		return buff;
	}

	const char* Packet::ReadUntil(const char* seq) {
		unsigned int startpos = this->InPos;
		unsigned int seqsize = strlen(seq);

		while (this->CanRead(1)) {
			this->InPos++;

			if (this->InPos - startpos >= seqsize) {
				bool done = true;

				for (unsigned int i = 0; i < seqsize; i++) {
					if (this->InBuffer[this->InPos - seqsize + i] != seq[i]) {
						done = false;
						break;
					}
				}

				if (done) {
					break;
				}
			}
		}

		if (startpos == this->InPos) return "";

		char* buff = new char[this->InPos - startpos + 1];
		memcpy(buff, this->InBuffer + startpos, this->InPos - startpos);
		buff[this->InPos - startpos] = 0;

		return buff;
	}

	int Packet::DataLeft() {
		return this->InSize - this->InPos;
	}

	bool Packet::HasDataLeft() {
		return this->InSize - this->InPos > 0;
	}

	bool Packet::CanRead(int numofbytes) {
		bool res = this->InSize - this->InPos >= (unsigned int)(numofbytes);

		if (res == false && this->Sock != nullptr) {
			unsigned char* tmp = new unsigned char[numofbytes];
			int recamount = 0;
			while (recamount != numofbytes) {
				int currec = recv(this->Sock->sock, (char*)tmp + recamount, numofbytes - recamount, 0);

				if (currec == -1 || currec == 0) {
					this->Sock->state = EzSock::skERROR;
					this->Valid = false;
					delete[] tmp;
					return false;
				}

				recamount += currec;
			}

			this->CheckSpaceIn(numofbytes);
			memcpy(this->InBuffer + this->InPos, tmp, numofbytes);
			delete[] tmp;

			return true;
		}

		ASSERT(res);
		return res;
	}

	bool Packet::CanRead(const char* seq) {
		if (this->Sock == nullptr)
			return false;

		char* buffer = new char[4096];
		int curoffset = 0;
		int seqsize = strlen(seq);

		while (true) {
			int currec = recv(this->Sock->sock, buffer + curoffset, 1, 0);

			if (currec == -1 || currec == 0) {
				this->Sock->state = EzSock::skERROR;
				this->Valid = false;
				delete[] buffer;
				return false;
			}

			curoffset++;

			// going into negative index won't really matter, but it's bad practice, so let's do this instead. better safe than sorry right?
			if (curoffset >= seqsize) {
				bool done = true;

				for (int i = 0; i < seqsize; i++) {
					if (buffer[curoffset - seqsize + i] != seq[i]) {
						done = false;
						break;
					}
				}

				if (done) {
					this->CheckSpaceIn(curoffset);
					memcpy(this->InBuffer + this->InPos, buffer, curoffset);

					delete[] buffer;
					return true;
				}
			}

			// enlarge buffer if neded
			if (curoffset % 4096 == 0) {
				char* newbuffer = new char[curoffset + 4096];
				memcpy(newbuffer, buffer, curoffset);

				delete[] buffer;
				buffer = newbuffer;
			}
		}

		delete[] buffer;
		return false;
	}

	void Packet::WriteByte(unsigned char num) {
		this->CheckSpaceOut(1);
		this->OutBuffer[this->OutPos++] = num;
	}

	void Packet::WriteBool(bool num) {
		this->CheckSpaceOut(1);
		this->OutBuffer[this->OutPos++] = num ? 1 : 0;
	}

	void Packet::WriteBytes(const void* arr, int size, bool sendsize) {
		this->CheckSpaceOut(size + sendsize);

		if (sendsize) this->WriteInt(size);
		memcpy(this->OutBuffer + this->OutPos, arr, size);
		this->OutPos += size;
	}

	void Packet::WriteShort(short num) {
		this->CheckSpaceOut(2);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_2(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 2);
		this->OutPos += 2;
	}

	void Packet::WriteUShort(unsigned short num) {
		this->CheckSpaceOut(2);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_2(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 2);
		this->OutPos += 2;
	}

	void Packet::WriteInt(int num) {
		this->CheckSpaceOut(4);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 4);
		this->OutPos += 4;
	}

	void Packet::WriteUInt(unsigned int num) {
		this->CheckSpaceOut(4);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 4);
		this->OutPos += 4;
	}

	void Packet::WriteLong(long long num) {
		this->CheckSpaceOut(8);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 8);
		this->OutPos += 8;
	}

	void Packet::WriteULong(unsigned long long num) {
		this->CheckSpaceOut(8);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 8);
		this->OutPos += 8;
	}

	void Packet::WriteFloat(float num) {
		this->CheckSpaceOut(4);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_4(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 4);
		this->OutPos += 4;
	}

	void Packet::WriteDouble(double num) {
		this->CheckSpaceOut(8);
		if (this->Endian != EndianType::System && this->Endian != get_local_endian()) swap_8(&num);
		memcpy(this->OutBuffer + this->OutPos, &num, 8);
		this->OutPos += 8;
	}

	void Packet::WriteString(const char* str) {
		int size = strlen(str);
		this->CheckSpaceOut(size + 2);

		this->WriteShort(size);

		for (int i = 0; i < size; i++)
			this->OutBuffer[this->OutPos++] = str[i];
	}

	void Packet::WriteStringNT(const char* str) {
		int size = strlen(str);
		this->CheckSpaceOut(size + 1);

		for (int i = 0; i < size; i++)
			this->OutBuffer[this->OutPos++] = str[i];

		this->OutBuffer[this->OutPos++] = 0;
	}

	void Packet::WriteStringRaw(const char* str) {
		int size = strlen(str);
		this->CheckSpaceOut(size);

		for (int i = 0; i < size; i++)
			this->OutBuffer[this->OutPos++] = str[i];
	}

	void Packet::WriteLine(const char* str) {
		int size = strlen(str);
		this->CheckSpaceOut(size + 2);

		for (int i = 0; i < size; i++)
			this->OutBuffer[this->OutPos++] = str[i];

		this->OutBuffer[this->OutPos++] = '\r';
		this->OutBuffer[this->OutPos++] = '\n';
	}

	void Packet::WriteVector2(const Vector2& v) {
		this->CheckSpaceOut(sizeof(float) * 2);

		this->WriteFloat(v.X);
		this->WriteFloat(v.Y);
	}

	void Packet::WriteVector3(const Vector3& v) {
		this->CheckSpaceOut(sizeof(float) * 3);

		this->WriteFloat(v.X);
		this->WriteFloat(v.Y);
		this->WriteFloat(v.Z);
	}

	void Packet::WriteColor(const Color& c) {
		this->WriteBytes(&c, sizeof(Color));
	}

	void Packet::WriteMatrix(const Matrix& m) {
		this->CheckSpaceOut(sizeof(float) * 4);

		for (int i = 0; i < 16; i++) {
			this->WriteFloat(m.values[i]);
		}
	}

	bool Packet::ReadDump(const char* path) {
		FILE* f = fopen(path, "rb");
		if (f == nullptr) return false;

		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);

		if (this->InBuffer != nullptr) delete[] this->InBuffer;
		this->InBuffer = new byte[size];
		this->InPos = 0;
		this->InSize = size;

		rewind(f);
		fread(this->InBuffer, sizeof(char), size, f);

		fclose(f);
		return true;
	}

	bool Packet::WriteDumpIn(const char* path) {
		remove(path);

		FILE* f = fopen(path, "wb");
		if (f == nullptr) return false;

		fwrite(this->InBuffer, 1, this->InSize, f);

		fclose(f);
		return false;
	}

	bool Packet::WriteDumpOut(const char* path) {
		remove(path);

		FILE* f = fopen(path, "wb");
		if (f == nullptr) return false;

		fwrite(this->OutBuffer, 1, this->OutPos, f);

		fclose(f);
		return false;
	}
}
