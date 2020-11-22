#pragma once

#include <winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>

#include <vector>
#include "BTSockDevice.h"
#include "BTSockAddressString.h"

namespace BTSock
{
	class DeviceQuery
	{
		public:
			DeviceQuery(Socket* _sockForProtcolInfo) : sockForProtcolInfo(_sockForProtcolInfo)
			{ }

			bool PerformQuery(std::vector<BTSock::Device*>& outDevices, GUID svcUUID = L2CAP_PROTOCOL_UUID)
			{
				WSAQUERYSETW querySet;
				memset(&querySet, 0, sizeof(WSAQUERYSETW));
				querySet.dwSize = sizeof(WSAQUERYSETW);
				querySet.dwNameSpace = NS_BTH;

				DWORD flags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE | LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;
				HANDLE hLookup;

				WSALookupServiceBeginW(&querySet, flags, &hLookup);
				int result = 0;

				while (result == 0)
				{
					BYTE buffer[4096];
					DWORD bufferLength = sizeof(buffer);

					WSAQUERYSETW* pResults = (WSAQUERYSETW*)&buffer;

					result = WSALookupServiceNextW(hLookup, flags, &bufferLength, pResults);

					if (result != 0)
					{
						//
						// ! error !
						//
					   /*printf("%s\n", GetLastErrorMessage(GetLastError()));
					   printf("%d\n", bufferLength);*/
					}
					else
					{
						BTSock::Device* dev = new BTSock::Device();

						CSADDR_INFO *pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;
						BTH_DEVICE_INFO *pDeviceInfo = (BTH_DEVICE_INFO*)pResults->lpBlob;

						// get dev's name
						dev->name = pResults->lpszServiceInstanceName;

						// get local address
						wchar_t strLocalAddress[1024];
						DWORD sizeLocalAddress = sizeof(strLocalAddress);

						int ret = WSAAddressToStringW(pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength, &sockForProtcolInfo->protocolInfo, 
													  strLocalAddress, &sizeLocalAddress);

						if(ret == 0)
						{
						   dev->localAddress = BTSock::AddressString::RemoveParens(strLocalAddress);
						}
						else
						{
						   dev->localAddress = L"";
						}


						// get device address
						wchar_t strDeviceAddress[1024];
						DWORD sizeDeviceAddress = sizeof(strDeviceAddress);

						ret = WSAAddressToStringW(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength, &sockForProtcolInfo->protocolInfo, 
												  strDeviceAddress, &sizeDeviceAddress);

						if(ret == 0)
						{
							dev->deviceAddress = BTSock::AddressString::RemoveParens(strDeviceAddress);
						}
						else
						{
							dev->deviceAddress = L"";
						}

						if(dev->deviceAddress != L"")
						{
							PerformDeviceProfileQuery(dev, strDeviceAddress, svcUUID);
						}

						outDevices.push_back(dev);
					}
				}

				return true;
			}

		protected:
			Socket* sockForProtcolInfo;

			bool PerformDeviceProfileQuery(Device* dev, wchar_t* deviceAddress, GUID svcUUID)
			{
				WSAQUERYSETW querySet;
				memset(&querySet, 0, sizeof(WSAQUERYSETW));
				querySet.dwSize = sizeof(WSAQUERYSETW);
				querySet.dwNameSpace = NS_BTH;

				GUID proto = svcUUID;
				querySet.lpServiceClassId = &proto;
				querySet.lpszContext = deviceAddress;

				HANDLE hLookup;
				DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;

				int result = WSALookupServiceBeginW(&querySet, flags, &hLookup);

				while (result == 0)
				{
					BYTE buffer[2000];
					DWORD bufferLength = sizeof(buffer);

					WSAQUERYSETW *pResults = (WSAQUERYSETW*)&buffer;

					result = WSALookupServiceNextW(hLookup, flags, &bufferLength, pResults);

					if (result != 0)
					{
						//
						// ! ERROR !
						// 
						//printf("%s\n", GetLastErrorMessage(GetLastError()));
						//printf("%d\n", bufferLength);
					}
					else
					{
						CSADDR_INFO *pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;

						BTSock::DeviceProfile* devBTProfile = new BTSock::DeviceProfile();

						// profile name
						devBTProfile->name = pResults->lpszServiceInstanceName;


						// address:port
						wchar_t strAddress[1024];
						DWORD sizeAddress = sizeof(strAddress);

						int ret = WSAAddressToStringW(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength, &sockForProtcolInfo->protocolInfo, 
													  strAddress, &sizeAddress);

						if(ret == 0)
						{
							devBTProfile->address = strAddress;
							devBTProfile->port = BTSock::AddressString::GetPort(devBTProfile->address);

							dev->supportedProfiles.push_back(devBTProfile);
						}
						else
						{
							devBTProfile->address = L"";
							devBTProfile->port = -1;
						}
					}

				}

				return true;
			}

	};
}