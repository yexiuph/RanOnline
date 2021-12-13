// DxEffectRainPoint.cpp: implementation of the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./SerialFile.h"
#include "./DxViewPort.h"
#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectRainDrop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD	DxEffectRainPoint::SEPERATEOBJ::dwFlag = 0L;
float	DxEffectRainPoint::SEPERATEOBJ::fElapsedTime03 = 0.f;
float	DxEffectRainPoint::SEPERATEOBJ::m_fElapsedTime = 0.f;
float	DxEffectRainPoint::SEPERATEOBJ::fRainSize = 0.f;
float	DxEffectRainPoint::SEPERATEOBJ::fSnowSize = 0.f;
float	DxEffectRainPoint::SEPERATEOBJ::fLeavesSize = 0.f;
float	DxEffectRainPoint::SEPERATEOBJ::fLeavesSpeed = 0.f;

float   DxEffectRainPoint::CNEWRAINDATA::fElapsedTime = 0.0f;
DWORD   DxEffectRainPoint::CNEWRAINDATA::dwFlag       = 0L;

const float	DxEffectRainPoint::RAINTIME			= 5.f;		// 음.. 비의 루프
const float	DxEffectRainPoint::RAINDROP_S		= 4.9f;		// 빗방울 시작 타임
const float	DxEffectRainPoint::SNOWTIMEUP		= 9.f;							// 하늘에 있는 시간.
const float	DxEffectRainPoint::SNOWTIMEDOWN		= 1.5f;							// 땅에 있는 시간. ( ALPHA 도 포함 )
const float	DxEffectRainPoint::SNOWTIMEALPHA	= 0.5f;							// 사라지는 시간 ( 크기가 서서히 작아진다. )
const float	DxEffectRainPoint::SNOWTIME			= SNOWTIMEUP + SNOWTIMEDOWN;
const float	DxEffectRainPoint::LEAVESTIMEUP		= 18.f;							// 하늘에 있는 시간.
const float	DxEffectRainPoint::LEAVESTIMEDOWN	= 2.f;							// 땅에 있는 시간. ( ALPHA 도 포함 )
const float	DxEffectRainPoint::LEAVESTIMEALPHA	= 1.f;							// 사라지는 시간 ( 크기가 서서히 작아진다. )
const float	DxEffectRainPoint::LEAVESTIME		= LEAVESTIMEUP + LEAVESTIMEDOWN;	// 전체 플레이 시간.

HRESULT	DxEffectRainPoint::SEPERATEOBJ::CreateSeperateVB ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT	hr = S_OK;

	if ( !dwFaces )		// 값이 없으면 아무것도 생성 안한다.
	{
		SAFE_DELETE_ARRAY ( pRainArray );
		SAFE_DELETE_ARRAY ( pSnowArray );
		SAFE_DELETE_ARRAY ( pLeavesArray );

		goto _RETURN;
	}

	{
		SAFE_DELETE_ARRAY ( pRainArray );
		pRainArray = new RAINVERTEX[dwFaces];
		for( DWORD i=0; i<dwFaces; i++ )
		{
			pRainArray[i].vPos		= pVertexArray[i];
			pRainArray[i].vPos.y	+= 1.f;
			pRainArray[i].vDirect	= D3DXVECTOR3 ( RANDOM_POS-0.5f, RANDOM_POS, RANDOM_POS-0.5f );
			pRainArray[i].bRainDrop	= FALSE;
			pRainArray[i].fHeight	= 0.f;
			pRainArray[i].fSize		= 1.f;
			pRainArray[i].fAlpha	= 1.f;
			pRainArray[i].fRandom	= (RANDOM_POS*2.5f);	//3~5사이 값
			pRainArray[i].fLoopSum	= (RANDOM_POS*RAINTIME);	// Loop point
		}
	}

	{
		SAFE_DELETE_ARRAY ( pSnowArray );
		pSnowArray = new SNOWVERTEX[dwFaces];
		for( DWORD i=0; i<dwFaces; i++ )
		{
			pSnowArray[i].vPos			= pVertexArray[i];
			pSnowArray[i].vDirection	= D3DXVECTOR3 ( RANDOM_POS-0.5f, -((RANDOM_POS*0.5f)+0.5f), RANDOM_POS-0.5f );
			pSnowArray[i].fRandom		= (RANDOM_POS*2.f) + 1.f;		// 1~3 사이 값
			pSnowArray[i].fSize			= 1.f;
			pSnowArray[i].fLoopSum		= SNOWTIMEUP + (RANDOM_POS*SNOWTIME);		//
		}
	}

	{
		SAFE_DELETE_ARRAY ( pLeavesArray );
		pLeavesArray = new LEAVESVERTEX[dwFaces];
		for( DWORD i=0; i<dwFaces; i++ )
		{
			pLeavesArray[i].vPos		= pVertexArray[i];
			//pLeavesArray[i].vDirection	= D3DXVECTOR3 ( ((RANDOM_POS*0.5f)+0.3f), (RANDOM_POS-0.5f)-1.f, ((RANDOM_POS*0.5f)+0.3f) );
			pLeavesArray[i].vDirection	= D3DXVECTOR3 ( (RANDOM_POS-0.5f), -(RANDOM_POS+2.f), (RANDOM_POS-0.5f)+2.f );
			pLeavesArray[i].vWindPos	= D3DXVECTOR2 ( 0.f, 0.f );
			pLeavesArray[i].fRandom		= 4.f+RANDOM_POS;		// 4~5 사이 값
			pLeavesArray[i].fSize		= 1.f;
			pLeavesArray[i].fLoopSum	= LEAVESTIMEUP + (RANDOM_POS*LEAVESTIME);
			pLeavesArray[i].fRotate		= 0.f;
		}
	}

	{
		SAFE_DELETE_ARRAY ( pFireFlyArray );
		pFireFlyArray = new FIREFLYORIGINAL[dwFaces];
		for( DWORD i=0; i<dwFaces; i++ )
		{
			pFireFlyArray[i].fCurTime	= 0.f;						// 현재 시간
			pFireFlyArray[i].fEndTime	= 5.f+(RANDOM_POS*25.f);	// 5~30 사이 값
		}
	}

	return hr;

_RETURN:

	return hr;
}


void DxEffectRainPoint::SEPERATEOBJ::SetTexture ( LPDIRECT3DTEXTUREQ _pRainTex, LPDIRECT3DTEXTUREQ _pCircleTex, LPDIRECT3DTEXTUREQ _pRainDropTex,
									LPDIRECT3DTEXTUREQ _pSnowTex, LPDIRECT3DTEXTUREQ _pLeavesTex,
									LPDIRECT3DTEXTUREQ _pGroundTex, LPDIRECT3DTEXTUREQ _pRain_GTex, LPDIRECT3DTEXTUREQ _pSnow_GTex )
{
	pRainTex		= _pRainTex;
	pCircleTex		= _pCircleTex;
	pRainDropTex	= _pRainDropTex;
	pSnowTex		= _pSnowTex;
	pLeavesTex		= _pLeavesTex;

	pGroundTex		= _pGroundTex;
	pRain_GTex		= _pRain_GTex;
	pSnow_GTex		= _pSnow_GTex;
}

float DxEffectRainPoint::SEPERATEOBJ::GetLengthRate ( D3DXVECTOR3 &vLookat )
{
	D3DXVECTOR3 vLength = vCenter - vLookat;
	float fLength = D3DXVec3Length ( &vLength );

	if ( fLength <= 100.f )		return 1.f;
	else if ( fLength > 800.f )	return 0.f;
	return ( (800.f-fLength)/700.f );		// 100 이하는 전부 다 나오도록 설정 
}

void	DxEffectRainPoint::SEPERATEOBJ::UpdateRain ( float fElapsedTime, float fAngle, DWORD dwRenderFaces )
{
	float	fLoop;

	for ( DWORD i=0; i<dwRenderFaces; i++ )
	{
		pRainArray[i].fLoopSum += fElapsedTime * 0.3f;
		fLoop = pRainArray[i].fLoopSum;

		pRainArray[i].fHeight = (RAINDROP_S-fLoop);

		if ( fLoop < RAINDROP_S )		// 그냥 비 떨어짐.
		{
			pRainArray[i].fAlpha = 1.f;

			pRainArray[i].bRainDrop	= FALSE;
		}
		else if ( fLoop > RAINTIME )		// 처음으로 되돌린다.
		{
			pRainArray[i].fLoopSum	= pRainArray[i].fRandom;
			pRainArray[i].fAlpha	= 1.f;

			pRainArray[i].bRainDrop	= FALSE;
		}
		else								// 빗방울이 터지기 시작한다.
		{
			pRainArray[i].fAlpha = (RAINTIME-fLoop) / (RAINTIME-RAINDROP_S);

			if ( fAngle >= 0.85f )		// 80도 에서 60도	원
			{
				pRainArray[i].fAlpha *= 0.6f;		// 좀 약하게 만든다.		1.f는 너무 세다.
				pRainArray[i].fSize = (fLoop-RAINDROP_S) / (RAINTIME-RAINDROP_S);
			}
			else
			{
				pRainArray[i].fAlpha *= 0.4f;		// 좀 약하게 만든다.		1.f는 너무 세다.
				pRainArray[i].fSize = 1.f;
			}

			pRainArray[i].bRainDrop	= TRUE;
		}
	}
}

HRESULT DxEffectRainPoint::SEPERATEOBJ::RenderRain ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT	hr = S_OK;

	PROFILE_BEGIN("DxEffectRain");

	DWORD		dwAplha;
	D3DXVECTOR3 Up, LookFrom;

	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	if ( !m_pVB || !m_pIB )		goto	_RETURN;

	// Note : 비율을 맞춘다.
	//
	float fFaceRate = GetLengthRate ( vFromPt );

	Up = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );
	LookFrom = vFromPt - vLookat;
	D3DXVec3Normalize ( &LookFrom, &LookFrom );

	float fAngle = D3DXVec3Dot ( &Up, &LookFrom );

	DWORD dwRainDropFaces = 0;
	DWORD dwRenderFaces = dwFaces;

	float fApplyRate = DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();
	dwRenderFaces = (DWORD)(dwFaces*fApplyRate*fFaceRate);

	if ( dwRenderFaces <= 0 )			goto	_RETURN;
	if ( dwRenderFaces > MAX_OBJECT )	dwRenderFaces = MAX_OBJECT;		// 최대 갯수를 제한한다.

	//	Note : 비 업데이트
	//
	UpdateRain ( fElapsedTime03, fAngle, dwRenderFaces );

	// Note : StateBlock Capture
	m_pSB_Effect_SAVE->Capture();
	m_pSB_Effect->Apply();

	pd3dDevice->SetFVF ( RAINRENDER::FVF );
	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pColorVB_1, 0, sizeof(RAINRENDER) );

	//	Note : Draw - RainDrop
	// 떨어지는 빗방울
	{
		D3DXVECTOR3	vWidth;
		D3DXVECTOR3	vNorDirect;
		D3DXVECTOR3	vUp ( 0.f, 1.f, 0.f );
		D3DXVECTOR3	vDirection = DxViewPort::GetInstance().GetLookDir();

		D3DXVec3Cross ( &vWidth, &vUp, &vDirection );
		D3DXVec3Normalize ( &vWidth, &vWidth );
		vWidth = vWidth * 0.15f;					//RAIN_WIDTH_SCALE;


		D3DXVECTOR3	vLocal;
		D3DXVECTOR3	vDirect;

		RAINRENDER* pVertices;
		m_pColorVB_1->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );

		pd3dDevice->SetTexture( 0, pRainTex );

		for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
		{
			vDirect = D3DXVECTOR3(-3.f,9.8f,-3.f);
			vDirect += pRainArray[i].vDirect;
			vDirect *= 80.f;

			D3DXVec3Normalize ( &vNorDirect, &vDirect );

			vDirect *= pRainArray[i].fHeight;

			vLocal = pRainArray[i].vPos + vDirect;
			pVertices[(i*4)+0].vPos	= vLocal - vWidth;
			pVertices[(i*4)+1].vPos	= vLocal - vWidth + ( vNorDirect * 35.0f );	//m_fLength = 35.0f
			pVertices[(i*4)+2].vPos	= vLocal + vWidth;
			pVertices[(i*4)+3].vPos	= vLocal + vWidth + ( vNorDirect * 35.0f );
		}
		m_pColorVB_1->Unlock ();

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwRenderFaces*4, 0, dwRenderFaces*2 );
	}

	pd3dDevice->SetFVF ( RAINRENDER::FVF );
	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pColorVB_2, 0, sizeof(RAINRENDER) );

	//	Note : Draw - RainDrop
	//
	// 튀기는 빗방울
	{
		D3DXVECTOR3	vLocal;
		D3DXMATRIX	matLocal;
		float		fScale;

		RAINRENDER* pVertices;
		m_pColorVB_2->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );

		if ( fAngle >= 0.85f )		// 80도 에서 60도		원
		{
			pd3dDevice->SetTexture( 0, pCircleTex );

			for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
			{
				if ( !pRainArray[i].bRainDrop )		continue;

				dwAplha = ((DWORD)(pRainArray[i].fAlpha*255))<<24;

				pVertices[(dwRainDropFaces*4)+0].cColor = dwAplha + 0xffffff;
				pVertices[(dwRainDropFaces*4)+1].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;
				pVertices[(dwRainDropFaces*4)+2].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;
				pVertices[(dwRainDropFaces*4)+3].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;

				fScale	= pRainArray[i].fSize * fRainSize * 1.5f;

				pVertices[(dwRainDropFaces*4)+0].vPos = D3DXVECTOR3( -fScale, 0.f, fScale );
				pVertices[(dwRainDropFaces*4)+1].vPos = D3DXVECTOR3( fScale, 0.f,fScale );
				pVertices[(dwRainDropFaces*4)+2].vPos = D3DXVECTOR3( -fScale, 0.f, -fScale );
				pVertices[(dwRainDropFaces*4)+3].vPos = D3DXVECTOR3( fScale, 0.f, -fScale );

				pVertices[(dwRainDropFaces*4)+0].vPos += pRainArray[i].vPos;
				pVertices[(dwRainDropFaces*4)+1].vPos += pRainArray[i].vPos;
				pVertices[(dwRainDropFaces*4)+2].vPos += pRainArray[i].vPos;
				pVertices[(dwRainDropFaces*4)+3].vPos += pRainArray[i].vPos;

				++dwRainDropFaces;
			}
		}
		else
		{
			pd3dDevice->SetTexture( 0, pRainDropTex );

			for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
			{
				if ( !pRainArray[i].bRainDrop )		continue;

				dwAplha = ((DWORD)(pRainArray[i].fAlpha*255))<<24;

				pVertices[(dwRainDropFaces*4)+0].cColor = dwAplha + 0xffffff;
				pVertices[(dwRainDropFaces*4)+1].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;
				pVertices[(dwRainDropFaces*4)+2].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;
				pVertices[(dwRainDropFaces*4)+3].cColor = pVertices[(dwRainDropFaces*4)+0].cColor;

				fScale	= pRainArray[i].fSize * fRainSize;
				matLocal = DxBillboardLookAt ( &pRainArray[i].vPos, DxViewPort::GetInstance().GetMatView() );

				vLocal = D3DXVECTOR3( -fScale, fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[(dwRainDropFaces*4)+0].vPos, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale, fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[(dwRainDropFaces*4)+1].vPos, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( -fScale, -fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[(dwRainDropFaces*4)+2].vPos, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale, -fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[(dwRainDropFaces*4)+3].vPos, &vLocal, &matLocal );

				++dwRainDropFaces;
			}
		}

		m_pColorVB_2->Unlock ();

		if ( dwRainDropFaces )		// 0보다 클때만 나오도록 한다.
			pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwRainDropFaces*4, 0, dwRainDropFaces*2 );
	}

	// Note : StateBlock Apply
	m_pSB_Effect_SAVE->Apply();

_RETURN:
	PROFILE_END("DxEffectRain");

	return hr;
}

void	DxEffectRainPoint::SEPERATEOBJ::UpdateParticle ( float fElapsedTime, DWORD dwRenderFaces )
{
	float	fRand;
	float	fLoop;
	for ( DWORD i=0; i<dwRenderFaces; i++ )
	{
		//																속도늦춤
		pSnowArray[i].fLoopSum -= pSnowArray[i].fRandom*fElapsedTime * 0.25f;
		pSnowArray[i].fLoopSum = (pSnowArray[i].fLoopSum<=0.f) ? SNOWTIME: pSnowArray[i].fLoopSum;
		fLoop = pSnowArray[i].fLoopSum - SNOWTIMEDOWN;

		if ( fLoop >= 0.f )			// 하늘에 있을 때
		{
			fRand = sinf(fLoop*3.f) * 3.f;			// 흔들림을 준다.
			pSnowArray[i].vPos = pVertexArray[i] - pSnowArray[i].vDirection*fLoop*15.f;
			pSnowArray[i].vPos += D3DXVECTOR3 ( fRand, 0.f, fRand );
			pSnowArray[i].fSize = 1.f;								// 원하는 크기를 넣으면 된다.
		}
		else 									// 땅에 있을 때
		{
			pSnowArray[i].vPos	= pVertexArray[i];
			pSnowArray[i].vPos.y += 0.2f;

			if ( pSnowArray[i].fLoopSum < SNOWTIMEALPHA )		pSnowArray[i].fSize = pSnowArray[i].fLoopSum / SNOWTIMEALPHA;
			else												pSnowArray[i].fSize = 1.f;
		}
	}
}

HRESULT DxEffectRainPoint::SEPERATEOBJ::RenderSnow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT	hr = S_OK;

	PROFILE_BEGIN("DxEffectSnow");

	if ( !m_pVB || !m_pIB )		goto	_RETURN;

	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	// Note : 비율을 맞춘다.
	//
	float fFaceRate = GetLengthRate ( vFromPt );

	DWORD dwRenderFaces = dwFaces;

	float fApplyRate = DxWeatherMan::GetInstance()->GetSnow()->GetApplyRate();
	dwRenderFaces = (DWORD)(dwFaces*fApplyRate*fFaceRate);

	if ( dwRenderFaces <= 0 )			goto	_RETURN;
	if ( dwRenderFaces > MAX_OBJECT )	dwRenderFaces = MAX_OBJECT;		// 최대 갯수를 제한한다.

	//	Note : 파티클 업데이트
	//
	UpdateParticle ( fElapsedTime03, dwRenderFaces );

	// Note : StateBlock Capture
	m_pSB_Snow_SAVE->Capture();
	m_pSB_Snow->Apply();

	pd3dDevice->SetTexture ( 0, pSnowTex );

	pd3dDevice->SetFVF ( SNOWRENDER::FVF );
	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(SNOWRENDER) );

	//	Note : Draw
	{
		D3DXVECTOR3	vLocal;
		D3DXMATRIX	matLocal;
		float		fScale;

		SNOWRENDER* pVertices;
		m_pVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );

		for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
		{
			fScale	= pSnowArray[i].fSize * fSnowSize;
			matLocal = DxBillboardLookAt ( &pSnowArray[i].vPos, DxViewPort::GetInstance().GetMatView() );

			vLocal = D3DXVECTOR3( -fScale, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fScale, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -fScale, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fScale, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+3].vPos, &vLocal, &matLocal );
		}

		m_pVB->Unlock ();

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwRenderFaces*4, 0, dwRenderFaces*2 );
	}

	// Note : StateBlock Apply
	m_pSB_Snow_SAVE->Apply();

_RETURN:
	PROFILE_END("DxEffectSnow");

	return hr;
}

void	DxEffectRainPoint::SEPERATEOBJ::UpdateLeaves ( float fElapsedTime, DWORD dwRenderFaces)
{
	float	fRand;
	float	fLoop;
	float	fLimitXZ;
	float	fLimitY;		// 높이에 제한을 준다.


	for ( DWORD i=0; i<dwRenderFaces; i++ )
	{
		//																속도늦춤
		pLeavesArray[i].fLoopSum -= pLeavesArray[i].fRandom*fElapsedTime * 0.3f;
		if ( pLeavesArray[i].fLoopSum<=0.f )
		{
			pLeavesArray[i].fLoopSum = LEAVESTIME;
			pLeavesArray[i].vWindPos = D3DXVECTOR2 ( 0.f, 0.f );
		}
		fLoop = pLeavesArray[i].fLoopSum;

		fLimitXZ = fLoop - LEAVESTIMEDOWN;
		fLimitY = (fLimitXZ<0.f) ? 0.f : fLimitXZ;
		
		fLimitY = fLimitY / LEAVESTIMEUP;		// 0~1 사이의 값으로 바꿈
		fLimitY = pow(fLimitY,2);				// 부드럽게 떨어지도록 수치 바꿈

		pLeavesArray[i].vPos.x	=	pVertexArray[i].x - pLeavesArray[i].vDirection.x*fLeavesSpeed*fLimitXZ;	// 원본
		pLeavesArray[i].vPos.y	=	pVertexArray[i].y - pLeavesArray[i].vDirection.y*50.f*fLimitY;	// 원본
		pLeavesArray[i].vPos.z	=	pVertexArray[i].z - pLeavesArray[i].vDirection.z*fLeavesSpeed*fLimitXZ;	// 원본

		pLeavesArray[i].vPos.y	+=	0.5f;				// 기본 높이

		fRand = (sinf(fLoop)-0.5f) * 3.f;
		pLeavesArray[i].vPos.x	+= fRand;				// 랜덤하게 흔들리도록
		pLeavesArray[i].vPos.z	-= fRand;				// 랜덤하게 흔들리도록

		pLeavesArray[i].fRotate	+=	(0.01f*pLeavesArray[i].fRandom);


		if ( fLoop >= LEAVESTIMEALPHA )
		{
			pLeavesArray[i].fSize = 1.f;								// 원하는 크기를 넣으면 된다.
		}
		else
		{
			pLeavesArray[i].fSize = fLoop / LEAVESTIMEALPHA;
		}
	}
}

HRESULT DxEffectRainPoint::SEPERATEOBJ::RenderLeaves ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;

	PROFILE_BEGIN("DxEffectLeaves");

	if ( !m_pLeavesVB || !m_pIB )		goto	_RETURN;

	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	// Note : 비율을 맞춘다.
	//
	float fFaceRate = GetLengthRate ( vFromPt );

	DWORD dwRenderFaces = dwFaces;

	float fApplyRate = DxWeatherMan::GetInstance()->GetLeaves()->GetApplyRate();
	dwRenderFaces = (DWORD)(dwFaces*fApplyRate*fFaceRate);

	if ( dwRenderFaces <= 0 )					goto	_RETURN;
	if ( dwRenderFaces > MAX_OBJECT_LEAVES )	dwRenderFaces = MAX_OBJECT_LEAVES;		// 최대 갯수를 제한한다.

	//	Note : Leaves 파티클 업데이트
	//
	UpdateLeaves ( fElapsedTime03, dwRenderFaces );

	// Note : StateBlock Capture
	m_pSB_Leaves_SAVE->Capture();
	m_pSB_Leaves->Apply();

	pd3dDevice->SetTexture ( 0, pLeavesTex );

	pd3dDevice->SetFVF ( LEAVESRENDER::FVF );
	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pLeavesVB, 0, sizeof(LEAVESRENDER) );

	//	Note : Draw
	//
	{
		D3DXVECTOR3	vLocal;
		D3DXVECTOR3	vDirUp ( 0.f, 1.f, 0.f );
		D3DXMATRIX	matRotate, matRotateY, matRotateZ;
		float		fScale;
	//	D3DXMatrixRotationY ( &matRotateY, D3DX_PI*fTime*1.f );

		LEAVESRENDER* pVertices;
		m_pLeavesVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );

		for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
		{
			fScale	= pLeavesArray[i].fSize * fLeavesSize;

			D3DXMatrixRotationAxis ( &matRotate, &pLeavesArray[i].vDirection, pLeavesArray[i].fRotate );
		//	D3DXMatrixMultiply ( &matRotate, &matRotateY, &matRotate );

			vLocal = D3DXVECTOR3( -fScale*3, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+0].vPos, &vLocal, &matRotate );
			vLocal = D3DXVECTOR3( fScale*3, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+1].vPos, &vLocal, &matRotate );
			vLocal = D3DXVECTOR3( -fScale*3, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+2].vPos, &vLocal, &matRotate );
			vLocal = D3DXVECTOR3( fScale*3, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[(i*4)+3].vPos, &vLocal, &matRotate );

			pVertices[(i*4)+0].vPos += pLeavesArray[i].vPos;	// 앞면
			pVertices[(i*4)+1].vPos += pLeavesArray[i].vPos;
			pVertices[(i*4)+2].vPos += pLeavesArray[i].vPos;
			pVertices[(i*4)+3].vPos += pLeavesArray[i].vPos;

			vLocal = D3DXVECTOR3( 0.f, 0.f, -1.f );
			D3DXVec3TransformNormal ( &vLocal, &vLocal, &matRotate );

			pVertices[(i*4)+0].vNor = vLocal;					// 앞면
			pVertices[(i*4)+1].vNor = vLocal;
			pVertices[(i*4)+2].vNor = vLocal;
			pVertices[(i*4)+3].vNor = vLocal;
		}

		m_pLeavesVB->Unlock ();

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwRenderFaces*4, 0, dwRenderFaces*2 );

		m_pLeavesVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
		for ( DWORD i=0; i<dwRenderFaces; i++ )
		{
			vLocal = -pVertices[(i*4)+0].vNor;

			pVertices[(i*4)+0].vNor = vLocal;
			pVertices[(i*4)+1].vNor = vLocal;
			pVertices[(i*4)+2].vNor = vLocal;
			pVertices[(i*4)+3].vNor = vLocal;
		}

		m_pLeavesVB->Unlock ();

		DWORD	dwCullMode;
		pd3dDevice->GetRenderState( D3DRS_CULLMODE,			&dwCullMode );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CW );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwRenderFaces*4, 0, dwRenderFaces*2 );

		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			dwCullMode );
	}

	// Note : StateBlock Apply
	m_pSB_Leaves_SAVE->Apply();

_RETURN:
	PROFILE_END("DxEffectLeaves");

	return hr;
}

HRESULT DxEffectRainPoint::SEPERATEOBJ::RenderGround ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
/*
	PROFILE_BEGIN("DxEffectGround");

	if ( !m_pVB || !m_pIB )		goto	_RETURN;

	//D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	//D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	//// Note : 비율을 맞춘다.
	////
	//float fFaceRate = GetLengthRate ( vFromPt );

	DWORD dwRenderFaces = dwGroundFaces;

	//float fApplyRate = DxWeatherMan::GetInstance()->GetLeaves()->GetApplyRate();
	//dwRenderFaces = (DWORD)(dwFaces*fApplyRate*fFaceRate);


	// Note : StateBlock Capture
	m_pSB_Grounds_SAVE->Capture();
	m_pSB_Grounds->Apply();

	pd3dDevice->SetTexture ( 0, pGroundTex );

	pd3dDevice->SetFVF ( SNOWRENDER::FVF );
	pd3dDevice->SetIndices ( m_pIB, 0 );
	pd3dDevice->SetStreamSource ( 0, m_pVB, sizeof(SNOWRENDER) );

	//	Note : Ground
	//
	{
		float fPosX, fPosZ;
		float fBase = fGroundSize*0.5f;
		float fRandom = fGroundSize*0.8f;

		SNOWRENDER* pVertices;
		m_pVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );

		for ( DWORD i=0; i<dwRenderFaces; i++ )		// 계산도 조금 시킨다.
		{
			fPosX = pGroundArray[i].fLengthX*fRandom + fBase;
			fPosZ = pGroundArray[i].fLengthZ*fRandom + fBase;

			pVertices[(i*4)+0].vPos.x = pGroundArray[i].vLeftUp.x * fPosX;
			pVertices[(i*4)+0].vPos.y = 0.f;
			pVertices[(i*4)+0].vPos.z = pGroundArray[i].vLeftUp.z * fPosZ;
			pVertices[(i*4)+0].vPos	+= pGroundArray[i].vPos;

			pVertices[(i*4)+1].vPos.x = pGroundArray[i].vLeftDown.x * fPosX;
			pVertices[(i*4)+1].vPos.y = 0.f;
			pVertices[(i*4)+1].vPos.z = pGroundArray[i].vLeftDown.z * fPosZ;
			pVertices[(i*4)+1].vPos	+= pGroundArray[i].vPos;

			pVertices[(i*4)+2].vPos.x = pGroundArray[i].vRightUp.x * fPosX;
			pVertices[(i*4)+2].vPos.y = 0.f;
			pVertices[(i*4)+2].vPos.z = pGroundArray[i].vRightUp.z * fPosZ;
			pVertices[(i*4)+2].vPos	+= pGroundArray[i].vPos;

			pVertices[(i*4)+3].vPos.x = pGroundArray[i].vRightDown.x * fPosX;
			pVertices[(i*4)+3].vPos.y = 0.f;
			pVertices[(i*4)+3].vPos.z = pGroundArray[i].vRightDown.z * fPosZ;
			pVertices[(i*4)+3].vPos	+= pGroundArray[i].vPos;
		}

		m_pVB->Unlock ();

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, dwRenderFaces*4, 0, dwRenderFaces*2 );
	}

	// Note : StateBlock Apply
	m_pSB_Grounds_SAVE->Apply();

_RETURN:
	PROFILE_END("DxEffectGround");
*/
	return hr;
}

void DxEffectRainPoint::SEPERATEOBJ::Save( CSerialFile &SFile )
{
	SFile << dwFaces;
	SFile << dwGroundFaces;
	SFile << vMax;
	SFile << vMin;
	SFile << vCenter;

	if ( dwFaces )
	{
		SFile.WriteBuffer ( pVertexArray, sizeof(D3DXVECTOR3)*dwFaces );

		/*SFile.WriteBuffer ( pRainArray, sizeof(RAINVERTEX)*dwFaces );

		SFile.WriteBuffer ( pSnowArray, sizeof(SNOWVERTEX)*dwFaces );

		SFile.WriteBuffer ( pLeavesArray, sizeof(LEAVESVERTEX)*dwFaces );*/
	}
}

void DxEffectRainPoint::SEPERATEOBJ::Load( CSerialFile &SFile, DWORD version )
{
	SFile >> dwFaces;
	SFile >> dwGroundFaces;
	SFile >> vMax;
	SFile >> vMin;
	SFile >> vCenter;

	SAFE_DELETE_ARRAY( pVertexArray );
	SAFE_DELETE_ARRAY( pRainArray );
	SAFE_DELETE_ARRAY( pSnowArray );
	SAFE_DELETE_ARRAY( pLeavesArray );

	if ( dwFaces )
	{
		pVertexArray = new D3DXVECTOR3[dwFaces];
		SFile.ReadBuffer ( pVertexArray, sizeof(D3DXVECTOR3)*dwFaces );

		if( version == VERSION || version == 0x105 )
		{
			CreateSeperateVB( NULL );
		}
		else if( version == 0x00000104 )
		{		
			pRainArray = new RAINVERTEX[dwFaces];
			SFile.ReadBuffer ( pRainArray, sizeof(RAINVERTEX)*dwFaces );
			
			pSnowArray = new SNOWVERTEX[dwFaces];
			SFile.ReadBuffer ( pSnowArray, sizeof(SNOWVERTEX)*dwFaces );
			
			pLeavesArray = new LEAVESVERTEX[dwFaces];
			SFile.ReadBuffer ( pLeavesArray, sizeof(LEAVESVERTEX)*dwFaces );
		}

		SAFE_DELETE_ARRAY ( pFireFlyArray );
		pFireFlyArray = new FIREFLYORIGINAL[dwFaces];
		for( DWORD i=0; i<dwFaces; i++ )
		{
			pFireFlyArray[i].fCurTime	= 0.f;						// 현재 시간
			pFireFlyArray[i].fEndTime	= 5.f+(RANDOM_POS*25.f);	// 5~30 사이 값
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//  새로 추가된 날씨 효과들
//////////////////////////////////////////////////////////////////////////

void SParticleArea::FirstCreateParticle( const SParticleData snowData, const SParticleData rainData, const SParticleData leavesData )
{
	vecSnowParticle.clear();
	vecRainParticle.clear();
	vecLeavesParticle.clear();

	int i = 0;

	// Create New Snow Effect 
	{
		for( i = 0; i < snowData.Count; i++ )
		{
			SParticle inputParticle;
			D3DXVECTOR3 vPos, vDir;
			float fDisappearYPos, fFallSpeed, fWaveRate;
			vPos = D3DXVECTOR3( RandomNumber( vMin.x, vMax.x ), RandomNumber( 0.0f, 300.0f), RandomNumber( vMin.z, vMax.z ) );
			vDir = D3DXVECTOR3( RandomNumber( -0.5f, 0.5f ), 0.0f, RandomNumber( -0.5f, 0.5f ) );

			fDisappearYPos = RandomNumber( snowData.fMinDisappear, snowData.fMaxDisappear );
			fFallSpeed = RandomNumber( -snowData.fMaxFallSpeed, -snowData.fMinFallSpeed );
			fWaveRate = RandomNumber( 4.0f, 10.0f );

			inputParticle.CreateParticle( vDir, vPos, fDisappearYPos, fFallSpeed, fWaveRate, vPos );

			vecSnowParticle.push_back( inputParticle );
		}
	}

	// Create New Rain Effect 
	{
		for( i = 0; i < rainData.Count; i++ )
		{
			SParticle inputParticle;
			D3DXVECTOR3 vPos, vDir;
			float fDisappearYPos, fFallSpeed, fWaveRate;
			vPos = D3DXVECTOR3( RandomNumber( vMin.x, vMax.x ), RandomNumber( 0.0f, 300.0f), RandomNumber( vMin.z, vMax.z ) );

			fDisappearYPos = RandomNumber( rainData.fMinDisappear, rainData.fMaxDisappear );
			fFallSpeed = RandomNumber( -rainData.fMaxFallSpeed, -rainData.fMinFallSpeed );
			fWaveRate = 0.0f;

			vDir = D3DXVECTOR3( RandomNumber( 1.5f, 2.0f ), fFallSpeed, RandomNumber( 1.5f, 2.0f ) );

			inputParticle.CreateParticle( vDir, vPos, fDisappearYPos, fFallSpeed, fWaveRate, vPos );

			vecRainParticle.push_back( inputParticle );
		}
	}

	// Create New Leaves Effect 
	{
		for( i = 0; i < leavesData.Count; i++ )
		{
			SParticle inputParticle;
			D3DXVECTOR3 vPos, vDir;
			float fDisappearYPos, fFallSpeed, fWaveRate;
			vPos = D3DXVECTOR3( RandomNumber( vMin.x, vMax.x ), RandomNumber( 0.0f, 300.0f), RandomNumber( vMin.z, vMax.z ) );
			vDir = D3DXVECTOR3( RandomNumber( -1.0f, 1.0f ), 0.0f, RandomNumber( -1.0f, 1.0f ) );

			fDisappearYPos = RandomNumber( leavesData.fMinDisappear, leavesData.fMaxDisappear );
			fFallSpeed = RandomNumber( -leavesData.fMaxFallSpeed, -leavesData.fMinFallSpeed );
			fWaveRate = RandomNumber( 15.0f, 30.0f );

			inputParticle.CreateParticle( vDir, vPos, fDisappearYPos, fFallSpeed, fWaveRate, vPos );

			vecLeavesParticle.push_back( inputParticle );
		}
	}

}

bool DxEffectRainPoint::CNEWRAINDATA::UpdateNewSnow( SParticleArea &partArea )
{
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 vLength;
	UINT i;
	float fDir;


	vLength = partArea.vCenter - vLookat;
	float fLength = D3DXVec3Length ( &vLength );
	if( fLength > 600.0f ) return FALSE;

	vLength = vLookat - vFromPt;

	D3DXPLANE plane;	
	D3DXVECTOR3 vTemp, vLeft, v1, v2;
	v1 = vLength;
	v2 = vLookat - D3DXVECTOR3( vLookat.x, vLookat.y + 3.0f, vLookat.z );
	D3DXVec3Scale( &vLeft, D3DXVec3Cross( &vTemp, &v1, &v2 ), 3.0f );
	vTemp = D3DXVECTOR3( vFromPt.x, vFromPt.y + 3.0f, vFromPt.z );
	D3DXPlaneFromPoints( &plane, &vFromPt, &vLeft, &vTemp );

	if( D3DXPlaneDotCoord( &plane, &partArea.vCenter ) < -1.0f ) return FALSE;


	for( i = 0; i < partArea.vecSnowParticle.size(); i++ )
	{
		partArea.vecSnowParticle[i].bRender = TRUE;
		vLength = partArea.vecSnowParticle[i].vPos - vFromPt;


	
		fDir = sinf(partArea.vecSnowParticle[i].vPos.y/5.0f)/partArea.vecSnowParticle[i].fWaveRate;			// 흔들림을 준다.

		partArea.vecSnowParticle[i].vPos += partArea.vecSnowParticle[i].vDir;
		partArea.vecSnowParticle[i].vPos.y += partArea.vecSnowParticle[i].fFallSpeed * fElapsedTime;
		partArea.vecSnowParticle[i].vPos.x += fDir;
		partArea.vecSnowParticle[i].vPos.z += fDir;

		if( partArea.vecSnowParticle[i].vPos.y <= partArea.vecSnowParticle[i].fDisappearYPos )
		{
			partArea.vecSnowParticle[i].vPos   = partArea.vecSnowParticle[i].vFirstCreatePos;
			partArea.vecSnowParticle[i].vPos.y = 300.0f;
		}
	}
	return TRUE;
}
void DxEffectRainPoint::CNEWRAINDATA::RenderNewSnow( SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !UpdateNewSnow(partArea) ) return;


	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	D3DXMATRIX camBillBoard, matBillboard, camMatrix;	
	D3DXMatrixIdentity( &camBillBoard );
	D3DXMatrixIdentity( &matBillboard );
	D3DXMatrixIdentity( &camMatrix );

	// Note : StateBlock Capture
	m_pSB_Snow_SAVE->Capture();
	m_pSB_Snow->Apply();


	pd3dDevice->GetTransform( D3DTS_VIEW, &camMatrix );
	camBillBoard._11 = camMatrix._11;
	camBillBoard._12 = camMatrix._12;
	camBillBoard._13 = camMatrix._13;
	camBillBoard._21 = camMatrix._21;
	camBillBoard._22 = camMatrix._22;
	camBillBoard._23 = camMatrix._23;
	camBillBoard._31 = camMatrix._31;
	camBillBoard._32 = camMatrix._32;
	camBillBoard._33 = camMatrix._33;
	D3DXMatrixInverse( &matBillboard, NULL, &camBillBoard );

	D3DXMATRIX world, vTrans, vScale, vRotate;

	pd3dDevice->SetTexture ( 0, pSnowTex );
	pd3dDevice->SetStreamSource( 0, m_pNewSnowVB, 0, sizeof(SNOWRENDER) );
	pd3dDevice->SetFVF( SNOWRENDER::FVF );



	UINT i;
	for( i = 0; i < partArea.vecSnowParticle.size(); i++ )
	{
		if( partArea.vecSnowParticle[i].bRender == FALSE )
		{
			continue;
		}
		D3DXMatrixTranslation( &vTrans, partArea.vecSnowParticle[i].vPos.x, partArea.vecSnowParticle[i].vPos.y,
										partArea.vecSnowParticle[i].vPos.z );

		world = matBillboard * vTrans;

		pd3dDevice->SetTransform( D3DTS_WORLD, &world );		
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );


	}

	// Note : StateBlock Apply
	m_pSB_Snow_SAVE->Apply();
}


bool DxEffectRainPoint::CNEWRAINDATA::UpdateNewRain( SParticleArea &partArea )
{
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 vLength;
	UINT i;


	vLength = partArea.vCenter - vLookat;
	float fLength = D3DXVec3Length ( &vLength );
	if( fLength > 600.0f ) return FALSE;

	vLength = vLookat - vFromPt;

	D3DXPLANE plane;	
	D3DXVECTOR3 vTemp, vLeft, v1, v2;
	v1 = vLength;
	v2 = vLookat - D3DXVECTOR3( vLookat.x, vLookat.y + 3.0f, vLookat.z );
	D3DXVec3Scale( &vLeft, D3DXVec3Cross( &vTemp, &v1, &v2 ), 3.0f );
	vTemp = D3DXVECTOR3( vFromPt.x, vFromPt.y + 3.0f, vFromPt.z );
	D3DXPlaneFromPoints( &plane, &vFromPt, &vLeft, &vTemp );

	if( D3DXPlaneDotCoord( &plane, &partArea.vCenter ) < -1.0f ) return FALSE;



	for( i = 0; i < partArea.vecRainParticle.size(); i++ )
	{
		partArea.vecRainParticle[i].bRender = TRUE;
		vLength = partArea.vecRainParticle[i].vPos - vFromPt;



		partArea.vecRainParticle[i].vPos += partArea.vecRainParticle[i].vDir;

		if( partArea.vecRainParticle[i].vPos.y <= partArea.vecRainParticle[i].fDisappearYPos )
		{
			partArea.vecRainParticle[i].vPos   = partArea.vecRainParticle[i].vFirstCreatePos;
			partArea.vecRainParticle[i].vPos.y = 300.0f;
		}
	}
	return TRUE;
}
void DxEffectRainPoint::CNEWRAINDATA::RenderNewRain(  SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !UpdateNewRain(partArea) ) return;


	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	D3DXMATRIX camBillBoard, matBillboard, camMatrix;	
	D3DXMatrixIdentity( &camBillBoard );
	D3DXMatrixIdentity( &matBillboard );
	D3DXMatrixIdentity( &camMatrix );

	// Note : StateBlock Capture
	m_pSB_Effect_SAVE->Capture();
	m_pSB_Effect->Apply();


	pd3dDevice->GetTransform( D3DTS_VIEW, &camMatrix );
	camBillBoard._11 = camMatrix._11;
	//camBillBoard._12 = camMatrix._12;
	camBillBoard._13 = camMatrix._13;
	/*camBillBoard._21 = camMatrix._21;
	camBillBoard._22 = camMatrix._22;
	camBillBoard._23 = camMatrix._23;*/
	camBillBoard._31 = camMatrix._31;
	//camBillBoard._32 = camMatrix._32;
	camBillBoard._33 = camMatrix._33;
	D3DXMatrixInverse( &matBillboard, NULL, &camBillBoard );

	D3DXMATRIX world, vTrans, vScale, vRotate;

	pd3dDevice->SetTexture ( 0, pRainTex );
	pd3dDevice->SetFVF( RAINRENDER::FVF );

	RAINRENDER pointVertex[4]; 
	ZeroMemory( pointVertex, 4 );
	pointVertex[0].vPos.x = pointVertex[2].vPos.x = -0.3f;
	pointVertex[0].vPos.y = pointVertex[1].vPos.y =  0.3f;
	pointVertex[1].vPos.x = pointVertex[3].vPos.x =  0.3f;
	pointVertex[2].vPos.y = pointVertex[3].vPos.y = -0.3f;
	pointVertex[0].vPos.z = pointVertex[1].vPos.z = pointVertex[2].vPos.z = pointVertex[3].vPos.z = 0.0f;

	DWORD	dwColor = ((70)<<24) + ((70)<<16) + ((70)<<8) + 70;

	pointVertex[0].cColor = pointVertex[2].cColor = dwColor;
	pointVertex[1].cColor = pointVertex[3].cColor = 0x00000000;

	pointVertex[1].vTex.x = pointVertex[2].vTex.y = pointVertex[3].vTex.x = pointVertex[3].vTex.y = 1.0f;
	pointVertex[0].vTex.x =	pointVertex[2].vTex.x = pointVertex[0].vTex.y = pointVertex[1].vTex.y = 0.0f;


	DWORD dwCullMode;

	pd3dDevice->GetRenderState( D3DRS_CULLMODE,			&dwCullMode );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );

	UINT i;
	float fRainLength = 0.0f;
	for( i = 0; i < partArea.vecRainParticle.size(); i++ )
	{
		if( partArea.vecRainParticle[i].bRender == FALSE )
		{
			continue;
		}

		fRainLength = partArea.vecRainParticle[i].fFallSpeed / 2.0f;

		pointVertex[2].vPos.x = partArea.vecRainParticle[i].vDir.x * fRainLength;
		pointVertex[2].vPos.y = partArea.vecRainParticle[i].vDir.y * fRainLength;
		pointVertex[2].vPos.z = partArea.vecRainParticle[i].vDir.z * fRainLength;
		pointVertex[3].vPos.x = partArea.vecRainParticle[i].vDir.x * fRainLength;
		pointVertex[3].vPos.y = partArea.vecRainParticle[i].vDir.y * fRainLength;
		pointVertex[3].vPos.z = partArea.vecRainParticle[i].vDir.z * fRainLength;
		D3DXMatrixTranslation( &vTrans, partArea.vecRainParticle[i].vPos.x, partArea.vecRainParticle[i].vPos.y, partArea.vecRainParticle[i].vPos.z );

		world = /*matBillboard **/ vTrans;
		//world = vScale * vRotate * matBillboard * vTrans;


		pd3dDevice->SetTransform( D3DTS_WORLD, &world );		
		pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pointVertex, sizeof(RAINRENDER) );
	}


	pd3dDevice->SetRenderState( D3DRS_CULLMODE,			dwCullMode );

	// Note : StateBlock Apply
	m_pSB_Effect_SAVE->Apply();
}

bool DxEffectRainPoint::CNEWRAINDATA::UpdateNewLeaves( SParticleArea &partArea )
{
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 vLength;
	UINT i;
	float fDir;


	vLength = partArea.vCenter - vLookat;
	float fLength = D3DXVec3Length ( &vLength );
	if( fLength > 600.0f ) return FALSE;

	vLength = vLookat - vFromPt;

	D3DXPLANE plane;	
	D3DXVECTOR3 vTemp, vLeft, v1, v2;
	v1 = vLength;
	v2 = vLookat - D3DXVECTOR3( vLookat.x, vLookat.y + 3.0f, vLookat.z );
	D3DXVec3Scale( &vLeft, D3DXVec3Cross( &vTemp, &v1, &v2 ), 3.0f );
	vTemp = D3DXVECTOR3( vFromPt.x, vFromPt.y + 3.0f, vFromPt.z );
	D3DXPlaneFromPoints( &plane, &vFromPt, &vLeft, &vTemp );

	if( D3DXPlaneDotCoord( &plane, &partArea.vCenter ) < -1.0f ) return FALSE;



	for( i = 0; i < partArea.vecLeavesParticle.size(); i++ )
	{
		partArea.vecLeavesParticle[i].bRender = TRUE;
		vLength = partArea.vecLeavesParticle[i].vPos - vFromPt;

		fDir = sinf(partArea.vecLeavesParticle[i].vPos.y/5.0f)/partArea.vecLeavesParticle[i].fWaveRate;			// 흔들림을 준다.

		partArea.vecLeavesParticle[i].vPos += partArea.vecLeavesParticle[i].vDir;
		partArea.vecLeavesParticle[i].vPos.y += partArea.vecLeavesParticle[i].fFallSpeed * fElapsedTime;
		partArea.vecLeavesParticle[i].vPos.x += fDir;
		partArea.vecLeavesParticle[i].vPos.z += fDir;

		if( partArea.vecLeavesParticle[i].vPos.y <= partArea.vecLeavesParticle[i].fDisappearYPos )
		{
			partArea.vecLeavesParticle[i].vPos   = partArea.vecLeavesParticle[i].vFirstCreatePos;
			partArea.vecLeavesParticle[i].vPos.y = 300.0f;
		}
	}
	return TRUE;
}
void DxEffectRainPoint::CNEWRAINDATA::RenderNewLeaves(  SParticleArea &partArea, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !UpdateNewLeaves(partArea) ) return;


	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookat = DxViewPort::GetInstance().GetLookatPt ();

	D3DXMATRIX camBillBoard, matBillboard, camMatrix;	
	D3DXMatrixIdentity( &camBillBoard );
	D3DXMatrixIdentity( &matBillboard );
	D3DXMatrixIdentity( &camMatrix );

	// Note : StateBlock Capture
	m_pSB_Leaves_SAVE->Capture();
	m_pSB_Leaves->Apply();


	pd3dDevice->GetTransform( D3DTS_VIEW, &camMatrix );
	camBillBoard._11 = camMatrix._11;
	camBillBoard._13 = camMatrix._13;
	camBillBoard._31 = camMatrix._31;
	camBillBoard._33 = camMatrix._33;
	D3DXMatrixInverse( &matBillboard, NULL, &camBillBoard );

	D3DXMATRIX world, matTrans, matScale, matRotate;

	pd3dDevice->SetTexture ( 0, pLeavesTex );
	pd3dDevice->SetStreamSource( 0, m_pNewLeavesVB, 0, sizeof(LEAVESRENDER) );
	pd3dDevice->SetFVF( LEAVESRENDER::FVF );


	DWORD dwCullMode;

	pd3dDevice->GetRenderState( D3DRS_CULLMODE,			&dwCullMode );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );


	UINT i;
	for( i = 0; i < partArea.vecLeavesParticle.size(); i++ )
	{
		if( partArea.vecLeavesParticle[i].bRender == FALSE )
		{
			continue;
		}
		D3DXMatrixTranslation( &matTrans, partArea.vecLeavesParticle[i].vPos.x, partArea.vecLeavesParticle[i].vPos.y, partArea.vecLeavesParticle[i].vPos.z );

		D3DXMatrixRotationAxis ( &matRotate, &partArea.vecLeavesParticle[i].vDir, partArea.vecLeavesParticle[i].vPos.y / 10.0f );

		world = matRotate * matBillboard * matTrans;

		pd3dDevice->SetTransform( D3DTS_WORLD, &world );		
		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	}

	pd3dDevice->SetRenderState( D3DRS_CULLMODE,			dwCullMode );
	// Note : StateBlock Apply
	m_pSB_Leaves_SAVE->Apply();
}


void DxEffectRainPoint::CNEWRAINDATA::CreateParticleArea( D3DXVECTOR3 vMin, D3DXVECTOR3 vMax, D3DXVECTOR3 vCenter,
														  SParticleData SnowParticleData, SParticleData RainParticleData, SParticleData LeavesParticleData )
{
	SParticleArea inputData;
	inputData.vCenter = vCenter;
	inputData.vMin    = vMin;
	inputData.vMax    = vMax;
	inputData.FirstCreateParticle( SnowParticleData, RainParticleData, LeavesParticleData );

	vecParticleArea.push_back(inputData);
}

void DxEffectRainPoint::CNEWRAINDATA::SetTexture ( LPDIRECT3DTEXTUREQ _pRainTex, LPDIRECT3DTEXTUREQ _pCircleTex, LPDIRECT3DTEXTUREQ _pRainDropTex,
				 LPDIRECT3DTEXTUREQ _pSnowTex, LPDIRECT3DTEXTUREQ _pLeavesTex,
				 LPDIRECT3DTEXTUREQ _pGroundTex, LPDIRECT3DTEXTUREQ _pRain_GTex, LPDIRECT3DTEXTUREQ _pSnow_GTex )
{
	pRainTex		= _pRainTex;
	pCircleTex		= _pCircleTex;
	pRainDropTex	= _pRainDropTex;
	pSnowTex		= _pSnowTex;
	pLeavesTex		= _pLeavesTex;

	pGroundTex		= _pGroundTex;
	pRain_GTex		= _pRain_GTex;
	pSnow_GTex		= _pSnow_GTex;
}

void DxEffectRainPoint::CNEWRAINDATA::NewRender( LPDIRECT3DDEVICEQ pd3dDevice )
{
	UINT i;
	for( i = 0; i < vecParticleArea.size(); i++ )
	{
		if ( dwFlag & PLAY_LEAVES )		RenderNewLeaves ( vecParticleArea[i], pd3dDevice );
		if( dwFlag & (PLAY_RAIN | PLAY_SNOW) )
		{
			if ( dwFlag & PLAY_RAIN )		RenderNewRain ( vecParticleArea[i], pd3dDevice );
			if ( dwFlag & PLAY_SNOW )		RenderNewSnow ( vecParticleArea[i], pd3dDevice );
		}
	}
}













