#include "pch.h"
#include "./DxGlobalStage.h"
#include "./GLCharacter.h"
#include "./GLQuest.h"
#include "./GLQuestMan.h"
#include "./GLItemMan.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#include "../[Lib]__Engine/Sources/DxEffect/DxEffGroupPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLCharacter::MsgProcessQuest( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_REQ_QUEST_START_FB:
		{
			GLMSG::SNETPC_REQ_QUEST_START_FB *pNetNsg = (GLMSG::SNETPC_REQ_QUEST_START_FB *)nmg;
			switch ( pNetNsg->emFB )
			{
			case EMQUEST_START_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
				break;
			case EMQUEST_START_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
				break;
			case EMQUEST_START_FB_DONAGAIN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONAGAIN") );
				break;
			case EMQUEST_START_FB_DONREPEAT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
				break;
			case EMQUEST_START_FB_NEEDMONEY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_NEEDMONEY") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_STREAM:
		{
			GLMSG::SNET_QUEST_PROG_STREAM *pNetMsg = (GLMSG::SNET_QUEST_PROG_STREAM *) nmg;

			CByteStream sByteStream;
			sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

			//	Note : 퀘스트 생성.
			//
			GLQUESTPROG sQUEST_PROG_NEW;
			sQUEST_PROG_NEW.GET_BYBUFFER ( sByteStream );

			//	Note : 리스트에 등록.
			//
			m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW );

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( sQUEST_PROG_NEW.m_sNID.dwID );
			if ( !pQUEST )	break;

			//	Note : 새로운 퀘스트 시작 메시지 출력.
			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_START"), pQUEST->GetTITLE() );

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = m_matTrans;
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
				(
				GLCONST_CHAR::strQUEST_START.c_str(),
				matCharHeight,
				&sTargetID
				);

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM:
		{
			GLMSG::SNET_QUEST_PROG_STEP_STREAM *pNetMsg = (GLMSG::SNET_QUEST_PROG_STEP_STREAM *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		return;

			pPROG->m_dwFlags = pNetMsg->dwFlag;
			pPROG->m_dwSTEP = pNetMsg->dwQSTEP;

			CByteStream cByteStream;
			cByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

			pPROG->m_sSTEP_PROG.GET_BYBUFFER ( cByteStream );


			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )	break;

			GLQUEST_STEP* pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
			if ( !pSTEP )	break;

			//	Note : 완료 메시지 출력.
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_STEP"), pSTEP->GetTITLE() );

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = m_matTrans;
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
				(
				GLCONST_CHAR::strQUEST_START.c_str(),
				matCharHeight,
				&sTargetID
				);

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN:
		{
			GLMSG::SNET_QUEST_PROG_INVEN *pNetMsg = (GLMSG::SNET_QUEST_PROG_INVEN *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		return;

			CByteStream cByteStream;
			cByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

			pPROG->m_sINVENTORY.SETITEM_BYBUFFER ( cByteStream );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_DEL:
		{
			GLMSG::SNET_QUEST_PROG_DEL *pNetMsg = (GLMSG::SNET_QUEST_PROG_DEL *) nmg;

			//	Note : '진행퀘스트 리스트'에서 퀘스트 정보 삭제.
			//
			m_cQuestPlay.DeleteProc ( pNetMsg->dwQUESTID );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_END_STREAM:
		{
			GLMSG::SNET_QUEST_END_STREAM *pNetMsg = (GLMSG::SNET_QUEST_END_STREAM *) nmg;

			//	Note : 버퍼 생성.
			//
			CByteStream sByteStream;
			sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

			//	Note : 퀘스트 생성.
			//
			GLQUESTPROG sQUEST_PROG_END;
			sQUEST_PROG_END.GET_BYBUFFER ( sByteStream );

			//	Note : 리스트에 등록.
			//
			m_cQuestPlay.InsertEnd ( sQUEST_PROG_END );

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( sQUEST_PROG_END.m_sNID.dwID );
			if ( !pQUEST )	break;

			std::string strEND_QUEST;
			//	Note : 완료 메시지 출력.
			if ( sQUEST_PROG_END.m_bCOMPLETE )
			{
				strEND_QUEST = GLCONST_CHAR::strQUEST_END_SUCCEED;
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_COMPLETE"), pQUEST->GetTITLE() );
			}
			else
			{
				strEND_QUEST = GLCONST_CHAR::strQUEST_END_FAIL;
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("QUEST_FAIL"), pQUEST->GetTITLE() );
			}

			// Note : 캐릭터 머리위에서 나오는 효과들이 있다.
			//			그 녀석들을 위한 것.
			D3DXMATRIX matCharHeight = m_matTrans;
			matCharHeight._42 += m_fHeight;

			STARGETID sTargetID(CROW_PC,m_dwGaeaID,m_vPos);
			DxEffGroupPlayer::GetInstance().NewEffGroup
				(
				strEND_QUEST.c_str(),
				matCharHeight,
				&sTargetID
				);

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_END_DEL:
		{
			GLMSG::SNET_QUEST_END_DEL *pNetMsg = (GLMSG::SNET_QUEST_END_DEL *) nmg;

			//	Note : '종료퀘스트 리스트'에서 퀘스트 정보 삭제.
			//
			m_cQuestPlay.DeleteEnd ( pNetMsg->dwQUESTID );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB:
		{
			GLMSG::SNET_QUEST_STEP_NPC_TALK_FB *pNetMsg = (GLMSG::SNET_QUEST_STEP_NPC_TALK_FB *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bTALK = true;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
			if ( !pQUEST )	break;

			GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
			if ( !pSTEP )	break;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_NPCTALK() );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
		{
			GLMSG::SNET_QUEST_PROG_MOBKILL *pNetMsg = (GLMSG::SNET_QUEST_PROG_MOBKILL *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = pNetMsg->bMOBKILL_ALL;
			pPROG->m_sSTEP_PROG.m_dwNUM_MOBKILL = pNetMsg->dwMOBKILL;

			if ( pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;

				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_MOBKILL() );
			}

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL:
		{
			GLMSG::SNET_QUEST_PARTY_PROG_MOBKILL *pNetMsg = (GLMSG::SNET_QUEST_PARTY_PROG_MOBKILL *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL = pNetMsg->bMOBKILL_ALL;

			if ( pPROG->m_sSTEP_PROG.m_bMOBKILL_ALL )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;

				GLMSG::SNETPC_REQ_QUEST_COMPLETE NetMsg;
				NetMsg.dwQID = pNetMsg->dwQID;

				NETSENDTOFIELD ( &NetMsg );                
			}
		}
		break;


	case NET_MSG_GCTRL_QUEST_PROG_QITEM:
		{
			GLMSG::SNET_QUEST_PROG_QITEM *pNetMsg = (GLMSG::SNET_QUEST_PROG_QITEM *) nmg;
			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bQITEM = pNetMsg->bQITEM;

			if ( pPROG->m_sSTEP_PROG.m_bQITEM )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;

				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_MOBGEN_QITEM() );
			}

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM:
		{
			GLMSG::SNET_QUEST_PARTY_PROG_QITEM *pNetMsg = (GLMSG::SNET_QUEST_PARTY_PROG_QITEM *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bQITEM = pNetMsg->bQITEM;

			if ( pPROG->m_sSTEP_PROG.m_bQITEM )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;

				GLMSG::SNETPC_REQ_QUEST_COMPLETE NetMsg;
				NetMsg.dwQID = pNetMsg->dwQID;

				NETSENDTOFIELD ( &NetMsg );                
			}
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_REACHZONE:
		{
			GLMSG::SNET_QUEST_PROG_REACHZONE *pNetMsg = (GLMSG::SNET_QUEST_PROG_REACHZONE *) nmg;
			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG || pPROG->m_dwSTEP!=pNetMsg->dwQSTEP )
				break;

			pPROG->m_sSTEP_PROG.m_bREACH_ZONE = pNetMsg->bREACHZONE;

			if ( pPROG->m_sSTEP_PROG.m_bREACH_ZONE )
			{
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pPROG->m_sNID.dwID );
				if ( !pQUEST )	break;

				GLQUEST_STEP *pSTEP = pQUEST->GetSTEP ( pPROG->m_dwSTEP );
				if ( !pSTEP )	break;

				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("QUEST_STEP_OBJ_COMPLETE"), pSTEP->GetOBJ_REACH_ZONE() );
			}

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_TIMEOVER:
		{
			GLMSG::SNETPC_QUEST_PROG_TIMEOVER *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_TIMEOVER *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("QUEST_TIMEOVER"), pQUEST->GetTITLE() );
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_TIME:
		{
			GLMSG::SNETPC_QUEST_PROG_TIME *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_TIME *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			pPROG->m_fLAPSTIME = pNetMsg->fELAPS;

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_NONDIE:
		{
			GLMSG::SNETPC_QUEST_PROG_NONDIE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_NONDIE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("QUEST_NONDIE"), pQUEST->GetTITLE() );
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP:
		{
			GLMSG::SNETPC_QUEST_PROG_NONDIE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_NONDIE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("QUEST_LEAVEMAP"), pQUEST->GetTITLE() );
		}
		break;
		

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			pPROG->m_sINVENTORY.InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_DELETE *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_DELETE *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )	break;

			pPROG->m_sINVENTORY.DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_TURNNUM *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			SINVENITEM* pInvenItem = pPROG->m_sINVENTORY.GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP:
		{
			GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP *pNetMsg = (GLMSG::SNETPC_QUEST_PROG_INVEN_PICKUP *) nmg;

			SITEM * pITEM = GLItemMan::GetInstance().GetItem ( pNetMsg->sNID_ITEM );
			if ( pITEM )
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("PICKUP_QUESTITEM"), pITEM->GetName() );
			}

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	case NET_MSG_GCTRL_QUEST_COMPLETE_FB:
		{
			GLMSG::SNETPC_QUEST_COMPLETE_FB *pNetMsg = (GLMSG::SNETPC_QUEST_COMPLETE_FB *) nmg;

			GLQUESTPROG* pPROG = m_cQuestPlay.FindProc ( pNetMsg->dwQID );
			if ( !pPROG )		break;

			GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pNetMsg->dwQID );
			if ( !pQUEST )		break;

			switch ( pNetMsg->emFB )
			{
			case EMQUEST_COMPLETE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_FAIL"), pQUEST->GetTITLE() );
				break;
			case EMQUEST_COMPLETE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_OK"), pQUEST->GetTITLE() );
				break;
			case EMQUEST_COMPLETE_FB_NOINVEN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN"), pQUEST->GetTITLE() );
				CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("EMQUEST_COMPLETE_FB_NOINVEN_EX"), pQUEST->GetTITLE() );
				break;
			};

			//	Note : 인터페이스에 갱신 요청.
			CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};
}

