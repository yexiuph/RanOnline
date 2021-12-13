#pragma	once




class CCalendar
{
public:
	enum 
	{
		SUNDAY = 0,
		MONDAY = 1,
		TUESDAY = 2,
		WEDNESDAY = 3,
		THURSDAY = 4,
		FRIDAY = 5,
		SATURDAY = 6,
		DAY_OF_WEEK = 7,
		MONTH_OF_YEAR = 12,
	};

	CCalendar();
	CCalendar( int nYear, int nMonth, int nDay );
	~CCalendar();

private:
	bool IsLeafMonth( int nYear );	// 윤달
	int GetDayOfFirstYear ( int nYear, int nMonth, int nDay );	//	시초부터 현재까지 일수
	int GetDayOfYear( int nYear, int nMonth, int nDay );		// 1월1일부터 현재일까지 경과 일수
	int GetDayOfWeek( int nYear, int nMonth, int nDay );		// 현재 날짜의 요일을 가져온다.

public:
	
	int GetLastDayOfMonth();	// 현재달의 마지막 날짜	
	int GetDayOfWeek_Month();	// 현재달 1일의 요일
	int GetDayOfWeek_Year();	// 현재년도 1일의 요일

	void SetDate( int nYear, int nMonth, int nDay );
	WORD GetDateYear()	{ return m_nYear; }
	WORD GetDateMonth()	{ return m_nMonth; }
	WORD GetDateDay()	{ return m_nDay; }

public:

	int m_nYear;
	int m_nMonth;
	int m_nDay;
};