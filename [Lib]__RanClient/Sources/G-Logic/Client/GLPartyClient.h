#pragma once

#include "GLParty.h"

struct NET_MSG_GENERIC;

struct GLPARTY_CLIENT
{
	DWORD			m_dwGaeaID;
	char			m_szName[CHAR_SZNAME];
	EMCHARCLASS		m_emClass;

	SNATIVEID		m_sMapID;

	D3DXVECTOR3		m_vPos;
	GLPADATA		m_sHP;
	GLPADATA		m_sMP;

	bool			m_bConfront;

	GLPARTY_CLIENT () 
		: m_dwGaeaID(GAEAID_NULL)
		, m_emClass(GLCC_FIGHTER_M)
		
		, m_sMapID(MAXLANDMID,MAXLANDSID)
		
		, m_vPos(0,0,0)
		, m_bConfront(false)
	{
		memset(m_szName, 0, sizeof(char) * (CHAR_SZNAME));
	}

	void RESET ()
	{
		m_dwGaeaID = GAEAID_NULL;
		m_emClass = GLCC_FIGHTER_M;

		m_sMapID = SNATIVEID(MAXLANDMID,MAXLANDSID);

		m_vPos = D3DXVECTOR3(0,0,0);
		m_sHP = GLPADATA(0,0);
		m_sMP = GLPADATA(0,0);

		memset(m_szName, 0, sizeof(char) * (CHAR_SZNAME));
	}

	BOOL VALID ()
	{
		if ( m_dwGaeaID!=GAEAID_NULL )							return TRUE;
		return FALSE;
	}

	BOOL VALIDMAP ()
	{
		if ( m_sMapID != SNATIVEID(MAXLANDMID,MAXLANDSID) )		return TRUE;
		return FALSE;
	}

	void ASSIGN ( GLPARTY_FNET &sPartyNet )
	{
		m_dwGaeaID = sPartyNet.m_dwGaeaID;
		StringCchCopy ( m_szName, CHAR_SZNAME, sPartyNet.m_szName );
		m_emClass = sPartyNet.m_emClass;

		m_sMapID = sPartyNet.m_sMapID;
	}

	BOOL ISONESELF ();
};

class GLPartyClient
{
protected:
	typedef std::pair<DWORD,GLPARTY_CLIENT>				PARTYPAIRCLIENT;
	typedef std::vector<PARTYPAIRCLIENT>				PARTYMAP;
	typedef PARTYMAP::iterator							PARTYMAP_ITER;

protected:
	DWORD			m_dwPartyID;
	GLPARTY_CLIENT	m_sMaster;
	PARTYMAP		m_mapParty;
	SPARTY_OPT		m_sOption;

public:
	DWORD GetPartyID ()				{ return m_dwPartyID; }
	DWORD GetMemberNum ();
	GLPARTY_CLIENT* GetMaster ();
	GLPARTY_CLIENT* GetMember ( DWORD dwIndex );

	GLPARTY_CLIENT* FindMember ( DWORD dwGaeaID );
	void DELETEMEMBER ( DWORD dwGaeaID );

	SPARTY_OPT&		GetOption ()	{ return m_sOption; }

	bool IsMember( DWORD dwGaeaID );
	bool IsMember( const char* szCharName );

public:
	void ResetParty ();

public:
	void Lure ( DWORD dwGaeaID );
	void Secede ( DWORD dwIndex );
	void Dissolve ();
	void Authority( DWORD dwIndex );

protected:
	BOOL MsgPartyFNew ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyLureFb ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyAdd ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyDel ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyDissolve ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyAuthority( NET_MSG_GENERIC* nmg );

	BOOL MsgPartyMoveMap ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMbrDetail ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMbrPoint ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMbrPos ( NET_MSG_GENERIC* nmg );

	BOOL MsgPartyMbrItemPickUp ( NET_MSG_GENERIC* nmg );

	BOOL MsgPartyMbrRename ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMasterRenew ( NET_MSG_GENERIC* nmg );

public:
	HRESULT MsgProcess ( NET_MSG_GENERIC* nmg );

public:
	BOOL SetConfront ( NET_MSG_GENERIC* nmg );
	void ReSetConfront ();
	void ResetConfrontMember ( DWORD dwGaeaID );
	DWORD GetConfrontNum ();

public:
	static GLPartyClient& GetInstance();

protected:
	GLPartyClient(void);

public:
	~GLPartyClient(void);
};

