#include "pch.h"
#include "./GLCharacter.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void GLCharacter::MsgProcessClubBattle( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB:
		{

			GLMSG::SNET_CLUB_BATTLE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			
			case EMCLUB_BATTLE_REQ_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_FAIL") );
				break;
			case EMCLUB_BATTLE_REQ_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_OK") );
				break;
			case EMCLUB_BATTLE_REQ_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_NOTMASTER") );
				break;
			case EMCLUB_BATTLE_REQ_FB_TARNOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARNOTMASTER") );
				break;
			case EMCLUB_BATTLE_REQ_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY") );
				break;
			case EMCLUB_BATTLE_REQ_FB_ALREADY2:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_ALREADY2") );
				break;
			case EMCLUB_BATTLE_REQ_FB_DISSOLUTION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_DISSOLUTION") );
				break;
			case EMCLUB_BATTLE_REQ_FB_DISSOLUTION2:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_DISSOLUTION2") );
				break;
			case EMCLUB_BATTLE_REQ_FB_MAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_MAX") );
				break;
			case EMCLUB_BATTLE_REQ_FB_TARMAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARMAX") );
				break;
			case EMCLUB_BATTLE_REQ_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_GUIDMAP") );
				break;
			case EMCLUB_BATTLE_REQ_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_REFUSE") );
				break;
			case EMCLUB_BATTLE_REQ_FB_RANK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_RANK") );
				break;
			case EMCLUB_BATTLE_REQ_FB_TARRANK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARRANK") );
				break;
			case EMCLUB_BATTLE_REQ_FB_CONFT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CONFT") );
				break;
			case EMCLUB_BATTLE_REQ_FB_TARCONFT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TARCONFT") );
				break;
			case EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_GUIDNBATTLE") );
				break;
			case EMCLUB_BATTLE_REQ_FB_TIMEMIN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE
					, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMIN")
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60);
				break;
			case EMCLUB_BATTLE_REQ_FB_TIMEMAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE
					, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_TIMEMAX")
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60);
				break;
			case EMCLUB_BATTLE_REQ_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_REQ_FB_CDMMAP") );
				break;
				
			}
		}
		break;
	
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK:
		{
			GLMSG::SNET_CLUB_BATTLE_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_REQ_ASK *)nmg;
			CInnerInterface::GetInstance().DOMODAL_CLUB_BATTLE_ASK( pNetMsg->dwClubCharID, pNetMsg->szClubName, pNetMsg->dwBattleTime );

		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT:
		{
			GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT *)nmg;
			
			if ( pNetMsg->bAlliance ) 
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
					ID2GAMEINTEXT("ALLIANCE_BATTLE_BEGIN_READY"), m_sCLUB.m_szName, pNetMsg->szClubName );
			}
			else
			{	
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
					ID2GAMEINTEXT("CLUB_BATTLE_BEGIN_READY"), m_sCLUB.m_szName, pNetMsg->szClubName );
			}
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2:
		{
			GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT2 *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_BEGIN_CLT2 *)nmg;

			GLCLUBBATTLE sClubBattle;
			sClubBattle.m_dwCLUBID =pNetMsg->dwClubID;
			sClubBattle.m_tStartTime = pNetMsg->tStartTime;
			sClubBattle.m_tEndTime = pNetMsg->tEndTime;
			sClubBattle.m_bAlliance = pNetMsg->bAlliance;
			StringCchCopy ( sClubBattle.m_szClubName, CHAR_SZNAME, pNetMsg->szClubName );

			m_sCLUB.ADDBATTLECLUB( sClubBattle );
			
			if ( pNetMsg->bAlliance )
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
					ID2GAMEINTEXT("ALLIANCE_BATTLE_BEGIN"), m_sCLUB.m_szName, pNetMsg->szClubName );
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
					ID2GAMEINTEXT("CLUB_BATTLE_BEGIN"), m_sCLUB.m_szName, pNetMsg->szClubName );
			}


			CInnerInterface::GetInstance().REFRESH_CLUB_BATTLE_LIST();			
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT:
		{
			GLMSG::SNET_CLUB_BATTLE_OVER_CLT *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_OVER_CLT *)nmg;

			GLCLUBBATTLE* pClubBattle = m_sCLUB.GetClubBattle( pNetMsg->dwClubID );
			if ( !pClubBattle )	return;

			CString strClubName = pClubBattle->m_szClubName;
			if ( pClubBattle->m_bAlliance )
			{
				switch( pNetMsg->emFB )
				{
				case EMCLUB_BATTLE_OVER_DRAW:
					{
						m_sCLUB.m_dwAllianceBattleDraw++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_DRAW"), 
							m_sCLUB.m_szName, strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_WIN:
					{
						m_sCLUB.m_dwAllianceBattleWin++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_WIN"), 
							strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint );
					}
					break;
				case EMCLUB_BATTLE_OVER_LOSE:
					{
						m_sCLUB.m_dwAllianceBattleLose++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_LOSE"), 
							strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint );
					}
					break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:
					{
						m_sCLUB.m_dwAllianceBattleDraw++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_ARMISTICE"), 
							strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:
					{
						m_sCLUB.m_dwAllianceBattleLose++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_SUBMISSION"), 
							strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:
					{
						m_sCLUB.m_dwAllianceBattleWin++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("ALLIANCE_BATTLE_OVER_TARSUBMISSION"), 
							strClubName );
					}
					break;
				}

			}
			else
			{
				switch( pNetMsg->emFB )
				{
				case EMCLUB_BATTLE_OVER_DRAW:
					{
						m_sCLUB.m_dwBattleDraw++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_DRAW"), 
							m_sCLUB.m_szName, strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_WIN:
					{
						m_sCLUB.m_dwBattleWin++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_WIN"), 
							strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint );
					}
					break;
				case EMCLUB_BATTLE_OVER_LOSE:
					{
						m_sCLUB.m_dwBattleLose++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_LOSE"), 
							strClubName, pNetMsg->wKillPoint, pNetMsg->wDeathPoint );
					}
					break;
				case EMCLUB_BATTLE_OVER_ARMISTICE:
					{
						m_sCLUB.m_dwBattleDraw++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_ARMISTICE"), 
							strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_SUBMISSION:
					{
						m_sCLUB.m_dwBattleLose++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_SUBMISSION"), 
							strClubName );
					}
					break;
				case EMCLUB_BATTLE_OVER_TARSUBMISSION:
					{
						m_sCLUB.m_dwBattleWin++;
						CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::IDCOLOR, 
							ID2GAMEINTEXT("CLUB_BATTLE_OVER_TARSUBMISSION"), 
							strClubName );
					}
					break;
				}
			}


			
			m_sCLUB.DELBATTLECLUB( pNetMsg->dwClubID );
			
			CInnerInterface::GetInstance().REFRESH_CLUB_BATTLE_LIST();
			CInnerInterface::GetInstance().REFRESH_CLUB_STATE();
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB:
		{
			GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_BATTLE_ARMISTICE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_FAIL") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_OK") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOTMASTER") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_OFFMASTER") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_REFUSE") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_GUIDMAP") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_NOBATTLE") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_DISTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_DISTIME") );
				break;
			case EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_ARMISTICE_FB_CDMMAP") );
				break;
			}			
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK:
		{
			GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_ARMISTICE_REQ_ASK *)nmg;
			
			GLCLUBBATTLE* pClubBattle = m_sCLUB.GetClubBattle( pNetMsg->dwClubID );
			if ( !pClubBattle ) return;

			CInnerInterface::GetInstance().DOMODAL_CLUB_BATTLE_ARMISTICE_ASK( pNetMsg->dwClubID, pClubBattle->m_szClubName );
		}
		break;

	case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB:
		{
			GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_SUBMISSION_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_BATTLE_SUBMISSION_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_FAIL") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_OK") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOTMASTER") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_GUIDMAP") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_NOBATTLE") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_DISTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_DISTIME") );
				break;
			case EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_BATTLE_SUBMISSION_FB_CDMMAP") );
				break;				
			}
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE:
		{
			GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_KILL_UPDATE *)nmg;
			
			GLCLUBBATTLE* pClubBattle = m_sCLUB.GetClubBattle( pNetMsg->dwBattleClubID );
			if ( !pClubBattle ) return;

			pClubBattle->m_wKillPoint = pNetMsg->wKillPoint;
			pClubBattle->m_wDeathPoint = pNetMsg->wDeathPoint;
			
			CInnerInterface::GetInstance().REFRESH_CLUB_BATTLE_LIST();
		}
		break;
	case NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE:
		{
			GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE *pNetMsg = (GLMSG::SNET_CLUB_BATTLE_POINT_UPDATE *)nmg;
			
			GLCLUBBATTLE* pClubBattle = m_sCLUB.GetClubBattle( pNetMsg->dwBattleClubID );
			if ( !pClubBattle ) return;

			if ( pNetMsg->bKillPoint )
			{
				pClubBattle->m_wKillPoint++;
				pClubBattle->m_wKillPointTemp++;
			}
			else
			{
				pClubBattle->m_wDeathPoint++;
				pClubBattle->m_wDeathPointTemp++;
			}
			
			CInnerInterface::GetInstance().REFRESH_CLUB_BATTLE_LIST();
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	}
}

void GLCharacter::MsgProcessAllianceBattle( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB:
		{
			
			GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			
			case EMALLIANCE_BATTLE_REQ_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_FAIL") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_OK") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_NOTMASTER") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARNOTMASTER") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_ALREADY2:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_ALREADY2") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_DISSOLUTION2") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_MAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_MAX") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TARMAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARMAX") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_GUIDMAP") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_REFUSE") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_RANK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_RANK") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TARRANK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARRANK") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_CONFT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CONFT") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TARCONFT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TARCONFT") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_GUIDNBATTLE") );
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TIMEMIN:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE
					, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMIN")
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN / 60
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMIN % 60);
				break;
			case EMALLIANCE_BATTLE_REQ_FB_TIMEMAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE
					, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_TIMEMAX")
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX / 60
					, GLCONST_CHAR::dwCLUB_BATTLE_TIMEMAX % 60);
				break;
			case EMALLIANCE_BATTLE_REQ_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_REQ_FB_CDMMAP") );
				break;
			}
		}
		break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK:
		{
			GLMSG::SNET_ALLIANCE_BATTLE_REQ_ASK *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_REQ_ASK *)nmg;
			CInnerInterface::GetInstance().DOMODAL_CLUB_BATTLE_ASK( pNetMsg->dwClubCharID, pNetMsg->szClubName, pNetMsg->dwBattleTime, true );
		}
		break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB:
		{
			GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_FAIL") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_OK") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOTMASTER") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_OFFMASTER") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_REFUSE") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_GUIDMAP") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_NOBATTLE") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_DISTIME") );
				break;
			case EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_ARMISTICE_FB_CDMMAP") );
				break;
			}			
		}
		break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK:
		{
			GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK *)nmg;
			
			GLCLUBBATTLE* pClubBattle = m_sCLUB.GetClubBattle( pNetMsg->dwClubID );
			if ( !pClubBattle ) return;

			CInnerInterface::GetInstance().DOMODAL_CLUB_BATTLE_ARMISTICE_ASK( pNetMsg->dwClubID, pClubBattle->m_szClubName, true );
		}
		break;

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB:
		{
			GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB *pNetMsg = (GLMSG::SNET_ALLIANCE_BATTLE_SUBMISSION_REQ_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_FAIL") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::ENABLE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_OK") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOTMASTER") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_GUIDMAP") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_NOBATTLE") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_DISTIME") );
				break;
			case EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMALLIANCE_BATTLE_SUBMISSION_FB_CDMMAP") );
				break;
			}
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	}
}