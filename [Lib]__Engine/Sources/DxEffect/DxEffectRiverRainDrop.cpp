// Terrain0.cpp: implementation of the DxEffectRiver class.
//
//	UPDATE [03.02.20] : ���Ӱ� �� ȿ���� �ٲ�
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"
#include "./DxSurfaceTex.h"
#include "./DxEnvironment.h"
#include "./DxCubeMap.h"

#include "./DxViewPort.h"

#include "./SerialFile.h"
#include "./Collision.h"

#include "./DxEffectRiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DXRAINDROPMAN::VERTEXCOLOR::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

BOOL RAINDROP::FrameMove ( float fETime )
{
	float fRate = fTime/0.5f;			// 0~1 �� ���� ���´�.

	fSize = 1.f - fRate;				// ũ�Ⱑ  1~0 ����.

	fTime += fETime;
	if ( fTime > 0.5f )	return FALSE;	// Ÿ�� ��

	return TRUE;
}

DXRAINDROPMAN::DXRAINDROPMAN () :
	m_pRainDropTEX(NULL),
	m_fWidth(10.f)
{
	StringCchCopy( m_szRainDropTEX, MAX_PATH, "_Wa_water1.bmp" );
}

DXRAINDROPMAN::~DXRAINDROPMAN ()
{
}

HRESULT DXRAINDROPMAN::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szRainDropTEX, pd3dDevice, m_pRainDropTEX, 0, 0 );	// �׽�Ʈ

	return S_OK;
}

HRESULT DXRAINDROPMAN::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szRainDropTEX, m_pRainDropTEX );

	return S_OK;
}

void	DXRAINDROPMAN::SetMaxMin ( D3DXVECTOR3 vMax, D3DXVECTOR3 vMin )
{
	float fRangeX = vMax.x - vMin.x;
	float fRangeZ = vMax.z - vMin.z;

	m_fAREA = fRangeX*fRangeZ;		//����
}

HRESULT DXRAINDROPMAN::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT DXRAINDROPMAN::Render ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB )
{
	return S_OK;
}
