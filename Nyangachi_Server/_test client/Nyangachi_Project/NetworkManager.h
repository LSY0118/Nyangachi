#pragma once

#include "..\..\Nyangachi_Server\Protocol.h"
#include "stdafx.h"
#include "Object.h"

class CNetworkManager
{
public:
	SOCKET				m_ClientSock;
	CHAR				m_Sendbuf[MAX_PACKET_SIZE];
	CHAR				m_Recvbuf[MAX_PACKET_SIZE];
	CHAR				m_Packbuf[MAX_PACKET_SIZE];
	thread				*m_pTransmitthread;
	WSABUF				m_wsaSendbuf;
	WSABUF				m_wsaRecvbuf;
	vector<CPlayer>		m_vector;
	BYTE				m_ClientID;
	UINT				m_pakSiz, m_savesiz;
	bool				m_bisConnect;

	void initNetwork();
	void connectServer(CHAR *iostr, HWND hWnd);
	void endNetwork();

	void transmitProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void packetProcess(CHAR* buf);
	void sendMovePacket(POINT &deltaPos);

	vector<CPlayer> getPlayerList() { return m_vector; }

	void err_display(int errcode);
	void err_quit(TCHAR *msg);
	void err_display(char *msg);

	CNetworkManager();
	~CNetworkManager();
};

