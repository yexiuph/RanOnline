#pragma once

#include "DxVertexFVF.h"
#include "./GLDefine.h"

#include "DxSkyBase.h"
#include "DxSkyCloud.h"

struct SKY_PROPERTY_100
{
	BOOL			m_bSkyEnable;
};

struct SKY_PROPERTY_101
{
	BOOL			m_bSkyEnable;		// 해, 달, 별, 유성
	BOOL			m_bCloudEnable;		// 구름, 번개
};

struct SKY_PROPERTY
{
	enum { VERSION = 0x0102, };

	BOOL			m_bSkyEnable;		// 해, 달, 별, 유성
	BOOL			m_bCloudEnable;		// 구름, 번개
	int				m_nRadioAxis;		// X, Y, Z 축
	float			m_fAxisValue;		// 축으로의 회전 값

	SKY_PROPERTY () :
		m_bSkyEnable(FALSE),
		m_bCloudEnable(FALSE),
		m_nRadioAxis(0),
		m_fAxisValue(0.f)
	{
	}

	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );
};

class	CCloud
{
protected:
	struct	VERTEXCOLORTEX
	{
		D3DXVECTOR3			vPos;
		D3DCOLOR			cColor;
		D3DXVECTOR2			vTex;
		static const DWORD	FVF;
	};

	struct	CLOUD_OPTION
	{
		float				fScale;
		D3DXVECTOR2			vWind;
		D3DXVECTOR2			vAddMove;

		CLOUD_OPTION() :
			fScale(0.f),
			vWind(0.f,0.f),
			vAddMove(0.f,0.f)
		{
		}
	};

	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudBaseSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawCloudBaseSB;

public:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawCloudSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudShadowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawCloudShadowSB;
	
protected:
	char					m_szCloudTex[MAX_PATH];
	char					m_szCloudShadowTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pCloudTex;
	LPDIRECT3DTEXTUREQ		m_pCloudShadowTex;

	LPDIRECT3DVERTEXBUFFERQ	m_pCloudVB;
	LPDIRECT3DINDEXBUFFERQ	m_pCloudIB;

	DWORD					m_dwVertexNum;	// 12
	DWORD					m_dwFacesNum;	// 14

	CLOUD_OPTION			m_sCloud_1;			// 1번째 구름
	CLOUD_OPTION			m_sCloud_2;			// 2번째 구름
	CLOUD_OPTION			m_sCloud_3;			// 1번째 구름
	CLOUD_OPTION			m_sCloud_4;			// 2번째 구름

	D3DXVECTOR2*			m_pSrcTex;
	D3DXVECTOR2*			m_pSrcTex_45;

protected:
	float			m_fElapsedTime;

	D3DXVECTOR3		m_vSkyCenter;
	float			m_fWeatherRate;

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect=FALSE );

public:
	void	SetData ( float fElapsedTime, D3DXVECTOR3 vSkyCenter, float fWeatherRate );
	DWORD	GetSkyColor ();	

protected:
	void	CloudRender_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud );
	
	void	CloudRender_Base ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud );
	void	CloudRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, D3DXVECTOR2* pTex );
	void	CloudShadowRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, D3DXVECTOR2* pTex, DWORD dwColor );
	void	CloudyRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, CLOUD_OPTION& sCloud, DWORD dwColor );
	DWORD	GetCloudyColor ();

public:
	CCloud();
	~CCloud();
};

class DxSkyMan
{
protected:
	DxSkyBaseMan	m_sSkyBaseMan;
	DxSkyCloudMan	m_sSkyCloudMan;

public:
	void SetProperty ( SKY_PROPERTY& Property );
	SKY_PROPERTY& GetProperty () { return m_Property; }

protected:
	union
	{
		struct
		{
			SKY_PROPERTY	m_Property;
		};

		struct
		{
			BOOL			m_bSkyEnable;
			BOOL			m_bCloudEnable;
			int				m_nRadioAxis;
			float			m_fAxisValue;
		};
	};

protected:
	struct	STAR_OPTION
	{
		D3DXVECTOR3		vPos;
		float			fTwinkle;
		D3DCOLOR		cColor;

		STAR_OPTION() :
			vPos(0.f,0.f,0.f),
			fTwinkle(1.f),
			cColor(0xffffffff)
		{
		}
	};

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSavedSunSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawSunSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedFlareSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawFlareSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedNightSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawNightSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedMoonSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawMoonSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedUStarSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawUStarSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedThunderSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawThunderSB;

	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawCloudSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSavedCloudShadowSB;
	static LPDIRECT3DSTATEBLOCK9	m_pDrawCloudShadowSB;

protected:
	BOOL			m_bOptionEnable;		// 옵션에서의 활성화

public:
	void	SetOptionEnable ( BOOL bUse )	{ m_bOptionEnable=bUse; }
	BOOL	IsOptionEnable ()				{ return m_bOptionEnable; }

protected:
	float			m_fElapsedTime;

	D3DXVECTOR3		m_vSkyCenter;
	float			m_fWeatherRate;

public:
	static D3DXMATRIX	m_matWorld;

protected:
	DWORD			m_dwModeWidth;
	DWORD			m_dwModeHeight;

	VERTEXCOLORTEX		m_sVertices[4];
	VERTEXCOLORTEX2	m_sVerticesTex2[4];

	float			m_fAlpha_Day;			// 낮에 뿌리는 모든 물체의 알파
	float			m_fAlpha_Night;			// 저녁에 뿌리는 모든 물체의 알파

protected:
	char					m_szSunTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pSunTex;

	D3DXVECTOR3				m_vSunPos;
	float					m_fSunScale;			// 태양의 크기

protected:
	char					m_szFlareTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pFlareTex;

	VERTEXCOLORRHW			m_sFlare[4];

protected:
	char					m_szMoonTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pMoonTex;

	enum	MOONCYCLE
	{
		EMMC_L_ZERO		= 0,
		EMMC_L_SMALL	= 1,
		EMMC_L_HALF		= 2,
		EMMC_L_BIG		= 3,
		EMMC_CIRCLE		= 4,
		EMMC_R_BIG		= 5,
		EMMC_R_HALF		= 6,
		EMMC_R_SMALL	= 7,
	};
	MOONCYCLE				m_emMoonCycle;
	DWORD					m_dwPrevHour;

	float					m_fMoonScale;			// 달의 크기

protected:
	char					m_szStarTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pStarTex;

	LPDIRECT3DVERTEXBUFFERQ	m_pStarVB;
	LPDIRECT3DINDEXBUFFERQ	m_pStarIB;

	enum { STAR_NUM = 100, };
	float					m_fStarScale;
	float					m_fStarTwinkleTime;

	STAR_OPTION				m_sStarObject [ STAR_NUM ];

protected:
	char					m_szUStarTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pUStarTex;

	float					m_fUStarScale;			// 스케일
	float					m_fUStarRate;			// Width와 Height 의 비율
	D3DXVECTOR3				m_vUStarDirect;			// 방향
	float					m_fUStarSpeed;			// 속도
	float					m_fUStarTime;			// 시간
	float					m_fUStarTimeEnd;		// 끝 시간
	float					m_fUStarTimeRest;		// 쉬는 시간

	enum { USTAR_NUM = 20, };
	D3DXVECTOR3				m_vUStarPos[USTAR_NUM];		// 위치		// 순식간에 유성이 20개 떨어진다. 캬캬캬.

	VERTEXCOLORTEX				m_sUStar[4];

protected:
	char					m_szLightingTex[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pLightingTex;

	BOOL					m_bLightingNow;
	BOOL					m_bLightingUSE;			// 사용중이다.
	int						m_nLightningNUM;
	
	float					m_fLightingTime;		// 타임
	float					m_fLightingEndTime;		// 끝 타임

	D3DXVECTOR3				m_vLightingPos;
	D3DCOLOR				m_cLightingColor;

protected:
	char					m_szThunderTex[MAX_PATH];	// 깜박이는 것
	char					m_szThunderTex_C[MAX_PATH];
	LPDIRECT3DTEXTUREQ		m_pThunderTex;
	LPDIRECT3DTEXTUREQ		m_pThunderTex_C;

	D3DXVECTOR3				m_vThunderPos;		// 위치
	D3DXVECTOR2				m_vThunderUV;		// UV
	D3DCOLOR				m_cThunderColor;	// 컬러
	float					m_fThunderScale;	// 스케일

	float					m_fThunderTime;		// 타임
	float					m_fThunderPlayTime;	// 시작 타임
	float					m_fThunderEndTime;	// 끝 타임

protected:
	char					m_szThundersTex[MAX_PATH];	// Lighting 쪽
	LPDIRECT3DTEXTUREQ		m_pThundersTex;

	enum	LIGHTINGTEXUV
	{
		EMLT_UV0	= 0,	// 뿌림
		EMLT_UV1	= 1,
		EMLT_UV2	= 2,
		EMLT_UV3	= 3,
		EMLT_NONE0	= 4,	// 안뿌림
		EMLT_NONE1	= 5,
		EMLT_NONE2	= 6,
		EMLT_NONE3	= 7,
		EMLT_MAX	= 8,
	};

	struct	LIGHTING_OPTION
	{
		D3DXVECTOR3		vPos;
		float			fTime;
		int				m_iSprite;
		LIGHTINGTEXUV	emTex;

		LIGHTING_OPTION() :
			vPos(0.f,0.f,0.f),
			fTime(0.f),
			m_iSprite(0),
			emTex(EMLT_NONE0)
		{
		}
	};

	LIGHTING_OPTION		m_sLighting_1;
	LIGHTING_OPTION		m_sLighting_2;
	LIGHTING_OPTION		m_sLighting_3;

public:
	void	SetThunderON ()		{ m_bLightingNow = TRUE; }

public:
	D3DXVECTOR3& GetSunPos()	{ return m_vSunPos; }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bReflect=FALSE );
	void Render_AFTER( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderGlOW( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	void	LightingCheckReset ();
	void	LightingRender ( LPDIRECT3DDEVICEQ pd3dDevice );
	void	ThunderRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vPos, float fScale, float fBase, float fFact );
	void	ThundersRender ( LPDIRECT3DDEVICEQ pd3dDevice, LIGHTING_OPTION vLighting );
																
	void	MoonRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct=FALSE );					// 3	// 밤에 나옴
	void	StarRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct=FALSE );					// 1
	void	UStarRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct=FALSE );					// 2

	void	SunRender ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bRefelct=FALSE );
	void	FlareRender ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vSunPos );
	void	FlareRender_Part ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwWidth, DWORD dwHeight, float fScale, int nTex );

protected:
	DxSkyMan();

public:
	virtual ~DxSkyMan();

public:
	static DxSkyMan& GetInstance();
};