#pragma once

#include <WinSock2.h>
#include <vector>
#include <string>

using std::string;
using std::to_string;
using std::vector;


enum NetworkType {HOST, CLIENT, SINGLE};

class pbClientSocketInfo
{
public:
	void initClientSocketInfo(SOCKET socket, const char* ipAddress);
	void runRecvThread();
	void releaseClientSocketInfo();

	bool isRecvThreadRunning();
	bool isLive();
	bool isDisconnectInfoSend();
	void setDisconnect();
	void setDisconnectInfoSend(bool flag);
	string getAndClearRecvBuffer();

	void addRecvBuffer(string str);
	SOCKET getSocket();

public:
	pbClientSocketInfo();
	

private:
	static DWORD WINAPI recvThread(LPVOID lpParameter);
	void realRecvThread();
	DWORD lockRecvBufferMutex(DWORD miliseconds = INFINITE);
	void unlockRecvBufferMutex();

private:
	
	SOCKET mSocket;
	string mIpAddress;
	HANDLE mRecvThreadHandle;

	string mRecvBuffer;
	HANDLE mRecvBufferMutexHandle;
	bool mLive;
	bool mDisconnectInfoSendFlag;	// default : false, send : true,
};


class pbNetwork
{
public:
	int initNetwork();
	void setNetworkType();
	int getNewPlayerId();
	int getDisconnectPlayerId();

	NetworkType getNetworkType();

	
	
	vector<string> getRecvBufferVector();

	void initHost();
	int createThread();
	
	
	
	
	int getMyPlayerId();
	void connectToHost();

	void setSendBuffer(string str);
	void sendInitInfo(int newPlayerId);

public:
	pbNetwork();
	~pbNetwork();

private:

	void recvInitInfo();

	int releaseNetwork();
	int findEmptyClientSocketInfo();

	// thread
	static DWORD WINAPI acceptThread(LPVOID lpParameter);
	static DWORD WINAPI sendThread(LPVOID lpParameter);
	void   realAcceptThread();
	void   realSendThread();
private:
	SOCKET  hostSocket;
	vector<pbClientSocketInfo> mClientSocketInfoVector;	// (include host, 0 is host)
	
	int mMyPlayerIndex;
	
	NetworkType mNetworkType;

	string mSendBuffer;
	
	
	HANDLE mAcceptThreadHandle;
	HANDLE mRecvThreadHandle;
	HANDLE mSendThreadHandle;

	// HANDLE mMutex;
	HANDLE mSendBufferMutex;

	static const unsigned short PORT_NUMBER;
};