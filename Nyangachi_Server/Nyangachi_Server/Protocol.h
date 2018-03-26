#pragma once
#include "stdafx.h"

#pragma pack(push, 1)
struct ROOM_INFO {
	BYTE		R_RoomNum;
	BYTE		R_RoomState		: 2;
	BYTE		R_PlayerNum		: 3;
	BYTE		R_Ready			: 1;
};
#pragma pack(pop)