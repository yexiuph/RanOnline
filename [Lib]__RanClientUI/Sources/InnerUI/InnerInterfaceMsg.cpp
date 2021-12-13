#include "pch.h"
#include "AdminMessageDisplay.h"
#include "BasicChat.h"
#include "BasicChatRightBody.h"
#include "BasicInfoView.h"
#include "BasicSkillTray.h"
#include "BlockProgramAlarm.h"
#include "ClubWindow.h"
#include "ConftModalWindow.h"
#include "DxViewPort.h"
#include "FriendWindow.h"
#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "HeadChatDisplayMan.h"
#include "InnerInterface.h"
#include "InventoryPageWear.h"
#include "InventoryPageWearEx.h"
#include "ItemSlot.h"
#include "MiniMap.h"
#include "ModalWindow.h"
#include "PartyModalWindow.h"
#include "PrivateMarketSellWindow.h"
#include "PrivateMarketShowMan.h"
#include "RANPARAM.h"
#include "RebirthDialogue.h"
#include "SkillWindowToTray.h"
#include "SubMasterSet.h"
#include "UILeftTopGroup.h"
#include "UITextControl.h"
#include "WaitServerDialogue.h"
#include "RanFilter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CInnerInterface::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{	
	BOOL bMOUSEIN_LBUP = FALSE;
	if ( CHECK_MOUSE_IN ( dwMsg ) )
	{
		switch ( ControlID )
		{
		case ITEM_MOVE:
		case ADMIN_MESSAGE_DISPLAY:
		case DAMAGE_MAN:
		case MAPMOVE_DISPLAY:
		case CONFT_DISPLAY_MAN:
		case HEADCHAT_MAN:
		case NAME_DISPLAY_MAN:		
		case LEFTTOP_CONTROL_GROUP:
		case SKILL_TIME_DISPLAY:
		case ACADEMY_CONFT_DISPLAY:		
		case SIMPLE_HP:
		case SIMPLE_MESSAGE_MAN:		
		case QUESTION_ITEM_DISPLAY:
		case CONFT_CONFIRM:
		case WARNING_MSG_WINDOW:
		case QUEST_HELPER_DISPLAY:
		case ITEM_SHOP_ICON_MAN:
		case CDM_RANKING_DISPLAY:
			break;

		case MINIMAP:
			{
				if ( UIMSG_MOUSEIN_FULLSCREENBUTTON & dwMsg )
				{
					SetCharMoveBlock ();
				}
			}
			break;

		case WAITSERVER_DISPLAY:
			{
				int nTYPE = m_pWaitServerDisplay->GetActionTYPE();
				if ( nTYPE == WAITSERVER_TOFRIEND )
				{
					SetCharMoveBlock ();
				}

				if ( UIMSG_MOUSEIN_OKBUTTON & dwMsg )
				{
					SetCharMoveBlock ();
				}
			}
			break;			

		case BLOCK_PROGRAM_ALARM:
			{
				if ( UIMSG_MOUSEIN_ALARMBUTTON & dwMsg )
				{
					SetCharMoveBlock ();
				}
			}
			break;

		case BASIC_CHAT_BOX:
			{
				if ( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE & dwMsg )
				{
					SetCharMoveBlock ();
				}

				if ( RANPARAM::bMOVABLE_ON_CHAT )
				{
					//	m_bFirstControl이 아니라면,
					//	키가 눌러져도, TranslateMessage.. 즉 이 함수 안으로
					//	들어오지 않는다.
					//	만약, 채팅박스위에서 좌클릭을 할 경우,
					//	m_bFirstControl은 채팅박스이므로,
					//	아래의 VisibleTest를 하는 컨트롤들은 아무 상관이 없게된다.
					//	이럴경우, 캐릭터가 아래의 컨트롤들이 열려져있음에도
					//	뛰어다니게 된다.
					if( IsOpenWindowToMoveBlock() )
					{
						SetCharMoveBlock ();
					}
				}
				else
				{
					SetCharMoveBlock ();
				}
			}
			break;

		default:
			{
				SetCharMoveBlock ();
				SetFirstItemSlot ();
			}
		}

		if ( CHECK_LB_UP_LIKE ( dwMsg ) )
		{
			bMOUSEIN_LBUP = TRUE;
		}
	}

	if ( MODAL_WINDOW <= ControlID ) //	NOTE :모달처리
	{
		// MEMO : 아래 함수는 다중 모달 때문에 넣은 것이다. 일단 빼자...
		//CModalWindow* pModalWindow = (CModalWindow*)FindControl( ControlID );
		if ( m_pModalWindow )
		{
			UIGUID CallerID = m_pModalWindow->GetCallerID ();
			ModalMsgProcess ( CallerID, dwMsg );

			if ( !m_pModalWindow->IsUseModaless () )
				SetCharMoveBlock ();
		}
		
		return ;
	}
	
	switch ( ControlID )
	{
	case LEFTTOP_CONTROL_GROUP:
		{
			if ( UIMSG_MOUSE_IN & dwMsg )
			{
				SetCharMoveBlock ();
			}
		}
		break;

	case QUICK_SKILL_TRAY_OPEN_BUTTON:
		{
			if ( bMOUSEIN_LBUP )
			{				
				HideGroup ( ControlID );
				ShowGroupBottom ( QUICK_SKILL_TRAY_TAB_WINDOW, true );
			}
		}
		break;	

	case GAME_MENU_OPEN_BUTTON:
		{
			if ( bMOUSEIN_LBUP )
			{
				HideGroup ( ControlID );
				ShowGroupBottom ( GAME_MENU, true );
			}
		}
		break;

	case MINIPARTY_OPEN:
		{
			if ( bMOUSEIN_LBUP )
			{
				HideGroup ( ControlID );
				ShowGroupFocus ( MINIPARTY_WINDOW );
			}
		}
		break;

	case QUEST_ALARM:
		{
			if ( bMOUSEIN_LBUP )
			{
				HideGroup ( ControlID );

//				#ifdef CH_PARAM // 중국 인터페이스 변경
//					UpdatePotionTrayPosition();
//				#endif

				if ( NATIVEID_NULL().dwID == m_dwEventQuestID )
				{
					ShowGroupFocus ( QUEST_WINDOW );
					REFRESH_QUEST_WINDOW ();					
				}
				else
				{
					SetQuestWindowOpen ( m_dwEventQuestID );
				}
			}
		}
		break;

	case QUICK_SKILL_TRAY_TAB_WINDOW:
		{
			CSkillWindowToTray* pSkillWindowToTray = GetSkillWindowToTray ();
			if ( pSkillWindowToTray )
			{
				pSkillWindowToTray->SetUseSnap ( (dwMsg & UIMSG_MOUSEIN_SKILLSLOT) ? TRUE : FALSE );
			}
		}
		break;

	case TRADE_WINDOW:
	case TRADEINVENTORY_WINDOW:
	case MARKET_WINDOW:	
	case STORAGE_WINDOW:
	case CLUB_STORAGE_WINDOW:
	case ITEMBANK_WINDOW:
		{			
			BOOL bSnap = (dwMsg & UIMSG_MOUSEIN_WEARSLOT) || (dwMsg & UIMSG_MOUSEIN_ITEMSLOT);
			if ( bSnap ) SetSnapItem ();

			SetCharMoveBlock ();
		}
		break;
        
	case VEHICLE_WINDOW:
	case PET_WINDOW:
	case INVENTORY_WINDOW:
		{
			BOOL bSnap = (dwMsg & UIMSG_MOUSEIN_WEARSLOT) || 
						 (dwMsg & UIMSG_MOUSEIN_WEARSLOTEX) ||
						 (dwMsg & UIMSG_MOUSEIN_ITEMSLOT);
			if ( bSnap ) SetSnapItem ();
		}
		break;

	case DIALOGUE_WINDOW:
		{
			SetCharMoveBlock ();
		}
		break;

	case CONFT_MODAL_WINDOW:
		{			
			if ( m_pConftModalWindow )
			{
				UIGUID CallerID = m_pConftModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
//			SetCharMoveBlock ();
		}
		break;

	case PARTY_MODAL_WINDOW:
		{			
			if ( m_pPartyModalWindow )
			{
				UIGUID CallerID = m_pPartyModalWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
//			SetCharMoveBlock ();
		}
		break;
	
		/*
	case PET_REBIRTH_DIALOGUE: //Monster7j
		{
			if(m_pPetRebirthDialogue)
			{

			}
		}
		break;
		*/
	case REBIRTH_DIALOGUE:
		{
			if ( m_pRebirthDialogue )
			{
				const UIGUID cID = m_pRebirthDialogue->GetCallerID ();
				ModalMsgProcess ( cID, dwMsg );
			}

			SetCharMoveBlock ();
		}
		break;

	case PRIVATE_MARKET_SELL_WINDOW:
		{
			if ( m_pPrivateMarketSellWindow )
			{
				const UIGUID& CallerID = m_pPrivateMarketSellWindow->GetCallerID ();
				ModalMsgProcess ( CallerID, dwMsg );
			}
		}
		break;

	case PRIVATE_MARKET_SHOW_MAN:
		{
			if ( UIMSG_MOUSEIN_MARKET_ADVERTISE_LBDNUP & dwMsg )
			{
				SetCharMoveBlock ();
			}
		}
		break;

	case CLUB_WINDOW:
		{
			if( UIMSG_MOUSEIN_SUBMASTER_MAKE & dwMsg )
			{
				if( !IsVisibleGroup( SUBMASTER_MAKE ) )
				{
					if( m_pSubMasterSet )
					{
						// 현재 선택된 인덱스의 클럽멤버의 이름을 얻어와서 서브마스터창에 넘긴다.
						DWORD dwCharID = m_pClubWindow->GetClubMemberID();
						CString strName = m_pClubWindow->GetClubMemberName();
						m_pSubMasterSet->SetSubMasterName( strName );
						m_pSubMasterSet->LoadCurrentOption( dwCharID );
						ShowGroupFocus( SUBMASTER_MAKE, true );
					}
				}
			}
			else if ( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW & dwMsg )
			{
				GetChat()->BEGIN_PRIVATE_CHAT ( m_pClubWindow->GetClubMemberName() );
				ShowGroupFocus ( BASIC_CHAT_BOX );
			}
		}
		break;

	case FRIEND_WINDOW:
		{
			if( UIMSG_ADDNAME_TO_CHATBOX & dwMsg )
			{
				GetChat()->BEGIN_PRIVATE_CHAT ( m_pFriendWindow->GET_FRIEND_NAME () );
				m_pChat->REFRESH_FRIEND_LIST(); // 채팅창의 친구리스트를 새로 갱신한다.
				ShowGroupFocus ( BASIC_CHAT_BOX );
			}
			else if( UIMSG_EDIT_END & dwMsg )
			{
				m_pFriendWindow->EDIT_END ();
			}
			else if( UIMSG_RECEIVE_NOTE_VISIBLE & dwMsg )
			{
				if( !IsVisibleGroup( RECEIVE_NOTE_WINDOW ) )
				{
					//if( m_pReceiveNoteWindow )
					//{
						ShowGroupFocus( RECEIVE_NOTE_WINDOW, true );
					//}
				}
			}
			else if( UIMSG_WRITE_NOTE_VISIBLE & dwMsg )
			{
#if defined(KRT_PARAM)
				if( !IsVisibleGroup( SMS_SEND_WINDOW ) )
				{
					ShowGroupFocus( SMS_SEND_WINDOW, true );
				}
#else
				if( !IsVisibleGroup( WRITE_NOTE_WINDOW ) )
				{
					ShowGroupFocus( WRITE_NOTE_WINDOW, true );
				}
#endif
			}
		}
		break;

	case BASIC_INFO_VIEW:
		{
			if( UIMSG_BASICINFOVIEW_LBDUP & dwMsg )
			{
				BasicInfoViewDoubleClick();
			}
		}
		break;
	}
}

void	CInnerInterface::DisplayChatMessage ( int nType, const char *szName, const char *szMsg )
{
	if ( m_pChat )
	{
		CString strCombine;
		CString strTemp = szMsg;

		if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
			strTemp = CRanFilter::GetInstance().GetProverb();

		
		switch ( nType )
		{
		case CHAT_TYPE_GLOBAL:		// 관리용 글로벌 메시지
			{
				strCombine.Format ( "[%s]:%s", ID2GAMEWORD("ADMIN_NAME"), strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_SYSTEM );
			}
			break;

		case CHAT_TYPE_NORMAL:		// 일반적 채팅 메시지
			{
				strCombine.Format ( "[%s]:%s", szName, strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_NORMAL );
			}
			break;

		case CHAT_TYPE_PARTY:		// 파티원에게 전달되는 메시지
			{
				strCombine.Format ( "[%s]:%s", szName, strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_PARTY );
			}
			break;

		case CHAT_TYPE_PRIVATE:		// 개인적인 메시지, 귓말, 속삭임
			{
				m_pChat->AddChat ( szName, strTemp, CHAT_PRIVATE );
			}
			break;

        case CHAT_TYPE_GUILD:		// 길드원에게 전달되는 메시지
			{
				strCombine.Format ( "[%s]:%s", szName, strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_GUILD );
			}
			break;

		case CHAT_TYPE_LOUDSPEAKER:
			{
				strCombine.Format ( "[%s]:%s", szName, strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_TOALL );
			}
			break;

		case CHAT_TYPE_ALLIANCE:
			{
				strCombine.Format ( "[%s]:%s", szName, strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_ALLIANCE );
			}
			break;
		
		case CHAT_TYPE_CTRL_GLOBAL:
			{
				strCombine.Format ( "[%s]:%s", ID2GAMEWORD("NOTIFY_NAME"), strTemp );	
				m_pChat->AddStringToChatEx ( strCombine, CHAT_SYSTEM );
			}
			break;

		default:
			GASSERT( FALSE );
			break;
		};

		D3DCOLOR dwIDColor = NS_UITEXTCOLOR::IDCOLOR;
		switch ( nType )
		{
		case CHAT_TYPE_GLOBAL:
			{
				if ( m_pAdminMessageDisplay )
				{
					if ( !IsVisibleGroup ( ADMIN_MESSAGE_DISPLAY ) )
						strCombine.Format ( "[%s]%s", ID2GAMEWORD("ADMIN_NAME"), strTemp );	
					else
						strCombine.Format ( "%s", strTemp );	

					m_pAdminMessageDisplay->AddText ( strCombine, NS_UITEXTCOLOR::ADMIN_COLOR );

					// 개행 기능을 위해 AddText를 여러번 한다.
					/*int i, nSize = strlen(szMsg);
					for( i = 0; i < nSize; i++ )
					{
						if( szMsg[i] == '\r' ) continue;
						if( szMsg[i] == '\n' )
						{
							if( strCombine.GetLength() != 0 )
								m_pAdminMessageDisplay->AddText ( strCombine, NS_UITEXTCOLOR::ADMIN_COLOR );
							strCombine = "";
						}else{
							strCombine += szMsg[i];
						}
					}*/
					//m_pAdminMessageDisplay->AddText ( strCombine, NS_UITEXTCOLOR::ADMIN_COLOR );
					
					ShowGroupTop ( ADMIN_MESSAGE_DISPLAY );					
				}
			}
			break;

		case CHAT_TYPE_NORMAL:
		case CHAT_TYPE_PARTY:
		case CHAT_TYPE_GUILD:
		case CHAT_TYPE_ALLIANCE:
//		case CHAT_TYPE_PRIVATE:		
			{
				if ( m_pHeadChatDisplayMan )
				{
					D3DXVECTOR3 *pPos = GLGaeaClient::GetInstance().FindCharHeadPos( szName );
					if ( pPos )
					{
						D3DXVECTOR3 vPos = *pPos;
						D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

						int nPosX = (int)( vScreen.x);
						int nPosY = (int)( vScreen.y);

						DWORD dwIDColor = NS_UITEXTCOLOR::IDCOLOR;

						////	이름 색 (PK_COLOR)
						//PGLCHARCLIENT pCHAR = GLGaeaClient::GetInstance().GetChar ( sTargetID.dwID );
						//if ( !pCHAR ) return ;
						//dwColor = pCHAR->GET_PK_COLOR ();

						if ( IsPartyMember ( szName ) ) dwIDColor = NS_UITEXTCOLOR::GREENYELLOW;
						D3DXVECTOR2 vCharPos = D3DXVECTOR2 ( float(nPosX), float(nPosY ) );
						m_pHeadChatDisplayMan->AddChat ( szName, dwIDColor, strTemp, NS_UITEXTCOLOR::DEFAULT, vCharPos );
					}
				}
			}
			break;

		}
	}
}

//void CInnerInterface::TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg )
//{
//	CUIMan::TranslateActionMsgQ ( ControlID, dwMsg );
//
//	switch ( ControlID )
//	{
//	case FRIEND_WINDOW:
//		{
//			if ( UIMSG_ADDNAME_TO_CHATBOX & dwMsg )
//			{
//				GetChat()->BEGIN_PRIVATE_CHAT ( m_pFriendWindow->GET_FRIEND_NAME () );
//				m_pChat->REFRESH_FRIEND_LIST(); // 채팅창의 친구리스트를 새로 갱신한다.
//				ShowGroupFocus ( BASIC_CHAT_BOX );
//			}
//			else if ( UIMSG_EDIT_END & dwMsg )
//			{
//				m_pFriendWindow->EDIT_END ();
//			}
//		}
//		break;		
//
//	case CLUB_WINDOW:
//		{
//			if ( UIMSG_ADDNAME_TO_CHATBOX_FROM_CLUBWINDOW & dwMsg )
//			{
//				GetChat()->BEGIN_PRIVATE_CHAT ( m_pClubWindow->GetClubMemberName() );
//				ShowGroupFocus ( BASIC_CHAT_BOX );
//			}
//		}
//		break;	
//	}
//
//	if( MODAL_WINDOW <= ControlID ) // MEMO : 아이디가 모달이면...
//	{
//		if( dwMsg & UIMSG_INVISIBLE )
//		{
//			CModalWindow* pModalWindow = (CModalWindow*) FindControl ( ControlID );
//			if ( pModalWindow && pModalWindow->m_bTEST_MULTI )
//			{
//				GetFocusList()->EraseControl ( ControlID );
//				DeleteControl ( ControlID );
//			}
//		}
//	}
//}