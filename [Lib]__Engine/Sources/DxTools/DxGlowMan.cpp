#include "pch.h"

#include "./glperiod.h"
#include "./glDefine.h"

#include "DxFrameMesh.h"
#include "DxLandMan.h"

#include "./DxSkinChar.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "DxGlowMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxGlowMan& DxGlowMan::GetInstance()
{
	static DxGlowMan Instance;
	return Instance;
}

const DWORD					DxGlowMan::VERTEX::FVF			= D3DFVF_XYZRHW|D3DFVF_TEX1;
const DWORD					DxGlowMan::VERTEXCOLOR::FVF		= D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pSavedSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pEffectSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pSavedGlowSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pEffectGlowSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pSavedBurnSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pEffectBurnSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pSavedBlurSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxGlowMan::m_pEffectBlurSB		= NULL;

DxGlowMan::DxGlowMan() :
	m_bOptionEnable(TRUE),
	m_bProjectActive(FALSE),
	m_bActive(FALSE),
	m_bActive_Burn(FALSE),
	m_bTEX4(FALSE),
	m_fElapsedTime(0.f),
	m_fMicro_256(0.f),
	m_vDirection(0.f,1.f,0.f)
{
}

DxGlowMan::~DxGlowMan()
{
}

HRESULT DxGlowMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	if( SUCCEEDED( pd3dDevice->GetDeviceCaps( &d3dCaps ) ) )
	{
		m_bTEX4 = (d3dCaps.MaxSimultaneousTextures>=4) ? TRUE : FALSE;
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0x00000000 );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedGlowSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectGlowSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DITHERENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,					D3DBLENDOP_MAX );		// Test Test TEST TEST
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );	// Test Test TEST TEST
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_DIFFUSE );		// Test Test TEST TEST
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );	// Test Test TEST TEST

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBurnSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBurnSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBlurSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBlurSB );
	}

	m_fMicro_256 = 0.5f/256.f;
	
	// 256.f	
	m_sVert_256[0].vPos = D3DXVECTOR4 ( 0.f,	0.f,	1.f, 1.f );
	m_sVert_256[1].vPos = D3DXVECTOR4 ( 256.f,	0.f,	1.f, 1.f );
	m_sVert_256[2].vPos = D3DXVECTOR4 ( 0.f,	256.f,	1.f, 1.f );
	m_sVert_256[3].vPos = D3DXVECTOR4 ( 256.f,	256.f,	1.f, 1.f );
	m_sVert_256[0].vTex = D3DXVECTOR2 ( 0.f+m_fMicro_256,	0.f+m_fMicro_256 );
	m_sVert_256[1].vTex = D3DXVECTOR2 ( 1.f+m_fMicro_256,	0.f+m_fMicro_256 );
	m_sVert_256[2].vTex = D3DXVECTOR2 ( 0.f+m_fMicro_256,	1.f+m_fMicro_256 );
	m_sVert_256[3].vTex = D3DXVECTOR2 ( 1.f+m_fMicro_256,	1.f+m_fMicro_256 );

	// 256.f	
	m_sVert_Color_256[0].vPos = D3DXVECTOR4 ( 0.f,		0.f,	1.f, 1.f );
	m_sVert_Color_256[1].vPos = D3DXVECTOR4 ( 256.f,	0.f,	1.f, 1.f );
	m_sVert_Color_256[2].vPos = D3DXVECTOR4 ( 0.f,		256.f,	1.f, 1.f );
	m_sVert_Color_256[3].vPos = D3DXVECTOR4 ( 256.f,	256.f,	1.f, 1.f );
	m_sVert_Color_256[0].cColor = m_sVert_Color_256[1].cColor = m_sVert_Color_256[2].cColor = m_sVert_Color_256[3].cColor = 0xfffffff;
	m_sVert_Color_256[0].vTex = D3DXVECTOR2 ( 0.f+m_fMicro_256,	0.f+m_fMicro_256 );
	m_sVert_Color_256[1].vTex = D3DXVECTOR2 ( 1.f+m_fMicro_256,	0.f+m_fMicro_256 );
	m_sVert_Color_256[2].vTex = D3DXVECTOR2 ( 0.f+m_fMicro_256,	1.f+m_fMicro_256 );
	m_sVert_Color_256[3].vTex = D3DXVECTOR2 ( 1.f+m_fMicro_256,	1.f+m_fMicro_256 );

	// 512
	m_sVert_512[0].vPos = D3DXVECTOR4 ( 0.f,	0.f,	1.f, 1.f );
	m_sVert_512[1].vPos = D3DXVECTOR4 ( 512.f,	0.f,	1.f, 1.f );
	m_sVert_512[2].vPos = D3DXVECTOR4 ( 0.f,	384.f,	1.f, 1.f );
	m_sVert_512[3].vPos = D3DXVECTOR4 ( 512.f,	384.f,	1.f, 1.f );
	m_sVert_512[0].vTex = D3DXVECTOR2 ( 0.f,	0.f );
	m_sVert_512[1].vTex = D3DXVECTOR2 ( 1.f,	0.f );
	m_sVert_512[2].vTex = D3DXVECTOR2 ( 0.f,	1.f );
	m_sVert_512[3].vTex = D3DXVECTOR2 ( 1.f,	1.f );

	// 전체 화면에 쓰이는 것.
	DWORD dwWidth = DxSurfaceTex::GetInstance().GetWidth();
	DWORD dwHeight = DxSurfaceTex::GetInstance().GetHeight();

	float fMicroWidth = 0.5f/(float)dwWidth;
	float fMicroHeight = 0.5f/(float)dwHeight;

	m_sVert_Full[0].vPos = D3DXVECTOR4 ( 0.f,				0.f,				1.f, 1.f );
	m_sVert_Full[1].vPos = D3DXVECTOR4 ( (float)dwWidth,	0.f,				1.f, 1.f );
	m_sVert_Full[2].vPos = D3DXVECTOR4 ( 0.f,				(float)dwHeight,	1.f, 1.f );
	m_sVert_Full[3].vPos = D3DXVECTOR4 ( (float)dwWidth,	(float)dwHeight,	1.f, 1.f );

	m_sVert_Full[0].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	0.f+fMicroHeight );
	m_sVert_Full[1].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	0.f+fMicroHeight );
	m_sVert_Full[2].vTex = D3DXVECTOR2 ( 0.f+fMicroWidth,	1.f+fMicroHeight );
	m_sVert_Full[3].vTex = D3DXVECTOR2 ( 1.f+fMicroWidth,	1.f+fMicroHeight );

	return S_OK;
}

HRESULT DxGlowMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pEffectSB );
	SAFE_RELEASE( m_pSavedGlowSB );
	SAFE_RELEASE( m_pEffectGlowSB );
	SAFE_RELEASE( m_pSavedBurnSB );
	SAFE_RELEASE( m_pEffectBurnSB );
	SAFE_RELEASE( m_pSavedBlurSB );
	SAFE_RELEASE( m_pEffectBlurSB );

	return S_OK;
}

void DxGlowMan::FrameMove ( float fTime, float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;
}

//----------------------------------------------------------------------------------------------------------------------------
//													Render
//----------------------------------------------------------------------------------------------------------------------------
void DxGlowMan::RenderTex ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;
	if ( !m_bProjectActive || !m_bOptionEnable )			return;
	if ( !m_bActive )										return;

	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	{
		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_TEMP );
		pd3dDevice->SetDepthStencilSurface( NULL );
		pd3dDevice->SetTexture(0, DxSurfaceTex::GetInstance().m_pGlowTex_SRC );
		Render2W( pd3dDevice );

		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf );
		pd3dDevice->SetDepthStencilSurface( NULL );
		pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pGlowTex_TEMP );
		Render2H( pd3dDevice );
	}
	

	// Note : 깨끗하게 지워야 한다.
	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );	
}

void DxGlowMan::Render( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;
	if ( !m_bProjectActive || !m_bOptionEnable )			return;
	if ( !m_bActive )	return;

	m_pSavedSB->Capture();
	m_pEffectSB->Apply();

	pd3dDevice->SetFVF ( VERTEX::FVF );

	pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pGlowTex );
	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVert_Full, sizeof(VERTEX) );

	m_pSavedSB->Apply();
	m_bActive = FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------
//													2 Tex
//----------------------------------------------------------------------------------------------------------------------------
void DxGlowMan::Render2W ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : StateBlock
	m_pSavedGlowSB->Capture();	
	m_pEffectGlowSB->Apply();

	float fHeight = 384.f;
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
		D3DXVECTOR2 vOffSet1( 0.f/512.f, 0.f/fHeight );
		D3DXVECTOR2 vOffSet2( 2.f/512.f, -1.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.5f );
	}

	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		D3DXVECTOR2 vOffSet1 = D3DXVECTOR2( -2.f/512.f, -1.f/fHeight );
		D3DXVECTOR2 vOffSet2 = D3DXVECTOR2( 4.f/512.f, 0.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.3f );
	}

	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		D3DXVECTOR2 vOffSet1 = D3DXVECTOR2( -4.f/512.f, 0.f/fHeight );
		D3DXVECTOR2 vOffSet2 = D3DXVECTOR2( 6.f/512.f, -1.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.2f );
	}

	// Note : StateBlock
	m_pSavedGlowSB->Apply();
}

void DxGlowMan::Render2H ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : StateBlock
	m_pSavedGlowSB->Capture();
	m_pEffectGlowSB->Apply();

	float fHeight = 384.f;
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
		D3DXVECTOR2 vOffSet1( 0.f/512.f, 0.f/fHeight );
		D3DXVECTOR2 vOffSet2( -1.f/512.f, 2.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.5f );
	}

	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		D3DXVECTOR2 vOffSet1 = D3DXVECTOR2(-1.f/512.f, -2.f/fHeight );
		D3DXVECTOR2 vOffSet2 = D3DXVECTOR2( 0.f/512.f, 4.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.3f );
	}

	{
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		D3DXVECTOR2 vOffSet1 = D3DXVECTOR2( 0.f/512.f, -4.f/fHeight );
		D3DXVECTOR2 vOffSet2 = D3DXVECTOR2(-1.f/512.f, 6.f/fHeight );
		TexRender2( pd3dDevice, vOffSet1, vOffSet2, 0.2f );
	}

	// Note : StateBlock
	m_pSavedGlowSB->Apply();
}

void DxGlowMan::TexRender2( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2 vOffSet1, const D3DXVECTOR2 vOffSet2, const float fRate )
{
	DWORD dwColor = (DWORD)( fRate*255.f );
	dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwColor );

	m_sVert_512[0].vTex = D3DXVECTOR2( 0.f+vOffSet1.x,	0.f+vOffSet1.y );
	m_sVert_512[1].vTex = D3DXVECTOR2( 1.f+vOffSet1.x,	0.f+vOffSet1.y );
	m_sVert_512[2].vTex = D3DXVECTOR2( 0.f+vOffSet1.x,	1.f+vOffSet1.y );
	m_sVert_512[3].vTex = D3DXVECTOR2( 1.f+vOffSet1.x,	1.f+vOffSet1.y );

	pd3dDevice->SetFVF( VERTEX::FVF );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_512, sizeof(VERTEX) );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

	m_sVert_512[0].vTex = D3DXVECTOR2( 0.f+vOffSet2.x,	0.f+vOffSet2.y );
	m_sVert_512[1].vTex = D3DXVECTOR2( 1.f+vOffSet2.x,	0.f+vOffSet2.y );
	m_sVert_512[2].vTex = D3DXVECTOR2( 0.f+vOffSet2.x,	1.f+vOffSet2.y );
	m_sVert_512[3].vTex = D3DXVECTOR2( 1.f+vOffSet2.x,	1.f+vOffSet2.y );

	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_sVert_512, sizeof(VERTEX) );
}

//----------------------------------------------------------------------------------------------------------------------------
//													4 Tex
//----------------------------------------------------------------------------------------------------------------------------
void DxGlowMan::Render4 ( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	//pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().GetTempTexture() );
	//pd3dDevice->SetTexture( 1, DxSurfaceTex::GetInstance().GetTempTexture() );
	//pd3dDevice->SetTexture( 2, DxSurfaceTex::GetInstance().GetTempTexture() );
	//pd3dDevice->SetTexture( 3, DxSurfaceTex::GetInstance().GetTempTexture() );

	TexRender4();

	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	pd3dDevice->SetTexture( 3, NULL );
}

void DxGlowMan::TexRender4()
{
}


////----------------------------------------------------------------------------------------------------------------------------
////													Prev Burn
////----------------------------------------------------------------------------------------------------------------------------
void DxGlowMan::RenderBURN( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;
	if( !m_bProjectActive || !m_bOptionEnable )				return;
	if( !m_bActive_Burn )									return;

	// Draw
	{
		GlowRender_Burn ( pd3dDevice );

		m_pSavedSB->Capture();
		m_pEffectSB->Apply();

		pd3dDevice->SetFVF ( VERTEX::FVF );

		pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pBurnResultTex );
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVert_Full, sizeof(VERTEX) );

		m_pSavedSB->Apply();
	}

	// Clear
	{
		LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
		pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
		pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

		pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pBurnNewSuf );
		pd3dDevice->SetDepthStencilSurface( NULL );

		pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
		pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
		SAFE_RELEASE ( pSrcSurface );
		SAFE_RELEASE ( pSrcZBuffer );
	}

	m_bActive_Burn = FALSE;
}

void DxGlowMan::GlowRender_Burn( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !DxSurfaceTex::GetInstance().IsEnable() )	return;

	BOOL		bNewTexture;
	D3DXVECTOR3	vDirection;
	D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	vDirection = vLookatPt - vFromPt;
	if ( ((vDirection.x+0.1f)>m_vDirection.x) && ((vDirection.x-0.1f)<m_vDirection.x) &&
		((vDirection.y+0.1f)>m_vDirection.y) && ((vDirection.y-0.1f)<m_vDirection.y) &&
		((vDirection.z+0.1f)>m_vDirection.z) && ((vDirection.z-0.1f)<m_vDirection.z) )
	{
		bNewTexture = TRUE;
	}
	else	bNewTexture = FALSE;

	m_vDirection = vDirection;


	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf256 );		// 렌더 타겟 설정
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);		// 서페이스 클리어
	pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pBurnResultTex );			// 텍스쳐 설정

	TexOneRender ( pd3dDevice, m_sVert_256 );

//	DWORD	dwValueUp	= (DWORD)(255.f*(0.9f+(RANDOM_POS*0.1f))*m_fReminedTime);
	DWORD	dwValueUp;
	if( m_fElapsedTime>0.1f )		dwValueUp = (DWORD)(255.f*(0.70f+(RANDOM_POS*0.30f)));
	else if( m_fElapsedTime>0.05f )	dwValueUp = (DWORD)(255.f*(0.81f+(RANDOM_POS*0.19f)));
	else							dwValueUp = (DWORD)(255.f*(0.89f+(RANDOM_POS*0.11f)));

	
	DWORD	dwColorUp	= (0xff<<24)	+ (dwValueUp<<16)	+ (dwValueUp<<8)	+ dwValueUp;

	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pBurnResultSuf );	// 렌더 타겟 설정
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);	// 서페이스 클리어

	pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pBurnNewTex );				// 텍스쳐 설정
	TexOneRender ( pd3dDevice, m_sVert_256 );

	if ( bNewTexture )
	{
		pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pTempTex256 );				// 텍스쳐 설정
		TexOneRender( pd3dDevice, m_sVert_Color_256, m_fMicro_256, dwColorUp, 0, 1 );	// 기본 (위)
	}

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );
}

void DxGlowMan::TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEXCOLOR* sVertices, float fMicro, DWORD cColor, int nPixelW, int nPixelH )
{
	float	fSize	= fMicro;					// 칸 이동					50% 작은 것이 보기 좋다.
	float	fWidth	= (nPixelW*fSize)+fMicro;	// 칸 이동 후 위치 보정
	float	fHeight	= (nPixelH*fSize)+fMicro;	// 칸 이동 후 위치 보정

	sVertices[0].cColor = sVertices[1].cColor = sVertices[2].cColor = sVertices[3].cColor = cColor;

	sVertices[0].vTex = D3DXVECTOR2 ( 0.f+fWidth,	0.f+fHeight );
	sVertices[1].vTex = D3DXVECTOR2 ( 1.f+fWidth,	0.f+fHeight );
	sVertices[2].vTex = D3DXVECTOR2 ( 0.f+fWidth,	1.f+fHeight );
	sVertices[3].vTex = D3DXVECTOR2 ( 1.f+fWidth,	1.f+fHeight );

	m_pSavedBurnSB->Capture();	
	m_pEffectBurnSB->Apply();

	pd3dDevice->SetFVF ( VERTEXCOLOR::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sVertices, sizeof(VERTEXCOLOR) );

	m_pSavedBurnSB->Apply();
}

void DxGlowMan::TexOneRender( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* sVertices )
{
	m_pSavedBlurSB->Capture();	
	m_pEffectBlurSB->Apply();

	pd3dDevice->SetFVF ( VERTEX::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sVertices, sizeof(VERTEX) );

	m_pSavedBlurSB->Apply();
}