#pragma once

#include <string>
#include <sstream>

namespace BTSock
{
	class AddressString
	{
		public:
			// does not support addresses w/ port
			static std::wstring RemoveParens(const std::wstring& btAddress)
			{
				if(btAddress[0] != '(' || btAddress[btAddress.length()-1] != ')')
					return btAddress;

				std::wstring result = btAddress.substr(1, btAddress.length()-2);
				return result;
			}

			// get port from (address):port string
			static int GetPort(const std::wstring& btAddress)
			{
				int i = btAddress.find_last_of(':');
				std::wstring port = btAddress.substr(i+1);

				return WStrToInt(port);
			}

			static int WStrToInt(const std::wstring& wstr)
			{
				int ret = -1;
				std::wstringstream wss(wstr);
				wss >> ret;

				return ret;
			}

	};
}