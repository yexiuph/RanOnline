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


const	float	DxEffectSnow::SNOW_TURN_TIME	= 30.f;	// ������ ���ϴ� �ð�
const	DWORD	DxEffectSnow::VERSION	=	0x102;

// Note	: �� ȿ��
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

	//	Note : ���� ����
	//
	if ( (m_emSnowStats==EMWS_NULL) && bUse )									// �ƹ��͵� �ƴ� ���¿��� �Ѹ���� ����� ������ ��
	{
//		if ( !m_SoundRain.IsPlaying() )		m_SoundRain.Play ();

		m_fTime = 0.f;
		m_emSnowStats = EMWS_STARTING;
	}
	else if ( (m_emSnowStats==EMWS_STARTING) && (m_fTime>=SNOW_TURN_TIME) )		// ���� ������ �����ϴ� �߿�, �غ� �ð��� ��
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_PLAYING;
	}
	else if ( (m_emSnowStats==EMWS_PLAYING) && !bUse )							// ���� ������ �߿� �׸� ������� �뺸�� ����
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_ENDING;
	}
	else if ( (m_emSnowStats==EMWS_ENDING) && (m_fTime>=SNOW_TURN_TIME) )		// ���� �����ٰ� ������ ������ ���
	{
		m_fTime = 0.f;
		m_emSnowStats = EMWS_NULL;
	}
	else if ( m_emSnowStats==EMWS_NULL )										// ���� �ȻѸ���.
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







const	float	DxEffectLeaves::LEAVES_TURN_TIME	= 30.f;	// ������ ���ϴ� �ð�
const	DWORD	DxEffectLeaves::VERSION	=	0x100;

// Note	: �� ȿ��
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

	//	Note : ���� ����
	//
	if ( (m_emLeavesStats==EMWS_NULL) && bUse )										// �ƹ��͵� �ƴ� ���¿��� �Ѹ���� ����� ������ ��
	{
//		if ( !m_SoundRain.IsPlaying() )		m_SoundRain.Play ();

		m_fTime = 0.f;
		m_emLeavesStats = EMWS_STARTING;
	}
	else if ( (m_emLeavesStats==EMWS_STARTING) && (m_fTime>=LEAVES_TURN_TIME) )		// ���� ������ �����ϴ� �߿�, �غ� �ð��� ��
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_PLAYING;
	}
	else if ( (m_emLeavesStats==EMWS_PLAYING) && !bUse )							// ���� ������ �߿� �׸� ������� �뺸�� ����
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_ENDING;
	}
	else if ( (m_emLeavesStats==EMWS_ENDING) && (m_fTime>=LEAVES_TURN_TIME) )		// ���� �����ٰ� ������ ������ ���
	{
		m_fTime = 0.f;
		m_emLeavesStats = EMWS_NULL;
	}
	else if ( m_emLeavesStats==EMWS_NULL )											// ���� �ȻѸ���.
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