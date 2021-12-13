#include "pch.h"
#include "DxSoundLib.h"
#include "DxSoundMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxSoundLib::DxSoundLib ()
{
}

DxSoundLib::~DxSoundLib ()
{
	ReleaseAllSound ();
}

bool	DxSoundLib::CreateSound ( std::string strKeyword, std::string strFileName, int nVolType, bool bLoop )
{
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	if ( citer_end != m_mapSoundLib.find ( strKeyword ) ) return true;

	//	사운드 생성
	SSoundEx* pNewSound = new SSoundEx;
	pNewSound->m_emVolType = (ET_SOUNDTYPE)nVolType;	//	볼륨 타입
	if ( !pNewSound->Load ( strFileName, bLoop, true ) )
	{
		SAFE_DELETE ( pNewSound );
		CDebugSet::ToLogFile(	"[DxSoundLib::CreateSound] KeyWord:%s, FileName:%s Load Fail.", 
								strKeyword.c_str(), 
								strFileName.c_str() );

		return false;
	}

	m_mapSoundLib.insert ( std::make_pair ( strKeyword, pNewSound ) );

	return true;
}

bool	DxSoundLib::ReleaseSound( std::string strKeyword )
{
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	SOUNDLIB_MAP_ITER found = m_mapSoundLib.find ( strKeyword );
	if ( citer_end == found ) return false;

	//	사운드 해제	
	SSoundEx* pReleaseSound = (*found).second;	
	SAFE_DELETE ( pReleaseSound );

	m_mapSoundLib.erase ( found );
	return true;
}

bool	DxSoundLib::PlaySound ( std::string strKeyword )
{
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	SOUNDLIB_MAP_CITER found = m_mapSoundLib.find ( strKeyword );
	if ( citer_end == found ) return false;

	const long lMapVolume = DxSoundMan::GetInstance().GetMapVolume ();
	const long lSfxVolume = DxSoundMan::GetInstance().GetSfxVolume ();
	const BOOL bMapMute = DxSoundMan::GetInstance().IsMapMute ();
	const BOOL bSfxMute = DxSoundMan::GetInstance().IsSfxMute ();

	SSoundEx* pPlaySound = (*found).second;

	//	MUTE 처리
	switch ( pPlaySound->m_emVolType )
	{
	case MAP_SOUND:	
		{
			if ( bMapMute ) return true;
		}
		break;
	case SFX_SOUND:
		{
			if ( bSfxMute ) return true;
		}
		break;
	}

	if ( !pPlaySound->Play () ) return false;

	//	볼륨 조절
	switch ( pPlaySound->m_emVolType )
	{
	case MAP_SOUND:
		{
			if ( !pPlaySound->SetVolume ( lMapVolume ) ) return false;
		}
		break;
	case SFX_SOUND:
		{
			if ( !pPlaySound->SetVolume ( lSfxVolume ) ) return false;
		}
		break;
	}

	return true;
}

bool	DxSoundLib::StopSound ( std::string strKeyword )
{
	if ( m_mapSoundLib.empty() )	return false;

	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	SOUNDLIB_MAP_CITER found = m_mapSoundLib.find ( strKeyword );
	if ( citer_end == found ) return false;

	SSoundEx* pStopSound = (*found).second;
	return pStopSound->Stop ();
}

bool	DxSoundLib::IsPlaying ( std::string strKeyword )
{
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	SOUNDLIB_MAP_CITER found = m_mapSoundLib.find ( strKeyword );
	if ( citer_end == found ) return false;

	SSoundEx* pPlayingSound = (*found).second;
	return pPlayingSound->IsPlaying ();
}

bool	DxSoundLib::ReleaseAllSound ()
{
	SOUNDLIB_MAP_CITER citer = m_mapSoundLib.begin ();
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();

	for ( ; citer != citer_end; ++citer )
	{
		SSoundEx* pReleaseSound = (*citer).second;
		SAFE_DELETE ( pReleaseSound );
	}

	m_mapSoundLib.clear ();

	return true;
}

DxSoundLib*	DxSoundLib::GetInstance ()
{
	static	DxSoundLib sInstance;
	return &sInstance;
}

void	DxSoundLib::ReleaseInstance ()
{
}

bool	DxSoundLib::Update ()
{
	const long lMapVolume = DxSoundMan::GetInstance().GetMapVolume ();
	const long lSfxVolume = DxSoundMan::GetInstance().GetSfxVolume ();
	const BOOL bMapMute = DxSoundMan::GetInstance().IsMapMute ();
	const BOOL bSfxMute = DxSoundMan::GetInstance().IsSfxMute ();

	//	기타 여러가지 상황을 업데이트 함
	SOUNDLIB_MAP_CITER citer = m_mapSoundLib.begin ();
	SOUNDLIB_MAP_CITER citer_end = m_mapSoundLib.end ();
	for ( ; citer != citer_end; ++citer )
	{
		SSoundEx* pUpdateSound = (*citer).second;

		if ( !pUpdateSound->IsPlaying () ) continue;

		switch ( pUpdateSound->m_emVolType )
		{
		case MAP_SOUND:
			{
				if ( bMapMute && !pUpdateSound->Stop () ) return false;
				if ( !pUpdateSound->SetVolume ( lMapVolume ) ) return false;				
			}
			break;
		case SFX_SOUND:
			{
				if ( bSfxMute && !pUpdateSound->Stop () ) return false;
				if ( !pUpdateSound->SetVolume ( lSfxVolume ) ) return false;
			}
			break;
		}
	}

	return true;
}