// DxEffectShadow.cpp: implementation of the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxShadowMap.h"

#include "./EditMeshs.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxFrameMesh.h"
#include "./DxSetLandMan.h"

#include "./DxBackUpRendTarget.h"
#include "./DxEffectShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT DxEffectShadow::MakeStaticShadow ( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFile, DxSetLandMan *pSetLandMan, DWORD dwTexSize )
{
	GASSERT(pSetLandMan);

	if ( !m_pThisFrame )	return S_OK;

	HRESULT	hr;

	//	Note : �� �ؽ��� �ε�
	//
	{
		TextureManager::LoadTexture ( m_szFilter, pd3dDevice, m_pFilterTex, 0, 0 );

		if ( !m_pFilterTex )
		{
			char szMsg[256] = "";
			StringCchPrintf( szMsg, 256, "�̹��� ���� '%s' �б⿡ ����", m_szFilter );
			MessageBox ( NULL, szMsg, "ERROR", MB_OK );
		}
	}

	D3DFORMAT	colorFormat =  D3DFMT_R5G6B5;

	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);

	//	Note : �ݻ� �ؽ��� �����
	//
	hr = pd3dDevice->CreateTexture ( dwTexSize, dwTexSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, D3DPOOL_DEFAULT, &m_pATexture, NULL );
	if( FAILED(hr) )	return E_FAIL;

	if(FAILED(m_pATexture->GetSurfaceLevel(0, &m_pASurface)))
		return E_FAIL;

	//	Note : ���� �ؽ��� �����
	//
	hr = pd3dDevice->CreateTexture ( dwTexSize, dwTexSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
		D3DPOOL_DEFAULT, &m_pBTexture, NULL );
	if( FAILED(hr) )	return E_FAIL;

	if(FAILED(m_pBTexture->GetSurfaceLevel(0, &m_pBSurface)))
		return E_FAIL;

	//	Note : Static Shadow
	//
	hr = pd3dDevice->CreateTexture ( dwTexSize, dwTexSize, 1, D3DUSAGE_RENDERTARGET, colorFormat, 
		D3DPOOL_DEFAULT, &m_pSShadowTexture, NULL );
	if( FAILED(hr) )	return E_FAIL;

	if(FAILED(m_pSShadowTexture->GetSurfaceLevel(0, &m_pSShadowSurface)))
		return E_FAIL;

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	//	Note : 3���� Surface�� �ʱ��
	//
	pd3dDevice->SetRenderTarget ( 0, m_pSShadowSurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	pd3dDevice->SetRenderTarget ( 0, m_pASurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	pd3dDevice->SetRenderTarget ( 0, m_pBSurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);


	//	Note : m_fDistance, m_vViewMax, m_vViewMin, UV ��ǥ�� ���� �Ѵ�.
	//
	ReSetTexUV ( pd3dDevice, pSetLandMan->GetFrameMesh(), dwTexSize );

	//	Note : �߿� �κ� ( ����, ��ŷ, ���籤��, ����Ʈ, �ε巴�� )
	//
	if ( m_dwFlag & USEDIRECTSHADOW )	ProjectionObjD ( pd3dDevice, m_pThisFrame, FALSE, pSetLandMan );
	//if ( m_dwFlag & USEPOINTSHADOW )	ProjectionObjP ( pd3dDevice, m_pThisFrame, pSetLandMan );
	ImageBlur ( pd3dDevice );									

	ResetLocalMeshs ( pd3dDevice, pSetLandMan->GetFrameMesh()->GetFrameRoot(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );

	sBackupTarget.RestoreTarget ( pd3dDevice );

	//	Note : �̺κп��� ���� �ؽ��ĸ� �����ؾ� �Ѵ�.
	//
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, m_szPath );
	StringCchCat( szPathName, MAX_PATH, szFile );

	D3DXSaveTextureToFile( szFile, D3DXIFF_DDS, m_pSShadowTexture, NULL );

	TextureManager::ReScanTexture ();

	//	Note : �ؽ��� ����
	TextureManager::ReleaseTexture( m_szFilter, m_pFilterTex );

	//	Note : ���� �ش�.
	//
	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);

	return S_OK;
}

VOID	DxEffectShadow::ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh )
{
	DWORD	dwTexSize = 512;

	if ( m_pSSDayTex )
	{
		D3DSURFACE_DESC pDesc;
		m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
		dwTexSize = pDesc.Width;
	}

	//	Note : ī�޶�� ������ �Ÿ��� ���� �˸µ��� �����
	//
	m_fDistance = 500.f;
	while ( !CheckViewPort ( pd3dDevice, pFrameMesh->GetFrameRoot(), m_fDistance, dwTexSize ) )
	{
		m_fDistance += 100.f;
	}

	//	Note : ���� ī�޶� CV�� ������ �� ���ϱ�.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = (m_vMax+m_vMin) * 0.5f;

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 100000.f, (float)dwTexSize, (float)dwTexSize, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 100000.f;
	float	fWidth		= (float)dwTexSize;
	float	fHeight		= (float)dwTexSize;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i�� ���� 4�̻��϶�
		//				�հ��� ���� ��ǥ�̴�.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fWidth/fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : �� �����
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMin.z );
	vPosition[1] = D3DXVECTOR3 ( m_vMin.x, vCenter.y, m_vMax.z );
	vPosition[2] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMax.z );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : �� �����
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	vViewMax = vViewMin = vPosition[0];

	vViewMax.x = ( vViewMax.x > vPosition[0].x ) ? vViewMax.x : vPosition[0].x;
	vViewMax.x = ( vViewMax.x > vPosition[1].x ) ? vViewMax.x : vPosition[1].x;
	vViewMax.x = ( vViewMax.x > vPosition[2].x ) ? vViewMax.x : vPosition[2].x;
	vViewMax.x = ( vViewMax.x > vPosition[3].x ) ? vViewMax.x : vPosition[3].x;

	vViewMax.y = ( vViewMax.y > vPosition[0].y ) ? vViewMax.y : vPosition[0].y;
	vViewMax.y = ( vViewMax.y > vPosition[1].y ) ? vViewMax.y : vPosition[1].y;
	vViewMax.y = ( vViewMax.y > vPosition[2].y ) ? vViewMax.y : vPosition[2].y;
	vViewMax.y = ( vViewMax.y > vPosition[3].y ) ? vViewMax.y : vPosition[3].y;

	vViewMax.z = ( vViewMax.z > vPosition[0].z ) ? vViewMax.z : vPosition[0].z;
	vViewMax.z = ( vViewMax.z > vPosition[1].z ) ? vViewMax.z : vPosition[1].z;
	vViewMax.z = ( vViewMax.z > vPosition[2].z ) ? vViewMax.z : vPosition[2].z;
	vViewMax.z = ( vViewMax.z > vPosition[3].z ) ? vViewMax.z : vPosition[3].z;

	vViewMin.x = ( vViewMin.x < vPosition[0].x ) ? vViewMin.x : vPosition[0].x;
	vViewMin.x = ( vViewMin.x < vPosition[1].x ) ? vViewMin.x : vPosition[1].x;
	vViewMin.x = ( vViewMin.x < vPosition[2].x ) ? vViewMin.x : vPosition[2].x;
	vViewMin.x = ( vViewMin.x < vPosition[3].x ) ? vViewMin.x : vPosition[3].x;

	vViewMin.y = ( vViewMin.y < vPosition[0].y ) ? vViewMin.y : vPosition[0].y;
	vViewMin.y = ( vViewMin.y < vPosition[1].y ) ? vViewMin.y : vPosition[1].y;
	vViewMin.y = ( vViewMin.y < vPosition[2].y ) ? vViewMin.y : vPosition[2].y;
	vViewMin.y = ( vViewMin.y < vPosition[3].y ) ? vViewMin.y : vPosition[3].y;

	vViewMin.z = ( vViewMin.z < vPosition[0].z ) ? vViewMin.z : vPosition[0].z;
	vViewMin.z = ( vViewMin.z < vPosition[1].z ) ? vViewMin.z : vPosition[1].z;
	vViewMin.z = ( vViewMin.z < vPosition[2].z ) ? vViewMin.z : vPosition[2].z;
	vViewMin.z = ( vViewMin.z < vPosition[3].z ) ? vViewMin.z : vPosition[3].z;

	m_vViewMax		= vViewMax;
	m_vViewMin		= vViewMin;
	m_vViewMax.y	= vCenter.y;
	m_vViewMin.y	= vCenter.y;
}

VOID	DxEffectShadow::ReSetTexUV ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DWORD dwTexSize )
{
	//	Note : ī�޶�� ������ �Ÿ��� ���� �˸µ��� �����
	//
	m_fDistance = 500.f;
	while ( !CheckViewPort ( pd3dDevice, pFrameMesh->GetFrameRoot(), m_fDistance, dwTexSize ) )
	{
		m_fDistance += 100.f;
	}

	//	Note : ���� ī�޶� CV�� ������ �� ���ϱ�.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = (m_vMax+m_vMin) * 0.5f;

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 100000.f, (float)dwTexSize, (float)dwTexSize, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 100000.f;
	float	fWidth		= (float)dwTexSize;
	float	fHeight		= (float)dwTexSize;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i�� ���� 4�̻��϶�
		//				�հ��� ���� ��ǥ�̴�.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fWidth/fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : �� �����
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMin.z );
	vPosition[1] = D3DXVECTOR3 ( m_vMin.x, vCenter.y, m_vMax.z );
	vPosition[2] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMax.z );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : �� �����
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	vViewMax = vViewMin = vPosition[0];

	vViewMax.x = ( vViewMax.x > vPosition[0].x ) ? vViewMax.x : vPosition[0].x;
	vViewMax.x = ( vViewMax.x > vPosition[1].x ) ? vViewMax.x : vPosition[1].x;
	vViewMax.x = ( vViewMax.x > vPosition[2].x ) ? vViewMax.x : vPosition[2].x;
	vViewMax.x = ( vViewMax.x > vPosition[3].x ) ? vViewMax.x : vPosition[3].x;

	vViewMax.y = ( vViewMax.y > vPosition[0].y ) ? vViewMax.y : vPosition[0].y;
	vViewMax.y = ( vViewMax.y > vPosition[1].y ) ? vViewMax.y : vPosition[1].y;
	vViewMax.y = ( vViewMax.y > vPosition[2].y ) ? vViewMax.y : vPosition[2].y;
	vViewMax.y = ( vViewMax.y > vPosition[3].y ) ? vViewMax.y : vPosition[3].y;

	vViewMax.z = ( vViewMax.z > vPosition[0].z ) ? vViewMax.z : vPosition[0].z;
	vViewMax.z = ( vViewMax.z > vPosition[1].z ) ? vViewMax.z : vPosition[1].z;
	vViewMax.z = ( vViewMax.z > vPosition[2].z ) ? vViewMax.z : vPosition[2].z;
	vViewMax.z = ( vViewMax.z > vPosition[3].z ) ? vViewMax.z : vPosition[3].z;

	vViewMin.x = ( vViewMin.x < vPosition[0].x ) ? vViewMin.x : vPosition[0].x;
	vViewMin.x = ( vViewMin.x < vPosition[1].x ) ? vViewMin.x : vPosition[1].x;
	vViewMin.x = ( vViewMin.x < vPosition[2].x ) ? vViewMin.x : vPosition[2].x;
	vViewMin.x = ( vViewMin.x < vPosition[3].x ) ? vViewMin.x : vPosition[3].x;

	vViewMin.y = ( vViewMin.y < vPosition[0].y ) ? vViewMin.y : vPosition[0].y;
	vViewMin.y = ( vViewMin.y < vPosition[1].y ) ? vViewMin.y : vPosition[1].y;
	vViewMin.y = ( vViewMin.y < vPosition[2].y ) ? vViewMin.y : vPosition[2].y;
	vViewMin.y = ( vViewMin.y < vPosition[3].y ) ? vViewMin.y : vPosition[3].y;

	vViewMin.z = ( vViewMin.z < vPosition[0].z ) ? vViewMin.z : vPosition[0].z;
	vViewMin.z = ( vViewMin.z < vPosition[1].z ) ? vViewMin.z : vPosition[1].z;
	vViewMin.z = ( vViewMin.z < vPosition[2].z ) ? vViewMin.z : vPosition[2].z;
	vViewMin.z = ( vViewMin.z < vPosition[3].z ) ? vViewMin.z : vPosition[3].z;

	m_vViewMax		= vViewMax;
	m_vViewMin		= vViewMin;
	m_vViewMax.y	= vCenter.y;
	m_vViewMin.y	= vCenter.y;
}

HRESULT DxEffectShadow::ProjectionObjD ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeShadow, BOOL bTest, DxSetLandMan *pSetLandMan )
{
	DxFrame*	pDxFrame;
	DxMeshes*	pmsMeshs;

	pDxFrame = pSetLandMan->GetFrameMesh()->GetFrameRoot();
	pmsMeshs = pDxFrame->pmsMeshs;

	D3DXVECTOR3 vDir = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;

	CLIPVOLUME pCV = DxViewPort::GetInstance().GetClipVolume ();

	ResetLocalMeshs ( pd3dDevice, pSetLandMan->GetFrameMesh()->GetFrameRoot(), D3DFVF_XYZ );
	if ( bTest )	ProjectionObj ( pd3dDevice, pDxFrame, &pCV, &vDir );				// �ٴ����� ( ���� ) ( Test�� ���� )			
	else			PickingObjD ( pd3dDevice, pDxFrame, pframeShadow, &pCV, &vDir, pSetLandMan );	// Picking �� �ϸ鼭 Ÿ�Ͽ� ���� - Direction

	D3DXMATRIX		matView, matProj, matOrgView, matOrgProj;

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;
	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	D3DXMatrixLookAtLH ( &matView, &vTempLookatPt, &vCenter, &vUpVec );		// �� �����..
	D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, 1.f, 0.5f, 100000.f);	// �������� �����

	pd3dDevice->GetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOrgProj );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	DWORD TexFactor = D3DCOLOR_COLORVALUE(m_fShadowD,m_fShadowD,m_fShadowD,m_fShadowD);
	pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

	m_pSavedColorSB->Capture();
	m_pEffectColorSB->Apply();

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );

	pd3dDevice->SetRenderTarget ( 0, m_pASurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	RenderObjD ( pd3dDevice, pDxFrame );					// ��������..

	sBackupTarget.RestoreTarget ( pd3dDevice );

	D3DXLoadSurfaceFromSurface (	m_pBSurface, NULL, NULL,						// ��������.
									m_pASurface, NULL, NULL, D3DX_FILTER_NONE, 0xff000000 );
	D3DXLoadSurfaceFromSurface (	m_pSShadowSurface, NULL, NULL,							// ��������.
									m_pASurface, NULL, NULL, D3DX_FILTER_NONE, 0xff000000 );

	m_pSavedColorSB->Apply();

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOrgProj );

	return S_OK;
}

HRESULT DxEffectShadow::ProjectionObjP ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeShadow, DxSetLandMan *pSetLandMan )
{
	D3DXVECTOR3	vViewMax, vViewMin;

	DxFrame*	pDxFrame;
	DxMeshes*	pmsMeshs;

	pDxFrame = pSetLandMan->GetFrameMesh()->GetFrameRoot();
	pmsMeshs = pDxFrame->pmsMeshs;

	CLIPVOLUME pCV = DxViewPort::GetInstance().GetClipVolume ();

	// Picking �� �ϸ鼭 Ÿ�Ͽ� ���� - Point
	PickingObjP ( pd3dDevice, pDxFrame, pframeShadow, &pCV, pSetLandMan );			

	return S_OK;
}

HRESULT DxEffectShadow::PickingObjD ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame*	pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, D3DXVECTOR3 *vDir, DxSetLandMan *pSetLandMan )
{
	HRESULT hr = S_OK;

	//	Note : ��굵 �ϸ� �ȵ�. ^^
	//
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;
	else if ( pframeCur->pEffectPrev )		return S_OK;
	else if ( pframeCur->pEffect )
	{
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_TILING )		return S_OK;
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_RIVER )		return S_OK;		
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_WATERLIGHT )	return S_OK;
	}
	else if ( pframeCur->pEffectNext )
	{
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_WATER2 )	return S_OK;
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_SHADOW )	return S_OK;
	}

	struct VERTEX { D3DXVECTOR3	vPos; };
	VERTEX*		pVertex;
	DxFrame*	pframeChild;
	DxMeshes*	pmsMeshs;
	D3DXVECTOR3	vVert1;
	D3DXVECTOR3	vVert2;
	D3DXVECTOR3	vCollPos;

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;

	CCollisionMap* pCollisionMap = pSetLandMan->GetCollisionMap();

	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				m_pUse = new BOOL [ pmsMeshs->m_pLocalMesh->GetNumVertices() ];

				pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertex );

				for ( DWORD i=0; i<pmsMeshs->m_pLocalMesh->GetNumVertices() ; i++ )
				{
					D3DXVec3TransformCoord ( &vVert1, &pVertex[i].vPos, &pframeCur->matCombined );
					vVert2 = vVert1;

					vVert1 += (*vDir*0.001f);
					vVert2 += (*vDir*1000.f);

					if ( pframeShadow )
					{
						if ( !pCollisionMap->IsCollision( vVert1, vVert2, TRUE, pframeShadow, NULL, TRUE ) )		// Picking �� �ȵǾ������� ������ �ʿ��ϴ�.
						{
							D3DXVec3TransformCoord ( &vCollPos, &pVertex[i].vPos, &pframeCur->matCombined );
							m_pUse[i] = FALSE;
						}
						else
						{
							vCollPos = pCollisionMap->GetCollisionPos();
							m_pUse[i] = TRUE;
						}
					}
					vCollPos.y = m_vViewMax.y;		// �̰� �ϳ��� ��ġ �� �´� ���� ���� �Ǿ���.

					//	Note : �� ����
					//
					pVertex[i].vPos = vCollPos;
				}
				pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();

				DeleteFaces ( pd3dDevice, pmsMeshs->m_pLocalMesh, D3DXVECTOR3(0.f,0.f,0.f) );
			

				SAFE_DELETE_ARRAY ( m_pUse );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = PickingObjD ( pd3dDevice, pframeChild, pframeShadow, pCV, vDir, pSetLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return hr;
}

VOID	DxEffectShadow::DeleteFaces ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXVECTOR3 vDelVert )
{
	WORD*		pIndices;

	// �ε���
	pMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	for ( DWORD i=0; i<pMesh->GetNumFaces(); i++ )
	{
		if ( m_pUse[ pIndices[i*3+0] ] || m_pUse[ pIndices[i*3+1] ] || m_pUse[ pIndices[i*3+2] ] )
		{

		}
		else
		{
			pIndices[i*3+0] = 0;
			pIndices[i*3+1] = 0;
			pIndices[i*3+2] = 0;
		}
	}
	pMesh->UnlockIndexBuffer ();
}

HRESULT DxEffectShadow::PickingObjP	( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, DxSetLandMan *pSetLandMan )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3	vPos;
	DXLIGHT* pLight = DxLightMan::GetInstance()->GetLightHead();
	while ( pLight )
	{
		vPos = D3DXVECTOR3 ( pLight->m_Light.Position.x, pLight->m_Light.Position.y, pLight->m_Light.Position.z );
		vPos.y += 15.f;

		if ( (m_vMax.x+200.f) >= vPos.x && (m_vMin.x-200.f) <= vPos.x && (m_vMax.z+200.f) >= vPos.z && (m_vMin.z-200.f) <= vPos.z )
		{
			ResetLocalMeshs ( pd3dDevice, pSetLandMan->GetFrameMesh()->GetFrameRoot(), D3DFVF_XYZ );		// ���󺹱�

			PickingObjP	( pd3dDevice, pframeCur, pframeShadow, pCV, &vPos, pLight->m_Light.Range, pSetLandMan );

			D3DXMATRIX		matView, matProj, matOrgView, matOrgProj;

			D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;
			D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
			D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
			vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

			D3DXMatrixLookAtLH ( &matView, &vTempLookatPt, &vCenter, &vUpVec );		// �� �����..
			D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, 1.f, 0.5f, 100000.f);	// �������� �����

			pd3dDevice->GetTransform ( D3DTS_VIEW, &matOrgView );
			pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOrgProj );

			pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
			pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );





			DxBackUpRendTarget sBackupTarget ( pd3dDevice );

			pd3dDevice->SetRenderTarget ( 0, m_pASurface );
			pd3dDevice->SetDepthStencilSurface ( NULL );
			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);




			RenderObjP ( pd3dDevice, pframeCur );					// ��������..

			{
				float		fLength = pLight->m_Light.Range + 120.f;
				float		fMulti = 2000.f;
				SHADOW_TEX1		pShadowPos[4];

				pShadowPos[0].vPos = D3DXVECTOR3 ( pLight->m_Light.Position.x - fLength*fMulti, 
													pLight->m_Light.Position.y,
													pLight->m_Light.Position.z + fLength*fMulti );

				pShadowPos[1].vPos = D3DXVECTOR3 ( pLight->m_Light.Position.x + fLength*fMulti, 
													pLight->m_Light.Position.y,
													pLight->m_Light.Position.z + fLength*fMulti );

				pShadowPos[2].vPos = D3DXVECTOR3 ( pLight->m_Light.Position.x - fLength*fMulti, 
													pLight->m_Light.Position.y,
													pLight->m_Light.Position.z - fLength*fMulti );

				pShadowPos[3].vPos = D3DXVECTOR3 ( pLight->m_Light.Position.x + fLength*fMulti, 
													pLight->m_Light.Position.y,
													pLight->m_Light.Position.z - fLength*fMulti );

				pShadowPos[0].vTex1 = D3DXVECTOR2 ( -0.5f*fMulti + 0.5f,	-0.5f*fMulti + 0.5f );
				pShadowPos[1].vTex1 = D3DXVECTOR2 ( 0.5f*fMulti + 0.5f,		-0.5f*fMulti + 0.5f );
				pShadowPos[2].vTex1 = D3DXVECTOR2 ( -0.5f*fMulti + 0.5f,	0.5f*fMulti + 0.5f );
				pShadowPos[3].vTex1 = D3DXVECTOR2 ( 0.5f*fMulti + 0.5f,		0.5f*fMulti + 0.5f );

				D3DXMATRIX		matIdentity;
				D3DXMatrixIdentity ( &matIdentity );

				pd3dDevice->SetTexture ( 0, m_pFilterTex );
				pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

				m_pSavedFilerSB->Capture();
				m_pEffectFilerSB->Apply();

				pd3dDevice->SetFVF ( SHADOW_TEX1::FVF );
				pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pShadowPos, sizeof(SHADOW_TEX1) );

				m_pSavedFilerSB->Apply();
			}


			//	Note : ���� �۾�
			//
			pd3dDevice->SetRenderTarget ( 0, m_pBSurface );
			pd3dDevice->SetDepthStencilSurface ( NULL );
			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

			D3DXLoadSurfaceFromSurface (	m_pBSurface, NULL, NULL,						// ��������.
											m_pSShadowSurface, NULL, NULL, D3DX_FILTER_NONE, 0xff000000 );

			pd3dDevice->SetRenderTarget ( 0, m_pSShadowSurface );
			pd3dDevice->SetDepthStencilSurface ( NULL );
			pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

			{
				struct SHADOW_TEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; };
				SHADOW_TEX2	pShadowPos[4];

				pShadowPos[0].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMin.z );
				pShadowPos[1].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMin.z );
				pShadowPos[2].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMax.z );
				pShadowPos[3].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMax.z );

				pShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
				pShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f, 0.f );
				pShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f, 1.f );
				pShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f );

				pShadowPos[0].vTex2 = D3DXVECTOR2 ( 0.f, 0.f );
				pShadowPos[1].vTex2 = D3DXVECTOR2 ( 1.f, 0.f );
				pShadowPos[2].vTex2 = D3DXVECTOR2 ( 0.f, 1.f );
				pShadowPos[3].vTex2 = D3DXVECTOR2 ( 1.f, 1.f );

				D3DXMATRIX		matIdentity;
				D3DXMatrixIdentity ( &matIdentity );

				pd3dDevice->SetTexture ( 0, m_pATexture );
				pd3dDevice->SetTexture ( 1, m_pBTexture );
				pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

				m_pSavedAddSB->Capture();
				m_pEffectAddSB->Apply();

				pd3dDevice->SetFVF ( D3DFVF_XYZ|D3DFVF_TEX2 );
				pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pShadowPos, sizeof(SHADOW_TEX2) );

				m_pSavedAddSB->Apply();

				pd3dDevice->SetTexture ( 1, NULL );
			}

			sBackupTarget.RestoreTarget ( pd3dDevice );

			pd3dDevice->SetTransform ( D3DTS_VIEW, &matOrgView );
			pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOrgProj );
		}

		pLight = pLight->pNext;
	}

	return hr;
}

HRESULT DxEffectShadow::PickingObjP	( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, DxFrame *pframeShadow, CLIPVOLUME *pCV, D3DXVECTOR3 *vPos, float fRange, DxSetLandMan *pSetLandMan )
{
	HRESULT hr = S_OK;

	struct VERTEX { D3DXVECTOR3	vPos; };
	VERTEX*		pVertex;
	DxFrame*	pframeChild;
	DxMeshes*	pmsMeshs;
	D3DXVECTOR3	vVert1;
	D3DXVECTOR3	vVert2;
	D3DXVECTOR3	vCollPos;
	D3DXVECTOR3	vDir;
	D3DXVECTOR3	vNorDir;
	D3DXVECTOR3	vDirXZ;
	D3DXVECTOR3	vColl_Light;
	//float		fColl_Light;
	float		fAddRange = fRange*2.f;		// ������ 2���.

	DXLIGHT*	pLight = DxLightMan::GetInstance()->GetDirectLight();
	DWORD		dwAmbient = (DWORD)(pLight->m_Light.Ambient.r*255.f);

	CCollisionMap* pCollisionMap = pSetLandMan->GetCollisionMap();

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;






	//	Note : ��굵 �ϸ� �ȵ�. ^^
	//
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	goto _RETURN;
	else if ( pframeCur->pEffectPrev )		goto _RETURN;
	else if ( pframeCur->pEffect )
	{
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_TILING )		goto _RETURN;//return S_OK;
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_RIVER )		goto _RETURN;//return S_OK;		
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_WATERLIGHT )	goto _RETURN;//return S_OK;
	}
	else if ( pframeCur->pEffectNext )
	{
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_WATER2 )	goto _RETURN;//return S_OK;
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_SHADOW )	goto _RETURN;//return S_OK;
	}






	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				m_pUse = new BOOL [ pmsMeshs->m_pLocalMesh->GetNumVertices() ];

				pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertex );

				DWORD i = 0;
				for ( i=0; i<pmsMeshs->m_pLocalMesh->GetNumVertices() ; i++ )
				{
					D3DXVec3TransformCoord ( &vVert1, &pVertex[i].vPos, &pframeCur->matCombined );
					vVert2 = vVert1;
					vDir = vVert1 - *vPos;	// 

					if ( D3DXVec3Length(&vDir) >= fAddRange || vPos->y < vVert1.y )			// �Ÿ��� ����� ��
					{
						m_pUse[i] = FALSE;

						vCollPos.x = vVert1.x + vDir.x*2.f;
						vCollPos.y = vCenter.y;
						vCollPos.z = vVert1.z + vDir.z*2.f;
					}
					else											// �Ÿ��ȿ� ��� ���� ��
					{
						//vNorDir = vDir;

						//D3DXVec3Normalize ( &vNorDir, &vDir );

						//vVert1 += (vNorDir*0.001f);
						//vVert2 += (vNorDir*1000.f);

						vVert1.y += 1.f;
						vVert2 = vVert1;
						vVert2.y -=	1000.f;

						if ( pframeShadow )
						{
							vCollPos.x = vVert1.x + vDir.x*2.f;
							vCollPos.y = vCenter.y;
							vCollPos.z = vVert1.z + vDir.z*2.f;

							if ( pCollisionMap->IsCollision( vVert1, vVert2, TRUE, pframeShadow, NULL, TRUE ) )
							{
								vCollPos = pCollisionMap->GetCollisionPos();

								if ( vCollPos.y <= vVert1.y )
								{
									m_pUse[i] = TRUE;

									vDirXZ = D3DXVECTOR3 ( vDir.x, 0.f, vDir.z );

									vVert1.y -= 1.f;
									if ( vVert1.y - vCollPos.y > 0.f )		vCollPos += vDirXZ * ( vVert1.y - vCollPos.y ) * 0.1f;
									else									vCollPos += vDirXZ * ( vCollPos.y - vVert1.y ) * 0.1f;
								}
								else		m_pUse[i] = FALSE;
							}
							else			m_pUse[i] = TRUE;
						}
					}

					vCollPos.y = m_vViewMax.y;		// �̰� �ϳ��� ��ġ �� �´� ���� ���� �Ǿ���.

					pVertex[i].vPos = vCollPos;
				}
				pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();

				DeleteFaces ( pd3dDevice, pmsMeshs->m_pLocalMesh, pVertex[i].vPos );
			
				SAFE_DELETE_ARRAY ( m_pUse );
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

_RETURN:
	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = PickingObjP ( pd3dDevice, pframeChild, pframeShadow, pCV, vPos, fRange, pSetLandMan );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return hr;
}

HRESULT DxEffectShadow::ProjectionObj ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, CLIPVOLUME *pCV, D3DXVECTOR3 *vDir )	// ���ܱ׸���
{
	HRESULT hr = S_OK;

	struct VERTEX { D3DXVECTOR3	vPos; };
	VERTEX*		pVertex;
	DxFrame*	pframeChild;
	DxMeshes*	pmsMeshs;
	D3DXVECTOR3	vTemp;

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;

	//	Note : ��굵 �ϸ� �ȵ�. ^^
	//
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;
	else if ( pframeCur->pEffectPrev )		return S_OK;
	else if ( pframeCur->pEffect )
	{
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_TILING )		return S_OK;
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_RIVER )		return S_OK;		
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_WATERLIGHT )	return S_OK;
	}
	else if ( pframeCur->pEffectNext )
	{
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_WATER2 )	return S_OK;
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_SHADOW )	return S_OK;
	}

	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{

				pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertex );

				for ( DWORD i=0; i<pmsMeshs->m_pLocalMesh->GetNumVertices() ; i++ )
				{
					D3DXVec3TransformCoord ( &vTemp, &pVertex[i].vPos, &pframeCur->matCombined );

					if ( vTemp.y > m_vMin.y )
					{
						pVertex[i].vPos.x = vTemp.x - (vTemp.y - vCenter.y) * (vDir->x/vDir->y);
						pVertex[i].vPos.y = vCenter.y;
						pVertex[i].vPos.z = vTemp.z - (vTemp.y - vCenter.y) * (vDir->z/vDir->y);
					}
					else
					{
						pVertex[i].vPos.x = vTemp.x;
						pVertex[i].vPos.y = vCenter.y;
						pVertex[i].vPos.z = vTemp.z;
					}
				}

				pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();

			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = ProjectionObj ( pd3dDevice, pframeChild, pCV, vDir );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return hr;
}

HRESULT DxEffectShadow::RenderObjD ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur )
{
	HRESULT hr = S_OK;
	D3DXMATRIX		matIdentity;
	DxMeshes*		pmsMeshs;
	DxFrame*		pframeChild;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	goto _RETURN;
	else if ( pframeCur->pEffectPrev )		goto _RETURN;
	else if ( pframeCur->pEffect )
	{
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_TILING )		goto _RETURN;//return S_OK;
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_RIVER )		goto _RETURN;//return S_OK;		
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_WATERLIGHT )	goto _RETURN;//return S_OK;
	}
	else if ( pframeCur->pEffectNext )
	{
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_WATER2 )	goto _RETURN;//return S_OK;
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_SHADOW )	goto _RETURN;//return S_OK;
	}
	//pframeCur->


	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		D3DXMatrixIdentity ( &matIdentity );
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->Render ( pd3dDevice );
				pmsMeshs->RenderAlpha ( pd3dDevice, TRUE );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

_RETURN:
	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = RenderObjD ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}


	return S_OK;
}

HRESULT DxEffectShadow::RenderObjP ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur )
{
	HRESULT hr = S_OK;
	D3DXMATRIX		matIdentity;
	DxMeshes*		pmsMeshs;
	DxFrame*		pframeChild;

	//	Note : ������̼� �޽� �׸��� ���� �˻�
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	goto _RETURN;
	else if ( pframeCur->pEffectPrev )		goto _RETURN;
	else if ( pframeCur->pEffect )
	{
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_TILING )		goto _RETURN;//return S_OK;
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_RIVER )		goto _RETURN;//return S_OK;		
		if ( pframeCur->pEffect->GetTypeID() == DEF_EFFECT_WATERLIGHT )	goto _RETURN;//return S_OK;
	}
	else if ( pframeCur->pEffectNext )
	{
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_WATER2 )	goto _RETURN;//return S_OK;
		if ( pframeCur->pEffectNext->GetTypeID() == DEF_EFFECT_SHADOW )	goto _RETURN;//return S_OK;
	}

	DWORD TexFactor = D3DCOLOR_COLORVALUE(m_fShadowP,m_fShadowP,m_fShadowP,m_fShadowP);;
	pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

	DWORD dwZWriteEnable, dwFogEnable, dwLighting, dwCullMode;
	DWORD dwColorARG1, dwColorOP;

	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,	&dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,	&dwFogEnable );
	pd3dDevice->GetRenderState( D3DRS_LIGHTING,		&dwLighting );
	pd3dDevice->GetRenderState( D3DRS_CULLMODE,		&dwCullMode );

	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1,	&dwColorARG1 );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,		D3DTA_TFACTOR  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);

	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : Ʈ������ ����.
		//
		D3DXMatrixIdentity ( &matIdentity );
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		if (FAILED(hr))
			return hr;

		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->Render ( pd3dDevice );
				pmsMeshs->RenderAlpha ( pd3dDevice, TRUE );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,	dwFogEnable );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,		dwLighting );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,		dwCullMode );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	dwColorARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );


_RETURN:
	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = RenderObjP ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}


	return S_OK;
}

BOOL DxEffectShadow::CheckViewPort ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pframeCur, float fDistance, DWORD dwTexSize )
{
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = (m_vMax+m_vMin) * 0.5f;

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 100000.f, (float)dwTexSize, (float)dwTexSize, D3DX_PI/4 );	// CLIPVOLUME

	float	fTempA = pCV.pLeft.a - pCV.pBottom.a;
	float	fTempB = pCV.pLeft.b - pCV.pBottom.b;
	float	fTempC = pCV.pLeft.c - pCV.pBottom.c;
	float	fTempD = pCV.pLeft.d - pCV.pBottom.d;

	vTemp = D3DXVECTOR3 ( m_vMax.x, m_vMax.y, m_vMax.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMax.x, m_vMax.y, m_vMin.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMax.x, m_vMin.y, m_vMax.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMin.x, m_vMax.y, m_vMax.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMax.x, m_vMin.y, m_vMin.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMin.x, m_vMin.y, m_vMax.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMin.x, m_vMax.y, m_vMin.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;
	vTemp = D3DXVECTOR3 ( m_vMin.x, m_vMin.y, m_vMin.z );
	if ( !COLLISION::IsCollisionVolume ( pCV, vTemp, vTemp ) )	return FALSE;

	return TRUE;
}

HRESULT DxEffectShadow::ResetLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DWORD dwFVF )
{
	HRESULT hr = S_OK;
	D3DXMATRIX		matIdentity;
	DxMeshes*		pmsMeshs;
	DxFrame*		pframeChild;

	if ( pframeCur->pmsMeshs != NULL )
	{
		//	Note : �޽� �׸���.
		//
		pmsMeshs = pframeCur->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->MakeLocalMeshs ( pd3dDevice, dwFVF );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	//	Note : �ڽ� ������ �׸���.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = ResetLocalMeshs ( pd3dDevice, pframeChild, dwFVF );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

VOID DxEffectShadow::ImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX		matView, matProj, matOrgView, matOrgProj;

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;
	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	D3DXMatrixLookAtLH ( &matView, &vTempLookatPt, &vCenter, &vUpVec );		// �� �����..
	D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, 1.f, 0.5f, 100000.f);	// �������� �����

	pd3dDevice->GetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOrgProj );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );


	DxBackUpRendTarget sBackupTarget ( pd3dDevice );

	//	Note : ���� �۾�
	//
	pd3dDevice->SetRenderTarget ( 0, m_pBSurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	D3DXLoadSurfaceFromSurface (	m_pBSurface, NULL, NULL,						// ��������.
									m_pSShadowSurface, NULL, NULL, D3DX_FILTER_NONE, 0xff000000 );

	pd3dDevice->SetRenderTarget ( 0, m_pSShadowSurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	{
		SHADOW_TEX1	pShadowPos[4];

		pShadowPos[0].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMin.z );
		pShadowPos[1].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMin.z );
		pShadowPos[2].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMax.z );
		pShadowPos[3].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMax.z );

		pShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
		pShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f, 0.f );
		pShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f, 1.f );
		pShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f );

		D3DXMATRIX		matIdentity;
		D3DXMatrixIdentity ( &matIdentity );

		pd3dDevice->SetTexture ( 0, m_pBTexture );
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

		m_pSavedBlurSB->Capture();
		m_pEffectBlurSB->Apply();

		pd3dDevice->SetFVF ( SHADOW_TEX1::FVF );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pShadowPos, sizeof(SHADOW_TEX1) );

		m_pSavedBlurSB->Apply();
	}

	sBackupTarget.RestoreTarget ( pd3dDevice );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOrgProj );
}

VOID DxEffectShadow::ImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSURFACEQ pSrcSurface, LPDIRECT3DTEXTUREQ pSrcTexture, LPDIRECT3DSURFACEQ pDestSurface )
{
	D3DXMATRIX		matView, matProj, matOrgView, matOrgProj;

	D3DXVECTOR3		vCenter = (m_vMax+m_vMin) * 0.5f;
	//vCenter = ( 0.f, 0.f, 0.f );
	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;	//m_fDistance	�̰� ���ϰ� �簢�� ���ؾ� �Ѵ�.
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	D3DXMatrixLookAtLH ( &matView, &vTempLookatPt, &vCenter, &vUpVec );		// �� �����..
	D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, 1.f, 0.5f, 100000.f);	// �������� �����

	pd3dDevice->GetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOrgProj );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );


	DxBackUpRendTarget sBackupTarget ( pd3dDevice );

	//	Note : ���� �۾�
	//
	pd3dDevice->SetRenderTarget ( 0, pDestSurface );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	{
		SHADOW_TEX1	pShadowPos[4];

		pShadowPos[0].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMin.z );
		pShadowPos[1].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMin.z );
		pShadowPos[2].vPos = D3DXVECTOR3 ( m_vViewMax.x, m_vViewMax.y, m_vViewMax.z );
		pShadowPos[3].vPos = D3DXVECTOR3 ( m_vViewMin.x, m_vViewMax.y, m_vViewMax.z );

		pShadowPos[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
		pShadowPos[1].vTex1 = D3DXVECTOR2 ( 1.f, 0.f );
		pShadowPos[2].vTex1 = D3DXVECTOR2 ( 0.f, 1.f );
		pShadowPos[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f );

		D3DXMATRIX		matIdentity;
		D3DXMatrixIdentity ( &matIdentity );

		pd3dDevice->SetTexture ( 0, pSrcTexture );
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

		m_pSavedBlurSB->Capture();
		m_pEffectBlurSB->Apply();

		pd3dDevice->SetFVF ( SHADOW_TEX1::FVF );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pShadowPos, sizeof(SHADOW_TEX1) );

		m_pSavedBlurSB->Apply();
	}

	sBackupTarget.RestoreTarget ( pd3dDevice );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matOrgView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOrgProj );
}