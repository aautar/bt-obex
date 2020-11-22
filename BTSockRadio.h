#pragma once

#include <windows.h>
#include <vector>
#include <string>

namespace BTSock
{
	struct Radio
	{
		std::wstring				name;
		std::vector<std::wstring>	devices;
	};
}