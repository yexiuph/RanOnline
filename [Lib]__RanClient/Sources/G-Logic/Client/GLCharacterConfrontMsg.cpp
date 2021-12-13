#include "pch.h"
#include "./GLCharacter.h"
#include "./GLPartyClient.h"
#include "./GLGaeaClient.h"

#include "./DxGlobalStage.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLCharacter::MsgProcessConfront( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_CONFRONT_FB:
		{
			GLMSG::SNETPC_REQ_CONFRONT_FB *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCONFRONT_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_FAIL") );
				break;
			case EMCONFRONT_REFUSE:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_REFUSE") );
				break;
			case EMCONFRONT_PEACE:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PEACE") );
				break;
			case EMCONFRONT_MAX:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_MAX"), GLCONST_CHAR::dwCONFRONT_MAX, (int)pNetMsg->fTIME );
				break;
			case EMCONFRONT_ELAP:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ELAP"), (int)pNetMsg->fTIME );
				break;
			case EMCONFRONT_ALREADY_ME:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ALREADY_ME") );
				break;
			case EMCONFRONT_ALREADY_TAR:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ALREADY_TAR") );
				break;
			case EMCONFRONT_PARTY:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PARTY") );
				break;
			case EMCONFRONT_DIE_YOU:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_DIE_YOU") );
				break;
			case EMCONFRONT_DIE_ME:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_DIE_ME") );
				break;
			};

			m_sCONFTING.RESET();
			DelConftBoundEffect();
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONT_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONT_START2_CLT *)nmg;

			m_sCONFTING.RESET();

			//	Note : 유효성 검사. strNAME이 비여 있다면 무효로 판단.
			//
			std::string strNAME;
			switch ( pNetMsg->emTYPE )
			{
			case EMCONFT_ONE:
				{
					PGLCHARCLIENT pChar = GLGaeaClient::GetInstance().GetChar ( pNetMsg->dwTARID );
					if ( pChar )	strNAME = pChar->GetName();
				}
				break;

			case EMCONFT_PARTY:
				break;

			case EMCONFT_GUILD:
				break;
			};

			if ( strNAME.empty() )	return;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("CONFT_START"), strNAME.c_str() );

			//	Note : 대련 정보 설정.
			//
			m_sCONFTING.emTYPE = pNetMsg->emTYPE;
			m_sCONFTING.dwTAR_ID = pNetMsg->dwTARID;
			m_sCONFTING.strTAR_NAME = strNAME;
			m_sCONFTING.vPosition = pNetMsg->vPosition;
			m_sCONFTING.sOption = pNetMsg->sOption;

			//	Note : 경계 표시.
			//
			if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

			// 대련이 시작되면 팻은 사라진다.
			PGLPETCLIENT pMyPet = GLGaeaClient::GetInstance().GetPetClient ();
			if ( pMyPet->IsVALID () )
			{
				GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg;
				NetMsg.dwGUID = pMyPet->m_dwGUID;
				NetMsg.dwPetID = pMyPet->m_dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}

			// 대련이 시작되면 내린다.
			if ( m_bVehicle ) 
			{
				ReqSetVehicle( false );
			}
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT:
		{
			m_sCONFTING.DoFIGHTING();
			CInnerInterface::GetInstance().SetFightBegin ( 0 );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_END2_CLT:
		{
			GLMSG::SNETPC_CONFRONT_END2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_CLT *)nmg;

			if ( !m_sCONFTING.IsCONFRONTING () )		break;

			switch ( pNetMsg->emEND )
			{
			case EMCONFRONT_END_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_FAIL") );
				break;

			case EMCONFRONT_END_WIN:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_WIN_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCONFRONT_END_WIN"), m_sCONFTING.GETTARNAME() );

				SetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_LOSS:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_LOSE_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_LOSS"), m_sCONFTING.GETTARNAME() );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_NOTWIN:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_NOTWIN") );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_TIME:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_DRAW_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_TIME"), m_sCONFTING.GETTARNAME() );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_MOVEMAP:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_DRAW_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_MOVEMAP"), m_sCONFTING.GETTARNAME() );
				break;

			case EMCONFRONT_END_DISWIN:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_DRAW_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_DISWIN"), m_sCONFTING.GETTARNAME() );

				SetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_DISLOSS:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_DRAW_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_DISLOSS"), m_sCONFTING.GETTARNAME() );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PARTY:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_DRAW_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_PARTY"), m_sCONFTING.GETTARNAME() );
				break;
			};

			//	Note : 대련 취소됨.
			m_sCONFTING.RESET();
			DelConftBoundEffect();

			//	Note : 연속 강제 공격 취소.
			m_sREACTION.RESET();

			//	Note : 학교간 대련 마크 표시 OFF.
			CInnerInterface::GetInstance().SetAcademyFightEnd();
		}
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONTPTY_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_START2_CLT *) nmg;

			GLPartyClient::GetInstance().SetConfront ( (NET_MSG_GENERIC*)pNetMsg );

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("CONFTPTY_START"), pNetMsg->szName );

			GLPARTY_CLIENT *pMyInfo = GLPartyClient::GetInstance().FindMember ( m_dwGaeaID );
			if ( pMyInfo && pMyInfo->m_bConfront )
			{
				m_sCONFTING.RESET();

				m_sCONFTING.emTYPE = EMCONFT_PARTY;
				m_sCONFTING.dwTAR_ID = pNetMsg->dwconftPARTY;
				m_sCONFTING.strTAR_NAME = pNetMsg->szName;
				m_sCONFTING.vPosition = pNetMsg->vPosition;
				m_sCONFTING.sOption = pNetMsg->sOption;

				m_sCONFTING.setConftMember.clear();
				for ( int i=0; i<MAXPARTY; ++i )
				{
					DWORD dwMEMBER = pNetMsg->dwCONFRONT_TAR_MEMBER[i];
					if ( dwMEMBER==GAEAID_NULL )	continue;
					m_sCONFTING.setConftMember.insert(dwMEMBER);
				}

				//	Note : 경계 표시.
				//
				if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

				//	Note : 학교간 대련 출력.
				//
				if ( m_sCONFTING.sOption.bSCHOOL )
				{
					//	Note : 학교간 대련 마크 표시 ON.
					CInnerInterface::GetInstance().SetAcademyFightBegin
						(
						m_sCONFTING.sOption.wMY_SCHOOL,
						GLPartyClient::GetInstance().GetConfrontNum (),
						m_sCONFTING.sOption.wTAR_SCHOOL,
						(int) m_sCONFTING.setConftMember.size()
						);
				}

				// 대련이 시작되면 팻은 사라진다.
				PGLPETCLIENT pMyPet = GLGaeaClient::GetInstance().GetPetClient ();
				if ( pMyPet->IsVALID () )
				{
					GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg;
					NetMsg.dwGUID = pMyPet->m_dwGUID;
					NetMsg.dwPetID = pMyPet->m_dwPetID;
					NETSENDTOFIELD ( &NetMsg );
				}

				// 대련이 시작되면 내린다.
				if ( m_bVehicle ) 
				{
					ReqSetVehicle( false );
				}
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CONFTPTY_START_NOT") );
			}
		}
		break;

	case NET_MSG_GCTRL_CONFRONTPTY_END2_CLT:
		{
			GLMSG::SNETPC_CONFRONTPTY_END2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTPTY_END2_CLT *) nmg;

			switch ( pNetMsg->emEND )
			{
			case EMCONFRONT_END_PWIN:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_WIN_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCONFRONT_END_PWIN") );

				SetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PLOSS:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_LOSE_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_PLOSS") );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_PTIME:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_PTIME") );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CWIN:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_WIN_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCONFRONT_END_CWIN") );

				SetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CLOSS:
				CInnerInterface::GetInstance().SetFightEnd ( CONFT_LOSE_INDEX );
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_CLOSS") );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;

			case EMCONFRONT_END_CTIME:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_END_CTIME") );

				ReSetSTATE(EM_ACT_CONFT_WIN);
				TurnAction ( GLAT_CONFT_END );
				break;
			};

			//	Note : 승리하였을 경우 아직 리셋이 안되어 있음.
			//
			m_sCONFTING.RESET();
			DelConftBoundEffect();

			//	Note : 연속 강제 공격 취소.
			m_sREACTION.RESET();

			//	Note : 학교간 대련 마크 표시 OFF.
			CInnerInterface::GetInstance().SetAcademyFightEnd();
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_RECOVE:
		{
			GLMSG::SNETPC_CONFRONT_RECOVE *pNetMsg = (GLMSG::SNETPC_CONFRONT_RECOVE *) nmg;

			//	Note : 회복약 사용 횟수 업대이트.
			//
			m_sCONFTING.wRECOVER = pNetMsg->wRECOVER;

			int nRECOVER = m_sCONFTING.sOption.wRECOVER-m_sCONFTING.wRECOVER;
			const DWORD dwCOLOR = nRECOVER>0 ? NS_UITEXTCOLOR::PALEGREEN : NS_UITEXTCOLOR::DISABLE;
			CInnerInterface::GetInstance().PrintMsgText ( dwCOLOR, ID2GAMEINTEXT("CONFRONT_RECOVE"), nRECOVER );
		}
		break;

	case NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR:
		{
			GLMSG::SNETPC_CONFRONT_END2_CLT_MBR *pNetMsg = (GLMSG::SNETPC_CONFRONT_END2_CLT_MBR *)nmg;

			DELCONFRONT_MEMBER(pNetMsg->dwID);
		}
		break;

	case NET_MSG_GCTRL_CONFRONTCLB_START2_CLT:
		{
			GLMSG::SNETPC_CONFRONTCLB_START2_CLT *pNetMsg = (GLMSG::SNETPC_CONFRONTCLB_START2_CLT *) nmg;

			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("CONFTCLB_START"), pNetMsg->szName );

			DWORD i(0);
			for ( i=0; i<pNetMsg->dwCLUB_MY_NUM; ++i )
			{
				m_sCLUB.ADDCONFT_MEMBER ( pNetMsg->dwCONFRONT_MY_MEMBER[i] );
			}

			m_sCONFTING.RESET();

			m_sCONFTING.emTYPE = EMCONFT_GUILD;
			m_sCONFTING.dwTAR_ID = pNetMsg->dwconftCLUB;
			m_sCONFTING.strTAR_NAME = pNetMsg->szName;
			m_sCONFTING.vPosition = pNetMsg->vPosition;
			m_sCONFTING.sOption = pNetMsg->sOption;

			m_sCONFTING.setConftMember.clear();
			for ( i=0; i<pNetMsg->dwCLUB_TAR_NUM; ++i )
			{
				m_sCONFTING.setConftMember.insert(pNetMsg->dwCONFRONT_TAR_MEMBER[i]);
			}

			//	Note : 경계 표시.
			//
			if ( m_sCONFTING.sOption.bBOUND )	NewConftBoundEffect(m_sCONFTING.emTYPE);

			// 대련이 시작되면 팻은 사라진다.
			PGLPETCLIENT pMyPet = GLGaeaClient::GetInstance().GetPetClient ();
			if ( pMyPet->IsVALID () )
			{
				GLMSG::SNETPET_REQ_UNUSEPETCARD NetMsg;
				NetMsg.dwGUID = pMyPet->m_dwGUID;
				NetMsg.dwPetID = pMyPet->m_dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}

			// 대련이 시작되면 내린다.
			if ( m_bVehicle ) 
			{
				ReqSetVehicle( false );
			}
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};
}

