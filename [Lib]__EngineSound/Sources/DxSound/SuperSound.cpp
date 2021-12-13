#include "pch.h"
#include "SuperSound.h"
#include "DxSoundMan.h"
#include "SerialFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const WORD	SSound::m_SoundVER = 101;

#ifdef	_DEBUG_SOUND
SSound::SOUNDMAP SSound::mapSound;

void	SSound::PRINT_TO_VIEW ()
{
	for ( int z = 0; z < 40; z++ )
	{
		CDebugSet::ToView ( 1, z, "" );
	}

	SOUNDMAP_CITER citer = mapSound.begin ();
	SOUNDMAP_CITER citer_end = mapSound.end ();

	int nAtOnce = 0;
	for ( int i = 0; citer != citer_end; ++citer, ++i )
	{
		CDebugSet::ToView ( 1, i, "%s - %d",
			((*citer).first).c_str(), (*citer).second );
		nAtOnce += (*citer).second;
	}

	static int nMAX = 0;
	static int nMIN = 0;
	static int nAVG = 0;
	nMIN = min ( nAtOnce, nMIN );
	nMAX = max ( nAtOnce, nMAX );
	nAVG = (int)(( nAVG + nAtOnce ) / 2);

	CDebugSet::ToView ( 1, i, "Total : %d at Once : %d", mapSound.size (), nAtOnce );
	CDebugSet::ToView ( 1, i + 1, "MIN : %d MAX %d AVG %d", nMIN, nMAX, nAVG );
}
#endif	//	_DEBUG_SOUND	

////////////////////////////////////////////////////////////////////////////////
//	<--	SSound 구조체 관련 메쏘드들
SSound::SSound() :
	m_BufferID ( UINT_MAX ),
	m_bLoop ( FALSE ),
	m_pSound ( NULL ),
	m_bStereo ( false )
{
}

SSound::SSound(const SSound& rValue )
{
	UnLoad ();
	Load ( rValue.m_strFileName, (rValue.m_bLoop)?true:false, rValue.m_bStereo );
}

SSound& SSound::operator= ( const SSound& rValue )
{
	if ( this == &rValue ) return *this;

	UnLoad ();
	Load ( rValue.m_strFileName, (rValue.m_bLoop)?true:false, rValue.m_bStereo );

	return *this;
}

SSound::~SSound()
{
	UnLoad ();
}

bool SSound::IsLoad ()
{	
	if ( m_pSound && m_strFileName.size() ) return true;
	else			return false;
}

bool SSound::Load ( std::string strFileName, bool bLoop, bool bStereo )
{
	if ( !strFileName.size() ) return false;

	//	만약, 디바이스가 스테레오로 잡혀있다면,
	//	모든 음원을 스테레오로 로드한다.
	if ( DxSoundMan::GetInstance().IsStereo () ) bStereo = true;

	UnLoad ();

	m_strFileName = strFileName;
	m_bLoop = (bLoop)?TRUE:FALSE;
	m_bStereo = bStereo;

	m_pSound = DxSoundMan::GetInstance().LoadWav ( m_strFileName, m_bStereo );

	return true;
}

bool SSound::UnLoad ()
{
	if ( m_pSound )
	{
		//	FindWav호출해서 존재하면 Stop, Release한다.
		//	일단 파일 이름부터 맞추자
		if ( DxSoundMan::GetInstance().IsLoadedWav ( m_strFileName, m_bStereo ) )
		{
			if ( !Stop () ) return false;
			DxSoundMan::GetInstance().ReleaseWav ( m_strFileName, m_bStereo );			
		}
	}

	m_bLoop = FALSE;
	m_strFileName.clear ();

	return true;
}

bool SSound::Play ()
{
	if ( !m_pSound ) return false;
	if ( IsPlaying () ) return true;
	if ( !Stop () ) return false;

	HRESULT hr = S_OK;
	DWORD dwFlag = (m_bLoop)?DSBPLAY_LOOPING:0;
	hr = m_pSound->PlayBuffer ( m_BufferID, 0, dwFlag );
	if ( FAILED ( hr ) ) return false;

#ifdef	_DEBUG_SOUND
	SOUNDMAP_ITER found = mapSound.find ( m_strFileName );
	if ( mapSound.end() != found )
	{
		int& rCount = (*found).second;
		rCount++;
	}
	else
	{
		mapSound.insert ( std::make_pair(m_strFileName,1) );
	}
#endif	//	_DEBUG_SOUND

	return true;
}

bool SSound::Stop ()
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return true;	
	
	HRESULT hr = S_OK;	
	hr = m_pSound->StopBuffer ( m_BufferID );
	if ( FAILED ( hr ) ) return false;

	hr = m_pSound->ResetBuffer ( m_BufferID );
	if ( FAILED ( hr ) ) return false;

	m_BufferID = UINT_MAX;

#ifdef	_DEBUG_SOUND
	SOUNDMAP_ITER found = mapSound.find ( m_strFileName );
	if ( mapSound.end() != found )
	{
		int& rCount = (*found).second;
		rCount--;
		if ( !rCount ) mapSound.erase ( found );
	}
#endif	//	_DEBUG_SOUND

	return true;
}

bool SSound::IsPlaying ()
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	
	BOOL bPlaying = m_pSound->IsSoundPlayingBuffer ( m_BufferID );
	return (bPlaying)?true:false;
}

bool  SSound::IsLoop ()
{
	return (m_bLoop)?true:false;
}

bool SSound::SetVolume ( const long lVolume )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUNDBUFFER pDSBuffer = m_pSound->GetBuffer ( m_BufferID );	
	if ( !pDSBuffer ) return false;

	long lPrevVolume;
	hr = pDSBuffer->GetVolume ( &lPrevVolume );
	if ( FAILED ( hr ) ) return false;

	if ( lVolume == lPrevVolume ) return true;

	hr = pDSBuffer->SetVolume ( lVolume );
	if ( FAILED ( hr ) ) return false;

	return true;
}

bool SSound::SetPan ( const long lPan )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	if ( !m_bStereo ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUNDBUFFER pDSBuffer = m_pSound->GetBuffer ( m_BufferID );
	if ( !pDSBuffer ) return false;

	hr = pDSBuffer->SetPan ( lPan );
	if ( FAILED ( hr ) ) return false;

	return true;
}

bool SSound::SetPosition ( const D3DXVECTOR3& vPos, const DWORD dwApply )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	if ( m_bStereo ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUND3DBUFFER pDS3DBuffer = NULL;
	hr = m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	if ( FAILED ( hr ) ) return false;

	hr = pDS3DBuffer->SetPosition ( vPos.x, vPos.y, vPos.z, dwApply );
	if ( FAILED ( hr ) ) return false;

	return true;
}

bool SSound::GetPosition ( D3DXVECTOR3* pvPos )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	if ( m_bStereo ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUND3DBUFFER pDS3DBuffer = NULL;
	hr = m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	if ( FAILED ( hr ) ) return false;

	hr = pDS3DBuffer->GetPosition ( pvPos );
	if ( FAILED ( hr ) ) return false;

	return true;
}

bool SSound::SetMinDistance ( const float fDist, const DWORD dwApply )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	if ( m_bStereo ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUND3DBUFFER pDS3DBuffer = NULL;
	hr = m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	if ( FAILED ( hr ) ) return false;

	hr = pDS3DBuffer->SetMinDistance ( fDist, dwApply );
	if ( FAILED ( hr ) ) return false;

	return true;
}

bool SSound::SetMaxDistance ( const float fDist, const DWORD dwApply )
{
	if ( !m_pSound ) return false;
	if ( UINT_MAX == m_BufferID ) return false;
	if ( m_bStereo ) return false;

	HRESULT hr = S_OK;

	LPDIRECTSOUND3DBUFFER pDS3DBuffer = NULL;
	hr = m_pSound->Get3DBufferInterface ( m_BufferID, &pDS3DBuffer );
	if ( FAILED ( hr ) ) return false;

	hr = pDS3DBuffer->SetMaxDistance ( fDist, dwApply );
	if ( FAILED ( hr ) ) return false;

	return true;
}


BOOL	SSound::SaveSet ( CSerialFile &SFile )
{
	int nFileName = (int)m_strFileName.size();	
	SFile.WriteBuffer ( &nFileName, sizeof ( int ) );
	if ( nFileName ) SFile.WriteBuffer ( m_strFileName.c_str(), sizeof ( char ) * nFileName ); 

	SFile << m_bLoop;

	return TRUE;
}

BOOL	SSound::LoadSet ( basestream &SFile, WORD SoundVer )
{
	if ( SoundVer == 100 ) return LoadSet100 ( SFile, SoundVer );

	int nFileName = 0;
	char *pszFileName = NULL;
	BOOL bLoop;

	SFile.ReadBuffer ( &nFileName, sizeof ( int ) );

	bool bFILE_EXIST = (nFileName)?true:false;
	if ( bFILE_EXIST )
	{
		pszFileName = new char[nFileName+1];
		memset ( pszFileName, 0, sizeof ( char ) * (nFileName+1) );
		SFile.ReadBuffer ( pszFileName, sizeof ( char ) * nFileName );
	}

	SFile >> bLoop;

	if ( bFILE_EXIST )
	{
		Load ( pszFileName, (bLoop)?true:false );
		SAFE_DELETE_ARRAY( pszFileName );
	}

	return TRUE;
}

BOOL	SSound::LoadSet100 ( basestream &SFile, WORD SoundVer )
{
	char szFileName[256] = "";
	WORD wBufferCount;
	BOOL bLoop;

	SFile.ReadBuffer ( szFileName, sizeof ( char ) * 256 );
	SFile >> wBufferCount;
	SFile >> bLoop;

	Load ( szFileName, (bLoop)?true:false );

	return TRUE;
}
//	-->	SSound 구조체 관련 메쏘드들
////////////////////////////////////////////////////////////////////////////////
