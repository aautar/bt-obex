#pragma once

#include <string>

namespace BTSock
{
	struct DeviceProfile
	{
		std::wstring	name;
		std::wstring	address; // (address):port, typically repeat of device address
		int				port;
	};
}