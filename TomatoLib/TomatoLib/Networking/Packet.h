#ifndef __PACKET_H__
#define __PACKET_H__

#include "EzSock.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix.h"
#include "../Graphics/Color.h"
#include "../Defines.h"

#ifndef null
#define null 0
#endif

namespace TomatoLib {
	enum class EndianType {
		System, Big, Little
	};

	class Packet {
	public:
		static void swap_2(void* source);
		static void swap_4(void* source);
		static void swap_8(void* source);

		EndianType get_local_endian(void);
		static bool InsertOutlenIntOnSend;

		static void DecryptBuffer(byte* buffer, int len);
		static byte ror(byte val, int num);
		static byte rol(byte val, int num);

		EndianType Endian;
		bool Valid;
		EzSock* Sock;
		unsigned int OutPos;
		unsigned int OutSize;
		unsigned int InPos;
		unsigned int InSize;
		unsigned char* InBuffer;
		unsigned char* OutBuffer;

		Packet();
		Packet(EzSock*);
		~Packet();

		void Clear();

		void WriteByte(unsigned char);
		void WriteBool(bool);
		void WriteBytes(const void* arr, int size, bool sendsize = false);
		void WriteShort(short);
		void WriteUShort(unsigned short);
		void WriteInt(int);
		void WriteUInt(unsigned int);
		void WriteLong(long long);
		void WriteULong(unsigned long long);
		void WriteFloat(float);
		void WriteDouble(double);
		void WriteString(const std::string&);
		void WriteStringNT(const std::string&);
		void WriteStringRaw(const std::string&);
		void WriteLine(const std::string&);
		void WriteVector2(const Vector2& v);
		void WriteVector3(const Vector3& v);
		void WriteColor(const Color& c);
		void WriteMatrix(const Matrix& m);

		unsigned char ReadByte();
		bool ReadBool();
		unsigned char* ReadBytes(int len);
		short ReadShort();
		unsigned short ReadUShort();
		int ReadInt();
		unsigned int ReadUInt();
		long long ReadLong();
		unsigned long long ReadULong();
		float ReadFloat();
		double ReadDouble();
		std::string ReadString(int len = -1);
		std::string ReadUntil(const std::string& seq);
		std::string ReadStringNT();
		std::string ReadStringAll();
		Vector2 ReadVector2();
		Vector3 ReadVector3();
		Color ReadColor();
		Matrix ReadMatrix();

		bool CanRead(int len);
		bool CanRead(const std::string& seq);
		bool HasDataLeft();
		int DataLeft();

		bool ReadDump(const std::string& path);
		bool WriteDumpIn(const std::string& path);
		bool WriteDumpOut(const std::string& path);

		void CheckSpaceOut(int);
		void AllocateMoreSpaceOut(int);
		void CheckSpaceIn(int);
		void AllocateMoreSpaceIn(int);
	};
}

#endif
