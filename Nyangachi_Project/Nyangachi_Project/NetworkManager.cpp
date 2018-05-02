#include "stdafx.h"
#include "NetworkManager.h"


CNetworkManager::CNetworkManager()
{
	m_ClientSock = NULL;
	ZeroMemory(m_Sendbuf, MAX_PACKET_SIZE);
	ZeroMemory(m_Recvbuf, MAX_PACKET_SIZE);
	ZeroMemory(m_Packbuf, MAX_PACKET_SIZE);
	m_ClientID = -1;
	m_pTransmitthread = nullptr;

	m_wsaSendbuf.buf = m_Sendbuf;
	m_wsaSendbuf.len = MAX_PACKET_SIZE;

	m_wsaRecvbuf.buf = m_Recvbuf;
	m_wsaRecvbuf.len = MAX_PACKET_SIZE;
}


CNetworkManager::~CNetworkManager()
{
}

void CNetworkManager::err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[오류] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void CNetworkManager::err_quit(TCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void CNetworkManager::err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


void CNetworkManager::initNetwork()
{
	m_pakSiz = 0;
	m_savesiz = 0;
	m_vector.resize(MAX_CLIENT);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) assert(0);

	m_ClientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, 0);
	if (m_ClientSock == INVALID_SOCKET) err_quit(L"socket()");

	CHAR opt_val = FALSE;
	setsockopt(m_ClientSock, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));

	m_wsaSendbuf.len = 256;
	m_wsaSendbuf.buf = m_Sendbuf;

	m_bisConnect = false;

	cout << "소켓 초기화 완료" << endl;
}

void CNetworkManager::connectServer(CHAR *ipstr, HWND hWnd)
{
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipstr);
	serveraddr.sin_port = htons(SERVER_PORT);

	int retval = connect(m_ClientSock, reinterpret_cast<SOCKADDR *>(&serveraddr), sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(L"connect()");

	retval = WSAAsyncSelect(m_ClientSock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
	if (retval == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	cout << "서버와 연결 완료" << endl;

	DWORD sendbyte = 256, flag = 0;

	HEADER *phead = reinterpret_cast<HEADER*>(m_Sendbuf);
	phead->byPacketID = PAK_ID;
	phead->ucSize = sizeof(HEADER);

	m_wsaSendbuf.len = phead->ucSize;

	//int retval = send(m_ClientSock, m_Sendbuf, 256, 0);
	retval = WSASend(m_ClientSock, &m_wsaSendbuf, 1, &sendbyte, 0, NULL, NULL);
	if (retval != 0)
	{
		err_display("WSARecv():");
		assert(0);
	}
	cout << " 로그인 패킷 전송 완료" << sendbyte << endl;

}

void CNetworkManager::endNetwork()
{
	closesocket(m_ClientSock);
	WSACleanup();
}

void CNetworkManager::packetProcess(CHAR* buf)
{
	switch (buf[1])
	{
		//case PAK_SYNC:
		//{
		//	STOC_SYNC *tmp = reinterpret_cast<STOC_SYNC*>(buf + sizeof(HEADER));
		//	m_vector[tmp->ID].setPos(tmp->position);
		//	m_vector[tmp->ID].setState(1);
		//}
		//break;
	case PAK_ID:
	{
		SC_ID *tmp = reinterpret_cast<SC_ID*>(buf + sizeof(HEADER));

		if (m_bisConnect == false)
		{
			m_ClientID = tmp->ID;
			m_bisConnect = true;
		}
		m_vector[tmp->ID].setState(1);
	}
	break;
	case PAK_REG:
	{
		SC_SYNC *tmp = reinterpret_cast<SC_SYNC*>(buf + sizeof(HEADER));
		//m_vector[tmp->ID].setPos(tmp->position);
		m_vector[tmp->ID].setState(1);
	}
	break;
	case PAK_RMV:
	{
		SC_ID *tmp = reinterpret_cast<SC_ID*>(buf + sizeof(HEADER));
		m_vector[tmp->ID].setState(0);
	}
	break;
	}
}

void CNetworkManager::transmitProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD recvbyte = 256, flag = 0;

	if (WSAGETSELECTERROR(lParam))
	{
		err_display(WSAGETSELECTERROR(lParam));
		endNetwork();
		PostQuitMessage(0);
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		int retval = WSARecv(m_ClientSock, &m_wsaRecvbuf, 1, &recvbyte, &flag, NULL, NULL);
		if (retval != 0)
		{
			if (GetLastError() == WSAEWOULDBLOCK) break;
			else
			{
				err_quit(L"fatal error!");
				break;
			}
		}
		if (recvbyte == 0)
		{
			closesocket(wParam);
			break;
		}

		BYTE *ptr = reinterpret_cast<BYTE *>(m_Recvbuf);

		while (recvbyte != 0)
		{
			if (0 == m_pakSiz)
				m_pakSiz = ptr[0];

			if (recvbyte + m_savesiz >= m_pakSiz)
			{
				memcpy(m_Packbuf + m_savesiz, ptr, m_pakSiz - m_savesiz);

				//패킷처리
				packetProcess(m_Packbuf);

				ptr += m_pakSiz - m_savesiz;
				recvbyte -= m_pakSiz - m_savesiz;
				m_pakSiz = 0;
				m_savesiz = 0;
			}
			else {
				memcpy(m_Packbuf + m_savesiz, ptr, recvbyte);
				m_savesiz += recvbyte;
				recvbyte = 0;
			}
		}
	}
	break;
	case FD_WRITE:
		PostMessage(hWnd, WM_SOCKET, wParam, FD_READ);
		break;
	case FD_CLOSE:
		endNetwork();
		PostQuitMessage(0);
		break;
	}
}

// 이동 패킷
//void CNetworkManager::sendMovePacket(POINT &deltaPos)
//{
//	DWORD sendbyte = 256, flag = 0;
//
//	HEADER *phead = reinterpret_cast<HEADER*>(m_Sendbuf);
//	phead->byPacketID = PAK_SYNC;
//	phead->ucSize = sizeof(CTOS_SYNC) + sizeof(HEADER);
//
//	CTOS_SYNC *data = reinterpret_cast<CTOS_SYNC*>(m_Sendbuf + sizeof(HEADER));
//	data->movedelta = deltaPos;
//
//	m_wsaSendbuf.len = phead->ucSize;
//
//	//int retval = send(m_ClientSock, m_Sendbuf, 256, 0);
//	int retval = WSASend(m_ClientSock, &m_wsaSendbuf, 1, &sendbyte, 0, NULL, NULL);
//	if (retval != 0)
//	{
//		err_display("WSARecv():");
//		assert(0);
//	}
//}