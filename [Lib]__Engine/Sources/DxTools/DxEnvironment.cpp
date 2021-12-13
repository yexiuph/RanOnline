#include "pch.h"

#include "./glperiod.h"

#include "DxFrameMesh.h"
#include "DxLandMan.h"

#include "DxDynamicVB.h"	// 지워야 하는 코드

#include "DxPostProcess.h"

#include "DxSkyMan.h"
#include "DxFogMan.h"

#include "./DxSkinChar.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "DxEnvironment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEnvironment& DxEnvironment::GetInstance()
{
	static DxEnvironment Instance;
	return Instance;
}

LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pSavedBasePosTexSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pDrawBasePosTexSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pSavedWaveSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pDrawWaveSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pSavedDualSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pDrawDualSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pSavedWaveTexSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pDrawWaveTexSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pSavedNightViewSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEnvironment::m_pDrawNightViewSB = NULL;

const	DWORD	DxEnvironment::D3DWAVETEX::FVF = D3DFVF_XYZRHW|D3DFVF_TEX1;
const	DWORD	DxEnvironment::VERTEX::FVF	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
const	DWORD	DxEnvironment::POS_DIFF_TEX1::FVF	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const	DWORD	DxEnvironment::VERTEX_POS_TEX1::FVF	= D3DFVF_XYZ|D3DFVF_TEX1;
DxEnvironment::POS_DIFF_TEX1	DxEnvironment::DxWave::aRefract[REFRACT_VERTEX];
DxEnvironment::VERTEX_POS_TEX1	DxEnvironment::DxWave::aTwist[TWIST_VERTEX];

LPDIRECT3DVERTEXBUFFERQ	DxEnvironment::m_pWaveTexVB = NULL;
LPDIRECT3DINDEXBUFFERQ	DxEnvironment::m_pWaveTexIB = NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEnvironment::m_pTwistVB = NULL;
LPDIRECT3DINDEXBUFFERQ	DxEnvironment::m_pTwistIB = NULL;

DxEnvironment::DxEnvironment() :
	m_bRefractPlay(FALSE),
	m_bReflectPlay(FALSE),
	m_bReflectPlayEX(FALSE),
	m_bFullTwistPlay(FALSE),

	m_bHeightSAME(TRUE),
	m_emHeightStats(EMHS_NODATA),
	m_vCenter(0.f,0.f,0.f),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_bFar(FALSE),

	m_dwTexDetail(512),
	m_vViewMin(0.f,0.f,0.f),
	m_vViewMax(0.f,0.f,0.f),
	m_pTexture(NULL),
	m_pSunSpecularTex(NULL),

	m_fTime(0.f),

	m_bUseTexture(TRUE),
	m_pWaveList(NULL)
{
	D3DXMatrixIdentity ( &m_matReflectView );
	D3DXMatrixIdentity ( &m_matReflection );
	D3DXMatrixIdentity ( &m_matRefraction );

	D3DXMatrixIdentity ( &m_matView );
	D3DXMatrixIdentity ( &m_matProj );
	D3DXMatrixIdentity ( &m_matLastVP );
}

DxEnvironment::~DxEnvironment()
{
	SAFE_DELETE ( m_pWaveList );
}

HRESULT DxEnvironment::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );

	DWORD		dwUseSwShader = D3DUSAGE_SOFTWAREPROCESSING;
	BOOL		bUseBumpMap = TRUE;
	BOOL		bUseMIXED = TRUE;
	D3DFORMAT	BumpMapFormat = D3DFMT_UNKNOWN;

	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		bUseMIXED = FALSE;
	}

	//	Note : Vertex Shader
	//
	if ( bUseMIXED &&	(d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 0)) )
	{
		dwUseSwShader = 0;	// 이럴 땐 MIXED -> H/W 가속 이다.
	}

	//	Note : H&W VS 사용 가능할 때..
	//			그래픽 카드가 지원하는  범프맵 텍스쳐를 찾는다.
	//
	BOOL bCanDoV8U8   = SUCCEEDED( pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal,
								d3dCaps.DeviceType, d3dsdBackBuffer.Format,
								0, D3DRTYPE_TEXTURE,
								D3DFMT_V8U8 ) ) &&
						(d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP );

	BOOL bCanDoL6V5U5 = SUCCEEDED( pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal,
								d3dCaps.DeviceType, d3dsdBackBuffer.Format,
								0, D3DRTYPE_TEXTURE,
								D3DFMT_L6V5U5 ) ) &&
						(d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE );

	BOOL bCanDoL8V8U8 = SUCCEEDED( pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal,
								d3dCaps.DeviceType, d3dsdBackBuffer.Format,
								0, D3DRTYPE_TEXTURE,
								D3DFMT_X8L8V8U8 ) ) &&
						(d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE );

	if( bCanDoV8U8 )        BumpMapFormat = D3DFMT_V8U8;
	else if( bCanDoL6V5U5 ) 
	{
		BumpMapFormat = D3DFMT_L6V5U5;
		bUseBumpMap = FALSE;
	}
	else if( bCanDoL8V8U8 ) 
	{
		BumpMapFormat = D3DFMT_X8L8V8U8;
		bUseBumpMap = FALSE;
	}
	else					bUseBumpMap = FALSE;					// 범프맵 사용 불가

	////	Note : 이때는 반사 텍스쳐를 생성하지 않는다.
	////
	//if ( !m_dwUseSwShader && bUseBumpMap && d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 0) )	m_bUseTexture = TRUE;
	//else	m_bUseTexture = FALSE;

	//TextureManager::LoadTexture ( "Sun_Specular.dds", pd3dDevice, m_pSunSpecularTex, 0, 0 );
	
	return S_OK;
}

HRESULT DxEnvironment::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT	hr = S_OK;

	VERTEX_POS_TEX1*	pVertices;
	SAFE_RELEASE ( m_pTwistVB );
	hr = pd3dDevice->CreateVertexBuffer( TWIST_VERTEX*sizeof(VERTEX_POS_TEX1), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									VERTEX_POS_TEX1::FVF, D3DPOOL_SYSTEMMEM, &m_pTwistVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- CreateVertexBuffer() -- Failed" );
		return hr;
	}

	hr = m_pTwistVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- Lock() -- Failed" );
		return hr;
	}

	for ( DWORD i=0; i<TWIST_VERTEX; i++ )
	{
		pVertices[i].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		pVertices[i].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	}
	m_pTwistVB->Unlock ();

	WORD*	pIndices;
	SAFE_RELEASE ( m_pTwistIB );
	hr = pd3dDevice->CreateIndexBuffer (	3*TWIST_FACES*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pTwistIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- CreateIndexBuffer() -- Failed" );
		return hr;
	}

	hr = m_pTwistIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- Lock() -- Failed" );
		return hr;
	}

	//WORD wFACES[3*TWIST_FACES] =
	//{
	//	0,3,4,
	//	0,4,1,
	//};

	*pIndices++ = (WORD)(0);	// 1
	*pIndices++ = (WORD)(4);
	*pIndices++ = (WORD)(3);

	*pIndices++ = (WORD)(0);	// 2
	*pIndices++ = (WORD)(1);
	*pIndices++ = (WORD)(4);

	*pIndices++ = (WORD)(1);	// 3
	*pIndices++ = (WORD)(5);
	*pIndices++ = (WORD)(4);

	*pIndices++ = (WORD)(1);	// 4
	*pIndices++ = (WORD)(2);
	*pIndices++ = (WORD)(5);

	*pIndices++ = (WORD)(2);	// 5
	*pIndices++ = (WORD)(6);
	*pIndices++ = (WORD)(5);

	*pIndices++ = (WORD)(2);	// 6
	*pIndices++ = (WORD)(7);
	*pIndices++ = (WORD)(6);

	for ( DWORD	i=0; i<2; i++ )
	{
		for ( DWORD	j=0; j<4; j++ )
		{
			*pIndices++ = (WORD)((5*(0+i))+0+j + 3);
			*pIndices++ = (WORD)((5*(0+i))+1+j + 3);
			*pIndices++ = (WORD)((5*(1+i))+0+j + 3);			

			*pIndices++ = (WORD)((5*(1+i))+0+j + 3);
			*pIndices++ = (WORD)((5*(0+i))+1+j + 3);
			*pIndices++ = (WORD)((5*(1+i))+1+j + 3);
		}
	}

	*pIndices++ = (WORD)(13);	// 23
	*pIndices++ = (WORD)(14);
	*pIndices++ = (WORD)(18);

	*pIndices++ = (WORD)(14);	// 24
	*pIndices++ = (WORD)(15);
	*pIndices++ = (WORD)(18);

	*pIndices++ = (WORD)(15);	// 25
	*pIndices++ = (WORD)(19);
	*pIndices++ = (WORD)(18);

	*pIndices++ = (WORD)(15);	// 26
	*pIndices++ = (WORD)(16);
	*pIndices++ = (WORD)(19);

	*pIndices++ = (WORD)(16);	// 27
	*pIndices++ = (WORD)(20);
	*pIndices++ = (WORD)(19);

	*pIndices++ = (WORD)(16);	// 28
	*pIndices++ = (WORD)(17);
	*pIndices++ = (WORD)(20);

	m_pTwistIB->Unlock ();


	hr = pd3dDevice->CreateVertexBuffer ( 25*sizeof(D3DWAVETEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DWAVETEX::FVF, D3DPOOL_SYSTEMMEM, &m_pWaveTexVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- CreateVertexBuffer( m_pWaveTexVB ) -- Failed" );
		return hr;
	}

	hr = pd3dDevice->CreateIndexBuffer ( 3*32*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWaveTexIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- CreateIndexBuffer( m_pWaveTexIB ) -- Failed" );
		return hr;
	}

	hr = m_pWaveTexIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::RestoreDeviceObjects() -- m_pWaveTexIB->Lock() -- Failed" );
		return hr;
	}

	for ( DWORD	i=0; i<4; i++ )
	{
		for ( DWORD	j=0; j<4; j++ )
		{
			*pIndices++ = (WORD)((5*(0+i))+0+j);
			*pIndices++ = (WORD)((5*(0+i))+1+j);
			*pIndices++ = (WORD)((5*(1+i))+0+j);

			*pIndices++ = (WORD)((5*(0+i))+1+j);
			*pIndices++ = (WORD)((5*(1+i))+1+j);
			*pIndices++ = (WORD)((5*(1+i))+0+j);
		}
	}
	m_pWaveTexIB->Unlock ();

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedBasePosTexSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawBasePosTexSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		//pd3dDevice->SetSamplerState( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_CLAMP );
		//pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2);

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedWaveSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawWaveSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedDualSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawDualSB );
	}

	for( UINT which=0; which<2; which++ )			// 어두운것 표현
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
		//pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedWaveTexSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawWaveTexSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedNightViewSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawNightViewSB );
	}

	// 다시 초기화 한다.
	m_bHeightSAME = TRUE;

	return hr;
}

HRESULT DxEnvironment::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedWaveSB );
	SAFE_RELEASE( m_pDrawWaveSB );
	SAFE_RELEASE( m_pSavedDualSB );
	SAFE_RELEASE( m_pDrawDualSB );
	SAFE_RELEASE( m_pSavedBasePosTexSB );
	SAFE_RELEASE( m_pDrawBasePosTexSB );
	SAFE_RELEASE( m_pSavedWaveTexSB );
	SAFE_RELEASE( m_pDrawWaveTexSB );
	SAFE_RELEASE( m_pSavedNightViewSB );
	SAFE_RELEASE( m_pDrawNightViewSB );


	SAFE_RELEASE ( m_pWaveTexVB );
	SAFE_RELEASE ( m_pWaveTexIB );

	SAFE_RELEASE ( m_pTwistVB );
	SAFE_RELEASE ( m_pTwistIB );

	return S_OK;
}

HRESULT DxEnvironment::DeleteDeviceObjects()
{
	TextureManager::ReleaseTexture ( "Sun_Specular.dds", m_pSunSpecularTex );

	return S_OK;
}

HRESULT DxEnvironment::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	return S_OK;
}

HRESULT	DxEnvironment::SetReflection ( D3DXVECTOR3 vCenter, BOOL bHeight, BOOL bFar, D3DXVECTOR3 vMax, D3DXVECTOR3 vMin )
{
	HRESULT	hr = S_OK;

	m_vMax		= vMax;
	m_vMin		= vMin;

	m_bReflectPlay = TRUE;

	if ( m_emHeightStats == EMHS_NODATA )
	{
		m_emHeightStats	= EMHS_INIT;
		m_vCenter		= vCenter;
	}
	else if ( m_emHeightStats == EMHS_INIT )
	{
		if ( ((m_vCenter.y-0.1f)<vCenter.y) && ((m_vCenter.y+0.1f)>vCenter.y) )
		{
		//	m_emUseReflect	= EMUR_RECENT;
		}
		else
		{
			m_bHeightSAME	= FALSE;
			m_emHeightStats	= EMHS_END;
			m_vCenter		= vCenter;
		}
	}

	m_bFar = bFar;

	return hr;
}

HRESULT DxEnvironment::Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	if ( m_bReflectPlay && DxSurfaceTex::GetInstance().m_pReflectTex )
	{
		if ( m_bHeightSAME )
		{
			// Note : Clip Plane
			D3DXPLANE sPlane;
			sPlane.a = 0.f;
			sPlane.b = -1.f;
			sPlane.c = 0.f;
			sPlane.d = m_vCenter.y;
			pd3dDevice->SetClipPlane( 0, sPlane );
			pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	D3DCLIPPLANE0 );

			// Note : Reflect Render
			RenderReflectionEX ( pd3dDevice, pFrameMesh, pLandMan );

			// Note : Clip Plane
			pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	0 );
		}
		else
		{
			RenderReflection ( pd3dDevice, pFrameMesh, pLandMan );
		}
	}

	//m_bReflectPlay = FALSE;		// 캐릭터를 비추면서 지웠다.
	//m_bReflectPlayEX = FALSE;	// 캐릭터를 비추면서 지웠다.

	//m_emDrawReflect = m_emUseReflect;	// 이전 데이터 백업

	//m_emUseReflect = EMUR_RECENT;	// 높이 변화가 있는지 알기 위한 초기화
	//m_emHeightStats = EMHS_NODATA;	// 높이 변화가 있는지 알기 위한 초기화

	return S_OK;
}

HRESULT DxEnvironment::RenderReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT			hr = S_OK;

	PROFILE_BEGIN("DxEffectReflect");

	CLIPVOLUME* pCV;
	D3DXVECTOR3	vPos;
	D3DXVECTOR3	vNearPlane;
	float		fNearLength;

	D3DXPLANE	vPlane;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	D3DXMATRIX		matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

	D3DXMATRIX&		matChangeView = DxViewPort::GetInstance().GetMatView();
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempFromPt		= vFromPt;
	D3DXVECTOR3		vTempLookatPt	= vLookatPt;
	D3DXMATRIX		matBackUpView = matChangeView;
	vLookatPt.y -= 10.f;						// 시점을 좀더 낮춘다.

	D3DXVECTOR3	vDirect;
	vDirect = vFromPt - vLookatPt;
	vDirect.y = -vDirect.y;

	float fLength = D3DXVec3Length ( &vDirect );
	D3DXVec3Normalize ( &vDirect, &vDirect );

	fLength = 200.f + fLength*0.5f;

	vFromPt = vLookatPt + vDirect*fLength;

	D3DXMatrixLookAtLH ( &m_matReflectView, &vFromPt, &vLookatPt, &vUpVec );

	DWORD		dwColor;
	D3DCOLOR	cColor_Half;
	D3DCOLOR	cColor = DxFogMan::GetInstance().GetFogDarkColor();

	cColor_Half = 0xff000000;
	dwColor = (cColor&0xff0000)>>16;
	cColor_Half += ((DWORD)(dwColor*0.7f))<<16;
	dwColor = (cColor&0xff00)>>8;
	cColor_Half += ((DWORD)(dwColor*0.7f))<<8;
	dwColor = cColor&0xff;
	cColor_Half += (DWORD)(dwColor*0.7f);

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pReflectSuf );
	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pReflectZBuf );
	hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, cColor_Half, 1.0f, 0L);
	if(FAILED(hr))
	{
		hr = E_FAIL;
		goto _RETURN;
	}

	if ( m_vMax.x != m_vMin.x )
	{
		vPos.x	= m_vMin.x;	vPos.y = m_vMax.y;	vPos.z = m_vMax.z;
	}
	else if ( m_vMax.y != m_vMin.y )
	{
		vPos.x	= m_vMax.x;	vPos.y = m_vMin.y;	vPos.z = m_vMax.z;
	}
	else
	{
		vPos.x	= m_vMax.x;	vPos.y = m_vMax.y;	vPos.z = m_vMin.z;
	}
	D3DXPlaneFromPoints ( &vPlane, &m_vMax, &m_vMin, &vPos );

	fNearLength = D3DXPlaneDotCoord ( &vPlane, &vFromPt );
	fNearLength = ( fNearLength > 0.f ) ? fNearLength : -fNearLength;

	pCV = &DxViewPort::GetInstance().ComputeClipVolume ( vLookatPt, vFromPt, vUpVec, fNearLength, m_bFar );

	pd3dDevice->SetTransform ( D3DTS_VIEW,			&m_matReflectView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&DxViewPort::GetInstance().GetMatProj() );
	DxViewPort::GetInstance().SetReflectProjection ( fNearLength );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&m_matReflectProj );
	D3DXMatrixMultiply ( &m_matLastVP, &m_matReflectView,	&m_matReflectProj );

	DWORD	dwAmbient, dwFogEnable;
	pd3dDevice->GetRenderState ( D3DRS_AMBIENT, &dwAmbient );
	pd3dDevice->SetRenderState ( D3DRS_AMBIENT, 0x00000000 );

	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	// Note : 데이터를 바꿈.
	matChangeView = m_matReflectView;

	//	Note : 하늘 렌더링..!!
	DxSkyMan::GetInstance().Render ( pd3dDevice, TRUE );

	pd3dDevice->SetTransform( D3DTS_WORLD,	&matIdentity );
	if ( pLandMan )		pLandMan->Render_ReflectOLD( pd3dDevice, *pCV );
	else				pFrameMesh->Render_NOEFF ( pd3dDevice, pCV );

	// Note : 반사맵을 만드는중 맵에서 나온 효과를 여기서 뿌린다.
	OPTMManager::GetInstance().Render( pd3dDevice );

	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, dwFogEnable );

	pd3dDevice->SetRenderState ( D3DRS_AMBIENT, dwAmbient );

	sBackupTarget.RestoreTarget ( pd3dDevice );

	//m_bReflectPlay = FALSE;			// 마지막에 꺼준다.	// 캐릭터를 비추면서 지웠다.

_RETURN:
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	// Note : DxViewPort::GetInstance().GetFromPt()로 얻은 것을 건들었기 때문에 값을 되돌려 준다.
	vLookatPt = vTempLookatPt;
	vFromPt = vTempFromPt;
	matChangeView = matBackUpView;

	PROFILE_END("DxEffectReflect");

	return hr;
}

HRESULT DxEnvironment::RenderReflectionEX ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT			hr = S_OK;

	PROFILE_BEGIN("DxEffectReflectEX");

	DWORD		dwColor;
	D3DCOLOR	cColor_Half;
	D3DCOLOR	cColor = DxFogMan::GetInstance().GetFogColor();

	cColor_Half = 0xff000000;
	dwColor = (cColor&0xff0000)>>16;
	cColor_Half += ((DWORD)(dwColor*0.7f))<<16;
	dwColor = (cColor&0xff00)>>8;
	cColor_Half += ((DWORD)(dwColor*0.7f))<<8;
	dwColor = cColor&0xff;
	cColor_Half += (DWORD)(dwColor*0.7f);

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pReflectSuf );
	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pReflectZBuf );
	hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, cColor_Half, 1.0f, 0L);
	if(FAILED(hr))			return hr;

	//CLIPVOLUME pCV = DxViewPort::GetInstance().GetClipVolume();

	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	float			fNearLength = 10.f;
	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempFromPt		= vFromPt;
	D3DXVECTOR3		vTempLookatPt	= vLookatPt;

	vTempFromPt.y = (2.f*m_vCenter.y) - vTempFromPt.y;
	vTempLookatPt.y = (2.f*m_vCenter.y) - vTempLookatPt.y;

	CLIPVOLUME* pCV;
	pCV = &DxViewPort::GetInstance().ComputeClipVolume ( vTempLookatPt, vTempFromPt, vUpVec, fNearLength, m_bFar );



	DWORD	dwAmbient, dwFogEnable;
	pd3dDevice->GetRenderState ( D3DRS_FOGENABLE, &dwFogEnable );
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	pd3dDevice->GetRenderState ( D3DRS_AMBIENT, &dwAmbient );
	pd3dDevice->SetRenderState ( D3DRS_AMBIENT, 0x00000000 );		

	////	Note : 태양및 야간불빛을 반짝이도록 하기 위해서 작업 시작
	////
	//NightViewRender ( pd3dDevice );

	D3DLIGHTQ sLight, sLightOrigin;
	pd3dDevice->GetLight ( 0, &sLightOrigin );

	sLight = sLightOrigin;
	sLight.Direction.y = -sLightOrigin.Direction.y;
	pd3dDevice->SetLight ( 0, &sLight );

	//	Note : 하늘 렌더링..!!
	//
	DxSkyMan::GetInstance().Render_Reflect ( pd3dDevice );

	//	Note : 지형 렌더
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD,	&matIdentity );
	if ( pLandMan )		pLandMan->Render_Reflect ( pd3dDevice, *pCV );
	else				pFrameMesh->Render_Reflect ( pd3dDevice, pCV );

	pd3dDevice->SetLight ( 0, &sLightOrigin );

	// Note : 반사맵을 만드는중 맵에서 나온 효과를 여기서 뿌린다.
	OPTMManager::GetInstance().Render( pd3dDevice );

	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, dwFogEnable );

	pd3dDevice->SetRenderState ( D3DRS_AMBIENT, dwAmbient );

	sBackupTarget.RestoreTarget ( pd3dDevice );

	//m_bReflectPlay = FALSE;			// 마지막에 꺼준다.	// 캐릭터를 비추면서 지웠다.

	PROFILE_END("DxEffectReflectEX");

	return hr;
}

void DxEnvironment::NightViewRender ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3	vLeft;
	D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3& vDir = DxViewPort::GetInstance().GetLookDir_Y0();
	D3DXVECTOR3 vUP = D3DXVECTOR3 ( 0.f, 1.f, 0.f );

	// Cross 선 만들기
	D3DXVec3Cross ( &vLeft, &vDir, &vUP );
	D3DXVec3Normalize ( &vLeft, &vLeft );

	// 평면 만들기
	D3DXVECTOR3 vTemp_1 = vFromPt + vUP;
	D3DXVECTOR3 vTemp_2 = vFromPt + vLeft;
	D3DXPLANE sPlane;
	D3DXPlaneFromPoints ( &sPlane, &vFromPt, &vTemp_1, &vTemp_2 );

	//	//	// 조명의 위치를 구해서 사각형의 빛을 만든다.	// 현재 테스트

	// 태양위치
	D3DXVECTOR3 vSunPos = DxSkyMan::GetInstance().GetSunPos();
	vSunPos.y = 0.f;

	//// 평면과 직선의 교점을 검색한다.
	//vTemp_1 = vSunPos + (vDir*1000000.f);
	//vTemp_2 = vSunPos - (vDir*1000000.f);;
	D3DXVECTOR3 vIntersect;
	//D3DXPlaneIntersectLine ( &vIntersect, &sPlane, &vTemp_1, &vTemp_2 );
	vIntersect.x = vFromPt.x;
	vIntersect.y = 0.f;
	vIntersect.z = vFromPt.z;

	// 사각형 설정
	VERTEX_POS_TEX1 sVertices[4];

	sVertices[0].vPos = vSunPos + (vLeft*15.f);
	sVertices[1].vPos = vSunPos - (vLeft*15.f);
	sVertices[2].vPos = vIntersect + (vLeft*15.f);
	sVertices[3].vPos = vIntersect - (vLeft*15.f);

	sVertices[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	sVertices[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
	sVertices[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	sVertices[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );

	// Draw
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	pd3dDevice->SetFVF ( VERTEX_POS_TEX1::FVF );

	m_pSavedNightViewSB->Capture();
	m_pDrawNightViewSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pSunSpecularTex );
	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sVertices, sizeof(VERTEX_POS_TEX1) );

	m_pSavedNightViewSB->Apply();
}

//	Note :	지형 다음에 뿌려야 한다...
//			반사를 그만 뿌린다는 것을 설정해야한다.
//
HRESULT DxEnvironment::RenderRefelctChar ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().m_pReflectTex || !m_bReflectPlay )
	{
		m_bReflectPlay = FALSE;
		return S_OK;
	}

	HRESULT			hr = S_OK;

	PROFILE_BEGIN("DxEffectReflectChar");

	if ( m_bHeightSAME )
	{
		// Note : Clip Plane
		D3DXPLANE sPlane;
		sPlane.a = 0.f;
		sPlane.b = -1.f;
		sPlane.c = 0.f;
		sPlane.d = m_vCenter.y;
		pd3dDevice->SetClipPlane( 0, sPlane );
		pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	D3DCLIPPLANE0 );

		// Note : Char Render
		DxBackUpRendTarget sBackupTarget ( pd3dDevice );
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pReflectSuf );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pReflectZBuf );

		D3DXMATRIX	matWorld = matRot;
		matWorld._42 = matWorld._42 - (m_vCenter.y*2.f);

		D3DXMatrixMultiply_MIRROR( matWorld );

		DWORD dwCullMode(0L);
		pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
		
		pSkinObj->RenderReflect( pd3dDevice, matWorld );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );

		sBackupTarget.RestoreTarget ( pd3dDevice );

		// Note : Clip Plane
		pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	0 );
	}
	else
	{
		D3DXMATRIX		matView, matProj;
		pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );	

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&m_matReflectView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matReflectProj );

		DxBackUpRendTarget sBackupTarget ( pd3dDevice );
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pReflectSuf );
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pReflectZBuf );

		pSkinObj->RenderReflect( pd3dDevice, matRot );

		sBackupTarget.RestoreTarget ( pd3dDevice );

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );	
	}
	

	// ㅜㅜ 뭐냐.

	

	PROFILE_END("DxEffectReflectChar");

	m_bReflectPlay = FALSE;

	m_emHeightStats = EMHS_NODATA;	// 높이 변화가 있는지 알기 위한 초기화

	return hr;
}

void	DxEnvironment::UserToRefraction ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bRefractPlay )							return;
	if( !DxSurfaceTex::GetInstance().m_pWaveTex )	return; 

	if( !DxSurfaceTex::GetInstance().IsEnable() && 
		!DxPostProcess::GetInstance().IsEnable() )
	{
		return;
	}

	//static int nCount = 0;

	D3DSURFACE_DESC	sDesc;
	LPDIRECT3DSURFACEQ pBackBuffer;
	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc ( &sDesc );
	pBackBuffer->Release();

	HRESULT hr = pd3dDevice->StretchRect( pBackBuffer, NULL, DxSurfaceTex::GetInstance().m_pWaveSuf, NULL, D3DTEXF_POINT );

	//RECT		sRect;
	//sRect.left	= 0;
	//sRect.right	= sDesc.Width - 1;

	//float fHeight	= sDesc.Height/4.f;
	//int nTemp = (int)fHeight+1;				// <-- 음... 1/4 
	//int nTemp2 = nTemp*nCount;	// 위치 확인

	//sRect.top		= nTemp2;	// 삽입
	//sRect.bottom	= nTemp2+nTemp;

	//int nData = sRect.bottom - (sDesc.Height-1);

	//if( nCount==3 )
	//{
	//	sRect.top		-= nData;
	//	sRect.bottom	-= nData;
	//}

	//RECT	sRect2;
	//sRect2.left = 1;
	//sRect2.right = 256;
	//sRect2.top = nCount*64;
	//sRect2.bottom = sRect2.top + 64;

	//HRESULT hr = pd3dDevice->StretchRect( pBackBuffer, &sRect, DxSurfaceTex::GetInstance().m_pWaveSuf, &sRect2, D3DTEXF_POINT );

	//++nCount;
	//if ( nCount > 3 )	nCount = 0;
}

void	DxEnvironment::SetWave ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBiil, DWORD dwType, D3DXVECTOR3& vPos, float fSize, float fAlpha, float fBumpMat, float fLifeTime, float fSpeed )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	DxWave* pCur = new DxWave;
	pCur->pNext = m_pWaveList;
	m_pWaveList = pCur;

	pCur->bBiil = bBiil;
	pCur->dwType = dwType;
	pCur->vPos = vPos;
	pCur->fSize = fSize;
	pCur->fAlpha = fAlpha;
	pCur->fBumpMat = fBumpMat;
	pCur->fLifeTime = fLifeTime;
	pCur->fSpeed = fSpeed;

	m_bRefractPlay = TRUE;
}

void	DxEnvironment::SetWave ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwType, LPDIRECT3DTEXTUREQ	pTex )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	DxWave* pCur = new DxWave;
	pCur->pNext = m_pWaveList;
	m_pWaveList = pCur;

	pCur->dwType = dwType;
	pCur->pTexture = pTex;

	m_bRefractPlay = TRUE;
}

void	DxEnvironment::SetWaveWater ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matWorld, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwVertics, DWORD dwFaces, float fAlpha )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	DxWave* pCur = new DxWave;
	pCur->pNext = m_pWaveList;
	m_pWaveList = pCur;

	pCur->matWorld = matWorld;
	pCur->pVBuffer = pVB;
	pCur->pIBuffer = pIB;
	pCur->dwVertics = dwVertics;
	pCur->dwFaces = dwFaces;
	pCur->fAlpha = fAlpha;

	m_bRefractPlay = TRUE;
}

void	DxEnvironment::SetWaveBlur ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVertics, DWORD dwFaces )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	DxWave* pCur = new DxWave;
	pCur->pNext = m_pWaveList;
	m_pWaveList = pCur;

	pCur->dwType = 6;

	pCur->pVBuffer = pVB;
	pCur->dwVertics = dwVertics;
	pCur->dwFaces = dwFaces;

	m_bFullTwistPlay = TRUE;		// 전체 텍스쳐를 움직여야 할 경우 TRUE 를 준다.
}

HRESULT	DxEnvironment::RenderWave ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pBumpTex )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return S_OK;

	HRESULT hr = S_OK;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	DxWave* pCur = m_pWaveList;
	
	while ( pCur )
	{
		if ( !pCur->dwType )			pCur->RenderVB( pd3dDevice, 
														m_fTime, 
														m_vViewMax, 
														m_vViewMin, 
														pBumpTex, 
														DxSurfaceTex::GetInstance().m_pWaveTex );		// 변형된 굴절을 사용하기 위해서
		//if ( !pCur->dwType )			pCur->RenderVB ( pd3dDevice, m_fTime, m_vViewMax, m_vViewMin, pBumpTex, m_pLightTexture );
		else if ( pCur->dwType == 5 )	pCur->Render ( pd3dDevice );
		else							pCur->Render( pd3dDevice, 
														m_fTime, 
														DxSurfaceTex::GetInstance().m_pWaveTex, 
														DxSurfaceTex::GetInstance().m_pTempSuf256, 
														DxSurfaceTex::GetInstance().m_pTempTex256 );

		pCur = pCur->pNext;
	}

	//	Note : Blur를 써야 하는 것이 있을 경우
	//
	if ( m_bFullTwistPlay )
	{
		DxUpdateWaveTex ( pd3dDevice, DxSurfaceTex::GetInstance().m_pWaveTex, DxSurfaceTex::GetInstance().m_pTempSuf256 );	// TEX 변형 시킨다.

		pCur = m_pWaveList;
		while ( pCur )
		{
			if ( pCur->dwType == 6 )	pCur->RenderBlur ( pd3dDevice, DxSurfaceTex::GetInstance().m_pTempTex256 );

			pCur = pCur->pNext;	
		}
	}

	SAFE_DELETE ( m_pWaveList );

	m_bRefractPlay = FALSE;
	m_bFullTwistPlay = FALSE;

	return hr;
}

HRESULT	DxEnvironment::DxWave::RenderVB ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, D3DXVECTOR3 vViewMax, D3DXVECTOR3 vViewMin, LPDIRECT3DTEXTUREQ pBumpTex, LPDIRECT3DTEXTUREQ pUserTex )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3	vLocal;
	D3DXMATRIX	matVP;
	D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView ();
	D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj ();
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	VERTEX*	pVertices;
	hr = pVBuffer->Lock( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::DxWave::RenderVB() -- Lock() -- Failed" );
		return hr;
	}

	for ( DWORD i=0; i<dwVertics; i++ )
	{
		pVertices[i].cDiffuse	= ((DWORD)(0xff*fAlpha))<<24;
	}

	pVBuffer->Unlock ();


	D3DXMATRIX	matTemp, matIdentity;

	D3DXMatrixIdentity ( &matIdentity );

	matTemp		= matWorld;
	matTemp._42	+= 0.2f;
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matTemp );

	pd3dDevice->SetTexture( 0, pBumpTex );
	pd3dDevice->SetTexture( 1, pUserTex );

	//	Note : 굴절
	//
	m_pSavedWaveSB->Capture();
	m_pDrawWaveSB->Apply();

	DxSetTextureMatrix ( pd3dDevice, 1, matVP );

	float fBumpMat_1000 = fBumpMat/1000.f;
	fBumpMat_1000 = 50.f/1000.f;
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT10, FtoDW(fBumpMat_1000) );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT11, FtoDW(fBumpMat_1000) );

	pd3dDevice->SetFVF ( VERTEX::FVF );
	pd3dDevice->SetIndices ( pIBuffer );
	pd3dDevice->SetStreamSource ( 0, pVBuffer, 0, sizeof(VERTEX) );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVertics, 0, dwFaces );

	DxResetTextureMatrix ( pd3dDevice, 1 );

	m_pSavedWaveSB->Apply();

	pd3dDevice->SetTexture( 1, NULL );

	return hr;
}

HRESULT	DxEnvironment::DxWave::RenderBlur ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pTwistTex )
{
	HRESULT hr = S_OK;

	if ( !dwFaces )			return	S_OK;

	D3DXVECTOR3	vLocal;
	D3DXMATRIX	matVP;
	D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView ();
	D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj ();
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	pd3dDevice->SetTexture( 0, pTwistTex );

	m_pSavedWaveSB->Capture();
	m_pDrawWaveSB->Apply();

	DWORD	dwCullMode;
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );

	DxSetTextureMatrix ( pd3dDevice, 0, matVP );

	pd3dDevice->SetFVF ( POS_DIFF_TEX1::FVF );
	pd3dDevice->SetStreamSource ( 0, pVBuffer, 0, sizeof(POS_DIFF_TEX1) );

	pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, dwVertics, dwFaces );

	DxResetTextureMatrix ( pd3dDevice, 0 );

	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );

	m_pSavedWaveSB->Apply();

	return hr;
}

void DxEnvironment::DxUpdateWaveTex ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface )
{
	HRESULT hr(S_OK);

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );


	pd3dDevice->SetRenderTarget ( 0, pTwistSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	D3DWAVETEX*	pVertices;

	int		nCount;
	float	fRateX;
	float	fRateY;
	DWORD	dwWidth	= 256;
	DWORD	dwHeight = 256;

	hr = m_pWaveTexVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::DxUpdateWaveTex() -- Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<5; i++ )
	{
		for ( DWORD j=0; j<5; j++ )
		{
			nCount = (i*5)+j;
			fRateX = (float)j/4;
			fRateY = (float)i/4;
			pVertices[nCount].vPos = D3DXVECTOR4 ( (float)(dwWidth*fRateX), (float)(dwHeight*fRateY), 1.f, 1.f );
			pVertices[nCount].vTex = D3DXVECTOR2 ( fRateX+sinf(m_fTime+i)*0.02f, fRateY+sinf(m_fTime+j)*0.02f );
		}
	}

	m_pWaveTexVB->Unlock ();

	pd3dDevice->SetTexture ( 0, pUserTex );

	m_pSavedWaveTexSB->Capture();
	m_pDrawWaveTexSB->Apply();

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DWAVETEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pWaveTexVB, 0, sizeof(D3DWAVETEX) );
		pd3dDevice->SetIndices ( m_pWaveTexIB );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, 25, 0, 32 );
	}

	m_pSavedWaveTexSB->Apply();

	pd3dDevice->SetRenderTarget( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void	DxEnvironment::DxWave::DxInitTwistBillboard ()
{
	int		nCount;
	float	fX, fZ;
	float	fFive = 0.5f*fSize;

	D3DXVECTOR3		vLocal;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXMATRIX		matLocal	= DxBillboardLookAt ( &vPos, DxViewPort::GetInstance().GetMatView() );

	vLocal = D3DXVECTOR3 ( -fFive,	fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[0].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( 0.f,		fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[1].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fFive,	fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[2].vPos, &vLocal, &matLocal );

	for ( DWORD i=0; i<3; i++ )
	{
		if ( i == 0 )		fZ = fFive;
		else if ( i == 1 )	fZ = 0.f;
		else if ( i == 2 )	fZ = -fFive;

		for ( DWORD j=0; j<5; j++ )
		{
			nCount = (i*5)+j + 3;

			if ( j == 0 )		fX = -fSize;
			else if ( j == 1 )	fX = -fFive;
			else if ( j == 2 )	fX = 0.f;
			else if ( j == 3 )	fX = fFive;
			else if ( j == 4 )	fX = fSize;

			vLocal = D3DXVECTOR3 ( fX,	fZ,	0.f );
			D3DXVec3TransformCoord ( &aTwist[nCount].vPos, &vLocal, &matLocal );
		}
	}

	vLocal = D3DXVECTOR3 ( -fFive,	-fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[18].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( 0.f,		-fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[19].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fFive,	-fSize,	0.f );
	D3DXVec3TransformCoord ( &aTwist[20].vPos, &vLocal, &matLocal );
}

void	DxEnvironment::DxWave::DxInitTwistGround ()
{
	int		nCount;
	float	fX, fZ;
	float	fFive = 0.5f*fSize;

	aTwist[0].vPos = D3DXVECTOR3 ( vPos.x - fFive,	vPos.y,	vPos.z + fSize );
	aTwist[1].vPos = D3DXVECTOR3 ( vPos.x,			vPos.y, vPos.z + fSize );
	aTwist[2].vPos = D3DXVECTOR3 ( vPos.x + fFive,	vPos.y, vPos.z + fSize );

	for ( DWORD i=0; i<3; i++ )
	{
		if ( i == 0 )		fZ = fFive;
		else if ( i == 1 )	fZ = 0.f;
		else if ( i == 2 )	fZ = -fFive;

		for ( DWORD j=0; j<5; j++ )
		{
			nCount = (i*5)+j + 3;

			if ( j == 0 )		fX = -fSize;
			else if ( j == 1 )	fX = -fFive;
			else if ( j == 2 )	fX = 0.f;
			else if ( j == 3 )	fX = fFive;
			else if ( j == 4 )	fX = fSize;

			aTwist[nCount].vPos = D3DXVECTOR3 ( vPos.x + fX,	vPos.y, vPos.z + fZ );
		}
	}

	aTwist[18].vPos = D3DXVECTOR3 ( vPos.x - fFive,	vPos.y,	vPos.z - fSize );
	aTwist[19].vPos = D3DXVECTOR3 ( vPos.x,			vPos.y, vPos.z - fSize );
	aTwist[20].vPos = D3DXVECTOR3 ( vPos.x + fFive,	vPos.y, vPos.z - fSize );
}

void	DxEnvironment::DxWave::DxTextureTwist ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface )
{
	HRESULT hr(S_OK);

	D3DXVECTOR3	vDistance;
	float		fDistance;
	float		fTimeAddOne = fLifeTime+1.f;
	float		fTimeReverse = 1.f-fLifeTime;
	float fOne		= 0.1f*fSize;
	float fThree	= 0.333f*fSize;
	float fSix		= 0.666f*fSize;
	float fScale	= fOne*fBumpMat*0.1f;	// 0.1f 는 fBumpMat의 값을 높여주기 위해서 이다.


	if ( bBiil )	DxInitTwistBillboard ();
	else			DxInitTwistGround ();
	
	if ( dwType == 1 )			//	No.1	랜덤
	{
		for ( DWORD i=0; i<TWIST_VERTEX; i++ )
		{
			aTwist[i].vPos += D3DXVECTOR3 ( (RANDOM_POS-0.5f)*fScale, (RANDOM_POS-0.5f)*fScale, (RANDOM_POS-0.5f)*fScale );
		}
	}
	else if ( dwType == 2 )			//	No.2	회전을 한다.
	{
		if ( bBiil )
		{
			D3DXVECTOR3		vLocalX, vLocalY;
			D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
			D3DXMATRIX		matLocal	= DxBillboardLookAt ( &vPos, DxViewPort::GetInstance().GetMatView() );

			vLocalX = D3DXVECTOR3 ( 1.f,	0.f,	0.f );
			D3DXVec3TransformCoord ( &vLocalX, &vLocalX, &matLocal );
			vLocalY = D3DXVECTOR3 ( 0.f,	1.f,	0.f );
			D3DXVec3TransformCoord ( &vLocalY, &vLocalY, &matLocal );

			for ( DWORD i=0; i<TWIST_VERTEX; i++ )
			{
				aTwist[i].vPos += sinf(fTime*fSpeed+i)*vLocalX*fScale;
				aTwist[i].vPos += cosf(fTime*fSpeed+i)*vLocalY*fScale;
			}
		}
		else
		{
			for ( DWORD i=0; i<TWIST_VERTEX; i++ )
			{
				aTwist[i].vPos.x += sinf(fTime*fSpeed+i)*fScale;
				aTwist[i].vPos.y += cosf(fTime*fSpeed+i)*fScale;
			}
		}
	}
	else if ( dwType == 3 )	//	No.3	퍼지다.
	{
		for ( DWORD i=0; i<TWIST_VERTEX; i++ )
		{
			vDistance = aTwist[i].vPos - vPos;			// 벡터
			fDistance = D3DXVec3Length ( &vDistance );	// 거리
			D3DXVec3Normalize ( &vDistance, &vDistance );

			aTwist[i].vPos = vPos + vDistance*fDistance*fTimeReverse;
		}
	}
	else if ( dwType == 4 )	//	No.4	모이다.
	{
		for ( DWORD i=0; i<TWIST_VERTEX; i++ )
		{
			vDistance = aTwist[i].vPos - vPos;			// 벡터
			fDistance = D3DXVec3Length ( &vDistance );	// 거리
			D3DXVec3Normalize ( &vDistance, &vDistance );

			aTwist[i].vPos = vPos + vDistance*fDistance*fTimeAddOne;
		}
	}

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, pTwistSurface );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	D3DXMATRIX	matTex, matVP;
	D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView ();
	D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj ();
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	//set special texture matrix for shadow mapping
	unsigned int range;
	float fOffsetX = 0.5f + (0.5f / (float)256);
	float fOffsetY = 0.5f + (0.5f / (float)256);

	range = 0xFFFFFFFF >> (32 - 16);//m_bitDepth)

	float fBias    = -0.001f * (float)range;
	D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
								0.0f,     -0.5f,     0.0f,         0.0f,
								0.0f,     0.0f,     (float)range, 0.0f,
								fOffsetX, fOffsetY, fBias,        1.0f );

	D3DXMatrixMultiply( &matTex, &matVP, &texScaleBiasMat );

	D3DXVECTOR3	vPos;

	for ( DWORD i=0; i<TWIST_VERTEX; i++ )
	{
		vPos.x = matTex._11*aTwist[i].vPos.x + matTex._21*aTwist[i].vPos.y + matTex._31*aTwist[i].vPos.z + matTex._41;
		vPos.y = matTex._12*aTwist[i].vPos.x + matTex._22*aTwist[i].vPos.y + matTex._32*aTwist[i].vPos.z + matTex._42;
		vPos.z = matTex._14*aTwist[i].vPos.x + matTex._24*aTwist[i].vPos.y + matTex._34*aTwist[i].vPos.z + matTex._44;

		aTwist[i].vTex = D3DXVECTOR2 ( vPos.x/vPos.z, vPos.y/vPos.z );
	}

	//	Note : 다시 초기화로 마무리
	//
	if ( bBiil )	DxInitTwistBillboard ();
	else			DxInitTwistGround ();

	//	Note : 마지막 완료 값을 넣는다.
	//
	VERTEX_POS_TEX1*	pVertices;
	hr = m_pTwistVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEnvironment::DxWave::DxTextureTwist() -- Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<TWIST_VERTEX; i++ )
	{
		pVertices[i].vPos = aTwist[i].vPos;
		pVertices[i].vTex = aTwist[i].vTex;
	}
	m_pTwistVB->Unlock ();

	m_pSavedBasePosTexSB->Capture();
	m_pDrawBasePosTexSB->Apply();

	pd3dDevice->SetFVF ( VERTEX_POS_TEX1::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pTwistVB, 0, sizeof(VERTEX_POS_TEX1) );
	pd3dDevice->SetIndices ( m_pTwistIB );

	pd3dDevice->SetTexture ( 0, pUserTex );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, TWIST_VERTEX, 0, TWIST_FACES );

	m_pSavedBasePosTexSB->Apply();

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

HRESULT	DxEnvironment::DxWave::Render ( LPDIRECT3DDEVICEQ pd3dDevice, float fTime, LPDIRECT3DTEXTUREQ pUserTex, LPDIRECT3DSURFACEQ pTwistSurface, LPDIRECT3DTEXTUREQ pTwistTex )
{
	HRESULT hr = S_OK;

	DxTextureTwist ( pd3dDevice, fTime, pUserTex, pTwistSurface );

	if ( bBiil )			// 빌보드 함
	{
		D3DXVECTOR3		vLocal;
		D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
		D3DXMATRIX		matLocal	= DxBillboardLookAt ( &vPos, DxViewPort::GetInstance().GetMatView() );

		vLocal = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		D3DXVec3TransformCoord ( &aRefract[0].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( 0.f,				1.f*fSize,		0.f );
		D3DXVec3TransformCoord ( &aRefract[1].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( 0.86f*fSize,		0.5f*fSize,		0.f );
		D3DXVec3TransformCoord ( &aRefract[2].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( 0.86f*fSize,		-0.5f*fSize,	0.f );
		D3DXVec3TransformCoord ( &aRefract[3].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( 0.f,				-1.f*fSize,		0.f );
		D3DXVec3TransformCoord ( &aRefract[4].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( -0.86f*fSize,	-0.5f*fSize,	0.f );
		D3DXVec3TransformCoord ( &aRefract[5].vPos, &vLocal, &matLocal );

		vLocal = D3DXVECTOR3 ( -0.86f*fSize,	0.5f*fSize,		0.f );
		D3DXVec3TransformCoord ( &aRefract[6].vPos, &vLocal, &matLocal );

		aRefract[7].vPos = aRefract[1].vPos;
	}
	else			// 바닥에 표현하기
	{
		aRefract[0].vPos = vPos;
		aRefract[1].vPos = D3DXVECTOR3 ( vPos.x,				vPos.y, vPos.z - 1.f*fSize );
		aRefract[2].vPos = D3DXVECTOR3 ( vPos.x - 0.86f*fSize,	vPos.y, vPos.z - 0.5f*fSize );
		aRefract[3].vPos = D3DXVECTOR3 ( vPos.x - 0.86f*fSize,	vPos.y,	vPos.z + 0.5f*fSize );
		aRefract[4].vPos = D3DXVECTOR3 ( vPos.x,				vPos.y, vPos.z + 1.f*fSize );
		aRefract[5].vPos = D3DXVECTOR3 ( vPos.x + 0.86f*fSize,	vPos.y,	vPos.z + 0.5f*fSize );
		aRefract[6].vPos = D3DXVECTOR3 ( vPos.x + 0.86f*fSize,	vPos.y, vPos.z - 0.5f*fSize );
		aRefract[7].vPos = aRefract[1].vPos;
	}

	aRefract[0].cDiffuse = ((DWORD)(0xff*fAlpha))<<24;
    aRefract[1].cDiffuse = 0L;
	aRefract[2].cDiffuse = 0L;
	aRefract[3].cDiffuse = 0L;
	aRefract[4].cDiffuse = 0L;
	aRefract[5].cDiffuse = 0L;
	aRefract[6].cDiffuse = 0L;
	aRefract[7].cDiffuse = 0L;

	D3DXMATRIX	matVP;
	D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView ();
	D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj ();
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	m_pSavedWaveSB->Capture();
	m_pDrawWaveSB->Apply();

	DxSetTextureMatrix ( pd3dDevice, 0, matVP );

	pd3dDevice->SetFVF ( POS_DIFF_TEX1::FVF );

	pd3dDevice->SetTexture ( 0, pTwistTex );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLEFAN, REFRACT_FACES, aRefract, sizeof(POS_DIFF_TEX1) );

	DxResetTextureMatrix ( pd3dDevice, 0 );

	m_pSavedWaveSB->Apply();

	return hr;
}

HRESULT	DxEnvironment::DxWave::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	struct D3DVERTEX4
	{
		D3DXVECTOR4		vPos;
		D3DXVECTOR2		vTex;
	};

	D3DVERTEX4	sDualScreen[4];

	sDualScreen[0].vPos	= D3DXVECTOR4 ( 0.f, 0.f, 1.f, 1.f );
	sDualScreen[1].vPos	= D3DXVECTOR4 ( 256.f, 0.f, 1.f, 1.f );
	sDualScreen[2].vPos	= D3DXVECTOR4 ( 0.f, 256.f, 1.f, 1.f );
	sDualScreen[3].vPos	= D3DXVECTOR4 ( 256.f, 256.f, 1.f, 1.f );

	sDualScreen[0].vTex = D3DXVECTOR2 ( 0.0f, 0.0f );
	sDualScreen[1].vTex = D3DXVECTOR2 ( 1.0f, 0.0f );
	sDualScreen[2].vTex = D3DXVECTOR2 ( 0.0f, 1.0f );
	sDualScreen[3].vTex = D3DXVECTOR2 ( 1.0f, 1.0f );

	m_pSavedDualSB->Capture();
	m_pDrawDualSB->Apply();

	pd3dDevice->SetFVF ( D3DFVF_XYZRHW|D3DFVF_TEX1 );

	pd3dDevice->SetTexture ( 0, pTexture );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sDualScreen, sizeof(D3DVERTEX4) );

	m_pSavedDualSB->Apply();

	return hr;
}