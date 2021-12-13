#include "pch.h"
#include "Calendar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int nLastDayOfMonth[CCalendar::MONTH_OF_YEAR] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

CCalendar::CCalendar()
	: m_nYear( 1 )
	, m_nMonth( 1 )
	, m_nDay( 1 )
{
}

CCalendar::CCalendar( int nYear, int nMonth, int nDay )
	: m_nYear( nYear )
	, m_nMonth( nMonth )
	, m_nDay( nDay )
{
}


CCalendar::~CCalendar()
{
}

void CCalendar::SetDate( int nYear, int nMonth, int nDay )
{
	m_nYear = nYear;
	m_nMonth = nMonth;
	m_nDay = nDay;
}

//	윤달
bool CCalendar::IsLeafMonth( int nYear )
{
	if ( !(nYear%4) && (nYear%100) || !(nYear%400) )  return true;
	
	return false;
}

//	시초부터 현재까지 일수
int CCalendar::GetDayOfFirstYear( int nYear, int nMonth, int nDay )
{
	int nCntDay = 0;
	for ( int i = 1; i < nYear; ++i )
	{
		if ( IsLeafMonth( i ) ) nCntDay += 366;
		else nCntDay += 365;
	}

	nCntDay += GetDayOfYear( nYear, nMonth, nDay );

	return nCntDay;
}

int CCalendar::GetDayOfYear( int nYear, int nMonth, int nDay )
{
	int nCntDay = 0;
	
	for ( int i = 1; i < nMonth; ++i )
	{
		nCntDay += nLastDayOfMonth[i-1];
		if ( i == 2 && IsLeafMonth( nYear ) ) nCntDay++;
	}

	nCntDay += nDay;

	return nCntDay;
}

// 날짜의 요일을 구해온다.
int CCalendar::GetDayOfWeek( int nYear, int nMonth, int nDay )
{
	int nCntDay = GetDayOfFirstYear( nYear, nMonth, nDay );
	int nDayOfWeek = nCntDay % DAY_OF_WEEK;
	
	return nDayOfWeek;
}

// 현재달의 마지막 날짜
int CCalendar::GetLastDayOfMonth()
{
	if ( m_nMonth == 2 && IsLeafMonth( m_nYear ) )	return 29;

	return nLastDayOfMonth[m_nMonth-1];
}

// 현재달 1일의 요일
int CCalendar::GetDayOfWeek_Month()
{
	return GetDayOfWeek( m_nYear, m_nMonth, 1 );
}

// 현재년도 1일의 요일
int CCalendar::GetDayOfWeek_Year()
{	
	return GetDayOfWeek( m_nYear, 1, 1 );
}


	