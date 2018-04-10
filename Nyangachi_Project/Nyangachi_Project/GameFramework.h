#pragma once

#include "Timer.h"
//#include "Common.h"

class CGameObject;
class CPlayer;
class CTestObj;

class CGameFramework
{
public:
	CGameFramework(void);
	~CGameFramework(void);

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void BuildObjects();
	void ReleaseObject();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void FrameAdvance();	
	void AnimateObjects();
	void ProcessInput();

private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	HDC			m_hDCFrameBuffer;
	HBITMAP		m_hBitmapFrameBuffer;
	bool		m_bActive;

	CGameTimer  m_GameTimer;
	_TCHAR		m_pszFrameRate[50];
	
	//int				m_nObjects;
	//list<CGameObject*>	m_pObjects;
	//FbxManager* m_pfbxMgr = NULL;
	//FbxScene*	m_pfbxScene = NULL;

	CPlayer*		m_pPlayer;

	/************************************test*************************************/
	CTestObj*		m_pObj;
	/*****************************************************************************/
	
public:
	POINT	m_ptOldCursorPos;
};

