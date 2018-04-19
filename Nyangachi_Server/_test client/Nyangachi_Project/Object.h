#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD			0x01
#define DIR_BACKWARD		0x02
#define DIR_LEFT			0x04
#define DIR_RIGHT			0x08
#define DIR_UP				0x10
#define DIR_DOWN			0x20

#define DegreeToRadian(x)	((x)*3.1415926535/180.0)

class CGameObject
{
public:
	CGameObject(CMesh *pMesh);
	CGameObject();
	virtual ~CGameObject();

public:
	CMesh					*m_pMesh;
	XMFLOAT4X4				m_xmf4x4World;

	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;
	CGameObject				*m_pCollider;

	DWORD					m_dwColor;

	XMFLOAT3				m_xmf3MovingDirection;
	float					m_fMovingSpeed;
	float					m_fMovingRange;

	XMFLOAT3				m_xmf3RotationAxis;
	float					m_fRotationSpeed;
	float					m_bLive;

public:
	void SetMesh(CMesh *pMesh) { m_pMesh = pMesh; if (pMesh) pMesh->AddRef(); }
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3& xmf3Position);

	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }

	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }
	void SetMovingSpeed(float fSpeed) { m_fMovingSpeed = fSpeed; }
	void SetMovingRange(float fRange) { m_fMovingRange = fRange; }

	void SetRotationAxis(XMFLOAT3& xmf3RotationAxis) { m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis); }
	void SetRotationSpeed(float fSpeed) { m_fRotationSpeed = fSpeed; }

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3& xmf3Axis, float fAngle);

	DWORD GetColor() { return m_dwColor; }

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void Animate();
	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);
};

class CPlayer : public CGameObject
{
	BYTE		m_byState;
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	CCamera						*m_pCamera;

	void SetPosition(float x, float y, float z);
	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);
	void Update(float fTimeElapsed = 0.016f);

	XMFLOAT3 GetMovingDirection();

	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera);

	// 네트워크
	//POINT& getPos() { return m_Position; }
	BYTE& getState() { return m_byState; }

	//void setPos(const POINT& pos) { m_Position = pos; }
	void setState(const BYTE &state) { m_byState = state; }
	//void Move(POINT& posDelta);
};

class CTestObj : public CGameObject
{
public:
	CTestObj() {};
	virtual ~CTestObj() {};

	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	XMFLOAT3					m_xmf3CameraOffset;
	XMFLOAT3					m_xmf3Velocity;
	float						m_fFriction;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	CCamera						*m_pCamera;

	virtual void Render(HDC hDCFrameBuffer, CCamera *pCamera)
	{
		XMFLOAT4X4 xm4x4Transform = Matrix4x4::Multiply(m_xmf4x4World, pCamera->m_xmf4x4ViewProject);
		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);

		if (m_pMesh) m_pMesh->Render(hDCFrameBuffer, xm4x4Transform, pCamera);

		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
};