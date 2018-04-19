#include "NetworkManager.h"

SockInfo::SockInfo() {
	ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
	ZeroMemory(&m_WSABuf, sizeof(m_WSABuf));
	ZeroMemory(m_IOBuf, sizeof(m_IOBuf));
	ZeroMemory(m_packetBuf, sizeof(m_packetBuf));

	m_operationType = OP_TYPE::OP_RECV;

	m_ncurPacketSize = 0;
	m_nstoredPacketSize = 0;
	m_Sock = NULL;
	m_player = nullptr;
}

SockInfo::~SockInfo() {
	if (m_player) delete m_player;
}

void SockInfo::initSockInfo(const SOCKET &Sock, const UINT &ID)
{
	m_operationType = OP_TYPE::OP_RECV;
	m_WSABuf.buf = m_IOBuf;
	m_WSABuf.len = MAX_PACKET_SIZE;

	m_Sock = Sock;
	m_nSockID = ID;

	m_player = new CPlayer();
}

CNetworkManager::CNetworkManager()
{
	m_listenSock = NULL;
	m_hIOCP = NULL;
	m_nID = 0;
}


CNetworkManager::~CNetworkManager()
{
}

void CNetworkManager::err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	TCHAR tmp[100];

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
	//_wsetlocale(LC_ALL, L"korean");

	for (int i = 0; i < RESERVE_ID_CNT; ++i)
	{
		m_vpClientInfo.push_back(nullptr);
	}

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) assert(0);

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!m_hIOCP)
	{
		assert(m_hIOCP != nullptr && "m_hIOCP 생성 오류");
	}
	cout << "네트워크 객체 초기화" << endl;
}

void CNetworkManager::startServer()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	for (DWORD i = 0; i < si.dwNumberOfProcessors * 2; ++i)
	{
		m_vpThreadList.push_back(new thread{ mem_fun(&CNetworkManager::workerThread), this });
	}
	m_vpThreadList.push_back(new thread{ mem_fun(&CNetworkManager::acceptThread), this });
}

// bind, listen, accept, recv
bool CNetworkManager::acceptThread()
{
	//listenSock생성
	m_listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSock == INVALID_SOCKET)
	{
		assert(m_listenSock != INVALID_SOCKET && "listen sock 생성 오류");
		return false;
	}
	cout << "listen sock 소켓 생성 성공" << endl;

	//주소 bind()
	sockaddr_in	listenSockAddr;
	ZeroMemory(&listenSockAddr, sizeof(listenSockAddr));
	listenSockAddr.sin_family = AF_INET;
	listenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	listenSockAddr.sin_port = htons(SERVER_PORT);

	int retval = ::bind(m_listenSock, (sockaddr*)&listenSockAddr, sizeof(listenSockAddr));
	if (retval == SOCKET_ERROR)
	{
		assert(retval != SOCKET_ERROR && "listen sock addr bind() 오류");
		return false;
	}
	cout << "listen sock bind 성공" << endl;

	//listen()
	retval = listen(m_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		assert(retval != SOCKET_ERROR && "listen() 오류");
		return false;
	}
	cout << "listen sock listen 상태 전환 성공" << endl;


	SOCKET			clientSock;
	SOCKADDR_IN		clientAddr;
	DWORD			flags = 0;
	int				addrLen;

	while (1)
	{
		addrLen = sizeof(clientAddr);
		clientSock = WSAAccept(m_listenSock, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen, NULL, NULL);
		if (clientSock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		int MyID = getID();
		SockInfo *tmpSock = new SockInfo();
		tmpSock->initSockInfo(clientSock, MyID);
		m_vpClientInfo[tmpSock->getSockID()] = tmpSock;

		CreateIoCompletionPort((HANDLE)clientSock, m_hIOCP, tmpSock->getSockID(), 0);

		cout << "클라이언트 접속 완료 " << tmpSock->getSockID() << endl;

		//해당 소켓을 Recv상태로 바꿔줌
		retval = WSARecv(clientSock, &tmpSock->getWSABuf(), 1, NULL, &flags, reinterpret_cast<LPOVERLAPPED>(tmpSock), NULL);
		if (0 != retval)
		{
			int err_code = WSAGetLastError();
			if (WSA_IO_PENDING != err_code)
			{
				cout << err_code << endl;
				assert(WSA_IO_PENDING != err_code && "[CNetworkManager::acceptThread()] WSARecv");
				//exit(-1);

			}
		}
		//cout << "recv 상태로 전환" << endl;
	}
	return true;
}

void  CNetworkManager::workerThread()
{
	cout << "worker Thread 생성" << endl;

	DWORD IOsize;
	ULONG key;
	SockInfo *sockInfo;

	while (1)
	{
		GetQueuedCompletionStatus(m_hIOCP, &IOsize, &key, reinterpret_cast<LPOVERLAPPED *>(&sockInfo), INFINITE);
		//cout << "io 작업 완료" << endl;

		// 접속 종료 처리
		if (IOsize == 0)
		{
			auto sockdata = m_vpClientInfo[key];
			m_vpClientInfo[key] = nullptr;
			delete sockdata;
			cout << "클라이언트 접속종료" << endl;
			Logout(nullptr, key);
			continue;
		}
		if (sockInfo->getOperationType() == OP_TYPE::OP_RECV)
		{
			// 패킷조립 및 실행
			unsigned restData = IOsize;
			char * recvbuf = sockInfo->getIOBuf();

			while (restData > 0)
			{
				//현재 처리하는 패킷이 없을 경우
				if (sockInfo->getCurPacketSize() == 0)
				{
					//iobuf의 첫번째 값을 패킷 사이즈로 가져온다.
					UCHAR *size = reinterpret_cast<UCHAR*>(recvbuf);
					//cout << "처리해야 할 패킷 사이즈" << (int)*size << endl;
					sockInfo->setCurrPacketSize(recvbuf[0]);
				}

				// 현재 패킷사이즈 - 남아있는 패킷사이즈 => 패킷을 만들기 위해 필요한 사이즈
				UINT buildData = sockInfo->getCurPacketSize() - sockInfo->getStoredPacketSize();

				// io로 받은 데이터의 크기가 패킷을 만들기 위해 필요한 사이즈보다 크거나 같은 경우
				// 패킷을 조립한다.
				if (buildData <= restData)
				{
					// 패킷버퍼에 패킷 사이즈를 채워 줄 만큼 
					memcpy(sockInfo->getPacketBuf() + sockInfo->getStoredPacketSize(), recvbuf, buildData);

					if (!packetProcess(sockInfo->getPacketBuf(), key))
					{
						auto sockdata = m_vpClientInfo[key];
						m_vpClientInfo[key] = nullptr;
						delete sockdata;
						Logout(nullptr, key);
						continue;
					}
					sockInfo->setCurrPacketSize(0);
					sockInfo->setStoredPacketSize(0);

					recvbuf += buildData;
					restData -= buildData;
				}
				else
				{
					// 처리할 만큼의 사이즈가 아닌 경우 패킷버퍼에 저장 해 놓음
					memcpy(sockInfo->getPacketBuf() + sockInfo->getStoredPacketSize(), recvbuf, restData);

					sockInfo->setStoredPacketSize(sockInfo->getStoredPacketSize() + restData);

					recvbuf += restData;
					restData = 0;
				}
			}
			// Recv
			unsigned long recv_flag = 0;
			int retval = WSARecv(sockInfo->getSock(), &sockInfo->getWSABuf(), 1, NULL, &recv_flag, reinterpret_cast<LPOVERLAPPED>(sockInfo), NULL);
			if (SOCKET_ERROR == retval)
			{
				int err_code = WSAGetLastError();
				if (WSA_IO_PENDING != err_code)
				{
					assert(WSA_IO_PENDING != err_code && "[CNetworkManager::workerThread()] WSARecv");
					//Logout(key, nullptr);
				}
			}
		}
		else if (sockInfo->getOperationType() == OP_TYPE::OP_SEND)
		{
			delete sockInfo;
		}
		else
		{
			exit(-1);
		}
	}
}

void CNetworkManager::endServer()
{
	for (auto &data : m_vpThreadList)
	{
		data->join();
	}
	WSACleanup();
}

bool CNetworkManager::packetProcess(CHAR* buf, int id)
{
	//cout << "패킷 처리"<< (int)buf[1] << endl;
	bool issuccess = true;

	switch (buf[1])
	{
	case PAK_SYNC:
		issuccess = syncData(buf, id);
		break;
	case PAK_ID:
		issuccess = Login(buf, id);
		break;
	}
	return issuccess;
}