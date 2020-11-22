#include <string>
#include <winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>

#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "irprops.lib")

#include "BTSockSocket.h"

namespace BTSock
{
	class Session
	{
		public:
			Session() : wsVersionRequested(0x202), devQueryObj(0), socketForDevQuery(0), radioQueryObj(0)
			{
				WSAStartup(wsVersionRequested, &wsaData);
			}

			~Session()
			{
				if(devQueryObj != 0)
				{
					delete socketForDevQuery;
					delete devQueryObj;
				}

				if(radioQueryObj != 0)
				{
					delete radioQueryObj;
				}

				WSACleanup();
			}
			
			BTSock::RadioQuery* RadioQueryObject()
			{
				if(radioQueryObj == 0)
					radioQueryObj = new BTSock::RadioQuery();

				return radioQueryObj;
			}

			BTSock::DeviceQuery* DeviceQueryObject()
			{
				if(devQueryObj == 0)
				{
					socketForDevQuery = new BTSock::Socket();
					devQueryObj = new BTSock::DeviceQuery(socketForDevQuery);
				}

				return devQueryObj;
			}

			bool Connect(Socket* sock, const std::wstring& devAddress, int port)
			{
				BTH_ADDR		btAddr = 0;
				BTAddressFromString(devAddress, btAddr);

				SOCKADDR_BTH	sockAddr = {0};
				ZeroMemory(&sockAddr, sizeof(SOCKADDR_BTH));
				sockAddr.addressFamily = AF_BTH;
				sockAddr.btAddr = btAddr;
				sockAddr.serviceClassId = GUID_NULL;
				sockAddr.port = port;

				int ret = ::connect(sock->btSock, (sockaddr*)&sockAddr, sizeof(SOCKADDR_BTH));

				if(ret == 0)
					return true;

				return false;
			}

			int Send(Socket* sock, char* buf, int numBytes)
			{
				int ret = ::send(sock->btSock, buf, numBytes, 0);
				return ret;
			}

			int Recv(Socket* sock, char* buf, int bufSize, int expectedBytes = INT_MAX)
			{
				int numBytesRecv = 0;

				while(numBytesRecv < expectedBytes)
				{
					int ret = ::recv(sock->btSock, buf+numBytesRecv, bufSize-numBytesRecv, 0);

					if(ret <= 0)
					{
						numBytesRecv = ret;
						break; // error or disconnect
					}

					numBytesRecv += ret;

					if(expectedBytes == INT_MAX)
						break;
				}

				return numBytesRecv;
			}

		protected:
			WSADATA					wsaData;
			WORD					wsVersionRequested;
			BTSock::RadioQuery*		radioQueryObj;
			BTSock::DeviceQuery*	devQueryObj;
			BTSock::Socket*			socketForDevQuery;

			static void BTAddressFromString(const std::wstring& devAddress, BTH_ADDR& result)
			{
				BTH_ADDR	btAddrTmp = 0;
				ULONG		btAddrPiece[6];

				swscanf(devAddress.c_str(), L"%02x:%02x:%02x:%02x:%02x:%02x",
					    &btAddrPiece[0],&btAddrPiece[1],&btAddrPiece[2],&btAddrPiece[3],&btAddrPiece[4],&btAddrPiece[5]);

				for(int i=0; i<6; i++)
				{
					btAddrTmp = (BTH_ADDR)( btAddrPiece[i] & 0xFF );
					result = (result << 8) + btAddrTmp;
				}
			}

	};
}