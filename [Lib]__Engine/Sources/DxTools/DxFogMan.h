#pragma once

struct FOG_PROPERTY_100_2
{
	BOOL			m_bFogEnable;
	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart;
	float			m_fFogEnd;
	float			m_fFogDensity;
};

struct FOG_PROPERTY_103_4					// Ver. 103, 104
{
	BOOL			m_bFogEnable;
	BOOL			m_bFogDayNight;
	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart;
	float			m_fFogEnd;
	float			m_fFogDensity;
	D3DCOLOR		m_cFogColor_Night;
	float			m_fFogStart_Night;
	float			m_fFogEnd_Night;
	float			m_fFogDensity_Night;
};

struct FOG_PROPERTY_105
{
	BOOL			m_bSkyFogEnable;

	D3DCOLOR		m_cFogColor;
	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor_Day;
	float			m_fFogStart_Day;
	float			m_fFogEnd_Day;
	D3DCOLOR		m_cFogColor_Night;
	float			m_fFogStart_Night;
	float			m_fFogEnd_Night;
};

struct FOG_PROPERTY_106
{
	BOOL			m_bSkyFogEnable;
	BOOL			m_bFogStaticRange;	// 정적 가시거리 활성화

	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;
};

struct FOG_PROPERTY
{
	enum { VERSION = 0x0108, };

	BOOL			m_bNewRange;		// 107 부터는 최대가시거리를 늘인다.
	BOOL			m_bSkyFogEnable;
	BOOL			m_bFogStaticRange;	// 정적 가시거리 활성화

	float			m_fFogStart;
	float			m_fFogEnd;

	D3DCOLOR		m_cFogColor;
	D3DCOLOR		m_cFogColor_Day;
	D3DCOLOR		m_cFogColor_Night;

	FOG_PROPERTY () :
		m_bNewRange(FALSE),
		m_bSkyFogEnable(FALSE),
		m_bFogStaticRange(FALSE),
		m_fFogStart(720.0f),
		m_fFogEnd(790.0f),
		m_cFogColor(D3DCOLOR_XRGB(0,0,0)),
		m_cFogColor_Day(D3DCOLOR_XRGB(89,135,179)),
		m_cFogColor_Night(D3DCOLOR_XRGB(60,60,94))
	{
	}

	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );
};

enum FOGRANGE
{
	EMFR_NSIZE	= 3,

	EMFR_LOW	= 0,
	EMFR_MIDDLE = 1,
	EMFR_HIGH	= 2,
};

class DxFogMan
{
public:
	void SetProperty ( FOG_PROPERTY& Property );
	FOG_PROPERTY& GetProperty () { return m_Property; }

protected:
	union
	{
		struct
		{
			FOG_PROPERTY	m_Property;
		};
		struct
		{
			BOOL			m_bNewRange;
			BOOL			m_bSkyFogEnable;
			BOOL			m_bFogStaticRange;	// 가시거리 활성화

			float			m_fFogStart;
			float			m_fFogEnd;

			D3DCOLOR		m_cFogColor;
			D3DCOLOR		m_cFogColor_Day;
			D3DCOLOR		m_cFogColor_Night;
		};
	};

protected:
	DWORD		m_dwSavedSB;
	DWORD		m_dwEffectSB;

	D3DCOLOR	m_cDayColor;
	D3DCOLOR	m_cFogColor_Game;
	D3DCOLOR	m_cFogColor_White;
	D3DCOLOR	m_cFogColor_Dark;
	D3DCOLOR	m_cFogColor_Object;
	BOOL		m_bFrameMode_Fog;

protected:
	float		m_fRangeLow_Near;
	float		m_fRangeLow_Far;
	float		m_fRangeMiddle_Near;
	float		m_fRangeMiddle_Far;
	float		m_fRangeHigh_Near;
	float		m_fRangeHigh_Far;

protected:
	BOOL		m_bFogEnable;

protected:
	FOGRANGE			m_emFogRange;		// 포그 범위에 대한 옵션

public:
	void		RenderFogSB ( LPDIRECT3DDEVICEQ pd3dDevice );

	D3DCOLOR	GetFogWhiteColor ()	{ return m_cFogColor_White; }
	D3DCOLOR	GetFogDarkColor ()	{ return m_cFogColor_Dark; }
	D3DCOLOR	GetFogColor ()		{ return m_cFogColor_Game; }
	D3DCOLOR	GetDayColor ()		{ return m_cDayColor; }

	void		SetFrameMode_Fog ( BOOL bUse )	{ m_bFrameMode_Fog = bUse; }
	BOOL		GetFrameMode_Fog ()				{ return m_bFrameMode_Fog; }

	FOGRANGE	GetFogRange ()						{ return m_emFogRange; }
	void		SetFogRange ( FOGRANGE emFogRange )	{ m_emFogRange = emFogRange; }	// 시야 셋팅

	float		GetFogRangeFar();

public:
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	DxFogMan();

public:
	virtual ~DxFogMan();

public:
	static DxFogMan& GetInstance();
};
