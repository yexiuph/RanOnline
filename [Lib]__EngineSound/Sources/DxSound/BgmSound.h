#ifndef	__DX_BGM_SOUND__
#define	__DX_BGM_SOUND__

#include "dsutil.h"
#include "dxutil.h"

#include "ogg/ogg.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

class DxBgmSound
{		
private:
	//	OGG 파일 정보 구조체
	//
	struct SOggFileInfo
	{
		int		m_iFileLength_Secs;
		int		m_iBitRate_Kbs;
		int		m_iFreq_Hz;
		BOOL	m_bStereo;
		BOOL	m_b16bit;
	};

	//	OGG 파일 구조체
	//
	struct SOggFile
	{		
		FILE*			m_hFile;
		OggVorbis_File	m_vf;
		int				m_CurrentSection;
		SOggFileInfo	m_FileInfo;		

		SOggFile () :	
			m_hFile ( NULL )
		{			
		}
	};
	
	//	재생 관련 디바이스 & 정보... 분석 더 필요.
	//
	struct SBgmDeviceContext
	{
		LPDIRECTSOUNDBUFFER	m_lpDSB;
		LPDIRECTSOUND8		m_lpDirectSound;
		WAVEFORMATEX		m_WaveFile;
		DWORD				m_WriteCursor;
		BOOL				m_TermState_Wrapped;
		DWORD				m_TermState_WriteCursor;
		DWORD				m_TermState_HighestPlayPos;
		DWORD				m_TimerId;
		BOOL				m_bStreamRunning;

		SBgmDeviceContext () :
			m_lpDSB ( NULL ),
			m_lpDirectSound ( NULL )
		{
		}
	};

	//	스레드 핸들링 관련
	//
	struct	SThreadCtrl
	{
		DWORD	m_ThreadID;			//	스레드 ID
		HANDLE	m_hThread;			//	스레드 핸들
		HANDLE	m_hEventTerminate;	//	스레드 종료

		SThreadCtrl () :
			m_ThreadID ( UINT_MAX ),
			m_hThread ( NULL ),
			m_hEventTerminate ( NULL )
		{
		}
	};	

private:
	DxBgmSound ();
	~DxBgmSound();

private:
	BOOL	LoadOggFile	 ();
	BOOL	UnLoadOggFile();

	BOOL	LoadSoundBuffer  ();
	BOOL	UnLoadSoundBuffer();

	HRESULT	CreateDirectSound ();
	HRESULT	CreateDirectSoundBuffer ();
	HRESULT	InitDirectSoundBuffer ();

	HRESULT	ReleaseDirectSoundBuffer();
	HRESULT	ReleaseDirectSound ();

public:
	BOOL	Run ( HWND hWnd );

	BOOL	SetPath ( CString strPath );
	const char * GetPath ( void );

	BOOL	SetFile ( CString strFileName );
	BOOL	SetFile( const TCHAR* pszFileName );
	const char * GetFile ( void );
	void	ClearFile ( void );

	BOOL	Play ();
	BOOL	Stop ();
	BOOL	ForceStop ();
	BOOL	IsPlaying ();

	LONG	GetVolume ();
	void	SetVolume ( LONG Volume );
	HRESULT	CtrlVolume ();

private:
	BOOL	PlayInternal ();
	BOOL	StopInternal ();
//
//public:
//	BOOL	SaveSet ( CSerialFile& SFile );
//	BOOL	LoadSet ( CSerialFile& SFile );

private:
	void	SetPlayFlag ( BOOL bPlay );

private:
	BOOL	SetHWND ( HWND hWnd );

	BOOL	StartThread();
	BOOL	StopThread ();

public:
	void	SetMute ( BOOL bMute );
	BOOL	IsMute ();

private:
	BOOL	Playing ();	
	BOOL	GetPCMBlock ( void* pBlock, DWORD* pdwBlockSize );
	void	EnablePlay ( const BOOL bEnable );	
	BOOL	IsOutputComplete();
	void	SetPause(const BOOL bPause);
	void	RefillBuffers();
	void	GetPlayPosAndInvalidLength(DWORD* pdwPlayPos, DWORD* pdwLength);

	static	unsigned int WINAPI PlaySound( LPVOID pData );

private:
	//	상수
	const	DWORD			OUTPUTBLOCKSIZE;
	const	DWORD			MAXFILLAMOUNT;
	const	DWORD			INVALIDCURSORPOS;

private:	
	SBgmDeviceContext	m_BgmDC;		//	디바이스 컨텍스트
	HANDLE				m_evtBlockFree;	//	싱크 이벤트 핸들
	SOggFile			m_OggFile;		//	재생 OGG 파일
	HWND				m_hWnd;			//	윈도우 핸들
	LONG				m_Volume;		//	볼륨
	BOOL				m_bMute;		//	MUTE 플래그

private:
	CString				m_strPath;		//	파일 경로
	CString				m_strFileName;	//	파일 이름

private:
	CRITICAL_SECTION	m_csLock;
	BOOL				m_bPlaying;		//	재생중?
	BOOL				m_bForceRefill;	//	버퍼 강제 채우기
	BOOL				m_bMoreData;	//	노래의 끝 체크

private:
	SThreadCtrl	m_ThreadCtrl;

private:
	BOOL				m_bActivated;	// 비활성 --> 활성화 유무
	WORD				m_wCount;

public:
	void				SetActivate ( BOOL bActivity );
	BOOL				IsActivated ()				   { return  m_bActivated; }


public:
	static DxBgmSound& GetInstance();
};

#endif	//	__DX_BGM_SOUND__