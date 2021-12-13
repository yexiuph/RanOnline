// DxEffectMan.cpp: implementation of the DxEffectMan class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxReplaceContainer.h"
#include "./DxFrameMesh.h"
#include "./DxLandMan.h"
#include "./DxEffectDot3.h"
#include "./DxEffectRiver.h"
#include "./DxEffectFur.h"
#include "./DxEffectRainDrop.h"
#include "./DxEffectReflect.h"
#include "./DxEffectToon.h"
#include "./DxEffectTiling.h"
#include "./DxEffectCloth.h"
#include "./DxEffectGrass.h"
#include "./DxEffectNature.h"
#include "./DxEffectShadow.h"
#include "./DxEffectShadowLow.h"
#include "./DxEffectWaterLight.h"
#include "./DxEffectRenderState.h"
#include "./DxEffectWater.h"
#include "./DxEffectWater2.h"
#include "./DxEffectSpore.h"
#include "./DxEffectSpecular.h"
#include "./DxEffectSpecular2.h"
#include "./DxEffectMultiTex.h"
#include "./DxEffectNeon.h"
#include "./DxEffectSpecReflect.h"
#include "./DxEffectLightMap.h"
#include "./DxEffectGlow.h"
#include "./DxShadowMap.h"
#include "./DxEffectMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD				g_dwCLOSED_POINT_INDEX	= 0;
D3DXVECTOR3			g_vCLOSED_POINT			= D3DXVECTOR3( 0.f, 0.f, 0.f);
D3DXVECTOR3			g_vCLOSED_NORMAL		= D3DXVECTOR3( 0.f, 1.f, 0.f);
D3DXVECTOR3			g_vPICK_RAY_DIR			= D3DXVECTOR3( 0.f, 0.f, 0.f);
D3DXVECTOR3			g_vPICK_RAY_ORIG		= D3DXVECTOR3( 0.f, 0.f, 0.f);
std::string			g_strPICK_FRAME			= "";
std::string			g_strPICK_TEXTURE		= "";
std::string			g_strPICK_BONE			= "";
BOOL				g_bPLAY_ANIMATION		= TRUE;
EMMOUSESTATE		g_emMOUSE_STATE			= MS_IDLE;
BOOL				g_bTILE_EDIT			= TRUE;
BOOL				g_bFRAME_LIMIT			= FALSE;
BOOL				g_bOBJECT100			= FALSE;
BOOL				g_bWORLD_TOOL			= FALSE;
BOOL				g_bCHAR_EDIT_RUN		= FALSE;


DxEffectMan& DxEffectMan::GetInstance()
{
	static DxEffectMan Instance;
	return Instance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectMan::DxEffectMan() :
	m_dwDetailFlag(NULL),
	m_emSSDetail(SS_ONETEX),
	m_emSkinDetail(SKD_BEST),
	m_dwUseSwShader(D3DUSAGE_SOFTWAREPROCESSING),
	m_bUseMIXED(TRUE),
	m_bUseZTexture(TRUE),
	m_bUse4Texture(TRUE),
	m_bUsePoint(TRUE),
	m_bUseBumpMap(TRUE),
	m_BumpMapFormat(D3DFMT_UNKNOWN),
	m_bBorder(TRUE),
	m_bPixelShader(FALSE),
	m_bPixelShader_1_4(FALSE),
	m_bPixelShader_2(FALSE),

	m_pBumpTexture(NULL),

	pEffectTypeList(NULL)
{
	//	Note : 효과 타입들을 등록해둔다.
	//
	//
//	RegistType ( DxEffectDot3::TYPEID,			DxEffectDot3::NAME );			// 사용 가능
//	RegistType ( DxEffectFur::TYPEID,			DxEffectFur::NAME );
//	RegistType ( DxEffectWater2::TYPEID,		DxEffectWater2::NAME );
//	RegistType ( DxEffectSpecular::TYPEID,		DxEffectSpecular::NAME );
//	RegistType ( DxEffectLightMap::TYPEID,		DxEffectLightMap::NAME );

	RegistType ( DxEffectTiling::TYPEID,		DxEffectTiling::NAME );
	RegistType ( DxEffectCloth::TYPEID,			DxEffectCloth::NAME );
	RegistType ( DxEffectRiver::TYPEID,			DxEffectRiver::NAME );
	RegistType ( DxEffectShadow::TYPEID,		DxEffectShadow::NAME );
	//RegistType ( DxEffectShadowLow::TYPEID,		DxEffectShadowLow::NAME );
	RegistType ( DxEffectReflect::TYPEID,		DxEffectReflect::NAME );
	RegistType ( DxEffectRainPoint::TYPEID,		DxEffectRainPoint::NAME );
	RegistType ( DxEffectGrass::TYPEID,			DxEffectGrass::NAME );
	RegistType ( DxEffectNature::TYPEID,		DxEffectNature::NAME );
	RegistType ( DxEffectWaterLight::TYPEID,	DxEffectWaterLight::NAME );
	RegistType ( DxEffectRenderState::TYPEID,	DxEffectRenderState::NAME );
	RegistType ( DxEffectSpore::TYPEID,			DxEffectSpore::NAME );
	RegistType ( DxEffectSpecular2::TYPEID,		DxEffectSpecular2::NAME );
	RegistType ( DxEffectMultiTex::TYPEID,		DxEffectMultiTex::NAME );
	RegistType ( DxEffectNeon::TYPEID,			DxEffectNeon::NAME );
	RegistType ( DxEffectSpecReflect::TYPEID,	DxEffectSpecReflect::NAME );
	RegistType ( DxEffectGlow::TYPEID,			DxEffectGlow::NAME );
}

DxEffectMan::~DxEffectMan()
{
	SAFE_DELETE(pEffectTypeList);
}

void DxEffectMan::RegistType ( const DWORD TypeID, const char *Name )
{
	//	Note : 새 노드 생성후 대입.
	//
	DxEffectType *pNewType = new DxEffectType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNextType = pEffectTypeList;
	pEffectTypeList = pNewType;
}

HRESULT DxEffectMan::SetCrowTracer ( GLCrowTracer* pCrowTracer )
{
	m_pCrowTracer = pCrowTracer;

	return S_OK;
}

HRESULT DxEffectMan::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//	Note : Border 사용 가능, 불가능
	//
	if ( (d3dCaps.TextureAddressCaps&D3DTADDRESS_BORDER) &&
		(d3dCaps.PixelShaderVersion>=D3DPS_VERSION(1, 0)) )		m_bBorder = TRUE;
	else														m_bBorder = FALSE;

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );

	m_dwDetailFlag |= REALSPECULAR;

	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		m_bUseMIXED = FALSE;
		
	}

	//	Note : Vertex Shader
	//
	if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
	{
		m_dwUseSwShader = 0;	// 이럴 땐 MIXED -> H/W 가속 이다.

		// m_emCharRenderTYPE = EMCRT_VERTEX;		// 강제 셋팅

		if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 0) )	m_bPixelShader = TRUE;
		if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 4) )	m_bPixelShader_1_4 = TRUE;
		if ( d3dCaps.PixelShaderVersion >= D3DPS_VERSION(2, 0) )
		{
			// m_emCharRenderTYPE = EMCRT_PIXEL;	// 강제 셋팅
			m_bPixelShader_2 = TRUE;
		}
	}
	
	//	Note : 텍스쳐가 4 개까지 사용 가능한가 ?
	//
	if ( d3dCaps.MaxSimultaneousTextures < 4 )
	{
		m_bUse4Texture = FALSE;
	}

	//	Note : Z 버퍼에 Texture를 사용 할 수 있나 없나를 테스트 하고 
	//			사용가능하면 TRUE 를 불가능 하면 FALSE 를 호출한다.
	//
	HRESULT hr;
	hr = pD3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
									d3dCaps.DeviceType, d3dsdBackBuffer.Format,
									D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE,
									D3DFMT_D24S8 );
	if ( FAILED(hr) )	m_bUseZTexture = FALSE;


	//	Note : PointSize 가 어디 까지 지원하는가 ? 512 이 넘는다면 
	//			Point 파티클 사용하고 아니면 사용하지 않는다.
	//
	if ( d3dCaps.MaxPointSize < 512 )
	{
		m_bUsePoint = FALSE;
	}
	m_bUsePoint = FALSE;		// FX 계열에서는 느려지는거 같다. 


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

	if( bCanDoV8U8 )        m_BumpMapFormat = D3DFMT_V8U8;
	else if( bCanDoL6V5U5 ) 
	{
		m_BumpMapFormat = D3DFMT_L6V5U5;
		m_bUseBumpMap = FALSE;
	}
	else if( bCanDoL8V8U8 ) 
	{
		m_BumpMapFormat = D3DFMT_X8L8V8U8;
		m_bUseBumpMap = FALSE;
	}
	else					m_bUseBumpMap = FALSE;					// 범프맵 사용 불가




	//	Note : 게임에 관한 각종 디테일 설정
	//
	if ( m_bUseMIXED )		
	{
		m_dwDetailFlag |= REALSPECULAR;
	}
	else
	{
		m_dwDetailFlag	&= ~REALSPECULAR;
		m_emSSDetail	=	SS_DISABLE;
	}



	if ( !m_dwUseSwShader && m_bUseBumpMap && d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 1) )
	{
		m_dwDetailFlag |= REALREFLECT;
		m_emSSDetail	= SS_CHANGETEX;
	}
	else
	{
		m_dwDetailFlag &= ~REALREFLECT;
	}
	m_dwDetailFlag &= ~REALREFRACT;				// 굴절은 기본적으로 꺼준다. (굴절보다는 반사가 눈에 띤다)

	m_emSkinDetail	= SKD_BEST;



	if ( DxEffectMan::GetInstance().GetUseBumpMap () )
	{
		CreateBumpMap( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffectMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT	hr = S_OK;

	//	Note : 그림자 만들기 전 셋팅 끝~!!
	DxEffectCloth::CreateDevice ( pd3dDevice );
	DxEffectDot3::CreateDevice ( pd3dDevice );
	DxEffectRiver::CreateDevice ( pd3dDevice );
	DxEffectFur::CreateDevice ( pd3dDevice );
	DxEffectRainPoint::CreateDevice ( pd3dDevice );
	DxEffectReflect::CreateDevice ( pd3dDevice );
//	DxEffectToon::CreateDevice ( pd3dDevice );
	DxEffectGrass::CreateDevice ( pd3dDevice );
	DxEffectNature::CreateDevice ( pd3dDevice );
	DxEffectShadow::CreateDevice ( pd3dDevice );
	DxEffectShadowLow::CreateDevice ( pd3dDevice );
	DxEffectWaterLight::CreateDevice ( pd3dDevice );
	DxEffectTiling::CreateDevice ( pd3dDevice );
	DxEffectWater::CreateDevice ( pd3dDevice );
	DxEffectWater2::CreateDevice ( pd3dDevice );
	DxEffectSpore::CreateDevice ( pd3dDevice );
	DxEffectSpecular::CreateDevice ( pd3dDevice );
	DxEffectSpecular2::CreateDevice ( pd3dDevice );
	DxEffectNeon::CreateDevice ( pd3dDevice );
	DxEffectSpecReflect::CreateDevice ( pd3dDevice );
	DxEffectLightMap::CreateDevice ( pd3dDevice );
	DxEffectGlow::CreateDevice ( pd3dDevice );
	DxEffectMultiTex::CreateDevice ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectCloth::ReleaseDevice ( pd3dDevice );
	DxEffectDot3::ReleaseDevice ( pd3dDevice );
	DxEffectRiver::ReleaseDevice ( pd3dDevice );
	DxEffectFur::ReleaseDevice ( pd3dDevice );
	DxEffectRainPoint::ReleaseDevice ( pd3dDevice );
	DxEffectReflect::ReleaseDevice ( pd3dDevice );
//	DxEffectToon::ReleaseDevice ( pd3dDevice );
	DxEffectGrass::ReleaseDevice ( pd3dDevice );
	DxEffectNature::ReleaseDevice ( pd3dDevice );
	DxEffectShadow::ReleaseDevice ( pd3dDevice );
	DxEffectShadowLow::ReleaseDevice ( pd3dDevice );
	DxEffectWaterLight::ReleaseDevice ( pd3dDevice );
	DxEffectTiling::ReleaseDevice ( pd3dDevice );
	DxEffectWater::ReleaseDevice ( pd3dDevice );
	DxEffectWater2::ReleaseDevice ( pd3dDevice );
	DxEffectSpore::ReleaseDevice ( pd3dDevice );
	DxEffectSpecular::ReleaseDevice ( pd3dDevice );
	DxEffectSpecular2::ReleaseDevice ( pd3dDevice );
	DxEffectNeon::ReleaseDevice ( pd3dDevice );
	DxEffectSpecReflect::ReleaseDevice ( pd3dDevice );
	DxEffectLightMap::ReleaseDevice ( pd3dDevice );
	DxEffectGlow::ReleaseDevice ( pd3dDevice );
	DxEffectMultiTex::ReleaseDevice ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectMan::DeleteDeviceObjects()
{
	SAFE_RELEASE ( m_pBumpTexture );

	return S_OK;
}

DxEffectBase* DxEffectMan::CreateEffInstance ( DWORD TypeID )
{
	DxEffectBase* pEffectBase = NULL;

	if ( DxEffectTiling::TYPEID == TypeID )				pEffectBase = new DxEffectTiling ();
	else if ( DxEffectCloth::TYPEID == TypeID )			pEffectBase = new DxEffectCloth ();
	else if ( DxEffectDot3::TYPEID == TypeID )			pEffectBase = new DxEffectDot3 ();
	else if ( DxEffectRiver::TYPEID == TypeID )			pEffectBase = new DxEffectRiver ();
	else if ( DxEffectShadow::TYPEID == TypeID )		pEffectBase = new DxEffectShadow ();
	else if ( DxEffectShadowLow::TYPEID == TypeID )		pEffectBase = new DxEffectShadowLow ();
	else if ( DxEffectRainPoint::TYPEID == TypeID )		pEffectBase = new DxEffectRainPoint ();
	else if ( DxEffectReflect::TYPEID == TypeID )		pEffectBase = new DxEffectReflect ();
	else if ( DxEffectGrass::TYPEID == TypeID )			pEffectBase = new DxEffectGrass ();
	else if ( DxEffectNature::TYPEID == TypeID )		pEffectBase = new DxEffectNature ();
	else if ( DxEffectWaterLight::TYPEID == TypeID )	pEffectBase = new DxEffectWaterLight ();
	else if ( DxEffectRenderState::TYPEID == TypeID )	pEffectBase = new DxEffectRenderState ();
	else if ( DxEffectWater::TYPEID == TypeID )			pEffectBase = new DxEffectWater ();
	else if ( DxEffectWater2::TYPEID == TypeID )		pEffectBase = new DxEffectWater2 ();
	else if ( DxEffectSpore::TYPEID == TypeID )			pEffectBase = new DxEffectSpore ();
	else if ( DxEffectSpecular::TYPEID == TypeID )		pEffectBase = new DxEffectSpecular ();
	else if ( DxEffectSpecular2::TYPEID == TypeID )		pEffectBase = new DxEffectSpecular2 ();
	else if ( DxEffectMultiTex::TYPEID == TypeID )		pEffectBase = new DxEffectMultiTex ();
	else if ( DxEffectNeon::TYPEID == TypeID )			pEffectBase = new DxEffectNeon ();
	else if ( DxEffectSpecReflect::TYPEID == TypeID )	pEffectBase = new DxEffectSpecReflect ();
	else if ( DxEffectLightMap::TYPEID == TypeID )		pEffectBase = new DxEffectLightMap ();
	else if ( DxEffectGlow::TYPEID == TypeID )			pEffectBase = new DxEffectGlow ();

	return pEffectBase;
}

DxEffectBase* DxEffectMan::AdaptToDxFrame ( DWORD TypeID, DxFrameMesh *pFrameMesh, DxFrame *pFrame,
					LPDIRECT3DDEVICEQ pd3dDevice, PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts )
{
	HRESULT hr;

	//	Note : Effect Instance 생성.
	//
	DxEffectBase* pEffBase = CreateEffInstance ( TypeID );
	if ( !pEffBase ) return NULL;

	//	Note : Property, Affine  값을 대입.
	//
	if ( pProperty )	pEffBase->SetProperty ( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffBase->IsUseAffineMatrix() )	pEffBase->SetAffineValue ( pAffineParts );

	pEffBase->AdaptToDxFrame ( pFrame, pd3dDevice );

	hr = pEffBase->Create ( pd3dDevice );
	if ( FAILED(hr) )	goto E_ERROR;

	pFrameMesh->AddEffect ( pEffBase );

	if ( pEffBase->GetFlag () & _EFF_REPLACE )
	{
		//	Note : 알파맵 리스트에서 제거.
		//
		pFrameMesh->DelAlphaMapFrame ( pFrame );
	}

	return pEffBase;

E_ERROR:
	pEffBase->CleanUp();
	SAFE_DELETE(pEffBase);
	return NULL;
}

//	Note : 
//
HRESULT DxEffectMan::FrameMove()
{

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateBumpMapFromSurface()
// Desc: Creates a bumpmap from a surface
//-----------------------------------------------------------------------------
HRESULT DxEffectMan::CreateBumpMap( LPDIRECT3DDEVICEQ pd3dDevice )
{
    UINT iWidth  = 256;
    UINT iHeight = 256;

    // Create the bumpmap's surface and texture objects
    if( FAILED( pd3dDevice->CreateTexture( iWidth, iHeight, 1, 0,
        D3DFMT_V8U8, D3DPOOL_MANAGED, &m_pBumpTexture, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the bumpmap texels to simulate a lens
    D3DLOCKED_RECT lrDst;
    m_pBumpTexture->LockRect( 0, &lrDst, 0, 0 );
    BYTE* pDst       = (BYTE*)lrDst.pBits;

    for( DWORD y=0; y<iHeight; y++ )
    {
        for( DWORD x=0; x<iWidth; x++ )
        {
			WORD uL;
			{	// 추가
				LONG v00; // Current pixel
				v00 = 0;
				// The luminance bump value (land masses are less shiny)
				uL = ( v00>1 ) ? 63 : 127;
			}

            FLOAT fx = x/(FLOAT)iWidth  - 0.5f;
            FLOAT fy = y/(FLOAT)iHeight - 0.5f;

            CHAR iDu = (CHAR)(64*cosf(4.0f*(fx+fy)*D3DX_PI));
            CHAR iDv = (CHAR)(64*sinf(4.0f*(fx+fy)*D3DX_PI));

			if( DxEffectMan::GetInstance().GetBumpMapFormat() == D3DFMT_V8U8 )
			{
				pDst[2*x+0] = iDu;
				pDst[2*x+1] = iDv;
			}
        }
        pDst += lrDst.Pitch;
    }

    m_pBumpTexture->UnlockRect(0);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: Given a ray origin (orig) and direction (dir), and three vertices of
//       of a triangle, this function returns TRUE and the interpolated texture
//       coordinates if the ray intersects the triangle
//-----------------------------------------------------------------------------
BOOL DxEffectMan::IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return TRUE;
}