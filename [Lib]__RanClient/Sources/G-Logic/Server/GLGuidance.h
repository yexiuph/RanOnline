#pragma once

#include <vector>

#include "GLCharDefine.h"

struct GLGUID_DB
{
	DWORD	m_dwID;				//	선도 지역 ID.
	DWORD	m_dwCLUBID;			//	선도 클럽 ID.
	float	m_fCommissionPer;	//	수수료율.

	GLGUID_DB () :
		m_dwID(UINT_MAX),
		m_dwCLUBID(CLUB_NULL),

		m_fCommissionPer(0)
	{
	}
};

typedef std::vector<GLGUID_DB>	VECGUID_DB;

struct SBATTLE_TIME
{
	DWORD dwWeekDay; // 일 월 화 수 목 금 토 1 2 3 4 5 6 7
	DWORD dwStartTime; // 시간 (0-23까지)

	SBATTLE_TIME () :
		dwWeekDay(0),
		dwStartTime(0)
	{
	}
};

struct SAWARD_MOB
{
	SNATIVEID	nidGenMob;	//	결정전 완료후 생성되는 보상-몹.
	WORD		wGenPosX;	//	' 의 위치.
	WORD		wGenPosY;

	SAWARD_MOB() :
		nidGenMob(0,0),
		wGenPosX(0),
		wGenPosY(0)
	{
	}
};

class GLGuidance
{
public:
	enum { MAX_TIME = 4, };

public:
	DWORD				m_dwID;					// 관리 ID.
	std::string			m_strName;				// 관리 이름.
	std::vector<DWORD>	m_vecMaps;				// 선도 관리 맵.

	DWORD				m_dwClubMap;			//	클럽 맵. ( 전산실 )
	DWORD				m_dwCLubMapGate;		//	클럽 맵에 있는 출입구.
	DWORD				m_dwClubHallMap;		//	클럽 맵. ( 입구맵 ) 

	float				m_fItemDropRate;		// 아이템 드롭 확율.
	float				m_fDefaultCommissionPer;// 기본 수수료율.

	DWORD				m_dwGuidanceClub;		//	선도 클럽 ID.
	float				m_fCommissionPercent;	//	수수료율.

	DWORD				m_dwBattleTime;
	SBATTLE_TIME		m_sTIME[MAX_TIME];		//	전투시간.

	std::vector<SAWARD_MOB> m_vecAwardMob;

	bool				m_bNewCommission;		//	새 커미션 변경 예약.
	float				m_fCommissionTimer;		//	새 커미션 변경 타이머.
	float				m_fNewCommissionPercent;//	새 커미션.

	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;
	
	bool				m_bBattle;
	DWORD				m_dwBattleOrder;
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;

	DWORD				m_dwCERTIFY_CHARID;
	DWORD				m_dwCERTIFY_NPCID;
	float				m_fCERTIFY_TIMER;
	D3DXVECTOR3			m_vCERTIFY_POS;

	bool				m_bBattleEndCheck;		// 선도전 종료직후 전산실 입장을 막기위해
	float				m_fCHECK_TIMER;
	float				m_fCHECK_TIME;

public:
	GLGuidance ();

	GLGuidance ( const GLGuidance &value )
	{
		operator= ( value );
	}

	GLGuidance& operator= ( const GLGuidance& value );

	bool operator < ( const GLGuidance& sGuidance )
	{
		return m_dwID < sGuidance.m_dwID;
	}

public:
	bool Load ( std::string strFile );

public:
	bool IsCertify()		{ return m_dwCERTIFY_CHARID!=0; }

public:
	bool IsNewCommission ();
	bool IsGuidClub ( DWORD dwClubID );

public:
	bool IsBattle ();
	DWORD IsBattleWeekDay ( int nDayOfWeek, int nDay );
	
	// 선도클럽 시작시간인가?
	bool IsBattleHour ( DWORD dwORDER, int nHour );

public:
	// 최종적으로 선도클럽결정전이 일어난 날을 세팅한다.
	void UpdateBattleDay ( int nDay );
	// 선도클럽 결정전 남은 시간을 클라이언트에게 알린다.
	void UpdateNotifyBattle ( DWORD dwORDER, int nHour, int nMinute );
	void UpdateCommission ( float fElaps );

	void CheckExtraGuild ( float fElaps );

public:
	// 선도클럽 결정전을 시작한다.
	void DoBattleStart ( DWORD dwORDER, int nDay );
	void DoBattleEnd();

	void DoCertifyEnd();

public:
	bool ChangeGuidClub ( DWORD dwClubID );
	bool ChangeCommission ( float fRate );
};

class GLGuidanceMan
{
public:
	typedef std::vector<GLGuidance>		GUIDANCE_VEC;
	typedef GUIDANCE_VEC::iterator		GUIDANCE_VEC_ITER;
	typedef std::pair<GUIDANCE_VEC_ITER,GUIDANCE_VEC_ITER>	GUIDANCE_VEC_PAIR;

protected:
	std::vector<GLGuidance>		m_vecGuidance;

public:
	bool Load ( std::vector<std::string> &vecFiles );
	bool SetState ( VECGUID_DB &vecGUID );

public:
	GLGuidance* Find ( DWORD dwID );
	bool IsBattle ();
	bool IsCheckExtraGuild ();

public:
	GLGuidanceMan ()
	{
	}

public:
	//static GLGuidanceMan& GetInstance();
};

class GLGuidanceAgentMan : public GLGuidanceMan
{
protected:
	float m_fRemainTimer, m_fTimer;

public:
	bool ChangeGuidClub ( DWORD dwID, DWORD dwClubID );
	bool ChangeCommission ( DWORD dwID, float fRate );

public:
	GLGuidance* FindByClubID ( DWORD dwClubID );

public:
	bool SetMapState ();

public:
	DWORD GetGuidID ( DWORD dwClubID );
	DWORD GetRemainTime () { return (DWORD)m_fRemainTimer; }

public:
	bool FrameMove ( float fElapsedAppTime );

public:
	GLGuidanceAgentMan ()
		: m_fRemainTimer(0)
		, m_fTimer(0)
	{
	}

public:
	static GLGuidanceAgentMan& GetInstance();
};

enum EMCHECKCERTIFY
{
	EMCHECKCERTIFY_FAIL			= 0,
	EMCHECKCERTIFY_OK			= 1,
	EMCHECKCERTIFY_DOING		= 2,
	EMCHECKCERTIFY_OTHERDOING	= 3,
	EMCHECKCERTIFY_NOTBATTLE	= 4,
};

class GLGuidanceFieldMan : public GLGuidanceMan
{
protected:

public:
	bool SetMapState ();

public:
	bool BeginBattle ( DWORD dwID );
	bool EndBattle ( DWORD dwID );

public:
	bool ChangeGuidClub ( DWORD dwID, DWORD dwClubID );
	bool ChangeCommission ( DWORD dwID, float fRate );

	bool DoCertify ( DWORD dwID, DWORD dwCHARID, DWORD dwNPCID, const D3DXVECTOR3 &vPOS );

public:
	DWORD GetGuidID ( DWORD dwClubID );
	EMCHECKCERTIFY CheckCertify ( DWORD dwID, DWORD dwCHARID );
	const std::string GetName ( DWORD dwID );

	void CheckExtraGuild ( float fElaps );

public:
	bool FrameMove ( float fElaps );

protected:
	GLGuidanceFieldMan ()
	{
	}

public:
	static GLGuidanceFieldMan& GetInstance();
};