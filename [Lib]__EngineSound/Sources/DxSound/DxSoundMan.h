////////////////////////////////////////////////////////////////////////////////
//	이름	:	DxSoundMan
//	목적	:	모든 효과 Wav파일을 관리한다.
//	설명	:	웨이브 파일의 목록을 관리한다.
//	작성자	:	성기엽
//
//	로그
//	$0012.	2003-02-17T19:45	[2D 사운드 버퍼 만들수 있도록 수정, 인터페이스 사용]
//								[따라서, 로그 11번은 의미 없다.]
//	$0011.	2003-02-17T17:00	[외부/내부 인터페이스 소리, 방향성가지는 것 수정..
//								Update부분 손봄]
//	$0010.	2002-12-05T15:35	[캐릭터 + 이펙트 볼륨, 맵 볼륨, BG 각각 조절 가능한
//								인터페이스 제작]
//								바꾼지 보름만의 일이다. 쩝.]
//	$0009.	2002-12-03T16:58	[Orientation 도 고려한 사운드, 어색해서 죽였다. ㅡ,.ㅡ;;
//								바꾼지 보름만의 일이다. 쩝.]
//	$0008.	2002-11-14T15:09	[리스너는 x, y, z축 모두에 대해서 반응하도록 바꿈,
//								예전에는, y축에 대해서 반응하지 않았으나, '김주임'님의
//								새로운 던전을 보는 순간, 높이값이 절실히!! 필요함을 느끼고,
//								구조 개선함]
//	$0007.	2002-11-14T10:47	[사운드 Root 경로를 가지게 됨, 그 이하 서브 경로는
//								적절히 설정하면, 구분하는것도 수월하게 될 것 같음,
//								주임님의 생각 반영. 헐... 난 왜 이 방법이 안 좋다고 생각했나?]
//								[파일이름 및 디렉토리 경로 모두 '포인터 -> 배열'로 바꿈]
//	$0006.	2002-11-13T09:31	[SaveSet/LoadSet 작업]
//	$0005.	2002-11-07T09:31	[프린트 후, Final 수정작업 -1,
//								클래스간 관계확인 작업이 주 목적임]				
//	$0004.	2002-11-01T11:34	[구조 변환중...
//								enginelib - ReplaceContainer.h 참고]
//	$0003.	2002-10-30T14:47	[SWavFile 구조체의 RefCount 보류]
//								[DxSoundMan 구조를
//								'전원 켜기, 끄기',
//								'레코드 얻기, 내리기',
//								'노래 켜기, 끄기'
//								로 구분해서	메쏘드 제공]
//	$0002.	2002-10-28T09:51	[상세구현 시작]
//	$0001.	2002-10-18T11:23	[최초 클래스 구현]
////////////////////////////////////////////////////////////////////////////////
#ifndef	__C_WAVFILEMAN__
#define	__C_WAVFILEMAN__

#include <string>
#include <map>

//	<--	클래스 선언	-->	//
class	CSound;
class	CSoundManager;
class	CSerialFile;

struct	SWavFileKey
{
	std::string strFileName;
	bool		bStereo;

	SWavFileKey () :
		bStereo ( false )
	{
	}

	inline bool operator < (const SWavFileKey& b )
	{		
		return (strFileName < (b.strFileName));
	}

	inline bool operator > (const SWavFileKey& b )
	{
		return (strFileName > (b.strFileName));
	}

	inline bool operator == (const SWavFileKey& b )
	{
		return ((strFileName == b.strFileName) && (bStereo == b.bStereo));
	}
};

inline bool operator < ( const SWavFileKey& a, const SWavFileKey& b )
{
	return (a.strFileName < b.strFileName);
}

inline bool operator > ( const SWavFileKey& a, const SWavFileKey& b )
{
	return (a.strFileName > b.strFileName);
}

inline bool operator == ( const SWavFileKey& a, const SWavFileKey& b )
{
	return ((a.strFileName == b.strFileName) && (a.bStereo == b.bStereo));
}

struct	SWavFileData
{
	CSound*	pSound;
	DWORD	dwRefCount;

	SWavFileData () :
		pSound ( NULL ),
		dwRefCount ( 0 )
	{
	}

	~SWavFileData()
	{
		if( pSound )
		{
			pSound->Stop ();
			SAFE_DELETE ( pSound );
		}
		dwRefCount = 0;
	}

	CSound*	ADD_REF ()
	{
		if ( !pSound ) return NULL;

		++dwRefCount;
		return pSound;
	}

	DWORD	DEL_REF ()
	{		
		--dwRefCount;
		return dwRefCount;
	}
};

//	<--	클래스 정의	-->	//
class	DxSoundMan
{
private:
static	const	FLOAT	fDONT_CARE_DIST_SQ;
static	const	FLOAT	fMAX_DIST_SQ;
static	const	DWORD	dwMAX_BUFFER_SIZE;

public:
static	const	FLOAT	GET_DONT_CARE_DIST_SQ();
static	const	FLOAT	GET_MAX_DIST_SQ();

public:
	typedef	std::map<SWavFileKey,SWavFileData*>	WAVFILE_MAP;
	typedef	WAVFILE_MAP::const_iterator		WAVFILE_MAP_CITER;
	typedef	WAVFILE_MAP::iterator			WAVFILE_MAP_ITER;
	typedef	WAVFILE_MAP::size_type			WAVFILE_MAP_SIZE;

	WAVFILE_MAP	m_mapWavFile;

public:
	//	3D Algorithm
	enum
	{
		DEFAULT,
		HRTF_LIGHT,
		HRTF_FULL,

		ALGORITHM_SIZE = 3
	};	

	GUID	guid3DAlgorithm[ALGORITHM_SIZE];

private:
	static	WORD	m_SoundManVER;

private:
	CSoundManager*			m_pSoundManager;	//	Directsound 객체	
	LPDIRECTSOUND3DLISTENER m_pDSListener;		//	리스너 하나 빼 놓음

	CString m_strPath;		//	사운드들의 루트 디렉토리

private:
	int		m_nBufferSize;

private:
	float			m_fRolloffFactor;
	float			m_fDistanceFactor;
	float			m_fDopplerFactor;
	D3DXVECTOR3		m_VelocityFactor;

private:
	BOOL	m_bMapMute;
	BOOL	m_bSfxMute;	

	long	m_lMapVolume;	
	long	m_lSfxVolume;

private:
	int		m_3DAlgorithm;

private:
	BOOL	m_bStereo;

private:
	DxSoundMan ();
	virtual	~DxSoundMan();

	//	<--	오디오 켜기 끄기[장치, 디바이스]	-->	//
public:
	BOOL	OneTimeSceneInit	( HWND hWnd, char *szDirectory );
	BOOL	FinalCleanup		();

public:
	void	SetPath( char *szDirectory );
	const char*	GetPath();

public:
	void	SetStereo ( BOOL bStereo );
	BOOL	IsStereo ();

public:
	void	SetBufferSize ( const int nBufferSize );
	int		GetBufferSize ();

public:
	const long	CALC_PAN ( const D3DXVECTOR3& vSoundPos );
	const float	CALC_FADE( const D3DXVECTOR3& vSoundPos, const float& fMAX_RANGE );

	//	<--	Listener 음색(?) 조절 메쏘드	-->	//
public:
	void		SetRolloffFactor	( float fRolloffFactor );
	void		SetDistanceFactor	( float	fDistanceFactor );
	void		SetDopplerFactor	( float fDopplerFactor );
	void		SetVelocityFactor	( D3DXVECTOR3 VelocityFactor );

	float		GetRolloffFactor	();
	float		GetDistanceFactor	();
	float		GetDopplerFactor	();
	D3DXVECTOR3	GetVelocityFactor	();

public:
	HRESULT	GetListernerPos ( D3DXVECTOR3* pPos );

	//	<--	볼륨 조절	-->	//
	//	<**	볼륨의 크기는 다른 사용자 프로필 파일에
	//		저장해야 한다. 이것은 프로그램 데이타와는
	//		완전히 독립되어 있는 놈이기 때문에,
	//		그렇게 하는 것이 좋겠다.
	//	**>
public:
	void	SetMapVolume ( long	lVolume );
	long	GetMapVolume ();

    void	SetSfxVolume ( long lVolume );
	long	GetSfxVolume ();

public:
	void	SetMapMute ( BOOL bMute );
	void	SetSfxMute ( BOOL bMute );

	BOOL	IsMapMute ();
	BOOL	IsSfxMute ();

public:
	void		Set3DAlgorithm ( int nAlgorithm );
	INT			Get3DAlgorithm ();
	GUID		Getguid3DAlgorithm ();

public:	
	void		FrameMove ( D3DXVECTOR3 OrientFront, D3DXVECTOR3 Position );

	//	<--	웨이브파일 리소스 관리 메쏘드	-->	//
public:
	CSound*		LoadWav ( std::string strFileName, bool bStereo );
	HRESULT		ReleaseWav ( std::string strFileName, bool bStereo );
	HRESULT		DeleateWav ( std::string strFileName, bool bStereo );
	bool		IsLoadedWav ( std::string strFileName, bool bStereo );

	void		DeleteAllWav ();

public:
	BOOL		SaveSet ( CSerialFile &SFile );
	BOOL		LoadSet ( CSerialFile &SFile );

public:
	CSoundManager* const GetSoundManager ()
	{
		return m_pSoundManager;
	}

	//	<--	Singleton	-->	//
public:
	static DxSoundMan& GetInstance();
};

inline void DxSoundMan::Set3DAlgorithm ( int nAlgorithm )
{
	GASSERT ( 0 <= nAlgorithm && nAlgorithm < ALGORITHM_SIZE );
	m_3DAlgorithm = nAlgorithm;
}

inline int DxSoundMan::Get3DAlgorithm ()
{
	return m_3DAlgorithm;
}

inline GUID DxSoundMan::Getguid3DAlgorithm ()
{
	return guid3DAlgorithm[m_3DAlgorithm];
}

#endif	//	__C_WAVFILEMAN__