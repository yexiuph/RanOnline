#include "pch.h"
#include <process.h>
#include "BgmSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxBgmSound& DxBgmSound::GetInstance()
{
	static DxBgmSound Instance;
	return Instance;
}

DxBgmSound::DxBgmSound () :
	OUTPUTBLOCKSIZE ( 0x100000 ), // 512k block
	MAXFILLAMOUNT 	( OUTPUTBLOCKSIZE>>4 ), // 1/16th of total block size
	INVALIDCURSORPOS( 0xFFFFFFFF ),
	m_evtBlockFree	( NULL ),
	m_hWnd			( NULL ),
	m_bPlaying		( FALSE),			
	m_Volume		( 0 ),
	m_bForceRefill	( FALSE ),
	m_bMoreData		( FALSE ),
	m_bMute			( FALSE ),
	m_bActivated	( FALSE ),
	m_wCount		( 0 )
{
	InitializeCriticalSection(&m_csLock);
}

DxBgmSound::~DxBgmSound()
{	
	DeleteCriticalSection(&m_csLock);
//	StopThread ();

    EnablePlay ( FALSE );
	UnLoadSoundBuffer ();
	UnLoadOggFile ();
	ReleaseDirectSound ();
}

void DxBgmSound::SetMute ( BOOL bMute )
{
	EnterCriticalSection(&m_csLock);
	{
		m_bMute = bMute;
	}
	LeaveCriticalSection(&m_csLock);

	if ( m_bMute )	Stop ();
	else			Play ();
}

BOOL DxBgmSound::IsMute ()
{
	return m_bMute;
}

void DxBgmSound::SetPlayFlag ( BOOL bPlay )
{
	EnterCriticalSection(&m_csLock);
	{
		m_bPlaying = bPlay;
	}
	LeaveCriticalSection(&m_csLock);
}

BOOL DxBgmSound::Run ( HWND hWnd )
{
	if ( !SetHWND ( hWnd ) )
	{
		return FALSE;
	}

	if ( FAILED ( CreateDirectSound () ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL	DxBgmSound::SetHWND ( HWND hWnd )
{
	if ( !hWnd )
	{
		return FALSE;
	}

	m_hWnd = hWnd;
	return TRUE;
}

BOOL	DxBgmSound::SetPath ( CString strPath )
{
	m_strPath = strPath;

	return TRUE;
}

const char * DxBgmSound::GetPath ( void )
{
	return m_strPath.GetString();
}

BOOL	DxBgmSound::SetFile ( CString strFileName )
{	
	ClearFile ();
	
	m_strFileName = strFileName;

	return TRUE;
}

BOOL	DxBgmSound::SetFile( const TCHAR* pszFileName )
{
	ClearFile ();
	
	m_strFileName = pszFileName;

	return TRUE;
}

void	DxBgmSound::ClearFile ( void )
{
	m_strFileName.Empty ();
}

const char*	DxBgmSound::GetFile ( void )
{
	return m_strFileName.GetString ();
}

BOOL  DxBgmSound::PlayInternal ()
{
	EnterCriticalSection(&m_csLock);

	if ( m_bPlaying )
	{
		LeaveCriticalSection(&m_csLock);
		return FALSE;
	}

	if ( !LoadOggFile () )
	{
		LeaveCriticalSection(&m_csLock);
		
		UnLoadOggFile ();
		return FALSE;
	}
	if ( !LoadSoundBuffer () )
	{
		LeaveCriticalSection(&m_csLock);

		UnLoadSoundBuffer ();
		return FALSE;
	}
	EnablePlay ( TRUE );

	m_bForceRefill = TRUE;
	m_bMoreData = TRUE;

	LeaveCriticalSection(&m_csLock);

	SetPlayFlag ( TRUE );

	return TRUE;
}

BOOL  DxBgmSound::StopInternal ()
{
	EnterCriticalSection(&m_csLock);

	if ( !m_bPlaying )
	{
		LeaveCriticalSection(&m_csLock);
		return FALSE;
	}

	m_bForceRefill = FALSE;
	m_bMoreData = FALSE;	

	UnLoadSoundBuffer ();
	UnLoadOggFile ();

	LeaveCriticalSection(&m_csLock);

	SetPlayFlag ( FALSE );

	return TRUE;
}


BOOL DxBgmSound::Play ()
{
	EnterCriticalSection(&m_csLock);

	if ( m_bMute )
	{
		LeaveCriticalSection(&m_csLock);
		return FALSE;
	}

	if ( m_bPlaying )
	{
		LeaveCriticalSection(&m_csLock);
		return FALSE;
	}

	if ( !LoadOggFile () )
	{
		LeaveCriticalSection(&m_csLock);
		
		UnLoadOggFile ();
		return FALSE;
	}
	if ( !LoadSoundBuffer () )
	{
		LeaveCriticalSection(&m_csLock);

		UnLoadSoundBuffer ();
		return FALSE;
	}

	EnablePlay ( TRUE );

	m_bForceRefill = TRUE;
	m_bMoreData = TRUE;

	//	바로 적용한다.
	CtrlVolume ();

	StartThread ();

	LeaveCriticalSection(&m_csLock);

	SetPlayFlag ( TRUE );

	return TRUE;
}

BOOL DxBgmSound::Stop ()
{
	EnterCriticalSection(&m_csLock);

	if ( !m_bPlaying )
	{
		LeaveCriticalSection(&m_csLock);
		return FALSE;
	}

	StopThread ();

	//	바로 적용한다.
	CtrlVolume ();

	m_bForceRefill = FALSE;
	m_bMoreData = FALSE;	

	UnLoadSoundBuffer ();
	UnLoadOggFile ();

	LeaveCriticalSection(&m_csLock);

	SetPlayFlag ( FALSE );

	return TRUE;
}

BOOL DxBgmSound::StartThread()
{
	if ( !m_ThreadCtrl.m_hThread )
	{
		m_ThreadCtrl.m_hEventTerminate = CreateEvent ( NULL, TRUE, FALSE, NULL );	

		m_ThreadCtrl.m_hThread =
			(HANDLE) ::_beginthreadex(
							NULL,
							0,
							PlaySound,
							&m_ThreadCtrl,
							0,
							(unsigned int*) &m_ThreadCtrl.m_ThreadID );
			//CreateThread ( NULL, 0, PlaySound, &m_ThreadCtrl, 0, &m_ThreadCtrl.m_ThreadID );
		return TRUE;
	}
	return FALSE;	
}

BOOL DxBgmSound::StopThread ()
{
	if ( m_ThreadCtrl.m_hThread )
	{
		//	종료 신호
		SetEvent ( m_ThreadCtrl.m_hEventTerminate );

		//	종료 대기
		WaitForSingleObject ( m_ThreadCtrl.m_hThread, 10000 );	//	10초 대기

		if ( m_ThreadCtrl.m_hEventTerminate )	CloseHandle ( m_ThreadCtrl.m_hEventTerminate );
		if ( m_ThreadCtrl.m_hThread )			CloseHandle ( m_ThreadCtrl.m_hThread );

		m_ThreadCtrl.m_hEventTerminate = NULL;
		m_ThreadCtrl.m_hThread = NULL;
	}

	return TRUE;
}

BOOL DxBgmSound::Playing ()
{	
	if ( !m_bPlaying ) return TRUE;

	DWORD dwWaitResult;
	if ( m_bForceRefill == FALSE )
	{
		if(m_evtBlockFree)
		{
			dwWaitResult =
				MsgWaitForMultipleObjects ( 1, &m_evtBlockFree, FALSE, 1000, QS_POSTMESSAGE );
		}
		else
		{
			dwWaitResult = WAIT_OBJECT_0;
		}
	}

	// If the buffer event is signaled then request a refill
	if ( m_bForceRefill == TRUE || dwWaitResult == WAIT_OBJECT_0 )
	{
		if ( m_bMoreData != FALSE )
		{
			RefillBuffers();
			m_bForceRefill = FALSE;

			//if ( FAILED ( CtrlVolume () ) )
			//{
			//	return FALSE;
			//}
		}

		// If output has finished everything that it was doing - close the engine
		else if(IsOutputComplete() == TRUE)
		{
			DxBgmSound::GetInstance().StopInternal ();
			DxBgmSound::GetInstance().PlayInternal ();			
		}
	}

	if ( FAILED ( CtrlVolume () ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL DxBgmSound::GetPCMBlock ( void* pBlock, DWORD* pdwBlockSize )
{
	int ret;
	BOOL CONTINUE = TRUE;
	DWORD bytes_to_read = *pdwBlockSize;

	// Read PCM data from decoder. Little endian (0), 16-Bit (2), Signed (1)
	while ( bytes_to_read > 0 ) 
	{ 
		ret = ov_read ( &m_OggFile.m_vf,
			(char*)((unsigned char *)pBlock + (*pdwBlockSize - bytes_to_read)),
			bytes_to_read, 0, 2, 1, &m_OggFile.m_CurrentSection );
		if (ret == 0)
		{ 
			*pdwBlockSize = 0;
			CONTINUE = FALSE;
			break;
		}
		else if ( ret < 0 )
		{
//			CP_TRACE0("Hole in OGG/VORBIS datastream, ignoring..."); 
		}
		else
		{
			bytes_to_read -= ret; 
		}
	} 
	
	return CONTINUE;	//More data to come
}

void DxBgmSound::EnablePlay ( const BOOL bEnable )
{
	if( !( m_BgmDC.m_lpDirectSound && m_BgmDC.m_lpDSB ) )
		return;

	m_BgmDC.m_bStreamRunning = bEnable;

	// Setup event timer - according to our current requirements
	if(bEnable)
	{	
		m_BgmDC.m_lpDSB->Play ( 0, 0, DSBPLAY_LOOPING );

		// If there is a CoDec set up looping play mode
		if ( m_BgmDC.m_TimerId == 0 )
		{
			m_BgmDC.m_TimerId =	timeSetEvent(20, 10,	// 10ms Resolution
				(LPTIMECALLBACK)m_evtBlockFree, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
		}
	}
	else
	{
		HRESULT hr = m_BgmDC.m_lpDSB->Stop ();
		timeKillEvent ( m_BgmDC.m_TimerId );
		m_BgmDC.m_TimerId = 0;
	}
}

BOOL DxBgmSound::IsOutputComplete()
{
	DWORD dwCurrentPlayCursor, dwInvalidLength;

	if( !m_BgmDC.m_lpDirectSound )
	{
		return TRUE;
	}

	// Was there ever any data in this buffer
	if(m_BgmDC.m_WriteCursor == INVALIDCURSORPOS)
	{
		return TRUE;
	}

	// Work out write cursor at terminate time
	if(m_BgmDC.m_TermState_WriteCursor == INVALIDCURSORPOS)
	{
		m_BgmDC.m_TermState_WriteCursor = m_BgmDC.m_WriteCursor;
	}

	GetPlayPosAndInvalidLength(&dwCurrentPlayCursor, &dwInvalidLength);

	// Empty the invalid area - this is because we cannot be sure that some of the 
	// invalid area will no be played because our event doesn't com in on time
	if(dwInvalidLength > 0)
	{
		BYTE *pbData = NULL;
		BYTE *pbData2 = NULL;
		DWORD dwLength=0;
		DWORD dwLength2=0;

		// Lock DirectSound buffer
		m_BgmDC.m_lpDSB->Lock (	m_BgmDC.m_WriteCursor,
								dwInvalidLength,
								(LPVOID*)&pbData,
								&dwLength,
								(LPVOID*)&pbData2,
								&dwLength2,
								0);	
		
		if(pbData)
			memset(pbData, 0, dwLength);
		if(pbData2)
			memset(pbData2, 0, dwLength2);

		// Unlock the buffer
		m_BgmDC.m_lpDSB->Unlock (	pbData,
									dwLength,
									pbData2,
									dwLength2);	

		// Move write cursor
		m_BgmDC.m_WriteCursor += dwInvalidLength;
		if(m_BgmDC.m_WriteCursor >= OUTPUTBLOCKSIZE)
			m_BgmDC.m_WriteCursor -= OUTPUTBLOCKSIZE;

	}

	// For this function to work there needs to be 2 calls - one to detect the wrap 
	// and another to detect the termination - it is therefore necessary for the 
	// event to be triggered on a reasonably regular interval
	if(m_BgmDC.m_TermState_HighestPlayPos == INVALIDCURSORPOS ||
		m_BgmDC.m_TermState_HighestPlayPos < dwCurrentPlayCursor)
	{
		m_BgmDC.m_TermState_HighestPlayPos = dwCurrentPlayCursor;
	}
	else
	{
		m_BgmDC.m_TermState_Wrapped = TRUE;
	}

	if(m_BgmDC.m_TermState_Wrapped == FALSE)
	{
		return FALSE;
	}

	if(dwCurrentPlayCursor > m_BgmDC.m_TermState_WriteCursor)
	{
		return TRUE;
	}

	return FALSE;
}

void DxBgmSound::GetPlayPosAndInvalidLength(DWORD* pdwPlayPos, DWORD* pdwLength)
{
	m_BgmDC.m_lpDSB->GetCurrentPosition ( pdwPlayPos, NULL);
	
	// Get amount of free data in the buffer
	if(m_BgmDC.m_WriteCursor != INVALIDCURSORPOS)
	{
		if(m_BgmDC.m_WriteCursor >= *pdwPlayPos)
			*pdwLength = (OUTPUTBLOCKSIZE - m_BgmDC.m_WriteCursor) + *pdwPlayPos;
		else
			*pdwLength = *pdwPlayPos - m_BgmDC.m_WriteCursor;
	}
	else 
	{
		// First write
		m_BgmDC.m_WriteCursor = 0;
		*pdwLength = OUTPUTBLOCKSIZE;
	}
}

void DxBgmSound::RefillBuffers()
{
	HRESULT	hr;
	BYTE *pbData;
	BYTE *pbData2;
	DWORD dwLength=0;
	DWORD dwLength2=0;
	BOOL bMoreData=TRUE;
	DWORD RealLength;
	DWORD RealLength2;
	DWORD dwAmountToFill, dwCurrentPlayCursor;

	if(!m_BgmDC.m_lpDirectSound)
		return;

	GetPlayPosAndInvalidLength(&dwCurrentPlayCursor, &dwAmountToFill);

	// Limit the amount of filling that we do per batch
	// - This will improve seek time and skipping on heavilly loaded systems
	if(dwAmountToFill > MAXFILLAMOUNT && m_BgmDC.m_bStreamRunning == FALSE)
	{
		dwAmountToFill = MAXFILLAMOUNT;
	}

	// Try to totally fill the buffer with sound data - this is the best policy because our
	// timer (event trigger) is likely to have v.poor accrucy
	if(dwAmountToFill > 0)
	{
		// Lock DirectSound buffer
		hr = m_BgmDC.m_lpDSB->Lock (m_BgmDC.m_WriteCursor,
											dwAmountToFill,
											(LPVOID*)&pbData,
											&dwLength,
											(LPVOID*)&pbData2,
											&dwLength2,
											0);			
		if(FAILED(hr))
		{
			return;
		}

		RealLength = dwLength;
		RealLength2 = dwLength2;
		if(RealLength)
		{
			bMoreData = GetPCMBlock(pbData, &RealLength);
		
			// Move cursor
			m_BgmDC.m_WriteCursor += RealLength;
			if(m_BgmDC.m_WriteCursor >= OUTPUTBLOCKSIZE)
				m_BgmDC.m_WriteCursor -= OUTPUTBLOCKSIZE;
			
			// Skip second block if there is no more data
			if(!bMoreData)
				RealLength2 = 0;
		}

		if(RealLength2) 
		{
			bMoreData = GetPCMBlock(pbData2, &RealLength2);
			
			// Move cursor
			m_BgmDC.m_WriteCursor += RealLength2;
			if(m_BgmDC.m_WriteCursor >= OUTPUTBLOCKSIZE)
				m_BgmDC.m_WriteCursor -= OUTPUTBLOCKSIZE;
		}

		// Unlock the buffer
		hr = m_BgmDC.m_lpDSB->Unlock ( pbData,
												dwLength,
												pbData2,
												dwLength2);	
		if(FAILED(hr))
		{
			return;
		}
	}
		
	if(bMoreData == FALSE)
	{
		m_bMoreData = FALSE;
	}

	m_BgmDC.m_TermState_Wrapped = FALSE;
	m_BgmDC.m_TermState_WriteCursor = INVALIDCURSORPOS;
	m_BgmDC.m_TermState_HighestPlayPos = INVALIDCURSORPOS;

	if(!m_BgmDC.m_bStreamRunning)
		EnablePlay ( FALSE );
}

HRESULT DxBgmSound::CtrlVolume ()
{
	if ( !m_BgmDC.m_lpDSB ) return S_FALSE;

	long lBufferVolume = 0;

	if ( m_bActivated )
	{
		m_BgmDC.m_lpDSB->GetVolume ( &lBufferVolume );
		m_BgmDC.m_lpDSB->SetVolume ( lBufferVolume-1 );
		m_BgmDC.m_lpDSB->SetVolume ( lBufferVolume+1 );
		m_BgmDC.m_lpDSB->SetVolume ( m_Volume );
		
		if ( ++m_wCount > 100 ) m_bActivated = FALSE;

		return S_OK;
	}

	m_BgmDC.m_lpDSB->GetVolume ( &lBufferVolume );
    if ( lBufferVolume == m_Volume ) return S_OK;
	return m_BgmDC.m_lpDSB->SetVolume ( m_Volume );
}

BOOL DxBgmSound::IsPlaying ()
{
	return m_bPlaying;
}

BOOL DxBgmSound::ForceStop ()
{
	BOOL bResult = Stop ();
	ClearFile ();

	return bResult;
}

void DxBgmSound::SetActivate ( BOOL bActivity )
{
	m_bActivated = bActivity;
	if ( m_bActivated ) m_wCount = 0;
}