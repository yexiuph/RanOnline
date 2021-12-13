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
	EMQF_REPEAT		= 0x0001,		//	�Ϸ�� ����Ʈ ���� ���� ����.
	EMQF_AGAIN		= 0x0002,		//	������ ����Ʈ ���� ���� ����.
	EMQF_ONLYONE	= 0x0004,		//	�� ����Ʈ�� �ٸ� ����Ʈ�� ���� ���� �Ұ���.
	EMQF_GIVEUP		= 0x0008,		//	����Ʈ ���� ���� ����.
	
	EMQF_PARTY		= 0x1000,		//	��Ƽ�� ���� ������ ����Ʈ.
	EMQF_PARTYQUEST	= 0x2000,		//	��Ƽ�� ���� ���� ����Ʈ.
};


enum EMQUESTPROGRESS
{
	EMQP_NON_EVENT = 0, // �̺�Ʈ ����
	EMQP_CHAR_DEAD,		// ĳ���� ���
	EMQP_START_MOVE,	// ������������ �̵�
    EMQP_SELECT_MOVE,   // ������ ��ġ�� �̵�
	EMQP_SIZE,
};

enum EMFAILTYPE
{
	EMFAIL_NONE		= 0x0000,		// Ÿ�Ծ���
	EMFAIL_TIMEOVER = 0x0001,		// Ÿ�ӿ����� ���н�
	EMFAIL_DIE      = 0x0002,		// �׾ ���н�
	EMFAIL_LEAVE    = 0x0004,		// Ư�� �ʿ��� ������ ���н�
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

	std::string		m_strTITLE;				//	���� �ܰ迡 ���� ����.
	std::string		m_strCOMMENT;			//	���� �ܰ迡 ���� ����.

	//	�̼� ���� #1.
	std::string		m_strOBJ_NPCTALK;		//	�̼����� ����.
	DWORD			m_dwNID_NPCTALK;		//	��ȭ�� �ʿ��� NPC ID.

	//	�̼� ���� #2.
	std::string		m_strOBJ_MOBGEN_QITEM;	//	�̼����� ����.
	GENMOBITEMARRAY	m_vecMOBGEN_QITEM;		//	MOB �߻� ����Ʈ ������.
	
	//	�̼� ���� #3.
	std::string		m_strOBJ_MOBKILL;		//	�̼����� ����.
	DWORD			m_dwNID_MOBKILL;		//	�׿����� Mob ID.
	DWORD			m_dwNUM_MOBKILL;		//	�׿����� Mob ����.

	//	�̼� ���� #4.
	std::string		m_strOBJ_NPCGUARD;		//	�̼����� ����.
	DWORD			m_dwNID_NPCGUARD;		//	��ȣ�ؾ��� NPC.

	//	�̼� ���� #5.
	std::string		m_strOBJ_REACH_ZONE;	//	�̼� ���� ����.
	SNATIVEID		m_sMAPID_REACH;			//	�����ؾ��� MAPID.
	WORD			m_wPOSX_REACH;			//	�����ؾ��� x��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wPOSY_REACH;			//	�����ؾ��� y��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wRADIUS_REACH;		//	���� �ݰ�.

	//	�̼� ���� #6.
	std::string		m_strOBJ_DEFENSE_ZONE;	//	�̼� ���� ����.
	SNATIVEID		m_sMAPID_DEFENSE;		//	����ؾ��� MAPID.
	WORD			m_wPOSX_DEFENSE;		//	����ؾ��� x��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wPOSY_DEFENSE;		//	����ؾ��� y��ǥ.	( �̴ϸ㿡�� ���� ��ǥ. )
	WORD			m_wRADIUS_DEFENSE;		//	��� �ݰ�.
	float			m_fDEFENSE_TIME;		//	����ؾ��� �ð�.

	//	�̼� ���� #7.
	WORD			m_wLevel;				//	���� ����.

	//	�Ϸ�� ó��.
	DWQARRAY			 m_vecRESET_QITEM;		//	���� �������� �Ѿ�� ���µ� Quest Item.
	INVENQARRAY			 m_vecGIFT_QITEM;		//	���� STEP �Ϸ�� ���� Quest Item.
	GLQUEST_PROGRESS_MAP m_stepMoveMap;

	GLQUEST_STEP () :
		m_strTITLE("���� �ܰ迡 ���� ����"),
		m_strCOMMENT("���� �ܰ迡 ���� ����"),

		m_strOBJ_NPCTALK("��ȭ�ؾ���"),
		m_dwNID_NPCTALK(UINT_MAX),

		m_strOBJ_MOBKILL("�׿����Ҹ�"),
		m_dwNID_MOBKILL(UINT_MAX),
		m_dwNUM_MOBKILL(0),

		m_strOBJ_MOBGEN_QITEM("�����Լ������Ҿ�����"),

		m_strOBJ_NPCGUARD("��ȣ�ؾ���"),
		m_dwNID_NPCGUARD(UINT_MAX),

		m_strOBJ_REACH_ZONE("�����ġ����"),
		m_sMAPID_REACH(false),
		m_wPOSX_REACH(0),
		m_wPOSY_REACH(0),
		m_wRADIUS_REACH(60),

		m_strOBJ_DEFENSE_ZONE("�����ġ���"),
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
	DWORD dwStartProgress;  // ���۽� �̺�Ʈ
	DWORD dwEndProgress;	  // ����� �̺�Ʈ
	DWORD dwFailProgress;	  // ���н� �̺�Ʈ
		
	DWORD			dwfailType;		  // ������ ����

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
	SNATIVEID		m_sNID;					//	���� ID.
	DWORD			m_dwEDITVER;			//	���� ����.
											//	( ���� ����Ʈ ������ ������ ������ Ʋ������ ���� �������� 
											//	�÷����� ����� �����ϱ� �Ұ����� ���. )
	DWORD			m_dwFlags;				//	�Ӽ�.

	std::string		m_strTITLE;				//	����Ʈ Ÿ��Ʋ.
	std::string		m_strCOMMENT;			//	����Ʈ ����.

	GLQUEST_START	m_sSTARTOPT;			//	�ڵ� ���� ����.

	//	���� �ο� ����.
	DWORD			m_dwBeginMoney;			//	����Ʈ �ޱ� ���� ���.
	DWORD			m_dwBeginPartyMemNum;	//	����Ʈ�� �ޱ� ���� �ּ� ��Ƽ �ο�.

	//	���� ����.
	DWORD			m_dwLimitTime;			//	���� �ð�.	( ���� �ð��� �д���. )
	DWORD			m_dwLimitPartyMemNum;	//	���� ��Ƽ �ο�.
	bool			m_bNonDie;				//	���� ���� ����ϸ� �ȵ�.

	VECQSTEP		m_vecProgStep;			//	��ü ���� ���� ����.

	//	�Ϸ�� ����.
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

	//	����Ʈ ���� �߿� ����Ǵ� �̺�Ʈ
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
	bool IsREPEAT ()		{ return (m_dwFlags&EMQF_REPEAT)!=NULL; }		//	������ ���� ����.
	bool IsAGAIN ()			{ return (m_dwFlags&EMQF_AGAIN)!=NULL; }		//	(�����Ѱ�) �ٽ� ���� ����.
	bool IsPARTY ()			{ return (m_dwFlags&EMQF_PARTY)!=NULL; }		//	��Ƽ�϶� ���� ����.
	bool IsPARTYQUEST ()	{ return (m_dwFlags&EMQF_PARTYQUEST)!=NULL; }	//	��Ƽ�� ���� ���� ����Ʈ.

	bool IsONLYONE ()		{ return (m_dwFlags&EMQF_ONLYONE)!=NULL; }		//	���ÿ� ����Ұ�?
	bool IsGIVEUP ()		{ return (m_dwFlags&EMQF_GIVEUP)!=NULL; }		//	���� ����
	
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

		m_strTITLE("����Ʈ ����"),
		m_strCOMMENT("����Ʈ�� ���� ���� �κ�."),

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
	EMQP_REQREADING		= 0x0001,	// ����Ʈ�� ��� �о�⸦ ��û.
};

struct GLQUESTPROG_STEP
{
	enum { VERSION = 0x0002, };

	//	�̼� ����.
	bool		m_bTALK;					//	��ȭ�ߴ��� ����.
	bool		m_bQITEM;					//	�ʿ� ����Ʈ ������ ���.

	bool		m_bMOBKILL_ALL;				//	�׿����� Mob ��� ����.
	DWORD		m_dwNUM_MOBKILL;			//	���� Mob ����.
	bool		m_bNPCGUARD_DEATH;			//	��ȣ�ؾ��� NPC ��� ����.

	bool		m_bREACH_ZONE;				//	���� ��ġ�� ���� ����.
	
	bool		m_bDEFENSE_ZONE_BEGIN;		//	��� ��ġ ��� ���� ����.
	bool		m_bDEFENSE_ZONE_COMPLETE;	//	��� ��ġ ��� �Ϸ�.
	float		m_fDEFENSE_ZONE_TIMER;		//	��� ��ġ ��� ����ð�.

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
	SNATIVEID	m_sNID;					//	���� ID.
	DWORD		m_dwEDITVER;
	DWORD		m_dwFlags;				//	�Ӽ�.
	DWORD		m_dwCOUNT;				//	�õ� Ƚ��.

	bool		m_bCOMPLETE;			//	�Ϸ� ����.
	float		m_fLAPSTIME;			//	������ ��� �ð�.

	DWORD				m_dwSTEP;		//	���� ���� ����Ʈ STEP.
	GLQUESTPROG_STEP	m_sSTEP_PROG;	//	���� ���� ����.

	GLInventory			m_sINVENTORY;	//	����Ʈ ���� �κ�.

	__time64_t			m_tStartTime;		//	����Ʈ ���� �ð�

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