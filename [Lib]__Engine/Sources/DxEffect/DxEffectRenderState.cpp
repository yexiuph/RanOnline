// DxEffectRenderState.cpp: implementation of the DxEffectRenderState class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxOctreeMesh.h"
#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./SerialFile.h"

#include "./dxeffectrenderstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectRenderState::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD DxEffectRenderState::TYPEID = DEF_EFFECT_RENDERSTATE;
const DWORD	DxEffectRenderState::VERSION = 0x00000104;
const char DxEffectRenderState::NAME[] = "[ 렌더 조정, 깜빡임 ]";
const DWORD DxEffectRenderState::FLAG = _EFF_REPLACE;


void DxEffectRenderState::GetProperty( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp  = (PBYTE) &m_Property;
	dwSize = sizeof(RENDERSTATE_PROPERTY);
	dwVer  = VERSION;
}

void DxEffectRenderState::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if( dwVer == VERSION && dwSize == sizeof(RENDERSTATE_PROPERTY) )
	{
		memcpy( &m_Property, pProp, dwSize );
	}

	else if( dwVer == 0x00000100 && dwSize == sizeof(RENDERSTATE_PROPERTY_100) )
	{
		RENDERSTATE_PROPERTY_100 Property00 = *((RENDERSTATE_PROPERTY_100*)pProp);

		m_nType = 0;
		m_FillMode = Property00.m_FillMode;
		m_CullMode = Property00.m_CullMode;
		m_Lighting = Property00.m_Lighting; 
		m_ZWriteEnable = Property00.m_ZWriteEnable;
		m_AlphaBlendeEnable = Property00.m_AlphaBlendeEnable;
		m_SrcBlend = Property00.m_SrcBlend; 
		m_DestBlend = Property00.m_DestBlend;
		m_ColorOP_0 = Property00.m_ColorOP_0;
		m_ColorARG1_0 = Property00.m_ColorARG1_0;
		m_ColorARG2_0 = Property00.m_ColorARG2_0;
		m_AlphaOP_0 = Property00.m_AlphaOP_0;
		m_AlphaARG1_0 = Property00.m_AlphaARG1_0;
		m_AlphaARG2_0 = Property00.m_AlphaARG2_0;

		m_fColorPower	= 0.6f;
		m_bLight		= FALSE;
		m_bNearLight	= FALSE;
		m_fSpeed	= 0.1f;		// 적을수록 빠르다.
		m_nRate		= 90;
		m_vMax		= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vMin		= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		D3DXMatrixIdentity ( &m_matFrameComb );
	}

	else if( dwVer == 0x00000101 && dwSize == sizeof(RENDERSTATE_PROPERTY_101) )
	{
		RENDERSTATE_PROPERTY_101 Property = *((RENDERSTATE_PROPERTY_101*)pProp);

		m_nType = Property.m_nType;
		m_FillMode = Property.m_FillMode;
		m_CullMode = Property.m_CullMode;
		m_Lighting = Property.m_Lighting; 
		m_ZWriteEnable = Property.m_ZWriteEnable;
		m_AlphaBlendeEnable = Property.m_AlphaBlendeEnable;
		m_SrcBlend = Property.m_SrcBlend; 
		m_DestBlend = Property.m_DestBlend;
		m_ColorOP_0 = Property.m_ColorOP_0;
		m_ColorARG1_0 = Property.m_ColorARG1_0;
		m_ColorARG2_0 = Property.m_ColorARG2_0;
		m_AlphaOP_0 = Property.m_AlphaOP_0;
		m_AlphaARG1_0 = Property.m_AlphaARG1_0;
		m_AlphaARG2_0 = Property.m_AlphaARG2_0;

		m_fColorPower	= 0.6f;
		m_bLight	= FALSE;
		m_bNearLight = FALSE;
		m_fSpeed	= 0.1f;		// 적을수록 빠르다.
		m_nRate		= 90;
		m_vMax		= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vMin		= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		D3DXMatrixIdentity ( &m_matFrameComb );
	}

	else if( dwVer == 0x00000102 && dwSize == sizeof(RENDERSTATE_PROPERTY_102) )
	{
		RENDERSTATE_PROPERTY_102 Property = *((RENDERSTATE_PROPERTY_102*)pProp);

		m_nType = Property.m_nType;
		m_FillMode = Property.m_FillMode;
		m_CullMode = Property.m_CullMode;
		m_Lighting = Property.m_Lighting; 
		m_ZWriteEnable = Property.m_ZWriteEnable;
		m_AlphaBlendeEnable = Property.m_AlphaBlendeEnable;
		m_SrcBlend = Property.m_SrcBlend; 
		m_DestBlend = Property.m_DestBlend;
		m_ColorOP_0 = Property.m_ColorOP_0;
		m_ColorARG1_0 = Property.m_ColorARG1_0;
		m_ColorARG2_0 = Property.m_ColorARG2_0;
		m_AlphaOP_0 = Property.m_AlphaOP_0;
		m_AlphaARG1_0 = Property.m_AlphaARG1_0;
		m_AlphaARG2_0 = Property.m_AlphaARG2_0;

		m_vMax	= D3DXVECTOR3 ( Property.m_fMaxX, Property.m_fMaxY, Property.m_fMaxZ );
		m_vMin	= D3DXVECTOR3 ( Property.m_fMinX, Property.m_fMinY, Property.m_fMinZ );
		m_matFrameComb = Property.m_matFrameComb;

		m_fColorPower	= 0.6f;
		m_bLight	= FALSE;
		m_bNearLight = FALSE;
		m_fSpeed	= 0.1f;		// 적을수록 빠르다.
		m_nRate		= 90;
	}

	else if( dwVer == 0x00000103 && dwSize == sizeof(RENDERSTATE_PROPERTY_102) )	// 실수로 인해 103버전이 사이즈 틀린것 2개가 있다..
	{
		RENDERSTATE_PROPERTY_102 Property = *((RENDERSTATE_PROPERTY_102*)pProp);

		m_nType = Property.m_nType;
		m_FillMode = Property.m_FillMode;
		m_CullMode = Property.m_CullMode;
		m_Lighting = Property.m_Lighting; 
		m_ZWriteEnable = Property.m_ZWriteEnable;
		m_AlphaBlendeEnable = Property.m_AlphaBlendeEnable;
		m_SrcBlend = Property.m_SrcBlend; 
		m_DestBlend = Property.m_DestBlend;
		m_ColorOP_0 = Property.m_ColorOP_0;
		m_ColorARG1_0 = Property.m_ColorARG1_0;
		m_ColorARG2_0 = Property.m_ColorARG2_0;
		m_AlphaOP_0 = Property.m_AlphaOP_0;
		m_AlphaARG1_0 = Property.m_AlphaARG1_0;
		m_AlphaARG2_0 = Property.m_AlphaARG2_0;

		m_vMax	= D3DXVECTOR3 ( Property.m_fMaxX, Property.m_fMaxY, Property.m_fMaxZ );
		m_vMin	= D3DXVECTOR3 ( Property.m_fMinX, Property.m_fMinY, Property.m_fMinZ );
		m_matFrameComb = Property.m_matFrameComb;

		m_fColorPower	= 0.6f;
		m_bLight	= FALSE;
		m_bNearLight = FALSE;
		m_fSpeed	= 0.1f;		// 적을수록 빠르다.
		m_nRate		= 90;
	}

	else if( dwVer == 0x00000103 && dwSize == sizeof(RENDERSTATE_PROPERTY) )		// 2번째 버전 
	{
		memcpy( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectRenderState::DxEffectRenderState() :
	m_nType(0),
	m_FillMode(D3DFILL_SOLID), 
	m_CullMode(D3DCULL_CCW), 
	m_Lighting(TRUE), 
	m_ZWriteEnable(TRUE),
	m_AlphaBlendeEnable(FALSE), 
	m_SrcBlend(D3DBLEND_DESTCOLOR), 
	m_DestBlend(D3DBLEND_SRCCOLOR),
	m_ColorOP_0(D3DTOP_MODULATE), 
	m_ColorARG1_0(D3DTA_TEXTURE), 
	m_ColorARG2_0(D3DTA_DIFFUSE), 
	m_AlphaOP_0(D3DTOP_DISABLE), 
	m_AlphaARG1_0(D3DTA_TEXTURE), 
	m_AlphaARG2_0(D3DTA_DIFFUSE),
	m_fColorPower(0.6f),
	m_bLight(FALSE),
	m_bNearLight(FALSE),
	m_fSpeed(0.1f),
	m_nRate(90),
	m_vMax(0,0,0),
	m_vMin(0,0,0),

	m_pLight(NULL),

	m_bUse(TRUE),
	m_fElapsedTime(0.f)
{
	m_pOcMesh = new DxOcMeshes;

	D3DXMatrixIdentity ( &m_matFrameComb );
}

DxEffectRenderState::~DxEffectRenderState()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	if ( m_pOcMesh )
	{
		SAFE_DELETE ( m_pOcMesh );
		m_pOcMesh = NULL;
	}
}

HRESULT DxEffectRenderState::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				m_pOcMesh->CloneMesh ( pd3dDevice, pmsMeshs, &pframeCur->matCombined );
			}
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBoundBox ( pmsMeshs->m_pLocalMesh );
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	GetLight ();	// 가까운 빛 얻기

	return S_OK;
}

HRESULT DxEffectRenderState::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}
	
	return AdaptToDxFrameChild ( pFrame, pd3dDevice );												//	H/W
}

HRESULT DxEffectRenderState::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectRenderState::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffectRenderState::InvalidateDeviceObjects ()
{	
	return S_OK;
}

HRESULT DxEffectRenderState::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{

	return S_OK;
}

HRESULT DxEffectRenderState::DeleteDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectRenderState::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !m_pOcMesh )	return S_OK;
	if ( !m_bLight	)	return S_OK;

	if ( m_nRate < 100 )
		m_fElapsedTime += fElapsedTime;

	//	Note : 깜빡임 구현
	//
	if ( m_nRate < 100 && m_fElapsedTime > m_fSpeed )
	{
		m_fElapsedTime = 0.f;

		int Temp = rand()%100 + 1;	// 1~100 사이의 값
		if ( m_nRate >= Temp )
		{
			m_bUse = TRUE;

			if ( m_pLight )
				m_pLight->m_Light.Diffuse = m_pLight->m_BaseDiffuse;
		}
		else
		{
			m_bUse = FALSE;

			if ( m_pLight && m_bNearLight )
			{
				m_pLight->m_Light.Diffuse.a = m_pLight->m_BaseDiffuse.a*m_fColorPower;
				m_pLight->m_Light.Diffuse.r = m_pLight->m_BaseDiffuse.r*m_fColorPower;
				m_pLight->m_Light.Diffuse.g = m_pLight->m_BaseDiffuse.g*m_fColorPower;
				m_pLight->m_Light.Diffuse.b = m_pLight->m_BaseDiffuse.b*m_fColorPower;
			}
		}

		GetLight ();	// 가까운 빛 얻기
	}

	return S_OK;
}

HRESULT DxEffectRenderState::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	if ( !m_pOcMesh )	return S_OK;

	//	Note : 렌더 함
	//
	if ( m_bUse )
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

		DWORD	dwFillMode, dwCullMode, dwLighting, dwZWriteEnable,
				dwAlphaBlendeEnable, dwAlphaRef, dwSrcBlend, dwDestBlend,
				dwColorOP, dwColorARG1, dwColorARG2, dwAlphaOP, dwAlphaARG1, dwAlphaARG2;

		pd3dDevice->GetRenderState ( D3DRS_FILLMODE,		&dwFillMode );
		pd3dDevice->GetRenderState ( D3DRS_CULLMODE,		&dwCullMode );
		pd3dDevice->GetRenderState ( D3DRS_LIGHTING,		&dwLighting );
		pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,	&dwZWriteEnable );

		pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendeEnable );
		pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
		pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
		pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );

		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,	&dwColorARG1 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG2, &dwColorARG2 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG1, &dwAlphaARG1 );
		pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAARG2, &dwAlphaARG2 );



		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,		m_FillMode );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,		m_CullMode );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		m_Lighting );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	m_ZWriteEnable );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	m_AlphaBlendeEnable );
		pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			0x00 );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			m_SrcBlend );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			m_DestBlend );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	m_ColorOP_0 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	m_ColorARG1_0 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, m_ColorARG2_0 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	m_AlphaOP_0 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, m_AlphaARG1_0 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, m_AlphaARG2_0 );


		m_pOcMesh->Render ( pd3dDevice );


		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,		dwFillMode );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,		dwCullMode );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		dwLighting );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	dwZWriteEnable );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendeEnable );
		pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	dwColorARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, dwColorARG2 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	dwAlphaOP );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, dwAlphaARG1 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, dwAlphaARG2 );
	}
	else
	{
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

		m_pOcMesh->Render ( pd3dDevice );
	}

	return S_OK;
}

void DxEffectRenderState::GetLight ()
{
	if ( m_pLight )	return;

	float		fTemp;
	float		fDistance = 10000.f;
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = ( m_vMax + m_vMin )*0.5f; 
	D3DXVec3TransformCoord ( &vCenter, &vCenter, &m_matFrameComb );
	DXLIGHT*	pCurLight = DxLightMan::GetInstance()->GetLightHead ();

	while ( pCurLight )
	{
		vTemp.x = pCurLight->m_Light.Position.x - vCenter.x;
		vTemp.y = pCurLight->m_Light.Position.y - vCenter.y;
		vTemp.z = pCurLight->m_Light.Position.z - vCenter.z;
		fTemp	= D3DXVec3Length ( &vTemp );
		if ( fTemp < fDistance )
		{
			m_pLight		= pCurLight;
			fDistance = fTemp;
		}

		pCurLight = pCurLight->pNext;
	}
}

void DxEffectRenderState::SaveBuffer ( CSerialFile &SFile )
{
	//	Size 저장 부분.
	SFile.BeginBlock ();
	{
		m_pOcMesh->SaveFile ( SFile );
	}
	SFile.EndBlock ();
}

void DxEffectRenderState::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( dwVer == VERSION )
	{
		DWORD dwBuffSize = SFile.ReadBlockSize (); //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );
	}
	else if ( dwVer == 0x0103 )
	{
		DWORD dwBuffSize = SFile.ReadBlockSize (); //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		m_pOcMesh->LoadFile ( SFile, pd3dDevice );
	}
	else if ( dwVer == 0x0102 )
	{
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );
	}
	else if ( dwVer == 0x0101 )
	{
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );
	}
	else if ( dwVer == 0x0100 )
	{
		DWORD dwBuffSize;
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
	}
	else
	{
		GASSERT(0&&"DxEffectRenderState::LoadBufferSet() 읽을수 없는 버전의 효과가 발견되었습니다.");
	}

	//if ( dwVer == 0x0100 )
	//{
	//	DWORD dwBuffSize;
	//	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//	return;
	//}

	//m_pOcMesh->LoadFile ( SFile, pd3dDevice );
}

void DxEffectRenderState::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( dwVer == VERSION )
	{
		DWORD dwBuffSize = SFile.ReadBlockSize (); //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );		
	}
	else if ( dwVer == 0x0103 )
	{
		DWORD dwBuffSize = SFile.ReadBlockSize (); //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );		
	}
	else if ( dwVer == 0x0102 )
	{
		m_pOcMesh->LoadFile ( SFile, pd3dDevice );
	}
	else
	{
		GASSERT(0&&"DxEffectRenderState::LoadBufferSet() 읽을수 없는 버전의 효과가 발견되었습니다.");
	}

	//if ( dwVer == 0x0100 )
	//{
	//	DWORD dwBuffSize;
	//	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//	if ( m_pOcMesh )
	//	{
	//		SAFE_DELETE ( m_pOcMesh );
	//		m_pOcMesh = NULL;
	//	}
	//	return;
	//}
}

HRESULT DxEffectRenderState::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectRenderState* pSrcRender = (DxEffectRenderState*) pSrcEffect;
	GASSERT(pd3dDevice);

	if ( pSrcRender->m_pOcMesh )
	{
		SAFE_DELETE(m_pOcMesh);
		m_pOcMesh = new DxOcMeshes;
		m_pOcMesh->CloneMesh ( pd3dDevice, pSrcRender->m_pOcMesh, NULL );
	}
	
	return S_OK;
}

HRESULT DxEffectRenderState::SetBoundBox ( LPD3DXMESH pMesh )
{
	DWORD dwVertices = pMesh->GetNumVertices();

	VERTEX*	pVertices;
	pMesh->LockVertexBuffer ( 0, (VOID**)&pVertices );

	m_vMax.x = m_vMin.x = pVertices[0].p.x;
	m_vMax.y = m_vMin.y = pVertices[0].p.y;
	m_vMax.z = m_vMin.z = pVertices[0].p.z;

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		m_vMax.x = (m_vMax.x > pVertices[i].p.x) ? m_vMax.x : pVertices[i].p.x;
		m_vMin.x = (m_vMin.x > pVertices[i].p.x) ? pVertices[i].p.x : m_vMin.x;

		m_vMax.y = (m_vMax.y > pVertices[i].p.y) ? m_vMax.y : pVertices[i].p.y;
		m_vMin.y = (m_vMin.y > pVertices[i].p.y) ? pVertices[i].p.y : m_vMin.y;

		m_vMax.z = (m_vMax.z > pVertices[i].p.z) ? m_vMax.z : pVertices[i].p.z;
		m_vMin.z = (m_vMin.z > pVertices[i].p.z) ? pVertices[i].p.z : m_vMin.z;
	}
	pMesh->UnlockVertexBuffer ();

	return S_OK;
}

void DxEffectRenderState::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax.x = m_vMax.x;
	vMax.y = m_vMax.y;
	vMax.z = m_vMax.z;

	vMin.x = m_vMin.x;
	vMin.y = m_vMin.y;
	vMin.z = m_vMin.z;

	COLLISION::TransformAABB( vMax, vMin, m_matFrameComb );
}


BOOL DxEffectRenderState::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = m_matFrameComb._41;
	vCenter.y = m_matFrameComb._42;
	vCenter.z = m_matFrameComb._43;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}