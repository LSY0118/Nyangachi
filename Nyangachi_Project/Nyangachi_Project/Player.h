#pragma once

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
	BYTE		m_byState;
protected:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	XMFLOAT3 m_xmf3Velocity;
	XMFLOAT3 m_xmf3Gravity;
	float m_fMaxVelocityXZ;
	float m_fMaxVelocityY;
	float m_fFriction;

	LPVOID m_pPlayerUpdatedContext;
	LPVOID m_pCameraUpdatedContext;

	CCamera* m_pCamera = NULL;

public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return m_xmf3Position; }
	XMFLOAT3 GetLookVector() { return m_xmf3Look; }
	XMFLOAT3 GetUpVector() { return m_xmf3Up; }
	XMFLOAT3 GetRightVector() { return m_xmf3Right; }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	void SetPosition(XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }

	XMFLOAT3& GetVelocity() { return m_xmf3Velocity; }
	float GetYaw() { return m_fYaw; }
	float GetPitch() { return m_fPitch; }
	float GetRoll() { return m_fRoll; }

	CCamera* GetCamera() { return m_pCamera; }
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.f, float fyOffset = 0.f, float fzOffset = 0.f);

	void Rotate(float x, float y, float z);
	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) {}
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) {}
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	CCamera* OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return NULL; }

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	// 네트워크
	//POINT& getPos() { return m_Position; }
	BYTE& getState() { return m_byState; }

	//void setPos(const POINT& pos) { m_Position = pos; }
	void setState(const BYTE &state) { m_byState = state; }
	//void Move(POINT& posDelta);
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CAirplanePlayer();

	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};
