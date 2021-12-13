#include "pch.h"

#include "DebugSet.h"
#include "Collision.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "editmeshs.h"
#include "./glperiod.h"
#include "./GLDefine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxLightMan DxLightMan::InstanceDefault;
DxLightMan*	DxLightMan::pInstance = NULL;

namespace
{
	D3DCOLORVALUE COLORBLEND ( const D3DCOLORVALUE& clValue, const D3DCOLORVALUE& crValue, const float fblendfact )
	{
		D3DCOLORVALUE cBlended;

		float finvblendfact = 1-fblendfact;
		cBlended.a = clValue.a*(finvblendfact) + crValue.a*fblendfact;
		cBlended.r = clValue.r*(finvblendfact) + crValue.r*fblendfact;
		cBlended.g = clValue.g*(finvblendfact) + crValue.g*fblendfact;
		cBlended.b = clValue.b*(finvblendfact) + crValue.b*fblendfact;

		return cBlended;
	}

	D3DLIGHTQ LIGHTBLEND ( const D3DLIGHTQ &m_Light, const float fblendfact )
	{
		D3DCOLORVALUE cFull = {1,1,1,1};

		D3DLIGHTQ d3dLight = m_Light;

		d3dLight.Ambient = COLORBLEND ( m_Light.Ambient, cFull, fblendfact );
		d3dLight.Diffuse = COLORBLEND ( m_Light.Diffuse, cFull, fblendfact );
		d3dLight.Specular = COLORBLEND ( m_Light.Specular, cFull, fblendfact );

		return d3dLight;
	}
};

void DXLIGHT::SetRange ( float fRange )
{
	SerialData ();
}

void DXLIGHT::SerialData ()
{
	m_Light.Position.x = m_matWorld._41;
	m_Light.Position.y = m_matWorld._42;
	m_Light.Position.z = m_matWorld._43;

	//	Note : 위치 수성시에 AABB-Tree 또한 재정렬 한다.
	//
	DxLightMan::GetInstance()->MakeAABBTree ();
}

HRESULT DXLIGHT::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	-----------------------------[테스트용]--------------------------------
	//
	EDITMESHS::RENDERSPHERE ( pd3dDevice, m_Light.Position, m_Light.Range );
	return S_OK;
}

DxLightMan::DxLightMan(void) :
	m_pLtListHead(NULL),
	m_bLighting(TRUE),
	m_bWhenDayPointOff(FALSE),
	m_bNightAndDay(TRUE),
	m_dwRenderCount(0),
	m_dwRegistCount(0),
	m_dwBackRenderCount(0),
	m_pLtTest(NULL),
	m_pTreeHead(NULL),
	m_bRendRange(FALSE),
	m_bDSPName(FALSE),

	m_fThunderTimer(0.0f),
	m_nThunder(0),
	m_nThunderFULL(0),
	m_fThunderPOWER(0.f),

	m_dwDPLight(0),
	m_dwDPLight_Count(0),
	m_dwSaveDPLight_Count(0)
{
	SetDefault ();

	StringCchCopy( m_LtDirectNoon.m_szLightName, STRING_LENGTH_256, "DIRECTIONAL-[NOON]" );
	StringCchCopy( m_LtDirectNight.m_szLightName, STRING_LENGTH_256, "DIRECTIONAL-[NIGHT]" );

	D3DUtil_InitLight ( m_LtDirectNow.m_Light, D3DLIGHT_DIRECTIONAL, 0.0f, 0.0f, 0.0f );
	m_LtDirectNow.m_Light.Diffuse.r = 0.f;
	m_LtDirectNow.m_Light.Diffuse.g = 0.f;
	m_LtDirectNow.m_Light.Diffuse.b = 0.f;
	m_LtDirectNow.m_Light.Direction = D3DXVECTOR3(0,-1,0);

	D3DUtil_InitLight ( m_sThunderLight, D3DLIGHT_DIRECTIONAL, 0.0f, 0.0f, 0.0f );
	m_sThunderLight.Diffuse.r = 0.f;
	m_sThunderLight.Diffuse.g = 0.f;
	m_sThunderLight.Diffuse.b = 0.f;
	m_sThunderLight.Direction = D3DXVECTOR3(0,-1,0);

	for ( DWORD i=0; i<_MAX_LIGHT; i++ )
	{
		D3DUtil_InitLight ( m_sDPLight[i], D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
		D3DUtil_InitLight ( m_sSaveDPLight[i], D3DLIGHT_POINT, 0.0f, 0.0f, 0.0f );
	}
}

DxLightMan::~DxLightMan(void)
{
	CleanUp ();

	//	Note : 현제 인스턴스가 삭제되는대 이것이 기본 LightMan으로 등록되어 있으면
	//		초기화 함.
	if ( this == DxLightMan::pInstance )
	{
		DxLightMan::pInstance = NULL;
	}
}

DXLIGHT* DxLightMan::GetDirectLight ()
{
	return &m_LtDirectNow;
}

DXLIGHT* DxLightMan::GetDirectNoonLight ()
{
	return &m_LtDirectNoon;
}

DXLIGHT* DxLightMan::GetDirectNightLight ()
{
	return &m_LtDirectNight;
}

void DxLightMan::CleanUp ()
{
	m_LtDirectNoon.SetDefault ();
	m_LtDirectNight.SetDefault ();

	m_pLtTest = NULL;

	m_dwRenderCount = 0;
	m_dwRegistCount = 0;
	m_dwBackRenderCount = 0;

	m_vecPointLight.clear();
	m_mmapClosedLight.clear ();
	SAFE_DELETE(m_pLtListHead);
	SAFE_DELETE(m_pTreeHead);
}

void DxLightMan::SetDefault ()
{
	m_LtDirectNoon.SetDefault ();
	m_LtDirectNight.SetDefault ();
}

void DxLightMan::SetDefDirect ( const DXLIGHT& ltDirectional )
{
	InstanceDefault.m_LtDirectNow = ltDirectional;
	InstanceDefault.m_LtDirectNoon = ltDirectional;
	InstanceDefault.m_LtDirectNight = ltDirectional;
}

void DxLightMan::SetDirectNoonLight ( const DXLIGHT& ltDirectional )
{
	m_LtDirectNoon = ltDirectional;
}

void DxLightMan::SetDirectNightLight ( const DXLIGHT& ltDirectional )
{
	m_LtDirectNight = ltDirectional;
}

HRESULT DxLightMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	m_pd3dDevice = pd3dDevice;

	DWORD dwD3dColor = D3DCOLOR_COLORVALUE ( m_LtDirectNoon.m_Light.Ambient.r, m_LtDirectNoon.m_Light.Ambient.g,
		m_LtDirectNoon.m_Light.Ambient.b, 0 );
	dwD3dColor = D3DCOLOR_COLORVALUE ( 0, 0, 0, 0 );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColor );

	m_pd3dDevice->SetLight ( 0, &m_LtDirectNoon.m_Light );
	m_pd3dDevice->LightEnable ( 0, m_LtDirectNoon.m_bEnable );
	m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, m_bLighting );

	pd3dDevice->LightEnable ( 1, FALSE );
	pd3dDevice->LightEnable ( 2, FALSE );
	pd3dDevice->LightEnable ( 3, FALSE );
	pd3dDevice->LightEnable ( 4, FALSE );
	pd3dDevice->LightEnable ( 5, FALSE );
	pd3dDevice->LightEnable ( 6, FALSE );
	pd3dDevice->LightEnable ( 7, FALSE );

	return S_OK;
}

HRESULT DxLightMan::DeleteDeviceObjects ()
{
	return S_OK;
}

void DxLightMan::Thunder ()
{
	// 반짝임 횟수 "5~9"
	m_nThunderFULL = m_nThunder = rand()%5 + 5;
}

HRESULT DxLightMan::FrameMove ( float fTime, float fElapsedTime )
{
	Clone2DPLight ();

	//	밤낫 변화가 없을 때에는 낫을 기준으로함.
	if ( !m_bNightAndDay )
	{
		m_LtDirectNow = m_LtDirectNoon;
	}
	else
	{
		float fBlendFact = GLPeriod::GetInstance().GetBlendFact ();

		if ( GLPeriod::GetInstance().IsNoon() )	m_LtDirectNow.m_bEnable = m_LtDirectNoon.m_bEnable;
		else								m_LtDirectNow.m_bEnable = m_LtDirectNight.m_bEnable;

		D3DLIGHTQ &LightNow = m_LtDirectNow.m_Light;
		const D3DLIGHTQ &LightNoon = m_LtDirectNoon.m_Light;
		const D3DLIGHTQ &LightNight = m_LtDirectNight.m_Light;

		//	일반 값들 초기화.
		LightNow = LightNoon;

		m_LtDirectNow.m_BaseDiffuse = COLORBLEND ( m_LtDirectNow.m_BaseDiffuse, m_LtDirectNow.m_BaseDiffuse, fBlendFact );

		LightNow.Ambient = COLORBLEND ( LightNight.Ambient, LightNoon.Ambient, fBlendFact );
		LightNow.Diffuse = COLORBLEND ( LightNight.Diffuse, LightNoon.Diffuse, fBlendFact );
		LightNow.Specular = COLORBLEND ( LightNight.Specular, LightNoon.Specular, fBlendFact );


		float fFADEFACT = 0.001f;
		const D3DXCOLOR cEvening(0xFF/2,0xAE/2,0x00,0x00);
		const D3DXCOLOR cMorning(0x00,0x36/2,0x99/2,0x00);

		//	아침일때.
		if ( GLPeriod::GetInstance().IsFadeInTime() )
		{
			float fHalfSec = abs ( static_cast<float> ( GLPeriod::GetInstance().GetSecond() - (GLPeriod::TIME_SEC/2) ) );
			fFADEFACT *= ( 1.0f - fHalfSec/(GLPeriod::TIME_SEC/2) );

			LightNow.Ambient = COLORBLEND ( LightNow.Ambient, cMorning, fFADEFACT );
			LightNow.Diffuse = COLORBLEND ( LightNow.Diffuse, cMorning, fFADEFACT );
		}

		//	저녁일때.
		if ( GLPeriod::GetInstance().IsFadeOutTime() )
		{
			float fHalfSec = abs ( static_cast<float> ( GLPeriod::GetInstance().GetSecond() - (GLPeriod::TIME_SEC/2) ) );
			fFADEFACT *= ( 1.0f - fHalfSec/(GLPeriod::TIME_SEC/2) );

			LightNow.Ambient = COLORBLEND ( LightNow.Ambient, cEvening, fFADEFACT );
			LightNow.Diffuse = COLORBLEND ( LightNow.Diffuse, cEvening, fFADEFACT );
		}
	}

	if ( m_nThunder> 0 )
	{
		m_fThunderTimer += fElapsedTime;

		if ( m_fThunderTimer > 0.09f )
		{
			m_sThunderLight = m_LtDirectNow.m_Light;

			m_fThunderPOWER = 0.f;
			float fDiffuseAdd = (float) 0.2f * RANDOM_POS;	// 번쩍임 강도 범위 "0.0f~0.21f"

			if ( (m_nThunderFULL==m_nThunder) || (1==m_nThunder) )
			{
				m_fThunderPOWER = fDiffuseAdd*0.3f;
				m_sThunderLight.Diffuse.r += m_fThunderPOWER;
				m_sThunderLight.Diffuse.g += m_fThunderPOWER;
				m_sThunderLight.Diffuse.b += m_fThunderPOWER;
			}
			else
			{
				m_fThunderPOWER = 0.1f+fDiffuseAdd;
				m_sThunderLight.Diffuse.r += m_fThunderPOWER;
				m_sThunderLight.Diffuse.g += m_fThunderPOWER;
				m_sThunderLight.Diffuse.b += m_fThunderPOWER;
			}

			--m_nThunder;
			m_fThunderTimer = 0.0f;
		}

		m_LtDirectNow.m_Light = m_sThunderLight;
	}

	return S_OK;
}

void DxLightMan::TreeRender ( DXLIGHTNODE *pLightNode, LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv )
{
	GASSERT ( pLightNode );
	if ( !pLightNode->IsCollisionVolume(cv) )	return;

	if ( m_bRendRange ) //	테스트용.
	{
		//EDITMESHS::RENDERAABB ( m_pd3dDevice, pLightNode->vMax, pLightNode->vMin );

		if ( pLightNode->pLight )	//pLightNode->pLight->m_bEditCheck
			EDITMESHS::RENDERSPHERE ( pd3dDevice, pLightNode->pLight->m_Light.Position, pLightNode->pLight->m_Light.Range );
	}

	if( m_bDSPName )	
	{
		if( pLightNode->pLight )
		{
			D3DXVECTOR3 vOut;
			D3DXVECTOR3 vSrc( pLightNode->pLight->m_matWorld._41, pLightNode->pLight->m_matWorld._42, pLightNode->pLight->m_matWorld._43 );
			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			D3DXVec3Project( &vOut, &vSrc, &DxViewPort::GetInstance().GetViewPort(), &DxViewPort::GetInstance().GetMatProj(), 
								&DxViewPort::GetInstance().GetMatView(), &matIdentity );
			CDebugSet::ToPos( vOut.x, vOut.y, "%s", pLightNode->pLight->m_szLightName );
		}
	}

	DXLIGHT *pLight = pLightNode->pLight;
	if ( pLight && pLight->m_bEnable )
	{
		//	Note : 라이트를 활성화 시킴.
		//
		D3DXVECTOR3 vLookAt = DxViewPort::GetInstance().GetLookatPt ();
		D3DXVECTOR3 vDistance = vLookAt - pLight->m_Light.Position;

		float fDistance = (vDistance.x*vDistance.x)+(vDistance.z*vDistance.z);
		m_mmapClosedLight.insert ( std::make_pair(fDistance,pLight) );
	}
	else
	{
		if( pLightNode->pLeftChild )	TreeRender( pLightNode->pLeftChild, pd3dDevice, cv );
		if( pLightNode->pRightChild )	TreeRender( pLightNode->pRightChild, pd3dDevice, cv );
	}
}

HRESULT DxLightMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_dwRenderCount = 0;

	//	Note  켜져있던 LIGHT 를 모두 끈다.
	//
	while ( m_dwBackRenderCount-- )		pd3dDevice->LightEnable ( m_dwBackRenderCount, FALSE );

	//	Note : 라이트 전체의 활성/비활성.
	//
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, m_bLighting );

	//	Note : Ambient 컬러 설정.
	//
	const D3DCOLORVALUE &cDirectAmbient = GetDirectLight()->m_Light.Ambient;
	DWORD dwD3dColor = D3DCOLOR_COLORVALUE ( cDirectAmbient.r, cDirectAmbient.g, cDirectAmbient.b, 0 );
	dwD3dColor = D3DCOLOR_COLORVALUE ( 0, 0, 0, 0 );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColor );

	//	Note : Directional Light.
	//
	if ( m_LtDirectNoon.m_bEnable )
	{
		pd3dDevice->SetLight ( m_dwRenderCount, &GetDirectLight()->m_Light );
		pd3dDevice->LightEnable ( m_dwRenderCount, GetDirectLight()->m_bEnable );
		++m_dwRenderCount;
	}

	//	-----------------------------[테스트용]--------------------------------
	//	Note : Test Light.
	//
	if ( m_pLtTest && m_pLtTest->m_bEnable )
	{
		m_pLtTest->m_Light.Position = D3DXVECTOR3( m_pLtTest->m_matWorld._41, m_pLtTest->m_matWorld._42, m_pLtTest->m_matWorld._43 );
		pd3dDevice->SetLight ( m_dwRenderCount, &m_pLtTest->m_Light );
		pd3dDevice->LightEnable ( m_dwRenderCount, m_pLtTest->m_bEnable );
		++m_dwRenderCount;
	}

	//	Note : 라이트 활성화 / AABBTree
	//
	//	낮에는 포인트 라이트가 동작 안할 때는 낮인지 검사.
	BOOL bPointLightOn = TRUE;
	if ( m_bWhenDayPointOff && GLPeriod::GetInstance().IsOffLight() )	bPointLightOn = FALSE;
	if ( !bPointLightOn )
	{
		m_dwBackRenderCount = 0;
		m_mmapClosedLight.clear();
	}

	float fBlendFact;
	if ( !m_bWhenDayPointOff )	fBlendFact = 0.f;										// 포인트 라이트 계속 켜 놓기
	else						fBlendFact = GLPeriod::GetInstance().GetBlendFact() * 2.f;	// 0~1.f 값. (1 낮에 가깝다) (0 밤에 가깝다.)
	
	if ( bPointLightOn )
	{
		if ( m_pTreeHead )
		{
			m_mmapClosedLight.clear();
			TreeRender ( m_pTreeHead, pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );

			CLMAN_ITER iter = m_mmapClosedLight.begin();
			CLMAN_ITER iter_end = m_mmapClosedLight.end();
			for ( ; iter!=iter_end && m_dwRenderCount<_MAX_LIGHT; ++iter, ++m_dwRenderCount )
			{
				PDXLIGHT pLight = (*iter).second;
				pLight->m_Light.Position = D3DXVECTOR3( pLight->m_matWorld._41, pLight->m_matWorld._42, pLight->m_matWorld._43 ); // 값은 바꿔준다.
		
				D3DLIGHTQ sLight = pLight->m_Light;

				D3DXCOLOR darkcolor(0,0,0,0);
				sLight.Diffuse = COLORBLEND ( sLight.Diffuse, darkcolor, fBlendFact );
				sLight.Specular = COLORBLEND ( sLight.Specular, darkcolor, fBlendFact );

				pd3dDevice->SetLight ( m_dwRenderCount, &sLight );
				pd3dDevice->LightEnable ( m_dwRenderCount, pLight->m_bEnable );
			}
		}
	
		m_dwBackRenderCount = m_dwRenderCount;
	}
	else
	{
		pd3dDevice->LightEnable( 1, FALSE );
		pd3dDevice->LightEnable( 2, FALSE );
		pd3dDevice->LightEnable( 3, FALSE );
		pd3dDevice->LightEnable( 4, FALSE );
		pd3dDevice->LightEnable( 5, FALSE );
		pd3dDevice->LightEnable( 6, FALSE );
		pd3dDevice->LightEnable( 7, FALSE );
	}

	return S_OK;
}

void DxLightMan::ReSetLight( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_bWhenDayPointOff && GLPeriod::GetInstance().IsOffLight() )
	{
		pd3dDevice->LightEnable( 1, FALSE );
		pd3dDevice->LightEnable( 2, FALSE );
		pd3dDevice->LightEnable( 3, FALSE );
		pd3dDevice->LightEnable( 4, FALSE );
		pd3dDevice->LightEnable( 5, FALSE );
		pd3dDevice->LightEnable( 6, FALSE );
		pd3dDevice->LightEnable( 7, FALSE );

		return;
	}

	DWORD dwRenderCount = 1;

	float fBlendFact(0.f);
	if( !m_bWhenDayPointOff )	fBlendFact = 0.f;										// 포인트 라이트 계속 켜 놓기
	else						fBlendFact = GLPeriod::GetInstance().GetBlendFact() * 2.f;	// 0~1.f 값. (1 낮에 가깝다) (0 밤에 가깝다.)

	CLMAN_ITER iter = m_mmapClosedLight.begin();
	CLMAN_ITER iter_end = m_mmapClosedLight.end();
	for ( ; iter!=iter_end && dwRenderCount<_MAX_LIGHT; ++iter, ++dwRenderCount )
	{
		PDXLIGHT pLight = (*iter).second;
		pLight->m_Light.Position = D3DXVECTOR3( pLight->m_matWorld._41, pLight->m_matWorld._42, pLight->m_matWorld._43 ); // 값은 바꿔준다.

		D3DLIGHTQ sLight = pLight->m_Light;

		D3DXCOLOR darkcolor(0,0,0,0);
		sLight.Diffuse = COLORBLEND ( sLight.Diffuse, darkcolor, fBlendFact );
		sLight.Specular = COLORBLEND ( sLight.Specular, darkcolor, fBlendFact );

		pd3dDevice->SetLight ( dwRenderCount, &sLight );
		pd3dDevice->LightEnable ( dwRenderCount, pLight->m_bEnable );
	}
}

void DxLightMan::LightDisable34567( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );
	pd3dDevice->LightEnable( 5, FALSE );
	pd3dDevice->LightEnable( 6, FALSE );
	pd3dDevice->LightEnable( 7, FALSE );
}

DXLIGHT* DxLightMan::GetLight ( const char* szLightName )
{
	DXLIGHT* pLightCur = m_pLtListHead;
	while ( pLightCur )
	{
		if ( !strcmp(pLightCur->m_szLightName,szLightName) )
			return pLightCur;

		pLightCur = pLightCur->pNext;
	}

	return NULL;
}

void DxLightMan::AddLight ( DXLIGHT* pLight )
{
	GASSERT(pLight);
	GASSERT(!pLight->pNext);

	pLight->pNext = m_pLtListHead;
	m_pLtListHead = pLight;

	// Note : List를 vector에도 적용 시켜준다.
	ListToVectorLIGHT();

	m_dwRegistCount++;
}

void DxLightMan::DelLight ( const char* szLightName )
{
	GASSERT(szLightName);
	DXLIGHT *pLightCur, *pLightBack;

	if ( !strcmp(m_pLtListHead->m_szLightName,szLightName) )
	{
		pLightCur = m_pLtListHead;
		m_pLtListHead = m_pLtListHead->pNext;

		pLightCur->pNext = NULL;
		delete pLightCur;

		// Note : List를 vector에도 적용 시켜준다.
		ListToVectorLIGHT();

		m_dwRegistCount--;
		return;
	}

	pLightCur = m_pLtListHead;
	while ( pLightCur )
	{
		if ( !strcmp(pLightCur->m_szLightName,szLightName) )
		{
			pLightBack->pNext = pLightCur->pNext;

			pLightCur->pNext = NULL;
			delete pLightCur;

			// Note : List를 vector에도 적용 시켜준다.
			ListToVectorLIGHT();

			m_dwRegistCount--;
			return;
		}

		pLightBack = pLightCur;
		pLightCur = pLightCur->pNext;
	}
}

void DxLightMan::TestLight ( DXLIGHT* pLight )
{
	m_pLtTest = pLight;
}

void DxLightMan::ResetTestLight ()
{
	m_pLtTest = NULL;
}

void DxLightMan::MakeAABBTree ()
{
	SAFE_DELETE(m_pTreeHead);

	DWORD dwLightCount = 0;
	DXLIGHT* pLightCur;

	//	Note : 갯수 파악.
	//
	pLightCur = m_pLtListHead;
	while ( pLightCur )
	{
		dwLightCount++;
		pLightCur = pLightCur->pNext;
	}

	if ( dwLightCount == 0 )	return;
	
	//	Note : 라이트 포인터 배열에 저장후 정렬수행.
	//
	PDXLIGHT *ppLightArray = new PDXLIGHT[dwLightCount];

	dwLightCount = 0;
	pLightCur = m_pLtListHead;
	while ( pLightCur )
	{
		ppLightArray[dwLightCount++] = pLightCur;
		pLightCur = pLightCur->pNext;
	}

	D3DXVECTOR3 vMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3 vMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	for ( DWORD i=0; i<dwLightCount; i++ )
	{
		float fRange = ppLightArray[i]->m_Light.Range;

		ppLightArray[i]->m_Light.Position = D3DXVECTOR3( ppLightArray[i]->m_matWorld._41, ppLightArray[i]->m_matWorld._42, ppLightArray[i]->m_matWorld._43 );
		D3DXVECTOR3 vPos = ppLightArray[i]->m_Light.Position;

		float fMaxX = vPos.x + fRange;
		float fMaxY = vPos.y + fRange;
		float fMaxZ = vPos.z + fRange;
		float fMinX = vPos.x - fRange;
		float fMinY = vPos.y - fRange;
		float fMinZ = vPos.z - fRange;

		if ( vMax.x < fMaxX )	vMax.x = fMaxX;
		if ( vMax.y < fMaxY )	vMax.y = fMaxY;
		if ( vMax.z < fMaxZ )	vMax.z = fMaxZ;

		if ( vMin.x > fMinX )	vMin.x = fMinX;
		if ( vMin.y > fMinY )	vMin.y = fMinY;
		if ( vMin.z > fMinZ )	vMin.z = fMinZ;
	}

	m_pTreeHead = new DXLIGHTNODE;
	m_pTreeHead->vMax = vMax;
	m_pTreeHead->vMin = vMin;
	MakeAABBTree ( ppLightArray, dwLightCount, m_pTreeHead, vMax, vMin );

	SAFE_DELETE(ppLightArray);
}

void DxLightMan::MakeAABBTree ( PDXLIGHT *ppLightArray, DWORD dwArraySize, DXLIGHTNODE* pLightNode,
								const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	if ( dwArraySize == 1 )
	{
		float fRange = ppLightArray[0]->m_Light.Range;
		D3DXVECTOR3 vPos = ppLightArray[0]->m_Light.Position;

		pLightNode->fMaxX = vPos.x + fRange;
		pLightNode->fMaxY = vPos.y + fRange;
		pLightNode->fMaxZ = vPos.z + fRange;
		pLightNode->fMinX = vPos.x - fRange;
		pLightNode->fMinY = vPos.y - fRange;
		pLightNode->fMinZ = vPos.z - fRange;

		pLightNode->pLight = ppLightArray[0];
		return;
	}

	float SizeX = vMax.x - vMin.x;
	float SizeY = vMax.y - vMin.y;
	float SizeZ = vMax.z - vMin.z;

	D3DXVECTOR3 vDivMax = vMax;
	D3DXVECTOR3 vDivMin = vMin;

	if ( SizeX > SizeY && SizeX > SizeZ )	vDivMin.x = vMin.x + SizeX/2;
	else if ( SizeY > SizeZ )				vDivMin.y = vMin.y + SizeY/2;
	else									vDivMin.z = vMin.z + SizeZ/2;

	DWORD dwLeftCount = 0, dwRightCount = 0;
	PDXLIGHT* pLeftArray = new PDXLIGHT[dwArraySize];
	PDXLIGHT* pRightArray = new PDXLIGHT[dwArraySize];

	DWORD i;
	for ( i=0; i<dwArraySize; i++ )
	{
		if ( COLLISION::IsWithInPoint(vDivMax,vDivMin,ppLightArray[i]->m_Light.Position) )
			pLeftArray[dwLeftCount++] = ppLightArray[i];
		else
			pRightArray[dwRightCount++] = ppLightArray[i];
	}

	if ( dwLeftCount == 0 || dwRightCount == 0 )
	{
		DWORD dwDivCount = dwArraySize / 2;
	
		dwLeftCount = 0;
		dwRightCount = 0;

		for ( i=0; i<dwDivCount; i++ )
			pLeftArray[dwLeftCount++] = ppLightArray[i];

		for ( i=dwDivCount; i<dwArraySize; i++ )
			pRightArray[dwRightCount++] = ppLightArray[i];
	}

	//	Note : 왼쪽 자식 노드 생성.
	//
	D3DXVECTOR3 vCMax, vCMin;
	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	for ( i=0; i<dwLeftCount; i++ )
	{
		float fRange = pLeftArray[i]->m_Light.Range;
		D3DXVECTOR3 vPos = pLeftArray[i]->m_Light.Position;

		float fMaxX = vPos.x + fRange;
		float fMaxY = vPos.y + fRange;
		float fMaxZ = vPos.z + fRange;
		float fMinX = vPos.x - fRange;
		float fMinY = vPos.y - fRange;
		float fMinZ = vPos.z - fRange;

		if ( vCMax.x < fMaxX )	vCMax.x = fMaxX;
		if ( vCMax.y < fMaxY )	vCMax.y = fMaxY;
		if ( vCMax.z < fMaxZ )	vCMax.z = fMaxZ;

		if ( vCMin.x > fMinX )	vCMin.x = fMinX;
		if ( vCMin.y > fMinY )	vCMin.y = fMinY;
		if ( vCMin.z > fMinZ )	vCMin.z = fMinZ;
	}


	pLightNode->pLeftChild = new DXLIGHTNODE;
	pLightNode->pLeftChild->vMax = vCMax;
	pLightNode->pLeftChild->vMin = vCMin;
	MakeAABBTree ( pLeftArray, dwLeftCount, pLightNode->pLeftChild, vCMax, vCMin );


	//	Note : 오른편 자식 노드 생성.
	//
	vCMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vCMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
	for ( i=0; i<dwRightCount; i++ )
	{
		float fRange = pRightArray[i]->m_Light.Range;
		D3DXVECTOR3 vPos = pRightArray[i]->m_Light.Position;

		float fMaxX = vPos.x + fRange;
		float fMaxY = vPos.y + fRange;
		float fMaxZ = vPos.z + fRange;
		float fMinX = vPos.x - fRange;
		float fMinY = vPos.y - fRange;
		float fMinZ = vPos.z - fRange;

		if ( vCMax.x < fMaxX )	vCMax.x = fMaxX;
		if ( vCMax.y < fMaxY )	vCMax.y = fMaxY;
		if ( vCMax.z < fMaxZ )	vCMax.z = fMaxZ;

		if ( vCMin.x > fMinX )	vCMin.x = fMinX;
		if ( vCMin.y > fMinY )	vCMin.y = fMinY;
		if ( vCMin.z > fMinZ )	vCMin.z = fMinZ;
	}


	pLightNode->pRightChild = new DXLIGHTNODE;
	pLightNode->pRightChild->vMax = vCMax;
	pLightNode->pRightChild->vMin = vCMin;
	MakeAABBTree ( pRightArray, dwRightCount, pLightNode->pRightChild, vCMax, vCMin );


	SAFE_DELETE(pLeftArray);
	SAFE_DELETE(pRightArray);
}

BOOL DXLIGHTNODE::IsCollisionVolume ( CLIPVOLUME& cv )
{
	if (((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pNear.a * fMinX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMaxY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pNear.a * fMinX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMaxZ + cv.pNear.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pNear.a * fMaxX + cv.pNear.b * fMinY + cv.pNear.c * fMinZ + cv.pNear.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pFar.a * fMinX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMaxY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pFar.a * fMinX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMaxZ + cv.pFar.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pFar.a * fMaxX + cv.pFar.b * fMinY + cv.pFar.c * fMinZ + cv.pFar.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pLeft.a * fMinX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMaxY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pLeft.a * fMinX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMaxZ + cv.pLeft.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pLeft.a * fMaxX + cv.pLeft.b * fMinY + cv.pLeft.c * fMinZ + cv.pLeft.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pRight.a * fMinX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMaxY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pRight.a * fMinX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMaxZ + cv.pRight.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pRight.a * fMaxX + cv.pRight.b * fMinY + cv.pRight.c * fMinZ + cv.pRight.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	if (((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pTop.a * fMinX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMaxY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pTop.a * fMinX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMaxZ + cv.pTop.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pTop.a * fMaxX + cv.pTop.b * fMinY + cv.pTop.c * fMinZ + cv.pTop.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	else if (((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_FRONT
		((cv.pBottom.a * fMinX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_LEFT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMaxY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//TOP_RIGHT_FRONT

		((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_FRONT
		((cv.pBottom.a * fMinX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_LEFT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMaxZ + cv.pBottom.d) > -0.01f) &&	//BOTTOM_RIGHT_BACK
		((cv.pBottom.a * fMaxX + cv.pBottom.b * fMinY + cv.pBottom.c * fMinZ + cv.pBottom.d) > -0.01f))		//BOTTOM_RIGHT_FRONT
		return FALSE;

	return TRUE;
}

void DxLightMan::CloneInstance ( DxLightMan* pLightMan )
{
	pLightMan->CleanUp ();

	pLightMan->m_bLighting = m_bLighting;
	pLightMan->m_bNightAndDay = m_bNightAndDay;
	pLightMan->m_bWhenDayPointOff = m_bWhenDayPointOff;
	pLightMan->m_LtDirectNoon = m_LtDirectNoon;
	pLightMan->m_LtDirectNight = m_LtDirectNight;

	DXLIGHT *pLight = m_pLtListHead;
	while ( pLight )
	{
		//	Note : 라이트 복제후 리스트에 삽입.
		//
		DXLIGHT *pNewLight = new DXLIGHT();
		*pNewLight = *pLight;

		pNewLight->pNext = pLightMan->m_pLtListHead;
		pLightMan->m_pLtListHead = pNewLight;

		pLight = pLight->pNext;
	}

	//	Note : 트리 재구성.
	pLightMan->MakeAABBTree ();

	// Note : List를 vector에도 적용 시켜준다.
	ListToVectorLIGHT();
}



void DxLightMan::SetDPLight ( D3DLIGHTQ sLight )
{
	if ( m_dwSaveDPLight_Count >= _MAX_LIGHT )	return;	// 디버그 모드에서 표시가 되도록 해보자.

	m_sSaveDPLight[m_dwSaveDPLight_Count] = sLight;
	m_dwSaveDPLight_Count++;
}

BOOL DxLightMan::ISColsedOnePL ( D3DXVECTOR3 vPos )
{
	if ( m_dwDPLight_Count == 0 )	return FALSE;

	DWORD		dwCount=0;
	float		fLength, fLength2;
	D3DXVECTOR3	vDirect;

	vDirect.x = m_sDPLight[0].Position.x - vPos.x;
	vDirect.y = 0.f;
	vDirect.z = m_sDPLight[0].Position.z - vPos.z;
	fLength = (vDirect.x*vDirect.x) + (vDirect.z*vDirect.z);	//D3DXVec3Length ( &vDirect );
	m_dwDPLight = 0;	// 가장 가까운것 선택

	for ( DWORD i=1; i<m_dwDPLight_Count; i++ )
	{
		vDirect.x = m_sDPLight[i].Position.x - vPos.x;
		vDirect.z = m_sDPLight[i].Position.z - vPos.z;
		fLength2 = (vDirect.x*vDirect.x) + (vDirect.z*vDirect.z);	//D3DXVec3Length ( &vDirect );

		if ( fLength > fLength2 )
		{
			m_dwDPLight = i;	// 가장 가까운것 선택
			fLength = fLength2;
		}
	}

	return TRUE;
}

void DxLightMan::Clone2DPLight ()
{
	m_dwDPLight_Count = m_dwSaveDPLight_Count;
	for ( DWORD i=0; i<m_dwDPLight_Count; i++ )
	{
		m_sDPLight[i] = m_sSaveDPLight[i];
	}

	m_dwSaveDPLight_Count = 0;	// 다시 초기화
}

void DxLightMan::SetDPLight ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<_MAX_LIGHT; i++ )
	{
		if ( i<m_dwDPLight_Count )
		{
			pd3dDevice->LightEnable ( i, TRUE );
			pd3dDevice->SetLight ( i, &m_sDPLight[i] );
		}
		else
		{
			pd3dDevice->LightEnable ( i, FALSE );
		}
	}
}

void DxLightMan::Clone_MouseShift( LPD3DXMATRIX& pMatrix )
{
	DXLIGHT* pLight = m_pLtListHead;
	while( pLight )
	{
		DWORD dwBuffer = (DWORD)&pLight->m_matWorld;
		if( dwBuffer == (DWORD)pMatrix )
		{
			// Note : 새로 만들어 준다.
			DXLIGHT* pNewLight = new DXLIGHT;
			*pNewLight = *pLight;	// 값셋팅

			char szNum[STRING_LENGTH_256] = "";
			DWORD dwNum = 0;
			while (1)
			{
				//if ( dwNum >= 1000 )	sprintf ( szNum, "[%d]", dwNum++ );
				//else					sprintf ( szNum, "[%03d]", dwNum++ );

				if ( dwNum >= 1000 )	StringCchPrintf( szNum, STRING_LENGTH_256, "[%d]", dwNum++ );
				else					StringCchPrintf( szNum, STRING_LENGTH_256, "[%03d]", dwNum++ );

				StringCchCopy( pNewLight->m_szLightName, STRING_LENGTH_256, "UNNAMED" );
				StringCchCat( pNewLight->m_szLightName, STRING_LENGTH_256, szNum );

				if ( !GetLight( pNewLight->m_szLightName ) )	// 이름 체크후 없는것으로 생성.
					break;
			}

			pNewLight->pNext = m_pLtListHead;
			m_pLtListHead = pNewLight;

			// Note : DxObjectMRS 커서를 바꿔준다. < 중요 >
			pMatrix = &pNewLight->m_matWorld;

			// Note : List를 vector에도 적용 시켜준다.
			ListToVectorLIGHT();

			return;
		}
		pLight = pLight->pNext;
	}
}

void DxLightMan::SetPiecePointIndex( const D3DXVECTOR3& vPos, const float& fLength, VEC_DWORD& vecPointIndex )
{
	DWORD dwCount(0L);
	float fLengthOK(0.f);
	float fLengthREAL(0.f);
	D3DXVECTOR3 vDiff(0.f,0.f,0.f);

	std::map<float,DWORD> mapPointLight;
	std::map<float,DWORD>::iterator iter;
	std::map<float,DWORD>::iterator iter_end;

	vecPointIndex.clear();
	DXLIGHT* pCur = m_pLtListHead;
	while( pCur )
	{
		vDiff.x = pCur->m_Light.Position.x - vPos.x;
		vDiff.y = pCur->m_Light.Position.y - vPos.y;
		vDiff.z = pCur->m_Light.Position.z - vPos.z;
		fLengthREAL = (vDiff.x*vDiff.x) + (vDiff.y*vDiff.y) + (vDiff.z*vDiff.z);

		// Note : 빛과 물체의 거리가 허용범위에 있다.
		//			가까운것 순서대로 놓는다.
		fLengthOK = pCur->m_Light.Range*pCur->m_Light.Range + fLength;
		if( fLengthREAL <= fLengthOK )
		{
			mapPointLight.insert( std::make_pair(fLengthREAL,dwCount) );
		}

		++dwCount;
		pCur = pCur->pNext;
	}

	// Note : 가까운 순서대로 넣어준다.
	iter = mapPointLight.begin();
	iter_end = mapPointLight.end();
	for( ; iter!=iter_end; ++iter )
	{
		vecPointIndex.push_back( (*iter).second );
	}
}

D3DLIGHTQ* DxLightMan::GetPointLight( const DWORD& dwIndex )
{
	if( dwIndex >= m_vecPointLight.size() )	return NULL;

	return &m_vecPointLight[dwIndex]->m_Light;
}

char* DxLightMan::GetStrPointLight( const DWORD& dwIndex )
{
 	if( dwIndex >= m_vecPointLight.size() )	return NULL;

	return m_vecPointLight[dwIndex]->m_szLightName;
}

void DxLightMan::ListToVectorLIGHT()
{
	m_vecPointLight.clear();
	DXLIGHT* pCur = m_pLtListHead;
	while( pCur )
	{
		m_vecPointLight.push_back( pCur );
		pCur = pCur->pNext;
	}
}

DWORD DxLightMan::ComputeClosedPointLight( D3DXVECTOR3* pPos, D3DXVECTOR3* pNor )
{
	D3DXVECTOR3 vColor(0.f,0.f,0.f);
	ComputePointLight( m_pTreeHead, pPos, pNor, vColor );

	if( vColor.x > 1.f )	vColor.x = 1.f;
	if( vColor.y > 1.f )	vColor.y = 1.f;
	if( vColor.z > 1.f )	vColor.z = 1.f;

	int nColorR = (int)(vColor.x*255.f);
	int nColorG = (int)(vColor.y*255.f);
	int nColorB = (int)(vColor.z*255.f);

	DWORD dwColor = 0xff000000;
	dwColor += (nColorR<<16);
	dwColor += (nColorG<<8);
	dwColor += nColorB;

	return dwColor;
}

void DxLightMan::ComputePointLight( DXLIGHTNODE *pLightNode, D3DXVECTOR3* pPos, D3DXVECTOR3* pNor, D3DXVECTOR3& vColor )
{
	if( !pLightNode )	return;

	if( pLightNode->vMax.x < pPos->x )	return;
	if( pLightNode->vMax.y < pPos->y )	return;
	if( pLightNode->vMax.z < pPos->z )	return;

	if( pLightNode->vMin.x > pPos->x )	return;
	if( pLightNode->vMin.y > pPos->y )	return;
	if( pLightNode->vMin.z > pPos->z )	return;

	// Note : 계산
	float fDis(0.f);
	float fDot(0.f);
	float fAlpha(0.f);
	float fTemp(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);
	DXLIGHT* pLight = pLightNode->pLight;
	if( pLight )
	{
		vDir.x = pLight->m_Light.Position.x - pPos->x;
		vDir.y = pLight->m_Light.Position.y - pPos->y;
		vDir.z = pLight->m_Light.Position.z - pPos->z;
		fDis = D3DXVec3Length( &vDir );

		D3DXVec3Normalize( &vDir, &vDir );
		fDot = D3DXVec3Dot( &vDir, pNor );
		if( fDot <= 0.f )	goto _RETURN;		// 점의 반대편에 있다면 할 필요 없음.

		fAlpha = fDis / pLight->m_Light.Range;
		if( fAlpha >= 1.f )	goto _RETURN;		// Light의 범위에 넘어 있다는 것을 알 수 있다.

		fAlpha = 1.f - fAlpha;
		fAlpha = fAlpha * fDot;

		fAlpha = sqrtf( fAlpha );

		// Note : 색을 약간 감쇠시킨다.
		fTemp = fAlpha * 2.f;	// 0,	1,		2
		fTemp = fTemp - 1.f;	// -1,	0,		1
		fTemp = fabsf( fTemp );	// 1,	0,		1
		fTemp = fTemp - 1.f;	// 0,	-1,		0
		fTemp *= 0.05f;
		fAlpha += fTemp;

		vColor.x += pLight->m_Light.Diffuse.r*fAlpha;
		vColor.y += pLight->m_Light.Diffuse.g*fAlpha;
		vColor.z += pLight->m_Light.Diffuse.b*fAlpha;
	}

_RETURN:
	ComputePointLight( pLightNode->pRightChild, pPos, pNor, vColor );
	ComputePointLight( pLightNode->pLeftChild, pPos, pNor, vColor );
}

void DxLightMan::SetCharStaticPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	BOOL bPointLightOn(TRUE);
	if ( m_bWhenDayPointOff && GLPeriod::GetInstance().IsOffLight() )	bPointLightOn = FALSE;

	pd3dDevice->LightEnable( 1, FALSE );
	pd3dDevice->LightEnable( 2, FALSE );

	if ( bPointLightOn )
	{
		CLMAN	mapPointLight;

		DWORD dwCount(0);
		const DWORD dwCHECKCOUNT(7);

		float fDis(0.f);
		float fRange(0.f);
		D3DXVECTOR3 vDis(0.f,0.f,0.f);

		// Note : 가장 가까운 PointLight를 구한다.
		CLMAN_ITER iter = m_mmapClosedLight.begin();
		CLMAN_ITER iter_end = m_mmapClosedLight.end();
		for ( ; iter!=iter_end && dwCount<_MAX_LIGHT; ++iter, ++dwCount )
		{
			PDXLIGHT pLight = (*iter).second;
			vDis.x = vPos.x - pLight->m_Light.Position.x;
			vDis.y = vPos.y - pLight->m_Light.Position.y;
			vDis.z = vPos.z - pLight->m_Light.Position.z;

			fDis = (vDis.x*vDis.x) + (vDis.y*vDis.y) + (vDis.z*vDis.z);	// 실제 거리
			fRange = pLight->m_Light.Range*pLight->m_Light.Range;		// 유효범위

			// Note : 이때만 삽입된다.
			if( fDis < fRange )
			{
				mapPointLight.insert( std::make_pair( fDis, pLight ) );
			}
		}

		// Note : Point를 셋팅한다.
		//		셋팅가능한 갯수는 2개 . 번호는 1 2를 사용한다.
		dwCount = 1;
		iter = mapPointLight.begin();
		iter_end = mapPointLight.end();
		for( ; iter!=iter_end && dwCount<3 ; ++iter, ++dwCount )
		{
			pd3dDevice->LightEnable( dwCount, TRUE );
			pd3dDevice->SetLight( dwCount, &((*iter).second->m_Light) );
		}
	}
}

void DxLightMan::SetCharDynamicPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos )
{
	pd3dDevice->LightEnable( 3, FALSE );
	pd3dDevice->LightEnable( 4, FALSE );

	MANPOINTLIGHT	mapPointLight;

	float fDis(0.f);
	float fRange(0.f);
	D3DXVECTOR3 vDis(0.f,0.f,0.f);

	for ( DWORD i=0; i<m_dwDPLight_Count; i++ )
	{
		// 캐릭터 높이 생각. +10.f
		vDis.x = vPos.x - m_sDPLight[i].Position.x;
		vDis.y = (vPos.y + 10.f) - m_sDPLight[i].Position.y;	
		vDis.z = vPos.z - m_sDPLight[i].Position.z;

		if( vDis.x < 0.f )	vDis.x = 0.f; 
		if( vDis.y < 0.f )	vDis.y = 0.f;
		if( vDis.z < 0.f )	vDis.z = 0.f;

		fDis = (vDis.x*vDis.x) + (vDis.y*vDis.y) + (vDis.z*vDis.z);	// 실제 거리
		fRange = m_sDPLight[i].Range*m_sDPLight[i].Range;		// 유효범위

		// Note : 이때만 삽입된다. +300.f = 10*10 + 10*10 + 10*10;
		if( fDis < (fRange+300.f) )
		{
			mapPointLight.insert( std::make_pair( fDis, &m_sDPLight[i] ) );
		}
	}

	// Note : Point를 셋팅한다.
	//		셋팅가능한 갯수는 2개 . 번호는 3 4를 사용한다.
	DWORD dwCount(3);
	MANPOINTLIGHT_ITER iter = mapPointLight.begin();
	MANPOINTLIGHT_ITER iter_end = mapPointLight.end();
	for( ; iter!=iter_end && dwCount<5 ; ++iter, ++dwCount )
	{
		pd3dDevice->LightEnable( dwCount, TRUE );
		pd3dDevice->SetLight( dwCount, (*iter).second );
	}
}

