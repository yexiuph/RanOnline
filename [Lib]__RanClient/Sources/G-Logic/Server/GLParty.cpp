#include "pch.h"
#include "glparty.h"

#include "GLAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPARTY::GLPARTY ()
{
	m_cMEMBER.reserve( MAXPARTY );
	m_conftSTART.reserve( MAXPARTY );
	m_conftCURRENT.reserve( MAXPARTY );
	RESET ();
}

void GLPARTY::RESET ()
{
	m_dwPARTYID = PARTY_NULL;
	m_dwMASTER = GAEAID_NULL;
	
	m_cMEMBER.clear();
	m_cREMEMBER.clear();

	m_fconftTIMER = 0.0f;
	m_conftSTART.clear();
	m_conftCURRENT.clear();

	m_dwconftPARTYID = PARTY_NULL;
}

BOOL GLPARTY::ISVAILD ()
{
	return ( m_dwMASTER!=GAEAID_NULL );
}

void GLPARTY::SETMASTER ( DWORD dwGaeaID, DWORD dwPartyID )
{
	m_dwMASTER = dwGaeaID;
	m_dwPARTYID = dwPartyID;

	m_cMEMBER.push_back ( m_dwMASTER );
}

void GLPARTY::CHANGEMASTER ( DWORD dwGaeaID )
{
	m_dwMASTER = dwGaeaID;
}

BOOL GLPARTY::ISMASTER ( DWORD dwGaeaID )
{
	return ( m_dwMASTER == dwGaeaID );
}

BOOL GLPARTY::ISFULL ()
{
	return ( m_cMEMBER.size() == MAXPARTY );
}

BOOL GLPARTY::ISMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = find ( m_cMEMBER.begin(), m_cMEMBER.end(), dwGaeaID );
	if ( iter != m_cMEMBER.end() ) return TRUE;

	return FALSE;
}

DWORD GLPARTY::GETNUMBER ()
{
	return (DWORD) m_cMEMBER.size();
}

BOOL GLPARTY::ADDMEMBER ( DWORD dwGaeaID )
{
	if ( ISFULL () )			return FALSE;
	if ( ISMEMBER(dwGaeaID) )	return FALSE;

	m_cMEMBER.push_back ( dwGaeaID );

	return TRUE;
}

BOOL GLPARTY::DELMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = find ( m_cMEMBER.begin(), m_cMEMBER.end(), dwGaeaID );
	if ( iter == m_cMEMBER.end() ) return FALSE;

	m_cMEMBER.erase(iter);

	//	Note : 대련 참가자에서 삭제.
	//
	DELCONFT_MEMBER ( dwGaeaID );

	return TRUE;
}

BOOL GLPARTY::ISPREMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = find ( m_cREMEMBER.begin(), m_cREMEMBER.end(), dwGaeaID );
	if ( iter!=m_cREMEMBER.end() )	return TRUE;

	return FALSE;
}

BOOL GLPARTY::PREADDMEMBER ( DWORD dwGaeaID )
{
	m_cREMEMBER.push_back ( dwGaeaID );

	return TRUE;
}

BOOL GLPARTY::PREDELMEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = find ( m_cREMEMBER.begin(), m_cREMEMBER.end(), dwGaeaID );
	if ( iter==m_cREMEMBER.end() )		return FALSE;

	m_cREMEMBER.erase ( iter );

	return TRUE;
}

void GLPARTY::ADDCONFT_MEMBER ( DWORD dwGaeaID )
{
	if ( !ISMEMBER(dwGaeaID) )			return;

	m_conftCURRENT.push_back (dwGaeaID);

	return;
}

void GLPARTY::DELCONFT_MEMBER ( DWORD dwGaeaID )
{
	MEMBER_ITER iter = find ( m_conftCURRENT.begin(), m_conftCURRENT.end(), dwGaeaID );
	if ( iter!=m_conftCURRENT.end() )	m_conftCURRENT.erase(iter);

	return;
}

void GLPARTY::SETCONFT_MEMBER ()
{
	m_conftSTART = m_conftCURRENT;
}

bool GLPARTY::ISCONFTING ()
{
	return m_dwconftPARTYID!=PARTY_NULL;
}

bool GLPARTY::ISCONFT_LOSS ()
{
	return m_conftCURRENT.empty()==true;
}

DWORD GLPARTY::GETCONFT_NUM ()
{
	return (DWORD) m_conftCURRENT.size();
}

void GLPARTY::RESET_CONFT ()
{
	m_fconftTIMER = 0.0f;

	m_conftSTART.clear();
	m_conftCURRENT.clear();

	m_conftOPTION.RESET();
	m_dwconftPARTYID = PARTY_NULL;


	GLPARTY::MEMBER_ITER iter = m_cMEMBER.begin();
	GLPARTY::MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const DWORD dwGaeaID = (*iter);
		PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
		if ( pChar )	pChar->m_sCONFTING.RESET();
	}
}

WORD GLPARTY::GetMASTER_SCHOOL ()
{
	PGLCHARAG pMaster = GLAgentServer::GetInstance().GetChar ( m_dwMASTER );
	if ( !pMaster )		return 0;

	return pMaster->m_wSchool;
}

SNATIVEID GLPARTY::GetMASTER_MAP ()
{
	PGLCHARAG pMaster = GLAgentServer::GetInstance().GetChar ( m_dwMASTER );
	if ( !pMaster )		return SNATIVEID(false);

	return pMaster->m_sCurMapID;
}

bool GLPARTY::IsSAME_SCHOOL ()
{
	PGLCHARAG pMaster = GLAgentServer::GetInstance().GetChar ( m_dwMASTER );
	if ( !pMaster )		return false;

	GLPARTY::MEMBER_ITER iter = m_cMEMBER.begin();
	GLPARTY::MEMBER_ITER iter_end = m_cMEMBER.end();
	for ( ; iter!=iter_end; ++iter )
	{
		const DWORD dwGaeaID = (*iter);
		PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
		if ( pChar )
		{
			if ( pChar->m_wSchool != pMaster->m_wSchool )	return false;
		}
	}

	return true;
}
