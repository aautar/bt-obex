// Required by BTOBEX.h

#pragma once

#include <string>
#include <windows.h>

namespace Win32
{
	class FileSize
	{
		public:
			static __int64 GetSize(const std::wstring& fullpath)
			{
				HANDLE hFile = CreateFileW(fullpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
				
				if(hFile == INVALID_HANDLE_VALUE)
					return -1;

				LARGE_INTEGER	size;
				if(::GetFileSizeEx(hFile, &size) == 0)
					return -1;

				return size.QuadPart;
			}
	};
}