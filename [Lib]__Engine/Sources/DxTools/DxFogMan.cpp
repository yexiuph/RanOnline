#include "pch.h"

#include "./glperiod.h"

#include "DxInputDevice.h"
#include "./DxSkinChar.h"
#include "DxViewPort.h"
#include "DxLightMan.h"
#include "./TextureManager.h"
#include "./SerialFile.h"

#include "DxWeatherMan.h"
#include "DxLightMan.h"
#include "DxBackUpRendTarget.h"
#include "DxSurfaceTex.h"

#include "DxFogMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static void ColorUp( DWORD& dwColor, float fValue )
{
	int nR = ((dwColor&0xff0000)>>16);
	int nG = ((dwColor&0xff00)>>8);
	int nB = (dwColor&0xff);

	//int nDiffR = 255 - nR;
	//int nDiffG = 255 - nG;
	//int nDiffB = 255 - nB;

	//nDiffR = (int)(nDiffR*fValue);
	//nDiffG = (int)(nDiffG*fValue);
	//nDiffB = (int)(nDiffB*fValue);

	//nR = nR + nDiffR;
	//nG = nG + nDiffG;
	//nB = nB + nDiffB;

	nR = (int)(nR * fValue);
	nG = (int)(nG * fValue);
	nB = (int)(nB * fValue);

	if( nR < 0 )		nR = 0;
	if( nG < 0 )		nG = 0;
	if( nB < 0 )		nB = 0;
	if( nR > 255 )		nR = 255;
	if( nG > 255 )		nG = 255;
	if( nB > 255 )		nB = 255;

	dwColor = 0xff000000;
	dwColor += (nR<<16);
	dwColor += (nG<<8);
	dwColor += nB;
}

void	FOG_PROPERTY::SaveSet ( CSerialFile &SFile )
{
	SFile << VERSION;
	SFile << (DWORD)sizeof(FOG_PROPERTY);

	SFile << m_bSkyFogEnable;
	SFile << m_bFogStaticRange;
	SFile << m_fFogStart;
	SFile << m_fFogEnd;
	SFile << m_cFogColor;
	SFile << m_cFogColor_Day;
	SFile << m_cFogColor_Night;
}

void	FOG_PROPERTY::LoadSet ( CSerialFile &SFile )
{
	DWORD	dwVer;
	DWORD	dwBuffSize;
	BOOL	bTemp;
	float	fTemp;

	m_bNewRange = FALSE;

	SFile >> dwVer;
	SFile >> dwBuffSize;
	if( dwVer == VERSION )
	{
		SFile >> m_bSkyFogEnable;
		SFile >> m_bFogStaticRange;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor;
		SFile >> m_cFogColor_Day;
		SFile >> m_cFogColor_Night;

		m_bNewRange = TRUE;
	}
	else if( dwVer == 0x0107 )
	{
		SFile >> m_bSkyFogEnable;
		SFile >> m_bFogStaticRange;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor;
		SFile >> m_cFogColor_Day;
		SFile >> m_cFogColor_Night;

		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );				// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_bNewRange = TRUE;
	}
	else if( dwVer == 0x0106 )
	{
		SFile >> m_bSkyFogEnable;
		SFile >> m_bFogStaticRange;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor;
		SFile >> m_cFogColor_Day;
		SFile >> m_cFogColor_Night;

		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );				// Ver.108에서 밤 하늘색을 전체적으로 올림.
	}
	else if( dwVer == 0x0105 )
	{
		SFile >> m_bSkyFogEnable;
		SFile >> m_cFogColor;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> m_cFogColor_Day;
		SFile >> fTemp;
		SFile >> fTemp;
		SFile >> m_cFogColor_Night;
		SFile >> fTemp;
		SFile >> fTemp;

		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );				// Ver.108에서 밤 하늘색을 전체적으로 올림.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else if( dwVer == 0x0104 )
	{
		SFile >> bTemp;
		SFile >> m_bSkyFogEnable;
		SFile >> m_cFogColor_Day;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> fTemp;
		SFile >> m_cFogColor_Night;
		SFile >> fTemp;
		SFile >> fTemp;
		SFile >> fTemp;

		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );				// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_cFogColor = m_cFogColor_Day;					// Ver.105에서 추가. 옮기고 넣는다.
		m_cFogColor_Day = D3DCOLOR_XRGB(89,135,179);	// Ver.105에서 추가. 옮기고 넣는다.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else if( dwVer == 0x0103 )
	{
		SFile >> bTemp;
		SFile >> m_bSkyFogEnable;
		SFile >> m_cFogColor_Day;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> fTemp;
		SFile >> m_cFogColor_Night;
		SFile >> fTemp;
		SFile >> fTemp;
		SFile >> fTemp;

		m_cFogColor_Night	= D3DCOLOR_XRGB(35,35,55);		// Ver.104 로 가면서 밤하늘색을 일괄적으로 바꿈
		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );					// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_cFogColor = m_cFogColor_Day;					// Ver.105에서 추가. 옮기고 넣는다.
		m_cFogColor_Day = D3DCOLOR_XRGB(89,135,179);	// Ver.105에서 추가. 옮기고 넣는다.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else if( dwVer == 0x0102 )
	{
		SFile >> bTemp;
		SFile >> m_cFogColor_Day;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> fTemp;

		m_bSkyFogEnable		= FALSE;							// Ver.103에서 추가
		m_cFogColor_Night	= D3DCOLOR_XRGB(35,35,55);		// Ver.104 로 가면서 밤하늘색을 일괄적으로 바꿈
		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );					// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_cFogColor = m_cFogColor_Day;					// Ver.105에서 추가. 옮기고 넣는다.
		m_cFogColor_Day = D3DCOLOR_XRGB(89,135,179);	// Ver.105에서 추가. 옮기고 넣는다.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else if ( dwVer == 0x0101 )
	{
		SFile >> bTemp;
		SFile >> m_cFogColor_Day;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> fTemp;

		m_bSkyFogEnable		= FALSE;							// Ver.103에서 추가
		m_cFogColor_Night	= D3DCOLOR_XRGB(35,35,55);		// Ver.104 로 가면서 밤하늘색을 일괄적으로 바꿈
		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );					// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_cFogColor = m_cFogColor_Day;					// Ver.105에서 추가. 옮기고 넣는다.
		m_cFogColor_Day = D3DCOLOR_XRGB(89,135,179);	// Ver.105에서 추가. 옮기고 넣는다.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else if ( dwVer == 0x0100 )
	{
		SFile >> bTemp;
		SFile >> m_cFogColor_Day;
		SFile >> m_fFogStart;
		SFile >> m_fFogEnd;
		SFile >> fTemp;

		if ( (m_fFogStart==1720.0f) && (m_fFogEnd==1790.0f) )
		{
			m_fFogStart = 720.f;
			m_fFogEnd = 790.0f;
		}

		m_bSkyFogEnable		= FALSE;							// Ver.103에서 추가

		m_cFogColor_Night	= D3DCOLOR_XRGB(35,35,55);		// Ver.104 로 가면서 밤하늘색을 일괄적으로 바꿈
		ColorUp( m_cFogColor_Day, 0.85f );					// Ver.108에서 낮 하늘색을 전체적으로 내림.
		ColorUp( m_cFogColor_Night, 1.6f );					// Ver.108에서 밤 하늘색을 전체적으로 올림.

		m_cFogColor = m_cFogColor_Day;					// Ver.105에서 추가. 옮기고 넣는다.
		m_cFogColor_Day = D3DCOLOR_XRGB(89,135,179);	// Ver.105에서 추가. 옮기고 넣는다.

		if ( m_fFogEnd > 1100 )		m_bFogStaticRange = TRUE;	// Ver.106에서 추가
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
}

DxFogMan& DxFogMan::GetInstance()
{
	static DxFogMan Instance;
	return Instance;
}

DxFogMan::DxFogMan() :
	m_dwSavedSB(NULL),
	m_dwEffectSB(NULL),
	m_cFogColor_Game(D3DCOLOR_XRGB(89,135,179)),
	m_cFogColor_White(D3DCOLOR_XRGB(89,135,179)),
	m_cFogColor_Dark(D3DCOLOR_XRGB(89,135,179)),
	m_cFogColor_Object(D3DCOLOR_XRGB(89,135,179)),
	m_cDayColor(D3DCOLOR_XRGB(89,135,179)),
	m_bFrameMode_Fog(FALSE),
	m_emFogRange(EMFR_HIGH),
	m_bNewRange(FALSE),
	m_fRangeLow_Near(490),		// 400
	m_fRangeLow_Far(700),
	m_fRangeMiddle_Near(620),	// 500
	m_fRangeMiddle_Far(900),
	m_fRangeHigh_Near(750),		// 600
	m_fRangeHigh_Far(1100),
	m_bFogEnable(FALSE)
{
}

DxFogMan::~DxFogMan()
{
}

HRESULT DxFogMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	if ( d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )	m_bFogEnable = TRUE;
	else													m_bFogEnable = FALSE;

	return S_OK;
}

HRESULT DxFogMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

void DxFogMan::SetProperty ( FOG_PROPERTY& Property )
{
	m_Property = Property;
}

float DxFogMan::GetFogRangeFar()
{
	// 강제 셋팅
	if( m_bFogStaticRange )	return m_fFogEnd;

	// 사용자 정의
	switch( m_emFogRange )
	{
	case EMFR_LOW:		return m_fRangeLow_Far;
	case EMFR_MIDDLE:	return m_fRangeMiddle_Far;
	case EMFR_HIGH:		return m_fRangeHigh_Far;
	};
	
	// Default
	return	500.f;
}

void	DxFogMan::RenderFogSB ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, m_bFogEnable );

	// 새로운 Octree부터는 2400 까지 볼 수 있다.
	if( m_bNewRange )	m_fRangeHigh_Far = 2200.f;
	else				m_fRangeHigh_Far = 1100.f;

	m_cFogColor_Game	= m_cFogColor;
	float		fFogStart	= 0.f;
	float		fFogEnd		= 0.f;
	float		fFogDensity = 1.f;

	float fRainRate		= DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();
	float fSnowRate		= DxWeatherMan::GetInstance()->GetSnow()->GetApplyRate();
	float fWeatherRate	= (fRainRate>fSnowRate) ? fRainRate : fSnowRate;
	float fBlendFact	= GLPeriod::GetInstance().GetBlendFact();

	if ( m_bSkyFogEnable )				// 포그를 사용 할때만 컬러를 변하게 한다.
	{
		float	fBlendFact = GLPeriod::GetInstance().GetBlendFact();

		if ( fBlendFact == 1.f )
		{
			m_cFogColor_Game	= m_cFogColor_Day;
		}
		else if ( fBlendFact == 0.f )
		{
			m_cFogColor_Game	= m_cFogColor_Night;
		}
		else 
		{
			m_cFogColor_Game	= 0L;
			m_cFogColor_Game	+= (DWORD)(((m_cFogColor_Day&0xff000000)>>24)*fBlendFact + 
								((m_cFogColor_Night&0xff000000)>>24)*(1.f-fBlendFact))<<24;
			m_cFogColor_Game	+= (DWORD)(((m_cFogColor_Day&0xff0000)>>16)*fBlendFact + 
								((m_cFogColor_Night&0xff0000)>>16)*(1.f-fBlendFact))<<16;
			m_cFogColor_Game	+= (DWORD)(((m_cFogColor_Day&0xff00)>>8)*fBlendFact + 
								((m_cFogColor_Night&0xff00)>>8)*(1.f-fBlendFact))<<8;
			m_cFogColor_Game	+= (DWORD)((m_cFogColor_Day&0xff)*fBlendFact + 
								(m_cFogColor_Night&0xff)*(1.f-fBlendFact));
		}

		DWORD	dwColorR, dwColorG, dwColorB;
		dwColorR = (m_cFogColor_Game&0xff0000)>>16;
		dwColorG = (m_cFogColor_Game&0xff00)>>8;
		dwColorB = m_cFogColor_Game&0xff;

		//	Note : 하늘색을 노을로 변하게 한다.
		//
		float fRedFact	= GLPeriod::GetInstance().GetRedFact();
		if ( fRedFact > 0.f )
		{
			DWORD	dwRedColorR, dwRedColorG, dwRedColorB;
			dwRedColorR = 210;
			dwRedColorG = 130;
			dwRedColorB = 60;

			dwColorR = (DWORD)(dwColorR*(1.f-fRedFact)) + (DWORD)(dwRedColorR*fRedFact);
			dwColorG = (DWORD)(dwColorG*(1.f-fRedFact)) + (DWORD)(dwRedColorG*fRedFact);
			dwColorB = (DWORD)(dwColorB*(1.f-fRedFact)) + (DWORD)(dwRedColorB*fRedFact);
		}

		// Note : 비가 오거나 눈이 올 때 어둡게 해줌.
		if( fWeatherRate > 0.f )
		{
			dwColorR -= (DWORD)(dwColorR*0.3f*fWeatherRate);
			dwColorG -= (DWORD)(dwColorG*0.3f*fWeatherRate);
			dwColorB -= (DWORD)(dwColorB*0.3f*fWeatherRate);
		}

		//	Note : 번개 칠 시 화면을 번쩍이게 한다.
		//
		float	fThunderPOWER = DxLightMan::GetInstance()->GetThunderPOWER();
		if ( fThunderPOWER )
		{
			DWORD	dwColor;
			fThunderPOWER += fThunderPOWER * (1.f-fBlendFact) * 0.5f;	// 밤에 더 쎄게

			dwColor = (DWORD) (255.f*fThunderPOWER);

			dwColorR += dwColor;
			dwColorG += dwColor;
			dwColorB += dwColor;
		}

		// Note : 변환 데이터 삽입
		if ( dwColorR < 0 )		dwColorR = 0;
		if ( dwColorG < 0 )		dwColorG = 0;
		if ( dwColorB < 0 )		dwColorB = 0;
		if ( dwColorR > 255 )	dwColorR = 255;
		if ( dwColorG > 255 )	dwColorG = 255;
		if ( dwColorB > 255 )	dwColorB = 255;

		m_cFogColor_Game = 0L;
		m_cFogColor_Game += (dwColorR<<16) + (dwColorG<<8) + dwColorB;
	}

	m_cFogColor_Object = m_cFogColor_Game;

	DWORD dwColor = m_cFogColor_Object;
	DWORD dwR = ((dwColor&0xff0000)>>16);
	DWORD dwG = ((dwColor&0xff00)>>8);
	DWORD dwB = dwColor&0xff;
	DWORD dwDiffR = 255 - dwR;
	DWORD dwDiffG = 255 - dwG;
	DWORD dwDiffB = 255 - dwB;

	DWORD dwTempR = dwR + (DWORD)(dwDiffR*0.5f);
	DWORD dwTempG = dwG + (DWORD)(dwDiffG*0.5f);
	DWORD dwTempB = dwB + (DWORD)(dwDiffB*0.5f);
	m_cFogColor_White = 0xff000000 + (dwTempR<<16) + (dwTempG<<8) + dwTempB;

	//dwTempR = dwR;
	//dwTempG = dwG;
	//dwTempB = dwB;
	//dwTempR = (DWORD)(dwR*0.5f);
	//dwTempG = (DWORD)(dwG*0.5f);
	//dwTempB = (DWORD)(dwB*0.5f);
	//m_cFogColor_Dark = 0xff000000 + (dwR<<16) + (dwG<<8) + dwB;

	//	Note : 시야에 따른 안개 거리 조정
	if ( m_bFogStaticRange )					// 강제 셋팅일 경우이다.
	{
		fFogStart = m_fFogStart;
		fFogEnd = m_fFogEnd;
	}
	else if ( m_emFogRange == EMFR_HIGH )		// 이후는 자동적인 거리 조정이다.
	{
		fFogStart = m_fRangeHigh_Near;
		fFogEnd = m_fRangeHigh_Far;
	}
	else if ( m_emFogRange == EMFR_MIDDLE )
	{
		fFogStart = m_fRangeMiddle_Near;
		fFogEnd = m_fRangeMiddle_Far;
	}
	else	// if ( m_emFogRange == EMFR_LOW )
	{
		fFogStart = m_fRangeLow_Near;
		fFogEnd = m_fRangeLow_Far;
	}


	if ( fWeatherRate > 0.f )	fFogEnd -= 50.f*fWeatherRate;			// 비나 눈이 올때 시야를 좁힌다.
	if ( fBlendFact < 1.f )		fFogEnd -= 50.f*(1.f-fBlendFact);		// 저녁일때 시야를 좁힌다.

	DxViewPort::GetInstance().SetFarPlane ( fFogEnd );				// View Cliping -> DxViewPort FrameMove Setting
	DxViewPort::GetInstance().SetProjection_FarPlane ( fFogEnd );	// 최대 시아를 제한한다.

	fFogEnd -= 20.f;

	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,		m_cFogColor_Object		);
	pd3dDevice->SetRenderState ( D3DRS_FOGSTART,		*((DWORD *)(&fFogStart))  );
	pd3dDevice->SetRenderState ( D3DRS_FOGEND,			*((DWORD *)(&fFogEnd))	);
	pd3dDevice->SetRenderState ( D3DRS_FOGDENSITY,		*((DWORD *)(&fFogDensity)));

	pd3dDevice->SetRenderState ( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	pd3dDevice->SetRenderState ( D3DRS_FOGTABLEMODE,	D3DFOG_NONE );
}
