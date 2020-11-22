#pragma once

#include <vector>

#include <winsock2.h>
#include <Ws2bth.h>
#include <BluetoothAPIs.h>

#include "BTSockRadio.h"

namespace BTSock
{
	class RadioQuery
	{
		protected:
			BTSock::Radio* ProcessRadio(HANDLE hRadio)
			{
				BTSock::Radio* theRadio = new BTSock::Radio();

				BLUETOOTH_DEVICE_INFO_STRUCT	deviceInfo;
				deviceInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO_STRUCT);

				BLUETOOTH_RADIO_INFO	radioInfo;
				radioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

				if (BluetoothGetRadioInfo(hRadio, &radioInfo) == ERROR_SUCCESS)
				{
					theRadio->name = radioInfo.szName;
				}

				BLUETOOTH_DEVICE_SEARCH_PARAMS deviceSearchParams;
				memset(&deviceSearchParams, 0, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));
				deviceSearchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
				deviceSearchParams.hRadio = hRadio;
				deviceSearchParams.fReturnRemembered = true;
				//deviceSearchParams.fReturnAuthenticated = true;
				//deviceSearchParams.fReturnUnknown = true;
				//deviceSearchParams.fReturnConnected = true;

				// find devices associated w/ radio
				HANDLE hDeviceFind = BluetoothFindFirstDevice(&deviceSearchParams, &deviceInfo);

				if (hDeviceFind != NULL)
				{
					do
					{
						theRadio->devices.push_back(deviceInfo.szName);

						// get installed services
						//DWORD numServices = sizeof(theRadio->guidServices);
						//DWORD result = BluetoothEnumerateInstalledServices(hRadio, &deviceInfo, &numServices, theRadio->guidServices);

						// get device into 
						// wtf does this do? we already have the dev info!
						//if (BluetoothGetDeviceInfo(hRadio, &deviceInfo))
						//{
						//	wprintf(L"+ Device: %s\n", deviceInfo.szName);
						//}

						// to display control panel bluetooth settings
						//BluetoothDisplayDeviceProperties(0, &deviceInfo);

					}
					while(BluetoothFindNextDevice(hDeviceFind, &deviceInfo));

					BluetoothFindDeviceClose(hDeviceFind);
				}

				return theRadio;
			}

		public:
			RadioQuery()
			{ }

			bool PerformQuery(std::vector<BTSock::Radio*>& radios)
			{
				HANDLE							hRadio;
				BLUETOOTH_FIND_RADIO_PARAMS		btfrp = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };

				HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio( &btfrp, &hRadio );

				if(hFind == NULL)
					return true;

				do
				{
					
					radios.push_back( ProcessRadio(hRadio) );

				}while( BluetoothFindNextRadio( hFind, &hRadio ) );

				BluetoothFindRadioClose( hFind );

				return true;
			}
	};
}