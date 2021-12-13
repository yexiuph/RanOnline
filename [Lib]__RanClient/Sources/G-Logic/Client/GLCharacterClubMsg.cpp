#include "pch.h"
#include "./GLCharacter.h"
#include "./GLGaeaClient.h"
#include "../DxGlobalStage.h"

#include "../[Lib]__RanClientUI/Sources/InnerUI/InnerInterface.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/UITextControl.h"
#include "../[Lib]__RanClientUI/Sources/TextUI/GameTextControl.h"

#include "../[Lib]__Engine/Sources/DxTools/DxClubMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void GLCharacter::MsgProcessClub( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	case NET_MSG_GCTRL_CLUB_INFO_2CLT:
		{
			GLMSG::SNET_CLUB_INFO_2CLT *pNetMsg = (GLMSG::SNET_CLUB_INFO_2CLT *) nmg;

			m_dwGuild = pNetMsg->dwClubID;
			m_sCLUB.m_dwID = pNetMsg->dwClubID;
			StringCchCopy ( m_sCLUB.m_szName, CHAR_SZNAME, pNetMsg->szClubName );

			m_sCLUB.m_dwMasterID = pNetMsg->dwMasterID;
			StringCchCopy ( m_sCLUB.m_szMasterName, CHAR_SZNAME, pNetMsg->szMasterName );
			m_sCLUB.m_dwCDCertifior = pNetMsg->dwCDCertifior;

			m_sCLUB.m_dwMarkVER = pNetMsg->dwMarkVER;
			m_sCLUB.m_dwRank = pNetMsg->dwRank;

			m_sCLUB.m_tOrganize = pNetMsg->tOrganize;
			m_sCLUB.m_tDissolution = pNetMsg->tDissolution;

			StringCchCopy ( m_sCLUB.m_szNotice, EMCLUB_NOTICE_LEN+1, pNetMsg->szNotice );

			StringCchCopy ( m_szNick, CHAR_SZNAME, pNetMsg->szNickName );

			//	Note : 클럽마크 버전 확인.
			//
			ReqClubMarkInfo ( m_sCLUB.m_dwID, m_sCLUB.m_dwMarkVER );

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_DEL_2CLT:
		{
			m_dwGuild = CLUB_NULL;
			m_sCLUB.RESET();

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION:
		{
			GLMSG::SNET_CLUB_INFO_DISSOLUTION *pNetMsg = (GLMSG::SNET_CLUB_INFO_DISSOLUTION *) nmg;
			
			m_sCLUB.m_tDissolution = pNetMsg->tDissolution;
			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
		{
			GLMSG::SNET_CLUB_MEMBER_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_2CLT *) nmg;

			for ( DWORD i=0; i<pNetMsg->dwMemberNum; ++i )
			{
				GLCLUBMEMBER cMEMBER;
				cMEMBER.dwID = pNetMsg->sMEMBER[i].dwID;
				StringCchCopy ( cMEMBER.m_szName, CHAR_SZNAME, pNetMsg->sMEMBER[i].szMember );
				cMEMBER.m_dwFlags = pNetMsg->sMEMBER[i].dwFlag;

				m_sCLUB.m_mapMembers[cMEMBER.dwID] = cMEMBER;
			}

			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_NEW_FB:
		{
			GLMSG::SNET_CLUB_NEW_FB *pNetMsg = (GLMSG::SNET_CLUB_NEW_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_NEW_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_FAIL") );
				break;

			case EMCLUB_NEW_FB_OK:
				CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_NEW_FB_OK"), pNetMsg->szClubName );
				break;

			case EMCLUB_NEW_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_ALREADY") );
				break;

			case EMCLUB_NEW_FB_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMASTER") );
				break;

			case EMCLUB_NEW_FB_NOTMEMBER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMEMBER") );
				break;

			case EMCLUB_NEW_FB_NOTEXIST_MEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTEXIST_MEM"), GLCONST_CHAR::dwCLUB_PARTYNUM );
				break;

			case EMCLUB_NEW_FB_MEM_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_MEM_ALREADY") );
				break;

			case EMCLUB_NEW_FB_MASTER_LVL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_MASTER_LVL"), GLCONST_CHAR::sCLUBRANK[0].m_dwMasterLvl );
				break;

			case EMCLUB_NEW_FB_AVER_LIVING:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_AVER_LIVING"), GLCONST_CHAR::sCLUBRANK[0].m_dwLivingPoint );
				break;

			case EMCLUB_NEW_FB_NOTMONEY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMONEY"), GLCONST_CHAR::sCLUBRANK[0].m_dwPay );
				break;

			case EMCLUB_NEW_FB_NAME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NAME"), pNetMsg->szClubName );
				break;

			case EMCLUB_NEW_FB_BADNAME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_BADNAME"), pNetMsg->szClubName );
				break;

			case EMCLUB_NEW_FB_OTHERSCHOOL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_OTHERSCHOOL") );
				break;
#ifdef TH_PARAM
			case EMCLUB_NEW_FB_THAICHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_THAICHAR_ERROR") );
				break;
#endif
#ifdef VN_PARAM
			case EMCLUB_NEW_FB_VNCHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_VNCHAR_ERROR") );
				break;
#endif
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK:
		{
			GLMSG::SNET_CLUB_MEMBER_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_ASK *) nmg;

			CInnerInterface::GetInstance().DOMODAL_CLUB_JOIN_ASK (
				pNetMsg->dwMaster, pNetMsg->szClubName, pNetMsg->szMasterName );
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB:
		{
			GLMSG::SNET_CLUB_MEMBER_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMCLUB_MEMBER_REQ_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_FAIL"), pNetMsg->szReqName );
				break;

			case EMCLUB_MEMBER_REQ_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OK"), pNetMsg->szReqName );
				break;

			case EMCLUB_MEMBER_REQ_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_REFUSE"), pNetMsg->szReqName );
				break;

			case EMCLUB_MEMBER_REQ_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_ALREADY"), pNetMsg->szReqName );
				break;

			case EMCLUB_MEMBER_REQ_FB_MAXNUM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_MAXNUM") );
				break;

			case EMCLUB_MEMBER_REQ_FB_SECEDED:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_SECEDED"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY );
				break;

			case EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL"), pNetMsg->szReqName );
				break;
			case EMCLUB_MEMBER_REQ_FB_CLUBBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_CLUBBATTLE") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT:
		{
			GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *) nmg;

			//	Note : 클럽 마크 변경.
			m_sCLUB.m_dwMarkVER = pNetMsg->dwMarkVER;
			memcpy ( m_sCLUB.m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

			//	Note : 이미지에 적용.
			DWORD dwServerID = GLGaeaClient::GetInstance().GetCharacter()->m_dwServerID;
			DxClubMan::GetInstance().SetClubData ( DxGlobalStage::GetInstance().GetD3dDevice(), dwServerID, m_dwGuild, pNetMsg->dwMarkVER, m_sCLUB.m_aryMark );

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_RANK_2CLT:
		{
			GLMSG::SNET_CLUB_RANK_2CLT *pNetMsg = (GLMSG::SNET_CLUB_RANK_2CLT *) nmg;

			m_sCLUB.m_dwRank = pNetMsg->dwRank;

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_RANK_FB:
		{
			GLMSG::SNET_CLUB_RANK_FB *pNetMsg = (GLMSG::SNET_CLUB_RANK_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_RANK_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_FAIL") );
				break;
			case EMCLUB_RANK_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_OK") );
				break;
			case EMCLUB_RANK_FB_MAX:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_MAX") );
				break;
			case EMCLUB_RANK_FB_MASTER_LVL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_MASTER_LVL"), pNetMsg->dwValue );
				break;
			case EMCLUB_RANK_FB_AVER_LIVING:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_AVER_LIVING"), pNetMsg->dwValue );
				break;
			case EMCLUB_RANK_FB_NOTMONEY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_NOTMONEY"), pNetMsg->dwValue );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB:
		{
			GLMSG::SNET_CLUB_MEMBER_NICK_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMCLUB_MEMBER_NICK_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_FAIL") );
				break;

			case EMCLUB_MEMBER_NICK_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_OK") );
					StringCchCopy ( m_szNick, CHAR_SZNAME, pNetMsg->szNick );
				}
				break;

			case EMCLUB_MEMBER_NICK_FB_BADNAME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_BADNAME") );
				break;

#ifdef TH_PARAM
			case EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR") );
				break;
#endif

#ifdef VN_PARAM
			case EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR:
				CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR"));
				break;
#endif
			};

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_STATE:
		{
			GLMSG::SNET_CLUB_MEMBER_STATE *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_STATE *) nmg;

			for ( DWORD i=0; i<pNetMsg->dwNumber; ++i )
			{
				GLMSG::SMEMSTATE &sSTATE = pNetMsg->sSTATE[i];
				m_sCLUB.SetMemberState ( sSTATE.dwCHARID, sSTATE.bONLINE, sSTATE.nCHANNEL, sSTATE.nidMAP );
			}

			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_POS:
		{
			GLMSG::SNET_CLUB_MEMBER_POS *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_POS *) nmg;

			for ( DWORD i=0; i<pNetMsg->dwNumber; ++i )
			{
				GLMSG::SMEMPOS &sPOS = pNetMsg->sPOS[i];
				m_sCLUB.SetMemberPos ( sPOS.dwCHARID, D3DXVECTOR3(sPOS.vecPOS.x,0,sPOS.vecPOS.y) );
			}

			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_DISSOLUTION_FB:
		{
			GLMSG::SNET_CLUB_DISSOLUTION_FB *pNetMsg = (GLMSG::SNET_CLUB_DISSOLUTION_FB *) nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_DIS_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_FAIL") );
				break;
			case EMCLUB_DIS_FB_OK:
				m_sCLUB.DoDissolution ( pNetMsg->tDissolution );
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_OK"), GLCONST_CHAR::dwCLUB_DISSOLUTION_DAY );
				break;
			case EMCLUB_DIS_FB_CANCEL:
				m_sCLUB.DoDissolutionCancel ();
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_CANCEL") );
				break;
			case EMCLUB_DIS_FB_CLUBBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_CLUBBATTLE") );
				break;
			};

			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT:
		{
			GLMSG::SNET_CLUB_MEMBER_DEL_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_DEL_2CLT *) nmg;

			if ( m_sCLUB.m_dwID == pNetMsg->dwClub )
			{
				m_sCLUB.DelMember ( pNetMsg->dwMember );
			}

			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB:
		{
			GLMSG::SNET_CLUB_MEMBER_DEL_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_DEL_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMCLUB_MEMBER_DEL_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_FAIL") );
				break;
			case EMCLUB_MEMBER_DEL_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_OK") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB:
		{
			GLMSG::SNET_CLUB_MEMBER_SECEDE_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_SECEDE_FB *) nmg;
			switch ( pNetMsg->emFB )
			{
			case EMCLUB_MEMBER_SECEDE_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_FAIL") );
				break;
			case EMCLUB_MEMBER_SECEDE_FB_OK:
				{
					m_dwGuild = CLUB_NULL;
					m_sCLUB.RESET();

					CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
					CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();

					CInnerInterface::GetInstance().PrintConsoleTextDlg ( ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_OK"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK:
		{
			CInnerInterface::GetInstance().DOMODAL_CLUB_AUTHORITY_ASK( m_sCLUB.m_szName );		
		}
		break;
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB:
		{
			GLMSG::SNET_CLUB_AUTHORITY_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_REQ_FB *)nmg;

			GLCLUBMEMBER* pMember = m_sCLUB.GetMember( pNetMsg->dwCharID );
			if ( !pMember ) break;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_AUTHORITY_REQ_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_FAIL") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_OK"), pMember->m_szName, m_sCLUB.m_szName );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_REFUSE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_REFUSE") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_NOMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOMASTER") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_NOONLINE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOONLINE") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_ALLIANCE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_ALLIANCE") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_CONFING:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CONFING") );
				break;
			case EMCLUB_AUTHORITY_REQ_FB_TIME:
				{
					CTime cTime(pNetMsg->tAuthority);
					CString strAuthorityDate;

					strAuthorityDate = CInnerInterface::GetInstance().MakeString ( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
																				cTime.GetYear(), 
																				cTime.GetMonth(), 
																				cTime.GetDay(), 
																				cTime.GetHour(), 
																				cTime.GetMinute() );
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_TIME"), strAuthorityDate );
				}
				break;
			case EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE") );
				break;
			}
		}
		break;
	
	case NET_MSG_GCTRL_CLUB_AUTHORITY_CLT:
		{
			GLMSG::SNET_CLUB_AUTHORITY_CLT *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_CLT *)nmg;
			
			GLCLUBMEMBER* pMember = m_sCLUB.GetMember( pNetMsg->dwCharID );
			if ( !pMember ) break;

			m_sCLUB.m_dwMasterID = pNetMsg->dwCharID;
			StringCchCopy( m_sCLUB.m_szMasterName, CHAR_SZNAME, pMember->m_szName );
			
			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
			CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_AUTHORITY_CLT"), pMember->m_szName );
		}
		break;

	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB:
		{
			GLMSG::SNET_CLUB_CD_CERTIFY_FB *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCDCERTIFY_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_FAIL") );
				break;
			case EMCDCERTIFY_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_OK") );

				m_bCLUB_CERTIFY = TRUE;
				m_fCLUB_CERTIFY_TIMER = 0.0f;
				CInnerInterface::GetInstance().SET_CONFT_CONFIRM_VISIBLE ( true );
				break;
			case EMCDCERTIFY_BADNPC:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_BADNPC") );
				break;
			case EMCDCERTIFY_NOTMASTER:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTMASTER") );
				break;
			case EMCDCERTIFY_AREADYCERTIFY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_AREADYCERTIFY") );
				break;
			case EMCDCERTIFY_HAVEGUIDCLUB:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_HAVEGUIDCLUB") );
				break;
			case EMCDCERTIFY_OTHERCERTIFING:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_OTHERCERTIFING") );
				break;
			case EMCDCERTIFY_AREADYGUID:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_AREADYGUID") );
				break;
			case EMCDCERTIFY_NOTTIME:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTTIME") );
				break;
			case EMCDCERTIFY_DISTANCE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_DISTANCE") );
				break;
			case EMCDCERTIFY_NOTITEM:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTITEM") );
				break;
			case EMCDCERTIFY_NOTCONDITION:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTCONDITION") );
				break;

			case EMCDCERTIFY_AREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_AREADY") );
				break;

			case EMCDCERTIFY_ING_TIMEOUT:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_TIMEOUT") );

				m_bCLUB_CERTIFY = FALSE;
				CInnerInterface::GetInstance().SET_CONFT_CONFIRM_VISIBLE ( false );
				break;
			case EMCDCERTIFY_ING_DISTANCE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_DISTANCE") );

				m_bCLUB_CERTIFY = FALSE;
				CInnerInterface::GetInstance().SET_CONFT_CONFIRM_VISIBLE ( false );
				break;
			case EMCDCERTIFY_ING_DIE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_DIE") );

				m_bCLUB_CERTIFY = FALSE;
				CInnerInterface::GetInstance().SET_CONFT_CONFIRM_VISIBLE ( false );
				break;

			case EMCDCERTIFY_COMPLETE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_COMPLETE") );

				m_bCLUB_CERTIFY = FALSE;
				CInnerInterface::GetInstance().SET_CONFT_CONFIRM_VISIBLE ( false );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_COMMISSION_FB:
		{
			GLMSG::SNET_CLUB_GUID_COMMISSION_FB *pNetMsg = (GLMSG::SNET_CLUB_GUID_COMMISSION_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMGUIDCOMMISSION_FB_FAIL:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_FAIL") );
				break;
			case EMGUIDCOMMISSION_FB_OK:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_OK"), pNetMsg->fCommission );
				break;
			case EMGUIDCOMMISSION_FB_RANGE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_RANGE") );
				break;
			case EMGUIDCOMMISSION_FB_NOTCLUB:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_NOTCLUB") );
				break;
			case EMGUIDCOMMISSION_FB_ALREADY:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_ALREADY") );
				break;
			case EMGUIDCOMMISSION_FB_BATTLE:
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_BATTLE") );
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_RESET:
		{
			GLMSG::SNET_CLUB_STORAGE_RESET *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_RESET *)nmg;

			m_sCLUB.RESET_STORAGE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY:
		{
			GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY *)nmg;

			m_sCLUB.m_bVALID_STORAGE = TRUE;
			m_sCLUB.m_lnStorageMoney = pNetMsg->lnMoney;
		}
		break;

	case NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM:
		{
			GLMSG::SNET_CLUB_STORAGE_INSERT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_INSERT *)nmg;

			m_sCLUB.m_bVALID_STORAGE = TRUE;
			m_sCLUB.m_cStorage[pNetMsg->dwChannel].InsertItem ( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY );
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_INSERT:
		{
			GLMSG::SNET_CLUB_STORAGE_INSERT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_INSERT *)nmg;

			const SINVENITEM &sInvenItem = pNetMsg->Data;
			const DWORD dwChannel = pNetMsg->dwChannel;

			m_sCLUB.m_cStorage[dwChannel].InsertItem ( sInvenItem.sItemCustom, sInvenItem.wPosX, sInvenItem.wPosY );

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = sInvenItem.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_DELETE:
		{
			GLMSG::SNET_CLUB_STORAGE_DELETE *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DELETE *)nmg;

			const DWORD dwChannel = pNetMsg->dwChannel;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
			m_sCLUB.m_cStorage[dwChannel].DeleteItem ( pNetMsg->wPosX, pNetMsg->wPosY );
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS:
		{
			GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *)nmg;

			DWORD dwChannel = pNetMsg->dwChannel;
/*
			// PET
			// 팻카드 정보를 제거해준다.
			SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
			if ( pInvenItem )
			{
				PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
				if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
			}
*/
			m_sCLUB.m_cStorage[dwChannel].DeleteItem ( pNetMsg->wDelX, pNetMsg->wDelY );
			m_sCLUB.m_cStorage[dwChannel].InsertItem ( pNetMsg->sInsert.sItemCustom, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY );

			// PET
			// 팻카드 정보를 요청한다.
			SITEM* pItem = GLItemMan::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
			if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.dwPetID != 0 )
			{
				GLMSG::SNETPET_REQ_PETCARDINFO NetMsg;
				NetMsg.dwPetID = pNetMsg->sInsert.sItemCustom.dwPetID;
				NETSENDTOFIELD ( &NetMsg );
			}
		}
		break;

	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM:
		{
			GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *)nmg;

			DWORD dwChannel = pNetMsg->dwChannel;

			SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem ( pNetMsg->wPosX, pNetMsg->wPosY );
			if ( pInvenItem )
			{
				pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
			}
		}
		break;

	case NET_MSG_GCTRL_CLUB_NOTICE_FB:
		{
			GLMSG::SNET_CLUB_NOTICE_FB *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUB_NOTICE_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_FAIL") );
				}
				break;
			case EMCLUB_NOTICE_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK") );
				}
				break;
			case EMCLUB_NOTICE_FB_NOTMATER:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_NOTMATER") );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_NOTICE_CLT:
		{
			GLMSG::SNET_CLUB_NOTICE_CLT *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_CLT *)nmg;
			StringCchCopy ( m_sCLUB.m_szNotice, EMCLUB_NOTICE_LEN+1, pNetMsg->szNotice );

			//	Note : 채팅창에 공지 출력.
			CInnerInterface::GetInstance().PrintMsgDlg ( NS_UITEXTCOLOR::GUILD, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK") );
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::GUILD, ID2GAMEINTEXT("CLUB_NOTICE"), m_sCLUB.m_szName );
			CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::GUILD, "%s", m_sCLUB.m_szNotice );

			//	Note : 클럽창 갱신.
			CInnerInterface::GetInstance().REFRESH_CLUB_NOTICE();
		}
		break;

	case NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT:
		{
			GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *)nmg;

			GLCLUBMEMBER* pCLUBMember = m_sCLUB.GetMember( pNetMsg->dwCharID );
			GASSERT ( pCLUBMember );
			if ( pCLUBMember )
			{
				StringCchCopy ( pCLUBMember->m_szName, CHAR_SZNAME, pNetMsg->szName );
			}
			if ( m_sCLUB.IsMaster ( pNetMsg->dwCharID ) )
			{
				StringCchCopy ( m_sCLUB.m_szMasterName, CHAR_SZNAME, pNetMsg->szName );
			}

			// 인터페이스 갱신
			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	case NET_MSG_GCTRL_CLUB_SUBMASTER_FB:
		{
			GLMSG::SNET_CLUB_SUBMASTER_FB *pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_FB *)nmg;

			switch ( pNetMsg->emFB )
			{
			case EMCLUBSUBMASTER_FB_FAIL:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_FAIL") );
				}
				break;
			case EMCLUBSUBMASTER_FB_OK:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_OK") );
				}
				break;

			case EMCLUBSUBMASTER_FB_RESET:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_RESET") );
				}
				break;

			case EMCLUBSUBMASTER_FB_NOTMASTER:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_NOTMASTER") );
				}
				break;
			case EMCLUBSUBMASTER_FB_BATTLE:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_BATTLE") );
				}
				break;
			case EMCLUBSUBMASTER_FB_CDM_MEM:
				{
					CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_CDM_MEM") );
				}
				break;
			};
		}
		break;

	case NET_MSG_GCTRL_CLUB_SUBMASTER_BRD:
		{
			GLMSG::SNET_CLUB_SUBMASTER_BRD *pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_BRD *)nmg;
			m_sCLUB.SetMemberFlag ( pNetMsg->dwCharID, pNetMsg->dwFlags );
			m_sCLUB.m_dwCDCertifior = pNetMsg->dwCDCertifior;

			CInnerInterface::GetInstance().REFRESH_CLUB_LIST ();
			CInnerInterface::GetInstance().REFRESH_CLUB_STATE ();
		}
		break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess() 분류되지 않은 메시지 수신. TYPE[%d]", nmg->nType );
		break;
	};
}

