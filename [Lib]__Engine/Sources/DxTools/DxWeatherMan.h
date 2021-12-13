// dxweather.h: interface for the DxViewPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DXWEATHERMAN_H__INCLUDED_)
#define _DXWEATHERMAN_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./GLDefine.h"

class	CSerialFile;

#define		ISACTIVE		0x0001		// Ȱ��ȭ
//#define		ISPRESSURE		0x0002		// ���� ���� - Ȱ��ȭ�� �Ǿ��־�� ���� ���൵ ���� �ϴ�.

#define		ISRAIN			0x0010		// ��		// ��	����	����
#define		ISSNOW			0x0020		// ��		//						�ܿ�
#define		ISLEAVES		0x0040		// ����		//				����
#define		ISSPORE			0x0080		// ����		// ��

#define		ISRAINDOING		0x0100		// ��		// ��	����	����			// ��������
#define		ISSNOWDOING		0x0200		// ��		//						�ܿ�	// ��������
#define		ISLEAVESDOING	0x0400		// ����		//				����			// ��������
#define		ISSPOREDOING	0x0800		// ����		// ��							// ��������

enum	WEATHERSTATS
{
	EMWS_NULL		= 0,
	EMWS_STARTING	= 1,
	EMWS_PLAYING	= 2,
	EMWS_ENDING		= 3,
};

//	Note : �� - ��(����)  ���� ȿ�� ����� �� ����Ѵ�.
//
struct RAIN_PROPERTY_101
{
	DWORD		m_dwFaces;
	int			m_iRadius;	// �� �Ѹ��� �ִ� �ݰ�
	int			m_iHeight;	// ����		- ��(����)��
	float		m_fLength;	// ����		- ��(�ʪ�)��
	float		m_fSpeed;	// ���ǵ�
	float		m_fLimit;	// �����ϴ� - ��������

	D3DCOLOR	m_vColorStart;	// �����ϴ�		- �(�Ϫ�)��몳��
	D3DCOLOR	m_vColorEnd;	// ������,��	- ��(��)���
};

struct RAIN_PROPERTY
{
	DWORD		m_dwFaces;
	int			m_iRadius;	// �� �Ѹ��� �ִ� �ݰ�
	int			m_iHeight;	// ����		- ��(����)��
	float		m_fLength;	// ����		- ��(�ʪ�)��
	float		m_fSpeed;	// ���ǵ�
	float		m_fLimit;	// �����ϴ� - ��������
	D3DCOLOR	m_vColor;

	RAIN_PROPERTY () :
		m_dwFaces(0),
		m_iRadius(0),
		m_iHeight(0),
		m_fLength(0.0f),
		m_fSpeed(0.0f),
		m_fLimit(0.0f),
		m_vColor(0x000000)
	{
	}
};

class DxEffectRain
{
public:
	const static DWORD	VERSION;
protected:
	union
	{
		struct
		{
			RAIN_PROPERTY	m_Property;
		};
		struct
		{
			DWORD		m_dwFaces;
			int			m_iRadius;	// �� �Ѹ��� �ִ� �ݰ�
			int			m_iHeight;	// ����		- ��(����)��
			float		m_fLength;	// ����		- ��(�ʪ�)��
			float		m_fSpeed;	// ���ǵ�
			float		m_fLimit;	// �����ϴ� - ��������
			D3DCOLOR	m_vColor;
		};
	};

public:
	void SetProperty ( RAIN_PROPERTY& Property )
	{
		m_Property = Property;
	}
	RAIN_PROPERTY& GetProperty () { return m_Property; }

protected:
	struct VERTEX 
	{ 
		D3DXVECTOR3 vPos; 
		D3DCOLOR	cColor;
		D3DXVECTOR2 vTex; 
		static const DWORD FVF;
	};

	struct	INFORMATION_RAIN
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vDirect; 
	};

protected:
	LPDIRECT3DVERTEXBUFFERQ m_pVB;
	LPDIRECT3DINDEXBUFFERQ	m_pIB;
	std::string				m_szRainTex;
	LPDIRECT3DTEXTUREQ		m_pTexture;

	INFORMATION_RAIN*		m_pInforRain;

protected:
	BOOL				m_bPrevUse;
	float				m_fTime;
	D3DXVECTOR3			m_vDirection;
	DWORD				m_dwPlayFaces;
	float				m_fApplyRate;

	WEATHERSTATS		m_emRainStats;

protected:
	const	static	float	RAIN_WIDTH_SCALE;
	const	static	float	RAIN_TURN_TIME;	// ������ ���ϴ� �ð�

public:
	HRESULT PlayThunder();

public:
	HRESULT StopSound ();

public:
	void		SetApplyRate ( float fApplyRate )  { m_fApplyRate = fApplyRate; }
	void		SetRainState ( WEATHERSTATS state) { m_emRainStats = state; }	// �� ��� ���� Ȥ �Ұ���
	float		GetApplyRate ()					   { return m_fApplyRate; }
	BOOL		UseRain ()						   { return m_emRainStats; }	// �� ��� ���� Ȥ �Ұ���

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vDirec, float fPower );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );//, float fNowTime, float fFullTime, bool Auto );

public:
	HRESULT ResetMesh ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );

public:
	DxEffectRain();
	virtual ~DxEffectRain();
};



struct	SNOW_PROPERTY_101
{
	DWORD		m_dwVertex;

	int			m_iRadius;	// �� �Ѹ��� �ִ� �ݰ�
	float		m_fLength;	// ����		- ��(�ʪ�)��
	float		m_fLimit;	// �����ϴ� - ��������
	int			m_iHeight;	// ����		- ��(����)��
	float		m_fSpeed;	// ���ǵ�

	D3DCOLOR	m_vColor;	// �����ϴ�		- �(�Ϫ�)��몳��

	char		m_szTexture[MAX_PATH];

	SNOW_PROPERTY_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};

struct	SNOW_PROPERTY
{
	DWORD		m_dwFlags;

	SNOW_PROPERTY () :
		m_dwFlags(NULL)
	{
	}
};

//	Note : �� ���� ȿ�� ����� �� ����Ѵ�.
//
class DxEffectSnow
{
public:
	const static DWORD	VERSION;

protected:	
	union
	{
		struct
		{
			SNOW_PROPERTY	m_Property;
		};
		struct
		{
			DWORD			m_dwFlags;
		};
	};

public:
	void SetProperty ( SNOW_PROPERTY& Property )
	{
		m_Property = Property;
	}
	SNOW_PROPERTY& GetProperty () { return m_Property; }

protected:
	float			m_fTime;
	float			m_fApplyRate;
	WEATHERSTATS	m_emSnowStats;

protected:
	const	static	float	SNOW_TURN_TIME;	// ������ ���ϴ� �ð�

public:
	void		SetApplyRate ( float fApplyRate )  { m_fApplyRate = fApplyRate; }
	void		SetSnowState ( WEATHERSTATS state) { m_emSnowStats = state; }	// �� ��� ���� Ȥ �Ұ���
	float		GetApplyRate ()			{ return m_fApplyRate; }
	BOOL		UseSnow ()				{ return m_emSnowStats; }	// ���� ��� ���� Ȥ �Ұ���

public:
	HRESULT FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse );

public:
	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );

public:
	DxEffectSnow();
	virtual ~DxEffectSnow();
};




//	Note : ���� ȿ�� ����� �� ����Ѵ�.
//
class DxEffectLeaves
{
public:
	const static DWORD	VERSION;

protected:
	float			m_fTime;
	float			m_fApplyRate;
	WEATHERSTATS	m_emLeavesStats;

protected:
	const	static	float	LEAVES_TURN_TIME;	// ������ ���ϴ� �ð�

public:
	void		SetApplyRate ( float fApplyRate )    { m_fApplyRate = fApplyRate; }
	void		SetLeavesState ( WEATHERSTATS state) { m_emLeavesStats = state; }	// �� ��� ���� Ȥ �Ұ���
	float		GetApplyRate ()			{ return m_fApplyRate; }
	BOOL		UseLeaves ()			{ return m_emLeavesStats; }	// ���� ��� ���� Ȥ �Ұ���

public:
	HRESULT FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse );

public:
	DxEffectLeaves();
	virtual ~DxEffectLeaves();
};





struct	CLOUD_PROPERTY
{
	bool			m_bUse;
	float			m_fWidth;
	char			m_szTexture[MAX_PATH];

	CLOUD_PROPERTY () :
		m_bUse(false),
		m_fWidth(0.0f)
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};

//	Note : ���� ���� ȿ�� ����� �� ����Ѵ�.	-	���� ��.~!!
//
class DxEffectCloud
{
public:
	const static DWORD	VERSION;

protected:	
	union
	{
		struct
		{
			CLOUD_PROPERTY	m_Property;
		};
		struct
		{
			bool			m_bUse;
			float			m_fWidth;
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( CLOUD_PROPERTY& Property )
	{
		m_Property = Property;
	}
	CLOUD_PROPERTY& GetProperty () { return m_Property; }

protected:	

	struct CLOUDSHADOW
	{
		D3DXVECTOR3		vPos;
		D3DCOLOR		vColor;
		D3DXVECTOR2		vTex;
		static const DWORD	FVF;
	};

	CLOUDSHADOW		m_CloudShadow[4];
	D3DXVECTOR2		m_vCumulateTex[4];

	LPDIRECT3DTEXTUREQ	m_pddsTexture;


protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudSB;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectCloudSB;

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fElapsedTime );	
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );

public:
	DxEffectCloud();
	virtual ~DxEffectCloud();
};



struct	WEATHER_PROPERTY_101
{
	bool				m_bActive;		// Ȱ��ȭ ����.

	bool				m_bUse;			// �ʱ� ����.
	bool				m_bRain;		// ��==true, ��==false

	D3DXVECTOR3			m_vWindDirec;	// �ٶ� ����
	float				m_fWindPower;	// �ٶ� ����

	float				m_fClear;		// ���� �ð�
	float				m_fRainy;		// �帰 �ð�
};

struct WEATHER_PROPERTY
{
	DWORD				m_dwFlag;

	D3DXVECTOR3			m_vWindDirec;	// �ٶ� ����
	float				m_fWindPower;	// �ٶ� ����

	WEATHER_PROPERTY () :
		m_vWindDirec( 1.f, 0.f, 1.f ),
		m_fWindPower(2.f)
	{
		m_dwFlag = 0L;
	}
};

class DxWeatherMan
{
public:
	const static DWORD	VERSION;

	void SetProperty ( WEATHER_PROPERTY& Property )
	{
		m_Property = Property;
	}
	WEATHER_PROPERTY& GetProperty () { return m_Property; }

protected:
	union
	{
		struct
		{
			WEATHER_PROPERTY	m_Property;
		};
		struct
		{
			DWORD				m_dwFlag;

			D3DXVECTOR3			m_vWindDirec;	// �ٶ� ����
			float				m_fWindPower;	// �ٶ� ����
		};
	};

protected:
	float			m_fTime;			// ���� �ð�
	float			m_fElapsedTime;

	bool			m_bWhimsical;		// �������� ����.

	DxEffectRain	m_sRain;
	DxEffectSnow	m_sSnow;
	DxEffectLeaves	m_sLeaves;
	DxEffectCloud	m_sCloud;

	WEATHERSTATS	m_emWeatherStats;	// �� ���� ����
	float			m_fApplyRate;		// ����� ����

	SNATIVEID		m_currentMapID;

public:

	D3DXVECTOR3*	GetWindDirec ()						{ return &m_vWindDirec; }
	float			GetWindPower ()						{ return m_fWindPower; }
	float			SetWindPower ( float fPower)		{ return m_fWindPower = fPower; }

	DxEffectRain*	GetRain ()			{ return &m_sRain; }
	DxEffectSnow*	GetSnow ()			{ return &m_sSnow; }
	DxEffectLeaves*	GetLeaves ()		{ return &m_sLeaves; }
	DxEffectCloud*	GetCloud ()			{ return &m_sCloud; }

	void			SetMapID( SNATIVEID mapID ) { m_currentMapID = mapID; }

public:
	void ReSetSound ();
	void ReceiveWhimsical ( DWORD dwWhimsical );
	void ReceiveMapWhimsical ( WORD map_Mid, WORD map_Sid, DWORD dwWhimsical  );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT Render_Prev ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static DWORD LoadSetProp ( CSerialFile &SFile, WEATHER_PROPERTY &sProperty );

public:
	void SaveSet ( CSerialFile &SFile );
	void LoadSet ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void CloneInstance ( DxWeatherMan* pWeatherMan, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxWeatherMan(void);
	~DxWeatherMan(void);

protected:
	static DxWeatherMan	InstanceDefault;
	static DxWeatherMan*	pInstance;

public:
	static DxWeatherMan* GetInstance ()
	{
		if ( pInstance )	return pInstance;
		return &InstanceDefault;
	}

	static void SetInstance ( DxWeatherMan* pWeatherMan )
	{
		if ( pWeatherMan==NULL )	pInstance = NULL;
		else						pInstance = pWeatherMan;
	}
};

#endif // !define(_DXWEATHERMAN_H__INCLUDED_)