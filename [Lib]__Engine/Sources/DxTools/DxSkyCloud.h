#pragma once

#include "DxVertexFVF.h"

class DxSkyCloud
{
private:
	UINT VERTEXNUM;
	UINT FACESNUM;

private:
	BOOL				m_bEnable;
	float				m_fRotateADD;
	float				m_fRotateSUM;
	D3DXMATRIX			m_matRotate;
	D3DXMATRIX			m_matScale;
	std::string			m_strTexName;
	LPDIRECT3DTEXTUREQ	m_pTexture;

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase );

public:
	void InitData( float fRotate, float fRotateSUM, float fWidthRate, float fHeightRate, const char* pName );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();

public:
	DxSkyCloud();
	~DxSkyCloud();
};

class DxSkyCloudMan
{
private:
	static const DWORD CLOUD_NUM;

private:
	DxSkyCloud*	m_pSkyCloud;

private:
	LPDIRECT3DSTATEBLOCK9 m_pSavedSB;
	LPDIRECT3DSTATEBLOCK9 m_pDrawSB;

private:
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;	// 이건은 공유한다.

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	DxSkyCloudMan();
	~DxSkyCloudMan();
};