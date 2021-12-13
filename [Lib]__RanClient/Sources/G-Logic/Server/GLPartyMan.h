#pragma once

#include <deque>
#include "GLParty.h"

struct NET_MSG_GENERIC;


class GLPartyMan
{
public:
	typedef std::deque<DWORD>		PARTYID;
	typedef PARTYID::iterator		PARTYID_ITER;

protected:
	DWORD		m_dwMaxClient;
	GLPARTY*	m_pParty;

	PARTYID		m_FreePartyID;

public:
	HRESULT Create ( DWORD dwMaxClient );

public:
	GLPARTY* NewParty ( DWORD dwGaeaID );
	void DelParty ( DWORD dwPartyID );
	void DelPartyAll ();

public:
	GLPARTY* GetParty ( DWORD dwID )
	{
		GASSERT(m_pParty);
		if ( dwID>=m_dwMaxClient )			return NULL;

		if ( !m_pParty[dwID].ISVAILD() )	return NULL;

		return &m_pParty[dwID];
	}

	BOOL IsPartyMaster ( DWORD dwPartyID, DWORD dwGaeaID );

public:
	void SendMsgToMember ( DWORD dwPartyID, NET_MSG_GENERIC *nmg );
	void SendMsgToMember ( DWORD dwGaeaID, DWORD dwPartyID, NET_MSG_GENERIC *nmg );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	void ConfrontFB ( NET_MSG_GENERIC *nmg );

public:
	GLPartyMan(void);
	~GLPartyMan(void);
};
