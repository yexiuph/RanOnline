#include "pch.h"
#include "GLCharAG.h"
#include "GLAgentServer.h"
#include "GLContrlMsg.h"
#include "dbactionlogic.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#include "glpartyman.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GLPartyMan::GLPartyMan(void) :
	m_dwMaxClient(0),
	m_pParty(NULL)
{
}

GLPartyMan::~GLPartyMan(void)
{
	SAFE_DELETE_ARRAY(m_pParty);
}

HRESULT GLPartyMan::Create ( DWORD dwMaxClient )
{
	m_dwMaxClient = dwMaxClient;

	SAFE_DELETE_ARRAY(m_pParty);
	m_pParty = new GLPARTY[m_dwMaxClient];

	m_FreePartyID.clear();
	for ( DWORD i=0; i<m_dwMaxClient; ++i )		m_FreePartyID.push_back ( i );

	return S_OK;
}

BOOL GLPartyMan::IsPartyMaster ( DWORD dwPartyID, DWORD dwGaeaID )
{
	GASSERT(dwPartyID<m_dwMaxClient);
	GASSERT(dwGaeaID<m_dwMaxClient);

	return m_pParty[dwPartyID].ISMASTER ( dwGaeaID );
}

//	Note : GaeaID는 유효하다고 가정함.
//
GLPARTY* GLPartyMan::NewParty ( DWORD dwGaeaID )
{
	PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
	if ( !pChar )					return NULL;

	if ( m_FreePartyID.empty() )	return NULL;

	DWORD dwPartyID = *m_FreePartyID.begin();
	m_FreePartyID.erase(m_FreePartyID.begin());

	m_pParty[dwPartyID].RESET();

	pChar->SetPartyID ( dwPartyID );
	m_pParty[dwPartyID].m_dwPARTYID = dwPartyID;
	m_pParty[dwPartyID].SETMASTER ( dwGaeaID, dwPartyID );

	return &(m_pParty[dwPartyID]);
}

void GLPartyMan::DelParty ( DWORD dwPartyID )
{
	GASSERT(dwPartyID<m_dwMaxClient);

	GLPARTY* pParty = GetParty(dwPartyID);
	if ( !pParty )				return;
	if ( !pParty->ISVAILD() )	return;

	GLMSG::SNET_PARTY_DISSOLVE NetMsg;
	NetMsg.dwPartyID = dwPartyID;

	m_FreePartyID.push_back(dwPartyID);

	GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	PGLCHARAG pChar = NULL;
	for ( ; iter!=iter_end; ++iter )
	{
		pChar = GLAgentServer::GetInstance().GetChar ( (*iter) );
		if ( pChar )
		{
			pChar->ReSetPartyID();

			//	Note : 파티 구성원에게 파티 해체를 알림.
			//
			GLAgentServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, &NetMsg );
		}
	}

	//	Note : 필드 서버에 파티 해체를 알림.
	//
	GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsg );

	//	Note : 파티 정보를 리셋.
	//
	m_pParty[dwPartyID].RESET();
}

void GLPartyMan::DelPartyAll ()
{
	for ( DWORD i=0 ; i < m_dwMaxClient; ++i )
	{
		DelParty ( i );
	}
}

void GLPartyMan::SendMsgToMember ( DWORD dwPartyID, NET_MSG_GENERIC *nmg )
{
	GASSERT(dwPartyID<m_dwMaxClient);

	GLPARTY* pParty = &m_pParty[dwPartyID];

	GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	PGLCHARAG pChar = NULL;
	for ( ; iter!=iter_end; ++iter )
	{
		pChar = GLAgentServer::GetInstance().GetChar ( (*iter) );
		if ( !pChar )
		{
			continue;
		}
		else
		{
			//	Note : 파티 구성원에게 메시지 전송.			
			GLAgentServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, nmg );
			/*NET_CHAT_FB* pNc = (NET_CHAT_FB*) nmg;
			GLAgentServer::GetInstance().SendToGMMsg( pChar, *pNc );*/
		}
	}
}

void GLPartyMan::SendMsgToMember ( DWORD dwGaeaID, DWORD dwPartyID, NET_MSG_GENERIC *nmg )
{
	GASSERT(dwPartyID<m_dwMaxClient);

	GLPARTY* pParty = &m_pParty[dwPartyID];

	GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
	GLPARTY::MEMBER_ITER iter_end = pParty->m_cMEMBER.end();

	PGLCHARAG pChar = NULL;
	for ( ; iter!=iter_end; ++iter )
	{
		pChar = GLAgentServer::GetInstance().GetChar ( (*iter) );
		if ( !pChar )							continue;
		if ( pChar->m_dwGaeaID == dwGaeaID )	continue;

		//	Note : 파티 구성원에게 메시지 전송.
		//
		GLAgentServer::GetInstance().SENDTOCLIENT ( pChar->m_dwClientID, nmg );
		NET_CHAT_FB* pNc = (NET_CHAT_FB*) nmg;
		GLAgentServer::GetInstance().SendToGMMsg( pChar, *pNc );
	}
}

HRESULT GLPartyMan::FrameMove ( float fTime, float fElapsedTime )
{
	for ( DWORD dwPartyID=0; dwPartyID<m_dwMaxClient; ++dwPartyID )
	{
		GLPARTY* pParty = GetParty (dwPartyID);
		if ( !pParty )		continue;

		DWORD dwOldMaster = pParty->m_dwMASTER;
		
		PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( pParty->m_dwMASTER );
		if ( !pChar )
		{
			if ( pParty->ISCONFTING() )
			{
				EMCONFRONT_END emEND_A(EMCONFRONT_END_FAIL);
				EMCONFRONT_END emEND_B(EMCONFRONT_END_FAIL);

				pParty->RESET_CONFT();
				
				GLPARTY *pConftParty = GetParty(pParty->m_dwconftPARTYID);
				if ( pConftParty )	pConftParty->RESET_CONFT();

				GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld;
				NetMsgFld.dwPARTY_A = pParty->m_dwPARTYID;
				NetMsgFld.dwPARTY_B = pParty->m_dwconftPARTYID;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				GLAgentServer::GetInstance().SENDTOALLCHANNEL(&NetMsgFld);
			}

			// 파티장이 사라져도 파티원이 2명 이상이면 그 다음 사람에게 
			// 파티장의 권한을 위임한다. (파티정보를 ReNew 한다)
			pParty->DELMEMBER ( dwOldMaster );
			GLPARTY::MEMBER_ITER iter_NewMaster = pParty->m_cMEMBER.begin();
			DWORD dwNewMaster = (*iter_NewMaster);
			if ( pParty->GETNUMBER () >= 2 )
			{
				pParty->CHANGEMASTER ( dwNewMaster );

				GLMSG::SNET_PARTY_MASTER_RENEW NetMsgRenew;
				NetMsgRenew.dwPartyID   = pParty->m_dwPARTYID;
				NetMsgRenew.dwDelMaster = dwOldMaster;
				NetMsgRenew.dwNewMaster = dwNewMaster;

				GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgRenew );
			}
			else
			{
				DelParty ( dwPartyID );
				continue;
			}
		}

		GLPARTY::MEMBER_ITER iter = pParty->m_cMEMBER.begin();
		
		for ( ; iter!=pParty->m_cMEMBER.end(); )
		{
			DWORD dwGaeaID = (*iter);

			PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
			if ( pChar )
			{
				iter++;
				continue;
			}

			GLMSG::SNET_PARTY_DEL NetMsgDel;
			NetMsgDel.dwPartyID = dwPartyID;
			NetMsgDel.dwDelMember = dwGaeaID;

			//	Note : 파티원이 사라짐을 파티원들에게 알림.
			//
			SendMsgToMember ( dwPartyID, (NET_MSG_GENERIC*) &NetMsgDel );

			//	Note : 파티원이 사라짐을 필드에 알림.
			//
			GLAgentServer::GetInstance().SENDTOALLCHANNEL ( &NetMsgDel );

			//	Note : 없어진 파티원 제거.
			//
			iter = pParty->m_cMEMBER.erase ( iter );

			//	Note : '파티대련'중일때는 사라진 파티원을 대련 목록에서 제거.
			//
			if ( pParty->ISCONFTING() )
			{
				pParty->DELCONFT_MEMBER(dwGaeaID);
			}
		}

		//	Note : 파티 장만 남은 파티의 경우 자동 소멸.
		//
		if ( pParty->GETNUMBER() == 1 )
		{
			if ( pParty->ISCONFTING() )
			{
				GLPARTY *pConftParty = GetParty(pParty->m_dwconftPARTYID);

				EMCONFRONT_END emEND_A(EMCONFRONT_END_FAIL);
				EMCONFRONT_END emEND_B(EMCONFRONT_END_FAIL);

				pParty->RESET_CONFT();
				
				if ( pConftParty )	pConftParty->RESET_CONFT();

				GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld;
				NetMsgFld.dwPARTY_A = pParty->m_dwPARTYID;
				NetMsgFld.dwPARTY_B = pParty->m_dwconftPARTYID;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				GLAgentServer::GetInstance().SENDTOALLCHANNEL(&NetMsgFld);
			}

			DelParty ( dwPartyID );
		}

		//	Note : 대련중 일때 대련이 종료되는지 검사.
		//
		if ( pParty->ISCONFTING() )
		{
			pParty->m_fconftTIMER += fElapsedTime;

			//	Note : 대련에 패하였을 경우.
			//
			bool bCONFT_END(false);
			bool bCONFT_SCHOOL(pParty->m_conftOPTION.bSCHOOL);
			EMCONFRONT_END emEND_A(EMCONFRONT_END_PLOSS);
			EMCONFRONT_END emEND_B(EMCONFRONT_END_PWIN);

			WORD wSCHOOL_A = pParty->m_conftOPTION.wMY_SCHOOL;
			WORD wSCHOOL_B = pParty->m_conftOPTION.wTAR_SCHOOL;
			WORD wSCHOOL_WIN(wSCHOOL_B);

			if ( pParty->ISCONFT_LOSS() )
			{
				GLPARTY *pConftParty = GetParty(pParty->m_dwconftPARTYID);

				if ( !pConftParty )
				{
					emEND_A = EMCONFRONT_END_FAIL;
					emEND_B = EMCONFRONT_END_FAIL;
				}
				else if ( pConftParty->ISCONFT_LOSS() )
				{
					emEND_A = EMCONFRONT_END_PLOSS;
					emEND_B = EMCONFRONT_END_PLOSS;
				}

				//	Note : 파티대련이 종료됨을 알림 to (필드)
				//
				GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld;
				NetMsgFld.dwPARTY_A = pParty->m_dwPARTYID;
				NetMsgFld.dwPARTY_B = pParty->m_dwconftPARTYID;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				GLAgentServer::GetInstance().SENDTOALLCHANNEL(&NetMsgFld);

				bCONFT_END = true;
				pParty->RESET_CONFT();
				if ( pConftParty )		pConftParty->RESET_CONFT();
			}
			else if ( pParty->m_fconftTIMER > GLCONST_CHAR::fCONFRONT_TIME )
			{
				GLPARTY *pConftParty = GetParty(pParty->m_dwconftPARTYID);

				emEND_A = (EMCONFRONT_END_PTIME);
				emEND_B = (EMCONFRONT_END_PTIME);

				//	Note : 파티대련이 종료됨을 알림 to (필드)
				//
				GLMSG::SNETPC_CONFRONTPTY_END2_FLD NetMsgFld;
				NetMsgFld.dwPARTY_A = pParty->m_dwPARTYID;
				NetMsgFld.dwPARTY_B = pParty->m_dwconftPARTYID;
				NetMsgFld.emEND_A = emEND_A;
				NetMsgFld.emEND_B = emEND_B;
				GLAgentServer::GetInstance().SENDTOALLCHANNEL(&NetMsgFld);

				bCONFT_END = true;
				pParty->RESET_CONFT();
				if ( pConftParty )		pConftParty->RESET_CONFT();
			}

			//	Note : 학교간 파티 대련의 결과.
			//
			if ( bCONFT_END && bCONFT_SCHOOL )
			{
				SNATIVEID sMapID = pParty->GetMASTER_MAP();
				const char *szMAP_NAME = GLAgentServer::GetInstance().GetMapName ( sMapID );

				CString strTEXT;
				if ( emEND_B==EMCONFRONT_END_PWIN )
				{
					strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_END_PWIN"),
						szMAP_NAME,
						GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_A), GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_B),
						GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_WIN) );

					//	Note : db 에 학교간 파티 대련 결과 전송.
					//
					int nServerGroup = GLAgentServer::GetInstance().GetMsgServer()->GetServerGroup();
					int nServerNum = GLAgentServer::GetInstance().GetMsgServer()->GetServerNum();
					CLogPartyMatch *pDbAction = new CLogPartyMatch(nServerGroup,nServerNum,wSCHOOL_B,wSCHOOL_A);
					GLAgentServer::GetInstance().GetDBMan()->AddJob ( pDbAction );

					//	Note : 학교간 파티 대련이 종료되었는데, 승패가 가려질 경우 전체 경험치 발생.
					//
					//GLMSG::SNET_CONFT_SPTY_EXP_FLD NetMsgFld;
					//NetMsgFld.wSCHOOL = wSCHOOL_WIN;
					//NetMsgFld.fEXP_RATE = GLCONST_CHAR::fCONFT_SPTY_EXP;
					//GLAgentServer::Instance.SENDTOALLFIELD(&NetMsgFld);
				}
				else
				{
					strTEXT.Format ( ID2SERVERTEXT("EMCONFRONT_END_PDRAWN"),
						szMAP_NAME, GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_A), GLCONST_CHAR::GETSCHOOLNAME(wSCHOOL_B) );
				}

				GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
				NetMsg.SETTEXT ( strTEXT.GetString() );
				
				GLAgentServer::GetInstance().SENDTOALLCLIENT ( &NetMsg );
			}
		}
	}

	return S_OK;
}

void GLPartyMan::ConfrontFB ( NET_MSG_GENERIC *nmg )
{
	GLMSG::SNETPC_REQ_CONFRONT_FB *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_FB *) nmg;

	GLPARTY *pPartyA = GetParty ( pNetMsg->dwID );
	if ( !pPartyA )									return;
	if ( pPartyA->m_dwconftPARTYID==PARTY_NULL )	return;

	GLMSG::SNETPC_REQ_CONFRONT_FB NetMsgFB;
	NetMsgFB.emFB = pNetMsg->emFB;

	SendMsgToMember ( pPartyA->m_dwPARTYID, (NET_MSG_GENERIC *) &NetMsgFB );
	pPartyA->RESET_CONFT();

	GLPARTY *pPartyB = GetParty ( pPartyA->m_dwconftPARTYID );
	if ( !pPartyB )									return;

	SendMsgToMember ( pPartyB->m_dwPARTYID, (NET_MSG_GENERIC *) &NetMsgFB );
	pPartyB->RESET_CONFT();
}

