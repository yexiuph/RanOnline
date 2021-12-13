#include "pch.h"

#include "DxLightMan.h"
#include "dxviewport.h"
#include "DxShadowMap.h"
#include "./TextureManager.h"
#include "./SerialFile.h"

#include "./GLPeriod.h"
#include "DxSkyMan.h"
#include "dxweatherman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DxWeatherMan	DxWeatherMan::InstanceDefault;
DxWeatherMan*	DxWeatherMan::pInstance = NULL;

const	DWORD	DxWeatherMan::VERSION = 0x102;

DxWeatherMan::DxWeatherMan(void) :
	m_fTime(0),
	m_fElapsedTime(0.f),
	m_bWhimsical(false),
	m_emWeatherStats(EMWS_NULL),
	m_fApplyRate(0.f)
{
}

DxWeatherMan::~DxWeatherMan(void)
{
}

HRESULT DxWeatherMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 비 삽입
	//
	m_sRain.InitDeviceObjects ( pd3dDevice, m_vWindDirec, m_fWindPower );

	return S_OK;
}

HRESULT DxWeatherMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 비 삽입
	//
	m_sRain.RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxWeatherMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 비 지움
	//
	m_sRain.InvalidateDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxWeatherMan::DeleteDeviceObjects ()
{
	//	Note : 비 지움
	//
	m_sRain.DeleteDeviceObjects ();

	return S_OK;
}

void DxWeatherMan::ReSetSound ()
{
	m_sRain.StopSound ();
}

void DxWeatherMan::ReceiveWhimsical ( DWORD dwWhimsical )
{
	if ( !(m_dwFlag&ISACTIVE) )				return;

	if ( m_dwFlag&ISRAIN )
	{
		if ( dwWhimsical & FGW_THUNDER )
		{
			DxSkyMan::GetInstance().SetThunderON();
			DxLightMan::GetInstance()->Thunder();
			m_sRain.PlayThunder();
		}
	}
}

void DxWeatherMan::ReceiveMapWhimsical ( WORD map_Mid, WORD map_Sid, DWORD dwWhimsical  )
{
	if( map_Mid != m_currentMapID.wMainID &&
		map_Sid != m_currentMapID.wSubID )
	{
		return;
	}

	if ( !(m_dwFlag&ISACTIVE) )				return;

	if ( m_dwFlag&ISRAIN )
	{
		if ( dwWhimsical & FGW_THUNDER )
		{
			DxSkyMan::GetInstance().SetThunderON();
			DxLightMan::GetInstance()->Thunder();
			m_sRain.PlayThunder();
		}
	}
}

HRESULT DxWeatherMan::FrameMove ( float fTime, float fElapsedTime )
{
	if ( !(m_dwFlag&ISACTIVE) )	return S_FALSE;

	m_fElapsedTime += fElapsedTime;

	BOOL	bRainUse = FALSE;
	BOOL	bSnowUse = FALSE;
	BOOL	bLeavesUse = FALSE;
	BOOL	bSpore = FALSE;

	DWORD						dwWeather	  = GLPeriod::GetInstance().GetWeather();

	// Add OneMap Weather
	std::vector<SONEMAPWEATHER> vecMapWeather = GLPeriod::GetInstance().GetOneMapWeather();

	DWORD i;
	for( i = 0; i < vecMapWeather.size(); i++ )
	{
		if( vecMapWeather[i].map_mID == m_currentMapID.wMainID &&
			vecMapWeather[i].map_sID == m_currentMapID.wSubID )
		{
			dwWeather = vecMapWeather[i].dwWeather;
		}
	}



	if ( (m_dwFlag&ISRAINDOING) || (dwWeather&FGW_RAIN) )		bRainUse = TRUE;
	if ( (m_dwFlag&ISSNOWDOING) || (dwWeather&FGW_SNOW) )		bSnowUse = TRUE;
	if ( (m_dwFlag&ISLEAVESDOING) || (dwWeather&FGW_LEAVES) )	bLeavesUse = TRUE;
	if ( (m_dwFlag&ISLEAVESDOING) || (dwWeather&FGW_SPORE) )	bSpore = TRUE;

	if ( m_fElapsedTime > 0.03f )
	{
		if ( m_dwFlag&ISRAIN )		m_sRain.FrameMove ( m_fElapsedTime, m_vWindDirec, m_fWindPower, bRainUse );
		if ( m_dwFlag&ISSNOW )		m_sSnow.FrameMove ( m_fElapsedTime, m_vWindDirec, m_fWindPower, bSnowUse );
		if ( m_dwFlag&ISLEAVES )	m_sLeaves.FrameMove ( m_fElapsedTime, m_vWindDirec, m_fWindPower, bLeavesUse );
		m_fElapsedTime = 0.f;
	}

	return S_OK;
}

HRESULT DxWeatherMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !(m_dwFlag&ISACTIVE) )	return S_OK;

	if ( m_dwFlag&ISRAIN )		m_sRain.Render ( pd3dDevice );

	return S_OK;
}

HRESULT DxWeatherMan::Render_Prev ( LPDIRECT3DDEVICEQ pd3dDevice )
{

	return S_OK;
}

//	Note : 날씨의 속성을 저장.
//		이 저장 포맷의 초기 형태. ( SIZE+VERSION+PROP를 GLLandMan에서도 활용하므로 주의하여야 한다. )
//
void DxWeatherMan::SaveSet ( CSerialFile &SFile )
{
	DWORD	dwSize = sizeof(WEATHER_PROPERTY);
	SFile << (DWORD) VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Property, sizeof(WEATHER_PROPERTY) );

	SFile << (BOOL)TRUE;
	m_sRain.SaveSet ( SFile );

	SFile << (BOOL)TRUE;
	m_sSnow.SaveSet ( SFile );

	SFile << (BOOL)TRUE;
	m_sCloud.SaveSet ( SFile );

	SFile << (BOOL)FALSE;
}

DWORD DxWeatherMan::LoadSetProp ( CSerialFile &SFile, WEATHER_PROPERTY &sProperty )
{
	DWORD	dwVer;
	DWORD	dwBuffSize;

	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( dwVer==VERSION && dwBuffSize==sizeof(WEATHER_PROPERTY) )
	{
		SFile.ReadBuffer ( &sProperty, sizeof(WEATHER_PROPERTY) );
	}
	else if ( dwVer==0x101 && dwBuffSize==sizeof(WEATHER_PROPERTY_101) )
	{
		WEATHER_PROPERTY_101	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(WEATHER_PROPERTY_101) );

		sProperty.m_vWindDirec = sProp.m_vWindDirec;
		sProperty.m_fWindPower = 2.f;

		sProperty.m_dwFlag = 0L;
		if ( sProp.m_bActive )	
		{
			sProperty.m_dwFlag |= ISACTIVE;
			sProperty.m_dwFlag |= ISRAIN;
		}
		if ( sProp.m_bUse )
		{
			sProperty.m_dwFlag |= ISRAINDOING;
		}
	}
	//GASSERT(dwBuffSize==sizeof(WEATHER_PROPERTY)&&dwVer==VERSION);

	return dwVer;
}

void DxWeatherMan::LoadSet ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD	dwVer;
	BOOL	bUse;

	dwVer = LoadSetProp ( SFile, m_Property );

	if ( dwVer==VERSION )
	{
		while ( true )
		{
			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sRain.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sSnow.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sCloud.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;

			break;
		};
	}
	else if ( dwVer==0x101 )
	{
		while ( true )
		{
			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sRain.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sSnow.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;
			m_sCloud.LoadSet ( SFile );

			SFile >> (BOOL)bUse;
			if ( !bUse )	break;

			break;
		};
	}

	InvalidateDeviceObjects ( pd3dDevice );
	DeleteDeviceObjects ();
	InitDeviceObjects ( pd3dDevice );
	RestoreDeviceObjects ( pd3dDevice );
}

void DxWeatherMan::CloneInstance ( DxWeatherMan* pWeatherMan, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//InvalidateDeviceObjects ( pd3dDevice );
	//DeleteDeviceObjects ();
	//InitDeviceObjects ( pd3dDevice );
	//RestoreDeviceObjects ( pd3dDevice );

	pWeatherMan->m_Property = m_Property;

	pWeatherMan->GetRain()->SetProperty ( m_sRain.GetProperty() );
	pWeatherMan->GetSnow()->SetProperty ( m_sSnow.GetProperty() );
	pWeatherMan->GetCloud()->SetProperty ( m_sCloud.GetProperty() );

	//pWeatherMan->InvalidateDeviceObjects ( pd3dDevice );
	//pWeatherMan->DeleteDeviceObjects ();
	//pWeatherMan->InitDeviceObjects ( pd3dDevice );
	//pWeatherMan->RestoreDeviceObjects ( pd3dDevice );
}

