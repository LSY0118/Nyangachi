#pragma once

class CCamera;

class CVertex
{
public:
	CVertex(){}
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }

	XMFLOAT3	m_xmf3Position;
};

class CPolygon
{
public:
	CPolygon(int nVertices);
	virtual ~CPolygon();

	int			m_nVertices;
	CVertex*	m_pVertices;

	void SetVertex(int nIndex, CVertex& vertex);
	void Draw(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera* pCamera);
};

class CMesh
{
public:
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int		m_nReferences;
	int		m_nPolygons;
	CPolygon**	m_ppPolygons;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);
	virtual void Render(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera *pCamera);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
	virtual ~CCubeMesh();

public:
	void Render(HDC hDCFrameBuffer, XMFLOAT4X4& xm4x4Transform, CCamera* pCamera);
};

class CWallMesh : public CMesh
{
public:
	CWallMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CWallMesh();
};

class CAirplaneMesh : public CMesh
{
public:
	CAirplaneMesh(float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~CAirplaneMesh();
};

