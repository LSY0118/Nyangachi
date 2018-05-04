#pragma once
#include "stdafx.h"

#define		WM_SOCKET		(WM_USER+1)
#define		MAX_PACKET_SIZE			256
#define		SERVER_PORT				9000
#define		RESERVE_ID_CNT			1000
#define		PAK_SYNC				0
#define		PAK_ID					1
#define		PAK_REG					2
#define		PAK_RMV					3
#define		PAK_PIKROOM				4
#define		MAX_CLIENT				100
#define		ROOM_MAX_PLAYER			6

#pragma pack(push, 1)

struct HEADER {
	UCHAR		ucSize;
	BYTE		byPacketID;
};

struct ROOM_INFO {
	BYTE		R_RoomNum;
	BYTE		R_RoomState;
	BYTE		R_PlayerNum;
};

struct SC_CONNECT {
	HEADER				header;
	USHORT				nPlayerID;
};

struct SC_LOGIN {
	BYTE		ID;
};

//struct CS_SYNC
//{
//	POINT	movedelta;
//};

struct SC_SYNC
{
	BYTE	ID;
	POINT	position;
};

struct SC_ID
{
	BYTE	ID;
};

struct CS_ID
{
	BYTE	ID;
};

struct CS_PICKROOM {
	HEADER	header;
	USHORT	playerID;
	BYTE	byRoom;
};

#pragma pack(pop)