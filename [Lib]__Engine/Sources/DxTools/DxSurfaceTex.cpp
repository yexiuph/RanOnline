#include "pch.h"
#include <algorithm>

#include "./ShaderConstant.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "DxSurfaceTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD					DxSurfaceTex::VERTEX::FVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
LPDIRECT3DVERTEXBUFFERQ		DxSurfaceTex::m_pObjectVB			= NULL;
LPDIRECT3DINDEXBUFFERQ		DxSurfaceTex::m_pObjectIB			= NULL;

const DWORD					DxSurfaceTex::POINTSHADOWVERTEX		= 16;
const DWORD					DxSurfaceTex::POINTSHADOWFACES		= 18;
const float					DxSurfaceTex::EYE_TO_LOOKAT			= 220.f;

LPDIRECT3DSTATEBLOCK9		DxSurfaceTex::m_pSavedPShadowSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxSurfaceTex::m_pEffectPShadowSB	= NULL;

DxSurfaceTex& DxSurfaceTex::GetInstance()
{
	static DxSurfaceTex Instance;
	return Instance;
}

DxSurfaceTex::DxSurfaceTex() :
	m_dwUseSwShader(D3DUSAGE_SOFTWAREPROCESSING),
	m_bUseMIXED(TRUE),
	m_bUseZTexture(TRUE),
	m_pColorBuffer(NULL),
	m_pZBuffer(NULL),
	m_pShadowTex(NULL),
	m_pShadowSuf(NULL),
	m_pShadowZBuf(NULL),
	m_pGlowTex(NULL),
	m_pGlowSuf(NULL),
	m_pWaveTex(NULL),
	m_pWaveSuf(NULL),
	m_pReflectTex(NULL),
	m_pReflectSuf(NULL),
	m_pReflectZBuf(NULL),
	m_pFlowTex(NULL),
	m_pFlowSuf(NULL),
	m_pGlowTex_SRC(NULL),
	m_pGlowSuf_SRC(NULL),
	m_pGlowTex_TEMP(NULL),
	m_pGlowSuf_TEMP(NULL),
	m_pTempTex512(NULL),
	m_pTempSuf512(NULL),
	m_pTempTex256(NULL),
	m_pTempSuf256(NULL),
	m_pTempTex128(NULL),
	m_pTempSuf128(NULL),
	m_pTempTex64(NULL),
	m_pTempSuf64(NULL),
	m_pBurnResultTex(NULL),
	m_pBurnResultSuf(NULL),
	m_pBurnNewTex(NULL),
	m_pBurnNewSuf(NULL),

	m_dwWidth(512),
	m_dwHeight(512),
	m_cMainFormat(D3DFMT_A8R8G8B8),
	m_cColorFormat(D3DFMT_X1R5G5B5),
	m_cZFormat(D3DFMT_D16),

	m_bDeviceEnable(TRUE),
	m_bOptionEnable(TRUE),
	m_bEnable(FALSE),
	m_bFlowOneDraw(FALSE),
	m_fTime(0.f),
	m_nNum(0),

	m_GRAPHIC_CARD( EMGC_DEFAULT )
{
}

DxSurfaceTex::~DxSurfaceTex()
{
}

HRESULT DxSurfaceTex::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	HRESULT	hr = S_OK;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );

	D3DADAPTER_IDENTIFIER9 sAdapter;
	pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &sAdapter );

	std::string	strName = sAdapter.Description;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

	std::string::size_type idx;
	
	idx = strName.find( "savage" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_SAVAGE_ETC;

	idx = strName.find( "82810e" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_INTEL_82810E;
	idx = strName.find( "82845g" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_INTEL_82845G;
	idx = strName.find( "intel" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_INTEL_ETC;

	
	idx = strName.find( "tnt" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_RIVA_TNT2;
	idx = strName.find( "geforce 2" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_GEFORCE2_MX;
	idx = strName.find( "geforce2" );
	if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_GEFORCE2_MX;

	idx = strName.find( "geforce 4" );
	if( idx!=std::string::npos )
	{
		idx = strName.find( "440" );
		if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_GEFORCE4_MX_440;
	}
	idx = strName.find( "geforce4" );
	if( idx!=std::string::npos )
	{
		idx = strName.find( "440" );
		if( idx!=std::string::npos )	m_GRAPHIC_CARD = EMGC_GEFORCE4_MX_440;
	}

	// Check : 초반 Loading시 에러나는 문제가 있다.
	//			나중에 이것을 찾아 봐야 한다.
	idx = strName.find( "igp" );
	if( idx!=std::string::npos )		m_GRAPHIC_CARD = EMGC_RADEON_IGP;

	
	


	
	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		m_bUseMIXED = FALSE;
	}

	////	Note : 그래픽 카드 성능 보고 텍스쳐 선택 
	////
	//m_dwTexDetail = (d3dCaps.MaxTextureWidth < m_dwTexDetail) ? d3dCaps.MaxTextureWidth : m_dwTexDetail;
	//m_dwTexDetail = (d3dCaps.MaxTextureHeight < m_dwTexDetail) ? d3dCaps.MaxTextureHeight : m_dwTexDetail;

	//	Note : Vertex Shader
	//
	if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 0) )
	{
		m_dwUseSwShader = 0;	// 이럴 땐 MIXED -> H/W 가속 이다.
	}

	//	Note : Z 버퍼에 Texture를 사용 할 수 있나 없나를 테스트 하고 
	//			사용가능하면 TRUE 를 불가능 하면 FALSE 를 호출한다.
	//
	hr = pD3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
									d3dCaps.DeviceType, d3dsdBackBuffer.Format,
									D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE,
									D3DFMT_D24S8 );
	if ( FAILED(hr) )	m_bUseZTexture = FALSE;

    return hr;
}

HRESULT DxSurfaceTex::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT	hr = S_OK;

	// Note :StretchRect Enable Test
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	if( !(d3dCaps.DevCaps2&D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES) )	m_bDeviceEnable = FALSE;
	if( !(d3dCaps.StretchRectFilterCaps&D3DPTFILTERCAPS_MINFPOINT) )	m_bDeviceEnable = FALSE;

	DWORD	dwCount;
	hr = pd3dDevice->CreateVertexBuffer ( POINTSHADOWVERTEX*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &m_pObjectVB, NULL );
	if( FAILED(hr) )	return E_FAIL;

	if ( m_pObjectVB )
	{
		VERTEX*	pVertex;
		hr = m_pObjectVB->Lock ( 0, 0, (VOID**)&pVertex, 0 );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxSurfaceTex::RestoreDeviceObjects() -- m_pObjectVB->Lock() -- Failed" );
			return E_FAIL;
		}

		for ( DWORD i=0; i<4; i++ )
		{
			for ( DWORD j=0; j<4; j++ )
			{
				dwCount = (i*4)+j;
				if ( (i==0) || (i==3) || (j==0) || (j==3) )		pVertex[dwCount].cColor = 0xff000000;
				else											pVertex[dwCount].cColor = 0xffffffff;
			}
		}

		m_pObjectVB->Unlock ();
	}

	hr = pd3dDevice->CreateIndexBuffer ( POINTSHADOWFACES*3*sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pObjectIB, NULL );
	if( FAILED(hr) )	return E_FAIL;
 
	if ( m_pObjectIB )
	{
		WORD*	pIndices;
		hr = m_pObjectIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxSurfaceTex::RestoreDeviceObjects() -- m_pObjectIB->Lock() -- Failed" );
			return E_FAIL;
		}

		for ( DWORD	i=0; i<3; i++ )
		{
			for ( DWORD	j=0; j<3; j++ )
			{
				*pIndices++ = (WORD)((4*(0+i))+0+j);
				*pIndices++ = (WORD)((4*(0+i))+1+j);
				*pIndices++ = (WORD)((4*(1+i))+0+j);

				*pIndices++ = (WORD)((4*(0+i))+1+j);
				*pIndices++ = (WORD)((4*(1+i))+1+j);
				*pIndices++ = (WORD)((4*(1+i))+0+j);
			}
		}
		m_pObjectIB->Unlock ();
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZENABLE,					FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,				FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,				D3DCULL_CW );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,				D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,		TRUE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_CLAMP );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedPShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectPShadowSB );
	}

	LPDIRECT3DSURFACEQ pBackBuffer;
	D3DSURFACE_DESC		d3dsdBackBuffer;

	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

    pBackBuffer->GetDesc( &d3dsdBackBuffer );
	m_dwWidth		= d3dsdBackBuffer.Width;
	m_dwHeight		= d3dsdBackBuffer.Height;
	m_cMainFormat	= d3dsdBackBuffer.Format;
    pBackBuffer->Release();

	m_cZFormat		= D3DFMT_D16;

	//	Note : 그림자 만들기 전 셋팅 시작~!!
	hr = pd3dDevice->GetRenderTarget( 0, &m_pColorBuffer );
	if( FAILED(hr) )	goto _RETURN;
	hr = pd3dDevice->GetDepthStencilSurface(&m_pZBuffer);
	if( FAILED(hr) )	goto _RETURN;

	//	Note : 임시 텍스쳐 만들기 - 64	( 안 될 경우 컬러 포멧을 바꾸어 준다. )
	m_cColorFormat	= D3DFMT_X1R5G5B5;
	if( FAILED ( pd3dDevice->CreateTexture( 64, 64, 1, D3DUSAGE_RENDERTARGET,				// 1. X1R5G5B5
											m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex64, NULL ) ) )
	{
		m_cColorFormat = D3DFMT_A1R5G5B5;
		if( FAILED ( pd3dDevice->CreateTexture( 64, 64, 1, D3DUSAGE_RENDERTARGET,			// 2. A1R5G5B5
											m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex64, NULL ) ) )	
		{
			m_cColorFormat = D3DFMT_A4R4G4B4;
			if( FAILED ( pd3dDevice->CreateTexture( 64, 64, 1, D3DUSAGE_RENDERTARGET, 		// 3. A4R4G4B4
											m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex64, NULL ) ) )
			{
				m_cColorFormat = m_cMainFormat;
				hr = pd3dDevice->CreateTexture( 64, 64, 1, D3DUSAGE_RENDERTARGET,			// 4. 현재 Format
											m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex64, NULL );
				if( FAILED(hr) )	goto _RETURN;
			}
		}
	}

	hr = m_pTempTex64->GetSurfaceLevel( 0, &m_pTempSuf64 );
	if( FAILED(hr) )	goto _RETURN;

	//	Flow 텍스쳐
	hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pFlowTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pFlowTex->GetSurfaceLevel( 0, &m_pFlowSuf );
	if( FAILED(hr) )	goto _RETURN;

	//	임시 128 이미지 만들기
	hr = pd3dDevice->CreateTexture ( 128, 128, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex128, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pTempTex128->GetSurfaceLevel( 0, &m_pTempSuf128 );
	if( FAILED(hr) )	goto _RETURN;

	//	 텍스쳐 만들기 - 256
	hr = pd3dDevice->CreateTexture( 256, 256, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex256, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pTempTex256->GetSurfaceLevel( 0, &m_pTempSuf256 );
	if( FAILED(hr) )	goto _RETURN;

	//	Note : Wave 텍스쳐 만들기 - 512
	hr = pd3dDevice->CreateTexture( 512, 512, 1, D3DUSAGE_RENDERTARGET, m_cMainFormat, D3DPOOL_DEFAULT, &m_pWaveTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pWaveTex->GetSurfaceLevel( 0, &m_pWaveSuf );
	if( FAILED(hr) )	goto _RETURN;

	//	Note : 그림자 텍스쳐 만들기 - 512
	hr = pd3dDevice->CreateTexture( 512, 512, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pShadowTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pShadowTex->GetSurfaceLevel( 0, &m_pShadowSuf );
	if( FAILED(hr) )	goto _RETURN;
	hr = pd3dDevice->CreateDepthStencilSurface( 512, 512, m_cZFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowZBuf, NULL );
	if( FAILED(hr) )	goto _RETURN;

	//	Note : 임시 텍스쳐 만들기 - 512
	hr = pd3dDevice->CreateTexture( 512, 512, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pTempTex512, NULL );
	if( FAILED(hr) )	goto _RETURN; 
	hr = m_pTempTex512->GetSurfaceLevel( 0, &m_pTempSuf512 );
	if( FAILED(hr) )	goto _RETURN;

	if( m_GRAPHIC_CARD == EMGC_SAVAGE_ETC )			goto _RETURN;	// 뒤에 것들은 상하 비율이 안 맞아서 속도가 느리게 되어 버린다. 메모리 상 문제도
	if( m_GRAPHIC_CARD == EMGC_INTEL_82810E )		goto _RETURN;	// 뒤에 것들은 상하 비율이 안 맞아서 속도가 느리게 되어 버린다. 메모리 상 문제도
	if( m_GRAPHIC_CARD == EMGC_INTEL_82845G )		goto _RETURN;	// 뒤에 것들은 상하 비율이 안 맞아서 속도가 느리게 되어 버린다.	메모리 상 문제도
	if( m_GRAPHIC_CARD == EMGC_INTEL_ETC )			goto _RETURN;	// 뒤에 것들은 상하 비율이 안 맞아서 속도가 느리게 되어 버린다.	메모리 상 문제도
	if( m_GRAPHIC_CARD == EMGC_RIVA_TNT2 )			goto _RETURN;	// 뒤에 것들은 상하 비율이 안 맞아서 속도가 느리게 되어 버린다.	메모리 상 문제도
	if( m_GRAPHIC_CARD == EMGC_RADEON_IGP )			goto _RETURN;
	//if( m_GRAPHIC_CARD == EMGC_GEFORCE2_MX )		goto _RETURN;
	//if( m_GRAPHIC_CARD == EMGC_GEFORCE4_MX_440 )	goto _RETURN;
	

	
	
	//	Note : Reflect 텍스쳐 만들기 - 512
	//			384로 써 봤지만 일부 그래픽 카드에서 범프 사용시 잘못 나오는 버그가 있어서 사용 불가.
	//
	hr = pd3dDevice->CreateTexture( 512, 512, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pReflectTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pReflectTex->GetSurfaceLevel( 0, &m_pReflectSuf );
	if( FAILED(hr) )	goto _RETURN;
	hr = pd3dDevice->CreateDepthStencilSurface( 512, 512, m_cZFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pReflectZBuf, NULL );
	if( FAILED(hr) )	goto _RETURN;

	//	 Note : Glow 임시 텍스쳐 만들기 - 512,384
	hr = pd3dDevice->CreateTexture( 512, 384, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pGlowTex_TEMP, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pGlowTex_TEMP->GetSurfaceLevel( 0, &m_pGlowSuf_TEMP );
	if( FAILED(hr) )	goto _RETURN;
	pd3dDevice->SetRenderTarget( 0, m_pGlowSuf_TEMP );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	//	Note : Glow 텍스쳐 만들기 - 512,384
	hr = pd3dDevice->CreateTexture( 512, 384, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pGlowTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pGlowTex->GetSurfaceLevel( 0, &m_pGlowSuf );
	if( FAILED(hr) )	goto _RETURN;
	pd3dDevice->SetRenderTarget( 0, m_pGlowSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	//	Note : Glow Src Make
	hr = pd3dDevice->CreateTexture( m_dwWidth, m_dwHeight, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pGlowTex_SRC, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pGlowTex_SRC->GetSurfaceLevel( 0, &m_pGlowSuf_SRC );
	if( FAILED(hr) )	goto _RETURN;
	pd3dDevice->SetRenderTarget( 0, m_pGlowSuf_SRC );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	//	Note : 글로우 결과 텍스쳐 만들기
	//
	hr = pd3dDevice->CreateTexture ( 256, 256, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pBurnResultTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pBurnResultTex->GetSurfaceLevel( 0, &m_pBurnResultSuf );
	if( FAILED(hr) )	goto _RETURN;
	pd3dDevice->SetRenderTarget( 0, m_pBurnResultSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	//	Note : 글로우 New 텍스쳐 만들기
	//
	hr = pd3dDevice->CreateTexture ( m_dwWidth, m_dwHeight, 1, D3DUSAGE_RENDERTARGET, m_cColorFormat, D3DPOOL_DEFAULT, &m_pBurnNewTex, NULL );
	if( FAILED(hr) )	goto _RETURN;
	hr = m_pBurnNewTex->GetSurfaceLevel( 0, &m_pBurnNewSuf );
	if( FAILED(hr) )	goto _RETURN;
	pd3dDevice->SetRenderTarget( 0, m_pBurnNewSuf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );

	pd3dDevice->SetRenderTarget( 0, m_pColorBuffer );
	pd3dDevice->SetDepthStencilSurface( m_pZBuffer );

	// Note : StretchRect Fail.. Release Surface and Texture
	if( !m_bDeviceEnable )
	{
		SAFE_RELEASE( m_pWaveTex );
		SAFE_RELEASE( m_pWaveSuf );
	}

	return S_OK;

_RETURN:
	m_bDeviceEnable = FALSE;
	SAFE_RELEASE( m_pWaveTex );	
	SAFE_RELEASE( m_pWaveSuf );

	pd3dDevice->SetRenderTarget ( 0, m_pColorBuffer );
	pd3dDevice->SetDepthStencilSurface( m_pZBuffer );



	return S_OK;
}

HRESULT DxSurfaceTex::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pObjectVB );
	SAFE_RELEASE( m_pObjectIB );	

	SAFE_RELEASE(m_pColorBuffer);
	SAFE_RELEASE(m_pZBuffer);

	SAFE_RELEASE(m_pShadowTex);
	SAFE_RELEASE(m_pShadowSuf);
	SAFE_RELEASE(m_pShadowZBuf);

	SAFE_RELEASE(m_pWaveTex);
	SAFE_RELEASE(m_pWaveSuf);

	SAFE_RELEASE(m_pReflectTex);
	SAFE_RELEASE(m_pReflectSuf);
	SAFE_RELEASE(m_pReflectZBuf);

	SAFE_RELEASE(m_pGlowTex_TEMP);
	SAFE_RELEASE(m_pGlowSuf_TEMP);

	SAFE_RELEASE(m_pGlowTex);
	SAFE_RELEASE(m_pGlowSuf);

	SAFE_RELEASE(m_pFlowTex);
	SAFE_RELEASE(m_pFlowSuf);

	SAFE_RELEASE(m_pTempTex512);
	SAFE_RELEASE(m_pTempSuf512);

	SAFE_RELEASE(m_pTempTex256);
	SAFE_RELEASE(m_pTempSuf256);

	SAFE_RELEASE(m_pTempTex128);
	SAFE_RELEASE(m_pTempSuf128);

	SAFE_RELEASE(m_pTempTex64);
	SAFE_RELEASE(m_pTempSuf64);

	SAFE_RELEASE(m_pGlowTex_SRC);
	SAFE_RELEASE(m_pGlowSuf_SRC);

	SAFE_RELEASE(m_pBurnResultTex);
	SAFE_RELEASE(m_pBurnResultSuf);

	SAFE_RELEASE(m_pBurnNewTex);
	SAFE_RELEASE(m_pBurnNewSuf);


	SAFE_RELEASE( m_pSavedPShadowSB );
	SAFE_RELEASE( m_pEffectPShadowSB );

	return S_OK;
}

HRESULT DxSurfaceTex::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT DxSurfaceTex::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime += fElapsedTime;

	m_bFlowOneDraw = FALSE;



	m_bEnable = FALSE;	// 초기화

	if( !m_pWaveTex )								return S_OK;
	if( !m_bOptionEnable || !m_bDeviceEnable )		return S_OK;

	m_bEnable = TRUE;	// 활성화

	return S_OK;
}

void DxSurfaceTex::InitViewProjection ( D3DXVECTOR3& vBaseViewMax, D3DXVECTOR3& vBaseViewMin, D3DXMATRIX& matProj, const float fDistance, 
										const float fTexWidth, const float fTexHeight )
{
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3	vMax( 10000.f, 0.f, 10000.f );
	D3DXVECTOR3	vMin( -10000.f, 0.f, -10000.f );

	//	Note : 평면과 카메라 CV의 만나는 점 구하기.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vCenter = (vMax+vMin) * 0.5f;

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

// 예전에는 이것으로만 하고 밑에건 없었다.
//	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 100000.f, (float)dwTexDetail, (float)dwTexDetail, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 100000.f;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fTexWidth/fTexHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : 면 만들기
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( vMax.x, vCenter.y, vMin.z );
	vPosition[1] = D3DXVECTOR3 ( vMin.x, vCenter.y, vMax.z );
	vPosition[2] = D3DXVECTOR3 ( vMax.x, vCenter.y, vMax.z );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : 점 만들기
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	vViewMax = vViewMin = vPosition[0];

	vViewMax.x = ( vViewMax.x > vPosition[0].x ) ? vViewMax.x : vPosition[0].x;
	vViewMax.x = ( vViewMax.x > vPosition[1].x ) ? vViewMax.x : vPosition[1].x;
	vViewMax.x = ( vViewMax.x > vPosition[2].x ) ? vViewMax.x : vPosition[2].x;
	vViewMax.x = ( vViewMax.x > vPosition[3].x ) ? vViewMax.x : vPosition[3].x;

	vViewMax.y = ( vViewMax.y > vPosition[0].y ) ? vViewMax.y : vPosition[0].y;
	vViewMax.y = ( vViewMax.y > vPosition[1].y ) ? vViewMax.y : vPosition[1].y;
	vViewMax.y = ( vViewMax.y > vPosition[2].y ) ? vViewMax.y : vPosition[2].y;
	vViewMax.y = ( vViewMax.y > vPosition[3].y ) ? vViewMax.y : vPosition[3].y;

	vViewMax.z = ( vViewMax.z > vPosition[0].z ) ? vViewMax.z : vPosition[0].z;
	vViewMax.z = ( vViewMax.z > vPosition[1].z ) ? vViewMax.z : vPosition[1].z;
	vViewMax.z = ( vViewMax.z > vPosition[2].z ) ? vViewMax.z : vPosition[2].z;
	vViewMax.z = ( vViewMax.z > vPosition[3].z ) ? vViewMax.z : vPosition[3].z;

	vViewMin.x = ( vViewMin.x < vPosition[0].x ) ? vViewMin.x : vPosition[0].x;
	vViewMin.x = ( vViewMin.x < vPosition[1].x ) ? vViewMin.x : vPosition[1].x;
	vViewMin.x = ( vViewMin.x < vPosition[2].x ) ? vViewMin.x : vPosition[2].x;
	vViewMin.x = ( vViewMin.x < vPosition[3].x ) ? vViewMin.x : vPosition[3].x;

	vViewMin.y = ( vViewMin.y < vPosition[0].y ) ? vViewMin.y : vPosition[0].y;
	vViewMin.y = ( vViewMin.y < vPosition[1].y ) ? vViewMin.y : vPosition[1].y;
	vViewMin.y = ( vViewMin.y < vPosition[2].y ) ? vViewMin.y : vPosition[2].y;
	vViewMin.y = ( vViewMin.y < vPosition[3].y ) ? vViewMin.y : vPosition[3].y;

	vViewMin.z = ( vViewMin.z < vPosition[0].z ) ? vViewMin.z : vPosition[0].z;
	vViewMin.z = ( vViewMin.z < vPosition[1].z ) ? vViewMin.z : vPosition[1].z;
	vViewMin.z = ( vViewMin.z < vPosition[2].z ) ? vViewMin.z : vPosition[2].z;
	vViewMin.z = ( vViewMin.z < vPosition[3].z ) ? vViewMin.z : vPosition[3].z;

	vBaseViewMax		= D3DXVECTOR3 ( vViewMax.x, vCenter.y, vViewMax.z );
	vBaseViewMin		= D3DXVECTOR3 ( vViewMin.x, vCenter.y, vViewMin.z );

	D3DXMatrixPerspectiveFovLH ( &matProj, D3DX_PI/4, fTexWidth/fTexHeight, 0.5f, 10000.f);	// 프로젝션 만들고
}

void DxSurfaceTex::SetViewProjection ( D3DXVECTOR3& _vViewMax, D3DXVECTOR3& _vViewMin, D3DXMATRIX& _matView, D3DXMATRIX& _matProj, 
									  DWORD dwTexDetail, const float _fDistance, const float _fHeight )
{
	const D3DXVECTOR3 &vLookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	const D3DXVECTOR3 &vFromPt		= DxViewPort::GetInstance().GetFromPt ();

	D3DXVECTOR3	vMax( 10000.f, _fHeight, 10000.f );
	D3DXVECTOR3	vMin( -10000.f, _fHeight, -10000.f );

	//	Note : 평면과 카메라 CV의 만나는 점 구하기.
	//
	D3DXVECTOR3		vViewMax, vViewMin;
	D3DXVECTOR3		vTempFromPt;
	D3DXVECTOR3		vTempLookatPt	= D3DXVECTOR3 ( 0.f, _fHeight, 0.f );
	D3DXVECTOR3		vUpVec ( 0.f, 0.f, -1.f );;
	D3DXVECTOR3		vPosition[4];

	vTempFromPt		= vTempLookatPt;
	vTempFromPt.y	+= _fDistance;

//	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vTempLookatPt, vTempFromPt, vUpVec, 0.5f, 100000.f, (float)dwTexDetail, (float)dwTexDetail, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 100000.f;
	float	fWidth		= (float)dwTexDetail;
	float	fHeight		= (float)dwTexDetail;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vTempLookatPt - vTempFromPt;
	vUp = vUpVec;
	vPos = vTempFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fWidth/fHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : 면 만들기
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( vMax.x, vMin.y, vMin.z );
	vPosition[1] = D3DXVECTOR3 ( vMin.x, vMin.y, vMax.z );
	vPosition[2] = D3DXVECTOR3 ( vMax.x, vMin.y, vMax.z );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : 점 만들기
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	vViewMax = vViewMin = vPosition[0];

	vViewMax.x = ( vViewMax.x > vPosition[0].x ) ? vViewMax.x : vPosition[0].x;
	vViewMax.x = ( vViewMax.x > vPosition[1].x ) ? vViewMax.x : vPosition[1].x;
	vViewMax.x = ( vViewMax.x > vPosition[2].x ) ? vViewMax.x : vPosition[2].x;
	vViewMax.x = ( vViewMax.x > vPosition[3].x ) ? vViewMax.x : vPosition[3].x;

	vViewMax.y = ( vViewMax.y > vPosition[0].y ) ? vViewMax.y : vPosition[0].y;
	vViewMax.y = ( vViewMax.y > vPosition[1].y ) ? vViewMax.y : vPosition[1].y;
	vViewMax.y = ( vViewMax.y > vPosition[2].y ) ? vViewMax.y : vPosition[2].y;
	vViewMax.y = ( vViewMax.y > vPosition[3].y ) ? vViewMax.y : vPosition[3].y;

	vViewMax.z = ( vViewMax.z > vPosition[0].z ) ? vViewMax.z : vPosition[0].z;
	vViewMax.z = ( vViewMax.z > vPosition[1].z ) ? vViewMax.z : vPosition[1].z;
	vViewMax.z = ( vViewMax.z > vPosition[2].z ) ? vViewMax.z : vPosition[2].z;
	vViewMax.z = ( vViewMax.z > vPosition[3].z ) ? vViewMax.z : vPosition[3].z;

	vViewMin.x = ( vViewMin.x < vPosition[0].x ) ? vViewMin.x : vPosition[0].x;
	vViewMin.x = ( vViewMin.x < vPosition[1].x ) ? vViewMin.x : vPosition[1].x;
	vViewMin.x = ( vViewMin.x < vPosition[2].x ) ? vViewMin.x : vPosition[2].x;
	vViewMin.x = ( vViewMin.x < vPosition[3].x ) ? vViewMin.x : vPosition[3].x;

	vViewMin.y = ( vViewMin.y < vPosition[0].y ) ? vViewMin.y : vPosition[0].y;
	vViewMin.y = ( vViewMin.y < vPosition[1].y ) ? vViewMin.y : vPosition[1].y;
	vViewMin.y = ( vViewMin.y < vPosition[2].y ) ? vViewMin.y : vPosition[2].y;
	vViewMin.y = ( vViewMin.y < vPosition[3].y ) ? vViewMin.y : vPosition[3].y;

	vViewMin.z = ( vViewMin.z < vPosition[0].z ) ? vViewMin.z : vPosition[0].z;
	vViewMin.z = ( vViewMin.z < vPosition[1].z ) ? vViewMin.z : vPosition[1].z;
	vViewMin.z = ( vViewMin.z < vPosition[2].z ) ? vViewMin.z : vPosition[2].z;
	vViewMin.z = ( vViewMin.z < vPosition[3].z ) ? vViewMin.z : vPosition[3].z;

	D3DXVECTOR3	vCenter			= vLookatPt;
	D3DXVECTOR3	vLightDirect	= vLookatPt - vFromPt;

	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	vCenter.x  = vCenter.x + (vLightDirect.x*50.f);
	vCenter.y  = vCenter.y;
	vCenter.z  = vCenter.z + (vLightDirect.z*50.f);

	_vViewMax		= D3DXVECTOR3 ( vViewMax.x+vCenter.x, vViewMin.y, vViewMax.z+vCenter.z );
	_vViewMin		= D3DXVECTOR3 ( vViewMin.x+vCenter.x, vViewMin.y, vViewMin.z+vCenter.z );

	vTempLookatPt	= D3DXVECTOR3 ( vCenter.x, _fHeight, vCenter.z );
	vTempFromPt		= vTempLookatPt;
	vTempFromPt.y	+= _fDistance;

	D3DXMatrixLookAtLH ( &_matView, &vTempFromPt, &vTempLookatPt, &vUpVec );		// 뷰 만들고..
	D3DXMatrixPerspectiveFovLH ( &_matProj, D3DX_PI/4, 1.f, 0.5f, 10000.f);	// 프로젝션 만들고
}

HRESULT	DxSurfaceTex::RenderPointShadow ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vViewMax, D3DXVECTOR3 vViewMin, D3DXMATRIX matLastVP, DWORD dwTexDetail )
{
	HRESULT	hr = S_OK;

	//	Note : m_matLastWVP 이 값을 알아내기 위하여 이것을 한다.
	//
	D3DXMATRIX texMat, matIdentity;
	D3DXMatrixIdentity(&texMat);
	D3DXMatrixIdentity(&matIdentity);

	//set special texture matrix for shadow mapping
	unsigned int range;
	float fOffsetX = 0.5f + (0.5f / (float)dwTexDetail);
	float fOffsetY = 0.5f + (0.5f / (float)dwTexDetail);

	range = 0xFFFFFFFF >> (32 - 16);

	float fBias    = -0.001f * (float)range;
	D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
								0.0f,     -0.5f,     0.0f,         0.0f,
								0.0f,     0.0f,     (float)range, 0.0f,
								fOffsetX, fOffsetY, fBias,        1.0f );

	D3DXMatrixMultiply(&texMat, &matLastVP, &texScaleBiasMat);
 
	D3DXMATRIX	matView, matProj, matWVP;
	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

	VERTEX*			pVert;
	DWORD			fTempI, fTempJ;
	D3DXVECTOR4		pD;
	D3DXVECTOR3		vDIsplay;
	D3DXVECTOR3		vTemp;
	D3DXVECTOR3		vDelta = (vViewMax-vViewMin) / 5.f;
	DWORD			dwCount;
	D3DXMATRIX		matDirect = texMat;
	hr = m_pObjectVB->Lock ( 0, 0, (VOID**)&pVert, 0 );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxSurfaceTex::RenderPointShadow() -- m_pObjectVB->Lock() -- Failed" );
		return E_FAIL;
	}

	for ( DWORD i=0; i<4; i++ )
	{
		for ( DWORD j=0; j<4; j++ )
		{
			dwCount = (i*4)+j;

			if (i>=2)	fTempI = i+3;		// 가상으로 늘인것에 대한 대책 
			else		fTempI = i;
			if (j>=2)	fTempJ = j+3;
			else		fTempJ = j;

			pVert[dwCount].vPos = D3DXVECTOR3 ( vViewMin.x + (vDelta.x*fTempJ), vViewMin.y, vViewMin.z + (vDelta.z*fTempI) );
		}
	}
	m_pObjectVB->Unlock ();

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	////
	//// 텍스쳐 행렬
	////
	//// [그림자 수신 객체 좌표] -> [텍스쳐 좌표]변환을 만든다.
	////
	//// 버텍스 쉐이더를 쓴다면 행렬곱 한방으로 끝.
	//// matLocalToWorld = 월드변환행렬(그림자 수신객체);
	//// matShadowUV = matLocalToWorld * matWorldToLight * matShadowProj * (scale * offset)
	//// c5,6,7,8에 matShadowUV를 넣음
	//// m4x4 oT1, v0, c5
	//// 
	//// 그런데...버텍스 쉐이더를 안쓰려면 좀 다르다.
	//// D3D가 텍스쳐 행렬에 입력으로 넣어 주는것이 [카메라 좌표]이기때문.
	//// 입력이 다르니 행렬도 조금 다르다.
	//// matShadowUV = matCameraToWorld * matWorldToLight * matShadowProj * (scale * offset)
	//// SetTransform( &matShadowUV )
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//D3DXMATRIX matShadowUV; // [그림자 수신 객체 좌표] -> [텍스쳐 좌표]

 //   D3DXMATRIX matCameraToWorld;// [카메라 좌표] -> [월드 좌표]
	//D3DXMatrixInverse( &matCameraToWorld, NULL, &matView );
	//
	//// [그림자 객체 좌표] -> [텍스쳐 좌표]변환(matShadowProj)은
	//// 공간좌표를 -1 ~ +1 공간으로 변환했었지만,
	//// 이번엔 0 ~ +1 공간으로 변환해야 하기때문에 
	//// 0.5로 스케일링한뒤 0.5만큼 오프셋을 걸어주어야 함.
	//D3DXMATRIX scale,offset;
	//D3DXMatrixScaling( &scale, 0.5f, -0.5f, 1.0f );
	//D3DXMatrixTranslation( &offset, 0.5f, 0.5f, 0.0f );

	//D3DXMatrixMultiply( &matShadowUV, &matCameraToWorld, &matWorldToLight );
	//D3DXMatrixMultiply( &matShadowUV, &matShadowUV, &matShadowProj );
	//D3DXMatrixMultiply( &matShadowUV, &matShadowUV, &scale );
	//D3DXMatrixMultiply( &matShadowUV, &matShadowUV, &offset );

	//// 카메라 좌표계의 버텍스좌표를 텍스쳐 행렬의 입력으로 사용함.
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
	//// 동차좌표계 텍스쳐 변환 행렬을 사용하도록 설정해준다. 
	//// 이렇게 해야 D3D가 투영 나눗셈(projection divide)연산을 한다.
	//pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4|D3DTTFF_PROJECTED );
	//// 텍스쳐 행렬 설정.
	//pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matShadowUV );

	m_pSavedPShadowSB->Capture();
	m_pEffectPShadowSB->Apply();

	DxSetTextureMatrix ( pd3dDevice, 0, matLastVP );

	pd3dDevice->SetFVF ( VERTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pObjectVB, 0, sizeof(VERTEX) );
	pd3dDevice->SetIndices ( m_pObjectIB );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, POINTSHADOWVERTEX, 0, POINTSHADOWFACES );

	DxResetTextureMatrix ( pd3dDevice, 0 );

	m_pSavedPShadowSB->Apply();

	return hr;
}

////-----------------------------------------------------------------------------
//// Name: InitBumpMap()
//// Desc: Converts data from m_pHeightTex into m_psBumpMap.
////-----------------------------------------------------------------------------
//HRESULT DxSurfaceTex::InitBumpMap( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ psBumpSrc )
//{
//    D3DSURFACE_DESC    d3dsd;
//    D3DLOCKED_RECT     d3dlr;
//
//    psBumpSrc->GetLevelDesc( 0, &d3dsd );
//
//	SAFE_RELEASE ( m_pWaterBumpTex );
//
//    // Create the bumpmap's surface and texture objects
//    if( FAILED( pd3dDevice->CreateTexture( d3dsd.Width, d3dsd.Height, 1, 0, 
//        D3DFMT_V8U8, D3DPOOL_MANAGED, &m_pWaterBumpTex ) ) )
//    {
//        return E_FAIL;
//    }
//
//    // Fill the bits of the new texture surface with bits from
//    // a private format.
//    psBumpSrc->LockRect( 0, &d3dlr, 0, 0 );
//    DWORD dwSrcPitch = (DWORD)d3dlr.Pitch;
//    BYTE* pSrcTopRow = (BYTE*)d3dlr.pBits;
//    BYTE* pSrcCurRow = pSrcTopRow;
//    BYTE* pSrcBotRow = pSrcTopRow + (dwSrcPitch * (d3dsd.Height - 1) );
//
//    m_pWaterBumpTex->LockRect( 0, &d3dlr, 0, 0 );
//    DWORD dwDstPitch = (DWORD)d3dlr.Pitch;
//    BYTE* pDstTopRow = (BYTE*)d3dlr.pBits;
//    BYTE* pDstCurRow = pDstTopRow;
//    BYTE* pDstBotRow = pDstTopRow + (dwDstPitch * (d3dsd.Height - 1) );
//
//    for( DWORD y=0; y<d3dsd.Height; y++ )
//    {
//        BYTE* pSrcB0; // addr of current pixel
//        BYTE* pSrcB1; // addr of pixel below current pixel, wrapping to top if necessary
//        BYTE* pSrcB2; // addr of pixel above current pixel, wrapping to bottom if necessary
//        BYTE* pDstT;  // addr of dest pixel;
//
//        pSrcB0 = pSrcCurRow;
//
//        if( y == d3dsd.Height - 1)
//            pSrcB1 = pSrcTopRow;
//        else
//            pSrcB1 = pSrcCurRow + dwSrcPitch;
//
//        if( y == 0 )
//            pSrcB2 = pSrcBotRow;
//        else
//            pSrcB2 = pSrcCurRow - dwSrcPitch;
//
//        pDstT = pDstCurRow;
//
//        for( DWORD x=0; x<d3dsd.Width; x++ )
//        {
//            LONG v00; // Current pixel
//            LONG v01; // Pixel to the right of current pixel, wrapping to left edge if necessary
//            LONG vM1; // Pixel to the left of current pixel, wrapping to right edge if necessary
//            LONG v10; // Pixel one line below.
//            LONG v1M; // Pixel one line above.
//
//            v00 = *(pSrcB0+0);
//            
//            if( x == d3dsd.Width - 1 )
//                v01 = *(pSrcCurRow);
//            else
//                v01 = *(pSrcB0+4);
//            
//            if( x == 0 )
//                vM1 = *(pSrcCurRow + (4 * (d3dsd.Width - 1)));
//            else
//                vM1 = *(pSrcB0-4);
//            v10 = *(pSrcB1+0);
//            v1M = *(pSrcB2+0);
//
//            LONG iDu = (vM1-v01); // The delta-u bump value
//            LONG iDv = (v1M-v10); // The delta-v bump value
// 
//       
//            *pDstT++ = (BYTE)(iDu / 2);
//            *pDstT++ = (BYTE)(iDv / 2);
//                   
//
//            // Move one pixel to the right (src is 32-bpp)
//            pSrcB0+=4;
//            pSrcB1+=4;
//            pSrcB2+=4;
//        }
//
//        // Move to the next line
//        pSrcCurRow += dwSrcPitch;
//        pDstCurRow += dwDstPitch;
//    }
//
//    m_pWaterBumpTex->UnlockRect(0);
//    psBumpSrc->UnlockRect(0);
//
//    return S_OK;
//}