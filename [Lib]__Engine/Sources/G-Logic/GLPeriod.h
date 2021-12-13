#pragma once

#include <map>

const float fCLOUDY_BLENDFACT = 0.6f;

struct SPRERIODTIME
{
	DWORD dwYear;
	DWORD dwYearToday;
	DWORD dwHour;
	float fSec;

	SPRERIODTIME () :
		dwYear(0),
		dwYearToday(0),
		dwHour(0),
		fSec(0)
	{
	}
	SPRERIODTIME ( DWORD _dwyear, DWORD _dwyeartoday, DWORD _dwHour, float _fsec ) :
		dwYear(_dwyear),
		dwYearToday(_dwyeartoday),
		dwHour(_dwHour),
		fSec(_fsec)
	{
	}
};

struct SONEMAPWEATHER 
{
	DWORD dwWeather;
	DWORD dwApplyTime;
	WORD  map_mID;
	WORD  map_sID;
	CTime startTime;
	DWORD dwWhimsicalWeather;
	float fThunderTimer;


	SONEMAPWEATHER() :
		dwWeather(0),
		dwApplyTime(0),
		map_mID(0),
		map_sID(0),
		dwWhimsicalWeather(0),
		fThunderTimer(0)
	{
	}

	BOOL operator != ( CONST SONEMAPWEATHER& ) const;
};

void GETSEEDGAMETIME ( const DWORD dwTIMESCALE, SPRERIODTIME &_cPRERIODTIME );

enum FGWEATHER
{
	FGW_RAIN		= 0x0001,
	FGW_SNOW		= 0x0002,

	FGW_LEAVES		= 0x0004,
	FGW_SPORE		= 0x0008,

	FGW_ALL			= ( FGW_RAIN | FGW_SNOW | FGW_LEAVES | FGW_SPORE ),

	FGW_THUNDER		= 0x0010,
	FGW_WHIMSICAL	= FGW_THUNDER,	//	일시적 자연효과 합산 플래그.
};

enum FGTIME
{
	FGT_DAWN = 0,
	FGT_DAYTIME = 1,
	FGT_SUNSET = 2,
	FGT_NIGHT = 3,
};

class GLPeriod
{
public:
	enum
	{
		YEAR_MONTH		= 12,
		YEAR_DAY		= 365,
		DAY_HOUR		= 24,
		TIME_SEC		= 3600,

		REALTIME_TO_VBR	= 12,

		WEATHER_TIMESET	= 1,

		WINTER_NIGHT_S	= 19,
		WINTER_NIGHT_E	= 7,
		SUMMMER_NIGHT_S	= 20,
		SUMMMER_NIGHT_E	= 6,

		WINTER_S		= 11,
		WINTER			= 1,
		WINTER_E		= 3,

		SPRING_S		= 3,
		SPRING			= 2,
		SPRING_E		= 7,

		SUMMER_S		= 7,
		SUMMER			= 3,
		SUMMER_E		= 9,

		AUTUMN_S		= 9,
		AUTUMN			= 4,
		AUTUMN_E		= 11,
	};

	enum
	{
		MTH_1S		= 1,
		MTH_1		= 31,
		MTH_1E		= 31,

		MTH_2S		= MTH_1E+1,
		MTH_2		= 28,
		MTH_2E		= MTH_1E+MTH_2,

		MTH_3S		= MTH_2E+1,
		MTH_3		= 29,
		MTH_3E		= MTH_2E+MTH_3,

		MTH_4S		= MTH_3E+1,
		MTH_4		= 30,
		MTH_4E		= MTH_3E+MTH_4,

		MTH_5S		= MTH_4E+1,
		MTH_5		= 31,
		MTH_5E		= MTH_4E+MTH_5,

		MTH_6S		= MTH_5E+1,
		MTH_6		= 30,
		MTH_6E		= MTH_5E+MTH_6,

		MTH_7S		= MTH_6E+1,
		MTH_7		= 31,
		MTH_7E		= MTH_6E+MTH_7,

		MTH_8S		= MTH_7E+1,
		MTH_8		= 31,
		MTH_8E		= MTH_7E+MTH_8,

		MTH_9S		= MTH_8E+1,
		MTH_9		= 30,
		MTH_9E		= MTH_8E+MTH_7,

		MTH_10S		= MTH_9E+1,
		MTH_10		= 31,
		MTH_10E		= MTH_9E+MTH_10,

		MTH_11S		= MTH_10E+1,
		MTH_11		= 30,
		MTH_11E		= MTH_10E+MTH_11,

		MTH_12S		= MTH_11E+1,
		MTH_12		= 31,
		MTH_12E		= MTH_11E+MTH_12+1,
	};

	struct SMTH
	{
		DWORD wMonthS;
		DWORD wMonth;
		DWORD wMonthE;

		SMTH () {}
		SMTH ( DWORD _ms, DWORD _m, DWORD _me ) :
			wMonthS(_ms),
			wMonth(_m),
			wMonthE(_me)
		{
		}
	};

	typedef std::map<DWORD,SMTH>	MTHARRAY;
	typedef MTHARRAY::iterator		MTHARRAY_ITER;

protected:
	MTHARRAY	m_MTH_LMT;
	DWORD		m_dwRAINRATE[YEAR_MONTH+1];
	DWORD		m_dwSNOWRATE[YEAR_MONTH+1];
	DWORD		m_dwSPORERATE[YEAR_MONTH+1];
	DWORD		m_dwLEAVESRATE[YEAR_MONTH+1];

protected:
	float	m_fTIMETOVBR;

	DWORD	m_dwThisYear;
	DWORD	m_dwThisMonth;
	DWORD	m_dwMonth_Today;
	DWORD	m_dwYear_Today;

	BOOL	m_bNight;
	DWORD	m_dwHour;
	float	m_fSecond;

	DWORD	m_dwRainRate;
	DWORD	m_dwSnowRate;
	DWORD	m_dwSporeRate;
	DWORD	m_dwLeavesRate;

	bool	m_bActiveWeather;
	DWORD	m_dwWeather;
	DWORD	m_dwWhimsicalWeather;

	float	m_fThunderTimer;
	float	m_fWeatherTimer;

	float	m_fBaseFact;		//	낮, 밤 섞이는 강도. 기본
	float	m_fBlendFact;		//	낮, 밤 섞이는 강도. 기본 + 날씨
	float	m_fNightFact;		// 밤.. Blend까지 들어간 것.
	float	m_fBlueFact;		//	새벽녁의 강도
	float	m_fRedFact;			//	저녁무렵의 노을 파워
	DWORD	m_dw4Season;		//	현제의 사계.
	BOOL	m_bSummerSeason;	//	하계인지?

	std::vector<SONEMAPWEATHER> m_vecOneMapWeather;

public:
	FGTIME	m_sFGTime;	// 새벽, 낮, 해질녘, 저녁
	float GetBaseFact()			{ return m_fBaseFact; }
	float GetBlendFact()		{ return m_fBlendFact; }
	float GetNightFact()		{ return m_fNightFact; }
	float GetBlueFact()			{ return m_fBlueFact; }
	float GetRedFact()			{ return m_fRedFact; }

public:
	void SetPeriod ( DWORD dwYear, DWORD dwYearToday, DWORD dwHour, float fSec );
	void SetPeriod ( const SPRERIODTIME &cTIME );
	SPRERIODTIME GetPeriod ();

	void SetTIMETOVBR ( float _vbr )		{ m_fTIMETOVBR = _vbr; }
	void SetHour ( DWORD dwHour )			{ m_dwHour = dwHour; }
	
	void TestToNight ( float _fvbr );
	void TestToNoon ( float _fvbr );

	void SetActiveWeather ( bool bActive )	{ m_bActiveWeather = bActive; }
	void SetWeather ( DWORD _dwWeather );

	void ResetOneMapWeather() { m_vecOneMapWeather.clear(); }
	void SetOneMapActiveWeather( SONEMAPWEATHER oneMapWeather, bool bServer = TRUE );

protected:
	DWORD SetThisMonth ();
	DWORD SetToday ();

public:
	float GetTIMETOVBR()		{ return m_fTIMETOVBR; }
	DWORD GetYear()				{ return m_dwThisYear; }
	DWORD GetYearToday()		{ return m_dwYear_Today; }
	DWORD GetMonth()			{ return m_dwThisMonth; }
	DWORD GetMonthToday()		{ return m_dwMonth_Today; }
	DWORD GetHour()				{ return m_dwHour; }
	float GetSecond()			{ return m_fSecond; }
	DWORD GetWeather()			{ return m_dwWeather; }
	DWORD GetMapWeather( WORD mID, WORD sID);
	DWORD GetWhimsicalWeather()	{ return m_dwWhimsicalWeather; }
	std::vector<SONEMAPWEATHER> GetOneMapWeather() { return m_vecOneMapWeather; }

	DWORD GetFadeInTime ()	//	밝아지는 시간.
	{
		if ( m_bSummerSeason )	return DWORD(WINTER_NIGHT_E-1);
		else					return DWORD(SUMMMER_NIGHT_E-1);
	}

	DWORD GetFadeOutTime ()	//	어두워지는 시간.
	{
		if ( m_bSummerSeason )	return WINTER_NIGHT_S;
		else					return SUMMMER_NIGHT_S;
	}

	DWORD GetBlueTime ()	//	새벽시간대.
	{
		if ( m_bSummerSeason )	return WINTER_NIGHT_E;
		else					return SUMMMER_NIGHT_E;
	}

	DWORD GetRedTime ()		//	노을이 지는 시간대.
	{
		if ( m_bSummerSeason )	return DWORD(WINTER_NIGHT_S-1);
		else					return DWORD(SUMMMER_NIGHT_S-1);
	}

	float		GetDirectionFact ();
	D3DXVECTOR2 GetDirectFact_XY ();

public:
	//	낮인가?
	BOOL IsNoon ()
	{
		if ( m_bSummerSeason )	return ( (WINTER_NIGHT_E-1) <= m_dwHour && m_dwHour < WINTER_NIGHT_S );
		else					return ( (SUMMMER_NIGHT_E-1) <= m_dwHour && m_dwHour < SUMMMER_NIGHT_S );
	}

	//	가로등 꺼져있는 시간인가?
	BOOL IsOffLight ()
	{
		if ( IsNoon() && !IsFadeInTime() && !IsFadeOutTime() )	return TRUE;
		else if ( m_fBlendFact > 0.5f )	return TRUE;
		return FALSE;
	}

	//	밝아지는 시간.
	BOOL IsFadeInTime ()		{	return GetFadeInTime()==m_dwHour; }

	//	어두워지는 시간.
	BOOL IsFadeOutTime ()		{	return GetFadeOutTime()==m_dwHour; }

	//	새벽시간대.
	BOOL IsBlueTime ()			{	return GetBlueTime()==m_dwHour; }

	//	노을이 지는 시간대.
	BOOL IsRedTime ()			{	return GetRedTime()==m_dwHour; }

public:
	HRESULT UpdateWeather ( float fElapsedTime );
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	void LoadFile ( const char* szFile );

protected:
	GLPeriod(void);

public:
	~GLPeriod(void);

public:
	static GLPeriod& GetInstance();
};
