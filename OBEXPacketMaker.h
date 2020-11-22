#pragma once

#include "OBEXTypes.h"
#include "OBEXNakedVector.h"
#include "OBEXHeader.h"
#include "OBEXRequestCode.h"
#include "OBEXResponseCode.h"
#include "OBEXPacket.h"
#include "OBEXResponsePacketParser.h"
#include "OBEXConnectConstant.h"

namespace OBEX
{
	// general OBEX packet maker
	class PacketMaker
	{
		public:
			static Packet* MakeConnectPacket(std::vector<Header*>& headers, unsigned short maxPacketSize)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  7 /* bytes in connect req */ );

				resultBytes->PushBack(OBEX::RequestCode::OBEX_CONNECT);
				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);
				resultBytes->PushBack(ConnectConstant::OBEX_VERSION);
				resultBytes->PushBack(ConnectConstant::OBEX_CONNECT_FLAGS);
				resultBytes->PushBack(maxPacketSize >> 8);
				resultBytes->PushBack(maxPacketSize & 0xff);		

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

			static Packet* MakeDisconnectPacket(std::vector<Header*>& headers)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  3 );

				resultBytes->PushBack(OBEX::RequestCode::OBEX_DISCONNECT);
				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

			static Packet* MakeSetPathPacket(std::vector<Header*>& headers, byte flags=0x02, byte constants=0)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  5 );

				resultBytes->PushBack(OBEX::RequestCode::OBEX_SET_PATH);
				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);
				resultBytes->PushBack(flags);
				resultBytes->PushBack(constants);

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

			static Packet* MakeGetPacket(std::vector<Header*>& headers, bool final)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  3 );

				if(final)
					resultBytes->PushBack(OBEX::RequestCode::OBEX_GET_FINAL);
				else
					resultBytes->PushBack(OBEX::RequestCode::OBEX_GET);

				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

			static Packet* MakePutPacket(std::vector<Header*>& headers, bool final)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  3 );

				if(final)
					resultBytes->PushBack(OBEX::RequestCode::OBEX_PUT_FINAL);
				else
					resultBytes->PushBack(OBEX::RequestCode::OBEX_PUT);

				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

			static Packet* MakeAbortPacket(std::vector<Header*>& headers)
			{
				NakedVector<byte>* resultBytes = new NakedVector<byte>();

				unsigned short totalSize = (unsigned short)( CalcNumBytesForHeaders(headers) +  3 );

				resultBytes->PushBack(OBEX::RequestCode::OBEX_ABORT);

				resultBytes->PushBack(totalSize >> 8);
				resultBytes->PushBack(totalSize & 0xff);

				PushHeaderBytesIntoVector(headers, resultBytes);

				return new Packet(resultBytes);
			}

		protected:
			static 	int CalcNumBytesForHeaders(const std::vector<Header*>& headers)
			{
				int totalHeaderBytes = 0;
				for(size_t i=0; i<headers.size(); i++)
					totalHeaderBytes += headers[i]->GetSize();

				return totalHeaderBytes;
			}

			static void PushHeaderBytesIntoVector(const std::vector<Header*>& headers, NakedVector<byte>* theVector)
			{
				for(size_t i=0; i<headers.size(); i++)
				{
					theVector->PushBack(*headers[i]->ToBytes());
				}
			}

	};
}