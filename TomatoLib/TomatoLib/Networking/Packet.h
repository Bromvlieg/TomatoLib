#ifndef __PACKET_H__
#define __PACKET_H__

#include "EzSock.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Defines.h"

#ifndef null
#define null 0
#endif

namespace TomatoLib {
	class Packet {
	public:
		static void DecryptBuffer(byte* buffer, int len);
		static byte ror(byte val, int num);
		static byte rol(byte val, int num);

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
		void WriteBytes(unsigned char* arr, int size, bool sendsize);
		void WriteShort(short);
		void WriteUShort(unsigned short);
		void WriteInt(int);
		void WriteUInt(unsigned int);
		void WriteLong(long long);
		void WriteULong(unsigned long long);
		void WriteFloat(float);
		void WriteDouble(double);
		void WriteString(const char*);
		void WriteStringNT(const char*);
		void WriteStringRaw(const char*);
		void WriteLine(const char*);
		void WriteVector2(Vector2& v);
		void WriteVector3(Vector3& v);

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
		const char* ReadString(int len = -1);
		const char* ReadUntil(char* seq);
		const char* ReadStringNT();
		const char* ReadStringAll();
		Vector2 ReadVector2();
		Vector3 ReadVector3();

		bool CanRead(int len);
		bool CanRead(char* seq);
		bool HasDataLeft();
		int DataLeft();

		void Send();
		void ReadDump();
		void Dump();

		void CheckSpaceOut(int);
		void AllocateMoreSpaceOut(int);
		void CheckSpaceIn(int);
		void AllocateMoreSpaceIn(int);
	};
}

#endif
