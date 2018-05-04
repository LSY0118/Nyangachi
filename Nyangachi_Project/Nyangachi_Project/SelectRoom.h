#pragma once
#include "Scene.h"
class CSelectRoom : public CScene
{
	ROOM_INFO	m_rooms[10];
	BYTE		m_myRoomNum;
public:
	CSelectRoom();
	~CSelectRoom();

	//virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommnadList);
	//virtual void ReleaseObjects();

	//virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//
	//virtual void AnimateObjects(float fTimeElapsed);
	//virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera);
	virtual void updateRoom(ROOM_INFO room);
	void consoleMsg();
};

