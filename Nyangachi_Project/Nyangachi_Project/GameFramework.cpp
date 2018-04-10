#include "stdafx.h"
#include "GameFramework.h"
#include "Mesh.h"
#include "Object.h"

CGameFramework::CGameFramework()
{
	m_hInstance = NULL;
	m_hWnd = NULL;
	m_hDCFrameBuffer = NULL;
	m_hBitmapFrameBuffer = NULL;

	_tcscpy_s(m_pszFrameRate, _T("3DGP_1("));

	m_bActive = true;
	//m_nObjects = 0;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	srand(timeGetTime());

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_pPlayer = new CPlayer();
	m_pPlayer->m_pCamera->SetViewport(rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	BuildFrameBuffer();
	BuildObjects();

	return(true);
}

void CGameFramework::OnDestroy()
{
	ReleaseObject();

	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);

	if (m_hWnd) DestroyWindow(m_hWnd);
}

void CGameFramework::ReleaseObject()
{
	//if (m_pObjects) delete[] m_pObjects;
	//m_pObjects = NULL;
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, (rcClient.right - rcClient.left) + 1, (rcClient.bottom - rcClient.top) + 1);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::BuildObjects()
{
	//InitializeSdkObjects(m_pfbxMgr, m_pfbxScene);

	//if (LoadScene(m_pfbxMgr, m_pfbxScene, ""))
	//{
	//	FbxGeometryConverter lGeomConverter(m_pfbxMgr);
	//	lGeomConverter.Triangulate(m_pfbxScene, true);

	//	FbxAxisSystem SceneAxisSystem = m_pfbxScene->GetGlobalSettings().GetAxisSystem();
	//	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eDirectX);
	//	if (SceneAxisSystem != OurAxisSystem) OurAxisSystem.ConvertScene(m_pfbxScene);

	//	//m_pfbxScene->FillAnimStackNameArray(m_fbxAnimationStackNameArray);
	//	//SetCurrentAnimStack(0);

	//	//m_fbxFrameTime.SetTime(0, 0, 0, 1, 0, m_pfbxScene->GetGlobalSettings().GetTimeMode());
	//}

	CAirplaneMesh *pAirplaneMesh = new CAirplaneMesh(6.0f, 6.0f, 1.0f);
	m_pPlayer->SetPosition(0.0f, 0.0f, -30.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 5.0f, -15.0f));

	CCubeMesh *pObjectCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	m_pObj = new CTestObj;
	m_pObj[0].SetMesh(pObjectCubeMesh);
	m_pObj[0].SetColor(RGB(255, 0, 0));
	m_pObj[0].SetPosition(0.f, 0.0f, 0.f);
	m_pObj[0].SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_pObj[0].SetRotationSpeed(0.f);
	m_pObj[0].SetMovingDirection(XMFLOAT3(0.f, 0.0f, 0.0f));
	m_pObj[0].SetMovingSpeed(0.f);
	m_pObj[0].SetOOBB(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, m_pPlayer->m_pCamera->m_Viewport.m_nWidth, m_pPlayer->m_pCamera->m_Viewport.m_nHeight, m_hDCFrameBuffer, m_pPlayer->m_pCamera->m_Viewport.m_xStart, m_pPlayer->m_pCamera->m_Viewport.m_yStart, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;
	if (GetCapture() == m_hWnd)
	{
		SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
			else
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.025f);
	}
	m_pPlayer->Update(0.00516f);
}

void CGameFramework::AnimateObjects()
{

}

void CGameFramework::FrameAdvance()
{
	if (!m_bActive) return;

	m_GameTimer.Tick(0.f);

	ProcessInput();
	AnimateObjects();

	ClearFrameBuffer(RGB(255, 255, 255));

	m_pPlayer->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);
	m_pObj->Render(m_hDCFrameBuffer, m_pPlayer->m_pCamera);

	PresentFrameBuffer();
		
	m_GameTimer.GetFrameRate(m_pszFrameRate + 7, 42);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}
