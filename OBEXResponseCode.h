#pragma once

namespace OBEX
{
	namespace ResponseCode
	{
		enum ResponseCode
		{
			OBEX_SUCCESS = 0xA0,
			OBEX_CONTINUE = 0x90,
			OBEX_FORBIDDEN = 0xC3,
			OBEX_NOT_FOUND = 0xC4,
			OBEX_FAILURE_SERVICE_UNAVAILABLE = 0xD3,
		};
	}
}