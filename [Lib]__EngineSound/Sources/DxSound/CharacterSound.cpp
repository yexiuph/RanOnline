#include "pch.h"
#include "CharacterSound.h"
#include "DxSoundMan.h"
#include "SerialFile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////
//	<--	SChaSoundData ����ü ���� �޽���
SChaSoundData::SChaSoundData()
{
	m_PlayFrameCount = 0;
	memset ( m_szFileName, 0, sizeof ( char ) * (MAXPLAYFRAME - 1) * 32 );
	memset ( m_bLoop, 0, sizeof ( BOOL ) * (MAXPLAYFRAME - 1) );
	memset ( m_PlayFrame, 0xFF, sizeof ( WORD ) * ( MAXPLAYFRAME ) );
}

SChaSoundData::~SChaSoundData()
{	
}

SChaSoundData&	SChaSoundData::operator= ( const SChaSoundData_102& rValue )
{
	//	�ʱ�ȭ
	{
		m_PlayFrameCount = 0;
		memset ( m_szFileName, 0, sizeof ( char ) * (MAXPLAYFRAME - 1) * 32 );
		memset ( m_bLoop, 0, sizeof ( BOOL ) * (MAXPLAYFRAME - 1) );
		memset ( m_PlayFrame, 0xFF, sizeof ( WORD ) * ( MAXPLAYFRAME ) );
	}

	if ( MAXPLAYFRAME < rValue.m_PlayFrameCount )
	{
		GASSERT ( 0 && "���۸� �ʰ��Ͽ�, �Ϻκ� �߷������ϴ�." );
	}

	int nMAX_PLAYFRAMECOUNT = min (rValue.m_PlayFrameCount,MAXPLAYFRAME);

	m_PlayFrameCount = nMAX_PLAYFRAMECOUNT;
	for ( int i = 0; i < m_PlayFrameCount; ++i )
	{
		StringCchCopy( m_szFileName[i], STRING_NUM_32, rValue.m_szFileName[i] );
		m_bLoop[i] = rValue.m_bLoop[i];
		m_PlayFrame[i] = rValue.m_PlayFrame[i];
	}

	return *this;
}

SChaSoundData&	SChaSoundData::operator= ( const SChaSoundData_103& rValue )
{
	//	�ʱ�ȭ
	{
		m_PlayFrameCount = 0;
		memset ( m_szFileName, 0, sizeof ( char ) * (MAXPLAYFRAME - 1) * 32 );
		memset ( m_bLoop, 0, sizeof ( BOOL ) * (MAXPLAYFRAME - 1) );
		memset ( m_PlayFrame, 0xFF, sizeof ( WORD ) * ( MAXPLAYFRAME ) );
	}

	if ( MAXPLAYFRAME < rValue.m_PlayFrameCount )
	{
		GASSERT ( 0 && "���۸� �ʰ��Ͽ�, �Ϻκ� �߷������ϴ�." );
	}

	int nMAX_PLAYFRAMECOUNT = min (rValue.m_PlayFrameCount,MAXPLAYFRAME);

	m_PlayFrameCount = nMAX_PLAYFRAMECOUNT;
	for ( int i = 0; i < m_PlayFrameCount; ++i )
	{
		StringCchCopy( m_szFileName[i], STRING_NUM_32, rValue.m_szFileName[i] );
		m_bLoop[i] = rValue.m_bLoop[i];
		m_PlayFrame[i] = rValue.m_PlayFrame[i];
	}

	return *this;
}
//	-->	SChaSoundData ����ü ���� �޽���
////////////////////////////////////////////////////////////////////////////////



#define		CHARSOUNDSCOPE 25.f
const float SChaSound::fMINRANGE = 8.0f;
const float SChaSound::fMAXRANGE = 512.0f;
////////////////////////////////////////////////////////////////////////////////
//	<--	SChaSound ����ü ���� �޽���
SChaSound::SChaSound()
{
	m_PlayFrameCount = 0;
	memset ( m_PlayType, 0, sizeof ( WORD ) * ( MAXPLAYFRAME - 1 ) );	//	eGENERIC�� ����	
	memset ( m_PlayFrame, 0, sizeof ( WORD ) * ( MAXPLAYFRAME ) );

	m_PlayingFrame = 0;					//	�����ؾ��� ���� ������ ������
}

SChaSound::~SChaSound()
{	
}

BOOL SChaSound::PlayReset ( DWORD dwSTime )
{
	//	<--	���� ������ ã��	-->	//
	for ( int i = 0; i < m_PlayFrameCount; i++ )
	{
		//	���� �������� �׻� '0'�� �ƴϴ�.
		if ( dwSTime <= m_PlayFrame[i] )
		{			
			m_PlayingFrame = i;
			return TRUE;
		}
	}

	return FALSE;
}

void SChaSound::PlayChaSound ( float fCurTime, D3DXVECTOR3 vPos )
{
	if ( !m_Sound[0].IsLoad () ) return ;

	//	<--	���� ��� ����	-->	//
	if ( m_PlayFrame[m_PlayingFrame] <= fCurTime &&
		m_PlayingFrame < m_PlayFrameCount )
	{
		PlayChaSoundOnce ( m_Sound[m_PlayingFrame], vPos );
		m_PlayingFrame++;

		//	���� �����ӿ� ���õ� �Ҹ� ���
		while ( m_PlayFrame[m_PlayingFrame] == m_PlayFrame[m_PlayingFrame - 1] &&
			m_PlayingFrame < m_PlayFrameCount )
		{				
			PlayChaSoundOnce ( m_Sound[m_PlayingFrame], vPos );
			m_PlayingFrame++;
		}		
	}

	PlayChaSoundPosition ( vPos );
}

void	SChaSound::PlayChaSoundOnce ( SSound& rSoundObj, D3DXVECTOR3& vPos )
{
	if ( rSoundObj.IsLoad () )
	{
		if ( DxSoundMan::GetInstance().IsSfxMute () )
		{
			rSoundObj.Stop ();
			return ;
		}

		D3DXVECTOR3	vListenerPos; 
		DxSoundMan::GetInstance().GetListernerPos ( &vListenerPos );
		D3DXVECTOR3 vDist = vPos - vListenerPos;
		FLOAT fDistSq = D3DXVec3LengthSq ( &vDist );
		if ( DxSoundMan::GetInstance().GET_MAX_DIST_SQ () < fDistSq ) return ;

		rSoundObj.Play ();

		if ( DxSoundMan::GetInstance().IsStereo () )
		{
			const long& lPan = DxSoundMan::GetInstance().CALC_PAN ( vPos );
			rSoundObj.SetPan ( lPan );

			const float fFadeRatio = DxSoundMan::GetInstance().CALC_FADE ( vPos, CHARSOUNDSCOPE );

			const long lMaxVolume = -DSBVOLUME_MIN;			
			const float fPostiveVolume = float(DxSoundMan::GetInstance().GetSfxVolume() + lMaxVolume);//	���� ��������
			const float fFadeVolume = fFadeRatio * fPostiveVolume;//	���̵� ����			
			const long lVolume = long( fFadeVolume ) - lMaxVolume;//	lMaxVolume �ø���ŭ ������� ����

			rSoundObj.SetVolume ( lVolume );
		}
		else
		{
			rSoundObj.SetMinDistance ( fMINRANGE, DS3D_DEFERRED ); 
			rSoundObj.SetMaxDistance ( fMAXRANGE, DS3D_DEFERRED );

			CtrlDS3DBuffer ( rSoundObj, vPos );

			rSoundObj.SetVolume ( DxSoundMan::GetInstance().GetSfxVolume () );
		}
	}
}

void	SChaSound::PlayChaSoundPosition ( D3DXVECTOR3& vPos )
{
	//	<--	������ �̵�	-->	//
	for ( int nIndex = 0; nIndex < m_PlayFrameCount; nIndex++ )
	{
		SSound& rSoundObj = m_Sound[nIndex];

		if ( DxSoundMan::GetInstance().IsSfxMute () )
		{
			rSoundObj.Stop ();
			return ;
		}


		if ( !rSoundObj.IsLoad () ) continue ;

		if ( rSoundObj.IsPlaying () )
		{			
			if ( DxSoundMan::GetInstance().IsStereo () )
			{
				const long& lPan = DxSoundMan::GetInstance().CALC_PAN ( vPos );
				rSoundObj.SetPan ( lPan );

				const float fFadeRatio = DxSoundMan::GetInstance().CALC_FADE ( vPos, CHARSOUNDSCOPE );

				const long lMaxVolume = -DSBVOLUME_MIN;			
				const float fPostiveVolume = float(DxSoundMan::GetInstance().GetSfxVolume() + lMaxVolume);//	���� ��������
				const float fFadeVolume = fFadeRatio * fPostiveVolume;//	���̵� ����			
				const long lVolume = long( fFadeVolume ) - lMaxVolume;//	lMaxVolume �ø���ŭ ������� ����

				rSoundObj.SetVolume ( lVolume );
			}
			else
			{
				CtrlDS3DBuffer ( rSoundObj, vPos );			
				rSoundObj.SetVolume ( DxSoundMan::GetInstance().GetSfxVolume () );
			}
		}
		else
		{
			rSoundObj.Stop ();
		}
	}
}

void	SChaSound::CtrlDS3DBuffer ( SSound& rSoundObj, D3DXVECTOR3& vPos )
{	
	//	NOTE
	//		3 Unit �̻� �������� �Ҹ��� ������Ʈ �Ѵ�.		
	D3DXVECTOR3 vOldPos;
	rSoundObj.GetPosition ( &vOldPos );

	D3DXVECTOR3 vDist = vPos - vOldPos;
	FLOAT fDistSq = D3DXVec3LengthSq ( &vDist );

	if ( fDistSq < DxSoundMan::GetInstance().GET_DONT_CARE_DIST_SQ () ) return ;

	rSoundObj.SetPosition ( vPos, DS3D_DEFERRED );
}

SChaSound&	SChaSound::operator= ( const SChaSoundData& rValue )
{
	m_PlayFrameCount = rValue.m_PlayFrameCount;
	int i = 0;
	for ( i = 0; i < rValue.m_PlayFrameCount; ++i )
	{
		m_PlayFrame[i] = rValue.m_PlayFrame[i];
		m_Sound[i].Load ( rValue.m_szFileName[i], (rValue.m_bLoop[i])?true:false );
	}
	for ( i = rValue.m_PlayFrameCount; i < MAXPLAYFRAME; i++ )
	{
		m_PlayFrame[i] = rValue.m_PlayFrame[i];
	}

	return *this;
}