#pragma once

namespace OBEX
{
	namespace RequestCode
	{
		enum RequestCode
		{
			OBEX_CONNECT = 0x80,
			OBEX_DISCONNECT = 0x81,
			OBEX_GET = 0x03,
			OBEX_GET_FINAL = 0x83,
			OBEX_PUT = 0x02,
			OBEX_PUT_FINAL = 0x82,
			OBEX_SET_PATH = 0x85,
			OBEX_ABORT = 0xFF,
		};
	}
}