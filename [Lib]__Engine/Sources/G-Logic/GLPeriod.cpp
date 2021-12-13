#include "pch.h"
#include "GLDefine.h"
#include "glperiod.h"
#include "./DxWeatherMan.h"

#include "GLOGIC.h"
#include "./IniLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GETSEEDGAMETIME ( const DWORD dwTIMESCALE, SPRERIODTIME &_cPRERIODTIME )
{
	//	Note : 기준 시간 설정.
	//
	SYSTEMTIME _seedtime;
	memset(&_seedtime,0,sizeof(SYSTEMTIME));
	_seedtime.wYear = 2003;
	_seedtime.wMonth = 10;
	_seedtime.wDay = 12;
	_seedtime.wHour = 12;
	_seedtime.wMinute = 0;
	_seedtime.wSecond = 0;

	CTime cSEEDTIME(_seedtime);
	CTime cCURTIME= CTime::GetCurrentTime();

	if ( cSEEDTIME>cCURTIME )
	{
		MessageBox ( NULL, "system time too old. \ncheck system time.", "ERROR", MB_OK );
	}

	//	Note : 기점 시간과의 실제 경과 시간 산출.
	//
	CTimeSpan cDXTIME = cCURTIME-cSEEDTIME;

	LONGLONG lnSECOND = cDXTIME.GetTotalSeconds();
	LONGLONG lnRESECOND = lnSECOND % (24*3600);
	LONGLONG lnGRESECOND = lnRESECOND*dwTIMESCALE;

	LONGLONG lnDXDAY = cDXTIME.GetDays();
	LONGLONG lnREDXDAY = (lnDXDAY*dwTIMESCALE);

	LONGLONG lnGAMESEC  = (lnGRESECOND) % 60;
	LONGLONG lnGAMEMIN  = (lnGRESECOND/60) % 60;
	LONGLONG lnGAMEHOUR = (lnGRESECOND/3600) % 24;
	LONGLONG lnGAMEDAY  = ( lnGRESECOND/(3600*24) + lnREDXDAY ) % 365;
	LONGLONG lnGAMEYEAR = ( lnGRESECOND/(3600*24) + lnREDXDAY ) / 365;

	//	Note : 게임 시간 반환.
	//
	_cPRERIODTIME.dwYear = (DWORD) lnGAMEYEAR;
	_cPRERIODTIME.dwYearToday = (DWORD) lnGAMEDAY;
	_cPRERIODTIME.dwHour = (DWORD) lnGAMEHOUR;
	_cPRERIODTIME.fSec = (float) ( lnGAMESEC + lnGAMEMIN*60 );

	return;
}

BOOL SONEMAPWEATHER::operator != ( CONST SONEMAPWEATHER& value ) const
{
	if( dwWeather   != value.dwWeather ||
		dwApplyTime != value.dwApplyTime ||
		map_mID     != value.map_mID ||
		map_sID     != value.map_sID ||
		startTime   != value.startTime )
	{
		return FALSE;
	}
	return TRUE;

}


//--------------------------------------------------------------------------------------------------
//
//
GLPeriod& GLPeriod::GetInstance()
{
	static GLPeriod Instance;
	return Instance;
}

GLPeriod::GLPeriod(void) :
	m_fTIMETOVBR(REALTIME_TO_VBR),

	m_dwThisYear(2003),
	m_dwThisMonth(10),
	m_dwMonth_Today(21),
	m_dwYear_Today(MTH_5E+22),
	
	m_bNight(FALSE),
	m_dwHour(14),
	m_fSecond(0.0f),

	m_dwRainRate(0),
	m_dwSnowRate(0),
	m_dwSporeRate(0),
	m_dwLeavesRate(0),

	m_bActiveWeather(true),
	m_dwWeather(NULL),
	m_fWeatherTimer(-1.0f),

	m_sFGTime(FGT_DAYTIME),
	m_fBaseFact(0.0f),
	m_fBlendFact(0.0f),
	m_fNightFact(0.0f),
	m_fBlueFact(0.0f),
	m_fRedFact(0.0f),
	m_dw4Season(SPRING),
	m_bSummerSeason(TRUE)
{
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_1E,SMTH(MTH_1S,1,MTH_1E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_2E,SMTH(MTH_2S,2,MTH_2E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_3E,SMTH(MTH_3S,3,MTH_3E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_4E,SMTH(MTH_4S,4,MTH_4E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_5E,SMTH(MTH_5S,5,MTH_5E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_6E,SMTH(MTH_6S,6,MTH_6E)));

	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_7E,SMTH(MTH_7S,7,MTH_7E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_8E,SMTH(MTH_8S,8,MTH_8E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_9E,SMTH(MTH_9S,9,MTH_9E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_10E,SMTH(MTH_10S,10,MTH_10E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_11E,SMTH(MTH_11S,11,MTH_11E)));
	m_MTH_LMT.insert(std::make_pair((DWORD)MTH_12E,SMTH(MTH_12S,12,MTH_12E)));

	m_dwRAINRATE[0] = 0;
	m_dwRAINRATE[1] = 5;
	m_dwRAINRATE[2] = 10;
	m_dwRAINRATE[3] = 20;
	m_dwRAINRATE[4] = 30;
	m_dwRAINRATE[5] = 30;
	m_dwRAINRATE[6] = 35;
	m_dwRAINRATE[7] = 35;
	m_dwRAINRATE[8] = 30;
	m_dwRAINRATE[9] = 25;
	m_dwRAINRATE[10] = 25;
	m_dwRAINRATE[11] = 10;
	m_dwRAINRATE[12] = 5;

	m_dwSNOWRATE[0] = 0;
	m_dwSNOWRATE[1] = 35;
	m_dwSNOWRATE[2] = 0;
	m_dwSNOWRATE[3] = 0;
	m_dwSNOWRATE[4] = 0;
	m_dwSNOWRATE[5] = 0;
	m_dwSNOWRATE[6] = 0;
	m_dwSNOWRATE[7] = 0;
	m_dwSNOWRATE[8] = 0;
	m_dwSNOWRATE[9] = 0;
	m_dwSNOWRATE[10] = 0;
	m_dwSNOWRATE[11] = 0;
	m_dwSNOWRATE[12] = 35;

	m_dwSPORERATE[0] = 0;
	m_dwSPORERATE[1] = 0;
	m_dwSPORERATE[2] = 5;
	m_dwSPORERATE[3] = 15;
	m_dwSPORERATE[4] = 35;
	m_dwSPORERATE[5] = 15;
	m_dwSPORERATE[6] = 0;
	m_dwSPORERATE[7] = 0;
	m_dwSPORERATE[8] = 0;
	m_dwSPORERATE[9] = 0;
	m_dwSPORERATE[10] = 0;
	m_dwSPORERATE[11] = 0;
	m_dwSPORERATE[12] = 0;

	m_dwLEAVESRATE[0] = 0;
	m_dwLEAVESRATE[1] = 0;
	m_dwLEAVESRATE[2] = 0;
	m_dwLEAVESRATE[3] = 0;
	m_dwLEAVESRATE[4] = 0;
	m_dwLEAVESRATE[5] = 0;
	m_dwLEAVESRATE[6] = 0;
	m_dwLEAVESRATE[7] = 0;
	m_dwLEAVESRATE[8] = 0;
	m_dwLEAVESRATE[9] = 15;
	m_dwLEAVESRATE[10] = 30;
	m_dwLEAVESRATE[11] = 20;
	m_dwLEAVESRATE[12] = 0;

	m_vecOneMapWeather.clear();
}

GLPeriod::~GLPeriod(void)
{
}

void GLPeriod::SetPeriod ( DWORD dwYear, DWORD dwYearToday, DWORD dwHour, float fSec )
{
	m_dwThisYear = dwYear;
	m_dwYear_Today = dwYearToday;
	m_dwHour = dwHour;
	m_fSecond = fSec;

	SetThisMonth ();
	SetToday ();

	if ( 3 <= m_dwThisMonth && m_dwThisMonth < 10 )	m_bSummerSeason = TRUE;
	else											m_bSummerSeason = FALSE;

	if ( m_dwThisMonth > WINTER_S )			m_dw4Season = WINTER;
	else if ( m_dwThisMonth < WINTER_E )	m_dw4Season = WINTER;
	else if ( m_dwThisMonth < SPRING_E )	m_dw4Season = SPRING;
	else if ( m_dwThisMonth < SUMMER_E )	m_dw4Season = SUMMER;
	else if ( m_dwThisMonth < AUTUMN_E )	m_dw4Season = AUTUMN;

	m_dwRainRate = m_dwRAINRATE[m_dwThisMonth];
	m_dwSnowRate = m_dwSNOWRATE[m_dwThisMonth];

	m_dwSporeRate = m_dwSPORERATE[m_dwThisMonth];
	m_dwLeavesRate = m_dwLEAVESRATE[m_dwThisMonth];
}

void GLPeriod::SetPeriod ( const SPRERIODTIME &cTIME )
{
	SetPeriod ( cTIME.dwYear, cTIME.dwYearToday, cTIME.dwHour, cTIME.fSec );
}

SPRERIODTIME GLPeriod::GetPeriod ()
{
	return SPRERIODTIME ( m_dwThisYear, m_dwYear_Today, m_dwHour, m_fSecond );
}

DWORD GLPeriod::SetThisMonth ()
{
	MTHARRAY_ITER iter = m_MTH_LMT.upper_bound ( m_dwYear_Today );
	if ( iter==m_MTH_LMT.end() )
	{
		GASSERT(0&&"날짜에서 몇월인지 찾기에 실패하였습니다.");
		m_dwThisMonth = 6;	// default;
	}
	else
	{
		m_dwThisMonth = (*iter).second.wMonth;
	}

	return m_dwThisMonth;
}

DWORD GLPeriod::SetToday ()
{
	MTHARRAY_ITER iter = m_MTH_LMT.upper_bound ( m_dwYear_Today );
	if ( iter==m_MTH_LMT.end() )
	{
		GASSERT(0&&"날짜에서 몇월인지 찾기에 실패하였습니다.");
		m_dwMonth_Today = 15;	// default;
	}
	else
	{
		m_dwMonth_Today = m_dwYear_Today - (*iter).second.wMonthS + 2;
	}

	return m_dwMonth_Today;
}

void GLPeriod::TestToNight ( float _fvbr )
{
	m_fTIMETOVBR = _fvbr;
	
	m_dwHour = GetFadeOutTime ();
	m_fSecond = 0.0f;
}

void GLPeriod::TestToNoon ( float _fvbr )
{
	m_fTIMETOVBR = _fvbr;

	m_dwHour = GetFadeInTime ();
	m_fSecond = 0.0f;
}

void GLPeriod::SetOneMapActiveWeather( SONEMAPWEATHER oneMapWeather, bool bServer )
{
	// 삭제 	
	DWORD i;
	if( oneMapWeather.dwWeather == NULL )
	{		
		for( i = 0; i < m_vecOneMapWeather.size(); i++ )
		{
			if( m_vecOneMapWeather[i].map_mID == oneMapWeather.map_mID &&
				m_vecOneMapWeather[i].map_sID == oneMapWeather.map_sID )
			{
				m_vecOneMapWeather.erase( m_vecOneMapWeather.begin() + i );
				break;
			}
		}
	} // 추가
	else{
		if( bServer )
		{
			bool bAlreadyAdd = FALSE;

			oneMapWeather.startTime = CTime::GetCurrentTime();

			for( i = 0; i < m_vecOneMapWeather.size(); i++ )
			{
				if( m_vecOneMapWeather[i].map_mID == oneMapWeather.map_mID &&
					m_vecOneMapWeather[i].map_sID == oneMapWeather.map_sID )
				{
					m_vecOneMapWeather[i] = oneMapWeather;
					bAlreadyAdd = TRUE;
					break;
				}
			}
			if( bAlreadyAdd == FALSE )
			{
				m_vecOneMapWeather.push_back( oneMapWeather );
			}	
		}else{
			m_vecOneMapWeather.push_back(oneMapWeather);
		}
	
	}
}

void GLPeriod::SetWeather ( DWORD _dwWeather )
{
	m_dwWeather = _dwWeather;
	if( m_dwWeather == NULL && m_bActiveWeather == FALSE )
	{
		m_vecOneMapWeather.clear();
	}
}

DWORD GLPeriod::GetMapWeather( WORD mID, WORD sID )
{
	DWORD i;
	for( i = 0; i < m_vecOneMapWeather.size(); i++ )
	{
		if( m_vecOneMapWeather[i].map_mID == mID &&
			m_vecOneMapWeather[i].map_sID == sID )
		{
			return m_vecOneMapWeather[i].dwWeather;
		}
	}

	return m_dwWeather;
}

HRESULT GLPeriod::UpdateWeather ( float fElapsedTime )
{
	if ( !m_bActiveWeather )	return S_OK;

	//	Note : 날씨 변화.
	//
	m_fWeatherTimer -= fElapsedTime*m_fTIMETOVBR;
	

	//	Note : 일시적 자연효과 - 번개 발생확율 조절.
	//
	m_dwWhimsicalWeather &= ~FGW_WHIMSICAL;

	if ( m_dwWeather & FGW_RAIN )
	{
		m_fThunderTimer -= fElapsedTime;

		if ( m_fThunderTimer <= 0.0f )
		{
			m_dwWhimsicalWeather |= FGW_THUNDER;

			m_fThunderTimer = 30.0f + 20.0f*RANDOM_POS;
		}
	}


	//	Note : 날씨 변화 타이밍이 되었을때.
	//
	if ( m_fWeatherTimer < 0.0f )
	{
		m_fWeatherTimer = WEATHER_TIMESET*TIME_SEC;

		//	종전 날씨들을 지워줌.
		if ( m_dwWeather&FGW_ALL )
		{
			m_dwWeather &= ~FGW_ALL;
			return S_OK;						// 넘어간다. 
		}

		//  날씨 비율중 가장 큰 값을 구한다.
		DWORD dwHigh = (m_dwRainRate>=m_dwSnowRate) ? m_dwRainRate : m_dwSnowRate;
		dwHigh = (dwHigh>=m_dwSporeRate) ? dwHigh : m_dwSporeRate;
		dwHigh = (dwHigh>=m_dwLeavesRate) ? dwHigh : m_dwLeavesRate;

		//  현재 쓰여야 하는 날씨로 셋팅한다.
		if ( RANDOM_POS*100.0f<=dwHigh )
		{
			DWORD dwSum		= m_dwRainRate+m_dwSnowRate+m_dwSporeRate+m_dwLeavesRate;
			DWORD dwResult	= DWORD(RANDOM_POS*dwSum);

			if ( dwResult <= m_dwRainRate )
			{
				m_dwWeather |= FGW_RAIN;
			}
			else if ( dwResult <= m_dwRainRate+m_dwSnowRate )
			{
				m_dwWeather |= FGW_SNOW;
			}
			else if ( dwResult <= m_dwRainRate+m_dwSnowRate+m_dwSporeRate )
			{
				m_dwWeather |= FGW_SPORE;
			}
			else
			{
				m_dwWeather |= FGW_LEAVES;
			}
		}
	}

	// 각 맵에 따로 적용되는 날씨 효과 업데이트
	if( m_vecOneMapWeather.size() != 0 )
	{
		DWORD i;
		for( i = 0; i < m_vecOneMapWeather.size(); i++ )
		{
			CTime currentTime = CTime::GetCurrentTime();
			CTimeSpan timeSpan = currentTime - m_vecOneMapWeather[i].startTime;

			if( timeSpan.GetTotalMinutes() >= m_vecOneMapWeather[i].dwApplyTime )
//			if( timeSpan.GetTotalSeconds() >= (LONG)m_vecOneMapWeather[i].dwApplyTime ) // 테스트용
			{
				m_vecOneMapWeather.erase( m_vecOneMapWeather.begin() + i );
				i--;
			}else{
				m_vecOneMapWeather[i].dwWhimsicalWeather &= ~FGW_WHIMSICAL;

				if ( m_vecOneMapWeather[i].dwWeather & FGW_RAIN )
				{
					m_vecOneMapWeather[i].fThunderTimer -= fElapsedTime;

					if ( m_vecOneMapWeather[i].fThunderTimer <= 0.0f )
					{
						m_vecOneMapWeather[i].dwWhimsicalWeather |= FGW_THUNDER;

						m_vecOneMapWeather[i].fThunderTimer = 30.0f + 20.0f*RANDOM_POS;
					}
				}

				////  날씨 비율중 가장 큰 값을 구한다.
				//DWORD dwHigh = (m_dwRainRate>=m_dwSnowRate) ? m_dwRainRate : m_dwSnowRate;
				//dwHigh = (dwHigh>=m_dwSporeRate) ? dwHigh : m_dwSporeRate;
				//dwHigh = (dwHigh>=m_dwLeavesRate) ? dwHigh : m_dwLeavesRate;

				////  현재 쓰여야 하는 날씨로 셋팅한다.
				//if ( RANDOM_POS*100.0f<=dwHigh )
				//{
				//	DWORD dwSum		= m_dwRainRate+m_dwSnowRate+m_dwSporeRate+m_dwLeavesRate;
				//	DWORD dwResult	= DWORD(RANDOM_POS*dwSum);

				//	if ( dwResult <= m_dwRainRate )
				//	{
				//		m_vecOneMapWeather[i].dwWeather |= FGW_RAIN;
				//	}
				//	else if ( dwResult <= m_dwRainRate+m_dwSnowRate )
				//	{
				//		m_vecOneMapWeather[i].dwWeather |= FGW_SNOW;
				//	}
				//	else if ( dwResult <= m_dwRainRate+m_dwSnowRate+m_dwSporeRate )
				//	{
				//		m_vecOneMapWeather[i].dwWeather |= FGW_SPORE;
				//	}
				//	else
				//	{
				//		m_vecOneMapWeather[i].dwWeather |= FGW_LEAVES;
				//	}
				//}
			}
		}
	}

	return S_OK;
}

float GLPeriod::GetDirectionFact ()
{
	float	fFact;
	float	fFullTime;
	float	fCurTime = m_dwHour*TIME_SEC + m_fSecond;		// 현재의 시간 나타냄

	if ( m_bSummerSeason )
	{
		fFullTime = (SUMMMER_NIGHT_S*TIME_SEC) - (SUMMMER_NIGHT_E*TIME_SEC);
		fCurTime -= SUMMMER_NIGHT_E*TIME_SEC;
		fFact = (fCurTime/fFullTime) - 0.5f;
	}
	else
	{
		fFullTime = (WINTER_NIGHT_S*TIME_SEC) - (WINTER_NIGHT_E*TIME_SEC);
		fCurTime -= WINTER_NIGHT_E*TIME_SEC;
		fFact = (fCurTime/fFullTime) - 0.5f;
	}

	fFact *= 2.f;

	return fFact;
}

D3DXVECTOR2 GLPeriod::GetDirectFact_XY ()		// 태양, 달 의 좌표를 X,Y 값으로 알아낼때 쓰임
{
	D3DXVECTOR2	vFact;
	float		fRate;
	float		fCurTime = m_dwHour*TIME_SEC + m_fSecond;		// 현재의 시간 나타냄

	float fSunTime_Left;
	float fSunTime_Up;
	float fSunTime_Right;
	float fSunTime_Down_S;
	float fSunTime_Down_L;

	if ( m_bSummerSeason )
	{
		fSunTime_Left	= SUMMMER_NIGHT_E+0.5f;
		fSunTime_Up		= (SUMMMER_NIGHT_E+(SUMMMER_NIGHT_S-SUMMMER_NIGHT_E)*0.5f)+0.5f;
		fSunTime_Right	= SUMMMER_NIGHT_S+0.5f;
		fSunTime_Down_S	= fSunTime_Up-12.f;
		fSunTime_Down_L	= fSunTime_Up+12.f;
	}
	else
	{
		fSunTime_Left	= WINTER_NIGHT_E+0.5f;
		fSunTime_Up		= (WINTER_NIGHT_E+(WINTER_NIGHT_S-WINTER_NIGHT_E)*0.5f)+0.5f;
		fSunTime_Right	= WINTER_NIGHT_S+0.5f;
		fSunTime_Down_S	= fSunTime_Up-12.f;
		fSunTime_Down_L	= fSunTime_Up+12.f;
	}

	fSunTime_Left	*= TIME_SEC;
	fSunTime_Up		*= TIME_SEC;
	fSunTime_Right	*= TIME_SEC;
	fSunTime_Down_S	*= TIME_SEC;
	fSunTime_Down_L	*= TIME_SEC;


	if ( (fCurTime>=fSunTime_Down_S) && (fCurTime<fSunTime_Left) )			// 밤->새벽
	{
		fRate = (fCurTime-fSunTime_Down_S) / (fSunTime_Left-fSunTime_Down_S);

		if ( fRate >= 0.f && fRate <= 1.f )
		{
			vFact.x = fRate;
			vFact.y = -1.f + fRate;
			return vFact;
		}
	}
	if ( (fCurTime>=fSunTime_Left) && (fCurTime<fSunTime_Up) )				// 새벽->정오
	{
		fRate = (fCurTime-fSunTime_Left) / (fSunTime_Up-fSunTime_Left);
		vFact.x = 1.f - fRate;
		vFact.y = fRate;

		return vFact;
	}
	if ( (fCurTime>=fSunTime_Up) && (fCurTime<fSunTime_Right) )				// 정오->저녁무렵
	{
		fRate = (fCurTime-fSunTime_Up) / (fSunTime_Right-fSunTime_Up);
		vFact.x = -fRate;
		vFact.y = 1.f - fRate;

		return vFact;
	}
	if ( (fCurTime>=fSunTime_Right) && (fCurTime<fSunTime_Down_L) )			// 저녁무렵->밤
	{
		fRate = (fCurTime-fSunTime_Right) / (fSunTime_Down_L-fSunTime_Right);

		if ( fRate >= 0.f && fRate <= 1.f )
		{
			vFact.x = -1.f + fRate;
			vFact.y = -fRate;
			return vFact;
		}
	}

	if ( fCurTime >= (12.f*TIME_SEC) )										// 값이 크고 "밤->새벽"
	{
		fCurTime = fCurTime - (24.f*TIME_SEC);

		fRate = (fCurTime-fSunTime_Down_S) / (fSunTime_Left-fSunTime_Down_S);
		vFact.x = fRate;
		vFact.y = -1.f + fRate;
		return vFact;
	}
	else																	// 값이 작고 "저녁무렵->밤"
	{
		fCurTime = fCurTime + (24.f*TIME_SEC);

		fRate = (fCurTime-fSunTime_Right) / (fSunTime_Down_L-fSunTime_Right);
		vFact.x = -1.f + fRate;
		vFact.y = -fRate;
		return vFact;
	}

	return vFact;
}

HRESULT GLPeriod::FrameMove ( float fTime, float fElapsedTime )
{
	m_fSecond += fElapsedTime*m_fTIMETOVBR;

	//	초 경과. ( 시간경과 체크 )
	if ( m_fSecond >= TIME_SEC )
	{
		m_fSecond = 0;
		m_dwHour += 1;
	}

	//	날 경과 체크
	if ( m_dwHour >= DAY_HOUR )
	{
		m_dwMonth_Today += 1;
		m_dwYear_Today += 1;
		m_dwHour = 0;
	}

	//	년 경과 체크.
	if ( m_dwYear_Today >= YEAR_DAY )
	{
		m_dwThisMonth = 1;
		m_dwThisYear += 1;

		m_dwYear_Today = 1;
	}


	//	달 경과 체크
	DWORD dwNewThisMonth = SetThisMonth();
	DWORD dwToday = SetToday();

//	CDebugSet::ToView ( 23, "day : %02d", m_dwMonth_Today );

	if ( m_bNight )
	{
		//	밤->낮 전환.
		if ( IsNoon() )		m_bNight = !m_bNight;
	}
	else
	{
		//	밤->낮 전환.
		if ( !IsNoon() )	m_bNight = !m_bNight;
	}

	m_fBlueFact = 0.f;
	m_fRedFact = 0.f;

	if( IsNoon() )
	{
		m_sFGTime = FGT_DAYTIME;
		m_fBaseFact = 1.0f;//	1.0f 일때 낮.
	}
	else
	{
		m_sFGTime = FGT_NIGHT;
		m_fBaseFact = 0.0f;//	0.0f 일때 밤.
	}

	//	밝아지는 시간.
	if ( IsFadeInTime() )
	{
		m_fBaseFact = m_fSecond/float(TIME_SEC);
		m_fRedFact = 0.f;
		if ( m_fBaseFact > 0.5f )	
		{
			float fFact = m_fBaseFact - 0.5f;
			m_fBlueFact = fFact*2.f;
		}
		else	m_fBlueFact = 0.f;
	}

	//	어두워지는 시간.
	if ( IsFadeOutTime() )
	{
		m_fBaseFact = 1.0f - (m_fSecond/float(TIME_SEC));
		m_fBlueFact = 0.f;
		if ( m_fBaseFact > 0.5f )	
		{
			float fFact = m_fBaseFact - 0.5f;
			m_fRedFact = fFact*2.f;
		}
		else	m_fRedFact = 0.f;
	}

	//	새벽시간대.
	if( IsBlueTime() )
	{
		m_sFGTime = FGT_DAWN;
		m_fBlueFact = 1.f - (m_fSecond/float(TIME_SEC));
	}

	//	노을이 지는 시간대.
	if( IsRedTime() )
	{
		m_sFGTime = FGT_SUNSET;
		m_fRedFact = m_fSecond/float(TIME_SEC);
	}

	
	//	낮일때. 흐린 날씨일때 조명을 조금 어둡게 만듬.
	float fRainRate = DxWeatherMan::GetInstance()->GetRain()->GetApplyRate();
	float fSnowRate = DxWeatherMan::GetInstance()->GetSnow()->GetApplyRate();
	if ( ( (fRainRate>0.f) || (fSnowRate>0.f) ) && m_fBaseFact>fCLOUDY_BLENDFACT )
	{
		float fWeatherRate = (fRainRate>fSnowRate) ? fRainRate : fSnowRate;

		//				기본 값	   | 기본값에 대응하기 위해 | 빼야하는 값을 얻기 위해 | 얼마나 상태가 심한가?
		m_fBlendFact = m_fBaseFact - (m_fBaseFact * ( 1.f - fCLOUDY_BLENDFACT ) * fWeatherRate);

		// 저녁 타임.. 
		m_fNightFact = 1.f - m_fBaseFact;
		m_fNightFact = m_fNightFact - (m_fNightFact * ( 1.f - fCLOUDY_BLENDFACT ) * fWeatherRate);

	}
	else
	{
		m_fBlendFact = m_fBaseFact;			// 원래 조명을 사용함.
		m_fNightFact = 1.f - m_fBaseFact;	// 저녁 타임.
	}

	//CDebugSet::ToView ( 16, "BF: %1.3f", m_fBlueFact );
	//CDebugSet::ToView ( 17, "BF: %1.3f", m_fRedFact );
	//CDebugSet::ToView ( 18, "H: %02d,  S: %3.1f", m_dwHour, m_fSecond );

	return S_OK;
}

void GLPeriod::LoadFile ( const char* szFile )
{
	std::string strFullPath;
	strFullPath = std::string(GLOGIC::GetServerPath()) + szFile;

	CIniLoader gFILE;
	
	if( GLOGIC::bGLOGIC_ZIPFILE )
		gFILE.SetZipFile( GLOGIC::strGLOGIC_SERVER_ZIPFILE );

	bool bOK = gFILE.open( strFullPath, true );
	if ( !bOK )
	{
		MessageBox( NULL, "period file open fail.", szFile, MB_OK );
		return;
	}

	for ( int i=0; i<YEAR_MONTH+1; ++i )
	{
		gFILE.getflag ( "PERIOD", "RAINRATE", i, (YEAR_MONTH+1), m_dwRAINRATE[i] );
		gFILE.getflag ( "PERIOD", "SNOWRATE", i, (YEAR_MONTH+1), m_dwSNOWRATE[i] );
		gFILE.getflag ( "PERIOD", "SPORERATE", i, (YEAR_MONTH+1), m_dwSPORERATE[i] );
		gFILE.getflag ( "PERIOD", "LEAVESRATE", i, (YEAR_MONTH+1), m_dwLEAVESRATE[i] );
	}
}