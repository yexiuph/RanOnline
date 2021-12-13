#pragma once

#include <map>
#include <set>

#include "./GLCharDefine.h"
#include "./GLogicData.h"

struct GLCLUBMEMBER
{
	DWORD		dwID;                  // 캐릭터 번호
	bool		bONLINE;               // 온라인상태
	int			nCHANNEL;
	SNATIVEID	nidMAP;                // 맵번호
	D3DXVECTOR3	vecPOS;                // 위치번호
	TCHAR		m_szName[CHAR_SZNAME]; // 캐릭터명
	DWORD		m_dwFlags;             // 길드 지위

	GLCLUBMEMBER () 
		: dwID(0)
		, bONLINE(false)
		, nCHANNEL(0)
		, nidMAP(false)
		, vecPOS(0,0,0)
		, m_dwFlags(0)
	{
		SecureZeroMemory( m_szName, sizeof(m_szName) );
	}

	GLCLUBMEMBER ( const GLCLUBMEMBER& value )
	{
		operator=( value );
	}

public:
	GLCLUBMEMBER& operator = ( const GLCLUBMEMBER& value );
};

struct GLCLUBALLIANCE
{
	DWORD		m_dwID;
	char		m_szName[CHAR_SZNAME];

	GLCLUBALLIANCE() 
		: m_dwID(CLUB_NULL)
	{
		memset (m_szName, 0, sizeof(char) * CHAR_SZNAME);		
	}

	GLCLUBALLIANCE ( DWORD dwID, const char *szName )
	{
		m_dwID = dwID;
		StringCchCopy ( m_szName, CHAR_SZNAME, szName );
	}
};

inline bool operator < ( const GLCLUBALLIANCE &lvalue, const GLCLUBALLIANCE &rvalue )
{
	return lvalue.m_dwID < rvalue.m_dwID;
}

typedef std::map<DWORD,GLCLUBMEMBER>			CLUBMEMBERS;
typedef CLUBMEMBERS::iterator					CLUBMEMBERS_ITER;

typedef std::list<DWORD>						CLUB_CDM_MEM;
typedef CLUB_CDM_MEM::iterator					CLUB_CDM_MEM_ITER;

typedef std::set<DWORD>							CLUBCONFT;
typedef CLUBCONFT::iterator						CLUBCONFT_ITER;

typedef std::set<GLCLUBALLIANCE>				CLUB_ALLIANCE;
typedef CLUB_ALLIANCE::iterator					CLUB_ALLIANCE_ITER;


struct GLCLUBINFO
{
	DWORD		m_dwID;
	TCHAR		m_szName[CHAR_SZNAME];
		
	DWORD		m_dwMasterID;
	TCHAR		m_szMasterName[CHAR_SZNAME];
	DWORD		m_dwCDCertifior;

	DWORD		m_dwRank;

	DWORD		m_dwMarkVER;
	DWORD		m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];

	__time64_t	m_tOrganize;		//	클럽 창설 시간.
	__time64_t	m_tDissolution;		//	클럽 해체 예정 시간.
	__time64_t	m_tAuthority;		//	클럽마스터 위임 권한

	__time64_t	m_tAllianceSec;		//	동맹 탈퇴 혹은 제명 시간 + ( 가입 제한시간 )
	__time64_t	m_tAllianceDis;		//	동맹 해산 시간 + ( 창설 제한 시간 )

	LONGLONG	m_lnIncomeMoney;
	LONGLONG	m_lnStorageMoney;

	DWORD		m_dwBattleWin;		//	배틀 포인트 ( 승 )
	DWORD		m_dwBattleLose;		//	배틀 포인트 ( 패 )
	DWORD		m_dwBattleDraw;		//	배틀 포인트 ( 무)
	__time64_t	m_tLastBattle;		//	마지막 배틀 종료시간

	DWORD		m_dwAllianceBattleWin;		//	동맹배틀 포인트 ( 승 )
	DWORD		m_dwAllianceBattleLose;		//	동맹배틀 포인트 ( 패 )
	DWORD		m_dwAllianceBattleDraw;		//	동맹배틀 포인트 ( 무)

	TCHAR		m_szNotice[EMCLUB_NOTICE_LEN+1];

	GLCLUBINFO () 
		: m_dwID(0)
		, m_dwMasterID(0)
		, m_dwCDCertifior(0)
		, m_dwRank(0)
		, m_dwMarkVER(0)
		, m_lnIncomeMoney(0)
		, m_lnStorageMoney(0)
		, m_dwBattleWin(0)
		, m_dwBattleLose(0)
		, m_dwBattleDraw(0)
		, m_dwAllianceBattleWin(0)
		, m_dwAllianceBattleLose(0)
		, m_dwAllianceBattleDraw(0)
	{
		SecureZeroMemory( m_szName, sizeof(m_szName) );
		SecureZeroMemory( m_szMasterName, sizeof(m_szMasterName) );
		SecureZeroMemory( m_aryMark, sizeof(m_aryMark) );
		SecureZeroMemory( m_szNotice, sizeof(m_szNotice) );

		CTime tDisTime(1970,2,1,1,1,1);

		m_tOrganize = tDisTime.GetTime();
		m_tDissolution = tDisTime.GetTime();
		m_tAuthority = tDisTime.GetTime();
		m_tAllianceSec = tDisTime.GetTime();
		m_tAllianceDis = tDisTime.GetTime();
		m_tLastBattle = tDisTime.GetTime();
	}

	GLCLUBINFO ( const GLCLUBINFO& value )
	{
		operator=( value );
	}

public:
	GLCLUBINFO& operator = ( const GLCLUBINFO& value );
};

struct GLCLUBBATTLE_LOBY;

struct GLCLUBBATTLE
{
	enum CLUB_BATTLE_ENUM
	{
		CLUB_BATTLE_NOW				= 0,	// 진행중	
		CLUB_BATTLE_DRAW			= 1,	// 무승부
		CLUB_BATTLE_ARMISTICE		= 2,	// 휴전
		CLUB_BATTLE_SUBMISSION		= 3,	// 항복
		CLUB_BATTLE_SUBMISSION_WIN	= 4,	// 항복 승리
		CLUB_BATTLE_LOSE			= 5,	// 패배
		CLUB_BATTLE_WIN				= 6		// 승리
	};

	DWORD				m_dwCLUBID;					// 적대 클럽 ID
	__time64_t			m_tStartTime;				// 시작시간
	__time64_t			m_tEndTime;					// 종료시간
	WORD				m_wKillPointDB;				// 배틀 점수 DB
	WORD				m_wDeathPointDB;			// 배틀 점수 DB
	WORD				m_wKillPoint;				// 배틀 점수
	WORD				m_wDeathPoint;				// 배틀 점수
	WORD				m_wKillPointTemp;			// Update용 배틀 포인트( 필드용 )
	WORD				m_wDeathPointTemp;			// Update용 배틀 포인트( 필드용 )
	char				m_szClubName[CHAR_SZNAME];	// 상대클럽 이름 ( 클라용 )
	bool				m_bKillUpdate;				// Kill Update 확인
	CLUB_BATTLE_ENUM	m_nBattleFlag;				// 현재 배틀 상태 확인
	bool				m_bAlliance;				// 동맹 배틀 정보
	
	GLCLUBBATTLE()
		: m_dwCLUBID ( 0 )
		, m_wKillPointDB ( 0 )
		, m_wDeathPointDB ( 0 )
		, m_wKillPoint ( 0 )
		, m_wDeathPoint ( 0 )
		, m_wKillPointTemp( 0 )
		, m_wDeathPointTemp( 0 )
		, m_bKillUpdate ( false )
		, m_bAlliance( false )
		, m_nBattleFlag ( CLUB_BATTLE_NOW )
	{
		CTime tDisTime(1970,2,1,1,1,1);
		m_tStartTime = tDisTime.GetTime();
		m_tEndTime = tDisTime.GetTime();

		memset( m_szClubName, 0, sizeof(char)* CHAR_SZNAME );
	}
	
	GLCLUBBATTLE ( const GLCLUBBATTLE& value )
	{
		operator=( value );
	}

	GLCLUBBATTLE ( const GLCLUBBATTLE_LOBY& value )
	{
		operator=( value );
	}

public:
	GLCLUBBATTLE& operator = ( const GLCLUBBATTLE& value );
	GLCLUBBATTLE& operator = ( const GLCLUBBATTLE_LOBY& value );
};

// 삭제 예정인 클럽배틀 정보
struct GLCLUBBATTLE_DEL
{
	GLCLUBBATTLE	m_sClubBattle;		// 클럽 정보
	DWORD			m_dwField;			// 필드 정보 수락여부
	bool			m_bIsComplated;		// 정보 조합 끝났는지

	GLCLUBBATTLE_DEL()
		: m_dwField ( 0 )
		, m_bIsComplated ( false )
	{
	}
};

typedef std::map<DWORD,GLCLUBBATTLE>				CLUB_BATTLE;
typedef std::map<DWORD,GLCLUBBATTLE_DEL>			CLUB_BATTLE_DEL;
typedef CLUB_BATTLE::iterator						CLUB_BATTLE_ITER;
typedef CLUB_BATTLE_DEL::iterator					CLUB_BATTLE_DEL_ITER;


struct GLCLUBBATTLE_LOBY
{
	DWORD			m_dwCLUBID;					// 적대 클럽 ID
	__time64_t		m_tStartTime;				// 시작시간
	__time64_t		m_tEndTime;					// 종료시간
	WORD			m_wKillPoint;				// 배틀 점수
	WORD			m_wDeathPoint;				// 배틀 점수
	bool			m_bAlliance;
	char			m_szClubName[CHAR_SZNAME];	// 상대클럽 이름 ( 클라용 )

	GLCLUBBATTLE_LOBY() 
		: m_dwCLUBID(0)
		, m_tStartTime(0)
		, m_tEndTime(0)
		, m_wKillPoint(0)
		, m_wDeathPoint(0)
	{
		memset( m_szClubName, 0, sizeof( char ) * CHAR_SZNAME );
	}
	
	GLCLUBBATTLE_LOBY ( const GLCLUBBATTLE_LOBY& value )
	{
		operator=( value );
	}

	GLCLUBBATTLE_LOBY ( const GLCLUBBATTLE& value )
	{
		operator=( value );
	}

	GLCLUBBATTLE_LOBY& operator = ( const GLCLUBBATTLE_LOBY& value );
	GLCLUBBATTLE_LOBY& operator = ( const GLCLUBBATTLE& value );

};

class GLCLUB
{
public:
	DWORD			m_dwID;										//	클럽 ID.
	char			m_szName[CHAR_SZNAME];						//	클럽 이름.
		
	DWORD			m_dwMasterID;								//	클럽 마스터의 CharID
	char			m_szMasterName[CHAR_SZNAME];				//	클럽 마스터의 char name
	DWORD			m_dwCDCertifior;							//	선도 클럽 인증 가능 클럽 멤버.
			
	DWORD			m_dwRank;									//	클럽 등급

	DWORD			m_dwMarkVER;								//	클럽 마크 버전
	DWORD			m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];	//	클럽 마크 이미지 RGB.

	__time64_t		m_tOrganize;								//	클럽 창설 시간.
	__time64_t		m_tDissolution;								//	클럽 해체 예정 시간.
	__time64_t		m_tAuthority;								//	클럽마스터 위임 제한 시간

	__time64_t		m_tAllianceSec;								//	동맹 탈퇴 혹은 제명 시간 + ( 가입 제한시간 )
	__time64_t		m_tAllianceDis;								//	동맹 해산 시간 + ( 창설 제한 시간 )

	char			m_szNotice[EMCLUB_NOTICE_LEN+1];				//	클럽 공지.

	CLUBMEMBERS		m_mapMembers;								//	클럽원.
	CLUB_CDM_MEM	m_listCDMMembers;							//	CDM 클럽원.

	DWORD			m_dwAlliance;								//	동맹 클럽장.
	CLUB_ALLIANCE	m_setAlliance;								//	동맹 클럽들.

	CLUBCONFT		m_setConftSTART;							//	클럽 대련 시작시 맴버.
	CLUBCONFT		m_setConftCURRENT;							//	클럽 대련 진행 맴버.
	SCONFT_OPTION	m_conftOPT;									//	클럽 대련 옵션.
	DWORD			m_dwconftCLUB;								//	클럽 대련 상대 클럽.
	float			m_fconftTIMER;								//	클럽 대련 경과 시간.

	float			m_fTIMER_POS;								//	클럽원 위치 갱신 타이머.
	float			m_fTIMER_MONEY;								//	클럽 수익 갱신 타이머.
	float			m_fTIMER_BATTLE;

	BOOL			m_bVALID_STORAGE;							//	클럽 창고 정보 유효성 검사.
	LONGLONG		m_lnIncomeMoney;							//	클럽 수익 금액.
	LONGLONG		m_lnStorageMoney;							//	클럽 창고 금액.
	GLInventory		m_cStorage[MAX_CLUBSTORAGE];				//	클럽 창고 인벤토리.

	DWORD			m_dwBattleWin;								//	배틀 포인트 ( 승 )
	DWORD			m_dwBattleLose;								//	배틀 포인트 ( 패 )
	DWORD			m_dwBattleDraw;								//	배틀 포인트 ( 무)
	__time64_t		m_tLastBattle;								//	마지막 배틀 종료시간

	DWORD			m_dwAllianceBattleWin;						//	동맹배틀 포인트 ( 승 )
	DWORD			m_dwAllianceBattleLose;						//	동맹배틀 포인트 ( 패 )
	DWORD			m_dwAllianceBattleDraw;						//	동맹배틀 포인트 ( 무)

	CLUB_BATTLE		m_mapBattleReady;							//  대기중인 배틀정보
	CLUB_BATTLE		m_mapBattle;								//	배틀 정보
	CLUB_BATTLE_DEL	m_mapBattleDel;								//	삭제 예정인 배틀 정보

	DWORD			m_dwBattleTime;								//  배틀 클럽 진행시간. (temp)
	DWORD			m_dwBattleArmisticeReq;						//  배틀 휴전 요청한 클럽 ID ( temp )

	DWORD			m_dwReqClubID;								//	요청 클럽 ID

public:
	GLCLUB ();
	~GLCLUB ();

	GLCLUB ( const GLCLUB& value )
	{
		operator=( value );
	}

public:
	GLCLUB& operator = ( const GLCLUB& value );
	void SetInfo ( GLCLUBINFO& sINFO );

	BOOL SETSTORAGE_BYBUF ( CByteStream &ByteStream );
	BOOL GETSTORAGE_BYBUF ( CByteStream &ByteStream ) const;

public:
	bool DoDissolution ( __time64_t tDISSOLUTION );
	bool DoDissolutionCancel ();

public:
	bool IsRegDissolution ();
	bool CheckDissolution ();

public:
	bool AddMember ( DWORD dwCharID, const char *szCharName, DWORD dwFlags );
	bool DelMember ( DWORD dwCharID );

public:
	bool AddAlliance ( DWORD dwAlliance, const char *szClubName );
	bool DelAlliance ( DWORD dwAlliance );
	bool ClearAlliance ();

	void SetAuthority();
	void SetAllianceDis ();
	void SetAllianceSec ();

public:
	GLCLUBMEMBER* GetMember ( DWORD dwCharID );

public:
	void SetMemberState ( DWORD dwCharID, bool bONLINE, int nCHANNEL, SNATIVEID nidMAP );
	void SetMemberPos ( DWORD dwCharID, D3DXVECTOR3 vecPOS );

public:
	bool SetMemberFlag ( DWORD dwCharID, DWORD dwFlags );
	DWORD GetMemberFlag ( DWORD dwCharID );
	bool SetMemberCDMFlag ( DWORD dwCharID, BOOL bSet );
	bool SetAuthorityMaster( DWORD dwCharID );

public:
	bool IsMaster ( DWORD dwCharID );
	bool IsSubMaster ( DWORD dwCharID );

	bool IsAlliance();
	bool IsAllianceGuild ( DWORD dwGuildID );
	bool IsChief ();

	
	bool IsAuthority();
	bool IsAllianceDis ();
	bool IsAllianceSec ();

public:
	bool IsMemberFlgJoin ( DWORD dwCharID );
	bool IsMemberFlgKick ( DWORD dwCharID );
	bool IsMemberFlgNotice (DWORD dwCharID );
	bool IsMemberFlgCDCertify ( DWORD dwCharID );
	bool IsMemberFlgMarkChange ( DWORD dwCharID );
	bool IsMemberFlgCDM( DWORD dwCharID );

public:
	bool EnableCDMFlag( DWORD dwCharID );

public:
	bool IsMember ( DWORD dwCharID );
	DWORD GetNemberNum ()		{ return (DWORD) m_mapMembers.size(); }
	DWORD GetAllianceNum ()		{ return (DWORD) m_setAlliance.size(); }

public:
	void CLEARCONFT ();
	void SETCONFT_MEMBER ();
	DWORD GETCONFT_NUM ();

	void ADDCONFT_MEMBER ( DWORD dwID );
	void DELCONFT_MEMBER ( DWORD dwID );
	bool ISCONFT_MEMBER ( DWORD dwID );

	bool ISCONFTING ();
	bool ISCONFT_LOSS ();

public:
	void RESET ();
	void RESET_STORAGE ();

public:
	void SAVESTORAGE2DB ();

public:
	void ADDBATTLECLUB( const GLCLUBBATTLE& sBattleClub );
	void DELBATTLECLUB( DWORD dwClubID );
	void ADDBATTLE_READY_CLUB( const GLCLUBBATTLE& sBattleClub );
	void DELBATTLE_READY_CLUB( DWORD dwClubID );
	
	bool IsBattle( DWORD dwClubID );
	bool IsBattleAlliance( DWORD dwClubID );
	bool IsBattleReady( DWORD dwClubID );
	bool IsBattleStop( DWORD dwClubID );
	bool IsGuidBattle();

	void KillBattleClubMem( DWORD dwClubID, WORD nKill = 1 );
	void DeathBattleClubMem( DWORD dwClubID, WORD nDeath = 1  );
	
	DWORD GetBattleNum()	{ return (DWORD)m_mapBattle.size(); }
	DWORD GetBattleReadyNum() { return (DWORD)m_mapBattleReady.size(); }
	DWORD GetAllBattleNum() { return GetBattleNum() + GetBattleReadyNum(); }
	GLCLUBBATTLE* GetClubBattle( DWORD dwClubID );
	GLCLUBBATTLE* GetClubBattleReady( DWORD dwClubID );
	GLCLUBBATTLE_DEL* GetClubBattleDel( DWORD dwClubID );


	DWORD GetBattleDelNum() { return (DWORD)m_mapBattleDel.size(); }
	void SetLastBattlePoint( DWORD dwClubID, WORD dwKillPoint, WORD dwDeathPoint );
	void ReSetAllianceBattle();
};

typedef std::map<DWORD,GLCLUB>				CLUBS;
typedef CLUBS::iterator						CLUBS_ITER;
typedef CLUBS::reverse_iterator				CLUBS_RITER;

typedef std::map<std::string,DWORD>			CLUBSNAME;
typedef CLUBSNAME::iterator					CLUBSNAME_ITER;

class GLDBMan;

class GLClubMan
{
public:
	CLUBS		m_mapCLUB;
	CLUBSNAME	m_mapCLUBNAME;

public:
	DWORD FindClub ( std::string strName );
	GLCLUB* GetClub ( DWORD dwID );

public:
	void AddClub ( GLCLUB &cCLUB );
	bool DelClub ( DWORD dwID );

public:
	bool LoadFromDB( GLDBMan *pDBMan, bool bFieldServer );

	void Update();
	HRESULT FrameMoveAgent( float fTime, float fElapsedTime );
	HRESULT FrameMoveField( float fTime, float fElapsedTime );
	
	// 일반적인 상황
	HRESULT EndClubBattle( DWORD dwClubID_P, DWORD dwClubID_S );
	HRESULT EndAllianceBattle ( DWORD dwClubID_P, DWORD dwClubID_S );

	// 로드할때 배틀 시간이 초과한경우 삭제
	HRESULT EndClubBattleAgent( DWORD dwClubID_P, DWORD dwClubID_S );
	HRESULT EndClubBattleField( DWORD dwClubID_P, DWORD dwClubID_S );

	bool InitClubBattle( bool bFieldServer );
	void InitAllianceBattle ();
	void DelAllianceBattle( DWORD dwClub_P, DWORD dwClub_S );
	

public:
	GLClubMan ()
	{
	}

	~GLClubMan ()
	{
	}
};