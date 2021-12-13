#include "pch.h"

#include "DxViewPort.h"
#include "DxFogMan.h"
#include "./TextureManager.h"

#include "DxSkyBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void ColorUpDown( DWORD& dwColor, float fValue )
{
	int nR = ((dwColor&0xff0000)>>16);
	int nG = ((dwColor&0xff00)>>8);
	int nB = (dwColor&0xff);

	//int nDiffR = 255 - nR;
	//int nDiffG = 255 - nG;
	//int nDiffB = 255 - nB;

	//nDiffR = (int)(nDiffR*fValue);
	//nDiffG = (int)(nDiffG*fValue);
	//nDiffB = (int)(nDiffB*fValue);

	//nR = nR + nDiffR;
	//nG = nG + nDiffG;
	//nB = nB + nDiffB;

	nR = (int)(nR * fValue);
	nG = (int)(nG * fValue);
	nB = (int)(nB * fValue);

	if( nR < 0 )		nR = 0;
	if( nG < 0 )		nG = 0;
	if( nB < 0 )		nB = 0;
	if( nR > 255 )		nR = 255;
	if( nG > 255 )		nG = 255;
	if( nB > 255 )		nB = 255;

	dwColor = 0xff000000;
	dwColor += (nR<<16);
	dwColor += (nG<<8);
	dwColor += nB;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//											D	x		S	k	y		B	a	s	e
//------------------------------------------------------------------------------------------------------------------------------------------
const UINT DxSkyBase::VERTEXNUM = 34;
const UINT DxSkyBase::FACESNUM = 32;

LPDIRECT3DINDEXBUFFERQ DxSkyBase::m_pIB = NULL;

DxSkyBase::DxSkyBase() :
	m_strTexName(""),
	m_pTexture(NULL),
	m_pVB(NULL),
	m_fRotateADD(0.0005f),
	m_fRotateSUM(0.f),
	m_fUV_W(4.f),
	m_bNoInverseUV(TRUE)
{
}

DxSkyBase::~DxSkyBase()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );
}

void DxSkyBase::FrameMove( float fElapsedTime )
{
	m_fRotateSUM += m_fRotateADD*fElapsedTime;

	if( m_fRotateSUM > FLT_MAX )	m_fRotateSUM -= D3DX_PI*2.f;
}

void DxSkyBase::Render( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bReverse, const D3DXMATRIX& matBase )
{
	D3DXMATRIX matWorld(m_matScale);
	if( bReverse )	D3DXMatrixMultiply_MIRROR( matWorld );

	D3DXMatrixRotationY( &m_matRotate, m_fRotateSUM );
	D3DXMatrixMultiply( &m_matRotate, &matWorld, &m_matRotate );		// 자신의 회전

	D3DXMatrixMultiply( &m_matRotate, &m_matRotate, &matBase );			// 전체 하늘의 회전

	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	m_matRotate._41 = vLookatPt.x;
	m_matRotate._42 = vLookatPt.y;
	m_matRotate._43 = vLookatPt.z;
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matRotate );

	pd3dDevice->SetTexture( 0, m_pTexture );

	pd3dDevice->SetFVF( POSTEX::FVF );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(POSTEX) );
	pd3dDevice->SetIndices( m_pIB );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, VERTEXNUM, 0, FACESNUM );
}

void DxSkyBase::InitData( float fRotate, float fWidthRate, float fHeightRate, float fUV_W, BOOL bNoInverse, const char* pName )
{
	m_fRotateADD = fRotate;
	D3DXMatrixScaling( &m_matScale, fWidthRate, fHeightRate, fWidthRate );

	m_fUV_W = fUV_W;
	m_bNoInverseUV = bNoInverse;
	m_strTexName = pName;
}

void DxSkyBase::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float sVertex[] = 
	{
		0.500000f,0.000000f,0.000000f,
		0.461940f,0.000000f,0.191342f,
		0.353553f,0.000000f,0.353553f,
		0.191342f,0.000000f,0.461940f,
		-0.000000f,0.000000f,0.500000f,
		-0.191342f,0.000000f,0.461940f,
		-0.353553f,0.000000f,0.353553f,
		-0.461940f,0.000000f,0.191342f,
		-0.500000f,0.000000f,-0.000000f,
		-0.461940f,0.000000f,-0.191342f,
		-0.353553f,0.000000f,-0.353553f,
		-0.191342f,0.000000f,-0.461940f,
		0.000000f,0.000000f,-0.500000f,
		0.191342f,0.000000f,-0.461940f,
		0.353553f,0.000000f,-0.353553f,
		0.461940f,0.000000f,-0.191342f,
		0.500000f,1.000000f,0.000000f,
		0.461940f,1.000000f,0.191342f,
		0.353553f,1.000000f,0.353553f,
		0.191342f,1.000000f,0.461940f,
		-0.000000f,1.000000f,0.500000f,
		-0.191342f,1.000000f,0.461940f,
		-0.353553f,1.000000f,0.353553f,
		-0.461940f,1.000000f,0.191342f,
		-0.500000f,1.000000f,-0.000000f,
		-0.461940f,1.000000f,-0.191342f,
		-0.353553f,1.000000f,-0.353553f,
		-0.191342f,1.000000f,-0.461940f,
		0.000000f,1.000000f,-0.500000f,
		0.191342f,1.000000f,-0.461940f,
		0.353553f,1.000000f,-0.353553f,
		0.461940f,1.000000f,-0.191342f,
		-0.191342f,0.000000f,0.461940f,
		-0.191342f,1.000000f,0.461940f

	};

	float sTex[] = 
	{
		0.750000f,0.999501f,
		0.812500f,0.999501f,
		0.875000f,0.999501f,
		0.937500f,0.999501f,
		1.000000f,0.999501f,
		1.062500f,0.999501f,
		0.125000f,0.999501f,
		0.187500f,0.999501f,
		0.250000f,0.999501f,
		0.312500f,0.999501f,
		0.375000f,0.999501f,
		0.437500f,0.999501f,
		0.500000f,0.999501f,
		0.562500f,0.999501f,
		0.625000f,0.999501f,
		0.687500f,0.999501f,
		0.750000f,0.000499f,
		0.812500f,0.000499f,
		0.875000f,0.000499f,
		0.937500f,0.000499f,
		1.000000f,0.000499f,
		1.062500f,0.000499f,
		0.125000f,0.000499f,
		0.187500f,0.000499f,
		0.250000f,0.000499f,
		0.312500f,0.000499f,
		0.375000f,0.000499f,
		0.437500f,0.000499f,
		0.500000f,0.000499f,
		0.562500f,0.000499f,
		0.625000f,0.000499f,
		0.687500f,0.000499f,
		0.062500f,0.999501f,
		0.062500f,0.000499f
	};

	WORD wIndices[] = 
	{
		0,17,16,
		17,0,1,
		1,18,17,
		18,1,2,
		2,19,18,
		19,2,3,
		3,20,19,
		20,3,4,
		4,21,20,
		21,4,5,
		32,22,33,
		22,32,6,
		6,23,22,
		23,6,7,
		7,24,23,
		24,7,8,
		8,25,24,
		25,8,9,
		9,26,25,
		26,9,10,
		10,27,26,
		27,10,11,
		11,28,27,
		28,11,12,
		12,29,28,
		29,12,13,
		13,30,29,
		30,13,14,
		14,31,30,
		31,14,15,
		15,16,31,
		16,15,0

	};

	SAFE_RELEASE( m_pVB );
	pd3dDevice->CreateVertexBuffer( sizeof(POSTEX)*VERTEXNUM, 0, POSTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( sizeof(WORD)*FACESNUM*3, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	POSTEX* pVertices(NULL);
	m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	for( DWORD i=0; i<VERTEXNUM; ++i )
	{
		pVertices[i].vPos.x = sVertex[i*3+0];
		pVertices[i].vPos.y = sVertex[i*3+1];
		pVertices[i].vPos.z = sVertex[i*3+2];
		pVertices[i].vTex.x = sTex[i*2+0]*m_fUV_W;	// 텍스쳐를 늘린다.

		if( m_bNoInverseUV )	pVertices[i].vTex.y = sTex[i*2+1];
		else					pVertices[i].vTex.y = -sTex[i*2+1];
	}
	m_pVB->Unlock();

	WORD* pIndices(NULL);
	m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L ); 
	memcpy( pIndices, wIndices, sizeof(WORD)*FACESNUM*3 );
	m_pIB->Unlock();
}

void DxSkyBase::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture( m_strTexName.c_str(), pd3dDevice, m_pTexture, 0L, 1, FALSE );
}

void DxSkyBase::OnLostDevice()
{
	TextureManager::ReleaseTexture( m_strTexName.c_str(), m_pTexture );
}

void DxSkyBase::OnDestroyDevice()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );
}


//------------------------------------------------------------------------------------------------------------------------------------------
//									D	x		S	k	y		B	a	s	e		M	a	n
//------------------------------------------------------------------------------------------------------------------------------------------
const DWORD DxSkyBaseMan::SKY_NUM = 2;

DxSkyBaseMan::DxSkyBaseMan() :
	m_pSavedBaseSkySB(NULL),
	m_pDrawBaseSkySB(NULL),
	m_pSkyBase(NULL)
{
}

DxSkyBaseMan::~DxSkyBaseMan()
{
	SAFE_DELETE_ARRAY( m_pSkyBase );
}

void DxSkyBaseMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_DELETE_ARRAY( m_pSkyBase );
	m_pSkyBase = new DxSkyBase[SKY_NUM];

	m_pSkyBase[0].InitData( 0.013f, 20000.f, 10000.f, 4.f, TRUE, "Fractal.dds" );
	m_pSkyBase[1].InitData( -0.013f, 20000.f, 10000.f, 4.f, FALSE, "Fractal.dds" );

	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		m_pSkyBase[i].OnCreateDevice( pd3dDevice );
	}
}

void DxSkyBaseMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			D3DZB_FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBaseSkySB );
		else			pd3dDevice->EndStateBlock( &m_pDrawBaseSkySB );
	}

	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		m_pSkyBase[i].OnResetDevice( pd3dDevice );
	}
}

void DxSkyBaseMan::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedBaseSkySB );
	SAFE_RELEASE( m_pDrawBaseSkySB );

	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		m_pSkyBase[i].OnLostDevice();
	}
}

void DxSkyBaseMan::OnDestroyDevice()
{
	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		m_pSkyBase[i].OnDestroyDevice();
	}

	SAFE_DELETE_ARRAY( m_pSkyBase );
}

void DxSkyBaseMan::FrameMove( float fElapsedTime )
{
	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		m_pSkyBase[i].FrameMove( fElapsedTime );
	}
}

void DxSkyBaseMan::Render( LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bReflect, const D3DXMATRIX& matBase )
{
	m_pSavedBaseSkySB->Capture();
	m_pDrawBaseSkySB->Apply();

	DWORD dwColor = DxFogMan::GetInstance().GetFogColor();
	if( bReflect )		ColorUpDown( dwColor, 0.6f );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		BOOL bReverse(FALSE);
		m_pSkyBase[i].Render( pd3dDevice, bReverse, matBase );
	}

	m_pSavedBaseSkySB->Apply();
}

void DxSkyBaseMan::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matBase )
{
	m_pSavedBaseSkySB->Capture();
	m_pDrawBaseSkySB->Apply();

	DWORD dwColor = DxFogMan::GetInstance().GetFogColor();
	ColorUpDown( dwColor, 0.6f );

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

	for( DWORD i=0; i<SKY_NUM; ++i )
	{
		BOOL bReverse(TRUE);
		m_pSkyBase[i].Render( pd3dDevice, bReverse, matBase );
	}

	m_pSavedBaseSkySB->Apply();
}


