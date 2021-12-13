//-----------------------------------------------------------------------------
// File: DSUtil.h
//
// Desc: 
//
// Copyright (c) 1999-2001 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef DSUTIL_H
#define DSUTIL_H

#pragma once
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>




//-----------------------------------------------------------------------------
// Classes used by this header
//-----------------------------------------------------------------------------
class CSoundManager;
class CSound;
class CStreamingSound;
class CWaveFile;




//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

#define DSUtil_StopSound(s)         { if(s) s->Stop(); }
#define DSUtil_PlaySound(s)         { if(s) s->Play( 0, 0 ); }
#define DSUtil_PlaySoundLooping(s)  { if(s) s->Play( 0, DSBPLAY_LOOPING ); }




//-----------------------------------------------------------------------------
// Name: class CSoundManager
// Desc: 
//-----------------------------------------------------------------------------
class CSoundManager
{
protected:
    LPDIRECTSOUND8	m_pDS;
	int				m_DlgID;

public:
    CSoundManager();
    ~CSoundManager();

	HRESULT	EnumDevice ( HWND hWnd, int DlgItemID );

    HRESULT Initialize( HWND hWnd, DWORD dwCoopLevel, DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    inline  LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
    HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
    HRESULT Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

    HRESULT Create( CSound** ppSound, LPTSTR strWaveFileName, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateFromMemory( CSound** ppSound, BYTE* pbData, ULONG ulDataSize, LPWAVEFORMATEX pwfx, DWORD dwCreationFlags = 0, GUID guid3DAlgorithm = GUID_NULL, DWORD dwNumBuffers = 1 );
    HRESULT CreateStreaming( CStreamingSound** ppStreamingSound, LPTSTR strWaveFileName, DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNotifyCount, DWORD dwNotifySize, HANDLE hNotifyEvent );

static	INT_PTR CALLBACK DSoundEnumCallback( GUID* pGUID, LPSTR strDesc, LPSTR strDrvName, VOID* pContext );

};




//-----------------------------------------------------------------------------
// Name: class CSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CSound
{
protected:
    LPDIRECTSOUNDBUFFER* m_apDSBuffer;
    DWORD                m_dwDSBufferSize;
    CWaveFile*           m_pWaveFile;
    DWORD                m_dwNumBuffers;
	BOOL*				 m_pbUseSound;

	HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:
    CSound( LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, CWaveFile* pWaveFile );
    virtual ~CSound();

    HRESULT Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
    HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );    
	LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );

	HRESULT Play( DWORD dwPriority = 0, DWORD dwFlags = 0 );
    HRESULT Stop();
    HRESULT Reset();
    BOOL    IsSoundPlaying();

	//	<--	버퍼단위의 제어	-->	//
	LPDIRECTSOUNDBUFFER GetFreeBuffer( DWORD &BufferID );
	HRESULT PlayBuffer ( DWORD &BufferID, DWORD dwPriority = 0, DWORD dwFlags = 0 );
	HRESULT	StopBuffer ( DWORD BufferID );
	HRESULT	ResetBuffer ( DWORD BufferID );
	BOOL	IsSoundPlayingBuffer ( DWORD BufferID );	
};

//-----------------------------------------------------------------------------
// Name: class CStreamingSound
// Desc: Encapsulates functionality to play a wave file with DirectSound.  
//       The Create() method loads a chunk of wave file into the buffer, 
//       and as sound plays more is written to the buffer by calling 
//       HandleWaveStreamNotification() whenever hNotifyEvent is signaled.
//-----------------------------------------------------------------------------
class CStreamingSound : public CSound
{
protected:
    DWORD m_dwLastPlayPos;
    DWORD m_dwPlayProgress;
    DWORD m_dwNotifySize;
    DWORD m_dwNextWriteOffset;
    BOOL  m_bFillNextNotificationWithSilence;

public:
    CStreamingSound( LPDIRECTSOUNDBUFFER pDSBuffer, DWORD dwDSBufferSize, CWaveFile* pWaveFile, DWORD dwNotifySize );
    ~CStreamingSound();

    HRESULT HandleWaveStreamNotification( BOOL bLoopedPlay );
    HRESULT Reset();
};




//-----------------------------------------------------------------------------
// Name: class CWaveFile
// Desc: Encapsulates reading or writing sound data to or from a wave file
//-----------------------------------------------------------------------------
class CWaveFile
{
public:
    WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
    HMMIO         m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO      m_ck;          // Multimedia RIFF chunk
    MMCKINFO      m_ckRiff;      // Use in opening a WAVE file
    DWORD         m_dwSize;      // The size of the wave file
    MMIOINFO      m_mmioinfoOut;
    DWORD         m_dwFlags;
    BOOL          m_bIsReadingFromMemory;
    BYTE*         m_pbData;
    BYTE*         m_pbDataCur;
    ULONG         m_ulDataSize;
    CHAR*         m_pResourceBuffer;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveFile();
    ~CWaveFile();

    HRESULT Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();

    HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );

    DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};



//-----------------------------------------------------------------------------
// Name: enum ESFXType
// Desc: each is a unique identifier mapped to a DirectSoundFX
//-----------------------------------------------------------------------------
enum ESFXType
{
    eSFX_chorus = 0,
    eSFX_compressor,
    eSFX_distortion,
    eSFX_echo,
    eSFX_flanger,
    eSFX_gargle,
    eSFX_parameq,
    eSFX_reverb,

    // number of enumerated effects
    eNUM_SFX
};

//-----------------------------------------------------------------------------
// Name: class CSoundFXManager
// Desc: Takes care of effects for one DirectSoundBuffer
//-----------------------------------------------------------------------------
class CSoundFXManager
{
public:
    CSoundFXManager( );
    ~CSoundFXManager( );

public: // interface
    HRESULT Initialize ( LPDIRECTSOUNDBUFFER lpDSB, BOOL bLoadDefaultParamValues );
    HRESULT UnInitialize ( );

    HRESULT SetFXEnable( DWORD esfxType );
    HRESULT ActivateFX( );
    HRESULT DisableAllFX( );
    HRESULT LoadCurrentFXParameters( );
	const	TCHAR*	GetFxName ( DWORD esfxType );

public: // members
    LPDIRECTSOUNDFXCHORUS8      m_lpChorus;
    LPDIRECTSOUNDFXCOMPRESSOR8  m_lpCompressor;
    LPDIRECTSOUNDFXDISTORTION8  m_lpDistortion;
    LPDIRECTSOUNDFXECHO8        m_lpEcho;
    LPDIRECTSOUNDFXFLANGER8     m_lpFlanger;
    LPDIRECTSOUNDFXGARGLE8      m_lpGargle;
    LPDIRECTSOUNDFXPARAMEQ8     m_lpParamEq;
    LPDIRECTSOUNDFXWAVESREVERB8 m_lpReverb;

    DSFXChorus                  m_paramsChorus;
    DSFXCompressor              m_paramsCompressor;
    DSFXDistortion              m_paramsDistortion;
    DSFXEcho                    m_paramsEcho;
    DSFXFlanger                 m_paramsFlanger;
    DSFXGargle                  m_paramsGargle;
    DSFXParamEq                 m_paramsParamEq;
    DSFXWavesReverb             m_paramsReverb;

    LPDIRECTSOUNDBUFFER8        m_lpDSB8;

protected:
    DSEFFECTDESC                m_rgFxDesc[eNUM_SFX];
    const GUID *                m_rgRefGuids[eNUM_SFX];
    LPVOID *                    m_rgPtrs[eNUM_SFX];
    BOOL                        m_rgLoaded[eNUM_SFX];
    DWORD                       m_dwNumFX;

    HRESULT EnableGenericFX( GUID guidSFXClass, REFGUID rguidInterface, LPVOID * ppObj );
    HRESULT LoadDefaultParamValues( );

public:
	static	const	TCHAR *		m_tszFXNames[8];
};

#endif // DSUTIL_H
