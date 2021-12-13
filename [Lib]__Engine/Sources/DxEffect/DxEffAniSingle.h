#pragma once

#include "DxEffAni.h"
#include "DxMethods.h"
#include "./DxEffSinglePropGMan.h"

#define STRING_NUM_128	128

#define	USEDRAWPOINT	0x00001	// 한점
#define	USEDRAWRANGE	0x00002	// 범위
#define	USEDRAWDIRECT	0x00004	// 이동
#define	USEDRAWMATRIX	0x00008	// Matrix
#define	USETIMEDAY		0x00010			// 낮
#define	USETIMENIGHT	0x00020			// 밤
#define	USESKYFINE		0x00100				// 맑음
#define	USESKYRAIN		0x00200				// 비
#define	USESKYSNOW		0x00400				// 눈
#define	USESKYLEAVES	0x00800				// 낙엽
#define	USECHARMOVE		0x01000					// 캐릭따름

class DxSkinChar;

class DxEffSingleGroup;

struct EFFANI_PROPERTY_SINGLE_100
{
	DWORD			m_dwFlags;
	DWORD			m_dwFrame;
	float			m_fFact;
	float			m_fScale;

	char			m_szEff[ STRING_NUM_128 ];
	char			m_szBipFirst[ STRING_NUM_128 ];
	char			m_szBipSecond[ STRING_NUM_128 ];
};

struct EFFANI_PROPERTY_SINGLE : public EFFANI_PROPERTY
{
	DWORD			m_dwFlags;
	DWORD			m_dwFrame;
	float			m_fFact;
	float			m_fScale;
	D3DXMATRIX		m_matBase;

	char			m_szEff[ STRING_NUM_128 ];
	char			m_szBipFirst[ STRING_NUM_128 ];
	char			m_szBipSecond[ STRING_NUM_128 ];

	EFFANI_PROPERTY_SINGLE () :
		m_dwFlags(0L),
		m_dwFrame(0),
		m_fFact(0.f),
		m_fScale(1.f)	
	{
		m_dwFlags |= ( USEDRAWPOINT | 
						USETIMEDAY|USETIMENIGHT | 
						USESKYFINE|USESKYRAIN|USESKYSNOW|USESKYLEAVES );

		D3DXMatrixIdentity( &m_matBase );

		memset( m_szEff, 0, sizeof(char)*STRING_NUM_128 );
		memset( m_szBipFirst, 0, sizeof(char)*STRING_NUM_128 );
		memset( m_szBipSecond, 0, sizeof(char)*STRING_NUM_128 );
	}
};

class DxEffAniData_Single : public DxEffAniData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	union
	{
		struct
		{
			EFFANI_PROPERTY_SINGLE		m_Property;
		};

		struct
		{
			DWORD			m_dwFlags;
			DWORD			m_dwFrame;
			float			m_fFact;
			float			m_fScale;
			D3DXMATRIX		m_matBase;	// 실제 곱해지는 값.

			char			m_szEff[ STRING_NUM_128 ];
			char			m_szBipFirst[ STRING_NUM_128 ];
			char			m_szBipSecond[ STRING_NUM_128 ];
		};
	};

protected:
	EFF_PROPGROUP*	m_pPropGroup;			//	속성값.

public:
	BOOL CheckEff ( DWORD dwCurKeyTime, int nPrevFrame );

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty )
	{
		m_Property = *((EFFANI_PROPERTY_SINGLE*)pProperty);
	}
	
	virtual EFFANI_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual void SetEffAniData ( DxEffAniData*	pData );

public:
	virtual DxEffAni* NEWOBJ ( SAnimContainer* pAnimContainer );	//	Note : 이팩트 생성자.
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	DxEffAniData_Single () :
		DxEffAniData(),
		m_pPropGroup(NULL)
	{
	}
};

class DxEffAniSingle : public DxEffAni
{
public:
	DWORD			m_dwFlags;
	DWORD			m_dwFrame;
	float			m_fFact;
	float			m_fScale;


	char			m_szEff[128];
	char			m_szBipFirst[128];
	char			m_szBipSecond[128];

	EFF_PROPGROUP*	m_pPropGroup;			//	속성값.

	BOOL			m_bDataSetting;
	D3DXVECTOR3		m_vPos_1;
	D3DXVECTOR3		m_vPos_2;
	D3DXMATRIX		m_matBase;
	D3DXMATRIX		m_matWorld;
	float			m_fRandomFact;

protected:
	D3DXMATRIX			m_matTrans;				//	트랜스 매트릭스.
	DxEffSingleGroup*	m_pSingleGroup;			//	활성 개체.

protected:
	BOOL	SetPosition ( DxSkinChar* pSkinChar, LPD3DXMATRIX& matBone );
	void	SetMatrix ( const D3DXMATRIX& matCurPos, D3DXMATRIX* pmatBone );

protected:
	BOOL	SetWeather ();
	BOOL	SetTime ();

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
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos );

public:
	DxEffAniSingle(void);
	virtual ~DxEffAniSingle(void);
};

