#include "pch.h"
#include "./DxRenderStates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxRenderStates& DxRenderStates::GetInstance()
{
	static DxRenderStates Instance;
	return Instance;
}

DxRenderStates::DxRenderStates(void) :
	m_pSavedAlphaMapRS(NULL),
	m_pDrawAlphaMapRS(NULL),
	m_pONSavedAlphaMapRS(NULL),
	m_pONDrawAlphaMapRS(NULL),
	m_pONSavedSoftAlphaSB(NULL),
	m_pONDrawSoftAlphaSB(NULL),

	m_pSavedUIMapRS(NULL),
	m_pDrawUIMapRS(NULL),
	m_pSavedMake3DMapRS(NULL),
	m_pDrawMake3DMapRS(NULL),
	m_pSavedRender3DMapRS(NULL),
	m_pDrawRender3DMapRS(NULL),
	m_pSavedRenderMiniMapRS(NULL),
	m_pDrawRenderMiniMapRS(NULL),

	m_pSavedAlpha1LandRS(NULL),
	m_pDrawAlpha1LandRS(NULL),
	m_pSavedAlpha2LandRS(NULL),
	m_pDrawAlpha2LandRS(NULL),
	m_pSavedAlpha3LandRS(NULL),
	m_pDrawAlpha3LandRS(NULL),
	m_pSavedAlphaVisibleOff(NULL),
	m_pDrawAlphaVisibleOff(NULL),

	m_pSavedShadowSB(NULL),
	m_pDrawShadowSB(NULL),
	m_pSavedShadowColorSB(NULL),
	m_pDrawShadowColorSB(NULL)
{
}

DxRenderStates::~DxRenderStates(void)
{
}

void DxRenderStates::SetAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlphaMapRS->Capture();
	m_pDrawAlphaMapRS->Apply();
}

void DxRenderStates::ReSetAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlphaMapRS->Apply();
}

void DxRenderStates::SetOnAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pONSavedAlphaMapRS->Capture();
	m_pONDrawAlphaMapRS->Apply();
}

void DxRenderStates::ReSetOnAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pONSavedAlphaMapRS->Apply();
}

void DxRenderStates::SetOnSoftAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pONSavedSoftAlphaSB->Capture();
	m_pONDrawSoftAlphaSB->Apply();
}

void DxRenderStates::ReSetOnSoftAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pONSavedSoftAlphaSB->Apply();
}

void DxRenderStates::SetUIMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedUIMapRS->Capture();
	m_pDrawUIMapRS->Apply();
}

void DxRenderStates::ReSetUIMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedUIMapRS->Apply();
}

void DxRenderStates::SetMake3DMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedMake3DMapRS->Capture();
	m_pDrawMake3DMapRS->Apply();
}

void DxRenderStates::ReSetMake3DMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
  	m_pSavedMake3DMapRS->Apply();
}

void DxRenderStates::SetRender3DMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedRender3DMapRS->Capture();
	m_pDrawRender3DMapRS->Apply();
}

void DxRenderStates::ReSetRender3DMap ( LPDIRECT3DDEVICEQ pd3dDevice )
{
 	m_pSavedRender3DMapRS->Apply();
}

void DxRenderStates::SetRenderMiniMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedRenderMiniMapRS->Capture();
	m_pDrawRenderMiniMapRS->Apply();
}

void DxRenderStates::ReSetRenderMiniMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
 	m_pSavedRenderMiniMapRS->Apply();
}

void DxRenderStates::SetAlphaLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha1LandRS->Capture();
	m_pDrawAlpha1LandRS->Apply();
}

void DxRenderStates::ReSetAlphaLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha1LandRS->Apply();
}

void DxRenderStates::SetAlphaNoZWriLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha2LandRS->Capture();
	m_pDrawAlpha2LandRS->Apply();
}

void DxRenderStates::ReSetAlphaNoZWriLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha2LandRS->Apply();
}

void DxRenderStates::SetAlphaNoTestLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha3LandRS->Capture();
	m_pDrawAlpha3LandRS->Apply();
}

void DxRenderStates::ReSetAlphaNoTestLand ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlpha3LandRS->Apply();
}

void DxRenderStates::SetAlphaVisibleOff ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlphaVisibleOff->Capture();
	m_pDrawAlphaVisibleOff->Apply();
}

void DxRenderStates::ReSetAlphaVisibleOff ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedAlphaVisibleOff->Apply();
}

void DxRenderStates::SetShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedShadowSB->Capture();
	m_pDrawShadowSB->Apply();
}

void DxRenderStates::ReSetShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedShadowSB->Apply();
}

void DxRenderStates::SetShadowColor ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedShadowColorSB->Capture();
	m_pDrawShadowColorSB->Apply();
}

void DxRenderStates::ReSetShadowColor ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedShadowColorSB->Apply();
}


HRESULT DxRenderStates::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	// Anisotropic Filter 확인
	DWORD dwMinFilter;
	if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedAlphaMapRS );
		else			pd3dDevice->EndStateBlock ( &m_pDrawAlphaMapRS );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();
	
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pONSavedAlphaMapRS );
		else			pd3dDevice->EndStateBlock ( &m_pONDrawAlphaMapRS );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pONSavedSoftAlphaSB );
		else			pd3dDevice->EndStateBlock ( &m_pONDrawSoftAlphaSB );
	}

	// Create the state blocks for rendering text
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_ALWAYS);
		pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
	
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedUIMapRS );
		else			pd3dDevice->EndStateBlock( &m_pDrawUIMapRS );
	}
	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE ,			D3DZB_TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE ,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT,			D3DCOLOR_XRGB(220,220,220) );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	
		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedMake3DMapRS );
		else			pd3dDevice->EndStateBlock( &m_pDrawMake3DMapRS );
	}
	
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState(D3DRS_LIGHTING,				FALSE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		TRUE );

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedRender3DMapRS );
		else			pd3dDevice->EndStateBlock( &m_pDrawRender3DMapRS );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_BORDERCOLOR, 0xff000000);
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR);
		
		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderMiniMapRS );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderMiniMapRS );
	}

	//	Land	-	z write
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();
		
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedAlpha1LandRS );
		else			pd3dDevice->EndStateBlock ( &m_pDrawAlpha1LandRS );
	}

	//	Land	-	no z write
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();
		
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,			FALSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedAlpha2LandRS );
		else			pd3dDevice->EndStateBlock ( &m_pDrawAlpha2LandRS );
	}

	//	Land	-	disable alpha test
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();
		
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedAlpha3LandRS );
		else			pd3dDevice->EndStateBlock ( &m_pDrawAlpha3LandRS );
	}

	//	visible:off 케릭터
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_INVSRCCOLOR );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedAlphaVisibleOff );
		else			pd3dDevice->EndStateBlock ( &m_pDrawAlphaVisibleOff );
	}

	// 그림자
	float fBias = -0.0002f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawShadowSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedShadowColorSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawShadowColorSB );
	}

	return S_OK;
}

void DxRenderStates::SAFE_RELEASE_BLOCK ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DSTATEBLOCK9 pBLOCK )
{
	SAFE_RELEASE( pBLOCK );
}

HRESULT DxRenderStates::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( pd3dDevice )
	{
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedAlphaMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawAlphaMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pONSavedAlphaMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pONDrawAlphaMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pONSavedSoftAlphaSB );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pONDrawSoftAlphaSB );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedUIMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawUIMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedMake3DMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawMake3DMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedRender3DMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawRender3DMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedRenderMiniMapRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawRenderMiniMapRS );
		
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedAlpha1LandRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawAlpha1LandRS );
		
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedAlpha2LandRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawAlpha2LandRS );
		
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedAlpha3LandRS );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawAlpha3LandRS );

		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedAlphaVisibleOff );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawAlphaVisibleOff );

		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedShadowSB );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawShadowSB );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pSavedShadowColorSB );
		SAFE_RELEASE_BLOCK ( pd3dDevice, m_pDrawShadowColorSB );
	}

	return S_OK;
}
