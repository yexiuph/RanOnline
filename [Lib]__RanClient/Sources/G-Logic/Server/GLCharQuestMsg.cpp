#include "pch.h"
#include "./GLChar.h"

#include "./GLQuest.h"
#include "./GLQuestMan.h"
#include "./GLGaeaServer.h"

#include "./NpcTalk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT GLChar::MsgReqNpcQuestStart ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_QUEST_START *pNetMsg = (GLMSG::SNETPC_REQ_QUEST_START *) nmg;

	GLMSG::SNETPC_REQ_QUEST_START_FB NetMsgFB;

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	PGLCROW pNPC = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pNPC || !pNPC->m_pCrowData )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;
	}

	//	Note : ����Ʈ ������ �ο� �޴� ��ȭ ����.
	SNpcTalk* pTalk = pNPC->m_pCrowData->m_sNpcTalkDlg.GetTalk ( pNetMsg->dwTalkID );
	if ( !pTalk )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;
	}

	//	Note : �ش� ����Ʈ�� �´��� �˻�.
	DWORD dwQUESTID = pNetMsg->dwQuestID;

	DWORD dwIndex = UINT_MAX;

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		if ( pTalk->m_dwQuestStartID[i] == dwQUESTID )
		{
			dwIndex = i; 
			break;
		}
	}
	
	if ( dwIndex == UINT_MAX )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;
	}

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	�̹� �������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	�̹� �Ϸ��� ����Ʈ���� �˻�.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	if ( pQUEST_END )
	{
		if ( pQUEST_END->m_bCOMPLETE )
		{
			//	������ �õ� �������� �˻�.
			if ( !pQUEST->IsREPEAT() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
				return S_FALSE;
			}
		}
		else
		{
			//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
			if ( !pQUEST->IsAGAIN() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONAGAIN;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
				return S_FALSE;
			}
		}
	}

	//	Note : ��ȭ ���ǿ� �����ϴ��� ����.
	if ( !pTalk->DoTEST( this, dwIndex ) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_CONDITION;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if ( m_lnMoney < pQUEST->m_dwBeginMoney )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_NEEDMONEY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	if( !CheckQuestProgressEvent( 0, pQUEST ) )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	Note : ������ �ִ� ���� �׼��� ����.
	CheckMoneyUpdate( m_lnMoney, pQUEST->m_dwBeginMoney, FALSE, "Quest Start Pay." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= pQUEST->m_dwBeginMoney;

	if ( pQUEST->m_dwBeginMoney )
	{
		//	Note : �ݾ� ��ȭ �α�.
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), EMITEM_ROUTE_DELETE );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

	//	Note : Ŭ���̾�Ʈ�� �� �׼� ��ȭ�� �˷���.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);	

	//	Note : ������ ���۵Ǵ� ����Ʈ ����.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	����Ʈ�� ���.
	m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

	//	Note : �߰��� ����Ʈ ���� ����.
	//
	CByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
	
	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );
	
	GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
	NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

	//	���� ������ ������ ���� ������ ����.
	if ( pQUEST_END )
	{
		//	Note : ����� ����Ʈ ����Ʈ���� ����Ʈ ���� ����.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : ������ ����Ʈ ���� ����.
		//
		GLMSG::SNET_QUEST_END_DEL	NetMsgDel;
		NetMsgDel.dwQUESTID = dwQUESTID;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgDel);
	}

	return S_OK;
}

HRESULT GLChar::MsgReqNpcQuestTalk ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QUEST_STEP_NPC_TALK *pNetMsg = (GLMSG::SNET_QUEST_STEP_NPC_TALK *) nmg;

	//	Note : ����Ʈ ���� �˻�.
	//
	PGLCROW pNPC = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pNPC || !pNPC->m_pCrowData )
		return S_OK;

	//	Note : ����Ʈ ��ȭ ����.
	SNpcTalk* pTalk = pNPC->m_pCrowData->m_sNpcTalkDlg.GetTalk ( pNetMsg->dwTalkID );
	if ( !pTalk )
		return S_OK;

	DWORD dwQUEST_STEP = pTalk->m_dwACTION_PARAM2;

	//	Note : �ش� ����Ʈ�� �´��� �˻�.
	DWORD dwQUESTID = pNetMsg->dwQUESTID;
	if ( pTalk->m_dwACTION_PARAM1 != dwQUESTID )
		return S_OK;

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
		return S_FALSE;

	GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( dwQUEST_STEP );
	if ( !pSTEP )
		return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
		return S_FALSE;

	if ( dwQUEST_STEP!=pQUEST_PROG->m_dwSTEP )
		return S_FALSE;

	if ( pQUEST_PROG->m_sSTEP_PROG.m_bTALK )
	{
		return S_FALSE;
	}

	if ( pNPC->m_sNativeID.dwID!=pSTEP->m_dwNID_NPCTALK )
	{
		return S_FALSE;
	}

	//	Note : ��ȭ ���� �Ϸ�.
	pQUEST_PROG->m_sSTEP_PROG.m_bTALK = true;

	//	Note : ��ȭ ���� �Ϸ������ �˸�.
	//
	GLMSG::SNET_QUEST_STEP_NPC_TALK_FB NetMsgFB;
	NetMsgFB.dwQID = dwQUESTID;
	NetMsgFB.dwQSTEP = dwQUEST_STEP;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
	

	//	Note : ����Ʈ ����. ( ���� pQUEST_PROG ��ȿȭ ���� ���ɼ� ����. )
	//
	DoQuestProgress ( pQUEST_PROG );
	pQUEST_PROG = NULL;

	return S_OK;
}

HRESULT GLChar::MsgReqQuestGiveUp ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QUEST_PROG_GIVEUP *pNetMsg = (GLMSG::SNET_QUEST_PROG_GIVEUP *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQUESTID;

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )					return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )				return S_FALSE;

	if ( !pQUEST->IsGIVEUP() )		return S_FALSE;

	//	����.
	pQUEST_PROG->DoGIVEUP();

	//	�Ϸ�� ����Ʈ�� �߰�.
	m_cQuestPlay.InsertEnd ( *pQUEST_PROG );

	CByteStream cByteStream;
	pQUEST_PROG->SET_BYBUFFER ( cByteStream );

	LPBYTE pBuff(NULL);
	DWORD dwSize(0);
	cByteStream.GetBuffer ( pBuff, dwSize );

	//	�������� ����.
	m_cQuestPlay.DeleteProc ( dwQUESTID );

	//	Note : ����Ʈ �Ϸ� ���� ����.
	//
	GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
	NetMsgStream.SETSTREAM ( pBuff, dwSize );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

	//	Note : ����Ʈ ���� ���� ���� ����.
	//
	GLMSG::SNET_QUEST_PROG_DEL NetMsgDel;
	NetMsgDel.dwQUESTID = dwQUESTID;
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgDel );

	return S_OK;
}

HRESULT GLChar::MsgReqQuestReadReset ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_QUEST_PROG_READ *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_READ *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQID;

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )							return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )						return S_FALSE;

	if ( !pQUEST_PROG->IsReqREADING() )		return S_FALSE;

	pQUEST_PROG->ResetReqREADING ();

	return S_OK;
}

HRESULT GLChar::MsgReqQuestComplete ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_QUEST_PROG_READ *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_READ *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQID;

	GLMSG::SNETPC_QUEST_COMPLETE_FB NetMsgFB;
	NetMsgFB.dwQID = dwQUESTID;

	//	Note : ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

		//	Note : ����Ʈ�� �Ϸ�̴��� �˻�.
	//
	if ( !pQUEST_PROG->CheckCOMPLETE() )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	if ( !CheckQuestItemReward(pQUEST) )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_NOINVEN;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	bool bOK = DoQuestProgress ( pQUEST_PROG );
	if ( bOK )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_OK;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
	}

	return S_OK;
}

