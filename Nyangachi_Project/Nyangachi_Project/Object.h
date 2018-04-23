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

class CShader;
class FBXContext;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;
	CShader* m_pShader = NULL;

public:
	BoundingOrientedBox		m_xmOOBB;
	BoundingOrientedBox		m_xmOOBBTransformed;

public:
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetOOBB(XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation) { m_xmOOBBTransformed = m_xmOOBB = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation); }

	void MoveStrafe(float fDistance = 1.f);
	void MoveUp(float fDistance = 1.f);
	void MoveForward(float fDistance = 1.f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.f, float fYaw = 10.f, float fRoll = 10.f);
	void ReleaseUploadBuffers();
};
