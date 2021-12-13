#include "pch.h"

#include "DxSoundMan.h"
#include "dxutil.h"
#include "SerialFile.h"
#include "DxViewPort.h"

//#include "DxInputDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxSoundMan& DxSoundMan::GetInstance()
{
	static DxSoundMan Instance;
	return Instance;
}

WORD	DxSoundMan::m_SoundManVER = 100;
const	FLOAT	DxSoundMan::fDONT_CARE_DIST_SQ = 9.0f;
const	FLOAT	DxSoundMan::fMAX_DIST_SQ = 10000.0f;
const	DWORD	DxSoundMan::dwMAX_BUFFER_SIZE = 10;



const	FLOAT	DxSoundMan::GET_DONT_CARE_DIST_SQ()
{	
	return fDONT_CARE_DIST_SQ;
}

const	FLOAT	DxSoundMan::GET_MAX_DIST_SQ()
{	
	return fMAX_DIST_SQ;
}

////////////////////////////////////////////////////////////////////////////////
//	<--	DxSoundMan 구조체 관련 메쏘드들
DxSoundMan::DxSoundMan() :
	m_pDSListener ( NULL ),
	m_pSoundManager ( NULL ),
	m_lSfxVolume ( 0 ),
	m_lMapVolume ( 0 ),
	m_bMapMute ( FALSE ),
	m_bSfxMute ( FALSE ),
	m_3DAlgorithm ( DEFAULT ),
	m_bStereo ( TRUE ),
	m_nBufferSize ( dwMAX_BUFFER_SIZE )
{
	guid3DAlgorithm[DEFAULT] = DS3DALG_DEFAULT;
	guid3DAlgorithm[HRTF_LIGHT] = DS3DALG_HRTF_LIGHT;
	guid3DAlgorithm[HRTF_FULL] = DS3DALG_HRTF_FULL;
}

DxSoundMan::~DxSoundMan()
{	
	m_pDSListener = NULL;
	m_pSoundManager = NULL;
	m_lSfxVolume = 0;
	m_lMapVolume = 0;

	DeleteAllWav ();
}

BOOL	DxSoundMan::OneTimeSceneInit	( HWND hWnd, char *szDirectory )
{
	if ( m_pSoundManager || !hWnd )
	{
		return FALSE;
	}

	m_pSoundManager = new CSoundManager();
    
	if ( FAILED ( m_pSoundManager->Initialize( hWnd, DSSCL_PRIORITY, 2, 22050, 16 ) ) )
	{
		SAFE_DELETE ( m_pSoundManager );
//		GASSERT ( 0 && "DirectSound 초기화에 실패했습니다." );
		return FALSE;	
	}

	if ( !m_bStereo )
	{
		if ( FAILED ( m_pSoundManager->Get3DListenerInterface ( &m_pDSListener ) ) )
		{
			return FALSE;
		}

		//	<--	초기 장치 값 읽어두기	-->	//
		m_pDSListener->GetRolloffFactor ( &m_fRolloffFactor );
		m_pDSListener->GetDistanceFactor ( &m_fDistanceFactor );
		m_pDSListener->GetDopplerFactor ( &m_fDopplerFactor );
		m_pDSListener->GetVelocity ( &m_VelocityFactor );
	}

	//	<--	디렉토리 설정	-->	//
	SetPath ( szDirectory );

	return TRUE;
}

BOOL	DxSoundMan::FinalCleanup	()
{
	DeleteAllWav ();

	m_pDSListener = NULL;
	SAFE_DELETE ( m_pSoundManager );	

	return TRUE;
}

void DxSoundMan::DeleteAllWav ()
{
	if ( !m_mapWavFile.size () ) return ;

	WAVFILE_MAP_CITER citer = m_mapWavFile.begin ();
	WAVFILE_MAP_CITER citer_end = m_mapWavFile.end ();

	for ( ; citer != citer_end; ++citer )
	{
		const SWavFileKey& rWavFileKey = (*citer).first;
		SWavFileData* pWavFileData = (*citer).second;

		SAFE_DELETE ( pWavFileData );
	}

	m_mapWavFile.clear ();
}

CSound*		DxSoundMan::LoadWav ( std::string strFileName, bool bStereo )
{
	if ( !m_pSoundManager ) return NULL;
	if ( !strFileName.size() ) return NULL;		

	SWavFileKey WavFileKey;
	WavFileKey.bStereo = bStereo;
	WavFileKey.strFileName = strFileName;

	//	이미 로드한 음원이 존재하는 경우
	WAVFILE_MAP_CITER found = m_mapWavFile.find ( WavFileKey );
	if ( found != m_mapWavFile.end () )
	{
		SWavFileData* pWavFileData = (*found).second;
		return pWavFileData->ADD_REF ();
	}
	
	SWavFileData* pWavFileData = new SWavFileData;

	//	실제 WAV 파일 로드하기
	{
		CString strPathName = m_strPath + strFileName.c_str();
		DWORD	dwCreationFlags =
			(bStereo) ?
			(DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN) :
			(DSBCAPS_MUTE3DATMAXDISTANCE | DSBCAPS_LOCDEFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME);

		GUID guid3DAlgorithm = (bStereo) ? DS3DALG_DEFAULT : Getguid3DAlgorithm();

		HRESULT hr = m_pSoundManager->Create ( &(pWavFileData->pSound), strPathName.GetBuffer(),
			dwCreationFlags, guid3DAlgorithm, m_nBufferSize );

		if ( FAILED ( hr ) )
		{
			SAFE_DELETE( pWavFileData );
			return NULL;
		}
	}	

	m_mapWavFile.insert ( std::make_pair ( WavFileKey, pWavFileData ) );

	return pWavFileData->ADD_REF ();
}

HRESULT		DxSoundMan::ReleaseWav ( std::string strFileName, bool bStereo )
{
	if ( !strFileName.size() ) return S_FALSE;

	SWavFileKey WavFileKey;
	WavFileKey.bStereo = bStereo;
	WavFileKey.strFileName = strFileName;

	WAVFILE_MAP_ITER found = m_mapWavFile.find ( WavFileKey );
	if ( found != m_mapWavFile.end () )
	{
		SWavFileData* pWavFileData = (*found).second;
		DWORD dwRefCount = pWavFileData->DEL_REF ();
		if ( !dwRefCount )
		{
			SAFE_DELETE ( pWavFileData );
			m_mapWavFile.erase ( found );

			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT 	DxSoundMan::DeleateWav ( std::string strFileName, bool bStereo )
{
	if ( !strFileName.size() ) return S_FALSE;

	SWavFileKey WavFileKey;
	WavFileKey.bStereo = bStereo;
	WavFileKey.strFileName = strFileName;

	WAVFILE_MAP_ITER found = m_mapWavFile.find ( WavFileKey );
	if ( found != m_mapWavFile.end () )
	{
		SWavFileData* pWavFileData = (*found).second;
		SAFE_DELETE ( pWavFileData );
		m_mapWavFile.erase ( found );

		return S_OK;
	}

	return S_FALSE;
}

bool DxSoundMan::IsLoadedWav ( std::string strFileName, bool bStereo )
{
	if ( !strFileName.size() ) return false;
	if ( !m_mapWavFile.size()) return false;

	SWavFileKey WavFileKey;
	WavFileKey.bStereo = bStereo;
	WavFileKey.strFileName = strFileName;

	//	이미 로드한 음원이 존재하는 경우
	WAVFILE_MAP_CITER found = m_mapWavFile.find ( WavFileKey );
	if ( found == m_mapWavFile.end () )
		return false;
	else
		return true;
}

void DxSoundMan::SetRolloffFactor	( float fRolloffFactor )
{
	if ( m_bStereo ) return ;
	m_fRolloffFactor = fRolloffFactor;
	m_pDSListener->SetRolloffFactor ( m_fRolloffFactor, DS3D_DEFERRED );		
}

void DxSoundMan::SetDistanceFactor	( float	fDistanceFactor )
{
	if ( m_bStereo ) return ;
	m_fDistanceFactor = fDistanceFactor;
	m_pDSListener->SetDistanceFactor ( m_fDistanceFactor, DS3D_DEFERRED );	
}

void		DxSoundMan::SetDopplerFactor	( float fDopplerFactor )
{
	if ( m_bStereo ) return ;

	m_fDopplerFactor = fDopplerFactor;
	m_pDSListener->SetDopplerFactor ( m_fDopplerFactor, DS3D_DEFERRED );	
}

void		DxSoundMan::SetVelocityFactor	( D3DXVECTOR3 VelocityFactor )
{
	if ( m_bStereo ) return ;

	m_VelocityFactor = VelocityFactor;
	m_pDSListener->SetVelocity (
			m_VelocityFactor.x, m_VelocityFactor.y,	m_VelocityFactor.z,
			DS3D_DEFERRED );
}

float		DxSoundMan::GetRolloffFactor	()
{
	return m_fRolloffFactor;
}

float		DxSoundMan::GetDistanceFactor	()
{
	return m_fDistanceFactor;
}

float		DxSoundMan::GetDopplerFactor	()
{
	return m_fDopplerFactor;
}

D3DXVECTOR3	DxSoundMan::GetVelocityFactor	()
{
	return m_VelocityFactor;
}

void		DxSoundMan::FrameMove ( D3DXVECTOR3 OrientFront, D3DXVECTOR3 Position )
{	
	//	NOTE
	//		맵과 효과음 모두가 꺼진 경우에는 업데이트가 필요없다.
	if ( IsMapMute () && IsSfxMute () )	return ;

	//	NOTE
	//		스테레오 작업은 여기까지한다.
	if ( m_bStereo ) return ;


	if ( m_pDSListener )
	{	
		//	<--	vFront값은 무시한다.
		//		그저 메쏘드 호출상 쓰일뿐이다.
		//	-->	//
		static D3DXVECTOR3 vFront, vUp;
		m_pDSListener->GetOrientation ( &vFront, &vUp );
		
		D3DXVECTOR3 vListenerPos;
		m_pDSListener->GetPosition ( &vListenerPos );

		D3DXVECTOR3 vDist = vListenerPos - Position;
		FLOAT fDistSq = D3DXVec3LengthSq ( &vDist );

		const float fCos10 = 0.984f;
		D3DXVec3Normalize ( &OrientFront, &OrientFront );
		D3DXVec3Normalize ( &vFront, &vFront );
		float fDiff = D3DXVec3Dot ( &OrientFront, &vFront );

		if ( GET_DONT_CARE_DIST_SQ() < fDistSq )
		{
			m_pDSListener->SetPosition (
				Position.x, Position.y, Position.z,
				DS3D_DEFERRED );
		}

		if ( fDiff < fCos10 )
		{
			m_pDSListener->SetOrientation (
				OrientFront.x, vFront.y, OrientFront.z,
				vUp.x, vUp.y, vUp.z,
				DS3D_DEFERRED );
		}

		m_pDSListener->CommitDeferredSettings();
	}
}

HRESULT	DxSoundMan::GetListernerPos ( D3DXVECTOR3* pPos )
{
	if ( m_bStereo )
	{
		const D3DXVECTOR3& vPos = DxViewPort::GetInstance().GetLookatPt ();
        if ( !pPos ) return E_FAIL;
		*pPos = vPos;
		return S_OK;
	}
	if ( m_pDSListener )
	{
		return m_pDSListener->GetPosition ( pPos );
	}
	return E_FAIL;
}

BOOL	DxSoundMan::SaveSet ( CSerialFile &SFile )
{
	SFile << m_SoundManVER;

	SFile << m_fRolloffFactor;
	SFile << m_fDistanceFactor;
	SFile << m_fDopplerFactor;
	SFile.WriteBuffer ( &m_VelocityFactor, sizeof ( D3DXVECTOR3 ) );

	return TRUE;
}

BOOL	DxSoundMan::LoadSet ( CSerialFile &SFile )
{	
	WORD	SoundManVER;
	SFile >> SoundManVER;

	SFile >> m_fRolloffFactor;
	SFile >> m_fDistanceFactor;
	SFile >> m_fDopplerFactor;
	SFile.ReadBuffer ( &m_VelocityFactor, sizeof ( D3DXVECTOR3 ) );

	return TRUE;
}

void	DxSoundMan::SetPath( char *szDirectory )
{
	m_strPath = szDirectory;	
}

const char*	DxSoundMan::GetPath()
{
	return m_strPath.GetString();
}

long	DxSoundMan::GetMapVolume()
{		
	return m_lMapVolume;
}

void	DxSoundMan::SetMapVolume( long	lVolume )
{
	m_lMapVolume = lVolume;
}

void DxSoundMan::SetMapMute ( BOOL bMute )
{
	m_bMapMute = bMute;
}

void DxSoundMan::SetSfxMute ( BOOL bMute )
{
	m_bSfxMute = bMute;
}

BOOL DxSoundMan::IsMapMute ()
{
	return (m_bMapMute || m_lMapVolume == DSBVOLUME_MIN);
}

BOOL DxSoundMan::IsSfxMute ()
{
	return (m_bSfxMute || m_lSfxVolume == DSBVOLUME_MIN);
}

void DxSoundMan::SetSfxVolume ( long lVolume )
{
	m_lSfxVolume = lVolume;
}

long DxSoundMan::GetSfxVolume ()
{
	return m_lSfxVolume;
}

void DxSoundMan::SetStereo ( BOOL bStereo )
{
	m_bStereo = bStereo;
}

BOOL DxSoundMan::IsStereo ()
{
	return m_bStereo;
}

void DxSoundMan::SetBufferSize ( const int nBufferSize )
{	
	m_nBufferSize = min(nBufferSize,dwMAX_BUFFER_SIZE);
}

int	DxSoundMan::GetBufferSize ()
{
	return m_nBufferSize;
}

const long	DxSoundMan::CALC_PAN ( const D3DXVECTOR3& vSoundPos )
{
	const D3DXVECTOR3& vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();

	float		fLength;
	D3DXPLANE	sPlane;
	D3DXVECTOR3 vUp = vLookatPt;
	vUp.y += 1.f;

	D3DXPlaneFromPoints ( &sPlane, &vUp, &vLookatPt, &vFromPt );

	fLength = sPlane.a*vSoundPos.x + sPlane.b*vSoundPos.y + sPlane.c*vSoundPos.z + sPlane.d;

	//	Note : 팬 리턴 ( -10000 ~ 10000 )
	//
	const float fFACTOR = 80.0f;
	long lPan = (long)(fLength * fFACTOR);
	return lPan;
}

const float	DxSoundMan::CALC_FADE( const D3DXVECTOR3& vSoundPos, const float& fMAX_RANGE )
{
	float fFadeRatio = 1.0f;

	const float fFACTOR = 0.2f;

	const D3DXVECTOR3& vListenerPos = DxViewPort::GetInstance().GetLookatPt();
	const D3DXVECTOR3 vDistance = vSoundPos - vListenerPos;
	const float fDistance = D3DXVec3Length ( &vDistance );

	const float fVOL_FULL_RANGE = fMAX_RANGE;// * 0.2f;		
	if ( fDistance <= fVOL_FULL_RANGE )
	{
		fFadeRatio = 1.0f;
	}
	else
	{
		const float fVOL_FADE_DIST = fDistance - fVOL_FULL_RANGE;
		fFadeRatio = 1.0f - (fVOL_FADE_DIST/fVOL_FULL_RANGE*fFACTOR);
		fFadeRatio = max ( fFadeRatio, 0.4f );
	}

	return fFadeRatio;
}

//	-->	DxSoundMan 구조체 관련 메쏘드들
////////////////////////////////////////////////////////////////////////////////