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
		assert(m_hIOCP != nullptr && "m_hIOCP ���� ����");
	}
	cout << "��Ʈ��ũ ��ü �ʱ�ȭ" << endl;
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
	//listenSock����
	m_listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSock == INVALID_SOCKET)
	{
		assert(m_listenSock != INVALID_SOCKET && "listen sock ���� ����");
		return false;
	}
	cout << "listen sock ���� ���� ����" << endl;

	//�ּ� bind()
	sockaddr_in	listenSockAddr;
	ZeroMemory(&listenSockAddr, sizeof(listenSockAddr));
	listenSockAddr.sin_family = AF_INET;
	listenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	listenSockAddr.sin_port = htons(SERVER_PORT);

	int retval = ::bind(m_listenSock, (sockaddr*)&listenSockAddr, sizeof(listenSockAddr));
	if (retval == SOCKET_ERROR)
	{
		assert(retval != SOCKET_ERROR && "listen sock addr bind() ����");
		return false;
	}
	cout << "listen sock bind ����" << endl;

	//listen()
	retval = listen(m_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		assert(retval != SOCKET_ERROR && "listen() ����");
		return false;
	}
	cout << "listen sock listen ���� ��ȯ ����" << endl;


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

		cout << "Ŭ���̾�Ʈ ���� �Ϸ� " << tmpSock->getSockID() << endl;

		//�ش� ������ Recv���·� �ٲ���
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
		//cout << "recv ���·� ��ȯ" << endl;
	}
	return true;
}

void  CNetworkManager::workerThread()
{
	cout << "worker Thread ����" << endl;

	DWORD IOsize;
	ULONG key;
	SockInfo *sockInfo;

	while (1)
	{
		GetQueuedCompletionStatus(m_hIOCP, &IOsize, &key, reinterpret_cast<LPOVERLAPPED *>(&sockInfo), INFINITE);
		//cout << "io �۾� �Ϸ�" << endl;

		// ���� ���� ó��
		if (IOsize == 0)
		{
			auto sockdata = m_vpClientInfo[key];
			m_vpClientInfo[key] = nullptr;
			delete sockdata;
			cout << "Ŭ���̾�Ʈ ��������" << endl;
			Logout(nullptr, key);
			continue;
		}
		if (sockInfo->getOperationType() == OP_TYPE::OP_RECV)
		{
			// ��Ŷ���� �� ����
			unsigned restData = IOsize;
			char * recvbuf = sockInfo->getIOBuf();

			while (restData > 0)
			{
				//���� ó���ϴ� ��Ŷ�� ���� ���
				if (sockInfo->getCurPacketSize() == 0)
				{
					//iobuf�� ù��° ���� ��Ŷ ������� �����´�.
					UCHAR *size = reinterpret_cast<UCHAR*>(recvbuf);
					//cout << "ó���ؾ� �� ��Ŷ ������" << (int)*size << endl;
					sockInfo->setCurrPacketSize(recvbuf[0]);
				}

				// ���� ��Ŷ������ - �����ִ� ��Ŷ������ => ��Ŷ�� ����� ���� �ʿ��� ������
				UINT buildData = sockInfo->getCurPacketSize() - sockInfo->getStoredPacketSize();

				// io�� ���� �������� ũ�Ⱑ ��Ŷ�� ����� ���� �ʿ��� ������� ũ�ų� ���� ���
				// ��Ŷ�� �����Ѵ�.
				if (buildData <= restData)
				{
					// ��Ŷ���ۿ� ��Ŷ ����� ä�� �� ��ŭ 
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
					// ó���� ��ŭ�� ����� �ƴ� ��� ��Ŷ���ۿ� ���� �� ����
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
	//cout << "��Ŷ ó��"<< (int)buf[1] << endl;
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