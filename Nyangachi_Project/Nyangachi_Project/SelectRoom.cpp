#include "stdafx.h"
#include "SelectRoom.h"


CSelectRoom::CSelectRoom()
{
}


CSelectRoom::~CSelectRoom()
{
}

void CSelectRoom::consoleMsg() {
	BYTE menuNum, roomNum;
	cout << ">> 1. �� ����� 2. �� ����\n";
	cin >> menuNum;
	switch (menuNum)
	{
	case 1:

		break;
	case 2:
		cout << ">> �� ��ȣ �Է� : ";
		cin >> roomNum;
		m_pNetworkManager->sendPickRoom(roomNum);
		break;
	default:
		break;
	}
}

void CSelectRoom::updateRoom(ROOM_INFO room) {

}