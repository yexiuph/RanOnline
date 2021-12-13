#include "pch.h"

#include "glperiod.h"

#include "./DxSkinChar.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "dxshadowmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxShadowMap& DxShadowMap::GetInstance()
{
	static DxShadowMap Instance;
	return Instance;
}

const DWORD					DxShadowMap::VERTEX::FVF			= D3DFVF_XYZ|D3DFVF_TEX1;
const float					DxShadowMap::m_fRange				= 12.f;
const float					DxShadowMap::EYE_TO_LOOKAT			= 220.f;
LPDIRECT3DVERTEXBUFFERQ		DxShadowMap::m_pShadowVB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedStateBlock		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pEffectStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedCircleSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pEffectCircleSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pSavedOneShadowSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxShadowMap::m_pDrawOneShadowSB		= NULL;

DxShadowMap::DxShadowMap() :
	m_dwUseSwShader(D3DUSAGE_SOFTWAREPROCESSING),
	m_bUseMIXED(TRUE),

	m_pCircleTexture(NULL),			// Shadow	Circle
	m_emShadowDetail(SHADOW_2TEX),	// Shadow	디테일						- 최고 0, 중상1, 중간 2, 최저 3
	m_fTexWidth(512.f),
	m_fTexHeight(512.f),

	m_fDistanceRate(2.0f),

	m_vBaseViewMin(0.f,0.f,0.f),
	m_vBaseViewMax(0.f,0.f,0.f),
	m_vViewMin(0.f,0.f,0.f),
	m_vViewMax(0.f,0.f,0.f),
	m_fDistance(0.f), 

	m_fPointDis1(0.f),
	m_nPointNum1(0),
	m_fPointDis2(0.f),
	m_nPointNum2(0),

	m_bCharUse(TRUE),
	m_bImageBlurUse(TRUE)
{
	D3DXMatrixIdentity ( &m_matView );
	D3DXMatrixIdentity ( &m_matProj );

	D3DXMatrixIdentity ( &m_matLastWVP_D );
}

DxShadowMap::~DxShadowMap()
{
}

HRESULT DxShadowMap::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );


	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		m_bUseMIXED = FALSE;
	}

	//	Note : Vertex Shader
	//
	if ( m_bUseMIXED &&	d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 0) )
	{
		m_dwUseSwShader = 0;	// 이럴 땐 MIXED -> H/W 가속 이다.
	}

	////	Note : 그래픽 카드 성능 보고 텍스쳐 선택 
	////
	//m_dwTexDetail = (d3dCaps.MaxTextureWidth < m_dwTexDetail) ? d3dCaps.MaxTextureWidth : m_dwTexDetail;
	//m_dwTexDetail = (d3dCaps.MaxTextureHeight < m_dwTexDetail) ? d3dCaps.MaxTextureHeight : m_dwTexDetail;

	//	Note : Z 버퍼에 Texture를 사용 할 수 있나 없나를 테스트 하고 
	//			사용가능하면 TRUE 를 불가능 하면 FALSE 를 호출한다.
	//
	HRESULT hr;
	BOOL		bUseZTexture = TRUE;
	hr = pD3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
									d3dCaps.DeviceType, d3dsdBackBuffer.Format,
									D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE,
									D3DFMT_D24S8 );
	if ( FAILED(hr) )	bUseZTexture = FALSE;

	//	Note : 그래픽 카드의 성능을 보고 결정
	//
	if ( !m_dwUseSwShader && d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 0) && bUseZTexture )
	{
		m_emShadowDetail = SHADOW_2TEX;													// 최고 - 2개
	}
	else if ( d3dCreateParam.BehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING )
	{
		m_emShadowDetail = SHADOW_DISABLE;												// 권장	- 안나옴
	}
	else
	{
		m_emShadowDetail = SHADOW_1TEX;													// 권장 - 1개
	}

	//	Note : 원 그림자를 생성한다.
	//
	m_szCircleTex = "Circle_Shadow.dds";
	TextureManager::LoadTexture ( m_szCircleTex.c_str(), pd3dDevice, m_pCircleTexture, 0, 0 );

	return S_OK;
}

HRESULT DxShadowMap::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	SAFE_RELEASE ( m_pShadowVB );
	hr = pd3dDevice->CreateVertexBuffer ( 4*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &m_pShadowVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxShadowMap::RestoreDeviceObjects() -- CreateVertexBuffer() -- Failed" );
		return E_FAIL;
	}

	if ( m_pShadowVB )
	{
		VERTEX*	pVertex;
		hr = m_pShadowVB->Lock ( 0, 0, (VOID**)&pVertex, 0 );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxShadowMap::RestoreDeviceObjects() -- Lock() -- Failed" );
			return E_FAIL;
		}

		pVertex[0].p = D3DXVECTOR3 ( -m_fRange, 0.f, m_fRange );
		pVertex[1].p = D3DXVECTOR3 ( m_fRange, 0.f, m_fRange );
		pVertex[2].p = D3DXVECTOR3 ( -m_fRange, 0.f, -m_fRange );
		pVertex[3].p = D3DXVECTOR3 ( m_fRange, 0.f, -m_fRange );
		pVertex[0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertex[1].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertex[2].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertex[3].t = D3DXVECTOR2 ( 1.f, 1.f );

		m_pShadowVB->Unlock ();
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		//pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR  );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );	//D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );	//D3DBLEND_SRCCOLOR );

		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR  );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCircleSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectCircleSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CW );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedOneShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawOneShadowSB );
	}

	//	Note : 계산이 많아서 .. 1번 만 해준다.
	//
	DxSurfaceTex::GetInstance().InitViewProjection ( m_vBaseViewMax, m_vBaseViewMin, m_matProj, EYE_TO_LOOKAT, m_fTexWidth, m_fTexHeight );

	return S_OK;
}

HRESULT DxShadowMap::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE ( m_pShadowVB );
	
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	SAFE_RELEASE( m_pSavedCircleSB );
	SAFE_RELEASE( m_pEffectCircleSB );
	SAFE_RELEASE( m_pSavedOneShadowSB );
	SAFE_RELEASE( m_pDrawOneShadowSB );

	return S_OK;
}

HRESULT DxShadowMap::DeleteDeviceObjects()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szCircleTex.c_str(), m_pCircleTexture );

	return S_OK;
}

HRESULT DxShadowMap::FrameMove ( float fTime, float fElapsedTime )
{
	m_bCharUse = TRUE;
	m_bImageBlurUse = TRUE;

	return S_OK;
}

void DxShadowMap::LastImageBlur ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_bImageBlurUse )					return;
	if( m_emShadowDetail==SHADOW_DISABLE )	return;
	if( m_emShadowDetail==SHADOW_NONE )		return;

	//	Note : 이미지를 문지른다.
	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	{
		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
		pd3dDevice->SetDepthStencilSurface( NULL );
		D3DXImageBlur( pd3dDevice, DxSurfaceTex::GetInstance().m_pTempTex512, m_fTexWidth, m_fTexHeight );
	}
	sBackupTarget.RestoreTarget ( pd3dDevice );

	m_bImageBlurUse = FALSE;
}

HRESULT DxShadowMap::RenderShadowCharMob ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
    if ( m_bCharUse )	RenderShadow ( pSkinObj, matRot, pd3dDevice );
	else				RenderShadowMob ( pSkinObj, matRot, pd3dDevice );

	m_bCharUse = FALSE;

	return S_OK;
}

HRESULT DxShadowMap::RenderShadow ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( m_emShadowDetail == SHADOW_DISABLE )	return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	m_fDistance = sqrtf(powf(FromPt.x - vCharPos.x,2) + powf(FromPt.y - vCharPos.y,2) + powf(FromPt.z - vCharPos.z,2) ) * m_fDistanceRate;

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	{
		switch( m_emShadowDetail )
		{
		case SHADOW_NONE:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
			break;
		default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
			break;
		};
	}
	sBackupTarget.RestoreTarget ( pd3dDevice );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT DxShadowMap::RenderShadowMob ( DxSkinChar *pSkinObj, D3DXMATRIX &matRot, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if( m_emShadowDetail == SHADOW_DISABLE )	return S_OK;

	D3DXVECTOR3		vMax, vMin;
	vMax.x = m_vBaseViewMax.x + 50.f;
	vMax.z = m_vBaseViewMax.z + 50.f;
	vMin.x = m_vBaseViewMin.x - 50.f;
	vMin.z = m_vBaseViewMin.z - 50.f;

	//	Note : 좀 멀리 있는 것일 경우 뿌리지 않는다.
	//
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );
	D3DXVECTOR3	vDis;
	vDis.x	= vCharPos.x - LookatPt.x;
	if ( vDis.x > vMax.x || vDis.x < vMin.x )	return S_OK;
	vDis.z	= vCharPos.z - LookatPt.z;
	if ( vDis.z > vMax.z || vDis.z < vMin.z )	return S_OK;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matLastWVP_D );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matIdentity );

	
	DxBackUpRendTarget sBackupTarget ( pd3dDevice );
	{
		switch( m_emShadowDetail )
		{
		case SHADOW_NONE:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Circle_Day_Night ( pd3dDevice, matRot, 0xffffffff );
			break;
		case SHADOW_CHAR:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Circle_Day_Night ( pd3dDevice, matRot, 0x80808080 );
			break;
		default:
			pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
			pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
			Shadow_Day_Night ( pd3dDevice, pSkinObj, matRot );
			break;
		};
	}
	sBackupTarget.RestoreTarget ( pd3dDevice );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	return S_OK;
}

HRESULT	DxShadowMap::Shadow_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, D3DXMATRIX &matRot )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;
	
	D3DXVECTOR3 &vCameraPos = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

	D3DXVECTOR3	 vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	D3DXVECTOR3 up;
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

	//GLPeriod::GetInstance().SetHour (1);	//지워야 한다.	강제적으로 밤을 만들고 싶을 때 사용한다.

	D3DXMATRIX lightView;

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	float kk	= 0.15f;
	if ( GLPeriod::GetInstance().IsOffLight() )	// 낮일 경우, 조명은 꺼져 있음 (시간에 따른 빛의 방향(각도) 조정 )
	{
		float		fDirectFact = GLPeriod::GetInstance().GetDirectionFact();
		if ( fDirectFact < 0.f ) fDirectFact = -fDirectFact;
		fDirectFact *= 0.4f;
		fDirectFact += 0.3f;		// 좀더 길어 진듯. ?

		vLightDirect.y = 0.f;
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightDirect.x = vLightDirect.x*fDirectFact;
		vLightDirect.y = fabsf(fDirectFact) - 1.f;
		vLightDirect.z = vLightDirect.z*fDirectFact;

		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		kk *= ( (0.7f-fabsf(fDirectFact)) + 0.5f );		// 합은 1.2 최소 0.5


		D3DXVECTOR3 &FromPt = DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3 vTempDir;

		vTempDir.x  = LookatPt.x - (vLightDirect.x*40.f);
		vTempDir.y  = LookatPt.y;
		vTempDir.z  = LookatPt.z - (vLightDirect.z*40.f);

		vLightPos.x = vTempDir.x - (vLightDirect.x*(50.f+m_fDistance));
		vLightPos.y = vTempDir.y - (vLightDirect.y*(50.f+m_fDistance));
		vLightPos.z = vTempDir.z - (vLightDirect.z*(50.f+m_fDistance));

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vTempDir, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW, &lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );

		D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView,	&m_matProj );

		m_pSavedStateBlock->Capture();
		m_pEffectStateBlock->Apply();

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);;
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		pSkinObj->RenderShadow( pd3dDevice, matRot, TRUE );

		m_pSavedStateBlock->Apply();
	}
	else									// 밤일 경우, 조명은 켜져 있음 (빛의 위치에 따른 빛의 방향(각도) 조정 )
	{
		hr = pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );		// 예전것을 지워 줘야 한다. ^^;
		pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
		if(FAILED(hr))	return E_FAIL;

		hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
		if(FAILED(hr))	return E_FAIL;

		SetPointLight ( pd3dDevice, pSkinObj, matRot, vCharPos, vLightPos, vLightDirect );

		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

		pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
		pd3dDevice->SetDepthStencilSurface( NULL );

		//	Note : 	View 와 Projection 을 얻는다.
		//
		SetViewProjection ();

		//	Note : 판에.. 그림자를 그려 본다.
		//			이전에 해야 할 것, World, View, Projection 설정, 텍스쳐 설정
		//
		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
		pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pShadowTex );

		//	Note : 평면과 카메라 CV의 만나는 점 구하기.
		//
		D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
		D3DXVECTOR3		vPosition[4];

		float	fNearPlane	= 0.5f;
		float	fFarPlane	= 100000.f;
		float	fFOV		= D3DX_PI/4;

		FLOAT dist, t;
		D3DXVECTOR3 pt[8];
		D3DXVECTOR3 v1, v2, n;
		D3DXVECTOR3 vDir, vUp, vPos;	
		D3DXVECTOR3 vCross;

		vCharPos.y = 5.f;
		vDir = vLightDirect;
		vUp = vUpVec;
		vPos = vCharPos - (vLightDirect*33.f);		// 33.f 라는 숫자. (이것은 그림자가 나타내는 범위이다.)
													// 그림자를 크게 하고 싶을경우 값을 높이면 된다.

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
			
			t = dist * tanf(fFOV/2) * m_fTexWidth/m_fTexHeight; // take into account screen proportions
			t = (i & 0x1) ? -t : t;
			pt[i] += vCross *t;
			pt[i] = vPos + pt[i];
		}

		//	Note : 면 만들기
		D3DXPLANE	pPlane;
		vPosition[0] = D3DXVECTOR3 ( 100000.f, 0.f, -100000.f );
		vPosition[1] = D3DXVECTOR3 ( -100000.f, 0.f, 100000.f );
		vPosition[2] = D3DXVECTOR3 ( 100000.f, 0.f, 100000.f );
		D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

		//	Note : 점 만들기
		vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );	// 3 2
		vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );	// 1 0
		vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
		vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

		VERTEX	sShadow[4];

		sShadow[0].p = vPosition[3];
		sShadow[1].p = vPosition[1];
		sShadow[2].p = vPosition[2];
		sShadow[3].p = vPosition[0];

		m_pSavedOneShadowSB->Capture();
		m_pDrawOneShadowSB->Apply();

		pd3dDevice->SetFVF ( VERTEX::FVF );

		DxSetTextureMatrix ( pd3dDevice, 0, m_matLastWVP_D );

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, sShadow, sizeof(VERTEX) );

		DxResetTextureMatrix ( pd3dDevice, 0 );

		m_pSavedOneShadowSB->Apply();

		//DxSurfaceTex::GetInstance().RenderPointShadow ( pd3dDevice, m_vViewMax, m_vViewMin, m_matLastWVP_D, m_dwTexDetail );	// 예전 것 ( 속도 느림 )
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	return hr;
}

HRESULT	DxShadowMap::Circle_Day_Night ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matRot, DWORD dwTextureFactor )
{
	D3DXVECTOR3	vLightPos;
	D3DXVECTOR3	vLightDirect;

	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vLightDirect = Light.m_Light.Direction;

	D3DXVECTOR3	 vCharPos	= D3DXVECTOR3 ( matRot._41, matRot._42, matRot._43 );

	D3DXMATRIX matWorld, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
	pd3dDevice->GetTransform ( D3DTS_VIEW, &matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matProj );

	if ( GLPeriod::GetInstance().IsOffLight() )	// 낮일 경우, 조명은 꺼져 있음 (시간에 따른 빛의 방향(각도) 조정 )
	{
		D3DXVECTOR3 up;
		up.x = 0.0f; up.y = 1.0f; up.z = 0.0f;

		D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();

		D3DXMATRIX	matIdentity, lightView;

		D3DXMatrixIdentity ( &matIdentity );

		float		fDirectFact = GLPeriod::GetInstance().GetDirectionFact();
		if ( fDirectFact < 0.f ) fDirectFact = -fDirectFact;
		fDirectFact *= 0.4f;
		fDirectFact += 0.3f;		// 좀더 길어 진듯. ?

		vLightDirect.y = 0.f;
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightDirect.x = vLightDirect.x*fDirectFact;
		vLightDirect.y = fabsf(fDirectFact) - 1.f;
		vLightDirect.z = vLightDirect.z*fDirectFact;

		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );


		D3DXVECTOR3 &FromPt = DxViewPort::GetInstance().GetFromPt ();
		D3DXVECTOR3 vTempDir;

		vTempDir.x  = LookatPt.x - (vLightDirect.x*40.f);
		vTempDir.y  = LookatPt.y;
		vTempDir.z  = LookatPt.z - (vLightDirect.z*40.f);

		vLightPos.x = vTempDir.x - (vLightDirect.x*(50.f+m_fDistance));
		vLightPos.y = vTempDir.y - (vLightDirect.y*(50.f+m_fDistance));
		vLightPos.z = vTempDir.z - (vLightDirect.z*(50.f+m_fDistance));

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vTempDir, &up);


		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matRot );
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView,	&m_matProj );

		pd3dDevice->SetTexture ( 0, m_pCircleTexture );

		m_pSavedCircleSB->Capture();
		m_pEffectCircleSB->Apply();

		if( dwTextureFactor!=0xffffffff)
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwTextureFactor );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
		}

		pd3dDevice->SetFVF ( VERTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, sizeof(VERTEX) );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pSavedCircleSB->Apply();
	}
	else
	{
		D3DXMATRIX	matTempRot = matRot;
		matTempRot._42 = 0.f;
		pd3dDevice->SetTransform ( D3DTS_WORLD,			&matTempRot );
		//	Note : 	View 와 Projection 을 얻는다.
		//
		SetViewProjection ();

		//	Note : 판에.. 그림자를 그려 본다.
		//			이전에 해야 할 것, World, View, Projection 설정, 텍스쳐 설정
		//
		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
		pd3dDevice->SetTexture ( 0, m_pCircleTexture );

		m_pSavedCircleSB->Capture();
		m_pEffectCircleSB->Apply();

		if( dwTextureFactor!=0xffffffff)
		{
			pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	dwTextureFactor );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
		}

		pd3dDevice->SetFVF ( VERTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pShadowVB, 0, sizeof(VERTEX) );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_pSavedCircleSB->Apply();
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD,			&matWorld );
	pd3dDevice->SetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&matProj );

	return S_OK;
}

void	DxShadowMap::ReSetClosedPL ( D3DLIGHTQ &sLight, D3DXVECTOR3 vCharPos, int nNum )
{
	float	Dis = powf(sLight.Position.x - vCharPos.x,2) + powf(sLight.Position.z - vCharPos.z,2);

	if ( m_nPointNum1 == 0 )
	{
		m_fPointDis1 = Dis;
		m_nPointNum1 = nNum;
	}
	else if ( m_nPointNum2 == 0 )
	{
		if ( Dis < m_fPointDis1 )		// 현재것이 더 작다면.
		{
			m_fPointDis2 = m_fPointDis1;
			m_nPointNum2 = m_nPointNum1;
			m_fPointDis1 = Dis;
			m_nPointNum1 = nNum;
		}
		else
		{
			m_fPointDis2 = Dis;
			m_nPointNum2 = nNum;
		}
	}
	else
	{
		if ( Dis < m_fPointDis2 )		// 현재것이 더 작다면.
		{
			m_fPointDis2 = Dis;
			m_nPointNum2 = nNum;

			if ( Dis < m_fPointDis1 )		// 현재것이 더 작다면.
			{
				m_fPointDis2 = m_fPointDis1;
				m_nPointNum2 = m_nPointNum1;
				m_fPointDis1 = Dis;
				m_nPointNum1 = nNum;
			}
		}
	}
}

void	DxShadowMap::SetPointLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar *pSkinObj, D3DXMATRIX &matRot, 
									D3DXVECTOR3& vCharPos, D3DXVECTOR3& vLightPos, D3DXVECTOR3& vLightDirect )
{
	D3DXMATRIX lightView;

	D3DXVECTOR3	vDistance;
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();

	D3DXVECTOR3 up;
	
	up.x = 0.0f; up.y = 0.0f; up.z = 1.0f;

	float fLightLeftOver = 0.4f;

	// 가까운 빛을 얻어 온다.
	DXLIGHT *pLightClosed = NULL;
	DXLIGHT *pLightClosed2 = NULL;

	D3DLIGHTQ sLight01, sLight02;

	//	기본적으로 놓아 둠
	float Dis, PosX, PosY, PosZ, Range;
	Dis		= 100.f;
	PosX	= 0.f;
	PosY	= 0.f;
	PosZ	= 0.f;
	Range	= FLT_MIN;

	float Dis2, PosX2, PosZ2, Range2;
	Dis2	= 100.f;
	PosX2	= 0.f;
	PosZ2	= 0.f;
	Range2	= FLT_MIN;

	float fDistance1;
	float fDistance2;

	//	초기화
	m_fPointDis1 = 0.f;
	m_nPointNum1 = 0;
	m_fPointDis2 = 0.f;
	m_nPointNum2 = 0;
	//	계산
	for ( DWORD i=1; i<=7; i++ )
	{
		DXLIGHT* pLightClosed = DxLightMan::GetInstance()->GetClosedLight(i);

		if ( pLightClosed )		ReSetClosedPL ( pLightClosed->m_Light, vCharPos, i );
	}

	if ( m_nPointNum1 )
	{
		pLightClosed = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum1);
		if ( pLightClosed )		sLight01 = pLightClosed->m_Light;
	}
	if ( m_nPointNum2 )
	{
		pLightClosed2 = DxLightMan::GetInstance()->GetClosedLight(m_nPointNum2);
		if ( pLightClosed2 )	sLight02 = pLightClosed2->m_Light;
	}

	// 빛이 있다면 그 빛의 정보를 얻어옴
	if ( pLightClosed )
	{
		Dis = m_fPointDis1;
		PosX = sLight01.Position.x;
		PosZ = sLight01.Position.z;

		Range	= sLight01.Range;

		fDistance1 = sqrtf(m_fPointDis1);
	}

	if ( pLightClosed2 )
	{
		Dis2 = m_fPointDis2;
		PosX2 = sLight02.Position.x;
		PosZ2 = sLight02.Position.z;

		Range2	= sLight02.Range;
		fDistance2 = sqrtf(m_fPointDis2);
	}

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	float	kk = 0.15f;
	DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
	pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

	if ( pLightClosed && pLightClosed2 )
	{
		float fRate = (fDistance2-fDistance1)/(fDistance1*0.7f);
		if ( fRate>1.f )	fRate = 1.f;

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );			//	빛에서 너무 멀리 떨어져 있으면,
		vDistance.z = powf ( vLightDirect.z, 2 );			//	거리를 일정하게 유지 시켜서
		vDistance.y = sqrtf(vDistance.x+vDistance.z);	//	그림자가 너무 크거나
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )		//	나오지 않는 버그를 방지한다.
		{													//	.
			vLightDirect.y = -(vDistance.y*2.f/3.f);		//	.
		}													//	.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f*fRate);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f*fRate);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		vLightDirect = vCharPos - vLightPos;					//	밖에서 사용 할것이다.
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );		//
		
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : 잠시 추가
		//
		D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView,	&m_matProj );

		//	Note : 스킨 오브젝트 랜더링.
		//
		pSkinObj->RenderShadow( pd3dDevice, matRot, FALSE );
	}

	//	Note : 아무 것도 안된다.
	//
	else if ( (!pLightClosed && !pLightClosed2) || GLPeriod::GetInstance().IsOffLight() )	// 빛이 없거나 불이 꺼져 있을때
	{
		float kk = 1.0f - fLightLeftOver;
		kk *= 0.25f;

		DWORD TexFactor = D3DCOLOR_COLORVALUE(kk,kk,kk,kk);
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, TexFactor );

		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( vCharPos.x, vCharPos.y+40.f, vCharPos.z );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x;
		vLightPos.y = vCharPos.y + 150.f;
		vLightPos.z = vCharPos.z;

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);

		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : 잠시 추가
		//
		D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView,	&m_matProj );

		//	Note : 스킨 오브젝트 랜더링.
		//
		pSkinObj->RenderShadow( pd3dDevice, matRot, FALSE );
	}

	//	Note : 빛 하나만 있다. 
	//
	else
	{
		vCharPos.y = 10.f;
		vLightPos = D3DXVECTOR3 ( PosX, vCharPos.y+40.f, PosZ );

		vLightDirect = vCharPos - vLightPos;
		vDistance.x = powf ( vLightDirect.x, 2 );
		vDistance.z = powf ( vLightDirect.z, 2 );
		vDistance.y = sqrtf(vDistance.x+vDistance.z);
		if ( fabsf(vLightDirect.y)*1.5f < vDistance.y )
		{
			vLightDirect.y = -(vDistance.y*2.f/3.f);
		}
		D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

		vLightPos.x = vCharPos.x - (vLightDirect.x*150.f);
		vLightPos.y = vCharPos.y - (vLightDirect.y*150.f);
		vLightPos.z = vCharPos.z - (vLightDirect.z*150.f);

		D3DXMatrixLookAtLH(&lightView, &vLightPos, &vCharPos, &up);
		
		pd3dDevice->SetTransform ( D3DTS_VIEW,			&lightView );
		pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&m_matProj );

		//	Note : 잠시 추가
		//
		D3DXMatrixMultiply ( &m_matLastWVP_D, &lightView,	&m_matProj );

		//	Note : 스킨 오브젝트 랜더링.
		//
		pSkinObj->RenderShadow( pd3dDevice, matRot, FALSE );
	}

	m_pSavedStateBlock->Apply();
}

void	DxShadowMap::SetViewProjection ()
{
	D3DXVECTOR3 &LookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &FromPt		= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3	vCenter		= LookatPt;
	vCenter.y				= 0.f;

	D3DXVECTOR3	vLightDirect = LookatPt - FromPt;

	D3DXVec3Normalize ( &vLightDirect, &vLightDirect );

	vCenter.x  = vCenter.x + (vLightDirect.x*50.f);
	vCenter.y  = vCenter.y;
	vCenter.z  = vCenter.z + (vLightDirect.z*50.f);

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*EYE_TO_LOOKAT;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );

	D3DXMatrixLookAtLH ( &m_matView, &vTempLookatPt, &vCenter, &vUpVec );		// 뷰 만들고..

	m_vViewMax		= D3DXVECTOR3 ( m_vBaseViewMax.x+vCenter.x, vCenter.y, m_vBaseViewMax.z+vCenter.z );
	m_vViewMin		= D3DXVECTOR3 ( m_vBaseViewMin.x+vCenter.x, vCenter.y, m_vBaseViewMin.z+vCenter.z );
}

HRESULT DxShadowMap::ClearShadow ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	if ( !DxSurfaceTex::GetInstance().m_pShadowSuf )	return S_FALSE;

	if ( m_emShadowDetail == SHADOW_DISABLE )
		return S_OK;

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );

	pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pTempSuf512 );
	pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf );
	hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
	if(FAILED(hr))	return E_FAIL;

	pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pShadowSuf );
	hr = pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);
	if(FAILED(hr))	return E_FAIL;

	sBackupTarget.RestoreTarget ( pd3dDevice );

	return hr;
}