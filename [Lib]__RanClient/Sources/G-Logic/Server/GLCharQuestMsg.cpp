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

	//	Note : 퀘스트 시작을 위한 검사.
	//
	PGLCROW pNPC = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pNPC || !pNPC->m_pCrowData )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;
	}

	//	Note : 퀘스트 시작을 부여 받는 대화 점검.
	SNpcTalk* pTalk = pNPC->m_pCrowData->m_sNpcTalkDlg.GetTalk ( pNetMsg->dwTalkID );
	if ( !pTalk )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_OK;
	}

	//	Note : 해당 퀘스트가 맞는지 검사.
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

	//	Note : 퀘스트 시작을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	이미 진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_START_FB_ALREADY;
		GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
		return S_FALSE;
	}

	//	이미 완료한 퀘스트인지 검사.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	if ( pQUEST_END )
	{
		if ( pQUEST_END->m_bCOMPLETE )
		{
			//	여러번 시도 가능한지 검사.
			if ( !pQUEST->IsREPEAT() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONREPEAT;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
				return S_FALSE;
			}
		}
		else
		{
			//	포기(실패)한 퀘스트를 다시 시도 불가능한 경우.
			if ( !pQUEST->IsAGAIN() )
			{
				NetMsgFB.emFB = EMQUEST_START_FB_DONAGAIN;
				GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
				return S_FALSE;
			}
		}
	}

	//	Note : 대화 조건에 부합하는지 점검.
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

	//	Note : 가지고 있는 돈의 액수를 조절.
	CheckMoneyUpdate( m_lnMoney, pQUEST->m_dwBeginMoney, FALSE, "Quest Start Pay." );
	m_bMoneyUpdate = TRUE;

	m_lnMoney -= pQUEST->m_dwBeginMoney;

	if ( pQUEST->m_dwBeginMoney )
	{
		//	Note : 금액 변화 로그.
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, -LONGLONG(pQUEST->m_dwBeginMoney), EMITEM_ROUTE_DELETE );
		GLITEMLMT::GetInstance().ReqMoneyExc ( ID_CHAR, m_dwCharID, ID_CHAR, 0, m_lnMoney, EMITEM_ROUTE_CHAR );
	}

	//	Note : 클라이언트에 돈 액수 변화를 알려줌.
	GLMSG::SNETPC_UPDATE_MONEY NetMsg;
	NetMsg.lnMoney = m_lnMoney;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsg);	

	//	Note : 새로이 시작되는 퀘스트 설정.
	//
	GLQUESTPROG sQUEST_PROG_NEW;
	sQUEST_PROG_NEW.START ( pQUEST, pQUEST_END );

	//	리스트에 등록.
	m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

	//	Note : 추가된 퀘스트 정보 전송.
	//
	CByteStream sByteStream;
	sQUEST_PROG_NEW.SET_BYBUFFER ( sByteStream );
	
	LPBYTE pBuff(0);
	DWORD dwSize(0);
	sByteStream.GetBuffer ( pBuff, dwSize );
	
	GLMSG::SNET_QUEST_PROG_STREAM NetMsgQuestNew;
	NetMsgQuestNew.SETSTREAM ( pBuff, dwSize );
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgQuestNew);

	//	만약 종전에 진행한 정보 있을때 삭제.
	if ( pQUEST_END )
	{
		//	Note : 종료된 퀘스트 리스트에서 퀘스트 정보 삭제.
		//
		m_cQuestPlay.DeleteEnd ( dwQUESTID );

		//	Note : 삭제된 퀘스트 정보 전송.
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

	//	Note : 퀘스트 진행 검사.
	//
	PGLCROW pNPC = m_pLandMan->GetCrow ( pNetMsg->dwNpcID );
	if ( !pNPC || !pNPC->m_pCrowData )
		return S_OK;

	//	Note : 퀘스트 대화 점검.
	SNpcTalk* pTalk = pNPC->m_pCrowData->m_sNpcTalkDlg.GetTalk ( pNetMsg->dwTalkID );
	if ( !pTalk )
		return S_OK;

	DWORD dwQUEST_STEP = pTalk->m_dwACTION_PARAM2;

	//	Note : 해당 퀘스트가 맞는지 검사.
	DWORD dwQUESTID = pNetMsg->dwQUESTID;
	if ( pTalk->m_dwACTION_PARAM1 != dwQUESTID )
		return S_OK;

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
		return S_FALSE;

	GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( dwQUEST_STEP );
	if ( !pSTEP )
		return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
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

	//	Note : 대화 조건 완료.
	pQUEST_PROG->m_sSTEP_PROG.m_bTALK = true;

	//	Note : 대화 조건 완료됫음을 알림.
	//
	GLMSG::SNET_QUEST_STEP_NPC_TALK_FB NetMsgFB;
	NetMsgFB.dwQID = dwQUESTID;
	NetMsgFB.dwQSTEP = dwQUEST_STEP;
	GLGaeaServer::GetInstance().SENDTOCLIENT(m_dwClientID,&NetMsgFB);
	

	//	Note : 퀘스트 갱신. ( 주의 pQUEST_PROG 무효화 됬을 가능성 있음. )
	//
	DoQuestProgress ( pQUEST_PROG );
	pQUEST_PROG = NULL;

	return S_OK;
}

HRESULT GLChar::MsgReqQuestGiveUp ( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNET_QUEST_PROG_GIVEUP *pNetMsg = (GLMSG::SNET_QUEST_PROG_GIVEUP *) nmg;

	DWORD dwQUESTID = pNetMsg->dwQUESTID;

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )					return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )				return S_FALSE;

	if ( !pQUEST->IsGIVEUP() )		return S_FALSE;

	//	리셋.
	pQUEST_PROG->DoGIVEUP();

	//	완료된 리스트에 추가.
	m_cQuestPlay.InsertEnd ( *pQUEST_PROG );

	CByteStream cByteStream;
	pQUEST_PROG->SET_BYBUFFER ( cByteStream );

	LPBYTE pBuff(NULL);
	DWORD dwSize(0);
	cByteStream.GetBuffer ( pBuff, dwSize );

	//	진행정보 삭제.
	m_cQuestPlay.DeleteProc ( dwQUESTID );

	//	Note : 퀘스트 완료 정보 전송.
	//
	GLMSG::SNET_QUEST_END_STREAM NetMsgStream;
	NetMsgStream.SETSTREAM ( pBuff, dwSize );
	GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgStream );

	//	Note : 퀘스트 진행 정보 삭제 전송.
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

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )							return S_FALSE;

	//	진행중인 퀘스트 인지 점검.
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

	//	Note : 퀘스트 진행을 위한 검사.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

	//	진행중인 퀘스트 인지 점검.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
	{
		NetMsgFB.emFB = EMQUEST_COMPLETE_FB_FAIL;
		GLGaeaServer::GetInstance().SENDTOCLIENT ( m_dwClientID, (NET_MSG_GENERIC*) &NetMsgFB );
		return S_FALSE;
	}

		//	Note : 퀘스트가 완료됫는지 검사.
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

