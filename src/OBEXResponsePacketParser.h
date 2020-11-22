#pragma once

#include "OBEXTypes.h"
#include "OBEXPacket.h"
#include "OBEXRequestCode.h"
#include "OBEXResponseCode.h"
#include "OBEXHeader.h"

namespace OBEX
{

	// favors computation time over memory usage, headers are parsed and created on request

	class ResponsePacketParser
	{
		public:
			static ResponseCode::ResponseCode GetResponseCode(Packet* packet)
			{
				return (ResponseCode::ResponseCode)(*packet->ToBytes())[0];
			}

			static Header* GetHeader(RequestCode::RequestCode reqCode, Packet* packet, HeaderFieldCode::HeaderFieldCode headerFieldCode)
			{
				int startByte = 3;

				if(reqCode == OBEX::RequestCode::OBEX_CONNECT)
					startByte = 7;
				
				NakedVector<byte>& bytes = (*packet->ToBytes());

				while(startByte < packet->NumBytes())
				{
					HeaderFieldCode::HeaderFieldCode fc = (HeaderFieldCode::HeaderFieldCode)(bytes[startByte++]);
					int fieldLen = 5; // default for connection-id (1 for code + 4 byte int)
					
					if(fc != HeaderFieldCode::OBEX_HEADER_CONNECTION_ID &&
					   fc != HeaderFieldCode::OBEX_HEADER_LENGTH)
					{
						fieldLen = (bytes[startByte] << 8) | bytes[startByte+1];
						startByte += 2;
					}
					
					int dataLen = fieldLen - 3;
					if(fc == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID ||
					   fc == HeaderFieldCode::OBEX_HEADER_LENGTH)
						dataLen = fieldLen - 1;

					if(fc == headerFieldCode)
					{
						Header* ret = new Header(fc, packet->ToBytes()->Ptr() + startByte, dataLen);
						return ret;
					}
					else
					{
						startByte += dataLen;
					}
				}

				return 0;
			}

	};

}