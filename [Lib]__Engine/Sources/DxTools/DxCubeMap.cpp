#include "pch.h"

#include "./glperiod.h"

#include "./DxFrameMesh.h"
#include "./DxLandMan.h"

#include "./DxSkinChar.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"

#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "DxCubeMap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxCubeMap& DxCubeMap::GetInstance()
{
	static DxCubeMap Instance;
	return Instance;
}

LPDIRECT3DSTATEBLOCK9	DxCubeMap::m_pSavedLightSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxCubeMap::m_pDrawLightSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxCubeMap::m_pSavedSkyBoxSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxCubeMap::m_pDrawSkyBoxSB = NULL;
const	DWORD	DxCubeMap::VERTEX::FVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const	DWORD	DxCubeMap::SKYBOXVERTEX::FVF	= D3DFVF_XYZRHW|D3DFVF_TEX1;
//DxEnvironment::POS_DIFF_TEX1	DxEnvironment::DxWave::aRefract[REFRACT_VERTEX];

DxCubeMap::DxCubeMap() :
	m_bGraphicCardEnable(FALSE),
	m_bOptionEnable(TRUE),
	m_bActive(FALSE),
	m_pSkyBoxTex(NULL),
	m_pLightTex(NULL),
	m_pCubeTex(NULL),
	m_pCubeTexTEST(NULL),
	m_pVB(NULL),
	m_pIB(NULL)
{
	StringCchCopy( m_szSkyBoxTex, MAX_PATH, "SkyBox.bmp" );
	StringCchCopy( m_szLightTex, MAX_PATH, "SkyBox.bmp" );
}

DxCubeMap::~DxCubeMap()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}

HRESULT DxCubeMap::InitDeviceObjects ( LPDIRECT3DQ pD3D, LPDIRECT3DDEVICEQ pd3dDevice, D3DSURFACE_DESC &d3dsdBackBuffer )
{
	HRESULT hr(S_OK);

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DDEVICE_CREATION_PARAMETERS d3dCreateParam;
	pd3dDevice->GetCreationParameters ( &d3dCreateParam );

	DWORD		dwUseSwShader = D3DUSAGE_SOFTWAREPROCESSING;
	BOOL		bUseMIXED = TRUE;

	//	Note : MIXED
	//
	if ( !(d3dCreateParam.BehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) )
	{
		bUseMIXED = FALSE;
	}

	//	Note : Vertex Shader
	//
	if ( bUseMIXED && d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1) )
	{
		dwUseSwShader = 0;	// 이럴 땐 MIXED -> H/W 가속 이다.
	}

	if ( !dwUseSwShader && (d3dCaps.PixelShaderVersion >= D3DPS_VERSION(1, 4)) &&		// 픽셀 1.1, 1.4 사용 가능
		(d3dCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP) )								// 큐브맵 사용 가능
	{
		m_bGraphicCardEnable = TRUE;
	}
	else		m_bGraphicCardEnable = FALSE;

	VERTEX*	pVertices;
	WORD*	pIndices;

	SAFE_RELEASE ( m_pVB );
	hr = pd3dDevice->CreateVertexBuffer ( 4*8*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxCubeMap::InitDeviceObjects() -- CreateVertexBuffer() -- Failed" );
		return hr;
	}

	hr = m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if ( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxCubeMap::InitDeviceObjects() -- Lock() -- Failed" );
		return hr;
	}

	for ( DWORD	i=0; i<8; ++i )
	{
		pVertices[i*4+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pVB->Unlock ();

	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer ( 3*2*8*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxCubeMap::InitDeviceObjects() -- CreateIndexBuffer() -- Failed" );
		return hr;
	}

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxCubeMap::InitDeviceObjects() -- Lock() -- Failed" );
		return hr;
	}

	for ( DWORD	i=0; i<8; ++i )
	{
		*pIndices++ = (WORD)((4*(0+i))+0);
		*pIndices++ = (WORD)((4*(1+i))+0);
		*pIndices++ = (WORD)((4*(0+i))+1);

		*pIndices++ = (WORD)((4*(1+i))+0);
		*pIndices++ = (WORD)((4*(1+i))+1);
		*pIndices++ = (WORD)((4*(0+i))+1);
	}
	m_pIB->Unlock ();

	TextureManager::LoadTexture ( m_szSkyBoxTex, pd3dDevice, m_pSkyBoxTex, 0, 0 );

	TextureManager::LoadTexture ( m_szLightTex, pd3dDevice, m_pLightTex, 0, 0 );

	TextureManager::LoadCubeTexture( "LobbyCube.dds", pd3dDevice, m_pCubeTexTEST, 0, 0 );

	/*
	m_bGraphicCardEnable(FALSE),
	m_bOptionEnable(FALSE),
	m_bActive(FALSE),
	*/
	return S_OK;
}

HRESULT DxCubeMap::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT	hr = S_OK;

	if ( !Is_GraphicCard_Option_Enable() ) return hr;

	D3DFORMAT	colorFormat = D3DFMT_X1R5G5B5;
	D3DFORMAT	zFormat		= D3DFMT_D16;

	//	Note : 큐브 텍스쳐 만들기
	//
	SAFE_RELEASE( m_pCubeTex );
	if( FAILED ( pd3dDevice->CreateCubeTexture ( 64, 1, D3DUSAGE_RENDERTARGET,				// 1. X1R5G5B5
											colorFormat, D3DPOOL_DEFAULT, &m_pCubeTex, NULL ) ) )
	{
		colorFormat = D3DFMT_A1R5G5B5;
		if( FAILED ( pd3dDevice->CreateCubeTexture ( 64, 1, D3DUSAGE_RENDERTARGET,			// 2. A1R5G5B5
											colorFormat, D3DPOOL_DEFAULT, &m_pCubeTex, NULL ) ) )	
		{
			colorFormat = D3DFMT_A4R4G4B4;
			if( FAILED ( pd3dDevice->CreateCubeTexture ( 64, 1, D3DUSAGE_RENDERTARGET,		// 3. A4R4G4B4
											colorFormat, D3DPOOL_DEFAULT, &m_pCubeTex, NULL ) ) )
			{
				colorFormat = D3DFMT_A8R8G8B8;
				hr = pd3dDevice->CreateCubeTexture ( 64, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
											colorFormat, D3DPOOL_DEFAULT, &m_pCubeTex, NULL );
				if( FAILED(hr) )
				{
//					GASSERT ( 0 && "<DxCubeMap> 텍스쳐 포맷 설정 불가능함" );
					return S_OK;
				}
			}
		}
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedLightSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawLightSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSkyBoxSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSkyBoxSB );
	}

	return hr;
}

HRESULT DxCubeMap::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pCubeTex );

	SAFE_RELEASE( m_pSavedLightSB );
	SAFE_RELEASE( m_pDrawLightSB );

	SAFE_RELEASE( m_pSavedSkyBoxSB );
	SAFE_RELEASE( m_pDrawSkyBoxSB );

	return S_OK;
}

HRESULT DxCubeMap::DeleteDeviceObjects ()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	TextureManager::ReleaseTexture ( m_szSkyBoxTex, m_pSkyBoxTex );
	TextureManager::ReleaseTexture ( m_szLightTex, m_pLightTex );

	TextureManager::ReleaseTexture ( "LobbyCube.dds", m_pCubeTexTEST );
	
	return S_OK;
}

HRESULT DxCubeMap::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !Is_GraphicCard_Option_Enable() )	return S_OK;
	if ( !m_bActive )						return S_OK;

	ComputeVB ();

	D3DXVECTOR3&	vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3&	vFromPt = DxViewPort::GetInstance().GetFromPt ();

	D3DXMATRIX	matIdentity;
	D3DXMATRIX	matOldView, matView;
	D3DXMATRIX	matOldProj, matProj;

	D3DXMatrixIdentity ( &matIdentity );
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 0.5f, 1000.0f );

	pd3dDevice->GetTransform ( D3DTS_VIEW, &matOldView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION, &matOldProj );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matProj );

	// Get the current view matrix, to concat it with the cubemap view vectors
    D3DXMATRIX matViewDir( matOldView );
    matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

	DxBackUpRendTarget sBackupTarget ( pd3dDevice );

	LPDIRECT3DSURFACEQ pSurface;

	for( UINT i = 0; i < 6; i++ )
    {
        m_pCubeTex->GetCubeMapSurface ( (D3DCUBEMAP_FACES)i, 0, &pSurface );

		pd3dDevice->SetRenderTarget ( 0, pSurface );
		pd3dDevice->SetDepthStencilSurface( NULL );

		pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );	// 클리어

        matView = D3DUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES) i );					// View 가져오기

	//	D3DXMatrixMultiply( &matView, &matViewDir, &matView );

		matView._41 = vFromPt.x;
		matView._42 = vFromPt.y;
		matView._43 = vFromPt.z;

		pd3dDevice->SetTransform ( D3DTS_VIEW, &matView );

		RenderSkybox ( pd3dDevice );				// 박스를 뿌린다.
		RenderLight ( pd3dDevice, matView );		// 빛을 뿌린다.

		SAFE_RELEASE ( pSurface );
    }

	sBackupTarget.RestoreTarget ( pd3dDevice );

	pd3dDevice->SetTransform ( D3DTS_VIEW, &matOldView );
	pd3dDevice->SetTransform ( D3DTS_PROJECTION, &matOldProj );

	m_bActive = FALSE;		// 1Frame 이 끝나면 활성화를 끈다.

	return S_OK;
}

void	DxCubeMap::ComputeVB ()
{
	//

	//VERTEX*	pVertices;
	//m_pVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );

	//vPos = D3DXVECTOR3 ( vLookatPt.x-(pLight.Direction.x*10.f), 
	//					vLookatPt.y-(pLight.Direction.y*10.f), 
	//					vLookatPt.z-(pLight.Direction.z*10.f) );
	//pVertices[0].vPos = D3DXVECTOR3 ( vPos.x-fWidth, vPos.y+fHeight, vPos.z );
	//pVertices[1].vPos = D3DXVECTOR3 ( vPos.x+fWidth, vPos.y+fHeight, vPos.z );
	//pVertices[2].vPos = D3DXVECTOR3 ( vPos.x-fWidth, vPos.y-fHeight, vPos.z );
	//pVertices[3].vPos = D3DXVECTOR3 ( vPos.x+fWidth, vPos.y-fHeight, vPos.z );

	//if ( !GLPeriod::GetInstance().IsOffLight() )		// Point Light 계산
	//{

	//}

	//m_pVB->Unlock ();
}

void	DxCubeMap::RenderLight ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX& matView )
{
	float			fWidth = 20.f;
	float			fHeight = 20.f;
	D3DXVECTOR3		vPos;

	VERTEX	pVertices[4];
	D3DXMATRIX	matLocal;

	D3DLIGHTQ		pLight;
	D3DXVECTOR3		vUp		= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	D3DXVECTOR3&	vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3&	vFromPt = DxViewPort::GetInstance().GetFromPt ();

	pLight = DxLightMan::GetInstance()->GetDirectLight()->m_Light;

	vPos = D3DXVECTOR3 ( vFromPt.x-(pLight.Direction.x*100.f), 
						vFromPt.y-(pLight.Direction.y*100.f), 
						vFromPt.z-(pLight.Direction.z*100.f) );

	matLocal	= DxBillboardLookAt ( &vPos, &vFromPt, &vUp );
	matLocal._41 = vPos.x;
	matLocal._42 = vPos.y;
	matLocal._43 = vPos.z;

	vPos = D3DXVECTOR3 ( -fWidth, fHeight, 0.f );
	D3DXVec3TransformCoord ( &pVertices[0].vPos, &vPos, &matLocal );
	vPos = D3DXVECTOR3 ( fWidth, fHeight, 0.f );
	D3DXVec3TransformCoord ( &pVertices[1].vPos, &vPos, &matLocal );
	vPos = D3DXVECTOR3 ( -fWidth, -fHeight, 0.f );
	D3DXVec3TransformCoord ( &pVertices[2].vPos, &vPos, &matLocal );
	vPos = D3DXVECTOR3 ( fWidth, -fHeight, 0.f );
	D3DXVec3TransformCoord ( &pVertices[3].vPos, &vPos, &matLocal );


	pVertices[0].cDiff = D3DCOLOR_COLORVALUE ( pLight.Diffuse.r*2.f, pLight.Diffuse.g*2.f, pLight.Diffuse.b*2.f, pLight.Diffuse.a*2.f );
	pVertices[1].cDiff = pVertices[0].cDiff;
	pVertices[2].cDiff = pVertices[0].cDiff;
	pVertices[3].cDiff = pVertices[0].cDiff;

	pVertices[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	pVertices[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
	pVertices[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	pVertices[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );

	m_pSavedLightSB->Capture();
	m_pDrawLightSB->Apply();

	pd3dDevice->SetFVF ( VERTEX::FVF );
	pd3dDevice->SetTexture ( 0, m_pLightTex );	

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(VERTEX) );

	//if ( !GLPeriod::GetInstance().IsOffLight() )
	//{
	//	pLight = DxLightMan::GetInstance()->GetClosedLight(1)->m_Light;

	//	vPos = pLight.Position;

	//	matLocal	= DxBillboardLookAt ( &vPos, &vFromPt, &vUp );
	//	matLocal._41 = vPos.x;
	//	matLocal._42 = vPos.y;
	//	matLocal._43 = vPos.z;

	//	fWidth = 5.f;
	//	fHeight = 20.f;

	//	vPos = D3DXVECTOR3 ( -fWidth, fHeight, 0.f );
	//	D3DXVec3TransformCoord ( &pVertices[0].vPos, &vPos, &matLocal );
	//	vPos = D3DXVECTOR3 ( fWidth, fHeight, 0.f );
	//	D3DXVec3TransformCoord ( &pVertices[1].vPos, &vPos, &matLocal );
	//	vPos = D3DXVECTOR3 ( -fWidth, -fHeight, 0.f );
	//	D3DXVec3TransformCoord ( &pVertices[2].vPos, &vPos, &matLocal );
	//	vPos = D3DXVECTOR3 ( fWidth, -fHeight, 0.f );
	//	D3DXVec3TransformCoord ( &pVertices[3].vPos, &vPos, &matLocal );

	//	pVertices[0].cDiff = D3DCOLOR_COLORVALUE ( pLight.Diffuse.r*2.f, pLight.Diffuse.g*2.f, pLight.Diffuse.b*2.f, pLight.Diffuse.a*2.f );
	//	pVertices[1].cDiff = pVertices[0].cDiff;
	//	pVertices[2].cDiff = pVertices[0].cDiff;
	//	pVertices[3].cDiff = pVertices[0].cDiff;

	//	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(VERTEX) );
	//}

	m_pSavedLightSB->Apply();
}

void	DxCubeMap::RenderSkybox ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SKYBOXVERTEX	pVertices[4];

	pVertices[0].vPos = D3DXVECTOR4 ( 0.f,0.f,1.f,1.f );
	pVertices[1].vPos = D3DXVECTOR4 ( 64.f,0.f,1.f,1.f );
	pVertices[2].vPos = D3DXVECTOR4 ( 0.f,64.f,1.f,1.f );
	pVertices[3].vPos = D3DXVECTOR4 ( 64.f,64.f,1.f,1.f );

	pVertices[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	pVertices[1].vTex = D3DXVECTOR2 ( 1.f+(1.f/64.f), 0.f );
	pVertices[2].vTex = D3DXVECTOR2 ( 0.f, 1.f+(1.f/64.f) );
	pVertices[3].vTex = D3DXVECTOR2 ( 1.f+(1.f/64.f), 1.f+(1.f/64.f) );

	m_pSavedSkyBoxSB->Capture();
	m_pDrawSkyBoxSB->Apply();

	pd3dDevice->SetFVF ( SKYBOXVERTEX::FVF );

	pd3dDevice->SetTexture ( 0, m_pSkyBoxTex );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pVertices, sizeof(SKYBOXVERTEX) );

	m_pSavedSkyBoxSB->Apply();
}