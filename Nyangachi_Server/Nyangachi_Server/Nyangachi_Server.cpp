#include "stdafx.h"
#include "NetworkManager.h"

CNetworkManager gNetwork;

int main()
{
	//��Ʈ��ũ�� �ʱ�ȭ �Ѵ�
	gNetwork.initNetwork();
	//������ �����Ѵ�
	gNetwork.startServer();
	//������ �����Ѵ�
	gNetwork.endServer();
}

