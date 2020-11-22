#pragma once

#include "OBEXTypes.h"
#include "OBEXNakedVector.h"

namespace OBEX
{
	class Packet
	{
		public:
			Packet(byte* _bytes, int length)
			{
				bytes = new NakedVector<byte>();
				bytes->PushBack(_bytes, length);
			}

			Packet(NakedVector<byte>* _bytes)
			{
				bytes = _bytes;
			}

			~Packet()
			{
				delete bytes;
				bytes = 0;
			}

			int NumBytes()
			{
				return bytes->Count();
			}

			NakedVector<byte>* ToBytes()
			{
				return bytes;
			}

			// get op (response or request) code (first byte)
			byte GetOpCode()
			{
				return (*bytes)[0];
			}

		protected:
			NakedVector<byte>*	bytes;
	};
}