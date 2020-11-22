#pragma once

#include <winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>

namespace BTSock
{
	class Session;
	class DeviceQuery;

	class Socket
	{
		friend class Session;
		friend class DeviceQuery;

		public:
			Socket()
			{
				int protocolInfoSize = sizeof(protocolInfo);
				btSock = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM); 	
				getsockopt(btSock, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize);
			}

			~Socket()
			{
				closesocket(btSock);
			}

		protected:
			SOCKET				btSock;
			WSAPROTOCOL_INFOW	protocolInfo;

	};
}
