#pragma once
#include "stdafx.h"
#include "Protocol.h"
#include "Player.h"

class SockInfo {
	WSAOVERLAPPED				m_Overlapped;
	WSABUF						m_WSABuf;
	SOCKET						m_Sock;
	char						m_IOBuf[MAX_PACKET_SIZE];
	char						m_packetBuf[MAX_PACKET_SIZE];
	int							m_nstoredPacketSize;
	int							m_ncurPacketSize;
	UINT						m_nSockID;
	CPlayer						*m_player;

public:
	SockInfo();
	~SockInfo();

	WSAOVERLAPPED&	getOvelappedStruct() { return m_Overlapped; }
	WSABUF&			getWSABuf() { return m_WSABuf; }
	char*			getIOBuf() { return m_IOBuf; }
	char*			getPacketBuf() { return m_packetBuf; }
	int&			getStoredPacketSize() { return m_nstoredPacketSize; }
	int&			getCurPacketSize() { return m_ncurPacketSize; }
	UINT&			getSockID() { return m_nSockID; }
	SOCKET&			getSock() { return m_Sock; }
	CPlayer*		getPlayer() { return m_player; }

	void initSockInfo(const SOCKET &m_Sock, const UINT& ID);
	void setSockID(UINT id) { m_nSockID = id; }
	void setSock(const SOCKET& sock) { m_Sock = sock; }
	void setOvelappedStruct(const WSAOVERLAPPED& overlapped) { m_Overlapped = overlapped; }
	void setIOBuf(const char* iobuf, int size) { memcpy(m_IOBuf, iobuf, sizeof(size)); }
	void setPacketBuf(const char* packbuf, int size) { memcpy(m_packetBuf, packbuf, sizeof(size)); }
	void setStoredPacketSize(const int& size) { m_nstoredPacketSize = size; }
	void setCurrPacketSize(const int& size) { m_ncurPacketSize = size; }
};

class CNetworkManager
{
	vector<thread*>					m_vThreadList;
	concurrent_vector<SockInfo*>	m_vClientInfo;
	HANDLE							m_hIOCP;
	SOCKET							m_listenSock;

public:
	CNetworkManager();
	~CNetworkManager();

	void initNetwork();
	void startServer();
	void endServer();

	void transmitProcess(void *buf, int id);
	bool packetProcess(CHAR* buf, int id);

	//스레드 함수
	bool acceptThread();
	void workerThread();
	bool inRange(int a, int b);

	//패킷 처리 함수
	bool Login(void *buf, int id);
	bool Logout(void *buf, int id);

	//오류 출력 함수
	void err_quit(char *msg);
	void err_display(char *msg);
};

