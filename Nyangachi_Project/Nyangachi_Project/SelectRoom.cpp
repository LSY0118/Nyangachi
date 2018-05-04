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
	cout << ">> 1. 방 만들기 2. 방 선택\n";
	cin >> menuNum;
	switch (menuNum)
	{
	case 1:

		break;
	case 2:
		cout << ">> 방 번호 입력 : ";
		cin >> roomNum;
		m_pNetworkManager->sendPickRoom(roomNum);
		break;
	default:
		break;
	}
}

void CSelectRoom::updateRoom(ROOM_INFO room) {

}