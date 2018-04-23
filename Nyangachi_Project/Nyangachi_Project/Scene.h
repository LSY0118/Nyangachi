#pragma once

#include "Timer.h"
#include "Shader.h"

class CCamera;
class CPlayer;

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommnadList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeyBuffer);
	void AnimateObjects(float fTimeElapsed);
	void ContainObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CPlayer* pPlayer);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
};

