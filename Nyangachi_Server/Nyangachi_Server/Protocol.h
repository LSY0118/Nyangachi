#pragma once
#include "stdafx.h"

#define		MAX_PACKET_SIZE			256
#define		SERVER_PORT				9000
#define		RESERVE_ID_CNT			1000
#define		PAK_SYNC				0
#define		PAK_ID					1
#define		PAK_REG					2
#define		PAK_RMV					3

#pragma pack(push, 1)

struct HEADER {
	UCHAR		ucSize;
	BYTE		byPacketID;
};

struct ROOM_INFO {
	BYTE		R_RoomNum;
	BYTE		R_RoomState		: 2;
	BYTE		R_PlayerNum		: 3;
	BYTE		R_Ready			: 1;
};

struct SC_LOGIN {
	BYTE		ID;
};

#pragma pack(pop)