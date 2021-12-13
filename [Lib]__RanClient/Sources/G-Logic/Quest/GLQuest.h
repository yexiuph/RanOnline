#ifndef GLQUEST_H_
#define GLQUEST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "./GLItem.h"
#include "./GLogicData.h"

#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__Engine/Sources/Common/SerialFile.h"
#include "../[Lib]__Engine/Sources/COMMON/StringFile.h"

enum EMQUESTFLAGS
{
	EMQF_REPEAT		= 0x0001,		//	완료된 퀘스트 수행 가능 여부.
	EMQF_AGAIN		= 0x0002,		//	실패한 퀘스트 수행 가능 여부.
	EMQF_ONLYONE	= 0x0004,		//	이 퀘스트는 다른 퀘스트와 동시 진행 불가능.
	EMQF_GIVEUP		= 0x0008,		//	퀘스트 포기 가능 여부.
	
	EMQF_PARTY		= 0x1000,		//	파티만 참여 가능한 퀘스트.
	EMQF_PARTYQUEST	= 0x2000,		//	파티원 동시 진행 퀘스트.
};


enum EMQUESTPROGRESS
{
	EMQP_NON_EVENT = 0, // 이벤트 없음
	EMQP_CHAR_DEAD,		// 캐릭터 사망
	EMQP_START_MOVE,	// 시작지점으로 이동
    EMQP_SELECT_MOVE,   // 지정된 위치로 이동
	EMQP_SIZE,
};

enum EMFAILTYPE
{
	EMFAIL_NONE		= 0x0000,		// 타입없음
	EMFAIL_TIMEOVER = 0x0001,		// 타임오버로 실패시
	EMFAIL_DIE      = 0x0002,		// 죽어서 실패시
	EMFAIL_LEAVE    = 0x0004,		// 특정 맵에서 떠나서 실패시
};

typedef std::vector<DWORD>			DWQARRAY;
typedef DWQARRAY::iterator			DWQARRAY_ITER;

struct GLQUEST_PROGRESS_MAP
{
	SNATIVEID	nidMAP;
	WORD		wPosX;
	WORD		wPosY;
	DWORD		dwGateID;

	GLQUEST_PROGRESS_MAP() :
	nidMAP(false),
		wPosX(USHRT_MAX),
		wPosY(USHRT_MAX),
		dwGateID(0)
	{
	}

	void Init()
	{
		nidMAP   = false;
		wPosX    = USHRT_MAX;
		wPosY    = USHRT_MAX;
		dwGateID = 0;
	}
};


struct SGENQUESTITEM
{
	enum { VERSION = 0x0001, };

	SNATIVEID	sNID;
	WORD		wNUM;
	float		fGEN_RATE;
	DWQARRAY	vecGEN_MOB;

	SGENQUESTITEM () :
		sNID(false),
		wNUM(1),
		fGEN_RATE(10.0f)
	{
	}

	SGENQUESTITEM& operator= ( const SGENQUESTITEM& rvalue );

	bool Find ( DWORD dwMOB );
	void GENMOB_ERASE ( DWORD dwIndex );

	bool LOAD ( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );
};

typedef std::vector<SGENQUESTITEM>	GENMOBITEMARRAY;
typedef GENMOBITEMARRAY::iterator	GENMOBITEMARRAY_ITER;

typedef std::vector<SITEMCUSTOM>	INVENQARRAY;
typedef INVENQARRAY::iterator		INVENQARRAY_ITER;

struct GLQUEST_STEP
{
	enum { VERSION = 0x0011, };

	std::string		m_strTITLE;				//	현재 단계에 대한 제목.
	std::string		m_strCOMMENT;			//	현재 단계에 대한 설명.

	//	이수 조건 #1.
	std::string		m_strOBJ_NPCTALK;		//	이수조건 설명.
	DWORD			m_dwNID_NPCTALK;		//	대화가 필요한 NPC ID.

	//	이수 조건 #2.
	std::string		m_strOBJ_MOBGEN_QITEM;	//	이수조건 설명.
	GENMOBITEMARRAY	m_vecMOBGEN_QITEM;		//	MOB 발생 퀘스트 아이템.
	
	//	이수 조건 #3.
	std::string		m_strOBJ_MOBKILL;		//	이수조건 설명.
	DWORD			m_dwNID_MOBKILL;		//	죽여야할 Mob ID.
	DWORD			m_dwNUM_MOBKILL;		//	죽여야할 Mob 숫자.

	//	이수 조건 #4.
	std::string		m_strOBJ_NPCGUARD;		//	이수조건 설명.
	DWORD			m_dwNID_NPCGUARD;		//	보호해야할 NPC.

	//	이수 조건 #5.
	std::string		m_strOBJ_REACH_ZONE;	//	이수 조건 설명.
	SNATIVEID		m_sMAPID_REACH;			//	도달해야할 MAPID.
	WORD			m_wPOSX_REACH;			//	도달해야할 x좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wPOSY_REACH;			//	도달해야할 y좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wRADIUS_REACH;		//	도달 반경.

	//	이수 조건 #6.
	std::string		m_strOBJ_DEFENSE_ZONE;	//	이수 조건 설명.
	SNATIVEID		m_sMAPID_DEFENSE;		//	방어해야할 MAPID.
	WORD			m_wPOSX_DEFENSE;		//	방어해야할 x좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wPOSY_DEFENSE;		//	방어해야할 y좌표.	( 미니멥에서 쓰는 좌표. )
	WORD			m_wRADIUS_DEFENSE;		//	방어 반경.
	float			m_fDEFENSE_TIME;		//	방어해야할 시간.

	//	이수 조건 #7.
	WORD			m_wLevel;				//	레벨 도달.

	//	완료시 처리.
	DWQARRAY			 m_vecRESET_QITEM;		//	다음 스텟으로 넘어갈때 리셋될 Quest Item.
	INVENQARRAY			 m_vecGIFT_QITEM;		//	현제 STEP 완료시 보상 Quest Item.
	GLQUEST_PROGRESS_MAP m_stepMoveMap;

	GLQUEST_STEP () :
		m_strTITLE("현제 단계에 대한 제목"),
		m_strCOMMENT("현제 단계에 대한 설명"),

		m_strOBJ_NPCTALK("대화해야함"),
		m_dwNID_NPCTALK(UINT_MAX),

		m_strOBJ_MOBKILL("죽여야할몹"),
		m_dwNID_MOBKILL(UINT_MAX),
		m_dwNUM_MOBKILL(0),

		m_strOBJ_MOBGEN_QITEM("몹에게서습득할아이템"),

		m_strOBJ_NPCGUARD("보호해야함"),
		m_dwNID_NPCGUARD(UINT_MAX),

		m_strOBJ_REACH_ZONE("어느위치도달"),
		m_sMAPID_REACH(false),
		m_wPOSX_REACH(0),
		m_wPOSY_REACH(0),
		m_wRADIUS_REACH(60),

		m_strOBJ_DEFENSE_ZONE("어느위치방어"),
		m_sMAPID_DEFENSE(false),
		m_wPOSX_DEFENSE(0),
		m_wPOSY_DEFENSE(0),
		m_wRADIUS_DEFENSE(0),
		m_fDEFENSE_TIME(0.0f),

		m_wLevel(USHRT_MAX)
	{
	}

	const char* GetTITLE () const				{ return m_strTITLE.c_str(); }
	const char* GetCOMMENT () const				{ return m_strCOMMENT.c_str(); }
	
	const char* GetOBJ_NPCTALK () const			{ return m_strOBJ_NPCTALK.c_str(); }
	const char* GetOBJ_MOBGEN_QITEM () const	{ return m_strOBJ_MOBGEN_QITEM.c_str(); }
	const char* GetOBJ_MOBKILL () const			{ return m_strOBJ_MOBKILL.c_str(); }
	const char* GetOBJ_NPCGUARD () const		{ return m_strOBJ_NPCGUARD.c_str(); }
	const char* GetOBJ_REACH_ZONE () const		{ return m_strOBJ_REACH_ZONE.c_str(); }
	const char* GetOBJ_DEFENSE_ZONE () const	{ return m_strOBJ_DEFENSE_ZONE.c_str(); }

	bool IsNEED_NPCTALK () const		{ return (m_dwNID_NPCTALK!=UINT_MAX); }
	bool IsNEED_QITEM () const			{ return (!m_vecMOBGEN_QITEM.empty()); }
	bool IsNEED_MOBKILL () const		{ return (m_dwNID_MOBKILL!=UINT_MAX); }
	bool IsNEED_NPCGUARD () const		{ return (m_dwNID_NPCGUARD!=UINT_MAX); }
	bool IsNEED_REACHZONE () const		{ return (m_sMAPID_REACH!=SNATIVEID(false)); }
	bool IsNEED_DEFENSEZONE () const	{ return (m_sMAPID_DEFENSE!=SNATIVEID(false)); }
	bool IsNEED_LEVEL () const			{ return (m_wLevel!=USHRT_MAX); }

	void RESET_NPCTALK ()
	{
		m_strOBJ_NPCTALK = "";
		m_dwNID_NPCTALK = UINT_MAX;
	}

	void RESET_MOBGEN_QITEM ()
	{
		m_strOBJ_MOBGEN_QITEM = "";
		m_vecMOBGEN_QITEM.clear();
	}

	void RESET_MOBKILL ()
	{
		m_strOBJ_MOBKILL = "";
		m_dwNID_MOBKILL = UINT_MAX;
		m_dwNUM_MOBKILL = 0;
	}

	void RESET_NPCGUARD ()
	{
		m_strOBJ_NPCGUARD = "";
		m_dwNID_NPCGUARD = UINT_MAX;
	}

	void RESET_REACHZONE ()
	{
		m_strOBJ_REACH_ZONE = "";
		m_sMAPID_REACH = SNATIVEID(false);
		m_wPOSX_REACH = (0);
		m_wPOSY_REACH = (0);
		m_wRADIUS_REACH = (60);
	}

	void RESET_DEFENSE ()
	{
		m_strOBJ_DEFENSE_ZONE = "";
		m_sMAPID_DEFENSE = SNATIVEID(false);
		m_wPOSX_DEFENSE = (0);
		m_wPOSY_DEFENSE = (0);
		m_wRADIUS_DEFENSE = (0);
		m_fDEFENSE_TIME = (0.0f);
	}

	void RESET_LEVEL ()
	{
		m_wLevel = USHRT_MAX;
	}

	bool LOAD_0001 ( basestream &SFile );
	bool LOAD_0002 ( basestream &SFile );
	bool LOAD_0003 ( basestream &SFile );
	bool LOAD_0004 ( basestream &SFile );
	bool LOAD_0005 ( basestream &SFile );
	bool LOAD_0006 ( basestream &SFile );
	bool LOAD_0007 ( basestream &SFile );
	bool LOAD_0008 ( basestream &SFile );
	bool LOAD_0009 ( basestream &SFile );
	bool LOAD_0010 ( basestream &SFile );
	bool LOAD_0011 ( basestream &SFile );

	bool LOAD ( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );

	GLQUEST_STEP& operator= ( const GLQUEST_STEP &rVALUE );

	void RESETITEM_ERASE ( DWORD dwIndex );
	void MOBGENITEM_ERASE ( DWORD dwIndex );
	void GIFTITEM_ERASE ( DWORD dwIndex );
};

struct GLQUEST_START
{
	enum { VERSION = 0x0002, };

	DWORD		dwCLASS;
	WORD		wSCHOOL;

	SNATIVEID	nidITEM;
	SNATIVEID	nidSKILL;
	
	SNATIVEID	nidMAP;
	WORD		wPosX;
	WORD		wPosY;
	WORD		wSIZE;

	WORD		wLEVEL;

	GLQUEST_START () :
		dwCLASS(GLCC_ALL),
		wSCHOOL(MAX_SCHOOL),

		nidITEM(false),
		nidSKILL(false),

		nidMAP(false),
		wPosX(USHRT_MAX),
		wPosY(USHRT_MAX),
		wSIZE(0),

		wLEVEL(USHRT_MAX)
	{
	}

	bool LOAD ( basestream &SFile );
	bool LOAD_001( basestream &SFile );
	bool LOAD_002( basestream &SFile );
	bool SAVE ( CSerialFile &SFile );
};


struct GLQUEST_PROGRESS 
{
	DWORD dwStartProgress;  // 시작시 이벤트
	DWORD dwEndProgress;	  // 종료시 이벤트
	DWORD dwFailProgress;	  // 실패시 이벤트
		
	DWORD			dwfailType;		  // 실패의 조건

	GLQUEST_PROGRESS_MAP startMap;
	GLQUEST_PROGRESS_MAP endMap;
	GLQUEST_PROGRESS_MAP failMap;

	bool				 bUseProgressEvent;


	GLQUEST_PROGRESS()
	{
		Init();
	}

	VOID Init()
	{
		dwStartProgress = EMQP_NON_EVENT;
		dwEndProgress   = EMQP_NON_EVENT;
		dwFailProgress  = EMQP_NON_EVENT;

		dwfailType    = 0;

		startMap.Init();	
		endMap.Init();
		failMap.Init();

		bUseProgressEvent = FALSE;
	};

	bool IsTIMEOVER ()		{ return (dwfailType&EMFAIL_TIMEOVER)!=NULL; }
	bool IsDIE ()			{ return (dwfailType&EMFAIL_DIE)!=NULL; }	
	bool IsLEAVE ()			{ return (dwfailType&EMFAIL_LEAVE)!=NULL; }	
    
};

class GLQUEST
{
public:
	enum { VERSION = 0x0012, };

	typedef std::vector<GLQUEST_STEP>	VECQSTEP;
	typedef VECQSTEP::iterator			VECQSTEP_ITER;

public:
	SNATIVEID		m_sNID;					//	고유 ID.
	DWORD			m_dwEDITVER;			//	편집 버전.
											//	( 종전 퀘스트 설정과 완전히 구도가 틀려저서 종전 설정으로 
											//	플레이한 사람이 진행하기 불가능할 경우. )
	DWORD			m_dwFlags;				//	속성.

	std::string		m_strTITLE;				//	퀘스트 타이틀.
	std::string		m_strCOMMENT;			//	퀘스트 설명.

	GLQUEST_START	m_sSTARTOPT;			//	자동 시작 조건.

	//	최초 부여 조건.
	DWORD			m_dwBeginMoney;			//	퀘스트 받기 위한 비용.
	DWORD			m_dwBeginPartyMemNum;	//	퀘스트를 받기 위한 최소 파티 인원.

	//	진행 조건.
	DWORD			m_dwLimitTime;			//	제한 시간.	( 실제 시간의 분단위. )
	DWORD			m_dwLimitPartyMemNum;	//	제한 파티 인원.
	bool			m_bNonDie;				//	수행 도중 사망하면 안됨.

	VECQSTEP		m_vecProgStep;			//	전체 진행 스텝 정보.

	//	완료시 보상.
	DWORD			m_dwGiftEXP;
	DWORD			m_dwGiftMONEY;
	DWORD			m_dwGiftELEMENT;
	DWORD			m_dwGiftLIFEPOINT;
	DWORD			m_dwGiftDisPK;
	DWORD			m_dwGiftSKILLPOINT;
	DWORD			m_dwGiftSTATSPOINT;

	INVENQARRAY		m_vecGiftITEM;
	DWQARRAY		m_vecGiftSKILL;
	DWORD			m_dwGiftQUEST;

	//	퀘스트 진행 중에 적용되는 이벤트
	GLQUEST_PROGRESS m_ProgressEvent;

public:
	const char* GetTITLE()			{ return m_strTITLE.c_str(); }
	DWORD GetSTEPNUM ()				{ return (DWORD) m_vecProgStep.size(); }
	GLQUEST::VECQSTEP& GetSTEP ()	{ return m_vecProgStep; }
	GLQUEST_STEP* GetSTEP ( DWORD dwSTEP );

public:
	DWORD ToUpStep ( DWORD dwSTEP );
	DWORD ToDownStep ( DWORD dwSTEP );

public:
	bool IsREPEAT ()		{ return (m_dwFlags&EMQF_REPEAT)!=NULL; }		//	여러번 수행 가능.
	bool IsAGAIN ()			{ return (m_dwFlags&EMQF_AGAIN)!=NULL; }		//	(포기한것) 다시 수행 가능.
	bool IsPARTY ()			{ return (m_dwFlags&EMQF_PARTY)!=NULL; }		//	파티일때 수행 가능.
	bool IsPARTYQUEST ()	{ return (m_dwFlags&EMQF_PARTYQUEST)!=NULL; }	//	파티원 동시 진행 퀘스트.

	bool IsONLYONE ()		{ return (m_dwFlags&EMQF_ONLYONE)!=NULL; }		//	동시에 진행불가?
	bool IsGIVEUP ()		{ return (m_dwFlags&EMQF_GIVEUP)!=NULL; }		//	포기 가능
	
	bool IsLIMITTIME ()		{ return m_dwLimitTime!=0; }
	bool IsNonDIE ()		{ return m_bNonDie; }

protected:
	bool LOAD_0001 ( basestream &SFile );
	bool LOAD_0002 ( basestream &SFile );
	bool LOAD_0003 ( basestream &SFile );
	bool LOAD_0004 ( basestream &SFile );
	bool LOAD_0005 ( basestream &SFile );
	bool LOAD_0006 ( basestream &SFile );
	bool LOAD_0007 ( basestream &SFile );
	bool LOAD_0008 ( basestream &SFile );
	bool LOAD_0009 ( basestream &SFile );
	bool LOAD_0010 ( basestream &SFile );
	bool LOAD_0011 ( basestream &SFile );
	bool LOAD_0012 ( basestream &SFile );

public:
	bool LOAD ( const char *szFile );
	bool SAVE ( const char *szFile );

	void SaveCsv( std::fstream &SFile, CString strFileName );
	void SaveCsvInfoHead( std::fstream &SFile, CString strFileName );
	void SaveCsvBaseHead( std::fstream &SFile );
	void SaveCsvInfoData( std::fstream &SFile );
	void SaveCsvBaseData( std::fstream &SFile );
	void SaveCsvProgress( std::fstream &SFile );
	void SaveCsvProgressBaseHead( std::fstream &SFile );
	void SaveCsvProgressBaseData( std::fstream &SFile );

	void LoadCsv( CStringArray &StrArray, CStringArray &StrArrayProgress );
	void LoadCsvInfoDate( CStringArray &StrArray );
	void LoadCsvBaseData( CStringArray &StrArray );
	void LoadCsvProgress( CStringArray &StrArray );
	void LoadCsvProgressBaseHead( CStringArray &StrArray );
	void LoadCsvProgressBaseData( CStringArray &StrArray );

	void ExportText( std::fstream &SFile );
	void ExportTextSubJect( std::fstream &SFile );
	void ExportTextProgress( std::fstream &SFile );
	void ImportText( CStringFile& strFile );
	void ImportTextSubject( CStringFile& strFile );
	void ImportTextProgress( CStringFile& strFile );

public:
	GLQUEST () :
		m_dwFlags(0),
		m_dwEDITVER(1),

		m_strTITLE("퀘스트 제목"),
		m_strCOMMENT("퀘스트에 대한 설명 부분."),

		m_dwBeginMoney(0),
		m_dwBeginPartyMemNum(0),

		m_dwLimitTime(0),
		m_dwLimitPartyMemNum(0),
		m_bNonDie(false),

		m_dwGiftEXP(0),
		m_dwGiftMONEY(0),
		m_dwGiftELEMENT(0),
		m_dwGiftLIFEPOINT(0),
		m_dwGiftDisPK(0),
		
		m_dwGiftSKILLPOINT(0),
		m_dwGiftSTATSPOINT(0),
		m_dwGiftQUEST(UINT_MAX)
	{
	}

	GLQUEST& operator= ( const GLQUEST &rVALUE );
};

enum EMQUESTPROG
{
	EMQP_REQREADING		= 0x0001,	// 퀘스트를 열어서 읽어보기를 요청.
};

struct GLQUESTPROG_STEP
{
	enum { VERSION = 0x0002, };

	//	이수 조건.
	bool		m_bTALK;					//	대화했는지 여부.
	bool		m_bQITEM;					//	필요 퀘스트 아이템 결과.

	bool		m_bMOBKILL_ALL;				//	죽여야할 Mob 모두 죽임.
	DWORD		m_dwNUM_MOBKILL;			//	죽인 Mob 숫자.
	bool		m_bNPCGUARD_DEATH;			//	보호해야할 NPC 사망 여부.

	bool		m_bREACH_ZONE;				//	도달 위치에 도착 여부.
	
	bool		m_bDEFENSE_ZONE_BEGIN;		//	방어 위치 사수 시작 여부.
	bool		m_bDEFENSE_ZONE_COMPLETE;	//	방어 위치 사수 완료.
	float		m_fDEFENSE_ZONE_TIMER;		//	방어 위치 사수 경과시간.

	bool		m_bLevel;

public:
	GLQUESTPROG_STEP () :
		m_bTALK(true),
		m_bQITEM(true),

		m_bMOBKILL_ALL(true),
		m_dwNUM_MOBKILL(UINT_MAX),
		m_bNPCGUARD_DEATH(false),

		m_bREACH_ZONE(true),

		m_bDEFENSE_ZONE_BEGIN(true),
		m_bDEFENSE_ZONE_COMPLETE(true),
		m_fDEFENSE_ZONE_TIMER(FLT_MAX),

		m_bLevel(true)
	{
	}

public:
	bool BEGIN_PROG ( const GLQUEST_STEP &sSTEP );
	void RESET ();

public:
	bool IsCOMPLETE ();

public:
	GLQUESTPROG_STEP& operator= ( const GLQUESTPROG_STEP &rVALUE );

public:
	BOOL SET_BYBUFFER ( CByteStream &ByteStream ) const;
	BOOL GET_BYBUFFER ( CByteStream &ByteStream );
};

class GLQUESTPROG
{
public:
	enum { VERSION = 0x0002, INVEN_SIZEX=8, INVEN_SIZEY=1 };

	typedef std::vector<DWORD>	DWARRAY;
	typedef DWARRAY::iterator	DWARRAY_ITER;

public:
	SNATIVEID	m_sNID;					//	고유 ID.
	DWORD		m_dwEDITVER;
	DWORD		m_dwFlags;				//	속성.
	DWORD		m_dwCOUNT;				//	시도 횟수.

	bool		m_bCOMPLETE;			//	완료 여부.
	float		m_fLAPSTIME;			//	시작후 경과 시간.

	DWORD				m_dwSTEP;		//	현재 진행 퀘스트 STEP.
	GLQUESTPROG_STEP	m_sSTEP_PROG;	//	현재 진행 정보.

	GLInventory			m_sINVENTORY;	//	퀘스트 전용 인벤.

	__time64_t			m_tStartTime;		//	퀘스트 시작 시간

public:
	bool START ( GLQUEST* pQUEST, GLQUESTPROG *pOLD_PROG );
	bool DoPROGRESS ( bool &_bModifyInven, GLQUEST_PROGRESS_MAP &stepMoveMap );
	void DoGIVEUP ();
	bool DoCOMPLETE ();

	bool CheckCOMPLETE ();
	bool CheckTIMEOVER ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	void ResetReqREADING ()	{ m_dwFlags &= ~EMQP_REQREADING; }

public:
	void ReqREADING ()		{ m_dwFlags |= EMQP_REQREADING; }
	bool IsReqREADING ()	{ return (m_dwFlags&EMQP_REQREADING)!=NULL; }
	bool IsNonDIE ();

public:
	GLQUESTPROG& operator= ( const GLQUESTPROG &rVALUE );

public:
	BOOL SET_BYBUFFER ( CByteStream &ByteStream ) const;
	BOOL GET_BYBUFFER ( CByteStream &ByteStream );

public:
	BOOL LOAD_001 ( CByteStream &ByteStream );
	BOOL LOAD_002 ( CByteStream &ByteStream );

public:
	GLQUESTPROG () :
		m_sNID(false),
		m_dwEDITVER(1),
		m_dwFlags(NULL),
		m_bCOMPLETE(false),
		m_dwCOUNT(0),
		m_fLAPSTIME(0.0f),
		m_tStartTime(0),
		m_dwSTEP(0)
	{
		m_sINVENTORY.SetState ( INVEN_SIZEX, INVEN_SIZEY );
	}
};

namespace COMMENT
{
	extern std::string QUEST_PROGRESS[EMQP_SIZE];
}

#endif // GLQUEST_H_