#include "pch.h"

#include "DxLightMan.h"
#include "dxviewport.h"
#include "DxShadowMap.h"
#include "./TextureManager.h"
#include "./SerialFile.h"

#include "./GLPeriod.h"
#include "dxweatherman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const	float	DxEffectSnow::SNOW_TURN_TIME	= 30.f;	// 날씨가 변하는 시간
const	DWORD	DxEffectSnow::VERSION	=	0x102;

// Note	: 눈 효과
//
DxEffectSnow::DxEffectSnow() :
	m_fTime(0.0f),
	m_fApplyRate(0.f),
	m_emSnowStats(EMWS_NULL)
{
}

DxEffectSnow::~DxEffectSnow()
{
}

HRESULT DxEffectSnow::FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse )
{
	m_fTime += fElapsedTime;

	//	Note : 상태 제어
	//
	if ( (m_emSnowStats==EMWS_NULL) && bUse )									// 아무것도 아닌 상태에서 뿌리라는 명령을 내렸을 때
	{
//		if ( !m_SoundRain.IsPlaying() )		m_SoundRain.Play ();

		m_fTime = 0.f;
		m_emSnowStats = EMWS_STARTING;
	}
	else if ( (m_emSnowStats==EMWS_STARTING) && (m_fTime>=SNOW_TURN_TIME) )		// 눈가 내리기 시작하는 중에, 준비 시간이 끝
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_PLAYING;
	}
	else if ( (m_emSnowStats==EMWS_PLAYING) && !bUse )							// 눈을 내리는 중에 그만 내리라는 통보를 받음
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_ENDING;
	}
	else if ( (m_emSnowStats==EMWS_ENDING) && (m_fTime>=SNOW_TURN_TIME) )		// 눈니 내리다가 완전히 그쳤을 경우
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_NULL;
	}
	else if ( m_emSnowStats==EMWS_NULL )										// 눈을 안뿌린다.
	{
//		StopSound ();
		return S_OK;
	}


	float	fApplyRate = 0.f;
	if ( m_emSnowStats==EMWS_STARTING )			m_fApplyRate = m_fTime/SNOW_TURN_TIME;
	else if ( m_emSnowStats==EMWS_PLAYING )		m_fApplyRate = 1.f;
	else if ( m_emSnowStats==EMWS_ENDING )		m_fApplyRate = 1.f-(m_fTime/SNOW_TURN_TIME);
	else										m_fApplyRate = 0.f;

	return S_OK;
}

void	DxEffectSnow::SaveSet ( CSerialFile &SFile )
{
	DWORD	dwSize = sizeof(SNOW_PROPERTY);
	SFile << (DWORD) VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Property, sizeof(SNOW_PROPERTY) );
}

void	DxEffectSnow::LoadSet ( CSerialFile &SFile )
{
	DWORD	dwBuffSize;
	DWORD	dwVer;

	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( &m_Property, sizeof(SNOW_PROPERTY) );
	}
	else if ( dwVer == 0x101 )
	{
		SNOW_PROPERTY_101	pProp;
		SFile.ReadBuffer ( &pProp, sizeof(SNOW_PROPERTY_101) );
	}
	else
	{
		dwBuffSize -= sizeof(DWORD);
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
}







const	float	DxEffectLeaves::LEAVES_TURN_TIME	= 30.f;	// 날씨가 변하는 시간
const	DWORD	DxEffectLeaves::VERSION	=	0x100;

// Note	: 눈 효과
//
DxEffectLeaves::DxEffectLeaves() :
	m_fTime(0.0f),
	m_fApplyRate(0.f),
	m_emLeavesStats(EMWS_NULL)
{
}

DxEffectLeaves::~DxEffectLeaves()
{
}

HRESULT DxEffectLeaves::FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse )
{
	m_fTime += fElapsedTime;

	//	Note : 상태 제어
	//
	if ( (m_emLeavesStats==EMWS_NULL) && bUse )										// 아무것도 아닌 상태에서 뿌리라는 명령을 내렸을 때
	{
//		if ( !m_SoundRain.IsPlaying() )		m_SoundRain.Play ();

		m_fTime = 0.f;
		m_emLeavesStats = EMWS_STARTING;
	}
	else if ( (m_emLeavesStats==EMWS_STARTING) && (m_fTime>=LEAVES_TURN_TIME) )		// 눈가 내리기 시작하는 중에, 준비 시간이 끝
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_PLAYING;
	}
	else if ( (m_emLeavesStats==EMWS_PLAYING) && !bUse )							// 눈을 내리는 중에 그만 내리라는 통보를 받음
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_ENDING;
	}
	else if ( (m_emLeavesStats==EMWS_ENDING) && (m_fTime>=LEAVES_TURN_TIME) )		// 눈니 내리다가 완전히 그쳤을 경우
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_NULL;
	}
	else if ( m_emLeavesStats==EMWS_NULL )											// 눈을 안뿌린다.
	{
//		StopSound ();
		return S_OK;
	}


	float	fApplyRate = 0.f;
	if ( m_emLeavesStats==EMWS_STARTING )			m_fApplyRate = m_fTime/LEAVES_TURN_TIME;
	else if ( m_emLeavesStats==EMWS_PLAYING )		m_fApplyRate = 1.f;
	else if ( m_emLeavesStats==EMWS_ENDING )		m_fApplyRate = 1.f-(m_fTime/LEAVES_TURN_TIME);
	else											m_fApplyRate = 0.f;

	return S_OK;
}