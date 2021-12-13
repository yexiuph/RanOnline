// DxEffectShadowLow.cpp: implementation of the DxEffectShadowLowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxShadowMap.h"

#include "./glperiod.h"

#include "./EditMeshs.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"

#include "./DxEffectShadowLow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectShadowLow::SHADOW_TEX1::FVF		= D3DFVF_XYZ|D3DFVF_TEX1;
const DWORD DxEffectShadowLow::SHADOW_TEX1_COLOR::FVF	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectShadowLow::SHADOW_TEX2::FVF		= D3DFVF_XYZ|D3DFVF_TEX2;
const DWORD DxEffectShadowLow::SHADOW_TEX2_COLOR::FVF	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;

const DWORD DxEffectShadowLow::TYPEID = DEF_EFFECT_SHADOW_LOW;
const DWORD	DxEffectShadowLow::VERSION = 0x106;
const char	DxEffectShadowLow::NAME[] = "[삭제후 그림자 다시 생성]";
const DWORD DxEffectShadowLow::FLAG = _EFF_AFTER;

char		DxEffectShadowLow::m_szPath[MAX_PATH]	= "";

LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pSavedTex1SSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pDrawTex1SSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pSavedTex1CSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pDrawTex1CSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pSavedTex1ColorSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pDrawTex1ColorSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pSavedTex2SB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pDrawTex2SB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pSavedTex2ColorSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadowLow::m_pDrawTex2ColorSB	= NULL;

void DxEffectShadowLow::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SHADOW_PROPERTY_L);
	dwVer = VERSION;
}

void DxEffectShadowLow::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SHADOW_PROPERTY_L) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==0x105 && dwSize==sizeof(SHADOW_PROPERTY_L) )

	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==0x104 && dwSize==sizeof(SHADOW_PROPERTY_L_103_104) )
	{
		SHADOW_PROPERTY_L_103_104 _pProp = *((SHADOW_PROPERTY_L_103_104*)pProp);

		m_dwFlag	= 0;
		if ( _pProp.m_bShadow )		m_dwFlag	|= USESTATICSHADOW;
		if ( _pProp.m_bDirect )		m_dwFlag	|= USEDIRECTSHADOW;
		if ( _pProp.m_bPoint )		m_dwFlag	|= USEPOINTSHADOW;
		
		m_vViewMax	= _pProp.m_vViewMax;
		m_vViewMin	= _pProp.m_vViewMin;
		m_vMax		= _pProp.m_vMax;
		m_vMin		= _pProp.m_vMin;
		m_dwTexSize	= _pProp.m_dwTexSize;
		m_fShadowD	= _pProp.m_fShadowD;
		m_fShadowP	= _pProp.m_fShadowP;

		StringCchCopy( m_szFilter,		MAX_PATH, _pProp.m_szFilter );
		StringCchCopy( m_szTexDayTime,	MAX_PATH, _pProp.m_szTexDayTime );
		StringCchCopy( m_szTexNight,	MAX_PATH, _pProp.m_szTexNight );
	}
	else if ( dwVer==0x103 && dwSize==sizeof(SHADOW_PROPERTY_L_103_104) )
	{
		SHADOW_PROPERTY_L_103_104 _pProp = *((SHADOW_PROPERTY_L_103_104*)pProp);

		m_dwFlag	= 0;
		if ( _pProp.m_bShadow )		m_dwFlag	|= USESTATICSHADOW;
		if ( _pProp.m_bDirect )		m_dwFlag	|= USEDIRECTSHADOW;
		if ( _pProp.m_bPoint )		m_dwFlag	|= USEPOINTSHADOW;
		
		m_vViewMax	= _pProp.m_vViewMax;
		m_vViewMin	= _pProp.m_vViewMin;
		m_vMax		= _pProp.m_vMax;
		m_vMin		= _pProp.m_vMin;
		m_dwTexSize	= _pProp.m_dwTexSize;
		m_fShadowD	= _pProp.m_fShadowD;
		m_fShadowP	= _pProp.m_fShadowP;

		StringCchCopy( m_szFilter,		MAX_PATH, _pProp.m_szFilter );
		StringCchCopy( m_szTexDayTime,	MAX_PATH, _pProp.m_szTexDayTime );
		StringCchCopy( m_szTexNight,	MAX_PATH, _pProp.m_szTexNight );
	}
	else if ( dwVer==0x102 && dwSize==sizeof(SHADOW_PROPERTY_L_101_102) )
	{
		SHADOW_PROPERTY_L_101_102 _pProp = *((SHADOW_PROPERTY_L_101_102*)pProp);

		m_dwFlag	= 0;
		if ( _pProp.m_bShadow )		m_dwFlag	|= USESTATICSHADOW;
		if ( _pProp.m_bDirect )		m_dwFlag	|= USEDIRECTSHADOW;
		if ( _pProp.m_bPoint )		m_dwFlag	|= USEPOINTSHADOW;

		m_vViewMax	= _pProp.m_vViewMax;
		m_vViewMin	= _pProp.m_vViewMin;
		m_vMax		= _pProp.m_vMax;
		m_vMin		= _pProp.m_vMin;

		StringCchCopy( m_szFilter,		MAX_PATH, _pProp.m_szFilter );
		StringCchCopy( m_szTexDayTime,	MAX_PATH, _pProp.m_szTexDayTime );
		StringCchCopy( m_szTexNight,	MAX_PATH, _pProp.m_szTexNight );
	}
	if ( dwVer==0x101 && dwSize==sizeof(SHADOW_PROPERTY_L_101_102) )
	{		
		SHADOW_PROPERTY_L_101_102 _pProp = *((SHADOW_PROPERTY_L_101_102*)pProp);

		m_dwFlag	= 0;
		if ( _pProp.m_bShadow )		m_dwFlag	|= USESTATICSHADOW;
		if ( _pProp.m_bDirect )		m_dwFlag	|= USEDIRECTSHADOW;
		if ( _pProp.m_bPoint )		m_dwFlag	|= USEPOINTSHADOW;
		m_vViewMax	= _pProp.m_vViewMax;
		m_vViewMin	= _pProp.m_vViewMin;
		m_vMax		= _pProp.m_vMax;
		m_vMin		= _pProp.m_vMin;

		StringCchCopy( m_szFilter,		MAX_PATH, _pProp.m_szFilter );
		StringCchCopy( m_szTexDayTime,	MAX_PATH, _pProp.m_szTexDayTime );
		StringCchCopy( m_szTexNight,	MAX_PATH, _pProp.m_szTexNight );
	}
	else if ( dwVer==0x100 && dwSize==sizeof(SHADOW_PROPERTY_L_100) )
	{
		SHADOW_PROPERTY_L_100 _pProp = *((SHADOW_PROPERTY_L_100*)pProp);

		m_dwFlag	|= USEDIRECTSHADOW;

		m_vViewMax	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vViewMin	= D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		m_vMax		= _pProp.m_vMax;
		m_vMin		= _pProp.m_vMin;

		StringCchCopy( m_szFilter,		MAX_PATH, "StaticShadowFilter.tga" );
		StringCchCopy( m_szTexDayTime,	MAX_PATH, "Test_Day.dds" );
		StringCchCopy( m_szTexNight,	MAX_PATH, "Test_Day.dds" );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectShadowLow::DxEffectShadowLow() :
	m_pShadowVB(NULL),
	m_vViewMax(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vViewMin(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vMax(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vMin(D3DXVECTOR3(0.f,0.f,0.f)),
	m_fDistance(500.f),
	m_pSSDayTex(NULL),
	m_pSSNightTex(NULL),
	m_pFilterTex(NULL),
	m_pThisFrame(NULL),
	m_pUse(NULL),
	m_pShadowTex(NULL),

	m_dwTexSize(512),		// 0-64, 1-128, 2-256, 3-512, 4-1024
	m_fShadowD(0.4f),
	m_fShadowP(0.7f),

	m_pASurface(NULL),
	m_pATexture(NULL),
	m_pBSurface(NULL),
	m_pBTexture(NULL),
	m_pSShadowSurface(NULL),
	m_pSShadowTexture(NULL)
{
	m_dwFlag	|= USEDIRECTSHADOW;

	m_emVBSET	= EM_VBSET_NONE;

	D3DXMatrixIdentity ( &m_matWorld );

	StringCchCopy( m_szFilter,		MAX_PATH, "StaticShadowFilter.tga" );
	StringCchCopy( m_szTexDayTime,	MAX_PATH, "Test_Day.dds" );
	StringCchCopy( m_szTexNight,	MAX_PATH, "Test_Day.dds" );
}

DxEffectShadowLow::~DxEffectShadowLow()
{
	m_pThisFrame = NULL;
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
	SAFE_RELEASE ( m_pShadowVB );
	SAFE_DELETE_ARRAY ( m_pUse );
	SAFE_DELETE_ARRAY ( m_pShadowTex );

	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);
}

HRESULT DxEffectShadowLow::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				CreateShadow( pd3dDevice, pmsMeshs, pframeCur->matCombined );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectShadowLow::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );		//	S/W
}

HRESULT DxEffectShadowLow::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedTex1SSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawTex1SSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedTex1CSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawTex1CSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedTex1ColorSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawTex1ColorSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_ADD );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedTex2SB );
		else			pd3dDevice->EndStateBlock( &m_pDrawTex2SB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_ADD );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedTex2ColorSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawTex2ColorSB );
	}

	return S_OK;
}

HRESULT DxEffectShadowLow::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedTex1SSB );
	SAFE_RELEASE( m_pDrawTex1SSB );
	SAFE_RELEASE( m_pSavedTex1CSB );
	SAFE_RELEASE( m_pDrawTex1CSB );
	SAFE_RELEASE( m_pSavedTex1ColorSB );
	SAFE_RELEASE( m_pDrawTex1ColorSB );
	SAFE_RELEASE( m_pSavedTex2SB );
	SAFE_RELEASE( m_pDrawTex2SB );
	SAFE_RELEASE( m_pSavedTex2ColorSB );
	SAFE_RELEASE( m_pDrawTex2ColorSB );
	
	return S_OK;
}

HRESULT DxEffectShadowLow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 문자가 같을 경우 0가 나온다.		다른 경우 참이다.		틀려야 배경이 바뀐다.
	//
	if ( strcmp ( m_szTexDayTime, m_szTexNight ) )	m_dwFlag |= USECHANGESHADOW;		// 이 옵션은 저장과는 별로 상관 없다.
	else											m_dwFlag &= ~USECHANGESHADOW;

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	if ( m_dwFlag & USESTATICSHADOW )
	{
		TextureManager::LoadTexture ( m_szTexDayTime, pd3dDevice, m_pSSDayTex, 0, 0 );

		if ( m_pSSDayTex )
		{
			D3DSURFACE_DESC pDesc;
			m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
			m_dwTexSize = pDesc.Width;
		}

		TextureManager::LoadTexture ( m_szTexNight, pd3dDevice, m_pSSNightTex, 0, 0 );

		if ( m_pSSNightTex )
		{
			D3DSURFACE_DESC pDesc;
			m_pSSNightTex->GetLevelDesc ( 0, &pDesc );
			m_dwTexSize = pDesc.Width;
		}
	}

	return S_OK;
}

HRESULT DxEffectShadowLow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectShadowLow::InvalidateDeviceObjects ()
{
	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);

	return S_OK;
}

HRESULT DxEffectShadowLow::DeleteDeviceObjects ()
{
	HRESULT hr  = S_OK;

	TextureManager::ReleaseTexture( m_szTexDayTime, m_pSSDayTex );
	TextureManager::ReleaseTexture( m_szTexNight, m_pSSNightTex );

	return hr;
}

HRESULT DxEffectShadowLow::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	m_pThisFrame		= pframeCur;
	DxMeshes*	pmsMeshs;
	pmsMeshs = pframeCur->pmsMeshs;

	if ( !pmsMeshs )	return S_OK;

	PROFILE_BEGIN("DxEffectShadowLow");

	VBSET	emVBSET;
	emVBSET = (VBSET) CheckVBSET ();					// VBSET 을 구한다.
	if ( m_emVBSET != emVBSET )
	{
		m_emVBSET = emVBSET;							// 현재의 m_emVBSET 을 넣는다.
		CreateVB ( pd3dDevice, pmsMeshs, emVBSET );		// 상황에 맞게 생성한다.
	}
	
	if ( DxShadowMap::GetInstance().GetShadowDetail() != SHADOW_DISABLE )
	{
		//	Note : m_matLastWVP 이 값을 알아내기 위하여 이것을 한다.
		//
		D3DXMATRIX texMat;

		D3DXMATRIX	matLastWVP_D;
		matLastWVP_D	= DxShadowMap::GetInstance().GetMatrix_D ();

		D3DXMatrixMultiply ( &matLastWVP_D, &(pframeCur->matCombined),	&matLastWVP_D );

		//set special texture matrix for shadow mapping
		unsigned int range;
		float fOffsetX = 0.5f + (0.5f / (float)512);
		float fOffsetY = 0.5f + (0.5f / (float)512);

		range = 0xFFFFFFFF >> (32 - 16);//m_bitDepth)

		float fBias    = -0.001f * (float)range;
		D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
									0.0f,     -0.5f,     0.0f,         0.0f,
									0.0f,     0.0f,     (float)range, 0.0f,
									fOffsetX, fOffsetY, fBias,        1.0f );
		
		D3DXMatrixMultiply(&texMat, &matLastWVP_D, &texScaleBiasMat);

		SetShadowUV ( texMat, emVBSET );
	}

	if ( emVBSET == EM_VBSET_1TEX_S )		
	{
		RenderObjTex  ( pframeCur, pd3dDevice, m_pSSDayTex, SHADOW_TEX1::FVF, m_pSavedTex1SSB, m_pDrawTex1SSB );
	}
	else if ( emVBSET == EM_VBSET_1TEX_C )		
	{
		RenderTnL ( pframeCur, pd3dDevice, SHADOW_TEX1::FVF, m_pSavedTex1CSB, m_pDrawTex1CSB );
	}
	else if ( emVBSET == EM_VBSET_1TEX_COLOR )	
	{
		RenderTnL ( pframeCur, pd3dDevice, SHADOW_TEX1_COLOR::FVF, m_pSavedTex1ColorSB, m_pDrawTex1ColorSB );
	}
	else if ( emVBSET == EM_VBSET_2TEX )		
	{
		RenderShadow  ( pframeCur, pd3dDevice, m_pSSDayTex, SHADOW_TEX2::FVF, m_pSavedTex2SB, m_pDrawTex2SB );
	}
	else if ( emVBSET == EM_VBSET_2TEX_COLOR )	
	{
		RenderShadow  ( pframeCur, pd3dDevice, m_pSSDayTex, SHADOW_TEX2_COLOR::FVF, m_pSavedTex2ColorSB, m_pDrawTex2ColorSB );

	}


	PROFILE_END("DxEffectShadowLow");

	return S_OK;
}

HRESULT	DxEffectShadowLow::SetShadowUV ( D3DXMATRIX matDirect, VBSET emVBSET )
{
	if ( !m_pShadowVB )	return S_OK;

	if ( GLPeriod::GetInstance().IsOffLight() )	// 낮일 경우
	{
		if ( emVBSET == EM_VBSET_1TEX_C )
		{
			SHADOW_TEX1*	pVert;
			D3DXVECTOR4		pD;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pD.x = matDirect._11*pVert[i].vPos.x + matDirect._21*(pVert[i].vPos.y-0.05f) + matDirect._31*pVert[i].vPos.z + matDirect._41;
				pD.y = matDirect._12*pVert[i].vPos.x + matDirect._22*(pVert[i].vPos.y-0.05f) + matDirect._32*pVert[i].vPos.z + matDirect._42;
				pD.z = matDirect._14*pVert[i].vPos.x + matDirect._24*(pVert[i].vPos.y-0.05f) + matDirect._34*pVert[i].vPos.z + matDirect._44;

				pVert[i].vTex1 = D3DXVECTOR2 ( (pD.x)/pD.z, (pD.y)/pD.z );
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_1TEX_COLOR )
		{
			SHADOW_TEX1_COLOR*	pVert;
			D3DXVECTOR4			pD;
			DWORD				dwColor;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pD.x = matDirect._11*pVert[i].vPos.x + matDirect._21*(pVert[i].vPos.y-0.05f) + matDirect._31*pVert[i].vPos.z + matDirect._41;
				pD.y = matDirect._12*pVert[i].vPos.x + matDirect._22*(pVert[i].vPos.y-0.05f) + matDirect._32*pVert[i].vPos.z + matDirect._42;
				pD.z = matDirect._14*pVert[i].vPos.x + matDirect._24*(pVert[i].vPos.y-0.05f) + matDirect._34*pVert[i].vPos.z + matDirect._44;

				pD.x = (pD.x)/pD.z;
				pD.y = (pD.y)/pD.z;

				pVert[i].vTex1 = D3DXVECTOR2 ( pD.x, pD.y );

				pD.x = (pD.x - 0.5f) * 2.f;
				pD.y = (pD.y - 0.5f) * 2.f;

				pD.x = fabsf ( pD.x );
				pD.y = fabsf ( pD.y );

				pD.x = 1.f - pD.x;
				pD.y = 1.f - pD.y;

				pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
				if ( pD.x <= 0.f )	pD.x = 0.f;

				dwColor = (DWORD)(pD.x*255.f);

				pVert[i].vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_2TEX )
		{
			SHADOW_TEX2*	pVert;
			D3DXVECTOR4		pD;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pD.x = matDirect._11*pVert[i].vPos.x + matDirect._21*(pVert[i].vPos.y-0.05f) + matDirect._31*pVert[i].vPos.z + matDirect._41;
				pD.y = matDirect._12*pVert[i].vPos.x + matDirect._22*(pVert[i].vPos.y-0.05f) + matDirect._32*pVert[i].vPos.z + matDirect._42;
				pD.z = matDirect._14*pVert[i].vPos.x + matDirect._24*(pVert[i].vPos.y-0.05f) + matDirect._34*pVert[i].vPos.z + matDirect._44;

				pVert[i].vTex1 = D3DXVECTOR2 ( (pD.x)/pD.z, (pD.y)/pD.z );
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_2TEX_COLOR )
		{
			SHADOW_TEX2_COLOR*	pVert;
			D3DXVECTOR4			pD;
			DWORD				dwColor;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pD.x = matDirect._11*pVert[i].vPos.x + matDirect._21*(pVert[i].vPos.y-0.05f) + matDirect._31*pVert[i].vPos.z + matDirect._41;
				pD.y = matDirect._12*pVert[i].vPos.x + matDirect._22*(pVert[i].vPos.y-0.05f) + matDirect._32*pVert[i].vPos.z + matDirect._42;
				pD.z = matDirect._14*pVert[i].vPos.x + matDirect._24*(pVert[i].vPos.y-0.05f) + matDirect._34*pVert[i].vPos.z + matDirect._44;

				pD.x = (pD.x)/pD.z;
				pD.y = (pD.y)/pD.z;

				pVert[i].vTex1 = D3DXVECTOR2 ( pD.x, pD.y );

				pD.x = (pD.x - 0.5f) * 2.f;
				pD.y = (pD.y - 0.5f) * 2.f;

				pD.x = fabsf ( pD.x );
				pD.y = fabsf ( pD.y );

				pD.x = 1.f - pD.x;
				pD.y = 1.f - pD.y;

				pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
				if ( pD.x <= 0.f )	pD.x = 0.f;

				dwColor = (DWORD)(pD.x*255.f);

				pVert[i].vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			}
			m_pShadowVB->Unlock ( );
		}
	}
	else				// 밤일 경우
	{
		D3DXVECTOR3	vMin = DxShadowMap::GetInstance().GetViewMin();
		D3DXVECTOR3 vMax = DxShadowMap::GetInstance().GetViewMax();
		float		fDeltaX = vMax.x-vMin.x;
		float		fDeltaZ = vMax.z-vMin.z;
		DWORD		dwTexSize = 512;
		float		fMicroMove = 1.f/dwTexSize;//(1<<(m_dwTexSize+6));

		if ( emVBSET == EM_VBSET_1TEX_C )
		{
			SHADOW_TEX1*	pVert;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pVert[i].vTex1 = D3DXVECTOR2 (	( 1.f - ((pVert[i].vPos.x-vMin.x)/fDeltaX) ) + fMicroMove, 
														((pVert[i].vPos.z-vMin.z)/fDeltaZ) + fMicroMove);
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_1TEX_COLOR )
		{
			SHADOW_TEX1_COLOR*	pVert;
			D3DXVECTOR2			pD;
			DWORD				dwColor;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pVert[i].vTex1 = D3DXVECTOR2 (	( 1.f - ((pVert[i].vPos.x-vMin.x)/fDeltaX) ) + fMicroMove, 
														((pVert[i].vPos.z-vMin.z)/fDeltaZ) + fMicroMove);

				pD = pVert[i].vTex1;

				pD.x = (pD.x - 0.5f) * 2.f;
				pD.y = (pD.y - 0.5f) * 2.f;

				pD.x = fabsf ( pD.x );
				pD.y = fabsf ( pD.y );

				pD.x = 1.f - pD.x;
				pD.y = 1.f - pD.y;

				pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
				if ( pD.x <= 0.f )	pD.x = 0.f;

				dwColor = (DWORD)(pD.x*255.f);

				pVert[i].vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_2TEX )
		{
			SHADOW_TEX2*	pVert;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pVert[i].vTex1 = D3DXVECTOR2 (	( 1.f - ((pVert[i].vPos.x-vMin.x)/fDeltaX) ) + fMicroMove, 
														((pVert[i].vPos.z-vMin.z)/fDeltaZ) + fMicroMove);
			}
			m_pShadowVB->Unlock ( );
		}
		else if ( emVBSET == EM_VBSET_2TEX_COLOR )
		{
			SHADOW_TEX2_COLOR*	pVert;
			D3DXVECTOR2			pD;
			DWORD				dwColor;
			m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<m_dwVertices; i++ )
			{						
				pVert[i].vTex1 = D3DXVECTOR2 (	( 1.f - ((pVert[i].vPos.x-vMin.x)/fDeltaX) ) + fMicroMove, 
														((pVert[i].vPos.z-vMin.z)/fDeltaZ) + fMicroMove);

				pD = pVert[i].vTex1;

				pD.x = (pD.x - 0.5f) * 2.f;
				pD.y = (pD.y - 0.5f) * 2.f;

				pD.x = fabsf ( pD.x );
				pD.y = fabsf ( pD.y );

				pD.x = 1.f - pD.x;
				pD.y = 1.f - pD.y;

				pD.x = (pD.x<=pD.y) ? pD.x: pD.y;
				if ( pD.x <= 0.f )	pD.x = 0.f;

				dwColor = (DWORD)(pD.x*255.f);

				pVert[i].vColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
			}
			m_pShadowVB->Unlock ( );
		}
	}

	return S_OK;
}

HRESULT DxEffectShadowLow::CreateShadow( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX	matWorld )
{
	D3DXVECTOR3		vPos, vMax, vMin;
	float			fMicroMove;
	DWORD			dwVertices;
	VERTEX*			pVertices;


	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		dwVertices		= pmsMeshs->m_pLocalMesh->GetNumVertices ();
		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );

	}
	else
	{
		dwVertices		= pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pVertices );
	}

	//	Note : LocalMesh 일 경우에만 최대점 최소점 찾는 작업을 시킨다.
	//
	if ( pmsMeshs->m_pLocalMesh )


	{
		float		fMicroMove = 1.f/m_dwTexSize;


		D3DXVECTOR3	vPos;


		SAFE_DELETE_ARRAY ( m_pShadowTex );
		m_pShadowTex = new D3DXVECTOR2 [ dwVertices ];
		for ( DWORD i=0; i<dwVertices; i++ )
		{
			D3DXVec3TransformCoord ( &vPos, &pVertices[i].vPos, &matWorld );
			m_pShadowTex[i]	= D3DXVECTOR2 (	1.f-(vPos.x-m_vViewMin.x)/(m_vViewMax.x-m_vViewMin.x) + fMicroMove, 
												(vPos.z-m_vViewMin.z)/(m_vViewMax.z-m_vViewMin.z) + fMicroMove);
		}

		m_vMin = m_vMax = pVertices[0].vPos;

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			m_vMax.x = (pVertices[i].vPos.x > m_vMax.x) ? pVertices[i].vPos.x : m_vMax.x ;
			m_vMax.y = (pVertices[i].vPos.y > m_vMax.y) ? pVertices[i].vPos.y : m_vMax.y ;
			m_vMax.z = (pVertices[i].vPos.z > m_vMax.z) ? pVertices[i].vPos.z : m_vMax.z ;

			m_vMin.x = (pVertices[i].vPos.x < m_vMin.x)	? pVertices[i].vPos.x : m_vMin.x ;
			m_vMin.y = (pVertices[i].vPos.y < m_vMin.y) ? pVertices[i].vPos.y : m_vMin.y ;
			m_vMin.z = (pVertices[i].vPos.z < m_vMin.z) ? pVertices[i].vPos.z : m_vMin.z ;
		}

		D3DXVec3TransformCoord ( &m_vMax, &m_vMax, &matWorld );
		D3DXVec3TransformCoord ( &m_vMin, &m_vMin, &matWorld );



	}

	m_matWorld		= matWorld;
	m_dwVertices	= dwVertices;

	if ( !m_pShadowTex && !pmsMeshs->m_pLocalMesh )		// (LoadBuffer) 옥트리 메시 로드에서 생성이 안되었을 경우 m_pShadowTex를 만든다.
	{
		vMax		= m_vViewMax;
		vMin		= m_vViewMin;
		fMicroMove	= 1.f/m_dwTexSize;


		m_pShadowTex = new D3DXVECTOR2[dwVertices];

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			D3DXVec3TransformCoord ( &vPos, &pVertices[i].vPos, &matWorld );
			m_pShadowTex[i] = D3DXVECTOR2 (	1.f-(vPos.x-vMin.x)/(vMax.x-vMin.x) + fMicroMove, 
											(vPos.z-vMin.z)/(vMax.z-vMin.z) + fMicroMove);
		}
	}

	if ( pmsMeshs->m_pLocalMesh )		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
	else								pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer();

	VBSET	emVBSET;
	emVBSET = (VBSET) CheckVBSET ();				// VBSET 을 구한다.
	m_emVBSET = emVBSET;							// 현재의 m_emVBSET 을 넣는다.
	CreateVB ( pd3dDevice, pmsMeshs, emVBSET );		// 상황에 맞게 생성한다.

	return S_OK;
}

int DxEffectShadowLow::CheckVBSET ()
{
	if ( DxShadowMap::GetInstance().GetShadowDetail()==SHADOW_DISABLE )			// 캐릭터 그림자를 사용 안 할 경우
	{
		if ( (m_dwFlag&USESTATICSHADOW) && DxEffectMan::GetInstance().GetSSDetail() )	// 지형그림자를 사용 할 경우
		{
			return EM_VBSET_1TEX_S;
		}
		else																		// 지형그림자를 사용 하지 않을 경우
		{
			return EM_VBSET_NONE;
		}
	}
	else																	// 캐릭터 그림자를 사용 할 경우
	{
		if ( (m_dwFlag&USESTATICSHADOW) && DxEffectMan::GetInstance().GetSSDetail() )	// 지형그림자를 사용 할 경우
		{
			return EM_VBSET_2TEX_COLOR;

			//if ( DxEffectMan::GetInstance().IsBorder() )											// Border 기능 사용 가능
			//{
			//	return EM_VBSET_2TEX;
			//}
			//else																			// Border 기능 사용 불가능
			//{
			//	return EM_VBSET_2TEX_COLOR;
			//}
		}
		else																		// 지형그림자를 사용 하지 않을 경우
		{
			return EM_VBSET_1TEX_COLOR;

			//if ( DxEffectMan::GetInstance().IsBorder() )											// Border 기능 사용 가능
			//{
			//	return EM_VBSET_1TEX_C;
			//}
			//else																			// Border 기능 사용 불가능
			//{
			//	return EM_VBSET_1TEX_COLOR;
			//}
		}
	}

	return EM_VBSET_NONE;
}

void DxEffectShadowLow::CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, VBSET emVBSET )
{
	DWORD			dwVertices;
	VERTEX*			pVertices;

	if ( pmsMeshs->m_pLocalMesh )
	{	
		dwVertices		= pmsMeshs->m_pLocalMesh->GetNumVertices ();
		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	}
	else
	{
		dwVertices		= pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pVertices );
	}

	SAFE_RELEASE ( m_pShadowVB );		// 기본적으로 지워준다.

	if ( emVBSET == EM_VBSET_1TEX_S )
	{
		SHADOW_TEX1*	pVert;
		pd3dDevice->CreateVertexBuffer ( dwVertices*sizeof(SHADOW_TEX1), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										SHADOW_TEX1::FVF, D3DPOOL_SYSTEMMEM, &m_pShadowVB, NULL );

		m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, 0 );

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			pVert[i].vPos	= pVertices[i].vPos;
			pVert[i].vTex1	= m_pShadowTex[i];
		}
		m_pShadowVB->Unlock ();
	}
	else if ( emVBSET == EM_VBSET_1TEX_C )
	{
		SHADOW_TEX1*	pVert;
		pd3dDevice->CreateVertexBuffer ( dwVertices*sizeof(SHADOW_TEX1), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										SHADOW_TEX1::FVF, D3DPOOL_SYSTEMMEM, &m_pShadowVB, NULL );

		m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, 0 );

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			pVert[i].vPos	= pVertices[i].vPos;
		}
		m_pShadowVB->Unlock ();
	}
	else if ( emVBSET == EM_VBSET_1TEX_COLOR )
	{
		SHADOW_TEX1_COLOR*	pVert;
		pd3dDevice->CreateVertexBuffer ( dwVertices*sizeof(SHADOW_TEX1_COLOR), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										SHADOW_TEX1_COLOR::FVF, D3DPOOL_SYSTEMMEM, &m_pShadowVB, NULL );

		m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, 0 );

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			pVert[i].vPos	= pVertices[i].vPos;
		}
		m_pShadowVB->Unlock ();
	}
	else if ( emVBSET == EM_VBSET_2TEX )
	{
		SHADOW_TEX2*	pVert;
		pd3dDevice->CreateVertexBuffer ( dwVertices*sizeof(SHADOW_TEX2), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										SHADOW_TEX2::FVF, D3DPOOL_SYSTEMMEM, &m_pShadowVB, NULL );

		m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, 0 );

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			pVert[i].vPos	= pVertices[i].vPos;
			pVert[i].vTex2	= m_pShadowTex[i];
		}
		m_pShadowVB->Unlock ();
	}
	else if ( emVBSET == EM_VBSET_2TEX_COLOR )
	{
		SHADOW_TEX2_COLOR*	pVert;
		pd3dDevice->CreateVertexBuffer ( dwVertices*sizeof(SHADOW_TEX2_COLOR), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										SHADOW_TEX2_COLOR::FVF, D3DPOOL_SYSTEMMEM, &m_pShadowVB, NULL );

		m_pShadowVB->Lock ( 0, 0, (VOID**)&pVert, 0 );

		for ( DWORD i=0; i<dwVertices; i++ )
		{
			pVert[i].vPos	= pVertices[i].vPos;
			pVert[i].vTex2	= m_pShadowTex[i];
		}
		m_pShadowVB->Unlock ();
	}

	if ( pmsMeshs->m_pLocalMesh )		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
	else								pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer();
}

void DxEffectShadowLow::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD) + sizeof(D3DXVECTOR2)*m_dwVertices;


	SFile << m_dwVertices;


	if ( m_dwVertices )
		SFile.WriteBuffer ( m_pShadowTex, sizeof(D3DXVECTOR2)*m_dwVertices );
}

void DxEffectShadowLow::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	// 여기서는 로드 해서는 안된다...... 
	// 왜냐면 X 파일을 변경할 수도 있기 때문에 이곳에는 값을 지정해서는 안된다.
	// < X 파일에 속해있는 효과, X 파일이 변하면 자신도 변해야 하기 때문에 >

	if ( VERSION == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );

		return;
	}
	else if ( (0x105==dwVer) || (0x104==dwVer) )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		return;
	}
	else if ( 0x103 == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else if ( 0x102 == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		DxEffectBase::LoadBufferSet ( SFile, dwVer, pd3dDevice );
		return;
	}
}

void DxEffectShadowLow::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	if ( VERSION == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		SFile >> m_dwVertices;

		SAFE_DELETE_ARRAY ( m_pShadowTex );
		if ( m_dwVertices )
		{
			m_pShadowTex = new D3DXVECTOR2[m_dwVertices];
			SFile.ReadBuffer ( m_pShadowTex, sizeof(D3DXVECTOR2)*m_dwVertices );
		}

		return;
	}
	else if ( (0x105==dwVer) || (0x104==dwVer) )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.
		return;
	}
	else if ( 0x103 == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else if ( 0x102 == dwVer )
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		DxEffectBase::LoadBuffer ( SFile, dwVer, pd3dDevice );
		return;
	}
}

HRESULT DxEffectShadowLow::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	assert(pSrcEffect);
	DxEffectShadowLow *pEffSrc = (DxEffectShadowLow*)pSrcEffect;

	return S_OK;
}