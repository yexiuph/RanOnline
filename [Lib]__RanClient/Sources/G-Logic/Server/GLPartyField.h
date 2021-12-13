#pragma once

#include <set>
#include <map>

#include "GLDefine.h"
#include "DxLandDef.h"
#include "GLCharDefine.h"
#include "glparty.h"
#include "glcontrlmsg.h"

class CItemDrop;
class CMoneyDrop;
class GLChar;
class GLLandMan;

struct GLPARTY_FIELD
{
	typedef std::pair<DWORD,GLPARTY_FNET>			PARTYPAIR;
	typedef std::vector<PARTYPAIR>					MEMBER;
	typedef MEMBER::iterator						MEMBER_ITER;

	typedef std::vector<DWORD>						VECROUND;
	typedef VECROUND::iterator						VECROUND_ITER;

	DWORD		m_dwPARTYID;		//	파티 ID.

	DWORD		m_dwMASTER;			//	파티 마스터 ID.
	MEMBER		m_cMEMBER;			//	멤버 리스트.

	SPARTY_OPT	m_sOPTION;			//	옵션.

	VECROUND	m_arrayROUND_ITEM_CUR;			//	아이템루팅 순서.
	VECROUND	m_arrayROUND_ITEM[MAXPARTY];	//	아이템루팅 랜덤 순서.

	VECROUND	m_arrayROUND_MONEY_CUR;			//	Money 루팅 순서.
	VECROUND	m_arrayROUND_MONEY[MAXPARTY];	//	Money 루팅 랜덤 순서.

	DWORD			m_dwconftPARTYID;
	SCONFT_OPTION	m_sconftOPTION;
	DWORD			m_dwconftMEMLEVEL;

	bool			m_bQBoxEnable;

	GLPARTY_FIELD ();
	
	void RESET ();

	DWORD GETMASTER(){ return m_dwMASTER;}
	void SETMASTER ( DWORD dwMaster, DWORD dwPartyID );

	BOOL ISVAILD ();
	BOOL ISMASTER ( DWORD dwGaeaID );
	BOOL ISFULL ();

	DWORD GETNUMBER ();

	BOOL ISMEMBER ( DWORD dwGaeaID );
	const GLPARTY_FNET* GETMEMBER ( DWORD dwGaeaID );

	BOOL ADDMEMBER (GLPARTY_FNET &sParty );
	BOOL DELMEMBER ( DWORD dwGaeaID );

	BOOL MBR_MOVEMAP ( DWORD dwGaeaID, SNATIVEID &sMapID );
	const SNATIVEID GETMBR_MAP ( DWORD dwGaeaID );

	bool MBR_CONFRONT_JOIN ( DWORD dwGaeaID );
	bool MBR_CONFRONT_LEAVE ( DWORD dwGaeaID );

	void RESET_CONFRONT ( EMCONFRONT_END emEND );

	DWORD GETAVER_CONFT_LEVEL ();
	void RECEIVELIVINGPOINT ( int nLivePoint );

	bool DOITEM_ALLOT ( GLChar* pKnockChar, CItemDrop* pItemDrop, bool bCheckAllotItem=false );
	bool DOMONEY_ALLOT ( GLChar* pKnockChar, CMoneyDrop* pMoneyDrop, GLLandMan* pLandMan, bool bCheckAllotItem=false );
	bool SENDMSG2NEAR ( GLChar* pKnockChar, void* pNetMsg );
};

