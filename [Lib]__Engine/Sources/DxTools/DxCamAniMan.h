#pragma once
#include "DxAnimationManager.h"

class CSerialFile;
class DxFrameMesh;

enum CAMERA_TYPE
{
	CAMERA_NULL			= 0,
	CAMERA_UICONTR		= 1,
	CAMERA_TFREE		= 2,
	CAMERA_TTARGET		= 3,
};

enum CAMERA_FLAG
{
	DXVP_LIMITDIR		= 0x0001,
	DXVP_LANDSCAPEMOVE	= 0x0002,
	DXVP_USER			= 0x0004,
	DXVP_GAME			= 0x0008,
	
	CAMERA_FREESET		= 0x0010,
	CAMERA_SET			= 0x0020,

	CAMERA_ANILOOP		= 0x0100,
};

class DxCameraAni
{
public:
	static const DWORD VERSION;

	char*			m_szName;

	CAMERA_TYPE		m_emCamera;
	DWORD			m_dwFlag;

	float			m_STime;
	float			m_ETime;

	char*			m_szFromCamera;
	BOOL			m_bLocalFromCamera;
	DxAnimation*	m_pFromCamera;

	char*			m_szTargetCamera;
	BOOL			m_bLocalTargetCamera;
	DxAnimation*	m_pTargetCamera;

	DxCameraAni*	m_pNext;

	void ModifySetCamera ( DxCameraAni* pCameraAni );
	void ModifyCamera ( DxCameraAni* pCameraAni );

public:
	void ClearSet ()
	{
		SAFE_DELETE_ARRAY(m_szName);

		m_STime = 0;
		m_ETime = 0;

		m_emCamera = CAMERA_TFREE;
		m_dwFlag = CAMERA_FREESET;

		SAFE_DELETE_ARRAY(m_szFromCamera);
		SAFE_DELETE_ARRAY(m_szTargetCamera);

		m_bLocalFromCamera = FALSE;
		m_bLocalTargetCamera = FALSE;
	}
	
	void Clear ()
	{
		if ( m_bLocalFromCamera )	SAFE_DELETE(m_pFromCamera);
		if ( m_bLocalTargetCamera )	SAFE_DELETE(m_pTargetCamera);

		ClearSet ();
	}

	HRESULT SaveSet ( CSerialFile &SFile );
	HRESULT LoadSet ( CSerialFile &SFile, DxFrameMesh *pFrameMesh );

	HRESULT Save ( CSerialFile &SFile );
	HRESULT Load ( CSerialFile &SFile );

public:
	DxCameraAni () :
		m_szName(NULL),
		m_emCamera(CAMERA_TFREE),
		m_dwFlag(CAMERA_FREESET),
		m_STime(0.0f),
		m_ETime(0.0f),
		m_szFromCamera(NULL),
		m_bLocalFromCamera(FALSE),
		m_pFromCamera(NULL),
		m_szTargetCamera(NULL),
		m_bLocalTargetCamera(FALSE),
		m_pTargetCamera(NULL),
		m_pNext(NULL)
	{
	}

	~DxCameraAni ()
	{
		SAFE_DELETE_ARRAY(m_szName);

		SAFE_DELETE_ARRAY(m_szFromCamera);
		SAFE_DELETE_ARRAY(m_szTargetCamera);

		if ( m_bLocalFromCamera )	SAFE_DELETE(m_pFromCamera);
		if ( m_bLocalTargetCamera )	SAFE_DELETE(m_pTargetCamera);

		SAFE_DELETE(m_pNext);
	}
};

class DxCamAniMan
{
protected:
	float			m_fAgeCamera;
	DxCameraAni*	m_pCurrentAni;

	DxCameraAni*	m_pAniList;

public:
	HRESULT AddCameraAni ( DxCameraAni* pCameraAni );
	DxCameraAni* FindCameraAni ( char* szName );
	HRESULT DelCameraAni ( char* szName );

public:
	BOOL IsActiveCameraAni ()		{ return m_pCurrentAni!=NULL; }
	HRESULT ActiveCameraAni ( char* szName );
	HRESULT ActiveCameraPos ( char* szName );

	DxCameraAni* GetCameraList ()	{ return m_pAniList; }

public:
	HRESULT Import ( DxCamAniMan* pCameraAniMan );

	//	에디터에서 카메라 에니메이션 테스트시에 필요로함.
public:
	HRESULT TestCameraAni ( DxCameraAni* pCameraAni );
	HRESULT ResetTestCameraAni ();

public:
	HRESULT SaveSet ( CSerialFile &SFile );
	HRESULT LoadSet ( CSerialFile &SFile, DxFrameMesh *pFrameMesh );

public:
	HRESULT Save ( CSerialFile &SFile );
	HRESULT Load ( CSerialFile &SFile );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	HRESULT Cleanup ();

public:
	DxCamAniMan(void);
	~DxCamAniMan(void);
};
