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
	bool IsLeafMonth( int nYear );	// ����
	int GetDayOfFirstYear ( int nYear, int nMonth, int nDay );	//	���ʺ��� ������� �ϼ�
	int GetDayOfYear( int nYear, int nMonth, int nDay );		// 1��1�Ϻ��� �����ϱ��� ��� �ϼ�
	int GetDayOfWeek( int nYear, int nMonth, int nDay );		// ���� ��¥�� ������ �����´�.

public:
	
	int GetLastDayOfMonth();	// ������� ������ ��¥	
	int GetDayOfWeek_Month();	// ����� 1���� ����
	int GetDayOfWeek_Year();	// ����⵵ 1���� ����

	void SetDate( int nYear, int nMonth, int nDay );
	WORD GetDateYear()	{ return m_nYear; }
	WORD GetDateMonth()	{ return m_nMonth; }
	WORD GetDateDay()	{ return m_nDay; }

public:

	int m_nYear;
	int m_nMonth;
	int m_nDay;
};