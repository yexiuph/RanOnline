#include "pch.h"

#include "./glperiod.h"

#include "DxWeatherMan.h"
#include "DxViewPort.h"

#include "DxEnvironment.h"

#include "DxSkyMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	------------------------------------------------------------------------------------------------------
//	---------------------------------------- 번개, 천둥, 구름 렌더링 -------------------------------------------------
//	------------------------------------------------------------------------------------------------------

//	Note : 번개가 치기 시작할 때 위치와 색 그리고 사용가능하다는 것을 알려준다.
//
void DxSkyMan::LightingCheckReset ()
{
	if ( m_bLightingUSE )	return;			// m_bLightingUSE 은 처음이어야 한다.
	if ( !m_bLightingNow )	return;			// m_bLightingNow 은 TRUE 이어야 한다.

	D3DXVECTOR3& vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3	vDirect		= DxViewPort::GetInstance().GetLookDir_Y0();

	m_vLightingPos	= D3DXVECTOR3 ( (RANDOM_NUM+(vDirect.x*3.f))*2000.f, vLookatPt.y+1400.f, (RANDOM_NUM+(vDirect.z*3.f))*2000.f );
	m_cLightingColor = (((DWORD)(RANDOM_POS*255.f))<<16) + (((DWORD)(RANDOM_POS*150.f))<<8) + (DWORD)(RANDOM_POS*255.f);

	m_bLightingUSE	= TRUE;		// 사용하게 함
	m_fLightingTime = 0.f;
	m_nLightningNUM = rand()%2;
}

void DxSkyMan::LightingRender ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float fRainRate	= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();

	D3DXMATRIX	matIdentity, matLocal;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD,		&m_matWorld );

	float	fWidth = 1000.f;
	float	fHeight = 1000.f;		// 원래는 높이다 더 길다.   밑에 평면 만드는 것 땜시. ~!

	matLocal	= DxBillboardLookAtHeight ( &m_vLightingPos, DxViewPort::GetInstance().GetMatView() );

	D3DXVECTOR3	vLocal;
	vLocal = D3DXVECTOR3 ( -fWidth, fHeight*0.5f, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[0].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	fHeight*0.5f, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[1].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( -fWidth, -fHeight*2.0f, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[2].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	-fHeight*2.0f, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[3].vPos, &vLocal, &matLocal );

	DWORD dwAlpha = (DWORD)( ((RANDOM_POS*0.5f)+0.5f)*255.f );
	dwAlpha = (dwAlpha<<24) + m_cThunderColor;

	m_sVertices[0].dwColor = dwAlpha;		// 알파와 컬러를 변화 시켜야 한다.
	m_sVertices[1].dwColor = dwAlpha;
	m_sVertices[2].dwColor = dwAlpha;
	m_sVertices[3].dwColor = dwAlpha;

	float fTexUV;

	if ( m_nLightningNUM == 0 )			fTexUV = 0.f;
	else if ( m_nLightningNUM == 1 )	fTexUV = 0.5f;

	m_sVertices[0].vTex = D3DXVECTOR2 ( fTexUV,			0.f );
	m_sVertices[1].vTex = D3DXVECTOR2 ( fTexUV+0.5f,	0.f );
	m_sVertices[2].vTex = D3DXVECTOR2 ( fTexUV,			1.f );
	m_sVertices[3].vTex = D3DXVECTOR2 ( fTexUV+0.5f,	1.f );

	m_pSavedCloudShadowSB->Capture();
	m_pDrawCloudShadowSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pLightingTex );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVertices, sizeof(VERTEXCOLORTEX) );

	m_pSavedCloudShadowSB->Apply();

	ThunderRender ( pd3dDevice, m_vLightingPos, 4000.f, 0.5f, 0.5f );	// 구름에 천둥이 치는 모습 구현
}

void DxSkyMan::ThunderRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vPos, float fScale, float fBase, float fFact )
{
	float fRainRate	= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();

	D3DXMATRIX	matIdentity, matLocal;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD,		&m_matWorld );

	float	fWidth = fScale;
	float	fHeight = fScale*0.3f;

	matLocal	= DxBillboardLookAt ( &vPos, DxViewPort::GetInstance().GetMatView() );

	D3DXVECTOR3	vLocal;
	vLocal = D3DXVECTOR3 ( -fWidth, fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[0].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[1].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( -fWidth, -fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[2].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	-fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[3].vPos, &vLocal, &matLocal );

	float fAlpha = fBase + (RANDOM_POS*fFact);
	fAlpha = fAlpha * fRainRate;
	DWORD dwColorR = (DWORD)(((m_cThunderColor&0xff0000)>>16)*fAlpha);
	DWORD dwColorG = (DWORD)(((m_cThunderColor&0xff00)>>8)*fAlpha);
	DWORD dwColorB = (DWORD)((m_cThunderColor&0xff)*fAlpha);
	dwColorR = (dwColorR<<16) + (dwColorG<<8) + dwColorB;

	m_sVerticesTex2[0].dwColor = dwColorR;		// 알파와 컬러를 변화 시켜야 한다.
	m_sVerticesTex2[1].dwColor = dwColorR;
	m_sVerticesTex2[2].dwColor = dwColorR;
	m_sVerticesTex2[3].dwColor = dwColorR;

	m_sVerticesTex2[0].vTex02 = D3DXVECTOR2 ( 0.f, 0.f );
	m_sVerticesTex2[1].vTex02 = D3DXVECTOR2 ( 0.5f, 0.f );
	m_sVerticesTex2[2].vTex02 = D3DXVECTOR2 ( 0.f, 0.5f );
	m_sVerticesTex2[3].vTex02 = D3DXVECTOR2 ( 0.5f, 0.5f );

	m_sVerticesTex2[0].vTex02 += m_vThunderUV;
	m_sVerticesTex2[1].vTex02 += m_vThunderUV;
	m_sVerticesTex2[2].vTex02 += m_vThunderUV;
	m_sVerticesTex2[3].vTex02 += m_vThunderUV;

	m_pSavedThunderSB->Capture();
	m_pDrawThunderSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pThunderTex_C );
	pd3dDevice->SetTexture ( 1, m_pThunderTex );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX2::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVerticesTex2, sizeof(VERTEXCOLORTEX2) );

	m_pSavedThunderSB->Apply();

	pd3dDevice->SetTexture ( 1, NULL );

	if ( m_sLighting_1.emTex < EMLT_NONE0 )
	{
		ThundersRender ( pd3dDevice, m_sLighting_1 );		// 얇게 빛나는 번개
	}
	if ( m_sLighting_2.emTex < EMLT_NONE0 )
	{
		ThundersRender ( pd3dDevice, m_sLighting_2 );		// 얇게 빛나는 번개
	}
	if ( m_sLighting_3.emTex < EMLT_NONE0 )
	{
		ThundersRender ( pd3dDevice, m_sLighting_3 );		// 얇게 빛나는 번개
	}
}

void DxSkyMan::ThundersRender ( LPDIRECT3DDEVICEQ pd3dDevice, LIGHTING_OPTION vLighting )
{
	int nSprite	= vLighting.m_iSprite;
	int nUV		= (int) (vLighting.emTex);

	if ( (nSprite>=4) || (nSprite<0) )	return;

	float	fWidth = 2000.f*0.2f;
	float	fHeight = 2000.f*0.01f;

	float		fRainRate	= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();
	D3DXMATRIX	matLocal	= DxBillboardLookAt ( &vLighting.vPos, DxViewPort::GetInstance().GetMatView() );

	D3DXVECTOR3	vLocal;
	vLocal = D3DXVECTOR3 ( -fWidth, fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[0].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[1].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( -fWidth, -fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[2].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( fWidth,	-fHeight, 0.f );
	D3DXVec3TransformCoord ( &m_sVertices[3].vPos, &vLocal, &matLocal );

	float fAlpha = 0.4f;
	fAlpha = fAlpha * fRainRate;
	DWORD dwColorR = (DWORD)(((m_cThunderColor&0xff0000)>>16)*fAlpha);
	DWORD dwColorG = (DWORD)(((m_cThunderColor&0xff00)>>8)*fAlpha);
	DWORD dwColorB = (DWORD)((m_cThunderColor&0xff)*fAlpha);
	dwColorR = (dwColorR<<16) + (dwColorG<<8) + dwColorB;

	m_sVertices[0].dwColor = dwColorR;		// 알파와 컬러를 변화 시켜야 한다.
	m_sVertices[1].dwColor = dwColorR;
	m_sVertices[2].dwColor = dwColorR;
	m_sVertices[3].dwColor = dwColorR;

	m_sVertices[0].vTex.x = nSprite / 4.f;
	m_sVertices[0].vTex.y = nUV/4.f;

	m_sVertices[1].vTex.x = (nSprite+1) / 4.f;
	m_sVertices[1].vTex.y = nUV/4.f;

	m_sVertices[2].vTex.x = nSprite / 4.f;
	m_sVertices[2].vTex.y = (nUV+1)/4.f;

	m_sVertices[3].vTex.x = (nSprite+1) / 4.f;
	m_sVertices[3].vTex.y = (nUV+1)/4.f;

	m_pSavedCloudSB->Capture();
	m_pDrawCloudSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pThundersTex );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVertices, sizeof(VERTEXCOLORTEX) );

	m_pSavedCloudSB->Apply();
}

//-------------------------------------------------------------------------------------------------------
//	Note : Cloud
//			구름을 만들자. 
//-------------------------------------------------------------------------------------------------------
LPDIRECT3DSTATEBLOCK9			CCloud::m_pSavedCloudSB		= NULL;
LPDIRECT3DSTATEBLOCK9			CCloud::m_pDrawCloudSB			= NULL;		// 구름
LPDIRECT3DSTATEBLOCK9			CCloud::m_pSavedCloudShadowSB	= NULL;
LPDIRECT3DSTATEBLOCK9			CCloud::m_pDrawCloudShadowSB	= NULL;		// 구름, 번개에 사용
LPDIRECT3DSTATEBLOCK9			CCloud::m_pSavedCloudBaseSB	= NULL;
LPDIRECT3DSTATEBLOCK9			CCloud::m_pDrawCloudBaseSB		= NULL;		// 구름 ( 기본적인 색 )

const DWORD		CCloud::VERTEXCOLORTEX::FVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

CCloud::CCloud() :
	m_pCloudTex(NULL),
	m_pCloudShadowTex(NULL),
	m_pSrcTex(NULL),
	m_pSrcTex_45(NULL),
	m_dwVertexNum(12),
	m_dwFacesNum(14)
{
	StringCchCopy( m_szCloudTex,		MAX_PATH, "Cloud.dds" );
	StringCchCopy( m_szCloudShadowTex,	MAX_PATH, "Cloudy.dds" );
}

CCloud::~CCloud()
{
}
HRESULT CCloud::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szCloudTex,			pd3dDevice, m_pCloudTex, 0, 0 );
	TextureManager::LoadTexture ( m_szCloudShadowTex,	pd3dDevice, m_pCloudShadowTex, 0, 0 );

	return S_OK;
}

HRESULT CCloud::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawCloudSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudShadowSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawCloudShadowSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_REVSUBTRACT );	//목적지 - 전송원 
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,		D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudBaseSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawCloudBaseSB );
	}

	m_sCloud_1.fScale	= 2.0f;
	m_sCloud_1.vWind	= D3DXVECTOR2 ( 0.0f, 0.0f );

	m_sCloud_2.fScale	= 4.f;
	m_sCloud_2.vWind	= D3DXVECTOR2 ( 0.3f, 0.3f );

	m_sCloud_3.fScale	= 8.f;
	m_sCloud_3.vWind	= D3DXVECTOR2 ( 0.7f, 0.7f );

	m_sCloud_4.fScale	= 16.f;
	m_sCloud_4.vWind	= D3DXVECTOR2 ( 0.6f, 1.0f );

	SAFE_DELETE_ARRAY ( m_pSrcTex );
	m_pSrcTex = new D3DXVECTOR2[m_dwVertexNum];		// new ~~!!

	SAFE_DELETE_ARRAY ( m_pSrcTex_45 );
	m_pSrcTex_45 = new D3DXVECTOR2[m_dwVertexNum];		// new ~~!!	
	
	VERTEXCOLORTEX*	pVertices;
	SAFE_RELEASE ( m_pCloudVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertexNum*sizeof(VERTEXCOLORTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									VERTEXCOLORTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pCloudVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::RestoreDeviceObjects() -- CreateVertexBuffer() -- Failed" );
		return E_FAIL;
	}

	float	fLengthS = 0.1f;
	float	fLengthL = 1.0f;

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::RestoreDeviceObjects() -- m_pCloudVB->Lock() -- Failed" );
		return E_FAIL;
	}

	pVertices[0].vPos	= D3DXVECTOR3 ( -fLengthS,	0.0f, -fLengthL );
	pVertices[1].vPos	= D3DXVECTOR3 ( fLengthS,	0.0f, -fLengthL );

	pVertices[2].vPos	= D3DXVECTOR3 ( -fLengthL,	0.0f, -fLengthS );
	pVertices[3].vPos	= D3DXVECTOR3 ( -fLengthS,	1.0f, -fLengthS );
	pVertices[4].vPos	= D3DXVECTOR3 ( fLengthS,	1.0f, -fLengthS );
	pVertices[5].vPos	= D3DXVECTOR3 ( fLengthL,	0.0f, -fLengthS );

	pVertices[6].vPos	= D3DXVECTOR3 ( -fLengthL,	0.0f, fLengthS );
	pVertices[7].vPos	= D3DXVECTOR3 ( -fLengthS,	1.0f, fLengthS );
	pVertices[8].vPos	= D3DXVECTOR3 ( fLengthS,	1.0f, fLengthS );
	pVertices[9].vPos	= D3DXVECTOR3 ( fLengthL,	0.0f, fLengthS );

	pVertices[10].vPos	= D3DXVECTOR3 ( -fLengthS,	0.0f, fLengthL );
	pVertices[11].vPos	= D3DXVECTOR3 ( fLengthS,	0.0f, fLengthL );

	D3DXVECTOR3	vRotate;
	D3DXMATRIX	matRotate;
	D3DXMatrixRotationY ( &matRotate, D3DX_PI*0.25f );

	float fHana = (RANDOM_POS*2.f)+1.f;		// 1.f~3.f;	구름을 길게 늘이는데 사용하는 값이다.
	fHana = 1.f;

	for ( DWORD i=0; i<m_dwVertexNum; ++i )
	{
		pVertices[i].cColor = 0x00ffffff;
		pVertices[i].vTex	= D3DXVECTOR2 ( (pVertices[i].vPos.x+fLengthL)/(fLengthL*2.f), (pVertices[i].vPos.z+fLengthL)/(fLengthL*2.f) );
		m_pSrcTex[i]		= pVertices[i].vTex;		// 소스값 저장

		D3DXVec3TransformCoord ( &vRotate, &pVertices[i].vPos, &matRotate );
		m_pSrcTex_45[i]		= D3DXVECTOR2 ( (vRotate.x+fLengthL)/(fLengthL*2.f), (vRotate.z+fLengthL)/(fLengthL*2.f*fHana) );
	}

	pVertices[3].cColor = 0xffffffff;
	pVertices[4].cColor = 0xffffffff;
	pVertices[7].cColor = 0xffffffff;
	pVertices[8].cColor = 0xffffffff;

	m_pCloudVB->Unlock ();

	WORD*	pIndices;
	SAFE_RELEASE ( m_pCloudIB );
	hr = pd3dDevice->CreateIndexBuffer (	3*m_dwFacesNum*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pCloudIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::RestoreDeviceObjects() -- CreateIndexBuffer() -- Failed" );
		return E_FAIL;
	}

	WORD wFACES[]=//3*m_dwFacesNum] =
	{
		0,3,2,
		0,1,3,
		1,4,3,
		1,5,4,
		2,3,6,	//5
		3,7,6,
		3,4,7,
		4,8,7,
		4,5,8,
		5,9,8,	//10
		6,7,10,
		7,8,10,
		8,11,10,
		8,9,11,
	};

	hr = m_pCloudIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::RestoreDeviceObjects() -- m_pCloudIB->Lock() -- Failed" );
		return E_FAIL;
	}

	for ( DWORD i=0; i<m_dwFacesNum; ++i )
	{
		*pIndices++ = (WORD)(wFACES[i*3+0]);
		*pIndices++ = (WORD)(wFACES[i*3+1]);
		*pIndices++ = (WORD)(wFACES[i*3+2]);
	}
	m_pCloudIB->Unlock ();

	return S_OK;
}

HRESULT CCloud::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedCloudSB );
	SAFE_RELEASE( m_pDrawCloudSB );
	SAFE_RELEASE( m_pSavedCloudShadowSB );
	SAFE_RELEASE( m_pDrawCloudShadowSB );
	SAFE_RELEASE( m_pSavedCloudBaseSB );
	SAFE_RELEASE( m_pDrawCloudBaseSB );

	SAFE_DELETE_ARRAY ( m_pSrcTex );
	SAFE_DELETE_ARRAY ( m_pSrcTex_45 );

	SAFE_RELEASE ( m_pCloudVB );
	SAFE_RELEASE ( m_pCloudIB );

	return S_OK;
}

HRESULT CCloud::DeleteDeviceObjects()
{
	TextureManager::ReleaseTexture( m_szCloudTex, m_pCloudTex );
	TextureManager::ReleaseTexture( m_szCloudShadowTex, m_pCloudShadowTex );;

	return S_OK;
}

HRESULT CCloud::FrameMove ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT CCloud::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect )
{
	//if ( bReflect )	return S_OK;

	D3DXMATRIX		matScale, matWorld, matTempWorld;

	float fFogScaleXZ = 100000.f;
	float fFogScaleY = 12000.f;
	D3DXMatrixScaling ( &matScale, fFogScaleXZ, fFogScaleY, fFogScaleXZ );	// 스케일 조정
	D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	if ( bReflect )		CloudRender_Part ( pd3dDevice, m_sCloud_3, m_pSrcTex );		// 밝은 것 표현
	else				CloudRender_Part ( pd3dDevice, m_sCloud_2, m_pSrcTex );		// 밝은 것 표현

	matScale._42 -= 20.f;
	D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	if ( !bReflect )	CloudRender_Part ( pd3dDevice, m_sCloud_3, m_pSrcTex_45 );

	float fBlendFact	= GLPeriod::GetInstance().GetBlendFact();
	DWORD dwCloudyColor	= (DWORD)((fBlendFact*100.f)+40.f);	// 40~120 - 구름의 어두운 것을 표현할 때 쓰이는 값

	matScale._42 -= 100.f;
	D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	if ( bReflect )
	{
		dwCloudyColor	= (DWORD)((fBlendFact*100.f)+40.f);	
		CloudShadowRender_Part ( pd3dDevice, m_sCloud_3, m_pSrcTex, dwCloudyColor );		// 밝은 것 표현
	}
	else
	{
		dwCloudyColor	= (DWORD)((fBlendFact*100.f)+40.f);	// 40~120 - 구름의 어두운 것을 표현할 때 쓰이는 값
		CloudShadowRender_Part ( pd3dDevice, m_sCloud_2, m_pSrcTex, dwCloudyColor );	// 어두운 것 표현
	}

	matScale._42 -= 20.f;
	D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
	if ( !bReflect )	CloudShadowRender_Part ( pd3dDevice, m_sCloud_3, m_pSrcTex_45, dwCloudyColor );

	float fFact = fabsf(fBlendFact-0.5f)*2.f;	// 1.f~0.f~1.f
	float fSrcFact = 1.f - fFact;				// 0.f~1.f~0.f



	//	Note : 비나 눈이 내리거때만..
	//
	if ( (m_fWeatherRate>0.f) )
	{
		DWORD	dwColorR, dwColorG, dwColorB;
		float fFact = fabsf(fBlendFact-0.5f)*2.f;	// 1.f~0.f~1.f
		float fSrcFact = 1.f - fFact;				// 0.f~1.f~0.f

		fSrcFact = fSrcFact - m_fWeatherRate;
		if ( fSrcFact < 0.f )	fSrcFact = 0.f;

		if ( GLPeriod::GetInstance().GetHour() < 12 )	// 새벽녁
		{
			dwColorR = (DWORD)(255.f*fSrcFact);
			dwColorG = (DWORD)(255.f*fSrcFact);
			dwColorB = (DWORD)(0.f*fSrcFact);
		}
		else										// 저녁무렵
		{
			dwColorR = (DWORD)(255.f*fSrcFact);
			dwColorG = (DWORD)(255.f*fSrcFact);
			dwColorB = (DWORD)(0.f*fSrcFact);
		}

		dwColorR += 100;
		dwColorG += 100;
		dwColorB += 100;

		if ( dwColorR > 255 )	dwColorR = 255;
		if ( dwColorG > 255 )	dwColorG = 255;
		if ( dwColorB > 255 )	dwColorB = 255;

		matScale._42 -= 30.f;
		D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
		dwCloudyColor = (dwColorR<<16) + (dwColorG<<8) + dwColorB;
		if ( bReflect )	CloudyRender_Part ( pd3dDevice, m_sCloud_2, dwCloudyColor );	// 날씨가 흐릴때 발동 걸린다.
		else			CloudyRender_Part ( pd3dDevice, m_sCloud_1, dwCloudyColor );	// 날씨가 흐릴때 발동 걸린다.
	}

	return S_OK;
}

void CCloud::SetData ( float fElapsedTime, D3DXVECTOR3 vSkyCenter, float fWeatherRate )
{
	m_fElapsedTime	= fElapsedTime;
	m_vSkyCenter	= vSkyCenter;
	m_fWeatherRate	= fWeatherRate;
}

void CCloud::CloudRender_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud )
{
	HRESULT hr(S_OK);

	D3DXMATRIX	matWorld, matScale;
	D3DXMatrixIdentity ( &matScale );
	float fFogScaleXZ = 100000.f;
	float fFogScaleY = 12000.f;
	D3DXMatrixScaling ( &matScale, fFogScaleXZ, fFogScaleY, fFogScaleXZ );		// 스케일 조정

	matScale._41 = m_vSkyCenter.x;
	matScale._42 = m_vSkyCenter.y;
	matScale._43 = m_vSkyCenter.z;

	D3DXMatrixMultiply ( &matWorld, &matScale, &DxSkyMan::m_matWorld );	// 회전 조정
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );


	VERTEXCOLORTEX*	pVertices;

	DWORD	dwColor = (DWORD)(255.f*0.5f);
	dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::CloudRender_Reflect() -- m_pCloudVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertexNum; i++ )
	{	
		pVertices[i].cColor = 0x00000000;
		pVertices[i].vTex = m_pSrcTex[i]*sCloud.fScale;
	}

	pVertices[3].cColor = dwColor;
	pVertices[4].cColor = dwColor;
	pVertices[7].cColor = dwColor;
	pVertices[8].cColor = dwColor;

	m_pCloudVB->Unlock ();

	m_pSavedCloudSB->Capture();
	m_pDrawCloudSB->Apply();

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCloudVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pCloudIB );

	pd3dDevice->SetTexture ( 0, m_pCloudShadowTex );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFacesNum );

	m_pSavedCloudSB->Apply();
}

void CCloud::CloudRender_Base ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud )
{
	HRESULT hr(S_OK);

	VERTEXCOLORTEX*	pVertices;

	DWORD	dwColor = (DWORD)(255.f*0.5f);
	dwColor = 0xff000000 + (dwColor<<16) + (dwColor<<8) + dwColor;

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::CloudRender_Base() -- m_pCloudVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertexNum; i++ )
	{	
		pVertices[i].cColor = 0x00000000;
		pVertices[i].vTex = m_pSrcTex[i]*sCloud.fScale;
	}

	pVertices[3].cColor = dwColor;
	pVertices[4].cColor = dwColor;
	pVertices[7].cColor = dwColor;
	pVertices[8].cColor = dwColor;

	m_pCloudVB->Unlock ();

	m_pSavedCloudSB->Capture();
	m_pDrawCloudSB->Apply();

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCloudVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pCloudIB );

	pd3dDevice->SetTexture ( 0, m_pCloudShadowTex );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFacesNum );

	m_pSavedCloudSB->Apply();
}

void CCloud::CloudRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, D3DXVECTOR2* pTex )
{
	HRESULT hr(S_OK);

	VERTEXCOLORTEX*	pVertices;

	sCloud.vAddMove += sCloud.vWind*m_fElapsedTime*0.001f;

	DWORD dwColor = GetSkyColor ();

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::CloudRender_Part() -- m_pCloudVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertexNum; i++ )
	{	
		pVertices[i].cColor = dwColor;
		pVertices[i].vTex = (pTex[i]+sCloud.vAddMove)*sCloud.fScale;
	}

//	dwColor += ((DWORD)(255.f*(1.f-m_fWeatherRate))) <<24;				// 중앙만 특별하다.
	dwColor += 0xff000000;
	pVertices[3].cColor = dwColor;
	pVertices[4].cColor = dwColor;
	pVertices[7].cColor = dwColor;
	pVertices[8].cColor = dwColor;

	m_pCloudVB->Unlock ();

	m_pSavedCloudShadowSB->Capture();
	m_pDrawCloudShadowSB->Apply();

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCloudVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pCloudIB );

	pd3dDevice->SetTexture ( 0, m_pCloudTex );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFacesNum );

	m_pSavedCloudShadowSB->Apply();
}

void CCloud::CloudShadowRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, D3DXVECTOR2* pTex, DWORD dwColor )
{
	HRESULT hr(S_OK);

	VERTEXCOLORTEX*	pVertices;

	sCloud.vAddMove += sCloud.vWind*m_fElapsedTime*0.001f;

	D3DCOLOR	cColor = ( (DWORD)(dwColor)<<16 )+( (DWORD)(dwColor)<<8 )+(DWORD)(dwColor);

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::CloudShadowRender_Part() -- m_pCloudVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertexNum; i++ )
	{	
		pVertices[i].cColor = cColor;
		pVertices[i].vTex	= (pTex[i]+sCloud.vAddMove)*sCloud.fScale;
	}

	//cColor += ((DWORD)(255.f*fFact))<<24;
	cColor += (255)<<24;
	pVertices[3].cColor = cColor;
	pVertices[4].cColor = cColor;
	pVertices[7].cColor = cColor;
	pVertices[8].cColor = cColor;

	m_pCloudVB->Unlock ();

	m_pSavedCloudShadowSB->Capture();
	m_pDrawCloudShadowSB->Apply();

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCloudVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pCloudIB );

	pd3dDevice->SetTexture ( 0, m_pCloudShadowTex );		// 텍스쳐 설정

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFacesNum );

	m_pSavedCloudShadowSB->Apply();
}

void CCloud::CloudyRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, DWORD dwColor )
{
	HRESULT hr(S_OK);

	VERTEXCOLORTEX*	pVertices;

	sCloud.vAddMove += sCloud.vWind*m_fElapsedTime*0.001f;

//	dwColor = GetSkyColor ();

	hr = m_pCloudVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "CCloud::CloudyRender_Part() -- m_pCloudVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertexNum; i++ )
	{	
		pVertices[i].cColor = dwColor;
		pVertices[i].vTex	= (m_pSrcTex[i]+sCloud.vAddMove)*sCloud.fScale;
	}

	dwColor += ((DWORD)(255*m_fWeatherRate*0.5f))<<24;//0xff000000;
	pVertices[3].cColor = dwColor;
	pVertices[4].cColor = dwColor;
	pVertices[7].cColor = dwColor;
	pVertices[8].cColor = dwColor;

	m_pCloudVB->Unlock ();

	m_pSavedCloudShadowSB->Capture();
	m_pDrawCloudShadowSB->Apply();

	DWORD	dwColorOP, dwAlphaOP;
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,		&dwColorOP );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_ALPHAOP,		&dwAlphaOP );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG2 );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pCloudVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pCloudIB );

	pd3dDevice->SetTexture ( 0, m_pCloudShadowTex );		// 텍스쳐 설정

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertexNum, 0, m_dwFacesNum );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		dwColorOP );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		dwAlphaOP );

	m_pSavedCloudShadowSB->Apply();
}

DWORD CCloud::GetSkyColor ()
{
	float fBlueFact = GLPeriod::GetInstance().GetBlueFact();
	float fRedFact	= GLPeriod::GetInstance().GetRedFact();

	float fBlendFact = GLPeriod::GetInstance().GetBaseFact();
	float fFact		= fBlendFact - fBlueFact - fRedFact;
	if ( fFact<=0.f )	fFact = 0.f;
	float fSumFact	= ((1.f-fFact)*0.6f) + (m_fWeatherRate*0.7f);
	DWORD dwColor	= (DWORD) ( 255.f - (fSumFact*170.f) );

	if ( (fBlueFact==0.f) && (fRedFact==0.f) )				// 새벽색과 노을색이 없을 겨우 걍 넘어간다.
	{
		return (dwColor<<16)+(dwColor<<8)+(dwColor);
	}

	DWORD	dwColorR = dwColor;
	DWORD	dwColorG = dwColor;
	DWORD	dwColorB = dwColor;

	if ( fBlueFact != 0.f )
	{
		dwColorR += (DWORD)(80.f*fBlueFact);
		dwColorG += (DWORD)(80.f*fBlueFact);
		dwColorB += (DWORD)(255.f*fBlueFact);
	}
	else if ( fRedFact != 0.f )
	{
		dwColorR += (DWORD)(255.f*fRedFact);
		dwColorG += (DWORD)(50.f*fRedFact);
		dwColorB += (DWORD)(40.f*fRedFact);
	}

	if ( dwColorR > 255 ) dwColorR = 255;
	if ( dwColorG > 255 ) dwColorG = 255;
	if ( dwColorB > 255 ) dwColorB = 255;

	return (dwColorR<<16)+(dwColorG<<8)+dwColorB;
}