#include "pch.h"

#include "./GLPeriod.h"

#include "DxViewPort.h"

#include "DxEnvironment.h"

#include "DxLandMan.h"
#include "./DxEffectMan.h"

#include "DxSkyMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	------------------------------------------------------------------------------------------------------
//	---------------------------------------- 밤 렌더링 ---------------------------------------------------
//	------------------------------------------------------------------------------------------------------

void DxSkyMan::MoonRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct )
{
	DWORD dwHour	= GLPeriod::GetInstance().GetHour();
	DWORD dwDay		= GLPeriod::GetInstance().GetMonthToday();

//	if ( (dwHour==12&&m_dwPrevHour==11) || (m_emMoonCycle==EMMC_L_ZERO) )		// 처음 또는 오후중에..
//	{
		DWORD dwToday = dwDay;

		dwToday = (dwToday-1)/4;						// 1~4/5~8/9~12/ 13~16,17~20 /21~24/25~28/29~31
		if ( dwToday>=4 )	--dwToday;					// 보름달은 2개를 먹는다.
		m_emMoonCycle = (MOONCYCLE)(dwToday+1);			// 초승달 시작은 1 부터 시작한다.
//	}
//	m_dwPrevHour = dwHour;		// 시간을 집어넣는다.

	if ( m_fAlpha_Night == 0.f )		return;

	D3DXVECTOR3		vLocal;

	vLocal.x = GLPeriod::GetInstance().GetDirectFact_XY().x;
	vLocal.x = vLocal.x*7.f;
	vLocal.y = GLPeriod::GetInstance().GetDirectFact_XY().y;
	vLocal.z = -vLocal.x;
	D3DXVec3Normalize ( &vLocal, &vLocal );
	D3DXVECTOR3		vMoonPos = m_vSkyCenter - ( vLocal * 120000.f );	//	달의 위치를 구함 _ 정반대 위치

	D3DXMATRIX	matIdentity, matLocal;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &DxSkyMan::m_matWorld );

	m_pSavedMoonSB->Capture();
	m_pDrawMoonSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pMoonTex );		// 텍스쳐 설정
	pd3dDevice->SetTexture ( 1, m_pMoonTex );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX2::FVF );

	matLocal	= DxBillboardLookAt ( &vMoonPos, DxViewPort::GetInstance().GetMatView() );

	if ( bRefelct )
	{
		D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

		matLocal._42 = matLocal._42 - (vCenter.y*2.f);

		D3DXMatrixMultiply_MIRROR( matLocal );
	}

	vLocal = D3DXVECTOR3 ( -m_fMoonScale, m_fMoonScale, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[0].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( m_fMoonScale, m_fMoonScale, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[1].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( -m_fMoonScale, -m_fMoonScale, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[2].vPos, &vLocal, &matLocal );
	vLocal = D3DXVECTOR3 ( m_fMoonScale, -m_fMoonScale, 0.f );
	D3DXVec3TransformCoord ( &m_sVerticesTex2[3].vPos, &vLocal, &matLocal );

	D3DCOLOR cColor = 0xffffffff;
	DWORD	dwAlpha = ((DWORD)(m_fAlpha_Night*255.f))<<24;			// 알파값 조절

	//	Note : 보름달에서 15,17일은 약간붉은 달을 16일은 붉으스름한 달을 생성
	//
	switch( dwDay )
	{
	case 15:
	case 17:
		cColor = dwAlpha + (255<<16) + (210<<8) + 190;
		break;

	case 16:
		cColor = dwAlpha + (255<<16) + (80<<8) + 70;
		break;

	default:
		cColor = dwAlpha + (255<<16) + (239<<8) + 210;
		break;
	}

	m_sVerticesTex2[0].dwColor = cColor;
	m_sVerticesTex2[1].dwColor = cColor;
	m_sVerticesTex2[2].dwColor = cColor;
	m_sVerticesTex2[3].dwColor = cColor;

	float fTexXS, fTexXE, fTexYS, fTexYE;
	if ( m_emMoonCycle==EMMC_L_SMALL )		{ fTexXS = 0.f; fTexXE = 0.5f; fTexYS = 0.f; fTexYE = 0.5f; }	//
	else if ( m_emMoonCycle==EMMC_R_SMALL )	{ fTexXS = 0.5f; fTexXE = 0.f; fTexYS = 0.f; fTexYE = 0.5f; }
	else if ( m_emMoonCycle==EMMC_L_HALF )	{ fTexXS = 0.5f; fTexXE = 1.f; fTexYS = 0.f; fTexYE = 0.5f; }	//
	else if ( m_emMoonCycle==EMMC_R_HALF )	{ fTexXS = 1.f; fTexXE = 0.5f; fTexYS = 0.f; fTexYE = 0.5f; }
	else if ( m_emMoonCycle==EMMC_L_BIG )	{ fTexXS = 0.f; fTexXE = 0.5f; fTexYS = 0.5f; fTexYE = 1.f; }	//
	else if ( m_emMoonCycle==EMMC_R_BIG )	{ fTexXS = 0.5f; fTexXE = 0.f; fTexYS = 0.5f; fTexYE = 1.f; }
	else if ( m_emMoonCycle==EMMC_CIRCLE )	{ fTexXS = 0.5f; fTexXE = 1.f; fTexYS = 0.5f; fTexYE = 1.f; }	//

	m_sVerticesTex2[0].vTex02 = D3DXVECTOR2 ( fTexXS, fTexYS );
	m_sVerticesTex2[1].vTex02 = D3DXVECTOR2 ( fTexXE, fTexYS );
	m_sVerticesTex2[2].vTex02 = D3DXVECTOR2 ( fTexXS, fTexYE );
	m_sVerticesTex2[3].vTex02 = D3DXVECTOR2 ( fTexXE, fTexYE );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVerticesTex2, sizeof(VERTEXCOLORTEX2) );

	m_pSavedMoonSB->Apply();

	pd3dDevice->SetTexture ( 1, NULL );
}

void DxSkyMan::StarRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct )
{
	HRESULT hr(S_OK);

	if ( m_fAlpha_Night == 0.f )		return;

	D3DXVECTOR3	vLocal, vPos;
	D3DXMATRIX	matLocal, matWorld, matProj, matOrigProj;

	D3DXMatrixIdentity ( &matWorld );

	VERTEXCOLORTEX*	pVertices;

	matLocal = DxBillboardLookAt ( &m_vSkyCenter, DxViewPort::GetInstance().GetMatView() );
	matLocal._41 = 0.f;
	matLocal._42 = 0.f;
	matLocal._43 = 0.f;

	float fScale;
	hr = m_pStarVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxSkyMan::StarRender() -- m_pStarVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<STAR_NUM; i++ )
	{
		fScale = m_fStarScale * m_sStarObject[i].fTwinkle * m_fAlpha_Night;

		vPos.x = m_sStarObject[i].vPos.x*1000000.f;
		vPos.y = m_sStarObject[i].vPos.y*200000.f;
		vPos.z = m_sStarObject[i].vPos.z*1000000.f;

		if ( bRefelct )	vPos.y = -vPos.y;

		vLocal = D3DXVECTOR3 ( -fScale, fScale, 0.f );
		D3DXVec3TransformCoord ( &vLocal, &vLocal, &matLocal );
		pVertices[i*4+0].vPos = vLocal + vPos;

		vLocal = D3DXVECTOR3 ( fScale, fScale, 0.f );
		D3DXVec3TransformCoord ( &vLocal, &vLocal, &matLocal );
		pVertices[i*4+1].vPos = vLocal + vPos;

		vLocal = D3DXVECTOR3 ( -fScale, -fScale, 0.f );
		D3DXVec3TransformCoord ( &vLocal, &vLocal, &matLocal );
		pVertices[i*4+2].vPos = vLocal + vPos;

		vLocal = D3DXVECTOR3 ( fScale, -fScale, 0.f );
		D3DXVec3TransformCoord ( &vLocal, &vLocal, &matLocal );
		pVertices[i*4+3].vPos = vLocal + vPos;
	}
	m_pStarVB->Unlock ();
	
	pd3dDevice->SetTransform ( D3DTS_WORLD,		&DxSkyMan::m_matWorld );

	m_pSavedNightSB->Capture();
	m_pDrawNightSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pStarTex );		// 텍스쳐 설정

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
	pd3dDevice->SetStreamSource ( 0, m_pStarVB, 0, sizeof(VERTEXCOLORTEX) );
	pd3dDevice->SetIndices ( m_pStarIB );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, STAR_NUM*4, 0, STAR_NUM*2 );

	m_pSavedNightSB->Apply();
}

void DxSkyMan::UStarRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct )
{
	if ( m_fAlpha_Night == 0.f )		return;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD,		&DxSkyMan::m_matWorld );

	D3DXVECTOR3	vCross;
	D3DXVECTOR3	vDirect = DxViewPort::GetInstance().GetLookDir();

	D3DXVec3Cross ( &vCross, &m_vUStarDirect, &vDirect);
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVECTOR3 vLeft	= vCross*m_fUStarScale;
	D3DXVECTOR3 vLength = m_vUStarDirect*m_fUStarScale*m_fUStarRate;

	DWORD		dwAlpha, dwColor;

	float fTime = m_fUStarTimeEnd - m_fUStarTime;
	if ( fTime >= 1.f )			dwAlpha = (255)<<24;
	else if ( fTime <= 0.f )	dwAlpha = 0;
	else						dwAlpha = ((DWORD)(255.f*fTime))<<24;
	
	dwColor = dwAlpha + 0xffffff;

	m_sUStar[0].dwColor = dwColor;
	m_sUStar[1].dwColor = dwColor;
	m_sUStar[2].dwColor = dwColor;
	m_sUStar[3].dwColor = dwColor;

	m_pSavedUStarSB->Capture();
	m_pDrawUStarSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pUStarTex );

	pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );

	for ( DWORD i=0; i<USTAR_NUM; ++i)		// 20개를 뿌린다.
	{
		m_sUStar[0].vPos = m_vUStarPos[i] + vLeft;
		m_sUStar[1].vPos = m_vUStarPos[i] - vLeft;
		m_sUStar[2].vPos = m_sUStar[0].vPos - vLength;
		m_sUStar[3].vPos = m_sUStar[1].vPos - vLength;

		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sUStar, sizeof(VERTEXCOLORTEX) );
	}

	m_pSavedUStarSB->Apply();
}

//	------------------------------------------------------------------------------------------------------
//	---------------------------------------- 낮 렌더링 ---------------------------------------------------
//	------------------------------------------------------------------------------------------------------

void DxSkyMan::SunRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct )
{
	return;
	//if ( m_fAlpha_Day == 0.f)	return;

	//D3DXVECTOR3		vLocal;

	//vLocal.x = GLPeriod::GetInstance().GetDirectFact_XY().x;
	//vLocal.x = vLocal.x*3.f;
	//vLocal.y = GLPeriod::GetInstance().GetDirectFact_XY().y;
	//vLocal.z = -vLocal.x*2.f;
	//D3DXVec3Normalize ( &vLocal, &vLocal );
	//m_vSunPos = m_vSkyCenter + ( vLocal * 48000.f );	//	태양의 위치를 구함

	//D3DXMATRIX	matIdentity, matLocal;
	//D3DXMatrixIdentity ( &matIdentity );
	//pd3dDevice->SetTransform ( D3DTS_WORLD, &DxSkyMan::m_matWorld );
	//D3DXVec3TransformCoord ( &m_vSunPos, &m_vSunPos, &DxSkyMan::m_matWorld );

	//m_pSavedSunSB->Capture();
	//m_pDrawSunSB->Apply();

	//pd3dDevice->SetTexture ( 0, m_pSunTex );		// 텍스쳐 설정

	//pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );

	//D3DXVECTOR3	vUp = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	//matLocal	= DxBillboardLookAt ( &m_vSunPos, &m_vSkyCenter, &vUp );
	//matLocal._41 = m_vSunPos.x;
	//matLocal._42 = m_vSunPos.y;
	//matLocal._43 = m_vSunPos.z;

	//if ( bRefelct )		matLocal._42 = -matLocal._42;

	//vLocal = D3DXVECTOR3 ( -m_fSunScale, m_fSunScale, 0.f );
	//D3DXVec3TransformCoord ( &m_sVertices[0].vPos, &vLocal, &matLocal );
	//vLocal = D3DXVECTOR3 ( m_fSunScale, m_fSunScale, 0.f );
	//D3DXVec3TransformCoord ( &m_sVertices[1].vPos, &vLocal, &matLocal );
	//vLocal = D3DXVECTOR3 ( -m_fSunScale, -m_fSunScale, 0.f );
	//D3DXVec3TransformCoord ( &m_sVertices[2].vPos, &vLocal, &matLocal );
	//vLocal = D3DXVECTOR3 ( m_fSunScale, -m_fSunScale, 0.f );
	//D3DXVec3TransformCoord ( &m_sVertices[3].vPos, &vLocal, &matLocal );

	//DWORD dwColor = m_sCloud.GetSkyColor ();
	//DWORD dwAlpha = ((DWORD)(255.f*m_fAlpha_Day))<<24;
	//dwColor = dwAlpha+dwColor;

	//m_sVertices[0].dwColor = dwColor;
	//m_sVertices[1].dwColor = dwColor;
	//m_sVertices[2].dwColor = dwColor;
	//m_sVertices[3].dwColor = dwColor;

	//m_sVertices[0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
	//m_sVertices[1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
	//m_sVertices[2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
	//m_sVertices[3].vTex = D3DXVECTOR2 ( 1.f, 1.f );

	//pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sVertices, sizeof(VERTEXCOLORTEX) );

	//m_pSavedSunSB->Apply();
}

void DxSkyMan::FlareRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vSunPos )
{
	//if ( m_fAlpha_Day == 0.f)	return;

	//D3DXVECTOR3& vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
	//D3DXVECTOR3& vFromPt	= DxViewPort::GetInstance().GetFromPt();

	//D3DXVECTOR3	vPos;
	//D3DXVECTOR3	vLookDir = DxViewPort::GetInstance().GetLookDir();
	//D3DXVECTOR3	vSunLook = vSunPos - vLookatPt;				// 바라보는 시점에서 태양 방향의 벡터
	//float		fSunLook = D3DXVec3Length ( &vSunLook );	// 바라보는 시점에서 태양까지의 거리
	//D3DXVec3Normalize ( &vSunLook, &vSunLook );				// 노멀라이즈
	//float	fDot = D3DXVec3Dot ( &vLookDir, &vSunLook );	// 내적한 값

	//if ( fDot <= 0 )		return;			// 내적한 값이 0 보다 작거나 같다면 뿌려주지 않는다.

	//float	fScale;

	//D3DXMATRIX	matIdentity;
	//D3DXMatrixIdentity ( &matIdentity );
	//pd3dDevice->SetTransform ( D3DTS_WORLD,		&DxSkyMan::m_matWorld );

	//D3DVIEWPORTQ	sViewPort8;
	//pd3dDevice->GetViewport ( &sViewPort8 );
	//D3DXVec3Project ( &vPos, &vSunPos, &sViewPort8, &DxViewPort::GetInstance().GetMatProj(),
	//												&DxViewPort::GetInstance().GetMatView(),
	//												&matIdentity );

	//float fAlpha;
	//float fRateX = vPos.x / m_dwModeWidth;
	//float fRateY = vPos.y / m_dwModeHeight;

	//fRateX = fRateX - 0.5f;		// 화면 중간에서 떨어진 거리
	//fRateY = fRateY - 0.5f;

	//if ( vPos.x<m_dwModeWidth && vPos.x>0.f &&
	//	vPos.y<m_dwModeHeight && vPos.y>0.f )
	//{
	//	fAlpha = fabsf(fRateY);
	//	fAlpha -= 0.5f;
	//	fAlpha /= -0.5f;
	//}
	//else
	//{
	//	return;								// 뿌리지 않는다.
	//}

	////	Note : 픽킹을 하자.
	//DxLandMan*	pLandMan = DxEffectMan::GetInstance().GetLandMan();
	//if ( pLandMan )
	//{
	//	BOOL		bCollision;
	//	D3DXVECTOR3	vCollision;
	//	LPDXFRAME	pDxFrame = NULL;

	//	pLandMan->IsCollision( vSunPos, vFromPt, vCollision, bCollision, pDxFrame, FALSE );

	//	if ( bCollision )	return;			// 화면에 걸리는 물체가 있으면 넘긴다.
	//}

	////
	//fAlpha = (0.3f*fAlpha) + 0.2f;					// 최소 0.2 최대 0.5
	//fAlpha = fAlpha + fAlpha*(RANDOM_POS*0.2f);		// 최소 0.24 최대 0.6
	//fAlpha = fAlpha*m_fAlpha_Day;					// 비나 눈이 온다면 플레어를 뿌리지 않겠지.

	//DWORD	dwColor = m_sCloud.GetSkyColor ();
	//DWORD	dwColorR = ((dwColor&0xff0000)>>16);
	//DWORD	dwColorG = ((dwColor&0xff00)>>8);
	//DWORD	dwColorB = dwColor&0xff;
	//dwColorR = ((DWORD)(dwColorR*fAlpha))<<16;
	//dwColorG = ((DWORD)(dwColorG*fAlpha))<<8;
	//dwColorB = (DWORD)(dwColorB*fAlpha);
	//dwColorR = 0xff000000 + dwColorR + dwColorG + dwColorB;

	//m_sFlare[0].dwColor = dwColorR;
	//m_sFlare[1].dwColor = dwColorR;
	//m_sFlare[2].dwColor = dwColorR;
	//m_sFlare[3].dwColor = dwColorR;



	//DWORD dwWidth, dwHeight;

	//fScale = fDot*60.f;
	//dwWidth	=	(DWORD)((fRateX*0.8f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.8f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 0 );

	//fScale = fDot*200.f;
	//dwWidth	=	(DWORD)((fRateX*0.9f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.9f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 3 );
	//dwWidth	=	(DWORD)((fRateX*0.7f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.7f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 3 );
	//dwWidth	=	(DWORD)((fRateX*0.5f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.5f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 3 );
	//dwWidth	=	(DWORD)((fRateX*0.3f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.3f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 3 );

	//fScale = fDot*90.f;
	//dwWidth	=	(DWORD)((fRateX*0.45f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.45f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 1 );

	//fScale = fDot*60.f;
	//dwWidth	=	(DWORD)((fRateX*0.35f+0.5f)*m_dwModeWidth);
	//dwHeight =	(DWORD)((fRateY*0.35f+0.5f)*m_dwModeHeight);
	//FlareRender_Part ( pd3dDevice, dwWidth, dwHeight, fScale, 2 );
}

void DxSkyMan::FlareRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwWidth, DWORD dwHeight, float fScale, int nTex )
{
	int nTexU = nTex%2;
	int nTexV = nTex/2;
	m_sFlare[0].vPos = D3DXVECTOR4 ( dwWidth-fScale, dwHeight-fScale, 1.f, 1.f );
	m_sFlare[1].vPos = D3DXVECTOR4 ( dwWidth+fScale, dwHeight-fScale, 1.f, 1.f );
	m_sFlare[2].vPos = D3DXVECTOR4 ( dwWidth-fScale, dwHeight+fScale, 1.f, 1.f );
	m_sFlare[3].vPos = D3DXVECTOR4 ( dwWidth+fScale, dwHeight+fScale, 1.f, 1.f );

	m_sFlare[0].vTex = D3DXVECTOR2 ( nTexU*0.5f,		nTexV*0.5f );
	m_sFlare[1].vTex = D3DXVECTOR2 ( nTexU*0.5f+0.5f,	nTexV*0.5f );
	m_sFlare[2].vTex = D3DXVECTOR2 ( nTexU*0.5f,		nTexV*0.5f+0.5f );
	m_sFlare[3].vTex = D3DXVECTOR2 ( nTexU*0.5f+0.5f,	nTexV*0.5f+0.5f );

	m_pSavedFlareSB->Capture();
	m_pDrawFlareSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pFlareTex );		// 텍스쳐 설정

	pd3dDevice->SetFVF ( VERTEXCOLORRHW::FVF );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_sFlare, sizeof(VERTEXCOLORRHW) );

	m_pSavedFlareSB->Apply();
}