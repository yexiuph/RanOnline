#pragma once

#include "DxVertexFVF.h"

class DxSkyBase
{
private:
	static const UINT VERTEXNUM;
	static const UINT FACESNUM;

private:
	float				m_fRotateADD;
	float				m_fRotateSUM;
	float				m_fUV_W;
	BOOL				m_bNoInverseUV;

	D3DXMATRIX			m_matRotate;
	D3DXMATRIX			m_matScale;
	std::string			m_strTexName;
	LPDIRECT3DTEXTUREQ	m_pTexture;

private:
	LPDIRECT3DVERTEXBUFFERQ	m_pVB;
	static LPDIRECT3DINDEXBUFFERQ	m_pIB;

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bReverse, const D3DXMATRIX& matBase );

public:
	void InitData( float fRotate, float fWidthRate, float fHeightRate, float fUV_W, BOOL bNoInverse, const char* pName );
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	DxSkyBase();
	~DxSkyBase();
};

class DxSkyBaseMan
{
private:
	LPDIRECT3DSTATEBLOCK9 m_pSavedBaseSkySB;
	LPDIRECT3DSTATEBLOCK9 m_pDrawBaseSkySB;

private:
	static const DWORD SKY_NUM;

private:
	DxSkyBase*	m_pSkyBase;

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bReflect, const D3DXMATRIX& matBase );
	void RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice();
	void OnDestroyDevice();

public:
	DxSkyBaseMan();
	~DxSkyBaseMan();
};
