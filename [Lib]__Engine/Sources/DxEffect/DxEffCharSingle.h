#pragma once

#include "DxEffChar.h"
#include "DxMethods.h"

struct EFF_PROPGROUP;
class DxEffSingleGroup;

struct EFFCHAR_PROPERTY_SINGLE_100
{
	int				m_nType;

	char			m_szFileName[MAX_PATH];	//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;				//	날씨..

	float			m_fMoveSpeed;

	BOOL			m_bAllMType;				//	타이밍 및 필요한 부분에 넣는 것
	BOOL			m_bAllSType;				//
	BOOL			m_bAllTime;					//
	DWORD			m_dwKeyStartTime;			//
	DWORD			m_dwKeyEndTime;				//
	EMANI_MAINTYPE	m_SetAniMType;				//
	EMANI_SUBTYPE	m_SetAniSType;				//

	DWORD			m_dwPlaySlngle;

	EFFCHAR_PROPERTY_SINGLE_100()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_101
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;						//	날씨..
	int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
	DWORD			m_dwPlaySlngle;

	EFFCHAR_PROPERTY_SINGLE_101()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_102
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;						//	날씨..
	int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
	DWORD			m_dwPlaySlngle;
	float			m_fScale;

	EFFCHAR_PROPERTY_SINGLE_102()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_103
{
	int				m_nType;
	char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;						//	날씨..
	int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;

	EFFCHAR_PROPERTY_SINGLE_103()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

struct EFFCHAR_PROPERTY_SINGLE_104
{
	int				m_nType;
	DWORD			m_dwFlag;
	float			m_fCoolTime;
	char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;						//	날씨..
	int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;

	EFFCHAR_PROPERTY_SINGLE_104()
	{
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		memset( m_szTrace, 0, sizeof(char)*STRACE_NSIZE );
		memset( m_szTraceBack, 0, sizeof(char)*STRACE_NSIZE );
	};
};

#define	USE_REPEAT		0x0001
#define	USE_REPEATMOVE	0x0002
#define	USE_AUTOMOVE	0x0004
#define	USE_1POINT		0x0010
#define	USE_2POINT		0x0020
#define	USE_MATRIX		0x0040

struct EFFCHAR_PROPERTY_SINGLE : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	float			m_fCoolTime;
	char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
	char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
	char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
	int				m_nWeather;						//	날씨..
	int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
	DWORD			m_dwPlaySlngle;
	float			m_fScale;
	float			m_fDepthUp;
	float			m_fDepthDown;
	float			m_fRepeatSpeed;

	EFFCHAR_PROPERTY_SINGLE () :
		m_nWeather(0),
		m_nAniMType(0),

		m_fCoolTime(10.f),
		m_dwPlaySlngle(10),
		m_fScale(1.f),
		m_fDepthUp(0.5f),
		m_fDepthDown(0.5f),
		m_fRepeatSpeed(1.f)
	{
		m_dwFlag = USE_1POINT;
		memset( m_szFileName, 0, sizeof(char)*MAX_PATH );

		StringCchCopy( m_szTrace,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceBack,	STRACE_NSIZE, STRACE_IMAGE02 );
	}
};

class DxEffCharSingle : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_SINGLE m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			float			m_fCoolTime;
			char			m_szFileName[MAX_PATH];			//	이펙트 싱글 이름.
			char			m_szTrace[STRACE_NSIZE];		//	추적 포인터 인덱스.
			char			m_szTraceBack[STRACE_NSIZE];	//	추적 포인터 인덱스.
			int				m_nWeather;						//	날씨..
			int				m_nAniMType;					//	공격 타임	0 전체, 1 공격, 2, 공격 외
			DWORD			m_dwPlaySlngle;
			float			m_fScale;
			float			m_fDepthUp;
			float			m_fDepthDown;
			float			m_fRepeatSpeed;
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_SINGLE*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:
	DxSkinMesh9*		m_pSkinMesh;

protected:
	D3DXVECTOR3			m_vTransUp;				//	위치.
	D3DXVECTOR3			m_vPrevUp;				//	위치.

	D3DXVECTOR3			m_vTransDown;			//	위치.

	D3DXVECTOR3			m_vBackUp;				//	백업 값.

	float				m_fTarDirection;
	D3DXVECTOR3			m_vDirection;

	D3DXMATRIX			m_matTrans;				//	트랜스 매트릭스.
	EFF_PROPGROUP*		m_pPropGroup;			//	속성값.
	DxEffSingleGroup*	m_pSingleGroup;			//	활성 개체.

	float				m_fCoolTimeADD;

	D3DXVECTOR3			m_vCurPos;				// 현재 위치
	float				m_fTime;

	float				m_fPlayTime;

protected:
	BOOL	SetWeather ();

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale );

	virtual DWORD GetStateOrder () const					{ return EMEFFSO_SINGLE_EFF; }
	virtual void ConvertTracePoint();

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	virtual void SetDirection ( D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharSingle(void);
	~DxEffCharSingle(void);
};

