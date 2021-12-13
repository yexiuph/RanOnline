#pragma once

#include <map>
#include <set>

#include "./GLCharDefine.h"
#include "./GLogicData.h"

struct GLCLUBMEMBER
{
	DWORD		dwID;                  // ĳ���� ��ȣ
	bool		bONLINE;               // �¶��λ���
	int			nCHANNEL;
	SNATIVEID	nidMAP;                // �ʹ�ȣ
	D3DXVECTOR3	vecPOS;                // ��ġ��ȣ
	TCHAR		m_szName[CHAR_SZNAME]; // ĳ���͸�
	DWORD		m_dwFlags;             // ��� ����

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

	__time64_t	m_tOrganize;		//	Ŭ�� â�� �ð�.
	__time64_t	m_tDissolution;		//	Ŭ�� ��ü ���� �ð�.
	__time64_t	m_tAuthority;		//	Ŭ�������� ���� ����

	__time64_t	m_tAllianceSec;		//	���� Ż�� Ȥ�� ���� �ð� + ( ���� ���ѽð� )
	__time64_t	m_tAllianceDis;		//	���� �ػ� �ð� + ( â�� ���� �ð� )

	LONGLONG	m_lnIncomeMoney;
	LONGLONG	m_lnStorageMoney;

	DWORD		m_dwBattleWin;		//	��Ʋ ����Ʈ ( �� )
	DWORD		m_dwBattleLose;		//	��Ʋ ����Ʈ ( �� )
	DWORD		m_dwBattleDraw;		//	��Ʋ ����Ʈ ( ��)
	__time64_t	m_tLastBattle;		//	������ ��Ʋ ����ð�

	DWORD		m_dwAllianceBattleWin;		//	���͹�Ʋ ����Ʈ ( �� )
	DWORD		m_dwAllianceBattleLose;		//	���͹�Ʋ ����Ʈ ( �� )
	DWORD		m_dwAllianceBattleDraw;		//	���͹�Ʋ ����Ʈ ( ��)

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
		CLUB_BATTLE_NOW				= 0,	// ������	
		CLUB_BATTLE_DRAW			= 1,	// ���º�
		CLUB_BATTLE_ARMISTICE		= 2,	// ����
		CLUB_BATTLE_SUBMISSION		= 3,	// �׺�
		CLUB_BATTLE_SUBMISSION_WIN	= 4,	// �׺� �¸�
		CLUB_BATTLE_LOSE			= 5,	// �й�
		CLUB_BATTLE_WIN				= 6		// �¸�
	};

	DWORD				m_dwCLUBID;					// ���� Ŭ�� ID
	__time64_t			m_tStartTime;				// ���۽ð�
	__time64_t			m_tEndTime;					// ����ð�
	WORD				m_wKillPointDB;				// ��Ʋ ���� DB
	WORD				m_wDeathPointDB;			// ��Ʋ ���� DB
	WORD				m_wKillPoint;				// ��Ʋ ����
	WORD				m_wDeathPoint;				// ��Ʋ ����
	WORD				m_wKillPointTemp;			// Update�� ��Ʋ ����Ʈ( �ʵ�� )
	WORD				m_wDeathPointTemp;			// Update�� ��Ʋ ����Ʈ( �ʵ�� )
	char				m_szClubName[CHAR_SZNAME];	// ���Ŭ�� �̸� ( Ŭ��� )
	bool				m_bKillUpdate;				// Kill Update Ȯ��
	CLUB_BATTLE_ENUM	m_nBattleFlag;				// ���� ��Ʋ ���� Ȯ��
	bool				m_bAlliance;				// ���� ��Ʋ ����
	
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

// ���� ������ Ŭ����Ʋ ����
struct GLCLUBBATTLE_DEL
{
	GLCLUBBATTLE	m_sClubBattle;		// Ŭ�� ����
	DWORD			m_dwField;			// �ʵ� ���� ��������
	bool			m_bIsComplated;		// ���� ���� ��������

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
	DWORD			m_dwCLUBID;					// ���� Ŭ�� ID
	__time64_t		m_tStartTime;				// ���۽ð�
	__time64_t		m_tEndTime;					// ����ð�
	WORD			m_wKillPoint;				// ��Ʋ ����
	WORD			m_wDeathPoint;				// ��Ʋ ����
	bool			m_bAlliance;
	char			m_szClubName[CHAR_SZNAME];	// ���Ŭ�� �̸� ( Ŭ��� )

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
	DWORD			m_dwID;										//	Ŭ�� ID.
	char			m_szName[CHAR_SZNAME];						//	Ŭ�� �̸�.
		
	DWORD			m_dwMasterID;								//	Ŭ�� �������� CharID
	char			m_szMasterName[CHAR_SZNAME];				//	Ŭ�� �������� char name
	DWORD			m_dwCDCertifior;							//	���� Ŭ�� ���� ���� Ŭ�� ���.
			
	DWORD			m_dwRank;									//	Ŭ�� ���

	DWORD			m_dwMarkVER;								//	Ŭ�� ��ũ ����
	DWORD			m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY];	//	Ŭ�� ��ũ �̹��� RGB.

	__time64_t		m_tOrganize;								//	Ŭ�� â�� �ð�.
	__time64_t		m_tDissolution;								//	Ŭ�� ��ü ���� �ð�.
	__time64_t		m_tAuthority;								//	Ŭ�������� ���� ���� �ð�

	__time64_t		m_tAllianceSec;								//	���� Ż�� Ȥ�� ���� �ð� + ( ���� ���ѽð� )
	__time64_t		m_tAllianceDis;								//	���� �ػ� �ð� + ( â�� ���� �ð� )

	char			m_szNotice[EMCLUB_NOTICE_LEN+1];				//	Ŭ�� ����.

	CLUBMEMBERS		m_mapMembers;								//	Ŭ����.
	CLUB_CDM_MEM	m_listCDMMembers;							//	CDM Ŭ����.

	DWORD			m_dwAlliance;								//	���� Ŭ����.
	CLUB_ALLIANCE	m_setAlliance;								//	���� Ŭ����.

	CLUBCONFT		m_setConftSTART;							//	Ŭ�� ��� ���۽� �ɹ�.
	CLUBCONFT		m_setConftCURRENT;							//	Ŭ�� ��� ���� �ɹ�.
	SCONFT_OPTION	m_conftOPT;									//	Ŭ�� ��� �ɼ�.
	DWORD			m_dwconftCLUB;								//	Ŭ�� ��� ��� Ŭ��.
	float			m_fconftTIMER;								//	Ŭ�� ��� ��� �ð�.

	float			m_fTIMER_POS;								//	Ŭ���� ��ġ ���� Ÿ�̸�.
	float			m_fTIMER_MONEY;								//	Ŭ�� ���� ���� Ÿ�̸�.
	float			m_fTIMER_BATTLE;

	BOOL			m_bVALID_STORAGE;							//	Ŭ�� â�� ���� ��ȿ�� �˻�.
	LONGLONG		m_lnIncomeMoney;							//	Ŭ�� ���� �ݾ�.
	LONGLONG		m_lnStorageMoney;							//	Ŭ�� â�� �ݾ�.
	GLInventory		m_cStorage[MAX_CLUBSTORAGE];				//	Ŭ�� â�� �κ��丮.

	DWORD			m_dwBattleWin;								//	��Ʋ ����Ʈ ( �� )
	DWORD			m_dwBattleLose;								//	��Ʋ ����Ʈ ( �� )
	DWORD			m_dwBattleDraw;								//	��Ʋ ����Ʈ ( ��)
	__time64_t		m_tLastBattle;								//	������ ��Ʋ ����ð�

	DWORD			m_dwAllianceBattleWin;						//	���͹�Ʋ ����Ʈ ( �� )
	DWORD			m_dwAllianceBattleLose;						//	���͹�Ʋ ����Ʈ ( �� )
	DWORD			m_dwAllianceBattleDraw;						//	���͹�Ʋ ����Ʈ ( ��)

	CLUB_BATTLE		m_mapBattleReady;							//  ������� ��Ʋ����
	CLUB_BATTLE		m_mapBattle;								//	��Ʋ ����
	CLUB_BATTLE_DEL	m_mapBattleDel;								//	���� ������ ��Ʋ ����

	DWORD			m_dwBattleTime;								//  ��Ʋ Ŭ�� ����ð�. (temp)
	DWORD			m_dwBattleArmisticeReq;						//  ��Ʋ ���� ��û�� Ŭ�� ID ( temp )

	DWORD			m_dwReqClubID;								//	��û Ŭ�� ID

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
	
	// �Ϲ����� ��Ȳ
	HRESULT EndClubBattle( DWORD dwClubID_P, DWORD dwClubID_S );
	HRESULT EndAllianceBattle ( DWORD dwClubID_P, DWORD dwClubID_S );

	// �ε��Ҷ� ��Ʋ �ð��� �ʰ��Ѱ�� ����
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