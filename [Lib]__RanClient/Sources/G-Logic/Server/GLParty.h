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

	DWORD		m_dwPARTYID;		//	��ƼID.

	DWORD		m_dwMASTER;			//	��Ƽ��.
	MEMBER		m_cMEMBER;			//	��Ƽ�� ������ ���� �ο�.
	SPARTY_OPT	m_sOPTION;			//	�ɼ�.

	MEMBER		m_cREMEMBER;		//	���� �ɹ�.

	float		m_fconftTIMER;		//	��� Ÿ�̸�.
	MEMBER		m_conftSTART;		//	��� �ʱ� ������.
	MEMBER		m_conftCURRENT;		//	��� ���� ������. ( �ʱ� �ο����� '���̵�/�й�'������ ������ )

	SCONFT_OPTION	m_conftOPTION;	//	��� �ɼ�.
	DWORD		m_dwconftPARTYID;	//	'��Ƽ���' �� ��� ��Ƽ.

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

	void SETCONFT_MEMBER (); // ���� m_conftCURRENT�� ���� m_conftSTART �� ����.

	bool ISCONFT_LOSS ();

	void RESET_CONFT ();
	bool IsSAME_SCHOOL ();
	WORD GetMASTER_SCHOOL ();
	SNATIVEID GetMASTER_MAP ();
};

