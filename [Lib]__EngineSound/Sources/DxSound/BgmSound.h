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
	//	OGG ���� ���� ����ü
	//
	struct SOggFileInfo
	{
		int		m_iFileLength_Secs;
		int		m_iBitRate_Kbs;
		int		m_iFreq_Hz;
		BOOL	m_bStereo;
		BOOL	m_b16bit;
	};

	//	OGG ���� ����ü
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
	
	//	��� ���� ����̽� & ����... �м� �� �ʿ�.
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

	//	������ �ڵ鸵 ����
	//
	struct	SThreadCtrl
	{
		DWORD	m_ThreadID;			//	������ ID
		HANDLE	m_hThread;			//	������ �ڵ�
		HANDLE	m_hEventTerminate;	//	������ ����

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
	//	���
	const	DWORD			OUTPUTBLOCKSIZE;
	const	DWORD			MAXFILLAMOUNT;
	const	DWORD			INVALIDCURSORPOS;

private:	
	SBgmDeviceContext	m_BgmDC;		//	����̽� ���ؽ�Ʈ
	HANDLE				m_evtBlockFree;	//	��ũ �̺�Ʈ �ڵ�
	SOggFile			m_OggFile;		//	��� OGG ����
	HWND				m_hWnd;			//	������ �ڵ�
	LONG				m_Volume;		//	����
	BOOL				m_bMute;		//	MUTE �÷���

private:
	CString				m_strPath;		//	���� ���
	CString				m_strFileName;	//	���� �̸�

private:
	CRITICAL_SECTION	m_csLock;
	BOOL				m_bPlaying;		//	�����?
	BOOL				m_bForceRefill;	//	���� ���� ä���
	BOOL				m_bMoreData;	//	�뷡�� �� üũ

private:
	SThreadCtrl	m_ThreadCtrl;

private:
	BOOL				m_bActivated;	// ��Ȱ�� --> Ȱ��ȭ ����
	WORD				m_wCount;

public:
	void				SetActivate ( BOOL bActivity );
	BOOL				IsActivated ()				   { return  m_bActivated; }


public:
	static DxBgmSound& GetInstance();
};

#endif	//	__DX_BGM_SOUND__