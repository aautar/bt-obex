#pragma once

#include "OBEXNakedVector.h"
#include "OBEXTypes.h"

namespace OBEX
{
	namespace HeaderFieldCode
	{
		enum HeaderFieldCode
		{
			OBEX_HEADER_NAME = 0x01,
			OBEX_HEADER_LENGTH = 0xC3,
			OBEX_HEADER_CONNECTION_ID = 0xCB,
			OBEX_HEADER_DESCRIPTION = 0x05,
			OBEX_HEADER_PALM_CREATOR_ID = 0xCF,
			OBEX_HEADER_TYPE = 0x42,
			OBEX_HEADER_WHO = 0x4A,
			OBEX_HEADER_TARGET = 0x46,
			OBEX_HEADER_BODY = 0x48,
			OBEX_HEADER_END_OF_BODY = 0x49,
		};
	}

	class Header
	{
		public:
			Header(HeaderFieldCode::HeaderFieldCode _fieldCode, const NakedVector<byte>& _data) : allBytes(0)
			{ 
				fieldCode = _fieldCode;
				data.PushBack(_data);
				totalSize = data.Count() + 3;

				if(fieldCode == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID)
					totalSize = data.Count() + 1;
			}

			Header(HeaderFieldCode::HeaderFieldCode _fieldCode, int _data) : allBytes(0)
			{ 
				fieldCode = _fieldCode;

				data.PushBack( ((byte*)&_data)[3] );
				data.PushBack( ((byte*)&_data)[2] );
				data.PushBack( ((byte*)&_data)[1] );
				data.PushBack( ((byte*)&_data)[0] );

				totalSize = data.Count() + 3;

				if(fieldCode == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID)
					totalSize = data.Count() + 1;
			}

			Header(HeaderFieldCode::HeaderFieldCode _fieldCode, const String& _data) : allBytes(0)
			{ 
				fieldCode = _fieldCode;

				for(size_t i=0; i<_data.length(); i++)
				{
					data.PushBack( (byte)_data[i] );
				}
				data.PushBack(0); // null term

				totalSize = data.Count() + 3;

				if(fieldCode == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID)
					totalSize = data.Count() + 1;
			}

			Header(HeaderFieldCode::HeaderFieldCode _fieldCode, const WString& _data) : allBytes(0)
			{ 
				fieldCode = _fieldCode;

				for(size_t i=0; i<_data.length(); i++)
				{
					data.PushBack( _data[i] >> 8 );
					data.PushBack( _data[i] & 0xff );
				}

				data.PushBack(0); // null term
				data.PushBack(0); // null term
				data.PushBack(0); // null term
				data.PushBack(0); // null term

				totalSize = data.Count() + 3;

				if(fieldCode == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID)
					totalSize = data.Count() + 1;
			}


			Header(HeaderFieldCode::HeaderFieldCode _fieldCode, byte* _dataBytes, int _dataLength) : allBytes(0)
			{ 
				fieldCode = _fieldCode;
				data.PushBack(_dataBytes, _dataLength);
				totalSize = data.Count() + 3;

				if(fieldCode == HeaderFieldCode::OBEX_HEADER_CONNECTION_ID)
					totalSize = data.Count() + 1;
			}

			~Header()
			{
				if(allBytes != 0)
				{
					delete allBytes;
					allBytes = 0;
				}
			}

			int GetSize() 
			{ 
				return totalSize;
			}

			int GetDataSize()
			{
				return data.Count();
			}

			NakedVector<byte>* DataToBytes()
			{
				return &data;
			}

			// note, returns ptr to internal location
			NakedVector<byte>* ToBytes()
			{
				if(allBytes != 0)
				{
					delete allBytes;
					allBytes = 0;
				}

				allBytes = new NakedVector<byte>();

				NakedVector<byte>* result = allBytes;

				result->PushBack((byte)fieldCode);

				if(fieldCode != HeaderFieldCode::OBEX_HEADER_CONNECTION_ID) // conn-id doesn't have size field
				{
					result->PushBack((byte)(totalSize >> 8));
					result->PushBack((byte)(totalSize & 0xff));
				}

				result->PushBack(data);

				return result;
			}

			static Header* MakeFileBrowsingUUIDTargetHeader()
			{
				NakedVector<byte> fileBrowsingUUID;

				fileBrowsingUUID.PushBack(0xF9); 
				fileBrowsingUUID.PushBack(0xEC); 
				fileBrowsingUUID.PushBack(0x7B); 
				fileBrowsingUUID.PushBack(0xC4); 
				fileBrowsingUUID.PushBack(0x95); 
				fileBrowsingUUID.PushBack(0x3C); 
				fileBrowsingUUID.PushBack(0x11); 
				fileBrowsingUUID.PushBack(0xD2); 
				fileBrowsingUUID.PushBack(0x98); 
				fileBrowsingUUID.PushBack(0x4E); 
				fileBrowsingUUID.PushBack(0x52); 
				fileBrowsingUUID.PushBack(0x54); 
				fileBrowsingUUID.PushBack(0x00); 
				fileBrowsingUUID.PushBack(0xDC); 
				fileBrowsingUUID.PushBack(0x9E); 
				fileBrowsingUUID.PushBack(0x09); 

				Header* result = new Header(HeaderFieldCode::OBEX_HEADER_TARGET, fileBrowsingUUID);
				return result;
			}

		protected:
			HeaderFieldCode::HeaderFieldCode	fieldCode;
			int									totalSize;
			NakedVector<byte>					data;
			NakedVector<byte>*					allBytes; // entire head in bytes

	};
}