#include "pch.h"
#include "MovableSound.h"
#include "SerialFile.h"
#include "DxSoundMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

WORD	SMovSound::m_MovSoundVER = 100;


////////////////////////////////////////////////////////////////////////////////
//	<--	SMovSound ����ü ���� �޽���
SMovSound::SMovSound()
{		
	m_MinRange = 0.0f;
	m_MaxRange = 0.0f;
	m_fGAgeBACK = 0.0f;
	m_bFORCE_STOP = false;
	m_bSTARTED = false;
}

SMovSound::~SMovSound()
{	
}

SMovSound::SMovSound(const SMovSound& rValue) : SSound ( rValue )
{
	*this = rValue;
}

SMovSound&	SMovSound::operator= ( const SMovSound& rValue )
{
	if ( this != &rValue )
	{
		m_MinRange = rValue.m_MinRange;
		m_MaxRange = rValue.m_MaxRange;

		SSound::operator = ( rValue );
	}
	return *this;
}

bool SMovSound::Play ()
{
	if ( SSound::IsPlaying () ) return true;

	bool bResult = true;
	bResult = SSound::Play ();
	if ( !bResult ) return false;

	if ( DxSoundMan::GetInstance().IsStereo () )
	{
	}
	else
	{
		bResult = SSound::SetMinDistance ( m_MinRange, DS3D_DEFERRED );
		if ( !bResult ) return false;

		bResult = SSound::SetMaxDistance ( m_MaxRange, DS3D_DEFERRED );
		if ( !bResult ) return false;
	}

	return true;
}

void SMovSound::Update ( const D3DXVECTOR3& vGNowPos,
	const float& fGBeginTime, const float& fGAge, const float& fGLifeTime )
{
	if ( IsLoad () )
	{
		bool bRESTART = (fGAge<m_fGAgeBACK);
		m_fGAgeBACK = fGAge;

		if ( bRESTART )
		{
			m_bFORCE_STOP = false;
			m_bSTARTED = false;
		}

		//	NOTE
		//		���带 MUTE ��Ŵ
		if ( DxSoundMan::GetInstance().IsSfxMute () )
		{
			Stop ();
			m_bFORCE_STOP = true;
			return ;
		}

		//	NOTE
		//		�Ÿ��� 100 Unit �̻��ΰ͵鿡 ���ؼ���,
		//		�Ҹ��� ������� �ʴ´�.				
		D3DXVECTOR3	vListenerPos;
		DxSoundMan::GetInstance().GetListernerPos ( &vListenerPos );
		D3DXVECTOR3 vDist = vGNowPos - vListenerPos;
		FLOAT fDistSq = D3DXVec3LengthSq ( &vDist );
		if ( DxSoundMan::GetInstance().GET_MAX_DIST_SQ () < fDistSq )
		{
			Stop ();
			m_bFORCE_STOP = true;
			return ;
		}

		//	�Ҹ� ����
		bool bSOUND_STOPED = m_bSTARTED&&!IsPlaying ();
		bool bEFFECT_STOPED = fGAge>=(fGBeginTime+fGLifeTime);
		if ( bEFFECT_STOPED || bSOUND_STOPED )
		{
			Stop ();
			m_bFORCE_STOP = true;

			return ;
		}
		
		//	�Ҹ� ������Ʈ
		if ( fGBeginTime <= fGAge && !m_bFORCE_STOP )
		{
			Play ();
			if ( DxSoundMan::GetInstance().IsStereo () )
			{
				const long lPan = DxSoundMan::GetInstance().CALC_PAN ( vGNowPos );
				SetPan ( lPan );

				const float fFadeRatio = DxSoundMan::GetInstance().CALC_FADE ( vGNowPos, m_MinRange );

				const long lMaxVolume = -DSBVOLUME_MIN;			
				const float fPostiveVolume = float(DxSoundMan::GetInstance().GetSfxVolume() + lMaxVolume);//	���� ��������
				const float fFadeVolume = fFadeRatio * fPostiveVolume;//	���̵� ����			
				const long lVolume = long( fFadeVolume ) - lMaxVolume;//	lMaxVolume �ø���ŭ ������� ����

				SetVolume ( lVolume );
			}
			else
			{
				SetPosition ( vGNowPos, DS3D_DEFERRED );
				SetVolume ( DxSoundMan::GetInstance().GetSfxVolume () );
			}
			
			m_bSTARTED = true;
		}
	}
}

BOOL	SMovSound::SaveSet ( CSerialFile &SFile )
{
	SFile << m_MinRange;
	SFile << m_MaxRange;

	SSound::SaveSet ( SFile );

	return TRUE;
}

BOOL	SMovSound::LoadSet ( basestream &SFile, WORD MovSoundVer, WORD SoundVer )
{
	SFile >> m_MinRange;
	SFile >> m_MaxRange;

	SSound::LoadSet ( SFile, SoundVer );	

	return TRUE;
}
//	-->	SMovSound ����ü ���� �޽���
////////////////////////////////////////////////////////////////////////////////
