#pragma once

#include <set>
#include "glpartyfield.h"

struct NET_MSG_GENERIC;

class GLPartyFieldMan
{
protected:
	DWORD			m_dwMaxClient;

	GLPARTY_FIELD*	m_pParty;
	DWORD*			m_pGaeaTOParty;

public:
	HRESULT Create ( DWORD dwMaxClient );

public:
	GLPARTY_FIELD* NewParty ( DWORD dwMaster, DWORD dwPartyID );
	void DelParty ( DWORD dwPartyID );
	void DelPartyAll ();

public:
	GLPARTY_FIELD* GetParty ( DWORD dwID )
	{
		GASSERT(m_pParty);
		if ( dwID>=m_dwMaxClient )			return NULL;

		if ( !m_pParty[dwID].ISVAILD() )	return NULL;

		return &m_pParty[dwID];
	}

	DWORD GetPartyID ( DWORD dwGaeaID );
	void SetPartyID ( DWORD dwGaeaID, DWORD dwPartyID );
	void ReSetPartyID ( DWORD dwGaeaID );

	BOOL IsPartyMaster ( DWORD dwPartyID, DWORD dwGaeaID );

protected:
	void SetPartyMember ( DWORD dwPartyID, DWORD dwGaeaID );
	void ReSetPartyMember ( DWORD dwPartyID, DWORD dwGaeaID );

public:
	void SendMsgToMember ( DWORD dwPartyID, NET_MSG_GENERIC *nmg );
	void SendMsgToMember ( DWORD dwGaeaID, DWORD dwPartyID, NET_MSG_GENERIC *nmg );

protected:
	BOOL MsgPartyFNew ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyAdd ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyDel ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyDissolve ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyAuthority( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMbrMoveMap ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMbrRename ( NET_MSG_GENERIC* nmg );
	BOOL MsgPartyMasterRenew ( NET_MSG_GENERIC* nmg );

public:
	HRESULT MsgProcess ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

public:
	GLPartyFieldMan(void);
	~GLPartyFieldMan(void);
};

