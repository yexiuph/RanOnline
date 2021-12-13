// DxEffectRainPoint.cpp: implementation of the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./SerialFile.h"

#include "./DxEffectRainDrop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float	DxEffectRainPoint::SEPARATELENGTH		= 250.f;
const DWORD	DxEffectRainPoint::MAX_OBJECT			= 200;
const DWORD	DxEffectRainPoint::MAX_OBJECT_LEAVES	= 30;

LPDIRECT3DVERTEXBUFFERQ	DxEffectRainPoint::m_pVB		= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectRainPoint::m_pColorVB_1	= NULL;
LPDIRECT3DVERTEXBUFFERQ DxEffectRainPoint::m_pNewSnowVB = NULL;
LPDIRECT3DVERTEXBUFFERQ DxEffectRainPoint::m_pNewLeavesVB = NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectRainPoint::m_pColorVB_2	= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectRainPoint::m_pLeavesVB	= NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectRainPoint::m_pIB		= NULL;

const DWORD DxEffectRainPoint::RAINRENDER::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectRainPoint::LEAVESRENDER::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD DxEffectRainPoint::SNOWRENDER::FVF = D3DFVF_XYZ|D3DFVF_TEX1;
const DWORD DxEffectRainPoint::TYPEID = DEF_EFFECT_RAINPOINT;
const DWORD	DxEffectRainPoint::VERSION = 0x00000106;
const char DxEffectRainPoint::NAME[] = "[ 빗방울, 눈, 낙엽 ]";	// 벚꽃
const DWORD DxEffectRainPoint::FLAG = _EFF_SINGLE_AFTER_2;

LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Effect = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Snow = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Leaves = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Ground = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_FireFly = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Effect_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Snow_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Leaves_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_Ground_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRainPoint::m_pSB_FireFly_SAVE = NULL;

const char* DxEffectRainPoint::GetName ()
{ 
	return NAME;
}

void DxEffectRainPoint::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(RAINPOINT_PROPERTY);
	dwVer = VERSION;
}

void DxEffectRainPoint::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(RAINPOINT_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );

		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );
	}
	else if ( ( dwVer==0x00000105 || dwVer==0x00000104 ) && dwSize==sizeof(RAINPOINT_PROPERTY_105) )
	{
		RAINPOINT_PROPERTY_105 sProp;
		memcpy ( &sProp, pProp, dwSize );


		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		m_Property.m_bNewRainEffect = FALSE;
		m_Property.m_fRainRate     = sProp.m_fRainRate;
		m_Property.m_fSnowSize	   = sProp.m_fSnowSize;
		m_Property.m_fLeavesSize   = sProp.m_fLeavesSize;	
		m_Property.m_fLeavesSpeed  = sProp.m_fLeavesSpeed;
		m_Property.m_fGroundSize   = sProp.m_fGroundSize;
		m_Property.m_vMax          = sProp.m_vMax;
		m_Property.m_vMin          = sProp.m_vMin;

	}
	else if ( dwVer==0x00000103 && dwSize==sizeof(RAINPOINT_PROPERTY_105) )
	{
		RAINPOINT_PROPERTY_105 sProp;
		memcpy ( &sProp, pProp, dwSize );

		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		m_Property.m_bNewRainEffect = FALSE;
		m_Property.m_fRainRate     = sProp.m_fRainRate;
		m_Property.m_fSnowSize	   = sProp.m_fSnowSize;
		m_Property.m_fLeavesSize   = sProp.m_fLeavesSize;	
		m_Property.m_fLeavesSpeed  = sProp.m_fLeavesSpeed;
		m_Property.m_fGroundSize   = sProp.m_fGroundSize;
		m_Property.m_vMax          = sProp.m_vMax;
		m_Property.m_vMin          = sProp.m_vMin;

	}
	else if ( dwVer==0x00000102 && dwSize==sizeof(RAINPOINT_PROPERTY_102) )
	{
		RAINPOINT_PROPERTY_102	sProp;

		memcpy ( &sProp, pProp, sizeof(RAINPOINT_PROPERTY_102) );

		m_fSnowSize		= sProp.m_fSnowSize;
		m_fLeavesSize	= sProp.m_fLeavesSize;
		m_fLeavesSpeed	= 10.f;

		m_vMax = sProp.m_vMax;
		m_vMin = sProp.m_vMin;

		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		m_fSnowSize		= 0.25f;
		m_fLeavesSize	= 0.25f;
		m_fLeavesSpeed	= 5.f;

	}
	else if ( dwVer==0x00000101 && dwSize==sizeof(RAINPOINT_PROPERTY_101) )
	{
		RAINPOINT_PROPERTY_101	sProp;

		memcpy ( &sProp, pProp, sizeof(RAINPOINT_PROPERTY_101) );

		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		//	Ver.102
		m_fSnowSize		= 0.5f;
		m_fLeavesSize	= 1.0f;
		m_fLeavesSpeed	= 5.f;

	}
	else if ( dwVer==0x00000100 && dwSize==sizeof(RAINPOINT_PROPERTY_100) )
	{
		RAINPOINT_PROPERTY_100	sProp;

		memcpy ( &sProp, pProp, sizeof(RAINPOINT_PROPERTY_100) );

		StringCchCopy( m_szRainTex,		MAX_PATH, "Rain.dds" );
		StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
		StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
		StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_Rain.bmp" );
		StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

		//	Ver.102
		m_fSnowSize		= 0.5f;
		m_fLeavesSize	= 1.0f;
		m_fLeavesSpeed	= 5.f;

	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectRainPoint::DxEffectRainPoint() :
	m_pframeCur(NULL),
	m_pRainTex(NULL),
	m_pCircleTex(NULL),
	m_pRainDropTex(NULL),
	m_pddsSnowTex(NULL),
	m_pddsLeavesTex(NULL),

	m_pGroundTex(NULL),
	m_pRain_GTex(NULL),
	m_pSnow_GTex(NULL),
	m_pFireFlyTex(NULL),

	m_fRainSize(1.f),

	m_dwFaces(0),
	m_pVertexArray(NULL),
	m_pSeperateList(NULL),
	m_pSeperateTree(NULL)
{
	D3DXMatrixIdentity ( &m_matWorld );
	D3DXMatrixIdentity ( &m_matFrameComb );

	StringCchCopy( m_szRainTex,		MAX_PATH, "RAIN.dds" );
	StringCchCopy( m_szCircleTex,	MAX_PATH, "Circle.bmp" );
	StringCchCopy( m_szRainDropTex,	MAX_PATH, "CircleZ.bmp" );
	StringCchCopy( m_szSnowTex,		MAX_PATH, "_Snow_RAIN.BMP" );
	StringCchCopy( m_szLeavesTex,	MAX_PATH, "leaf_02.tga" );

	StringCchCopy( m_szGroundTex,	MAX_PATH, "Pile_Sand_1.tga" );
	StringCchCopy( m_szRain_GTex,	MAX_PATH, "Pile_Rain_1.tga" );
	StringCchCopy( m_szSnow_GTex,	MAX_PATH, "Pile_Snow_1.tga" );
}

DxEffectRainPoint::~DxEffectRainPoint()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pVertexArray );

	SAFE_DELETE(m_pSeperateList);
	SAFE_DELETE(m_pSeperateTree);
}

HRESULT DxEffectRainPoint::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Effect_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Effect );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Snow_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Snow );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

		// Enable alpha testing (skips pixels with less than a certain alpha.)
		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Leaves_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Leaves );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		float fBias = -0.0003f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		//	Note : SetTextureStageState() 선언
		//
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Ground_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Ground );
	}

	{
		SAFE_RELEASE ( m_pVB );
		SAFE_RELEASE ( m_pColorVB_1 );
		SAFE_RELEASE ( m_pColorVB_2 );
		SAFE_RELEASE ( m_pLeavesVB );
		SAFE_RELEASE ( m_pIB );
		SAFE_RELEASE ( m_pNewSnowVB );
		SAFE_RELEASE ( m_pNewLeavesVB );

		pd3dDevice->CreateVertexBuffer ( 4*MAX_OBJECT*sizeof(SNOWRENDER), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
		{
			SNOWRENDER	*pVertices;
			m_pVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<MAX_OBJECT; i++ )
			{
				pVertices[(i*4)+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
				pVertices[(i*4)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
				pVertices[(i*4)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
				pVertices[(i*4)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
			}
			m_pVB->Unlock ();
		}

		pd3dDevice->CreateVertexBuffer ( 4*MAX_OBJECT*sizeof(RAINRENDER), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM, &m_pColorVB_1, NULL );
		{
			DWORD	dwColor = ((70)<<24) + ((70)<<16) + ((70)<<8) + 70;

			RAINRENDER	*pVertices;
			m_pColorVB_1->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<MAX_OBJECT; i++ )
			{
				pVertices[(i*4)+0].cColor = dwColor;
				pVertices[(i*4)+1].cColor = 0x00000000;
				pVertices[(i*4)+2].cColor = dwColor;
				pVertices[(i*4)+3].cColor = 0x00000000;

				pVertices[(i*4)+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
				pVertices[(i*4)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
				pVertices[(i*4)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
				pVertices[(i*4)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
			}
			m_pColorVB_1->Unlock ();
		}

		pd3dDevice->CreateVertexBuffer ( 4*MAX_OBJECT*sizeof(RAINRENDER), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM, &m_pColorVB_2, NULL );
		{
			RAINRENDER	*pVertices;
			m_pColorVB_2->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<MAX_OBJECT; i++ )
			{
				pVertices[(i*4)+0].cColor = 0xffffffff;
				pVertices[(i*4)+1].cColor = 0xffffffff;
				pVertices[(i*4)+2].cColor = 0xffffffff;
				pVertices[(i*4)+3].cColor = 0xffffffff;

				pVertices[(i*4)+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
				pVertices[(i*4)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
				pVertices[(i*4)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
				pVertices[(i*4)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
			}
			m_pColorVB_2->Unlock ();
		}

		pd3dDevice->CreateVertexBuffer ( 4*MAX_OBJECT_LEAVES*sizeof(LEAVESRENDER), D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY, 0, D3DPOOL_SYSTEMMEM, &m_pLeavesVB, NULL );
		{
			LEAVESRENDER	*pVertices;
			m_pLeavesVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
			for ( DWORD i=0; i<MAX_OBJECT_LEAVES; i++ )
			{
				pVertices[(i*4)+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
				pVertices[(i*4)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
				pVertices[(i*4)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
				pVertices[(i*4)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
			}
			m_pLeavesVB->Unlock ();
		}

		pd3dDevice->CreateIndexBuffer ( 6*MAX_FIREFLY*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL );
		WORD	*pIndices;
		m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		for ( DWORD	i=0; i<MAX_FIREFLY; i++ )
		{
			*pIndices++ = (WORD)((i*4)+0);
			*pIndices++ = (WORD)((i*4)+1);
			*pIndices++ = (WORD)((i*4)+2);

			*pIndices++ = (WORD)((i*4)+1);
			*pIndices++ = (WORD)((i*4)+3);
			*pIndices++ = (WORD)((i*4)+2);
		}
		m_pIB->Unlock ();


		

		// 새로운 눈 효과 버텍스
		{
			SNOWRENDER pointVertex[4]; 
			ZeroMemory( pointVertex, 4 );
			pointVertex[0].vPos.x = pointVertex[2].vPos.x = -0.5f;
			pointVertex[0].vPos.y = pointVertex[1].vPos.y =  0.5f;
			pointVertex[1].vPos.x = pointVertex[3].vPos.x =  0.5f;
			pointVertex[2].vPos.y = pointVertex[3].vPos.y = -0.5f;
			pointVertex[0].vPos.z = pointVertex[1].vPos.z = pointVertex[2].vPos.z = pointVertex[3].vPos.z = 0.0f;

			pointVertex[1].vTex.x = pointVertex[2].vTex.y = pointVertex[3].vTex.x = pointVertex[3].vTex.y = 1.0f;
			pointVertex[0].vTex.x =	pointVertex[2].vTex.x = pointVertex[0].vTex.y = pointVertex[1].vTex.y = 0.0f;

			pd3dDevice->CreateVertexBuffer( 4 * sizeof(SNOWRENDER), 0, SNOWRENDER::FVF, D3DPOOL_DEFAULT, &m_pNewSnowVB, NULL );
			VOID *pVertices;
			m_pNewSnowVB->Lock( 0, 4 * sizeof(SNOWRENDER), (void**)&pVertices, 0 );
			memcpy( pVertices, pointVertex, 4 * sizeof(SNOWRENDER) );
			m_pNewSnowVB->Unlock();
		}

		// 새로운 낙엽 효과 버텍스
		{
			LEAVESRENDER pointVertex[4]; 
			ZeroMemory( pointVertex, 4 );
			pointVertex[0].vPos.x = pointVertex[2].vPos.x = -0.2f;
			pointVertex[0].vPos.y = pointVertex[1].vPos.y =  1.2f;
			pointVertex[1].vPos.x = pointVertex[3].vPos.x =  0.2f;
			pointVertex[2].vPos.y = pointVertex[3].vPos.y = -1.2f;
			pointVertex[0].vPos.z = pointVertex[1].vPos.z = pointVertex[2].vPos.z = pointVertex[3].vPos.z = 0.0f;

			pointVertex[0].vNor = pointVertex[1].vNor = pointVertex[2].vNor = pointVertex[3].vNor =  D3DXVECTOR3( 0.0f, 1.0f, 0.0f );



			pointVertex[1].vTex.x = pointVertex[2].vTex.y = pointVertex[3].vTex.x = pointVertex[3].vTex.y = 1.0f;
			pointVertex[0].vTex.x =	pointVertex[2].vTex.x = pointVertex[0].vTex.y = pointVertex[1].vTex.y = 0.0f;

			pd3dDevice->CreateVertexBuffer( 4 * sizeof(LEAVESRENDER), 0, LEAVESRENDER::FVF, D3DPOOL_DEFAULT, &m_pNewLeavesVB, NULL );
			VOID *pVertices;
			m_pNewLeavesVB->Lock( 0, 4 * sizeof(LEAVESRENDER), (void**)&pVertices, 0 );
			memcpy( pVertices, pointVertex, 4 * sizeof(LEAVESRENDER) );
			m_pNewLeavesVB->Unlock();
		}
	}

	CreateDevice_FireFly( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectRainPoint::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB_Effect );
	SAFE_RELEASE( m_pSB_Snow );
	SAFE_RELEASE( m_pSB_Leaves );
	SAFE_RELEASE( m_pSB_Ground );
	SAFE_RELEASE( m_pSB_Effect_SAVE );
	SAFE_RELEASE( m_pSB_Snow_SAVE );
	SAFE_RELEASE( m_pSB_Leaves_SAVE );
	SAFE_RELEASE( m_pSB_Ground_SAVE );

	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pColorVB_1 );
	SAFE_RELEASE ( m_pColorVB_2 );
	SAFE_RELEASE ( m_pLeavesVB );
	SAFE_RELEASE ( m_pIB );
	SAFE_RELEASE ( m_pNewSnowVB );	
	SAFE_RELEASE ( m_pNewLeavesVB );	

	ReleaseDevice_FireFly( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectRainPoint::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBoundBox ( pmsMeshs->m_pLocalMesh, &pframeCur->matCombined );
				MakeEffRainMesh( pd3dDevice, pmsMeshs );
			}
			//if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			//{
			//	MakeEffRainMesh( pd3dDevice, pmsMeshs );
			//}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}



HRESULT DxEffectRainPoint::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	m_pframeCur = pFrame;
	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );

		// 이 부분에서 처음 뿌려지는 파티클을 미리 생성해두면 된다.
	}
	

	HRESULT hr = AdaptToDxFrameChild ( pFrame, pd3dDevice );

	return hr;
}

HRESULT DxEffectRainPoint::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectRainPoint::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture( m_szRainTex,		pd3dDevice, m_pRainTex, 0, 0 );
	TextureManager::LoadTexture( m_szCircleTex,		pd3dDevice, m_pCircleTex, 0, 0 );
	TextureManager::LoadTexture( m_szRainDropTex,	pd3dDevice, m_pRainDropTex, 0, 0 );
	TextureManager::LoadTexture( m_szSnowTex,		pd3dDevice, m_pddsSnowTex, 0, 0 );
	TextureManager::LoadTexture( m_szLeavesTex,		pd3dDevice, m_pddsLeavesTex, 0, 0 );
	TextureManager::LoadTexture( m_szGroundTex,		pd3dDevice, m_pGroundTex, 0, 0 );
	TextureManager::LoadTexture( m_szRain_GTex,		pd3dDevice, m_pRain_GTex, 0, 0 );
	TextureManager::LoadTexture( m_szSnow_GTex,		pd3dDevice, m_pSnow_GTex, 0, 0 );
	TextureManager::LoadTexture( "Sun.dds",			pd3dDevice, m_pFireFlyTex, 0, 1 );

	if ( m_pframeCur )
		AdaptToDxFrameChild ( m_pframeCur, pd3dDevice );	

	//	옥트리일 때 Texture가 로드가 안된다. 그래서 따로 둠
	//
	SetSeperateTexture ();
	m_NewRainData.SetTexture( m_pRainTex, m_pCircleTex, m_pRainDropTex, m_pddsSnowTex, m_pddsLeavesTex, m_pGroundTex, m_pRain_GTex, m_pSnow_GTex );

	return S_OK;
}

HRESULT DxEffectRainPoint::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szRainTex,	m_pRainTex );
	TextureManager::ReleaseTexture( m_szCircleTex,	m_pCircleTex );
	TextureManager::ReleaseTexture( m_szRainDropTex, m_pRainDropTex );
	TextureManager::ReleaseTexture( m_szSnowTex,	m_pddsSnowTex );
	TextureManager::ReleaseTexture( m_szLeavesTex,	m_pddsLeavesTex );
	TextureManager::ReleaseTexture( m_szGroundTex,	m_pGroundTex );
	TextureManager::ReleaseTexture( m_szRain_GTex,	m_pRain_GTex );
	TextureManager::ReleaseTexture( m_szSnow_GTex,	m_pSnow_GTex );
	TextureManager::ReleaseTexture( "Sun.dds",	m_pFireFlyTex );

	return S_OK;
}

//void DxEffectRainPoint::FirstCreateParticles()
//{
//	m_vecSnowParticle.clear();
//	m_vecRainParticle.clear();
//	m_vecLeavesParticle.clear();
//
//	int i = 0;
//
//	for( i = 0; i < m_iMaxParticleCount; i++ )
//	{
//		SParticle inputParticle;
//		D3DXVECTOR3 vPos, vDir;
//		float fDisappearYPos, fFallSpeed, fWaveRate;
//		vPos = D3DXVECTOR3( RandomNumber( m_vMin.x, m_vMax.x ), RandomNumber( 0.0f, m_fParticleYPos), RandomNumber( m_vMin.z, m_vMax.z ) );
//		vDir = D3DXVECTOR3( RandomNumber( -0.5f, 0.5f ), 0.0f, RandomNumber( -0.5f, 0.5f ) );
//
//		fDisappearYPos = RandomNumber( m_fMinDisappearYPos, m_fMaxDisappearYPos );
//		fFallSpeed = RandomNumber( -10.0f, -5.0f );
//		fWaveRate = RandomNumber( 4.0f, 10.0f );
//
//		inputParticle.CreateParticle( vDir, vPos, fDisappearYPos, fFallSpeed, fWaveRate, vPos );
//
//		m_vecSnowParticle.push_back( inputParticle );
//	}
//}


//void DxEffectRainPoint::UpdateNewSnow( float fElapsedTime )
//{
//	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();
//	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
//	D3DXVECTOR3 vLength;
//	UINT i;
//	float fDir;
//	int testNum1 = 0, testNum2 = 0, testNum3 = 0;
//
//	vLength = vLookat - vFromPt;
//
//	D3DXPLANE plane;	
//	D3DXVECTOR3 vTemp, vLeft, v1, v2;
//	v1 = vLength;
//	v2 = vLookat - D3DXVECTOR3( vLookat.x, vLookat.y + 3.0f, vLookat.z );
//	D3DXVec3Scale( &vLeft, D3DXVec3Cross( &vTemp, &v1, &v2 ), 3.0f );
//	vTemp = D3DXVECTOR3( vFromPt.x, vFromPt.y + 3.0f, vFromPt.z );
//	D3DXPlaneFromPoints( &plane, &vFromPt, &vLeft, &vTemp );
//
//
//	
//	for( i = 0; i < m_vecSnowParticle.size(); i++ )
//	{
//		m_vecSnowParticle[i].bRender = TRUE;
//		vLength = m_vecSnowParticle[i].vPos - vFromPt;
//
//		D3DXVECTOR3 vPos = m_vecSnowParticle[i].vPos;
//
//		if( D3DXVec3Length ( &vLength ) > 1200.0f )
//		{
//			testNum1++;
//			m_vecSnowParticle[i].bRender = FALSE;
//			continue;
//		}
//		if( D3DXPlaneDotCoord( &plane, &m_vecSnowParticle[i].vPos ) < -1.0f )
//		{
//			testNum2++;
//			m_vecSnowParticle[i].bRender = FALSE;
//			continue;
//		}
//
//		testNum3++;
//
//		fDir = sinf(m_vecSnowParticle[i].vPos.y/5.0f)/m_vecSnowParticle[i].fWaveRate;			// 흔들림을 준다.
//
//		m_vecSnowParticle[i].vPos += m_vecSnowParticle[i].vDir;
//		m_vecSnowParticle[i].vPos.y += m_vecSnowParticle[i].fFallSpeed * fElapsedTime;
//		m_vecSnowParticle[i].vPos.x += fDir;
//		m_vecSnowParticle[i].vPos.z += fDir;
//
//
//
//		m_vecSnowParticle[i].fFrame += fElapsedTime;
//
//		if( m_vecSnowParticle[i].vPos.y <= m_vecSnowParticle[i].fDisappearYPos )
//		{
//			m_vecSnowParticle[i].vPos   = m_vecSnowParticle[i].vFirstCreatePos;
//			m_vecSnowParticle[i].vPos.y = m_fParticleYPos;
//		}
//	}
//
//	int test = 0;
//
//}

HRESULT DxEffectRainPoint::FrameMove ( float fTime, float fElapsedTime )
{
	SEPERATEOBJ::m_fElapsedTime	= fElapsedTime;
	CNEWRAINDATA::fElapsedTime  = fElapsedTime;

	if ( fElapsedTime > 0.03f )		fElapsedTime = 0.03f;

	SEPERATEOBJ::dwFlag = 0L;
	CNEWRAINDATA::dwFlag = 0L;
	if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() )		
	{
		SEPERATEOBJ::dwFlag |= PLAY_RAIN; 
		CNEWRAINDATA::dwFlag |= PLAY_RAIN;
	}
	if ( DxWeatherMan::GetInstance()->GetSnow()->UseSnow() )		
	{
		SEPERATEOBJ::dwFlag |= PLAY_SNOW; 
		CNEWRAINDATA::dwFlag |= PLAY_SNOW;
	}
	if ( DxWeatherMan::GetInstance()->GetLeaves()->UseLeaves() )	
	{
		SEPERATEOBJ::dwFlag |= PLAY_LEAVES; 
		CNEWRAINDATA::dwFlag |= PLAY_LEAVES;
	}	



	SEPERATEOBJ::fElapsedTime03	= fElapsedTime;

	SEPERATEOBJ::fRainSize		= m_fRainSize;
	SEPERATEOBJ::fSnowSize		= m_fSnowSize;
	SEPERATEOBJ::fLeavesSize	= m_fLeavesSize;
	SEPERATEOBJ::fLeavesSpeed	= m_fLeavesSpeed;

	return S_OK;
}


HRESULT DxEffectRainPoint::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectRainDrop");

	// Get Texture 
	if( !m_pRainTex )		TextureManager::GetTexture( m_szRainTex,	m_pRainTex );
	if( !m_pCircleTex )		TextureManager::GetTexture( m_szCircleTex,	m_pCircleTex );
	if( !m_pRainDropTex )	TextureManager::GetTexture( m_szRainDropTex, m_pRainDropTex );
	if( !m_pddsSnowTex )	TextureManager::GetTexture( m_szSnowTex,	m_pddsSnowTex );
	if( !m_pddsLeavesTex )	TextureManager::GetTexture( m_szLeavesTex,	m_pddsLeavesTex );
	if( !m_pGroundTex )		TextureManager::GetTexture( m_szGroundTex,	m_pGroundTex );
	if( !m_pRain_GTex)		TextureManager::GetTexture( m_szRain_GTex,	m_pRain_GTex );
	if( !m_pSnow_GTex )		TextureManager::GetTexture( m_szSnow_GTex,	m_pSnow_GTex );


	D3DCOLOR	dwFogColor, dwNewFogColor;
	dwNewFogColor = 0x00000000;
	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR, dwNewFogColor );
	pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR, &dwFogColor );


	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matIdentity );

	if( m_bNewRainEffect == FALSE )
	{
		CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();
		COLLISION::RenderAABBTree ( pd3dDevice, &cv, m_pSeperateTree, FALSE );

		// Note : FireFly를 만든다.
		//FrameMoveFireFly( SEPERATEOBJ::m_fElapsedTime );
		//RenderFireFly( pd3dDevice );
		
	}else{
		m_NewRainData.NewRender( pd3dDevice );
	}

	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR, dwFogColor );
	PROFILE_END("DxEffectRainDrop");



	return S_OK;
}

// This function computes the surface area of a triangle
float DxEffectRainPoint::CalcSurfaceArea(VERTEX* pFV[3])
{
	float s;
	float len[3];

	for (DWORD i=0;i<3;i++)
	{
		float x,y,z;
		x = pFV[(i+1)%3]->p.x - pFV[i]->p.x;
		y = pFV[(i+1)%3]->p.y - pFV[i]->p.y;
		z = pFV[(i+1)%3]->p.z - pFV[i]->p.z;
		len[i] = (float)sqrt(x*x+y*y+z*z);
	}
	
	s = (len[0]+len[1]+len[2])/2.0f;
	s = s*(s-len[0])*(s-len[1])*(s-len[2]);
	if ( s < 0.001f )	return	s = 0.f;
	else				return	sqrtf(s);
}

HRESULT DxEffectRainPoint::MakeEffRainMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	VERTEX*		Array[3];
	float		area=0;
	DWORD		Fullarea=0;

	VERTEX*		pVertices;
	WORD*		pIndices;
	DWORD		Faces;

	Faces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

	pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );

	for(DWORD i=0; i<Faces; i++)
	{
		Array[0] = &pVertices[pIndices[(i*3)+0]];
		Array[1] = &pVertices[pIndices[(i*3)+1]];
		Array[2] = &pVertices[pIndices[(i*3)+2]];
		area += m_fRainRate*CalcSurfaceArea(Array)*0.001f;
	}


//	Vertices = (int)(area*m_fRainRate);	// 빗방울의 갯수를 결정한다.

	//	빗방울이 하나도 생성이 안돼면 지워버린다.
//	if( Vertices < 1.0f )
//		GXRELEASE(*pEffectMesh);
	
	Fullarea = (DWORD)area;
	if ( Fullarea <= 0 )
	{
		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
		pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();

		return S_OK;							// 없다면 그냥 넘긴다.
	}

	SAFE_DELETE_ARRAY ( m_pVertexArray );
	m_pVertexArray = new POSITIONBOOL[Fullarea];

	D3DXVECTOR3* pRainVertex = new D3DXVECTOR3[Fullarea];
	area = 0.f;
	DWORD		dwRainVert = 0;

	for( DWORD j=0; j<Faces; j++ )
	{
		Array[0] = &pVertices[pIndices[(j*3)+0]];
		Array[1] = &pVertices[pIndices[(j*3)+1]];
		Array[2] = &pVertices[pIndices[(j*3)+2]];
		area += m_fRainRate*CalcSurfaceArea(Array)*0.001f;	// 1면의 면적을 계산해서 빗방울 갯수 정함.

		while ( area>1.0f )
		{
			float r1,r2;
			D3DXVECTOR3 s,t;
			// Create new vertex
			r1 = ((float)(rand()%1000)) / 1000.0f;
			r2 = 1.0f-r1;
			r2 = r2 * ((rand()%1000) / 1000.0f);
			s.x = Array[1]->p.x - Array[0]->p.x;
			s.y = Array[1]->p.y - Array[0]->p.y;
			s.z = Array[1]->p.z - Array[0]->p.z;
			t.x = Array[2]->p.x - Array[0]->p.x;
			t.y = Array[2]->p.y - Array[0]->p.y;
			t.z = Array[2]->p.z - Array[0]->p.z;
			pRainVertex[dwRainVert].x = Array[0]->p.x + s.x*r1 + t.x*r2;							// 1 정점 근처에 Vertex를 생성
			pRainVertex[dwRainVert].y = Array[0]->p.y + s.y*r1 + t.y*r2;
			pRainVertex[dwRainVert].z = Array[0]->p.z + s.z*r1 + t.z*r2;

			area -= 1.0f;
			dwRainVert++;

			if ( dwRainVert >= Fullarea )	break;
		}
	
		if ( dwRainVert >= Fullarea )	break;
	}

	pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
	pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();

	m_dwFaces = dwRainVert;

	D3DXMatrixMultiply ( &m_matWorld, m_pmatLocal, &m_matFrameComb );

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		D3DXVec3TransformCoord ( &m_pVertexArray[i].vPos, &pRainVertex[i], &m_matWorld );
		m_pVertexArray[i].bUse = FALSE;
	}

	SAFE_DELETE_ARRAY ( pRainVertex );

	//	분할 시키자.
	//
	SeparateVB ( pd3dDevice, m_vMax, m_vMin, SEPARATELENGTH );

	return S_OK;
}

void DxEffectRainPoint::SetSeperateTexture ()
{
	SEPERATEOBJ*	pCur;
	pCur			= m_pSeperateList;

	while ( pCur )
	{
		pCur->SetTexture ( m_pRainTex, m_pCircleTex, m_pRainDropTex, m_pddsSnowTex, m_pddsLeavesTex,
							m_pGroundTex, m_pRain_GTex, m_pSnow_GTex );
		pCur = pCur->pNext;
	}
}

void DxEffectRainPoint::SeparateVB ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vMax, D3DXVECTOR3 vMin, float fDistance )
{
	SAFE_DELETE(m_pSeperateList);
	SAFE_DELETE(m_pSeperateTree);
	m_NewRainData.vecParticleArea.clear();

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	int	nCol	= (int)((vMax.x - vMin.x)/fDistance) + 1;		// 분할 할..
	int	nRow	= (int)((vMax.z - vMin.z)/fDistance) + 1;
	int nObj	= nCol*nRow;

	float	DeltaX	= (vMax.x - vMin.x) / nCol;
	float	DeltaZ	= (vMax.z - vMin.z) / nRow;

	SEPERATEOBJ*	pCur;
	pCur		= m_pSeperateList;

	D3DXMatrixMultiply ( &m_matWorld, m_pmatLocal, &m_matFrameComb );

	//	Note : 초기화
	//
	for ( DWORD i=0; i<m_dwFaces; i++ )
	{
		m_pVertexArray[i].bUse = FALSE;
	}

	for ( int i=0; i<nCol; i++ )
	{
		for ( int j=0; j<nRow; j++ )
		{
			pCur = new SEPERATEOBJ;

			pCur->vMax	= D3DXVECTOR3 ( vMin.x+(DeltaX*(i+1)) , vMax.y, vMin.z+(DeltaZ*(j+1)) );
			pCur->vMin	= D3DXVECTOR3 ( vMin.x+(DeltaX*i) , vMin.y, vMin.z+(DeltaZ*j) );
			pCur->vCenter = (pCur->vMax+pCur->vMin) * 0.5f;

			//	Note : 현재 범위에 들어온 점 찾기
			//			면 갯수와 위치를 여기서 저장한다.
			//
			FindPoint ( pCur, pCur->vMax, pCur->vMin );

			//	Note : 버텍스 버퍼와 인덱스 버퍼를 생성한다.
			//
			pCur->CreateSeperateVB ( pd3dDevice );

			m_NewRainData.CreateParticleArea( pCur->vMin, pCur->vMax, pCur->vCenter, 
											  m_SnowParticleData, m_RainParticleData, m_LeavesParticleData );
			
			COLLISION::TransformAABB ( pCur->vMax, pCur->vMin, matIdentity );
			//COLLISION::TransformAABB ( pCur->vMax, pCur->vMin, m_matWorld );

			pCur->pNext			= m_pSeperateList;
			m_pSeperateList		= pCur;
		}
	}

	COLLISION::MakeAABBTree ( m_pSeperateTree, m_pSeperateList );
}

void DxEffectRainPoint::FindPoint ( SEPERATEOBJ* pCur, D3DXVECTOR3 vMax, D3DXVECTOR3 vMin )		// Seperate 안쪽으로 넣어야 할듯
{
	DWORD dwTempFaces = 0;

	int*	pSaveIndex = new int[m_dwFaces];

	for ( DWORD i=0; i<m_dwFaces; i++ )
	{
		if ( m_pVertexArray[i].bUse )	continue;

		if ( (m_pVertexArray[i].vPos.x>=vMin.x) && (m_pVertexArray[i].vPos.x<vMax.x) &&
			(m_pVertexArray[i].vPos.z>=vMin.z) && (m_pVertexArray[i].vPos.z<vMax.z) )
		{
			pSaveIndex[dwTempFaces] = i;	// 인덱스 저장
			m_pVertexArray[i].bUse = TRUE;
			dwTempFaces++;
		}
	}

	//	Note : 면 갯수 저장
	//
	pCur->dwFaces = dwTempFaces;

	SAFE_DELETE_ARRAY ( pCur->pVertexArray );
	pCur->pVertexArray = new D3DXVECTOR3[dwTempFaces];

	//	Note : 위치 저장
	//
	for ( DWORD i=0; i<dwTempFaces; i++ )
	{
		pCur->pVertexArray[i] = m_pVertexArray[ pSaveIndex[i] ].vPos;
	}

	SAFE_DELETE_ARRAY ( pSaveIndex );
}



void DxEffectRainPoint::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(POSITIONBOOL)*m_dwFaces;

	if( m_bNewRainEffect )
	{
		/*SFile.WriteBuffer( &m_SnowParticleData, sizeof(SParticleData) ); 
		SFile.WriteBuffer( &m_RainParticleData, sizeof(SParticleData) );
		SFile.WriteBuffer( &m_LeavesParticleData, sizeof(SParticleData) );*/

		int nSize = m_NewRainData.vecParticleArea.size();

		SFile << nSize;

		int i;
		for( i = 0; i < nSize; i++ )
		{
			SFile << m_NewRainData.vecParticleArea[i].vMin;
			SFile << m_NewRainData.vecParticleArea[i].vMax;
			SFile << m_NewRainData.vecParticleArea[i].vCenter;
		}

	}else{
		SFile << m_dwFaces;
		if ( m_dwFaces )
			SFile.WriteBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

		int nSize = 0;
		SEPERATEOBJ* pCur = m_pSeperateList;
		while ( pCur )
		{
			++nSize;
			pCur = pCur->pNext;
		}

		SFile << nSize;

		pCur = m_pSeperateList;
		while ( pCur )
		{
			pCur->Save ( SFile );
			pCur = pCur->pNext;
		}
	}
}

void DxEffectRainPoint::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	if ( VERSION== dwVer )
	{
		if( m_bNewRainEffect )
		{
			m_NewRainData.vecParticleArea.clear();

			/*SFile.ReadBuffer( &m_SnowParticleData, sizeof(SParticleData) ); 
			SFile.ReadBuffer( &m_RainParticleData, sizeof(SParticleData) );
			SFile.ReadBuffer( &m_LeavesParticleData, sizeof(SParticleData) );*/

			int nSize = 0;
			D3DXVECTOR3 vMin, vMax, vCenter;
			SFile >> nSize;

			int i;
			for( i = 0; i < nSize; i++ )
			{
				SFile >> vMin;
				SFile >> vMax;
				SFile >> vCenter;

				m_NewRainData.CreateParticleArea( vMin, vMax, vCenter, m_SnowParticleData, m_RainParticleData, m_LeavesParticleData );
			}
		}else{

			SAFE_DELETE(m_pSeperateTree);
			SAFE_DELETE(m_pSeperateList);

			SFile >> m_dwFaces;

			SAFE_DELETE_ARRAY ( m_pVertexArray );
			m_pVertexArray = new POSITIONBOOL[m_dwFaces];

			if ( m_dwFaces )
				SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

			int nSize = 0;
			SFile >> nSize;

			for ( int i=0; i<nSize; ++i )
			{
				SEPERATEOBJ* pNew = new SEPERATEOBJ;

				pNew->Load( SFile, dwVer );

				pNew->pNext = m_pSeperateList;
				m_pSeperateList = pNew;
			}

			COLLISION::MakeAABBTree ( m_pSeperateTree, m_pSeperateList );
		}
	}else if ( 0x104==dwVer || 0x105 == dwVer )
	{
		SAFE_DELETE(m_pSeperateTree);
		SAFE_DELETE(m_pSeperateList);

		SFile >> m_dwFaces;

		SAFE_DELETE_ARRAY ( m_pVertexArray );
		m_pVertexArray = new POSITIONBOOL[m_dwFaces];

		if ( m_dwFaces )
			SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

		int nSize = 0;
		SFile >> nSize;

		for ( int i=0; i<nSize; ++i )
		{
			SEPERATEOBJ* pNew = new SEPERATEOBJ;

			pNew->Load( SFile, dwVer );

			pNew->pNext = m_pSeperateList;
			m_pSeperateList = pNew;
		}

		COLLISION::MakeAABBTree ( m_pSeperateTree, m_pSeperateList );
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
	return;
}

void DxEffectRainPoint::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

//	GASSERT(pd3dDevice);

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	if(	VERSION == dwVer )
	{
		if( m_bNewRainEffect )
		{
			m_NewRainData.vecParticleArea.clear();

			/*SFile.ReadBuffer( &m_SnowParticleData, sizeof(SParticleData) ); 
			SFile.ReadBuffer( &m_RainParticleData, sizeof(SParticleData) );
			SFile.ReadBuffer( &m_LeavesParticleData, sizeof(SParticleData) );*/

			int nSize = 0;
			D3DXVECTOR3 vMin, vMax, vCenter;
			SFile >> nSize;

			int i;
			for( i = 0; i < nSize; i++ )
			{
				SFile >> vMin;
				SFile >> vMax;
				SFile >> vCenter;

				m_NewRainData.CreateParticleArea( vMin, vMax, vCenter, m_SnowParticleData, m_RainParticleData, m_LeavesParticleData );
			}
		}else{

			SAFE_DELETE(m_pSeperateTree);
			SAFE_DELETE(m_pSeperateList);

			SFile >> m_dwFaces;

			SAFE_DELETE_ARRAY ( m_pVertexArray );
			m_pVertexArray = new POSITIONBOOL[m_dwFaces];

			if ( m_dwFaces )
				SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

			int nSize = 0;
			SFile >> nSize;

			for ( int i=0; i<nSize; ++i )
			{
				SEPERATEOBJ* pNew = new SEPERATEOBJ;

				pNew->Load( SFile, dwVer );

				pNew->pNext = m_pSeperateList;
				m_pSeperateList = pNew;
			}

			COLLISION::MakeAABBTree ( m_pSeperateTree, m_pSeperateList );
		}
		
	}else if( 0x104==dwVer || 0x105 == dwVer )
	{
		SAFE_DELETE(m_pSeperateTree);
		SAFE_DELETE(m_pSeperateList);

		SFile >> m_dwFaces;

		SAFE_DELETE_ARRAY ( m_pVertexArray );
		m_pVertexArray = new POSITIONBOOL[m_dwFaces];

		if ( m_dwFaces )
			SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

		int nSize = 0;
		SFile >> nSize;

		for ( int i=0; i<nSize; ++i )
		{
			SEPERATEOBJ* pNew = new SEPERATEOBJ;

			pNew->Load( SFile, dwVer );

			pNew->pNext = m_pSeperateList;
			m_pSeperateList = pNew;
		}

		COLLISION::MakeAABBTree ( m_pSeperateTree, m_pSeperateList );
	}
	else if( 0x102==dwVer || 0x103==dwVer )
	{
		SFile >> m_dwFaces;

		SAFE_DELETE_ARRAY ( m_pVertexArray );
		m_pVertexArray = new POSITIONBOOL[m_dwFaces];

		if ( m_dwFaces )
			SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

		//	분할 시키자.
		//
		SeparateVB ( pd3dDevice, m_vMax, m_vMin, SEPARATELENGTH );
	}
	else if( 0x101==dwVer )
	{
		SFile >> m_dwFaces;

		SAFE_DELETE_ARRAY ( m_pVertexArray );
		m_pVertexArray = new POSITIONBOOL[m_dwFaces];

		if ( m_dwFaces )
			SFile.ReadBuffer ( m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );

		//	분할 시키자.
		//
		SeparateVB ( pd3dDevice, m_vMax, m_vMin, SEPARATELENGTH );
	}
	else if ( 0x100==dwVer )
	{
		GASSERT(0&&"(비,눈,낙엽)효과의 버전이 낮습니다. 다시 옥트리 메쉬를 만들어 주세요");
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

HRESULT DxEffectRainPoint::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	DxEffectRainPoint* pSrc = (DxEffectRainPoint*) pSrcEffect;

	m_dwFaces = pSrc->m_dwFaces;

	if ( pSrc->m_dwFaces )
	{
		SAFE_DELETE_ARRAY(m_pVertexArray);
		m_pVertexArray = new POSITIONBOOL[m_dwFaces];
		memcpy ( m_pVertexArray, pSrc->m_pVertexArray, sizeof(POSITIONBOOL)*m_dwFaces );
	}

	//	분할 시키자.
	//
	SeparateVB ( pd3dDevice, m_vMax, m_vMin, SEPARATELENGTH );

	return S_OK;
}

HRESULT DxEffectRainPoint::SetBoundBox ( LPD3DXMESH pMesh, D3DXMATRIX *matWorld )
{
	DWORD dwVertices = pMesh->GetNumVertices();

	VERTEX*	pVertices;
	pMesh->LockVertexBuffer ( 0, (VOID**)&pVertices );

	D3DXVECTOR3		pVecter;
	D3DXVec3TransformCoord ( &pVecter, &pVertices[0].p, matWorld );
	m_vMax = m_vMin = pVecter;

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		D3DXVec3TransformCoord ( &pVecter, &pVertices[i].p, matWorld );

		m_vMax.x = (m_vMax.x > pVecter.x) ? m_vMax.x : pVecter.x;
		m_vMin.x = (m_vMin.x > pVecter.x) ? pVecter.x : m_vMin.x;

		m_vMax.y = (m_vMax.y > pVecter.y) ? m_vMax.y : pVecter.y;
		m_vMin.y = (m_vMin.y > pVecter.y) ? pVecter.y : m_vMin.y;

		m_vMax.z = (m_vMax.z > pVecter.z) ? m_vMax.z : pVecter.z;
		m_vMin.z = (m_vMin.z > pVecter.z) ? pVecter.z : m_vMin.z;
	}
	pMesh->UnlockVertexBuffer ();

	return S_OK;
}

void DxEffectRainPoint::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	//COLLISION::TransformAABB( vMax, vMin, m_matFrameComb );
	COLLISION::TransformAABB( vMax, vMin, matIdentity );
}

BOOL DxEffectRainPoint::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}