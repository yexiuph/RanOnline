#include "pch.h"
#include "./GLCharacter.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLCharacter::MsgProcessAlliance( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB:
		{
			GLMSG::SNET_CLUB_ALLIANCE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_ALLIANCE_REQ_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_FAIL") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_OK") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTMASTER") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_NOTCHIEF") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TARNOTMASTER") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_ALREADY") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISSOLUTION2") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_MAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_MAX") );
				break;
			case EMCLUB_ALLIANCE_REQ_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_REFUSE") );
				break;

			case EMCLUB_ALLIANCE_REQ_FB_BOTHGUID:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_BOTHGUID") );
				break;

			case EMCLUB_ALLIANCE_REQ_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_GUIDMAP") );
				break;

			case EMCLUB_ALLIANCE_REQ_FB_DISTIME:
				{
					CTime cTime(pNetMsg->tBlock);
					CString strExpireDate;

					strExpireDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
																				cTime.GetYear(), 
																				cTime.GetMonth(), 
																				cTime.GetDay(), 
																				cTime.GetHour(), 
																				cTime.GetMinute() );

					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_DISTIME"), strExpireDate.GetString() );
				}
				break;

			case EMCLUB_ALLIANCE_REQ_FB_SECTIME:
				{
					CTime cTime(pNetMsg->tBlock);
					CString strExpireDate;

					strExpireDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
																				cTime.GetYear(), 
																				cTime.GetMonth(), 
																				cTime.GetDay(), 
																				cTime.GetHour(), 
																				cTime.GetMinute() );

					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_SECTIME"), strExpireDate.GetString() );
				}
				break;

			case EMCLUB_ALLIANCE_REQ_FB_TOSECTIME:
				{
					CTime cTime(pNetMsg->tBlock);
					CString strExpireDate;

					strExpireDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
						cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );

					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TOSECTIME"), strExpireDate.GetString() );
				}
				break;

			case EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_OTHERSCHOOL") );
				}
				break;
			case EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE:
				{	
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_CLUBBATTLE") );
				}
				break;
			case EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_REQ_FB_TARCLUBBATTLE") );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB:
		{
			GLMSG::SNET_CLUB_ALLIANCE_DEL_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_ALLIANCE_DEL_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_FAIL") );
				break;
			case EMCLUB_ALLIANCE_DEL_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_OK") );
				break;
			case EMCLUB_ALLIANCE_DEL_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTMASTER") );
				break;
			case EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_NOTCHIEF") );
				break;
			case EMCLUB_ALLIANCE_DEL_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_GUIDMAP") );
				break;
			case EMCLUB_ALLIANCE_DEL_FB_BATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DEL_FB_BATTLE") );
				break;
			};
		}
		break;
	case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB:
		{
			GLMSG::SNET_CLUB_ALLIANCE_SEC_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_SEC_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_ALLIANCE_SEC_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_FAIL") );
				break;
			case EMCLUB_ALLIANCE_SEC_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_OK") );
				break;
			case EMCLUB_ALLIANCE_SEC_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_NOTMASTER") );
				break;
			case EMCLUB_ALLIANCE_SEC_FB_ALLIANCE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_ALLIANCE") );
				break;
			case EMCLUB_ALLIANCE_SEC_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_GUIDMAP") );
				break;
			case EMCLUB_ALLIANCE_SEC_FB_BATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_SEC_FB_BATTLE") );
				break;				
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB:
		{
			GLMSG::SNET_CLUB_ALLIANCE_DIS_FB *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_ALLIANCE_DIS_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_FAIL") );
				break;
			case EMCLUB_ALLIANCE_DIS_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_OK") );
				break;
			case EMCLUB_ALLIANCE_DIS_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTMASTER") );
				break;
			case EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_NOTCHIEF") );
				break;
			case EMCLUB_ALLIANCE_DIS_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_GUIDMAP") );
				break;
			case EMCLUB_ALLIANCE_DIS_FB_BATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_ALLIANCE_DIS_FB_BATTLE") );
				break;				
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK:
		{
			GLMSG::SNET_CLUB_ALLIANCE_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_REQ_ASK *)nmg;

			CInnerInterface::GetInstance().DOMODAL_UNION_JOIN_ASK( pNetMsg->dwChiefCharID, pNetMsg->szChiefName );
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT:
		{
			GLMSG::SNET_CLUB_ALLIANCE_ADD_CLT *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_ADD_CLT *)nmg;

			m_sCLUB.m_dwAlliance = pNetMsg->dwChiefClubID;
			m_sCLUB.AddAlliance ( pNetMsg->dwChiefClubID, pNetMsg->szChiefClub );
			m_sCLUB.AddAlliance ( pNetMsg->dwIndianClubID, pNetMsg->szIndianClub );
			
			m_sCLUB.m_dwAllianceBattleWin = pNetMsg->dwAllianceBattleWin;
			m_sCLUB.m_dwAllianceBattleLose = pNetMsg->dwAllianceBattleLose;
			m_sCLUB.m_dwAllianceBattleDraw = pNetMsg->dwAllianceBattleDraw;

			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("ALLIANCE_ADD"), pNetMsg->szChiefClub, pNetMsg->szIndianClub );

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE();
			CInnerInterface::GetInstance().REFRESH_CLUB_ALLIANCE_LIST();
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT:
		{
			GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DEL_CLT *)nmg;

			// 삭제될 클럽이 자신이면 모든 동맹정보 삭제함.
			if ( m_sCLUB.m_dwID==pNetMsg->dwDelClubID )
			{
				m_sCLUB.ClearAlliance();
			}
			// 아니면 동맹들 중에서 찾아서 삭제함.
			else
			{
				CLUB_ALLIANCE_ITER del = m_sCLUB.m_setAlliance.find ( GLCLUBALLIANCE(pNetMsg->dwDelClubID,"") );
				if ( m_sCLUB.m_setAlliance.end()!=del )
				{
					//	동맹에서 제외.
					m_sCLUB.m_setAlliance.erase ( del );
				}
			}

			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("ALLIANCE_DEL"), pNetMsg->szDelClub );

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE();
			CInnerInterface::GetInstance().REFRESH_CLUB_ALLIANCE_LIST();
		}
		break;

	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT:
		{
			GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT *pNetMsg = (GLMSG::SNET_CLUB_ALLIANCE_DIS_CLT *)nmg;

			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("ALLIANCE_DIS"), pNetMsg->szChiefClub );

			m_sCLUB.ClearAlliance();

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE();
			CInnerInterface::GetInstance().REFRESH_CLUB_ALLIANCE_LIST();
		}
		break;	

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};
}

