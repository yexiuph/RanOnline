#pragma once

#include "GLCDMDefine.h"

#define RANKING_NUM		9
#define CLUB_NAME		10

class GLClubDeathMatch
{
public:
	enum 
	{		
		MAX_TIME = 4,
	};

	enum EM_BATTLE_STATE 
	{
		BATTLE_NOSTART		= 0,	//	시작하지 않음
		BATTLE_READY		= 1,	//	배틀 대기중
		BATTLE_START		= 2,	//	배틀 중
		BATTLE_END_AWARD	= 3,	//	배틀 종료후 정리 시간 ( 상품 지급 )
		BATTLE_END			= 4,	//	배틀 종료후 튕기는 시간
	};

public:
	DWORD				m_dwID;					// 관리 ID.
	std::string			m_strName;				// 관리 이름.
//	std::vector<DWORD>	m_vecMaps;				// 선도 관리 맵.

	DWORD				m_dwClubMap;			//	클럽 맵. ( 전산실 )
	DWORD				m_dwCLubMapGate;		//	클럽 맵에 있는 출입구.
	DWORD				m_dwClubHallMap;		//	클럽 맵. ( 입구맵 ) 
	DWORD				m_dwClubHallGate;		//	클럽 맵 출구  ( 입구맵 ) 

//	float				m_fItemDropRate;		// 아이템 드롭 확율.
//	float				m_fDefaultCommissionPer;// 기본 수수료율.

	DWORD				m_dwWinClub;		//	승리 클럽 ID.
//	float				m_fCommissionPercent;	//	수수료율.

	DWORD				m_dwBattleTime;
	SCDM_TIME			m_sCdmTIME[MAX_TIME];		//	전투시간.

	SCDM_AWARD_ITEM		m_sCdmAwardItem;		
	
	CDM_AWARD_CHAR		m_vecCdmAwardChar;		//	보상을 받은 캐릭터 ( 두번받는걸 방지하기 위해 )
	CDM_RANK_INFO_MAP	m_mapCdmScore;			//	cdm 스코어
//	CDM_RANK_INFO_VEC	m_vecCdmRank;			//	클럽랭크 백업본

//	bool				m_bNewCommission;		//	새 커미션 변경 예약.
//	float				m_fCommissionTimer;		//	새 커미션 변경 타이머.
//	float				m_fNewCommissionPercent;//	새 커미션.

	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;

	EM_BATTLE_STATE		m_emBattleState;
	DWORD				m_dwBattleOrder;		//	이번주에 몇번째인지...
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;

//	DWORD				m_dwCERTIFY_CHARID;
//	DWORD				m_dwCERTIFY_NPCID;
//	float				m_fCERTIFY_TIMER;
//	D3DXVECTOR3			m_vCERTIFY_POS;

	float				m_fCHECK_TIMER;			//	배틀 종료후 타이머들 ( 상품이랑 유저 팅길때 사용 ) 
	float				m_fCHECK_TIME;			//

//	bool				m_bBattleEndCheck;		// 선도전 종료직후 전산실 입장을 막기위해
//	float				m_fCHECK_TIMER;
//	float				m_fCHECK_TIME;


	float				m_fRankingUpdate;		//	랭킹 타이머

public:
	GLClubDeathMatch ();

	GLClubDeathMatch ( const GLClubDeathMatch &value )
	{
		operator= ( value );
	}

	GLClubDeathMatch& operator= ( const GLClubDeathMatch& value );

	bool operator < ( const GLClubDeathMatch& sClubDM )
	{
		return m_dwID < sClubDM.m_dwID;
	}

public:
	bool Load ( std::string strFile );

public:
	void AddCDMScore( DWORD dwKillClub, DWORD dwDeathClub );
	SCDM_RANK_INFO* GetCDMRankInof( DWORD dwClubID );

public:
//	bool IsCertify()		{ return m_dwCERTIFY_CHARID!=0; }

public:
//	bool IsNewCommission ();
//	bool IsGuidClub ( DWORD dwClubID );

public:
	bool IsBattle () { return ( m_emBattleState == BATTLE_START ); }
	bool IsBattleReady() { return ( m_emBattleState == BATTLE_READY ); }
	bool IsBattleEndAward() { return ( m_emBattleState = BATTLE_END_AWARD); }
	bool IsBattleEnd() { return ( m_emBattleState == BATTLE_END ); }
	bool IsEnterMap( DWORD dwClubID );
	DWORD IsBattleWeekDay ( int nDayOfWeek, int nDay );

	void SetBattle( EM_BATTLE_STATE emBattleState )	{ m_emBattleState = emBattleState; }
	
	// 선도클럽 시작시간인가?
	bool IsBattleHour ( DWORD dwORDER, int nHour );

public:
	// 최종적으로 선도클럽결정전이 일어난 날을 세팅한다.
	void UpdateBattleDay ( int nDay );
	// 선도클럽 결정전 남은 시간을 클라이언트에게 알린다.
	void UpdateNotifyBattle ( DWORD dwORDER, int nHour, int nMinute );
//	void UpdateCommission ( float fElaps );

	//	선도 클럽이 종료되고 나서 유저를 튕군다.
//	void CheckExtraGuild ( float fElaps );
//	bool IsCheckExtraGuild() { return true; }

	void CheckAwardClub( float fElaps );	//	보상 지급
	//void CheckExtraClub( float fElaps );	//	유저 내보내기 

public:
	// 선도클럽 결정전을 시작한다.
	void DoBattleStart ( DWORD dwORDER, int nDay );
	void DoBattleEnd();

//	void DoCertifyEnd();

public:
//	bool ChangeGuidClub ( DWORD dwClubID );
//	bool ChangeCommission ( float fRate );
};

class GLClubDeathMatchMan
{
public:
	typedef std::vector<GLClubDeathMatch>			CDM_VEC;
	typedef CDM_VEC::iterator						CDM_VEC_ITER;
	typedef std::pair<CDM_VEC_ITER,CDM_VEC_ITER>	CDM_VEC_PAIR;

protected:
	CDM_VEC											m_vecClubDM;

public:
	bool Load ( std::vector<std::string> &vecFiles );
//	bool SetState ( VECGUID_DB &vecGUID );

public:
	GLClubDeathMatch* Find ( DWORD dwID );
	bool IsBattle ();
//	bool IsCheckExtraGuild ();

public:
	GLClubDeathMatchMan ()
	{
	}

public:
	//static GLGuidanceMan& GetInstance();
};

class GLClubDeathMatchAgentMan : public GLClubDeathMatchMan
{
protected:
	float m_fRemainTimer, m_fTimer;

public:
//	bool ChangeGuidClub ( DWORD dwID, DWORD dwClubID );
//	bool ChangeCommission ( DWORD dwID, float fRate );

public:
//	GLClubDeathMatch* FindByClubID ( DWORD dwClubID );

public:
	bool SetMapState ();

public:
//	DWORD GetGuidID ( DWORD dwClubID );
	DWORD GetRemainTime () { return (DWORD)m_fRemainTimer; }

public:
	bool FrameMove ( float fElapsedAppTime );

public:
	GLClubDeathMatchAgentMan ()
		: m_fRemainTimer(0)
		, m_fTimer(0)
	{
	}

public:
	static GLClubDeathMatchAgentMan& GetInstance();
};

class GLClubDeathMatchFieldMan : public GLClubDeathMatchMan
{
public:
	bool SetMapState ();

public:
	bool ReadyBattle( DWORD dwID );
	bool BeginBattle ( DWORD dwID );
	bool EndBattle ( DWORD dwID );

public:
//	bool ChangeGuidClub ( DWORD dwID, DWORD dwClubID );
//	bool ChangeCommission ( DWORD dwID, float fRate );

//	bool DoCertify ( DWORD dwID, DWORD dwCHARID, DWORD dwNPCID, const D3DXVECTOR3 &vPOS );

public:
	DWORD GetWinClubID ( DWORD dwClubID );
//	EMCHECKCERTIFY CheckCertify ( DWORD dwID, DWORD dwCHARID );
	const std::string GetName ( DWORD dwID );

//	void CheckExtraGuild ( float fElaps );

	void UpdateCDMRanking( DWORD dwID );
	void CDMAwardItem( DWORD dwID );

public:
	bool FrameMove ( float fElaps );

protected:
	GLClubDeathMatchFieldMan ()
	{
	}

public:
	static GLClubDeathMatchFieldMan& GetInstance();
};