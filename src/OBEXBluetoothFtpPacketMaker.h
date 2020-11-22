#pragma once

#include "OBEXPacketMaker.h"

namespace OBEX
{
	// packet maker for the bluetooth file transfer profile
	class BluetoothFtpPacketMaker
	{
		public:
			static Packet* MakeConnectPacket(unsigned short maxPacketSize)
			{
				std::vector<Header*> headers;
				OBEX::Header* btFBTarget = OBEX::Header::MakeFileBrowsingUUIDTargetHeader();
				headers.push_back(btFBTarget);

				Packet* ret = OBEX::PacketMaker::MakeConnectPacket(headers, maxPacketSize);

				delete btFBTarget;
				btFBTarget = 0;

				return ret;
			}

			static Packet* MakeDisconnectPacket(OBEX::Header* connectionIdHeader)
			{
				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				Packet* ret = OBEX::PacketMaker::MakeDisconnectPacket(headers);
				
				return ret;
			}

			static Packet* MakeSetPathForwardPacket(OBEX::Header* connectionIdHeader, const WString& nameVal)
			{
				byte flags = 0x02;
				byte constants = 0;

				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				OBEX::Header* nameHeader = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);
				headers.push_back(nameHeader);

				Packet* ret = OBEX::PacketMaker::MakeSetPathPacket(headers, flags, constants);	

				delete nameHeader;

				return ret;
			}

			static Packet* MakeSetPathBackwardPacket(OBEX::Header* connectionIdHeader)
			{
				byte flags = 0x03;
				byte constants = 0;

				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				Packet* ret = OBEX::PacketMaker::MakeSetPathPacket(headers, flags, constants);	

				return ret;
			}

			static Packet* MakeSetPathRootPacket(OBEX::Header* connectionIdHeader)
			{
				byte flags = 0x02;
				byte constants = 0;

				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				OBEX::Header* nameHeader = new Header(HeaderFieldCode::OBEX_HEADER_NAME, "");
				headers.push_back(nameHeader);

				Packet* ret = OBEX::PacketMaker::MakeSetPathPacket(headers, flags, constants);	

				delete nameHeader;

				return ret;
			}

			static Packet* MakeSetPathCreateFolderPacket(OBEX::Header* connectionIdHeader, const WString& nameVal)
			{
				byte flags = 0x00;
				byte constants = 0;

				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				OBEX::Header* nameHeader = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);
				headers.push_back(nameHeader);

				Packet* ret = OBEX::PacketMaker::MakeSetPathPacket(headers, flags, constants);	

				delete nameHeader;

				return ret;
			}

			static Packet* MakePutDeleteObjectPacket(OBEX::Header* connectionIdHeader, const WString& nameVal)
			{
				std::vector<Header*> headers;
				headers.push_back(connectionIdHeader);

				OBEX::Header* nameHeader = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);
				headers.push_back(nameHeader);

				Packet* ret = OBEX::PacketMaker::MakePutPacket(headers, true);	

				delete nameHeader;

				return ret;
			}

			static Packet* MakeGetFilePacket(OBEX::Header* connectionIdHeader, const WString& nameVal, bool final)
			{
				std::vector<Header*> headers;
				
				OBEX::Header* connId = connectionIdHeader;

				OBEX::Header* name = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);

				headers.push_back(connId); // rememeber, conn id must be first header
				headers.push_back(name);

				Packet* ret = OBEX::PacketMaker::MakeGetPacket(headers, final);	

				delete name;

				return ret;
			}

			static Packet* MakePutBodyPacket(OBEX::Header* connectionIdHeader, const WString& nameVal, NakedVector<byte>& bodyData, bool final)
			{
				std::vector<Header*> headers;
				
				OBEX::Header* connId = connectionIdHeader;
				OBEX::Header* name = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);
				OBEX::Header* body = new Header(HeaderFieldCode::OBEX_HEADER_BODY, bodyData);

				headers.push_back(connId); // rememeber, conn id must be first header
				headers.push_back(name);
				headers.push_back(body);

				Packet* ret = OBEX::PacketMaker::MakePutPacket(headers, final);	

				delete name;
				delete body;

				return ret;
			}

			static Packet* MakePutEndOfBodyPacket(OBEX::Header* connectionIdHeader, const WString& nameVal, NakedVector<byte>& endOfBodyData, bool final)
			{
				std::vector<Header*> headers;
				
				OBEX::Header* connId = connectionIdHeader;
				OBEX::Header* name = new Header(HeaderFieldCode::OBEX_HEADER_NAME, nameVal);
				OBEX::Header* body = new Header(HeaderFieldCode::OBEX_HEADER_END_OF_BODY, endOfBodyData);

				headers.push_back(connId); // rememeber, conn id must be first header
				headers.push_back(name);
				headers.push_back(body);

				Packet* ret = OBEX::PacketMaker::MakePutPacket(headers, final);	

				delete name;
				delete body;

				return ret;
			}

			static Packet* MakeGetFolderListingPacket(OBEX::Header* connectionIdHeader)
			{
				std::vector<Header*> headers;
				
				OBEX::Header* connId = connectionIdHeader;

				OBEX::Header* name = new Header(HeaderFieldCode::OBEX_HEADER_NAME, NakedVector<byte>());
				OBEX::Header* type = new Header(HeaderFieldCode::OBEX_HEADER_TYPE, "x-obex/folder-listing");

				headers.push_back(connId); // rememeber, conn id must be first header
				headers.push_back(name);
				headers.push_back(type);

				Packet* ret = OBEX::PacketMaker::MakeGetPacket(headers, true);	

				delete name;
				delete type;

				return ret;
			}

	};
}