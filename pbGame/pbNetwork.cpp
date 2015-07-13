#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#include <WinSock2.h>
#include <time.h>
#pragma comment(lib,"wsock32.lib")

#include "pbNetwork.h"

const unsigned short pbNetwork::PORT_NUMBER = 10001;

//
// ====== class clientSocketInfo ======
//
void pbClientSocketInfo::initClientSocketInfo(SOCKET socket, const char* ipAddress)
{
	mSocket = socket;
	mIpAddress = ipAddress;
	mLive = true;
	mRecvBufferMutexHandle = CreateMutex(NULL, FALSE, NULL);
	mRecvThreadHandle = NULL;
}

void pbClientSocketInfo::runRecvThread()
{
	
	mRecvThreadHandle = CreateThread(NULL, 0, recvThread, (LPVOID)this, 0, NULL);
}

void pbClientSocketInfo::releaseClientSocketInfo()
{
	closesocket(mSocket);
	CloseHandle(mRecvBufferMutexHandle);
}

bool pbClientSocketInfo::isRecvThreadRunning()
{
	if ( mRecvThreadHandle == NULL )
		return false;
	else
		return true;
}

bool pbClientSocketInfo::isLive()
{
	return mLive;
}


bool pbClientSocketInfo::isDisconnectInfoSend()
{
	if ( !mLive )
	{
		if ( mDisconnectInfoSendFlag )
			return true;
		else
			return false;
	}
	return true;
}

void pbClientSocketInfo::setDisconnect()
{
	mLive = false;
}

void pbClientSocketInfo::setDisconnectInfoSend(bool flag)
{
	mDisconnectInfoSendFlag = flag;
}

string pbClientSocketInfo::getAndClearRecvBuffer()
{
	string temp;
	lockRecvBufferMutex();
	temp = mRecvBuffer;
	mRecvBuffer.clear();
	unlockRecvBufferMutex();

	return temp;
}



void pbClientSocketInfo::addRecvBuffer(string str)
{
	lockRecvBufferMutex();
	mRecvBuffer += str;
	unlockRecvBufferMutex();
}

SOCKET pbClientSocketInfo::getSocket()
{
	return mSocket;
}

pbClientSocketInfo::pbClientSocketInfo()
{
	mDisconnectInfoSendFlag = false;
	mLive = true;
}

DWORD WINAPI pbClientSocketInfo::recvThread(LPVOID lpParameter)
{
	pbClientSocketInfo* socketInfo = (pbClientSocketInfo*)lpParameter;

	socketInfo->realRecvThread();
	
	return 0;
}

void pbClientSocketInfo::realRecvThread()
{
	char tempRecv[1024];
	int recvLen;
	while( mLive )
	{
		recvLen = recv(mSocket, tempRecv, 1024, 0);
		if ( recvLen == SOCKET_ERROR )
		{
			printf("disconnect in recv\n");
			setDisconnect();
		}
		else
		{
			addRecvBuffer(string(tempRecv,recvLen));
		}
	}

}


DWORD pbClientSocketInfo::lockRecvBufferMutex(DWORD miliseconds)
{
	return WaitForSingleObject(mRecvBufferMutexHandle, miliseconds);
}

void pbClientSocketInfo::unlockRecvBufferMutex()
{
	ReleaseMutex(mRecvBufferMutexHandle);
}


//
// ====== class pbNetwork ======
//
void pbNetwork::setNetworkType()
{
	int networkTypeInput;

	while(true)
	{
		cout << "  === menu ===" <<endl
			 << "  1. host"      <<endl
			 << "  2. client"    <<endl
			 << "  3. single test" << endl
			 << "  input : ";
		cin  >> networkTypeInput;
		cout << endl;
		
		if ( networkTypeInput == 1 )
		{
			mNetworkType = HOST;
			return;
		}
		else if (networkTypeInput == 2)
		{
			mNetworkType = CLIENT;
			return;
		}
		else if ( networkTypeInput == 3 )
		{
			mNetworkType = SINGLE;
			return;
		}
	}
}

int pbNetwork::getNewPlayerId()
{
	int clientNum = mClientSocketInfoVector.size();
	for( int i = 0; i < clientNum; i++)
	{
		if ( i == mMyPlayerIndex )
			continue;
		if ( mClientSocketInfoVector[i].isRecvThreadRunning() == false )
		{
			return i;
		}
	}

	return -1;
}

int pbNetwork::getDisconnectPlayerId()
{
	int clientNum = mClientSocketInfoVector.size();

	for( int i = 0; i < clientNum; i++)
	{
		if ( mClientSocketInfoVector[i].isDisconnectInfoSend() == false )
		{
			mClientSocketInfoVector[i].setDisconnectInfoSend(true);
			return i;
		}
	}

	return -1;
}


NetworkType pbNetwork::getNetworkType()
{
	return mNetworkType;
}

int pbNetwork::createThread()
{

	if ( mNetworkType == HOST)
		mAcceptThreadHandle = CreateThread(NULL, 0, acceptThread, (LPVOID)this, 0, NULL);

	mSendThreadHandle = CreateThread(NULL, 0, sendThread, (LPVOID)this, 0, NULL);
	return 0;
}



pbNetwork::pbNetwork()
{

	mSendBufferMutex = CreateMutex(NULL, FALSE, NULL);
}

pbNetwork::~pbNetwork()
{
	if ( mNetworkType != SINGLE )
	{
		closesocket(hostSocket);

		int clientNum = mClientSocketInfoVector.size();
		for( int i = 0; i < clientNum; i++)
		{
			mClientSocketInfoVector[i].releaseClientSocketInfo();
		}


		/*
		if ( mNetworkType == HOST )
			CloseHandle(mAcceptThreadHandle);

		CloseHandle(mRecvThreadHandle);
		CloseHandle(mSendThreadHandle);
		*/
		releaseNetwork();
	}
}

int pbNetwork::initNetwork()
{
	WSADATA wsaData;
	if(WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		cout << "WSAStartup fail" << endl;
		return -1;
	}

	return 0;
}

int pbNetwork::releaseNetwork()
{
	WSACleanup();
	return 0;
}


void pbNetwork::sendInitInfo(int newPlayerId)
{
	if ( newPlayerId == -1 )
		return;

	// 플레이어 위치 ( send buffer)
	// 유령 위치 (send buffer)
	// 위치 길이( init send의 길이 )
	// 클라 아이디 : i
	
	char tempBuffer[16];

	// send buffer lock
	WaitForSingleObject(mSendBufferMutex, INFINITE);
	int sendBufferLen = mSendBuffer.size();
	string sendBuffer;
	sprintf_s(tempBuffer, "%04d,%08d,",newPlayerId, sendBufferLen);
	sendBuffer += tempBuffer;
	sendBuffer += mSendBuffer;
	
	// send buffer unlock
	ReleaseMutex(mSendBufferMutex);

	// send initial info
	send(mClientSocketInfoVector[newPlayerId].getSocket(), sendBuffer.c_str(), sendBufferLen + 14, 0);
	
	// recv thread run
	mClientSocketInfoVector[newPlayerId].runRecvThread();

}


void pbNetwork::recvInitInfo()
{
	char recvBuffer[1024];
	string recvString;
	int recvLen;

	while ( recvString.size() < 14 )
	{
		recvLen = recv(mClientSocketInfoVector[0].getSocket(), recvBuffer, 1024, 0);
		recvString += string(recvBuffer, recvLen);
	}

	unsigned int initRecvLen;
	sscanf_s(recvString.c_str(), "%d,%ud,", &mMyPlayerIndex, &initRecvLen);
	recvString = recvString.substr(14);

	while ( recvString.size() < initRecvLen )
	{
		recvLen = recv(mClientSocketInfoVector[0].getSocket(), recvBuffer, 1024, 0);
		recvString += string(recvBuffer, recvLen);
	}
	
	mClientSocketInfoVector[0].addRecvBuffer(recvString);

}

int pbNetwork::getMyPlayerId()
{
	return mMyPlayerIndex;
}

void pbNetwork::connectToHost()
{
	string hostIpAddress;
	struct sockaddr_in hostAddr;
	SOCKET tempSocket;

	cout << "  input host ip address " << endl << "  ";
	cin >> hostIpAddress;

	tempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = inet_addr(hostIpAddress.c_str());
	hostAddr.sin_port = htons(PORT_NUMBER);

	if( connect(tempSocket, (struct sockaddr*)&hostAddr, sizeof(hostAddr)) != 0)
	{
		cout << "  connect to host fail" << endl;
		return;
	}
	else
	{
		cout << "  connect to host success" << endl
			 << "  === host info ===" << endl
			 << "  ip address : " << inet_ntoa(hostAddr.sin_addr) << endl << endl;
	}

	mClientSocketInfoVector.push_back(pbClientSocketInfo());
	mClientSocketInfoVector[0].initClientSocketInfo(tempSocket, hostIpAddress.c_str());
	recvInitInfo();
	mClientSocketInfoVector[0].runRecvThread();

	return;
}


void pbNetwork::setSendBuffer(string str)
{
	WaitForSingleObject(mSendBufferMutex, INFINITE);
 	mSendBuffer.clear();
	mSendBuffer = str;
	ReleaseMutex(mSendBufferMutex);
}


vector<string> pbNetwork::getRecvBufferVector()
{
	vector<string> temp;

	int clientNum = mClientSocketInfoVector.size();
	for ( int i = 0; i < clientNum; i++)
	{
		temp.push_back(mClientSocketInfoVector[i].getAndClearRecvBuffer());
	}

	return temp;
}

DWORD WINAPI pbNetwork::acceptThread(LPVOID lpParameter)
{
	pbNetwork* network = (pbNetwork*)lpParameter;
	network->realAcceptThread();
	
	return 0;
}

DWORD WINAPI pbNetwork::sendThread(LPVOID lpParameter)
{
	pbNetwork* network = (pbNetwork*)lpParameter;

	network->realSendThread();

	return 0;

}
void pbNetwork::initHost()
{
	mMyPlayerIndex = 0;
	pbClientSocketInfo hostClientSocketInfo;
	mClientSocketInfoVector.push_back(hostClientSocketInfo);
}

void pbNetwork::realAcceptThread()
{
	//
	// host network init
	//
	hostSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( hostSocket == INVALID_SOCKET)
	{
		cout << "socket create fail" << endl;
		return;
	}

	struct sockaddr_in hostAddr;
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(PORT_NUMBER);
	hostAddr.sin_addr.s_addr = htonl(ADDR_ANY);
	
	if( bind(hostSocket, (struct sockaddr*)&hostAddr, sizeof(hostAddr)) == SOCKET_ERROR)
	{
		cout << "socket bind fail" << endl;
		return;
	}
	
	if( listen(hostSocket, SOMAXCONN) != 0)
	{
		cout << "socket listen fail" << endl;
		return;
	
	}

	struct sockaddr_in cliAddr;
	int cliAddrLen = sizeof(cliAddr);
	
	//
	// accept root
	//
	while(true)
	{
		SOCKET tempSocket = accept(hostSocket, (struct sockaddr*)&cliAddr, &cliAddrLen);
		if ( tempSocket == INVALID_SOCKET )
		{
			cout << "  accept client fail" << endl;
			continue;
		}

		// print client info
		cout << "  connect client success" << endl << endl
			 << "  === client info ===" << endl
			 << "  ip address : " << inet_ntoa(cliAddr.sin_addr) << endl << endl;
		
		
		//
		// insert in client socket vector
		//

		int emptyIndex = findEmptyClientSocketInfo();

		if ( emptyIndex == -1 )
		{
			
			mClientSocketInfoVector.push_back(pbClientSocketInfo());
			mClientSocketInfoVector.back().initClientSocketInfo(tempSocket, inet_ntoa(cliAddr.sin_addr));
		}
		else
		{
			mClientSocketInfoVector[emptyIndex].releaseClientSocketInfo();
			mClientSocketInfoVector[emptyIndex] = pbClientSocketInfo();
			mClientSocketInfoVector[emptyIndex].initClientSocketInfo(tempSocket, inet_ntoa(cliAddr.sin_addr));
		}
	}
}



void pbNetwork::realSendThread()
{
	while(1)
	{
		
		int clientNum = mClientSocketInfoVector.size();

		for ( int i = 0; i < clientNum; i++)
		{
			if ( i == mMyPlayerIndex )
				continue;
			if ( mClientSocketInfoVector[i].isLive() && mClientSocketInfoVector[i].isRecvThreadRunning())
			{
				WaitForSingleObject(mSendBufferMutex, INFINITE);
				if ( send(mClientSocketInfoVector[i].getSocket(), mSendBuffer.c_str(), mSendBuffer.size(), 0) == SOCKET_ERROR )
				{
					printf("disconnect in send\n");
					mClientSocketInfoVector[i].setDisconnect();
				}

				ReleaseMutex(mSendBufferMutex);
			}
		}
		Sleep(1);
	}
}
int pbNetwork::findEmptyClientSocketInfo()
{
	int clientSocketInfoNum = mClientSocketInfoVector.size();
	for ( int i = 0; i < clientSocketInfoNum; i++)
	{
		if ( !mClientSocketInfoVector[i].isLive() )
			return i;

	}
	return -1;
}
