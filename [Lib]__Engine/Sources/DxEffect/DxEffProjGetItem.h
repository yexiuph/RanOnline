#pragma once

#include <string>

#include "./DxVertexFVF.h"

class DxEffProjGetItem
{
protected:
	LPDIRECT3DSTATEBLOCK9 m_pSavedRS;
	LPDIRECT3DSTATEBLOCK9 m_pDrawRS;
	float m_fElapsedTime;

protected:
	D3DMATERIALQ		m_sMaterial;
	std::string			m_strTexName;
	LPDIRECT3DTEXTUREQ	m_pTexture;

protected:
	BOOL			m_bEnableON;
	BOOL			m_bPlayON;
	float			m_fAlpha;
	VERTEXCOLORRHW	m_sVertex[4];

protected:
	int		m_nCount00;	// 시퀀스 번호
	int		m_nCount01;	// 시퀀스 번호

public:
	void EnableEFF( BOOL bUse );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice();

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void SequnceMake( int nCount );
	void RenderEX( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjGetItem();
	~DxEffProjGetItem();
};