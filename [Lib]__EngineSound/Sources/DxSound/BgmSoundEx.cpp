#include "pch.h"
#include "BgmSound.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned int DxBgmSound::PlaySound( LPVOID pData )
{
	SThreadCtrl&	rhThreadCtrl = *((SThreadCtrl*)pData);
	HANDLE&			rhThreadTerminate  = rhThreadCtrl.m_hEventTerminate;

	while ( WaitForSingleObject ( rhThreadTerminate, 0 ) != WAIT_OBJECT_0 )
	{	
		DxBgmSound::GetInstance().Playing ();
		Sleep( 1 );
	}	

	return 0;
}

BOOL DxBgmSound::LoadOggFile ()
{
	//	파일 전체 경로로 만들기
	//
	if ( ! ( m_strPath.GetLength () && m_strFileName.GetLength () ) )
	{
		return FALSE;
	}

	CString strFilePath;
	strFilePath = m_strPath + m_strFileName;
	
	if ( m_OggFile.m_hFile )
	{
		ov_clear ( &m_OggFile.m_vf );
		fclose ( m_OggFile.m_hFile );
		m_OggFile.m_hFile = NULL;
		return FALSE;
	}

    //	파일 열기
	fopen_s(&m_OggFile.m_hFile, strFilePath.GetString(), "rb");
	if ( m_OggFile.m_hFile == NULL )
	{
		return FALSE;
	}

	memset ( &m_OggFile.m_vf, 0, sizeof ( m_OggFile.m_vf ) );
	if ( ov_open(m_OggFile.m_hFile, &m_OggFile.m_vf, NULL, 0 ) < 0 ) 
	{
		fclose ( m_OggFile.m_hFile );
		m_OggFile.m_hFile = NULL;
		return FALSE;
	}

	if ( ov_streams ( &m_OggFile.m_vf ) != 1 )
	{
		ov_clear ( &m_OggFile.m_vf );
		fclose ( m_OggFile.m_hFile );
		m_OggFile.m_hFile = NULL;
		return FALSE;
	}

	vorbis_info *pInfo = NULL;
	pInfo = ov_info ( &m_OggFile.m_vf, -1 );

	if ( pInfo == NULL )
	{		
		ov_clear ( &m_OggFile.m_vf );
		fclose ( m_OggFile.m_hFile );
		m_OggFile.m_hFile = NULL;
		return FALSE;
	}

	if ( pInfo->channels > 2 )
	{		
		ov_clear ( &m_OggFile.m_vf );
		fclose ( m_OggFile.m_hFile );
		m_OggFile.m_hFile = NULL;
		return FALSE;
	}
	
	m_OggFile.m_FileInfo.m_b16bit = TRUE;
	m_OggFile.m_FileInfo.m_iBitRate_Kbs = ov_bitrate ( &m_OggFile.m_vf, -1 ) / 1000;
	m_OggFile.m_FileInfo.m_iFileLength_Secs = (int) ov_time_total ( &m_OggFile.m_vf, -1 );
	m_OggFile.m_FileInfo.m_iFreq_Hz = pInfo->rate;
	m_OggFile.m_FileInfo.m_bStereo = ( pInfo->channels == 2 );

	return TRUE;
}

BOOL DxBgmSound::UnLoadOggFile ()
{
	if ( !m_OggFile.m_hFile )
	{
		return FALSE;
	}

	ov_clear(&m_OggFile.m_vf);
	fclose ( m_OggFile.m_hFile );
    m_OggFile.m_hFile = NULL;

	return TRUE;
}

BOOL DxBgmSound::LoadSoundBuffer ()
{	
	HRESULT hr;	
	hr = CreateDirectSoundBuffer ();
	if ( FAILED ( hr ) )
	{
		return FALSE;
	}	

	hr = InitDirectSoundBuffer ();
	if ( FAILED ( hr ) )
	{
		return FALSE;
	}

	//	싱크 오브젝트 만들기
	if ( m_evtBlockFree )
	{		
		CloseHandle ( m_evtBlockFree );
		m_evtBlockFree = NULL;
	}

	m_evtBlockFree = CreateEvent(NULL, FALSE, FALSE, NULL);
	if ( !m_evtBlockFree )
	{
		return FALSE;
	}
	
	return TRUE;
}

HRESULT	DxBgmSound::InitDirectSoundBuffer ()
{
	HRESULT hr;

	// Empty sound buffer
	BYTE *pbData = NULL;
	BYTE *pbData2 = NULL;
	DWORD dwLength=0;
	DWORD dwLength2=0;

	// Lock DirectSound buffer
	hr = m_BgmDC.m_lpDSB->Lock ( 0, OUTPUTBLOCKSIZE,
		(LPVOID*)&pbData, &dwLength,
        (LPVOID*)&pbData2,&dwLength2, 0);
	if ( FAILED ( hr ) )
	{
		return hr;
	}
	
	if(pbData)
		memset(pbData, 0, dwLength);
	if(pbData2)
		memset(pbData2, 0, dwLength2);

	// Unlock the buffer
	hr = m_BgmDC.m_lpDSB->Unlock ( pbData, dwLength, pbData2, dwLength2);
	if ( FAILED ( hr ) )
	{
		return hr;
	}	
	
	// Set bufferoffset to 0	
	m_BgmDC.m_WriteCursor = INVALIDCURSORPOS;
	m_BgmDC.m_TermState_Wrapped = FALSE;
	m_BgmDC.m_TermState_WriteCursor = INVALIDCURSORPOS;
	m_BgmDC.m_TermState_HighestPlayPos = INVALIDCURSORPOS;
	m_BgmDC.m_TimerId = 0;
	m_BgmDC.m_bStreamRunning = FALSE;

	return S_OK;
}

BOOL DxBgmSound::UnLoadSoundBuffer ()
{
	EnablePlay ( FALSE );

	if ( m_evtBlockFree )
	{
		CloseHandle ( m_evtBlockFree );
		m_evtBlockFree = NULL;
	}

	ReleaseDirectSoundBuffer ();

	return TRUE;
}


HRESULT	DxBgmSound::CreateDirectSound ()
{
	HRESULT hr;

	GUID* pSoundGUID = NULL;

	// Create IDirectSound using the primary sound device
	if( FAILED( hr = DirectSoundCreate8( pSoundGUID, &m_BgmDC.m_lpDirectSound, NULL ) ) )
		return DXTRACE_ERR( TEXT("DirectSoundCreate8"), hr );

	// Set DirectSound coop level 
	if( FAILED( hr = m_BgmDC.m_lpDirectSound->SetCooperativeLevel( m_hWnd, DSSCL_PRIORITY ) ) )
		return DXTRACE_ERR( TEXT("SetCooperativeLevel"), hr );	

	return S_OK;
}

HRESULT	DxBgmSound::CreateDirectSoundBuffer ()
{
	HRESULT hr;

	// Set primary buffer format
	{
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		if( m_BgmDC.m_lpDirectSound == NULL )
			return CO_E_NOTINITIALIZED;

		// Get the primary buffer 
		DSBUFFERDESC dsbd;
		SecureZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;
	       
		if( FAILED( hr = m_BgmDC.m_lpDirectSound->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
			return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

		WAVEFORMATEX wfx;
		SecureZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = WAVE_FORMAT_PCM; 
		wfx.nChannels       = (WORD) m_OggFile.m_FileInfo.m_bStereo ? 2 : 1; 
		wfx.nSamplesPerSec  = m_OggFile.m_FileInfo.m_iFreq_Hz; 
		wfx.wBitsPerSample  = (WORD) m_OggFile.m_FileInfo.m_b16bit ? 16 : 8; 
		wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
			return DXTRACE_ERR( TEXT("SetFormat"), hr );

		SAFE_RELEASE( pDSBPrimary );
	}

	//	Create Sound Buffer
	{
		m_BgmDC.m_WaveFile.wFormatTag = WAVE_FORMAT_PCM;
		m_BgmDC.m_WaveFile.nChannels = m_OggFile.m_FileInfo.m_bStereo ? 2 : 1;
		m_BgmDC.m_WaveFile.nSamplesPerSec = m_OggFile.m_FileInfo.m_iFreq_Hz;
		m_BgmDC.m_WaveFile.wBitsPerSample = m_OggFile.m_FileInfo.m_b16bit ? 16 : 8;
		m_BgmDC.m_WaveFile.nBlockAlign = (m_BgmDC.m_WaveFile.nChannels * m_BgmDC.m_WaveFile.wBitsPerSample)>>3;
		m_BgmDC.m_WaveFile.nAvgBytesPerSec = m_BgmDC.m_WaveFile.nSamplesPerSec * m_BgmDC.m_WaveFile.nBlockAlign;
		m_BgmDC.m_WaveFile.cbSize = 0;
		
		// Create sound buffer
		DSBUFFERDESC dsbd;
		memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		dsbd.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME;	
		dsbd.dwBufferBytes = OUTPUTBLOCKSIZE; 
		dsbd.lpwfxFormat = &m_BgmDC.m_WaveFile;
		hr = m_BgmDC.m_lpDirectSound->CreateSoundBuffer ( &dsbd, &(m_BgmDC.m_lpDSB ), NULL );
		if(FAILED(hr))
		{
			m_BgmDC.m_lpDSB = NULL;
			return hr;
		}
	}

	return S_OK;
}

HRESULT	DxBgmSound::ReleaseDirectSoundBuffer()
{
	SAFE_RELEASE ( m_BgmDC.m_lpDSB );
	return S_OK;
}

HRESULT	DxBgmSound::ReleaseDirectSound ()
{
	SAFE_RELEASE ( m_BgmDC.m_lpDirectSound );
	return S_OK;
}

LONG DxBgmSound::GetVolume ()
{
	return m_Volume;
}

void DxBgmSound::SetVolume ( LONG Volume )
{
	m_Volume = Volume;
}