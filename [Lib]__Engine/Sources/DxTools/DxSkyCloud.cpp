#include "pch.h"

#include "./GLDefine.h"
#include "DxViewPort.h"
#include "DxFogMan.h"
#include "./TextureManager.h"

#include "DxSkyCloud.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------------------------------------------------
//											D	x		S	k	y		C	l	o	u	d
//---------------------------------------------------------------------------------------------------------------------------------
DxSkyCloud::DxSkyCloud() :
	m_bEnable(FALSE),
	m_strTexName(""),
	m_pTexture(NULL),
	m_fRotateADD(0.001f),
	m_fRotateSUM(0.f)
{
}

DxSkyCloud::~DxSkyCloud()
{
}

void DxSkyCloud::InitData( float fRotate, float fRotateSUM, float fWidthRate, float fHeightRate, const char* pName )
{
	m_bEnable = TRUE;
	m_fRotateADD = fRotate;
	D3DXMatrixScaling( &m_matScale, fWidthRate, fHeightRate, fWidthRate );
	m_strTexName = pName;

	if( fRotateSUM < 0.0001f )
	{
		m_fRotateSUM = D3DX_PI*2.f*RANDOM_POS;
	}
	else
	{
		m_fRotateSUM = fRotateSUM;
	}
}

void DxSkyCloud::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( m_bEnable && "DxSkyCloud isn`t Init" );

	TextureManager::LoadTexture( m_strTexName.c_str(), pd3dDevice, m_pTexture, 0L, 1, FALSE );
}

void DxSkyCloud::OnLostDevice()
{
	TextureManager::ReleaseTexture( m_strTexName.c_str(), m_pTexture );
}

void DxSkyCloud::FrameMove( float fElapsedTime )
{
	m_fRotateSUM += m_fRotateADD*fElapsedTime;

	if( m_fRotateSUM > FLT_MAX )	m_fRotateSUM -= D3DX_PI*2.f;
	if( m_fRotateSUM < FLT_MIN )	m_fRotateSUM += D3DX_PI*2.f;
}

void DxSkyCloud::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase )
{
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, DxFogMan::GetInstance().GetFogWhiteColor() );

	D3DXMatrixRotationY( &m_matRotate, m_fRotateSUM );
	D3DXMatrixMultiply( &m_matRotate, &m_matScale, &m_matRotate );		// 자신의 회전

	D3DXMatrixMultiply( &m_matRotate, &m_matRotate, &matBase );			// 전체 하늘의 회전

	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	m_matRotate._41 = vFromPt.x;
	m_matRotate._42 = vFromPt.y;
	m_matRotate._43 = vFromPt.z;
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matRotate );

	pd3dDevice->SetTexture( 0, m_pTexture );

	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

//---------------------------------------------------------------------------------------------------------------------------------
//									D	x		S	k	y		C	l	o	u	d		M	a	n
//---------------------------------------------------------------------------------------------------------------------------------
const DWORD DxSkyCloudMan::CLOUD_NUM = 8;

DxSkyCloudMan::DxSkyCloudMan() :
	m_pSkyCloud(NULL),
	m_pSavedSB(NULL),
	m_pDrawSB(NULL),
	m_pVB(NULL)
{
}

DxSkyCloudMan::~DxSkyCloudMan()
{
	SAFE_RELEASE( m_pVB );
	SAFE_DELETE_ARRAY( m_pSkyCloud );
}

void DxSkyCloudMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pVB );
	pd3dDevice->CreateVertexBuffer( sizeof(POSTEX)*4, 0, POSTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );

	POSTEX* pVertices(NULL);
	m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	pVertices[0].vPos = D3DXVECTOR3( -1.f, 2.f, 0.f );
	pVertices[1].vPos = D3DXVECTOR3( 1.f, 2.f, 0.f );
	pVertices[2].vPos = D3DXVECTOR3( -1.5f, 0.f, 2.f );
	pVertices[3].vPos = D3DXVECTOR3( 1.5f, 0.f, 2.f );
	pVertices[0].vTex = D3DXVECTOR2( 0.f, 0.f );
	pVertices[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	pVertices[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	pVertices[3].vTex = D3DXVECTOR2( 1.f, 1.f );
	m_pVB->Unlock();

	// CLOUD_NUM
	SAFE_DELETE_ARRAY( m_pSkyCloud );
	m_pSkyCloud = new DxSkyCloud[CLOUD_NUM];
	m_pSkyCloud[0].InitData( 0.0022f, 0.f, 200.f, 75.f, "Cloud00.dds" );
	m_pSkyCloud[1].InitData( -0.0022f, 0.f, 200.f, 75.f, "Cloud01.dds" );
	m_pSkyCloud[2].InitData( 0.0028f, 0.f, 200.f, 75.f, "Cloud02.dds" );
	m_pSkyCloud[3].InitData( -0.0028f, 0.f, 200.f, 75.f, "Cloud03.dds" );
	m_pSkyCloud[4].InitData( 0.0032f, 0.f, 250.f, 75.f, "Cloud04.dds" );
	m_pSkyCloud[5].InitData( -0.0032f, 0.f, 250.f, 75.f, "Cloud05.dds" );
	m_pSkyCloud[6].InitData( 0.0034f, 0.f, 300.f, 75.f, "Cloud06.dds" );
	m_pSkyCloud[7].InitData( -0.0034f, 0.f, 300.f, 75.f, "Cloud07.dds" );
}

void DxSkyCloudMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	for( DWORD i=0; i<CLOUD_NUM; ++i )
	{
		m_pSkyCloud[i].OnResetDevice( pd3dDevice );
	}
}

void DxSkyCloudMan::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	if( !m_pSkyCloud )	return;
	for( DWORD i=0; i<CLOUD_NUM; ++i )
	{
		m_pSkyCloud[i].OnLostDevice();
	}
}

void DxSkyCloudMan::OnDestroyDevice()
{
	SAFE_RELEASE( m_pVB );
	SAFE_DELETE_ARRAY( m_pSkyCloud );
}

void DxSkyCloudMan::FrameMove( float fElapsedTime )
{
	if( !m_pSkyCloud )	return;

	for( DWORD i=0; i<CLOUD_NUM; ++i )
	{
		m_pSkyCloud[i].FrameMove( fElapsedTime );
	}
}

void DxSkyCloudMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase )
{
	if( !m_pSkyCloud )	return;

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetFVF( POSTEX::FVF );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POSTEX) );
	pd3dDevice->SetIndices( NULL );

	for( DWORD i=0; i<CLOUD_NUM; ++i )
	{
		m_pSkyCloud[i].Render( pd3dDevice, matBase );
	}

	//m_pSkyCloud[0].Render( pd3dDevice );

	m_pSavedSB->Apply();
}