#pragma once

#include <vector>
#include <string>

#include "BTSockDeviceProfile.h"

namespace BTSock
{
	struct Device
	{
		~Device()
		{
			name = L"";
			localAddress = L"";
			deviceAddress = L"";

			for(size_t i=0; i<supportedProfiles.size(); i++)
			{
				delete supportedProfiles[i];
				supportedProfiles[i] = 0;
			}

			supportedProfiles.clear();
		}

		std::wstring							name;
		std::wstring							localAddress;
		std::wstring							deviceAddress;
		std::vector<BTSock::DeviceProfile*>		supportedProfiles;
	};
}