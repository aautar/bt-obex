#pragma once

#include "BTSock.h"
#include "OBEX.h"
#include "Win32FileSize.h"
#include "BTOBEXProgress.h"

namespace BTOBEX
{
	class Session
	{
		public:
			Session() : btSock(0), btSession(0), connId(0), maxPacketSize(0)
			{
				btSession = new BTSock::Session();
				btSock = new BTSock::Socket();
			}

			~Session()
			{
				delete btSock;
				delete btSession;
			}


			bool Connect(const std::wstring& devAddress, int port, int _maxPacketSize=2048)
			{
				maxPacketSize = _maxPacketSize;

				bool connOk = btSession->Connect(btSock, devAddress, port);

				if(!connOk)
					return false;

				OBEX::Packet* connectPckt = OBEX::BluetoothFtpPacketMaker::MakeConnectPacket(maxPacketSize);
				int ret = btSession->Send(btSock, (char*)connectPckt->ToBytes()->Ptr(), connectPckt->NumBytes());
				delete connectPckt;

				if(ret <= 0)
					return false;

				char* recvBuf = new char[maxPacketSize];

				ret = btSession->Recv(btSock, recvBuf, maxPacketSize);
				OBEX::Packet* recvPacket = new OBEX::Packet((OBEX::byte*)recvBuf, ret);
				
				// get and store connection id
				connId = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_CONNECT, recvPacket, OBEX::HeaderFieldCode::OBEX_HEADER_CONNECTION_ID);
				
				delete recvPacket;

				delete [] recvBuf;

				return true;
			}

			void Disconnect()
			{
				if(connId == 0)
					return;

				char* recvBuf = new char[maxPacketSize];

				OBEX::Packet* disconnectPacket = OBEX::BluetoothFtpPacketMaker::MakeDisconnectPacket(connId);
				int numBytesSent = btSession->Send(btSock, (char*)disconnectPacket->ToBytes()->Ptr(), disconnectPacket->NumBytes());
				int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);

				delete [] recvBuf;
			}

			bool SetPathBackward()
			{
				if(connId == 0)
					return false;

				OBEX::Packet* setPathPckt = OBEX::BluetoothFtpPacketMaker::MakeSetPathBackwardPacket(connId);
				
				int numBytesSent = btSession->Send(btSock, (char*)setPathPckt->ToBytes()->Ptr(), setPathPckt->NumBytes());
				
				if(numBytesSent <= 0)
					return false;

				char* recvBuf = new char[maxPacketSize];

				int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);
				delete [] recvBuf;
				
				delete setPathPckt;

				if(numBytesRecv <= 0)
				{
					return false;
				}

				// todo: construct packet from recv data to verify op was successful

				return true;
			}

			bool SetPathForward(const std::wstring& nameVal)
			{
				if(connId == 0)
					return false;

				OBEX::Packet* setPathPckt = OBEX::BluetoothFtpPacketMaker::MakeSetPathForwardPacket(connId, nameVal);
				
				int numBytesSent = btSession->Send(btSock, (char*)setPathPckt->ToBytes()->Ptr(), setPathPckt->NumBytes());
				
				if(numBytesSent <= 0)
					return false;

				char* recvBuf = new char[maxPacketSize];

				int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);
				delete [] recvBuf;
				
				delete setPathPckt;

				if(numBytesRecv <= 0)
				{
					return false;
				}

				// todo: construct packet from recv data to verify op was successful

				return true;
			}

			bool GetFolderListing(OBEX::NakedVector<OBEX::byte>& result)
			{
				if(connId == 0)
					return false;

				OBEX::Packet* folderListPacket = OBEX::BluetoothFtpPacketMaker::MakeGetFolderListingPacket(connId);
				int numBytesSent = btSession->Send(btSock, (char*)folderListPacket->ToBytes()->Ptr(), folderListPacket->NumBytes());

				if(numBytesSent <= 0)
				{
					delete folderListPacket;
					return false;
				}

				bool ret = true;
				char* recvBuf = new char[maxPacketSize];

				while(true)
				{
					int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);
					if(numBytesRecv <= 0)
					{
						ret = false;
						break;
					}

					OBEX::Packet* flPacket = new OBEX::Packet((OBEX::byte*)recvBuf, numBytesRecv);
					
					OBEX::Header* bodyHead = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_GET, flPacket, OBEX::HeaderFieldCode::OBEX_HEADER_BODY);
					OBEX::Header* endBodyHead = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_GET, flPacket, OBEX::HeaderFieldCode::OBEX_HEADER_END_OF_BODY);

					if(bodyHead != NULL)
						result.PushBack((*bodyHead->DataToBytes()));
					else if(endBodyHead != NULL)
						result.PushBack((*endBodyHead->DataToBytes()));
					else
					{ }

					if(flPacket->GetOpCode() != OBEX::ResponseCode::OBEX_CONTINUE)
					{
						ret = true;
						delete flPacket;
						break;
					}

					numBytesSent = btSession->Send(btSock, (char*)folderListPacket->ToBytes()->Ptr(), folderListPacket->NumBytes());
					delete flPacket;

					if(numBytesSent <= 0)
					{
						ret = false;
						break;
					}
				}

				delete folderListPacket;

				delete [] recvBuf;
				return ret;
			}

			bool DeleteObject(const std::wstring& objName)
			{
				if(connId == 0)
					return false;

				char* recvBuf = new char[maxPacketSize];

				OBEX::Packet* delPckt = OBEX::BluetoothFtpPacketMaker::MakePutDeleteObjectPacket(connId, objName.c_str());
				int numBytesSent = btSession->Send(btSock, (char*)delPckt->ToBytes()->Ptr(), delPckt->NumBytes());
				int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);
				delete delPckt;

				delete [] recvBuf;

				return true;
			}

			bool PutFile(const std::wstring& remoteFileName, const std::wstring& localFilePath, BTOBEX::Progress* progress = 0)
			{
				if(connId == 0)
					return false;

				if(progress != 0)
				{
					progress->current = 0;
					progress->total = (int)Win32::FileSize::GetSize(localFilePath);
				}

				bool ret = true;
				char* recvBuf = new char[maxPacketSize];

				FILE* fp = _wfopen(localFilePath.c_str(), L"rb");

				for(int i=0;;i++)
				{
					OBEX::Packet* putFilePckt = 0;

					char buf[1024];
					int numBytesRead = fread(buf, 1, 1024, fp);

					OBEX::NakedVector<OBEX::byte> putBytes;
					putBytes.PushBack((OBEX::byte*)buf, numBytesRead);

					if(numBytesRead < 1024)
						putFilePckt = OBEX::BluetoothFtpPacketMaker::MakePutEndOfBodyPacket(connId, remoteFileName.c_str(), putBytes, true);
					else
						putFilePckt = OBEX::BluetoothFtpPacketMaker::MakePutBodyPacket(connId, remoteFileName.c_str(), putBytes, false);

					int numBytesSent = btSession->Send(btSock, (char*)putFilePckt->ToBytes()->Ptr(), putFilePckt->NumBytes());
					delete putFilePckt;

					if(progress != 0)
					{
						progress->current += numBytesRead;
					}

					ZeroMemory(recvBuf, maxPacketSize);
					int numBytesRecv = btSession->Recv(btSock, recvBuf, maxPacketSize);
					int expectedBytes = (((unsigned char*)recvBuf)[1] << 8) | (((unsigned char*)recvBuf)[2]);
					if(numBytesRecv < expectedBytes)
					{
						numBytesRecv += btSession->Recv(btSock, recvBuf+numBytesRecv, maxPacketSize-numBytesRecv, expectedBytes-numBytesRecv);
					}

					OBEX::Packet* recvPacket = new OBEX::Packet((OBEX::byte*)recvBuf, numBytesRecv);

					bool done = false;
					if(recvPacket->GetOpCode() != (OBEX::byte)OBEX::ResponseCode::OBEX_CONTINUE)
					{
						done = true;
					}

					delete recvPacket;
					if(done)
					{
						break;
					}
				}

				fclose(fp);

				delete [] recvBuf;

				return ret;
			}

			bool GetFile(const std::wstring& remoteFileName, const std::wstring& localFilePath, BTOBEX::Progress* progress = 0)
			{
				if(connId == 0)
					return false;

				if(progress != 0)
				{
					progress->current = 0;
					progress->total = -1;
				}

				bool ret = true;
				char* recvBuf = new char[maxPacketSize];

				FILE* fp = _wfopen(localFilePath.c_str(), L"wb");

				if(fp == 0)
					return false;

				for(int i=0;;i++)
				{
					OBEX::Packet* getFilePckt = 0;

					if(i == 0)
						getFilePckt = OBEX::BluetoothFtpPacketMaker::MakeGetFilePacket(connId, remoteFileName.c_str(), false);
					else
						getFilePckt = OBEX::BluetoothFtpPacketMaker::MakeGetFilePacket(connId, remoteFileName.c_str(), true);

					int numBytesSent = btSession->Send(btSock, (char*)getFilePckt->ToBytes()->Ptr(), getFilePckt->NumBytes());
					delete getFilePckt;

					if(numBytesSent <= 0)
					{
						ret = false;
						break;
					}

					ZeroMemory(recvBuf, maxPacketSize);
					int numBytesRecv = btSession->Recv(btSock, recvBuf, 3, 3);
					int expectedBytes = (((unsigned char*)recvBuf)[1] << 8) | (((unsigned char*)recvBuf)[2]);
					if(numBytesRecv < expectedBytes)
					{
						numBytesRecv += btSession->Recv(btSock, recvBuf+numBytesRecv, maxPacketSize-numBytesRecv, expectedBytes-numBytesRecv);
					}

					if(numBytesRecv != expectedBytes) // typically recv error
					{
						ret = false;
						break;
					}

					OBEX::Packet* fileRecvPacket = new OBEX::Packet((OBEX::byte*)recvBuf, numBytesRecv);

					OBEX::Header* lenHead = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_GET, fileRecvPacket, OBEX::HeaderFieldCode::OBEX_HEADER_LENGTH);
					OBEX::Header* bodyHead = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_GET, fileRecvPacket, OBEX::HeaderFieldCode::OBEX_HEADER_BODY);
					OBEX::Header* endBodyHead = OBEX::ResponsePacketParser::GetHeader(OBEX::RequestCode::OBEX_GET, fileRecvPacket, OBEX::HeaderFieldCode::OBEX_HEADER_END_OF_BODY);

					if(lenHead != NULL)
					{
						OBEX::NakedVector<OBEX::byte>& lenBytes = *(lenHead->DataToBytes());
						unsigned int fileSize = (lenBytes[0] << 24) | (lenBytes[1] << 16) | (lenBytes[2] << 8) | lenBytes[3];

						if(progress != 0)
						{
							progress->total = (int)fileSize;
						}
					}

					int bytesWrittenToDisk = 0;
					if(bodyHead != 0)
					{
						fwrite(bodyHead->DataToBytes()->Ptr(), sizeof(OBEX::byte), bodyHead->GetDataSize(), fp);
						bytesWrittenToDisk = bodyHead->GetDataSize();
					}
					else if(endBodyHead != 0)
					{
						fwrite(endBodyHead->DataToBytes()->Ptr(), sizeof(OBEX::byte), endBodyHead->GetDataSize(), fp);
						bytesWrittenToDisk = endBodyHead->GetDataSize();
					}
					else 
					{ }

					if(progress != 0)
					{
						progress->current += bytesWrittenToDisk;
					}

					bool done = false;
					if(fileRecvPacket->GetOpCode() != (OBEX::byte)OBEX::ResponseCode::OBEX_CONTINUE)
					{
						done = true;
					}

					delete fileRecvPacket;
					fileRecvPacket = 0;

					if(done)
					{
						break;
					}					
				}

				fclose(fp);

				delete [] recvBuf;

				return ret;
			}


		protected:
			int maxPacketSize;
			OBEX::Header* connId;
			BTSock::Socket* btSock;
			BTSock::Session* btSession;
	};
}