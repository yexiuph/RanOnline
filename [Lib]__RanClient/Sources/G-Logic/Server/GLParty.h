#pragma once

#include <set>
#include <map>
#include <vector>

#include "GLDefine.h"
#include "DxLandDef.h"
#include "GLCharDefine.h"

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "strsafe.h" // Safe string function

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH	33
#endif

enum
{
	MAXPARTY	= 8,
};

struct GLPARTY_FNET
{
	DWORD			m_dwGaeaID;
	char			m_szName[CHAR_SZNAME];
	EMCHARCLASS		m_emClass;

	bool			m_bConfront;
	SNATIVEID		m_sMapID;

	GLPARTY_FNET () 
		: m_dwGaeaID(GAEAID_NULL)
		, m_emClass(GLCC_FIGHTER_M)
		
		, m_bConfront(true)
		, m_sMapID(MAXLANDMID,MAXLANDSID)
	{
        memset(m_szName, 0, sizeof(char) * (CHAR_SZNAME));
	}

	GLPARTY_FNET ( DWORD dwGaeaID, char* szName, EMCHARCLASS emClass, SNATIVEID sMapID )
	{
		m_dwGaeaID = dwGaeaID;
		::StringCchCopy( m_szName, CHAR_SZNAME, szName );
		m_emClass = emClass;
		m_sMapID = sMapID;
	}
};

struct GLPARTY
{
	typedef std::vector<DWORD>			MEMBER;
	typedef MEMBER::iterator			MEMBER_ITER;

	DWORD		m_dwPARTYID;		//	파티ID.

	DWORD		m_dwMASTER;			//	파티장.
	MEMBER		m_cMEMBER;			//	파티에 참가한 현재 인원.
	SPARTY_OPT	m_sOPTION;			//	옵션.

	MEMBER		m_cREMEMBER;		//	예비 맴버.

	float		m_fconftTIMER;		//	대련 타이머.
	MEMBER		m_conftSTART;		//	대련 초기 참가자.
	MEMBER		m_conftCURRENT;		//	대련 현제 참가자. ( 초기 인원에서 '멥이동/패배'등으로 감소함 )

	SCONFT_OPTION	m_conftOPTION;	//	대련 옵션.
	DWORD		m_dwconftPARTYID;	//	'파티대련' 의 상대 파티.

	GLPARTY ();
	
	void RESET ();

	void SETMASTER ( DWORD dwGaeaID, DWORD dwPartyID );
	void CHANGEMASTER ( DWORD dwGaeaID );

	BOOL ISVAILD ();
	BOOL ISMASTER ( DWORD dwGaeaID );
	BOOL ISFULL ();

	DWORD GETNUMBER ();

	BOOL ISMEMBER ( DWORD dwGaeaID );
	BOOL ADDMEMBER ( DWORD dwGaeaID );
	BOOL DELMEMBER ( DWORD dwGaeaID );

	BOOL ISPREMEMBER ( DWORD dwGaeaID );

	BOOL PREADDMEMBER ( DWORD dwGaeaID );
	BOOL PREDELMEMBER ( DWORD dwGaeaID );

	bool ISCONFTING();
	void ADDCONFT_MEMBER ( DWORD dwGaeaID );
	void DELCONFT_MEMBER ( DWORD dwGaeaID );
	DWORD GETCONFT_NUM ();

	void SETCONFT_MEMBER (); // 현제 m_conftCURRENT의 값을 m_conftSTART 에 대입.

	bool ISCONFT_LOSS ();

	void RESET_CONFT ();
	bool IsSAME_SCHOOL ();
	WORD GetMASTER_SCHOOL ();
	SNATIVEID GetMASTER_MAP ();
};

