#pragma once

class CVertex
{
protected:
	XMFLOAT3 m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4 m_xmf4Diffuse;

public:
	CDiffusedVertex() { m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f); m_xmf4Diffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.f); }
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; }
	~CDiffusedVertex() {}
};

class CMesh
{
public:
	CMesh();
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();

protected:
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	UINT m_nIndices = 0;
	UINT m_nStartIndex = 0;
	UINT m_nBaseVertex = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommadList);
};


class CAirPlaneMeshDiffused : public CMesh
{
public:
	CAirPlaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.f, float fHeight = 20.f, float fDepth = 4.f, XMFLOAT4 xmf4Color = XMFLOAT4(1.f, 1.f, 0.f, 0.f));
	virtual ~CAirPlaneMeshDiffused();
};