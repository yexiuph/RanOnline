#pragma once

#include <queue>

#define MAX_EVENT_POS 5

struct GLLEV_TIME
{
	DWORD dwHour;		// 시간 (0-23까지)
	DWORD dwMinute;		// 분 (0-59까지)

	GLLEV_TIME()
		: dwHour(0)
		, dwMinute(0)
	{
	}
};

struct GLLEV_SCRIPT
{
	GLLEV_TIME levTime;				
	std::string strCommand;		// 자동 레벨 명령어
	std::string strArgument;	// 자동 레벨 명령어 인자
};

struct GLLEV_POS
{
	DWORD	m_dwPOS_X;
	DWORD	m_dwPOS_Y;

	GLLEV_POS()
		: m_dwPOS_X(0)
		, m_dwPOS_Y(0)
	{
	}
};

typedef std::vector<GLLEV_SCRIPT>	VEC_LEV_SCRIPT;
typedef VEC_LEV_SCRIPT::iterator	VEC_LEV_SCRIPT_ITER;

typedef std::queue<GLLEV_SCRIPT>	QUEUE_LEV_SCRIPT;

typedef std::list<std::string>		STRING_LIST;
typedef STRING_LIST::iterator		STRING_LIST_ITER;

class GLAutoLevel
{
public:
	enum { MAX_DAY = 7 };

protected:
	bool		m_bPVPFlag;				// 0:NonPVP, 1:PVP=선도전
	std::string	m_strName;				// 이벤트 명칭
	DWORD		m_dwMapID;
	DWORD		m_dwGateID;
	DWORD		m_dwHallMapID;
	GLLEV_POS	m_dwOutPos[MAX_EVENT_POS];	// 캐릭터가 이벤트 홀로 돌아갈 좌표
	DWORD		m_dwDay[MAX_DAY];		// [월,화,수,목,금,토,일]
	GLLEV_TIME	m_StartTime;
	GLLEV_TIME	m_EndTime;
	bool		m_bEvent;
	DWORD		m_dwLastEventDay;

	VEC_LEV_SCRIPT	m_vecLevScript;		// 스크립트 리스트
	VEC_LEV_SCRIPT::size_type	m_LevScriptPos;

	QUEUE_LEV_SCRIPT m_queueLevScript;

public:
	GLAutoLevel ();
	~GLAutoLevel ();

public:
	bool IsEvent()									{ return m_bEvent; }
	bool IsEventStartTime( int nDayOfWeek, int nDay, int nHour, int nMinute );
	bool IsEventEndTime( int nHour, int nMinute );

public:
	void DoEventStart( int nDay )								
	{ 
		m_LevScriptPos = 0;
		m_dwLastEventDay = nDay;
		m_bEvent = true; 
	}
	void DoEventEnd()								{ m_bEvent = false; }

public:
	void DoEventScript( int nHour, int nMinute );
	void RunScriptCommand( GLLEV_SCRIPT & glLevScript );

public:
	bool LOAD( const std::string & strFileName, STRING_LIST & listCommand );
};