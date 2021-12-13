// Terrain0.cpp: implementation of the DxEffectRiver class.
//
//	UPDATE [03.02.20] : 새롭게 물 효과를 바꿈
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxLoadShader.h"
#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./DxSurfaceTex.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"
#include "./DxEnvironment.h"
#include "./DxCubeMap.h"
#include "./DxSkyMan.h"

#include "./DxViewPort.h"

#include "./SerialFile.h"
#include "./Collision.h"

#include "./DxEffectRiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note	:	디버그용,	
BOOL			g_bDebugRiverWireFrame	= FALSE;

LPDIRECT3DVERTEXBUFFERQ	DxEffectRiver::m_pWaveTexVB		= NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectRiver::m_pWaveTexIB		= NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectRiver::m_pRainDropVB	= NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectRiver::m_pRainDropIB	= NULL;

const DWORD				DxEffectRiver::RAINDROP_MAXNUM	= 100;
const float				DxEffectRiver::DEFAULT_SCALE	= 0.01f;
const float				DxEffectRiver::DEFAULT_SPEED	= 0.1f;

const DWORD DxEffectRiver::VERTEXCOLOR::FVF		= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectRiver::D3DWAVETEX::FVF		= D3DFVF_XYZRHW|D3DFVF_TEX1;
const DWORD DxEffectRiver::TYPEID = DEF_EFFECT_RIVER;
const DWORD	DxEffectRiver::VERSION = 0x00000107;
const char	DxEffectRiver::NAME[] = "[ 물 효과 (큰물)( 굴절,반사) ]";
const DWORD DxEffectRiver::FLAG = _EFF_REPLACE_AFTER;

LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_Reflect		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_TnL			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_Dark			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_WaveTex		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_VSPS			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_Reflect_SAVE	= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_TnL_SAVE		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_Dark_SAVE		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_WaveTex_SAVE	= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectRiver::m_pSB_VSPS_SAVE		= NULL;

void DxEffectRiver::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(RIVER_PROPERTY);
	dwVer = VERSION;
}

void DxEffectRiver::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(RIVER_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );

		// 데이터의 추가 셋팅
		m_vCenter = (m_vMax+m_vMin) * 0.5f;
	}
	else if ( dwVer==0x00000106 && dwSize==sizeof(RIVER_PROPERTY_106) )
	{
		RIVER_PROPERTY_106 sProp;
		memcpy ( &sProp, pProp, dwSize );

		m_dwFlag		= sProp.m_dwFlag;
		m_fBumpAlpha	= sProp.m_fBumpAlpha;
		m_vColor		= sProp.m_vColor;
		m_vDarkColor	= sProp.m_vDarkColor;
		m_vDarkVel		= sProp.m_vDarkVel;
		m_vMax			= sProp.m_vMax;
		m_vMin			= sProp.m_vMin;
		m_matFrameComb	= sProp.m_matFrameComb;

		StringCchCopy( m_szTexDark,		MAX_PATH, sProp.m_szTexDark );
		StringCchCopy( m_szTexFlash,	MAX_PATH, sProp.m_szTexFlash );

		// Ver 107
		m_fVel			= 0.6f;
		m_fDarkScale	= sProp.m_vDarkScale.x;
		m_fScale		= 1.f;

		// 데이터의 추가 셋팅
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matFrameComb );
		m_vCenter = (m_vMax+m_vMin) * 0.5f;
	}
	else if ( ((dwVer==0x00000101) || (dwVer==0x00000102) || (dwVer==0x00000103) || (dwVer==0x00000104) || (dwVer==0x00000105)) 
		&& dwSize==sizeof(RIVER_PROPERTY_101_5) )
	{
		RIVER_PROPERTY_101_5	pProp105;
		memcpy ( &pProp105, pProp, dwSize );

		m_dwFlag		= 0L;
		if ( pProp105.m_bDark )		m_dwFlag |= USEDARK;
		if ( pProp105.m_bBase )		m_dwFlag |= USEFLASH;
		if ( pProp105.m_bReflect )	m_dwFlag |= USEREFLECT;
		m_fBumpAlpha	= 0.8f;							// 범프 반사 알파
		
		m_vColor		= D3DXVECTOR3 (200, 200, 200);			
		m_vDarkVel		= D3DXVECTOR2 (0.2f,0.2f);		// 어둠용 이동
		m_vDarkColor	= D3DXVECTOR3 (255,255,255);	// 어둠용 컬러
		m_vMax			= pProp105.m_vMax;
		m_vMin			= pProp105.m_vMin;
		m_matFrameComb	= pProp105.m_matFrameComb;

		StringCchCopy( m_szTexDark,		MAX_PATH, pProp105.m_szTexMove );
		StringCchCopy( m_szTexFlash,	MAX_PATH, pProp105.m_szTexMove );

		// Ver 107
		m_fVel			= 0.6f;
		m_fDarkScale	= 2.f;
		m_fScale		= 1.f;

		// 데이터의 추가 셋팅
		COLLISION::TransformAABB ( m_vMax, m_vMin, m_matFrameComb );
		m_vCenter = (m_vMax+m_vMin) * 0.5f;
	}    
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectRiver::DxEffectRiver () :
	m_pddsTexDark(NULL),
	m_pddsTexFlash(NULL),
	m_pNormalMap(NULL),
	m_p1DMap(NULL),

	m_fBumpAlpha(0.8f),

	m_fVel(0.6f),
	m_fScale(1.f),
	m_vColor(200,200,200),

	m_vDarkVel(0.2f,0.2f),
	m_fDarkScale(2.f),
	m_vDarkColor(255,255,255),

	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),

	m_vAddTex1(D3DXVECTOR2(0.f,0.f)),
	m_vAddTex2(D3DXVECTOR2(0.f,0.f)),
	m_vAddTex_Dark(D3DXVECTOR2(0.f,0.f)),
	m_vCenter(0.f,0.f,0.f),
	m_fTime(0.f),
	m_fBumpStart(0.008f),
	m_fBumpDelta(0.00001f),
	m_fBumpMat(0.0f),
	m_fElapsedTime(0.f),

	m_bEnable_EDIT(FALSE),
	m_bPS14_EDIT(FALSE)
{
	m_dwFlag = 0L;
	m_dwFlag = USEDARK|USEFLASH|USEREFLECT;

	StringCchCopy( m_szTexDark,		MAX_PATH, "_Wa_water1.bmp" );
	StringCchCopy( m_szTexFlash,	MAX_PATH, "_Wa_water1.bmp" );	
	StringCchCopy( m_szNormalMap,	MAX_PATH, "WaterDOT3_256.tga" );
	StringCchCopy( m_sz1DMap,		MAX_PATH, "OceanGradient.bmp" );
}

DxEffectRiver::~DxEffectRiver ()
{
}

HRESULT DxEffectRiver::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				CreateVB ( pd3dDevice, pmsMeshs, pframeCur->matCombined );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectRiver::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;
//	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectRiver::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_pAdaptFrame = NULL;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectRiver::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT00, FtoDW(0.00f) );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT01, FtoDW(0.00f) );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT10, FtoDW(0.01f) );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT11, FtoDW(0.01f) );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVLSCALE, FtoDW(4.0f) );
        pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVLOFFSET, FtoDW(0.0f) );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		if( DxEffectMan::GetInstance().GetBumpMapFormat() == D3DFMT_V8U8 )
            pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP );
        else
            pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE );

		pd3dDevice->SetSamplerState ( 1, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );//D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );//D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Reflect_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Reflect );
	}

	// 일렁이는 모습 표현 할 때 쓰임
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();	

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );

        pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_TnL_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_TnL );
	}

	// 어두운것 표현
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();	

		float fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR );

        pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Dark_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Dark );
	}

	// 어두운것 표현
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();	

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CCW );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_SRCCOLOR );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_WaveTex_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_WaveTex );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();	

		//pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );		// Shader에 Fog 관련 셋팅을 하면 지우도록 하자.

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_VSPS_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_VSPS );
	}

	//// Create vertex shader
 //   {
	//	DWORD dwVertexDecl[] =
	//	{
	//		D3DVSD_STREAM(0),
	//		D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
	//		D3DVSD_REG( 5, D3DVSDT_D3DCOLOR ),			//D3DVSDE_
	//		D3DVSD_REG( 7, D3DVSDT_FLOAT2 ),			//D3DVSDE_
	//		D3DVSD_REG( 8, D3DVSDT_FLOAT2 ),			//D3DVSDE_
	//		D3DVSD_END()
	//	};

 //       LPD3DXBUFFER pCode;

	//	std::string strFilePath = DXShaderMan::GetInstance().GetPath();
	//	strFilePath += "Reflect.vsh";
 //       // Assemble the vertex shader from the file
	//	if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
 //                                                    0, NULL, &pCode, NULL ) ) )
	//		return hr;

 //       // Create the vertex shader
 //       hr = pd3dDevice->CreateVertexShader( dwVertexDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwRiverVS, 0 );
 //       pCode->Release();
 //       if( FAILED(hr) )
 //           return hr;


	//	strFilePath = DXShaderMan::GetInstance().GetPath();
	//	strFilePath += "Reflect.psh";
 //       // Assemble the vertex shader from the file
	//	if( FAILED( hr = D3DXAssembleShaderFromFile( strFilePath.c_str(), 
 //                                                    0, NULL, &pCode, NULL ) ) )
	//		return hr;

 //       // Create the vertex shader
	//	hr = pd3dDevice->CreatePixelShader ( (DWORD*)pCode->GetBufferPointer(), &m_dwRiverPS );
 //       pCode->Release();
 //       if( FAILED(hr) )
 //           return hr;
 //   }

	if ( !DxEffectMan::GetInstance().GetUseSwShader() )	// VS 하드웨어 가속이 가능 할 경우만 로드
	{
		// Note : m_pReflectVS
		hr = pd3dDevice->CreateVertexShader( (DWORD*)m_dwReflectAVS, &m_pReflectVS );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] VS _ Reflect River FAILED" );
			return E_FAIL;
		}

		D3DVERTEXELEMENT9 decl[] = 
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
			//D3DVSD_STREAM(0),
			//D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		//D3DVSDE_POSITION,  0  
			//D3DVSD_REG(5, D3DVSDT_D3DCOLOR ),	//D3DVSDE_DIFFUSE,   5  
			//D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//D3DVSDE_
			//D3DVSD_END()
		};

		hr = pd3dDevice->CreateVertexDeclaration( decl,  &m_pReflectDCRT );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] CreateVertexDeclaration _ Reflect River FAILED" );
			return E_FAIL;
		}

		// Note : m_dwOceanWaterVS
		hr = pd3dDevice->CreateVertexShader( (DWORD*)m_dwOceanWaterAVS, &m_pOceanWaterVS );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] VS _ m_dwOceanWaterVS FAILED" );
			return E_FAIL;
		}

		// Note : m_pRiverVS
		hr = pd3dDevice->CreateVertexShader( (DWORD*)m_dwRiverAVS, &m_pRiverVS );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] VS _ m_pRiverVS FAILED" );
			return E_FAIL;
		}

		//Note : CreateVertexDeclaration
		D3DVERTEXELEMENT9 dwVertexDecl[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
			D3DDECL_END()
			//D3DVSD_STREAM(0),
			//D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),			//D3DVSDE_POSITION,  0  
			//D3DVSD_REG( 5, D3DVSDT_D3DCOLOR ),			//D3DVSDE_
			//D3DVSD_REG( 7, D3DVSDT_FLOAT2 ),			//D3DVSDE_
			//D3DVSD_REG( 8, D3DVSDT_FLOAT2 ),			//D3DVSDE_
			//D3DVSD_END()
		};

		hr = pd3dDevice->CreateVertexDeclaration( dwVertexDecl,  &m_pRiverDCRT );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] CreateVertexDeclaration _ m_pOceanWaterDCRT FAILED" );
			return E_FAIL;
		}
	}

	if ( DxEffectMan::GetInstance().IsPixelShader_1_4() )	// PS.1.4 하드웨어 가속이 가능 할 경우만 로드
	{
		hr = pd3dDevice->CreatePixelShader( (DWORD*)m_dwOceanWaterAPS, &m_pOceanWaterPS );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] PS _ m_dwOceanWaterPS FAILED" );
			return E_FAIL;
		}

		hr = pd3dDevice->CreatePixelShader( (DWORD*)m_dwRiverAPS, &m_pRiverPS );
		if( FAILED(hr) )
		{
			CDebugSet::ToListView ( "[ERROR] PS _ m_dwRiverPS FAILED" );
			return E_FAIL;
		}
	}


	pd3dDevice->CreateVertexBuffer ( 25*sizeof(D3DWAVETEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DWAVETEX::FVF, D3DPOOL_SYSTEMMEM, &m_pWaveTexVB, NULL );
	GASSERT ( m_pWaveTexVB && "DxEffectRiver의 m_pWaveTexVB 생성 실패" );

	pd3dDevice->CreateIndexBuffer ( 3*32*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWaveTexIB, NULL );
	GASSERT ( m_pWaveTexIB && "DxEffectRiver의 m_pWaveTexIB 생성 실패" );

	WORD*	pIndices;
	m_pWaveTexIB->Lock ( 0, 0, (VOID**)&pIndices, 0L );
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


	pd3dDevice->CreateVertexBuffer ( 4*RAINDROP_MAXNUM*sizeof(VERTEXCOLOR), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEXCOLOR::FVF, D3DPOOL_SYSTEMMEM, &m_pRainDropVB, NULL );
	GASSERT ( m_pRainDropVB && "DxEffectRiver의 m_pRainDropVB 생성 실패" );

	pd3dDevice->CreateIndexBuffer ( 6*RAINDROP_MAXNUM*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pRainDropIB, NULL );
	GASSERT ( m_pRainDropIB && "DxEffectRiver의 m_pRainDropIB 생성 실패" );

	VERTEXCOLOR*	pVertices;
	m_pRainDropVB->Lock( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD	i=0; i<RAINDROP_MAXNUM; i++ )
	{
		pVertices[(4*i)+0].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		pVertices[(4*i)+0].dwColor = 0xffffffff;
		pVertices[(4*i)+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );

		pVertices[(4*i)+1].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		pVertices[(4*i)+1].dwColor = 0xffffffff;
		pVertices[(4*i)+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );

		pVertices[(4*i)+2].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		pVertices[(4*i)+2].dwColor = 0xffffffff;
		pVertices[(4*i)+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );

		pVertices[(4*i)+3].vPos = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		pVertices[(4*i)+3].dwColor = 0xffffffff;
		pVertices[(4*i)+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pRainDropVB->Unlock ();

	m_pRainDropIB->Lock ( 0, 0, (VOID**)&pIndices, 0L );
	for ( DWORD	i=0; i<RAINDROP_MAXNUM; i++ )
	{
		*pIndices++ = (WORD)((4*i)+0);
		*pIndices++ = (WORD)((4*i)+1);
		*pIndices++ = (WORD)((4*i)+2);

		*pIndices++ = (WORD)((4*i)+1);
		*pIndices++ = (WORD)((4*i)+3);
		*pIndices++ = (WORD)((4*i)+2);
	}
	m_pRainDropIB->Unlock ();

	return S_OK;
}

HRESULT DxEffectRiver::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : State Block
	SAFE_RELEASE( m_pSB_Reflect );
	SAFE_RELEASE( m_pSB_TnL );
	SAFE_RELEASE( m_pSB_Dark );
	SAFE_RELEASE( m_pSB_WaveTex );
	SAFE_RELEASE( m_pSB_VSPS );
	SAFE_RELEASE( m_pSB_Reflect_SAVE );
	SAFE_RELEASE( m_pSB_TnL_SAVE );
	SAFE_RELEASE( m_pSB_Dark_SAVE );
	SAFE_RELEASE( m_pSB_WaveTex_SAVE );
	SAFE_RELEASE( m_pSB_VSPS_SAVE );

	// Note : Shader
	SAFE_RELEASE( m_pRiverVS );
	SAFE_RELEASE( m_pRiverPS );
	SAFE_RELEASE( m_pReflectVS );
	SAFE_RELEASE( m_pOceanWaterVS );
	SAFE_RELEASE( m_pOceanWaterPS );

	SAFE_RELEASE( m_pReflectDCRT );
	SAFE_RELEASE( m_pRiverDCRT );

	// Note : VB, IB
	SAFE_RELEASE ( m_pWaveTexVB );
	SAFE_RELEASE ( m_pWaveTexIB );

	SAFE_RELEASE ( m_pRainDropVB );
	SAFE_RELEASE ( m_pRainDropIB );

	return S_OK;
}

HRESULT DxEffectRiver::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexDark, pd3dDevice, m_pddsTexDark, 0, 0 );
	TextureManager::LoadTexture ( m_szTexFlash, pd3dDevice, m_pddsTexFlash, 0, 0 );		// 테스트
	TextureManager::LoadTexture ( m_szNormalMap, pd3dDevice, m_pNormalMap, 0, 0 );		// 테스트
	TextureManager::LoadTexture ( m_sz1DMap, pd3dDevice, m_p1DMap, 0, 0 );				// 테스트
	
	//m_cRainDrop.InitDeviceObjects ( pd3dDevice );	// 빗방울

	return S_OK;
}

HRESULT DxEffectRiver::DeleteDeviceObjects ()
{
	HRESULT hr  = S_OK;

	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szTexDark,	m_pddsTexDark );
	TextureManager::ReleaseTexture( m_szTexFlash,	m_pddsTexFlash );
	TextureManager::ReleaseTexture( m_szNormalMap,	m_pNormalMap );
	TextureManager::ReleaseTexture( m_sz1DMap,		m_p1DMap );

	//m_cRainDrop.DeleteDeviceObjects ();	// 빗방울

	return hr;
}

HRESULT DxEffectRiver::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectRiver::InvalidateDeviceObjects ()
{
	return S_OK;
}

//	Note : 랜더링 되지 않을때 필요 없는 부분을 제거하고
//		랜더링 되지 않을때의 부하 경감을 위해 가장 단순하게 유지한다.
//
HRESULT DxEffectRiver::FrameMove ( float fTime, float fElapsedTime )
{
	m_fSNOWSTOP = 1.f - DxWeatherMan::GetInstance()->GetSnow()->GetApplyRate();
	fElapsedTime *= m_fSNOWSTOP;	// 눈올때 멈춘다.

	m_fTime			+= fElapsedTime;
	m_fElapsedTime	= fElapsedTime;

//	m_cRainDrop.FrameMove ( fTime, fElapsedTime );	// 빗방울

	return S_OK;
}	

HRESULT DxEffectRiver::Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;

	if ( m_dwFlag & USESEE )	m_dwFlag |= USESAMEHEIGHT;


	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	if ( g_bDebugRiverWireFrame )
	{
		DWORD	dwWireFrame, dwAmbient;
		pd3dDevice->GetRenderState ( D3DRS_AMBIENT,		&dwAmbient );
		pd3dDevice->GetRenderState ( D3DRS_FILLMODE,	&dwWireFrame );

		pd3dDevice->SetRenderState ( D3DRS_AMBIENT,		0xffffffff );
		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );

		RenderTnL ( pd3dDevice, pLandMan );
		
		pd3dDevice->SetRenderState ( D3DRS_AMBIENT,		dwAmbient );
		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,	dwWireFrame );
	}
	else if ( m_bEnable_EDIT )	
	{
		RenderEditMODE ( pd3dDevice, pLandMan );
	}
	else
	{
		RenderGameMODE ( pd3dDevice, pLandMan );
	}

	return S_OK;
}

HRESULT DxEffectRiver::RenderEditMODE ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( m_bPS14_EDIT && DxEffectMan::GetInstance().GetUseBumpMap ())
	{
		if ( DxEffectMan::GetInstance().IsPixelShader_1_4() && DxEffectMan::GetInstance().GetUseBumpMap() && 
			(m_dwFlag&USEREFLECT) && DxEffectMan::GetInstance().IsRealReflect() )
		{
			//DxCubeMap::GetInstance().SetActiveON();						// 큐브맵을 활성화 한다.
			DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE), m_vMax, m_vMin );	//	반사
			Render_VS_PS_2 ( pd3dDevice, pLandMan );
		}
	}
	else
	{
		D3DCOLOR	dwFogColor;
		pd3dDevice->GetRenderState( D3DRS_FOGCOLOR, &dwFogColor );

		D3DXVECTOR3	vFromPt			= DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3	vCenterFromPt	= m_vCenter - vFromPt;
		float		fLength			= D3DXVec3Length ( &vCenterFromPt );
		m_fBumpMat = m_fBumpStart - m_fBumpDelta*fLength;
		if ( m_fBumpMat > 0.04f )	m_fBumpMat = 0.04f;
		if ( m_fBumpMat < 0.00f )	m_fBumpMat = 0.00f;
		m_fBumpMat = m_fBumpMat * m_fSNOWSTOP;			// 눈올때 멈춘다.

		if ( (m_dwFlag&USEREFLECT) && DxEffectMan::GetInstance().IsRealReflect() && DxEffectMan::GetInstance().GetUseBumpMap() )
		{
			if ( DxEnvironment::GetInstance().IsRecentReflect() )
			{
				DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE) );	//	반사
				RenderReflectionEX ( pd3dDevice, pLandMan );
			}
			else
			{
				DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE), m_vMax, m_vMin );	//	반사
				RenderReflection ( pd3dDevice, pLandMan );
			}
		}
		if ( m_dwFlag&USEFLASH )
		{
			pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0x00000000 );
			RenderTnL ( pd3dDevice, pLandMan );
		}
		if ( m_dwFlag&USEDARK )			//	Note : 어두운거 표현
		{
			pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0xffffffff );
			RenderDark ( pd3dDevice, pLandMan );
		}

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, dwFogColor );
	}

	return S_OK;
}

HRESULT DxEffectRiver::RenderGameMODE ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	//	P.S 1.4 버젼 물이다.
	//
	if ( DxEffectMan::GetInstance().IsPixelShader_1_4() && DxEffectMan::GetInstance().GetUseBumpMap() && 
		(m_dwFlag&USEREFLECT) && DxEffectMan::GetInstance().IsRealReflect() &&
		DxSurfaceTex::GetInstance().IsEnable() )
	{
		//DxCubeMap::GetInstance().SetActiveON();						// 큐브맵을 활성화 한다.
		DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE), m_vMax, m_vMin );	//	반사
		Render_VS_PS_2 ( pd3dDevice, pLandMan );

		return S_OK;
	}

	//if ( DxCubeMap::GetInstance().Is_GraphicCard_Option_Enable() && (m_nTestDrawing==1) )
	//{
	//	DxCubeMap::GetInstance().SetActiveON();		// 큐브맵을 활성화 한다.

	//	Render_VS_PS ( pd3dDevice );

	//	return S_OK;						// 이것하면 더이상 아무것도 뿌리지 않는다.
	//}

	D3DCOLOR	dwFogColor;
	pd3dDevice->GetRenderState( D3DRS_FOGCOLOR, &dwFogColor );

	if ( DxEffectMan::GetInstance().GetUseBumpMap() )	//	Note : 반사와 굴절
	{
		D3DXVECTOR3	vFromPt			= DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3	vCenterFromPt	= m_vCenter - vFromPt;
		float		fLength			= D3DXVec3Length ( &vCenterFromPt );
		m_fBumpMat = m_fBumpStart - m_fBumpDelta*fLength;
		if ( m_fBumpMat > 0.04f )	m_fBumpMat = 0.04f;
		if ( m_fBumpMat < 0.00f )	m_fBumpMat = 0.00f;
		m_fBumpMat = m_fBumpMat * m_fSNOWSTOP;			// 눈올때 멈춘다.

		if ( (m_dwFlag&USEREFLECT) && DxEffectMan::GetInstance().IsRealReflect() && DxEffectMan::GetInstance().GetUseBumpMap() )	//.GetDetailFlag () & REALREFLECT) )
		{
			if ( DxEnvironment::GetInstance().IsRecentReflect() )
			{
				DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE) );	//	반사
				RenderReflectionEX ( pd3dDevice, pLandMan );
			}
			else
			{
				DxEnvironment::GetInstance().SetReflection ( m_vCenter, (m_dwFlag&USESAMEHEIGHT), (m_dwFlag&USESEE), m_vMax, m_vMin );	//	반사
				RenderReflection ( pd3dDevice, pLandMan );
			}
		}
		////	Note : 굴절 효과
		////
		//if ( m_bRefract )
		//{
		//	DxEnvironment::GetInstance().SetWaveWater ( pd3dDevice, m_matWorld, m_pWaterVB, m_pStaticIB, m_dwVertices, m_dwRenderFaces, 1.0f );
		//}
	}
	if ( m_dwFlag&USEFLASH )						//	Note : 기본형
	{
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0x00000000 );
		RenderTnL ( pd3dDevice, pLandMan );
	}
	if ( m_dwFlag&USEDARK )			//	Note : 어두운거 표현
	{
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, 0xffffffff );
		RenderDark ( pd3dDevice, pLandMan );
	}

	pd3dDevice->SetRenderState( D3DRS_FOGCOLOR, dwFogColor );

	//// Note : 굴절용 좌표
	////
	//if ( m_bRefract )
	//{
	//	D3DWATERVERTEX* pVert;
	//	m_pWaterVB->Lock( 0, 0, (VOID**)&pVert, 0L  );
	//	for ( DWORD i=0; i<m_dwVertices; i++ )
	//	{
	//		pVert[i].vTex1.x = m_pTexUV[i].x;
	//		pVert[i].vTex1.y = 0.5f*m_fTime + m_pTexUV[i].y;
	//	}
	//	m_pWaterVB->Unlock();
	//}

//	m_cRainDrop.Render ( pd3dDevice, m_pRainDropVB, m_pRainDropIB );	// 빗방울

	return S_OK;
}

void DxEffectRiver::D3DXMatrixTexScaleBias ( D3DXMATRIX& Out )
{
	//set special texture matrix for shadow mapping
	unsigned int range;
	float fOffsetX = 0.5f + (0.5f / (float)512);
	float fOffsetY = 0.5f + (0.5f / (float)512);

	range = 0xFFFFFFFF >> (32 - 16);

	float fBias    = -0.001f * (float)range;
	Out._11 = 0.5f;		Out._12 = 0.f;		Out._13 = 0.f;			Out._14 = 0.f;
	Out._21 = 0.0f;		Out._22 = -0.5f;	Out._23 = 0.f;			Out._24 = 0.f;
	Out._31 = 0.0f;		Out._32 = 0.f;		Out._33 = (float)range;	Out._34 = 0.f;
	Out._41 = fOffsetX;	Out._42 = fOffsetY;	Out._43 = fBias;		Out._44 = 1.f;
}


HRESULT	DxEffectRiver::CreateVB ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, D3DXMATRIX& matWorld )
{
	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( !pmsMeshs->m_pLocalMesh )	return S_OK;
		
	DWORD dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
	DWORD dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

	VERTEX*			pSrcVertices;
	WORD*			pSrcIndices;
	pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );

	m_sWaterAABB.Create ( pd3dDevice, (BYTE*)pSrcVertices, pSrcIndices, dwFaces, matWorld, VERTEX::FVF );


	m_vMax = m_vMin = pSrcVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		m_vMax.x = (pSrcVertices[i].vPos.x > m_vMax.x) ? pSrcVertices[i].vPos.x : m_vMax.x ;
		m_vMax.y = (pSrcVertices[i].vPos.y > m_vMax.y) ? pSrcVertices[i].vPos.y : m_vMax.y ;
		m_vMax.z = (pSrcVertices[i].vPos.z > m_vMax.z) ? pSrcVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pSrcVertices[i].vPos.x < m_vMin.x) ? pSrcVertices[i].vPos.x : m_vMin.x ;
		m_vMin.y = (pSrcVertices[i].vPos.y < m_vMin.y) ? pSrcVertices[i].vPos.y : m_vMin.y ;
		m_vMin.z = (pSrcVertices[i].vPos.z < m_vMin.z) ? pSrcVertices[i].vPos.z : m_vMin.z ;
	}

	COLLISION::TransformAABB ( m_vMax, m_vMin, matWorld );

	m_vCenter = (m_vMax+m_vMin) * 0.5f;

	pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();
	pmsMeshs->m_pLocalMesh->UnlockIndexBuffer ();

	return S_OK;
}

void DxEffectRiver::ReSetDiffuse ()
{
	float fDistance;
	float fDistanceX = m_vMax.x - m_vMin.x;
	float fDistanceZ = m_vMax.z - m_vMin.z;

	if ( fDistanceX > fDistanceZ )	fDistance = fDistanceX;
	else							fDistance = fDistanceZ;

	if ( fDistance < 0.001f )	return;

	fDistance = 10.f / fDistance;


	//D3DWATERVERTEX*	pWater;
	//m_pWaterVB->Lock( 0, 0, (VOID**)&pWater, 0L );
	//for ( DWORD i=0; i<m_dwVertices; i++ )
	//{
	//	//((DWORD)(m_fAlpha*255.f)<<24) + 
	//	pWater[i].vColor = ((DWORD)(0xff)<<24) + ((DWORD)(m_vColor.x)<<16) + ((DWORD)(m_vColor.y)<<8) + (DWORD)(m_vColor.z);

	//	m_pTexUV[i] = D3DXVECTOR2 ( (pWater[i].vPos.x-m_vMin.x)*fDistance, 
	//								(pWater[i].vPos.z-m_vMin.z)*fDistance );
	//}
	//m_pWaterVB->Unlock();
}

void DxEffectRiver::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;
}

BOOL DxEffectRiver::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	return COLLISION::IsWithInPoint(vDivMax,vDivMin,m_vCenter);
}

void DxEffectRiver::SaveBuffer ( CSerialFile &SFile )
{
	SFile.BeginBlock ();
	{
		m_sWaterAABB.Save ( SFile );
	}
	SFile.EndBlock ();
}

void DxEffectRiver::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	if ( dwVer==0x00000104 )
	{
		// SEPARATEOBJ::SaveFile 안의 size 크기 설정 에러. ( DWORD )를 넣어서 크기를 틀리게 했다.
		GASSERT ( 0 && "Ver.104 버전에 에러가 있습니다.<River>, 성환에게.. ㅡㅡ" );
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

void DxEffectRiver::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	BOOL bExt;

	//GASSERT(pd3dDevice);

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	if ( VERSION==dwVer )
	{
		m_sWaterAABB.Load ( SFile );
		return;
	}
	else if ( dwVer==0x00000106 )
	{
		DWORD dwVertices;
		DWORD dwFaces;
		D3DWATERVERTEX* pSrcVert = NULL;

		SFile >> dwVertices;
		SFile >> dwFaces;

		SFile >> bExt;
		if ( bExt )
		{
			D3DXVECTOR2* pTexUV = new D3DXVECTOR2[dwVertices];
			SFile.ReadBuffer ( pTexUV, sizeof(D3DXVECTOR2)*dwVertices );
			SAFE_DELETE_ARRAY( pTexUV );
		}

		SFile >> bExt;
		if ( bExt )
		{
			pSrcVert = new D3DWATERVERTEX[dwVertices];
			SFile.ReadBuffer ( pSrcVert, sizeof(D3DWATERVERTEX)*dwVertices );
		}

		D3DXVECTOR3 vTemp;
		WORD		wTemp;

		DWORD dwFaceNUM = 0;
		WORD* pIndex = new WORD[dwFaces*3];

		SFile >> bExt;
		while ( bExt )
		{
			SFile >> vTemp;
			SFile >> vTemp;
			SFile >> pIndex[dwFaceNUM*3+0];
			SFile >> pIndex[dwFaceNUM*3+1];
			SFile >> pIndex[dwFaceNUM*3+2];
			SFile >> wTemp;

			SFile >> bExt;

			++dwFaceNUM;
		};

		m_sWaterAABB.Create ( pd3dDevice, (BYTE*)pSrcVert, pIndex, dwFaces, m_matFrameComb, D3DWATERVERTEX::FVF );

		SAFE_DELETE_ARRAY(pSrcVert);
		SAFE_DELETE_ARRAY(pIndex);
		
		return;
	}
	else if ( dwVer==0x00000104 )
	{
		GASSERT ( 0 && "Ver.104 버전에 에러가 있습니다.<River> 옥트리 다시 생성해 주세요" );
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

HRESULT DxEffectRiver::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectRiver *pEffSrc = (DxEffectRiver*)pSrcEffect;
	m_sWaterAABB.CloneTree ( pd3dDevice, pEffSrc->m_sWaterAABB.m_pTree );

	return S_OK;
}


//FLOAT r = 0.4f;//0.04f;
//D3DXMATRIX	m_matBumpMat;
//m_matBumpMat._11 =  r * cosf( m_fTime*0.1f );//* 9.0f );
//m_matBumpMat._12 = -r * sinf( m_fTime*0.1f );//* 9.0f );
//m_matBumpMat._21 =  r * sinf( m_fTime*0.1f );//* 9.0f );
//m_matBumpMat._22 =  r * cosf( m_fTime*0.1f );//* 9.0f );

//pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00,   FtoDW( m_matBumpMat._11 ) );
//pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01,   FtoDW( m_matBumpMat._12 ) );
//pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10,   FtoDW( m_matBumpMat._21 ) );
//pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11,   FtoDW( m_matBumpMat._22 ) );
//pd3dDevice->SetTextureStageState( 0, D3DTSS_BUMPENVLSCALE,  FtoDW(0.8f) );
