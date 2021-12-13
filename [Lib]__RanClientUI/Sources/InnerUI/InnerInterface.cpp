#include "pch.h"
#include "AdminMessageDisplay.h"
#include "BasicChat.h"
#include "BasicInfoView.h"
#include "BasicQuickSkillSlot.h"
#include "BasicVarTextBox.h"
#include "BlockProgramAlarm.h"
#include "CommonWeb.h"
#include "DxGlobalStage.h"
#include "DxInputString.h"
#include "GameTextControl.h"
#include "GLGaeaClient.h"
#include "GLPartyClient.h"
#include "InnerInterface.h"
#include "InventoryWindow.h"
#include "ItemShopWindowWeb.h"
#include "MapMoveDisplay.h"
#include "ModalCallerID.h"
#include "ModalWindow.h"
#include "NameDisplayMan.h"
#include "PrivateMarketWindow.h"
#include "QuestAlarm.h"
#include "QuestWindow.h"
#include "RANPARAM.h"
#include "RebirthDialogue.h"
#include "ShopItemSearchWindow.h"
#include "ItemSearchResultWindow.h"
#include "MapRequireCheck.h"
#include "SkillTrayTab.h"
#include "SystemMessageWindow.h"
#include "UILeftTopGroup.h"
#include "UITextControl.h"
#include "s_CHttpPatch.h"
#include "ItemMixWindow.h"
#include "GatherGauge.h"
#include "CdmRankingDisplay.h"

#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHAT_MACRO_SIZE	10

CInnerInterface& CInnerInterface::GetInstance()
{
	static CInnerInterface Instance;
	return Instance;
}

const float	CInnerInterface::fMENU_LIFTUP = 90.0f;
const int CInnerInterface::nOUTOFRANGE = -1;

CInnerInterface::CInnerInterface () :
	m_emConflictReqType ( EMCONFT_ONE ),
	m_ConflictReqID ( GAEAID_NULL ),
	m_bPartyStateBack ( FALSE ),
	m_bBlockProgramFound ( false ),
	m_dwEventQuestID ( NATIVEID_NULL().dwID ),
	m_dwEventQuestStep ( UINT_MAX ),
	m_bBlockProgramAlarm ( false ),
	m_bITEM_INFO_EX_DISPLAY_MODE ( false ),
	m_bCHANNEL ( false ),
	m_bFirstVNGainSysCall ( true ),
	m_bTabReserve ( false ),
	m_bItemShopLoad ( false )
{
	ResetControl();

	
}

CInnerInterface::~CInnerInterface ()
{
}

void CInnerInterface::ResetControl()
{
	m_pSimpleHP = NULL;
	m_pBusWindow = NULL;
	m_pMiniMap = NULL;
	m_pChat = NULL;
	m_pQuestWindow = NULL;
	m_pQuestAlarm = NULL;	
	m_pFriendWindow = NULL;	
	m_pSkillWindowToTray = NULL;	
	m_pInventoryWindow = NULL;
	m_pPartyWindow = NULL;
	m_pTradeWindow = NULL;	
	m_pPtoPWindow = NULL;
	m_pDamageDisplayMan = NULL;
	m_pTargetInfoDisplay = NULL;
	m_pNameDisplayMan = NULL;
	m_pDialogueWindow = NULL;
	m_pMarketWindow = NULL;
	m_pStorageWindow = NULL;
	m_pItemMove = NULL;
	m_pBlockProgramAlarm = NULL;
	m_pItemBankWindow = NULL;
	m_pVNGainSysInventory = NULL;
	m_pTradeInventoryWindow = NULL;
	m_pHeadChatDisplayMan = NULL;	
	m_pAdminMessageDisplay = NULL;	
	m_pConftModalWindow = NULL;	
	m_pPartyModalWindow = NULL;
	m_pConftDisplayMan = NULL;
	m_pInfoDisplay = NULL;
	m_pMapMoveDisplay = NULL;
	m_pWaitServerDisplay = NULL;
	m_pAcademyConftDisplay = NULL;
	m_pLargeMapWindow = NULL;
	m_pStorageChargeCard = NULL;
	m_pRebirthDialogue = NULL;
	m_pPrivateMarketWindow = NULL;
	m_pPrivateMarketMake = NULL;
	m_pPrivateMarketSellWindow = NULL;
	m_pPrivateMarketShowMan = NULL;
	m_pClubWindow = NULL;
	m_pSubMasterSet = NULL;
	m_pClubMake = NULL;
	m_pSystemMessageWindow = NULL;
	m_pQuestionItemDisplay = NULL;
	m_pInfoDisplayEx = NULL;
	m_pClubStorageWindow = NULL;
	m_pConftConfirm = NULL;
	m_pBasicInfoView = NULL;
	m_pUILeftTopGroup = NULL;
	m_pSkillTrayTab = NULL;
	m_pBasicQuickSkillSlot = NULL;
	m_pCountMsgWindow = NULL;
	m_pGameMenu = NULL;
	m_pQBoxButton = NULL;
	m_pItemRebuildWindow = NULL;	// ITEMREBUILD_MARK
	//m_pRebuildInventoryWindow = NULL;
	m_pPetRebirthDialogue = NULL; // monster7j
	m_pGambleBox = NULL;
	m_pGambleSelectBox = NULL;
	m_pGambleAgainBox = NULL;
	m_pGambleResultOddBox = NULL;
	m_pGambleResultEvenBox = NULL;
	m_pPetWindow = NULL;
	m_pCharacterWindow = NULL;

	m_pModalWindow = NULL;

	m_pSystemMessageWindowDummy = NULL;
	m_pMapMoveDisplayDummy = NULL;
	m_pBlockProgramAlarmDummy = NULL;
	m_pLeftTopGroupDummy = NULL;
	m_pBasicInfoViewDummy = NULL;
	m_pBasicPotionTrayDummy = NULL;
	m_pQuestAlarmDummy = NULL;

	m_pKeySettingWindow = NULL;
	m_pChatMacroWindow = NULL;

	m_pBonusTimeGauge = NULL;
	m_pBonusTimeDisplay = NULL;
	m_pQuestHelper = NULL;
	m_pVehicleWindow = NULL;
	m_pThaiCCafeMark = NULL;

	m_pItemGarbageWindow = NULL;
	m_pGarbageInventoryWindow = NULL;

	m_pItemShopIconMan = NULL;
	m_pNameDisplayMan = NULL;

	m_pShopItemSearchWindow   = NULL;
	m_pItemSearchResultWindow = NULL;

	m_pAttendanceBookWindow = NULL;
	m_pClubBattleModalWindow = NULL;

	m_pMapRequireCheckWindow = NULL;
	m_pTaxiWindow = NULL;

	m_bFirstVNGainSysCall = true;	

	m_fItemBankDelay = 5.0f;

	m_pItemMixWindow = NULL;
	m_pItemMixInvenWindow = NULL;
	m_pGatherGauge = NULL;
	m_pCdmRankingDisplay = NULL;
}

HRESULT CInnerInterface::OneTimeSceneInit ()
{
	switch ( RANPARAM::emSERVICE_TYPE )
	{
	case EMSERVICE_DEFAULT:
	case EMSERVICE_KOREA:
	case EMSERVICE_FEYA:
	case EMSERVICE_MALAYSIA_CN:
	case EMSERVICE_MALAYSIA_EN:
	case EMSERVICE_JAPAN:
	case EMSERVICE_THAILAND:
	case EMSERVICE_CHINA:
	case EMSERVICE_INDONESIA:
	case EMSERVICE_PHILIPPINES:
	case EMSERVICE_VIETNAM:
	case EMSERVICE_GLOBAL:
		m_bCHANNEL = true;
		break;

	default:
		m_bCHANNEL = false;
		break;
	};

	return S_OK;
}

HRESULT CInnerInterface::FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	GASSERT( pd3dDevice );

	// 탈것 딜레이 측정
	m_fVehicleDelay += fElapsedTime;

	//	무조건 안 보이게 만듦.
	//	사용시에 Update()에서 켜고 동작.
	HideGroup ( INFO_DISPLAY );
	HideGroup ( INFO_DISPLAY_EX );

	ResetTargetInfo ();		//	타겟 정보창 리셋
	ResetCharMoveBlock ();	//	캐릭터 움직임 잠금 풀기	
	ResetSnapItem ();		//	아이템 스냅 리셋
	ResetFirstItemSlot ();	//	아이템슬롯 붙기 리셋

	m_bUSING_INFO_DISPLAY = false;


	// 아이템 뱅크창 딜레이 측정
	if( !IsVisibleGroup( ITEMBANK_WINDOW ) && m_fItemBankDelay < 5.0f )
	{
		m_fItemBankDelay += fElapsedTime;
	}

	bool bKEYBOARD_BLOCK = false;
	if( IsVisibleGroup ( MODAL_WINDOW ) ||
		IsVisibleGroup ( REBIRTH_DIALOGUE ) ||
		//IsVisibleGroup ( HELP_WINDOW ) ||	// 웹 브라우저용, 아직 사용 안함 ( 준혁 )
		IsVisibleGroup ( ITEMSHOP_WINDOW ) ||
		DXInputString::GetInstance().IsOn () )
	{
		bKEYBOARD_BLOCK = true;
	}

	// WebBrowser가 반투명이 지원되지 않으므로
	// 필요한 상황에서는 HELP_WINDOW를 닫는다 ( 준혁 )
	if( CCommonWeb::Get()->GetCreate() )
	{
		if( IsVisibleGroup ( MODAL_WINDOW ) ||
			IsVisibleGroup ( REBIRTH_DIALOGUE ) ||
			IsVisibleGroup( ESC_MENU ) )
		{
			HideGroup( HELP_WINDOW );
			HideGroup( ITEMSHOP_WINDOW );
		}
	}

	if ( !bKEYBOARD_BLOCK ) UpdateShortcutBefore ();

	HRESULT hr = S_OK;
	hr = CUIMan::FrameMove ( pd3dDevice, fElapsedTime );
	if ( FAILED ( hr ) ) return hr;

	if( IsOpenWindowToMoveBlock() )
	{
		SetCharMoveBlock ();
	}

	UpdateStatus ();

	if ( !m_bUSING_INFO_DISPLAY ) RESET_INFO ();
	if ( !bKEYBOARD_BLOCK )	UpdateShortcutAfter ();

	if ( IsExclusiveControl() ) SetCharMoveBlock ();

	int LB = (int)DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT);
	if ( (LB & DXKEY_UP) || (LB & DXKEY_DUP) )
	{
		ResetExclusiveControl();
	}

	MoveBasicInfoWindow ();

	CUIFocusContainer::UICONTROL_FOCUSLIST focus_list = GetFocusList()->GetFocusList();
	CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = focus_list.rbegin ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = focus_list.rend ();

	bool bBasicChatBoxShow = false;
	for ( ; riter != riter_end; ++riter )
	{
		UIGUID cID = riter->cID;
		if ( BASIC_INFO_VIEW == cID )
		{
			if ( bBasicChatBoxShow )
			{
				if ( !GetChat()->IsCHAT_BEGIN() )
				{
					GetFocusList()->InsertAfter ( BASIC_CHAT_BOX, BASIC_INFO_VIEW, riter->pUIControl );
				}
			}
			break;
		}
		else if ( BASIC_CHAT_BOX == cID )
		{
			bBasicChatBoxShow = true;
		}
	}

	{
		GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
		Actor& actor = pCharacter->GetActorMove ();
		if ( actor.PathIsActive () ) m_vCharDir = actor.Movement ();
	}

	{
		if ( IsVisibleGroup ( INFO_DISPLAY_EX ) )
		{
			UIRECT rcGlobalPos = m_pInfoDisplay->GetGlobalPos();
			UIRECT rcGlobalPosEx = m_pInfoDisplayEx->GetGlobalPos();
			float x = rcGlobalPos.right + 2.0f;
			float y = min(rcGlobalPos.top,rcGlobalPosEx.top);

			const long lResolution = GetResolution ();
			WORD X_RES = HIWORD ( lResolution );
			WORD Y_RES = LOWORD ( lResolution );

			if ( X_RES < x + rcGlobalPosEx.sizeX )
			{
				x = X_RES - rcGlobalPosEx.sizeX;

				m_pInfoDisplay->SetGlobalPos(D3DXVECTOR2(x-2.0f-rcGlobalPos.sizeX,y));
				m_pInfoDisplayEx->SetGlobalPos(D3DXVECTOR2(x,y));
			}
			else
			{
				m_pInfoDisplay->SetGlobalPos(D3DXVECTOR2(rcGlobalPos.left,y));
				m_pInfoDisplayEx->SetGlobalPos(D3DXVECTOR2(x,y));
			}
		}
	}

#ifdef _RELEASED // 소환수 내부에서만 되게 막음
	{
		if( IsVisibleGroup ( SUMMON_POSION_DISPLAY ) )
		{
			GLSummonClient *pMySummon = GLGaeaClient::GetInstance().GetSummonClient();
			if( !pMySummon || !pMySummon->IsVALID() )
			{
				HideGroup ( SUMMON_POSION_DISPLAY );
			}
		}
	}
#endif

	return hr;
}

void CInnerInterface::UpdateShortcutBefore ()
{	
	{
		if ( !m_pNameDisplayMan ) return ;		

		if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
		{
			for (int i= 0; i < CHAT_MACRO_SIZE; ++i){
				if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_1+i, DXKEY_DOWN ) )
				{			
					AddChatMacro(i);
					break;
				}
			}
		}



		if( RANPARAM::bNAME_DISPLAY )
		{
			if( IsVisibleGroup( NAME_DISPLAY_MAN ) )
			{
				if( m_pNameDisplayMan->GetNameType() != NAME_DISPLAY_MOBCHAR )
				{
					HideGroup( NAME_DISPLAY_MAN );
					m_pNameDisplayMan->SetNameType( NAME_DISPLAY_MOBCHAR );
					ShowGroupBottom( NAME_DISPLAY_MAN );
				}
			}
			else
			{
				m_pNameDisplayMan->SetNameType( NAME_DISPLAY_MOBCHAR );
				ShowGroupBottom( NAME_DISPLAY_MAN );
			}

			return ;
		}

		HideGroup ( NAME_DISPLAY_MAN );		
		if ( UIKeyCheck::GetInstance()->Check ( DIK_LMENU, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_ITEMMONEY );
			ShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else if ( UIKeyCheck::GetInstance()->Check ( DIK_LCONTROL, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_MOBCHAR );
			ShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else if ( UIKeyCheck::GetInstance()->Check ( DIK_LSHIFT, DXKEY_PRESSED ) )
		{
			m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_ALL );
			ShowGroupBottom ( NAME_DISPLAY_MAN );
		}
		else
		{
			//	BEGIN : CLUB CONFT SETTING
			const EMCONFT_TYPE emTYPE = GLGaeaClient::GetInstance().GetCharacter()->GetConfting().emTYPE;
			if ( emTYPE == EMCONFT_GUILD )
			{
				m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_CLUB_CONFT );
				ShowGroupBottom ( NAME_DISPLAY_MAN );                
				return ;
			}

			//	파티
			GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	마스터
			if ( pMaster )
			{
				GLCharacter *pMyCharacter = GLGaeaClient::GetInstance().GetCharacter();			

				if ( pMyCharacter->GetConfting().IsCONFRONTING() )	//	파티 대련 중인가?
				{
					m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_PARTY_CONFT );			
				}
				else	//	파티 구성 된 경우
				{
					m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_PARTY );		
				}
				ShowGroupBottom ( NAME_DISPLAY_MAN );
			}
			else
			{
				//	무조건 뿌림
				m_pNameDisplayMan->SetNameType ( NAME_DISPLAY_DEFAULT );
				ShowGroupBottom ( NAME_DISPLAY_MAN );
			}
		}
	}	
}

void CInnerInterface::UpdateShortcutAfter ()
{
	if ( UIKeyCheck::GetInstance()->Check ( DIK_ESCAPE, DXKEY_DOWN ) )
	{
		bool bALLHIDE = true;

		//	NOTE
		//		동시에 두 곳에서 FocusList에
		//		접근할 가능성이 없다는 가정이 있어야 한다.
		//		위험하군 ㅡ.ㅡ
		CUIFocusContainer::UICONTROL_FOCUSLIST list = GetFocusList()->GetFocusList ();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = list.rbegin ();
		CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = list.rend ();
		for ( ; riter != riter_end; ++riter )
		{
			bool bAnotherProcess = false;
			UIGUID cID = riter->cID;

			if ( IsVisibleGroup ( cID ) && 
				cID != BASIC_CHAT_BOX && 
				cID != BASIC_INFO_VIEW &&
				cID != PRIVATE_MARKET_SHOW_MAN && 
				cID != MINIMAP)
			{
				if ( cID == INVENTORY_WINDOW || cID == MARKET_WINDOW ||
					cID == STORAGE_WINDOW || cID == CLUB_STORAGE_WINDOW )
				{
					SetMarketWindowClose ();
					SetStorageWindowClose ();
					SetClubStorageWindowClose ();
				}
				else if ( cID == TRADEINVENTORY_WINDOW )
				{
					if ( IsVisibleGroup ( TRADE_WINDOW ) )
					{
						SetTradeWindowCloseReq ();
					}
					else if ( IsVisibleGroup ( PRIVATE_MARKET_WINDOW ) )
					{
						if ( m_pPrivateMarketWindow->IsOPENER () )
						{
							DoModal ( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), MODAL_QUESTION, YESNO, MODAL_PRIVATE_MARKET_QUIT );
						}
						else
						{
							SetPrivateMarketClose ();
						}
						bAnotherProcess = true;
					}
				}
				else if ( cID == TRADE_WINDOW )
				{							
					SetTradeWindowCloseReq ();
				}
				else if ( cID == MINIPARTY_WINDOW )
				{
					ShowGroupBottom ( MINIPARTY_OPEN );
// #ifdef CH_PARAM // 중국 인터페이스 변경
//					UpdatePotionTrayPosition();
// #endif
				}
				else if ( cID == PRIVATE_MARKET_WINDOW )
				{
					if ( m_pPrivateMarketWindow->IsOPENER () )
					{
						DoModal ( ID2GAMEINTEXT("PRIVATE_MARKET_QUIT"), MODAL_QUESTION, YESNO, MODAL_PRIVATE_MARKET_QUIT );
					}
					else
					{
						SetPrivateMarketClose ();
					}
					bAnotherProcess = true;					
				}
				else if( cID == ITEM_REBUILD_WINDOW || cID == REBUILDINVENTORY_WINDOW )	// ITEMREBUILD_MARK
				{
					CloseItemRebuildWindow();
				}
				else if( cID == ITEM_GARBAGE_WINDOW || cID == GARBAGEINVENTORY_WINDOW )	// 휴지통
				{
					CloseItemGarbageWindow();
				}
				else if( cID == ITEM_MIX_WINDOW || cID == ITEM_MIX_INVEN_WINDOW )	// 아이템 조합
				{
					CloseItemMixWindow();
				}
				else if( cID == GAMBLE_DIALOGUE || cID == GAMBLE_SELECT_DIALOGUE || 
						cID == GAMBLE_AGAIN_DIALOGUE || cID == GAMBLE_ANIMATION_DIALOGUE || 
						cID == GAMBLE_RESULT_EVEN_DIALOGUE || cID == GAMBLE_RESULT_ODD_DIALOGUE  )
				{
					HideGroup( INVENTORY_WINDOW );
					GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
					if( pCharacter )
					{
						pCharacter->ReqMGameOddEvenCancel();
					}
				}
				else if ( cID == QUEST_WINDOW )
				{
					int nPage = m_pQuestWindow->GET_SHOW_PAGE ();
					switch ( nPage )
					{
					case CQuestWindow::QUEST_STEP:
						{							
							m_pQuestWindow->SET_SHOW_PAGE ( CQuestWindow::QUEST_MAIN );
							bAnotherProcess = true;
						}
						break;

					case CQuestWindow::QUEST_MAIN:
						{												
							m_pQuestWindow->SET_SHOW_PAGE ( CQuestWindow::QUEST_LIST );
							bAnotherProcess = true;
						}
						break;
					}					
				}
				else if ( cID == WAITSERVER_DISPLAY || cID == REBIRTH_DIALOGUE || cID == MAP_REQUIRE_CHECK )
				{
					bAnotherProcess = true;
				}
				else if ( (MODAL_WINDOW + 1) <= cID )
				{
					//					CLOSE_MODAL ( cID, false );
					bAnotherProcess = true;
				}
				if ( !bAnotherProcess )	
				{
					HideGroup ( cID );
				}

				bALLHIDE = false;
				break;
			}
		}

		if ( bALLHIDE )
		{
			if ( !IsVisibleGroup ( ESC_MENU ) )
				ShowGroupFocus ( ESC_MENU );
		}
	}

	// Tab Key 예약중이라면
	if ( m_bTabReserve ) 
	{
		if ( !GLGaeaClient::GetInstance().GetCharacter()->IsACTION(GLAT_SKILL) )
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange();
			m_bTabReserve = false;
		}
	}	
	else 
	{
		if ( UIKeyCheck::GetInstance()->Check( DIK_TAB, DXKEY_DOWN ) )
		{
			EMCHARCLASS emClass = GLGaeaClient::GetInstance().GetCharacter()->m_emClass;

			if( emClass&GLCC_EXTREME_M || emClass&GLCC_EXTREME_W )
			{
				// 스킬 사용중으로 실패하면 Tab Key 예약
				if ( E_FAIL == GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange() )
					m_bTabReserve = true;
			}
				
		}
	}



	if ( IsVisibleGroup ( ESC_MENU ) )				return ;
	if ( IsVisibleGroup ( HELP_WINDOW ) )			return ;
	if ( IsVisibleGroup ( ITEMSHOP_WINDOW ) )		return ;
	if ( IsVisibleGroup ( KEY_SETTING_WINDOW ) )	return ;

	if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_V, DXKEY_DOWN ) )
	{
		if ( GLTradeClient::GetInstance().Valid() ) return;
		GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter ();

		if ( pCharacter->m_sVehicle.IsActiveValue() && m_fVehicleDelay < 1.0f )	
		{
			PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_DELAY") );
			return;
		}

		pCharacter->ReqSetVehicle( !pCharacter->m_bVehicle ); // 활성 비활성
		m_fVehicleDelay = 0.0f;
	}


	if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
	{
		for (int i= 0; i < CHAT_MACRO_SIZE; ++i){
			if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_1+i, DXKEY_DOWN ) )
			{			
				return ;
			}
		}
	}

	GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
	if ( pCharacter )
	{
		//	퀵 스킬
		for( int i = 0; i < QUICK_SKILL_SLOT_MAX; ++i)
		{
            if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::SkillSlot[i], DXKEY_DOWN ) )
			{
				INT nIndex = m_pSkillTrayTab->GetTabIndex() * QUICK_SKILL_SLOT_MAX + i;
				pCharacter->ReqSkillRunSet( nIndex );
			}
		}
	}
	
	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_INVEN], DXKEY_DOWN ) )
	{		
		if ( !IsVisibleGroup ( INVENTORY_WINDOW ) && !IsVisibleGroup ( TRADEINVENTORY_WINDOW ) )
		{
			ShowGroupFocus( INVENTORY_WINDOW );
		}
		else
		{
			HideGroup ( INVENTORY_WINDOW );
		}
	}

#ifdef _RELEASED // 소환수 내부에서만 되게 막음
	if ( UIKeyCheck::GetInstance()->CheckSimple( DIK_SLASH, DXKEY_DOWN ) )
	{
		GLGaeaClient::GetInstance().ReqUseSummon(SNATIVEID(2,2));
	}
#endif


	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_SKILL], DXKEY_DOWN ) )
	{		
		if ( !IsVisibleGroup ( SKILL_WINDOW ) )
			ShowGroupFocus ( SKILL_WINDOW );
		else
			HideGroup ( SKILL_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_PARTY], DXKEY_DOWN ) )
	{		
		if ( !IsVisibleGroup ( PARTY_WINDOW ) )
			ShowGroupFocus ( PARTY_WINDOW );
		else
			HideGroup ( PARTY_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_CHAR], DXKEY_DOWN ) )
	{		
		if ( !IsVisibleGroup ( CHARACTER_WINDOW ) )
			ShowGroupFocus ( CHARACTER_WINDOW );
		else
			HideGroup ( CHARACTER_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->CheckSimple ( RANPARAM::MenuShotcut[SHOTCUT_MAP], DXKEY_DOWN ) )
	{
		if ( !IsVisibleGroup ( LARGEMAP_WINDOW ) )
			ShowGroupFocus ( LARGEMAP_WINDOW );
		else
			HideGroup ( LARGEMAP_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->CheckSimple ( RANPARAM::MenuShotcut[SHOTCUT_CLUB], DXKEY_DOWN ) )
	{
		if ( !IsVisibleGroup ( CLUB_WINDOW ) )
			ShowGroupFocus ( CLUB_WINDOW );
		else
			HideGroup ( CLUB_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_QUEST], DXKEY_DOWN ) )
	{
		if ( !IsVisibleGroup ( QUEST_WINDOW ) )
		{			
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
		else
		{
			HideGroup ( QUEST_WINDOW );
		}
	}

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_HELP], DXKEY_DOWN) )
	{
		if ( !IsVisibleGroup ( HELP_WINDOW ) )
		{
			HideGroup ( ITEMSHOP_WINDOW );
			ShowGroupFocus ( HELP_WINDOW );
		}
		else
		{
			HideGroup ( HELP_WINDOW );
		}
	}

#if defined ( CH_PARAM ) || defined ( TH_PARAM ) || defined( PH_PARAM ) || defined ( JP_PARAM ) || defined( _RELEASED ) // 필리핀 아이템샵
	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_ITEMSHOP], DXKEY_DOWN) )
	{
		if ( !IsVisibleGroup ( ITEMSHOP_WINDOW ) )
		{
			//	ItemShopAuth
			if ( ItemShopAuth() )
			{
				ShowGroupFocus ( ITEMSHOP_WINDOW );
			}

			HideGroup ( HELP_WINDOW );
		}
		else
		{
			HideGroup ( ITEMSHOP_WINDOW );
		}
	}
	
	//	ItemShopAuth	
	ItemShopVisible();

#endif	

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_FRIEND], DXKEY_DOWN ) )
	{
		if ( IsVisibleGroup ( FRIEND_WINDOW ) )		HideGroup ( FRIEND_WINDOW );			
		else										ShowGroupFocus ( FRIEND_WINDOW );
	}

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_ITEMBANK], DXKEY_DOWN ) )
	{
		if ( IsVisibleGroup ( ITEMBANK_WINDOW ) )	HideGroup ( ITEMBANK_WINDOW );
		else										SetItemBankWindowOpen ();

		/*if ( !IsVisibleGroup ( ITEMBANK_WINDOW ) )	
		{
			SetItemBankWindowOpen();			
		}*/
	}

	/*if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_VIETNAMGAINSYS], DXKEY_DOWN ) )
	{
		if ( IsVisibleGroup ( VNGAINSYS_WINDOW ) )	HideGroup ( VNGAINSYS_WINDOW );
		else										SetVNGainSysWindowOpen();
	}*/

	if ( UIKeyCheck::GetInstance()->Check ( RANPARAM::MenuShotcut[SHOTCUT_CHATMACRO], DXKEY_DOWN ) )
	{
		if ( IsVisibleGroup ( CHATMACRO_WINDOW ) )
		{
			HideGroup ( CHATMACRO_WINDOW );
		}
		else
		{
			GetChatMacro();
			ShowGroupFocus ( CHATMACRO_WINDOW );
		}
	}



	if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_LMENU, DXKEY_PRESSED ) )
	{
		if ( UIKeyCheck::GetInstance()->CheckSimple ( DIK_F4, DXKEY_DOWN ) )
		{			
			DoModal ( ID2GAMEINTEXT("CLOSE_GAME"), MODAL_QUESTION, OKCANCEL, MODAL_CLOSEGAME );
		}
	}

//#ifndef CH_PARAM // 중국 인터페이스 변경
	if( UIKeyCheck::GetInstance()->Check ( DIK_GRAVE, DXKEY_DOWN ) ||
		UIKeyCheck::GetInstance()->Check ( DIK_Y, DXKEY_DOWN ) )
	{
		if ( IsVisibleGroup ( QUICK_SKILL_TRAY_OPEN_BUTTON ) )
		{
			HideGroup ( QUICK_SKILL_TRAY_OPEN_BUTTON );
			ShowGroupBottom ( QUICK_SKILL_TRAY_TAB_WINDOW );
		}
		else
		{
			HideGroup ( QUICK_SKILL_TRAY_TAB_WINDOW );
			ShowGroupBottom ( QUICK_SKILL_TRAY_OPEN_BUTTON );
		}
	}
//#endif

	if( UIKeyCheck::GetInstance()->Check( DIK_F1, DXKEY_DOWN ) )	
		m_pSkillTrayTab->SetTabIndex( 0 );
	if( UIKeyCheck::GetInstance()->Check( DIK_F2, DXKEY_DOWN ) )	
		m_pSkillTrayTab->SetTabIndex( 1 );
	if( UIKeyCheck::GetInstance()->Check( DIK_F3, DXKEY_DOWN ) )	
		m_pSkillTrayTab->SetTabIndex( 2 );

	if( UIKeyCheck::GetInstance()->Check( RANPARAM::MenuShotcut[SHOTCUT_PET], DXKEY_DOWN ) )
	{
		if( GLGaeaClient::GetInstance().GetPetClient()->IsVALID() )
		{
			if ( GLTradeClient::GetInstance().Valid() ) return;
			
			if ( IsVisibleGroup( PET_WINDOW ) )		HideGroup ( PET_WINDOW );
			else									ShowGroupFocus( PET_WINDOW );

			
		}
	}

#ifdef _RELEASED // 소환수 내부에서만 되게 막음
	if( UIKeyCheck::GetInstance()->Check( RANPARAM::MenuShotcut[SHOTCUT_SUMMON], DXKEY_DOWN ) )
	{
		if( GLGaeaClient::GetInstance().GetSummonClient()->IsVALID() )
		{
			if ( GLTradeClient::GetInstance().Valid() ) return;

			if ( IsVisibleGroup( SUMMON_WINDOW ) )		HideGroup ( SUMMON_WINDOW );
			else									ShowGroupFocus( SUMMON_WINDOW );


		}
	}
#endif

	

	if( UIKeyCheck::GetInstance()->Check( DIK_N, DXKEY_DOWN ) )
	{
		if( GLGaeaClient::GetInstance().GetCharacter()->m_sVehicle.IsActiveValue() )
		{
			if ( GLTradeClient::GetInstance().Valid() ) return;
			
			if ( IsVisibleGroup( VEHICLE_WINDOW ) )	HideGroup ( VEHICLE_WINDOW );
			else									ShowGroupFocus( VEHICLE_WINDOW );

			
		}
	}
}

void CInnerInterface::UpdateStatus ()
{
	GLCharacter* const pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	if ( IsVisibleGroup ( SYSTEM_MESSAGE_WINDOW ) )
	{
		UIRECT rcGlobalPos = m_pSystemMessageWindowDummy->GetGlobalPos ();
		//rcGlobalPos.top += fMENU_LIFTUP;
		m_pSystemMessageWindow->SetGlobalPos ( rcGlobalPos );
	}

	if ( IsBlockProgramAlarm () )
	{		
		if ( !IsVisibleGroup ( BLOCK_PROGRAM_ALARM ) )
		{
			UIRECT rcGlobalPos = m_pBlockProgramAlarmDummy->GetGlobalPos ();
			rcGlobalPos.top += float((rand()%200) - 100);
			rcGlobalPos.left += float((rand()%200) - 100);
			m_pBlockProgramAlarm->SetGlobalPos ( rcGlobalPos );

			GetPositionKeeper()->SetPosition ( BLOCK_PROGRAM_ALARM, rcGlobalPos );

			ShowGroupFocus ( BLOCK_PROGRAM_ALARM );
		}
	}
	else
	{
		HideGroup ( BLOCK_PROGRAM_ALARM );
	}
	
	
	//	캐릭터 생존 상태 확인
	if ( pCharacter->IsDie () )
	{
		if ( !IsVisibleGroup ( REBIRTH_DIALOGUE ) )
		{
			bool bItemRebirth = (pCharacter->ISREVIVE ())?true:false;
			bool bAutoRebirth = false;
			bool bCantUseRebirth = FALSE;

			//	귀혼주 쿨 타임 체크
			if ( bItemRebirth ) 
			{
				SITEM* pITEM = pCharacter->GET_SLOT_ITEMDATA(SLOT_NECK);
				if ( pITEM && pCharacter->IsCoolTime( pITEM->sBasicOp.sNativeID ) )
				{
					bItemRebirth = FALSE;
				}
			}

			if( pCharacter->IsRestartPossible() == FALSE )
			{
				bItemRebirth = FALSE;
				bCantUseRebirth = TRUE;
			}

			if ( bItemRebirth )
			{
				// 소모성 귀혼주이면 자동 귀혼주 사용
				SITEM* pITEM = pCharacter->GET_SLOT_ITEMDATA(SLOT_NECK);
				if ( pITEM && !pITEM->ISINSTANCE() ) bAutoRebirth = true;
			}
			
			m_pRebirthDialogue->SET_ITEMREBIRTH ( bItemRebirth, bAutoRebirth, bCantUseRebirth );
			m_pRebirthDialogue->SetCallerID ( MODAL_REBIRTH );
			ShowGroupFocus ( REBIRTH_DIALOGUE, true );
			ShowGroupFocus ( BASIC_CHAT_BOX, true );
		}
	}
	else
	{
		HideGroup ( REBIRTH_DIALOGUE, true );

		if ( IsGateOpen () )
		{
			//	만약 맵 이름이 없다면, 이동할 수 없다.
			if ( m_pMapMoveDisplay )
			{
				CString strMapName = pCharacter->DetectGateToMapName();
				m_pMapMoveDisplay->SetMapName ( strMapName + ID2GAMEINTEXT ( "MAPMOVE_MESSAGE" ) );

				UIRECT rcGlobalPos = m_pMapMoveDisplayDummy->GetGlobalPos ();
				rcGlobalPos.top -= fMENU_LIFTUP;
				m_pMapMoveDisplay->SetGlobalPos ( rcGlobalPos );

				if ( strMapName.GetLength () )	ShowGroupFocus ( MAPMOVE_DISPLAY );
				else							HideGroup ( MAPMOVE_DISPLAY );
			}		
		}
		else
		{
			HideGroup ( MAPMOVE_DISPLAY );
		}
	}

	//	스핵, 오토 프로세스 체크
	if ( IsBlockProgramFound () )
	{
		if ( !IsVisibleGroup ( WAITSERVER_DISPLAY ) )
			DoModal ( ID2GAMEINTEXT("BLOCK_PROGRAM_FOUND"), MODAL_INFOMATION, OK, MODAL_CLOSEGAME );
	}

	if ( !DxGlobalStage::GetInstance().IsEmulator() )		//네트워크 연결이 끊어진 경우
	{
		if ( !DxGlobalStage::GetInstance().IsSTATE( DxGlobalStage::EM_CHANGE ) )
		{
			if ( !DxGlobalStage::GetInstance().GetNetClient()->IsOnline() )
			{
				if ( !IsVisibleGroup ( WAITSERVER_DISPLAY ) )
					DoModal ( ID2GAMEINTEXT("PLAYINTERFACE_5"), MODAL_INFOMATION, OK, MODAL_CLOSEGAME );				
			}
		}
	}

	if ( m_pAdminMessageDisplay )
	{
		if ( IsVisibleGroup ( ADMIN_MESSAGE_DISPLAY ) )
		{
			if ( m_pAdminMessageDisplay->GetLifeTime () < 0.0f )
			{
				HideGroup ( ADMIN_MESSAGE_DISPLAY );			
			}
		}
	}

	//if ( m_pSimpleMessageMan )
	//{
	//if ( IsVisibleGroup ( SIMPLE_MESSAGE_MAN ) )
	//{
	//	if ( m_pSimpleMessageMan->GetLifeTime () < 0.0f )
	//	{
	//		HideGroup ( SIMPLE_MESSAGE_MAN );			
	//	}
	//}
	//	}

	BOOL bPartyState = FALSE;
	GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster();	//	마스터
	if ( pMaster )
	{
		bPartyState = TRUE;
	}

	if ( bPartyState != m_bPartyStateBack )
	{
		if ( GLPARTY_CLIENT *pMaster = GLPartyClient::GetInstance().GetMaster() )
		{
			ShowGroupFocus ( MINIPARTY_WINDOW );
			HideGroup ( MINIPARTY_OPEN );
		}
		else
		{
			ShowGroupBottom ( MINIPARTY_OPEN );
			HideGroup ( MINIPARTY_WINDOW );
		}
		m_bPartyStateBack = bPartyState;

// #ifdef CH_PARAM // 중국 인터페이스 변경
//		UpdatePotionTrayPosition();
// #endif
	}

	UpdateStateQuestAlarm ();
	UpdateStateSimpleHP ();	

	if ( IsVisibleGroup ( ACADEMY_CONFT_DISPLAY ) || IsVisibleGroup ( SYSTEM_MESSAGE_WINDOW ) )
	{
		HideGroup ( SIMPLE_MESSAGE_MAN );
	}
	else
	{
		if ( RANPARAM::bSHOW_TIP )
		{
			ShowGroupBottom ( SIMPLE_MESSAGE_MAN, true );
		}
		else
		{
			HideGroup ( SIMPLE_MESSAGE_MAN, true );
		}
	}
}

void CInnerInterface::MoveBasicInfoWindow ()
{
	CUIControl * pControl(NULL);

//#ifdef CH_PARAM // 중국 인터페이스 변경
//	pControl = m_pBasicQuickSkillSlot;
//#else
	pControl = m_pUILeftTopGroup;
//#endif

	if ( !pControl ) return;

	const UIRECT& rcBasicInfoViewDummy = m_pBasicInfoViewDummy->GetGlobalPos ();
	const UIRECT& rcLeftTopGroupDummy = m_pLeftTopGroupDummy->GetGlobalPos ();
	const UIRECT& rcBasicInfoView = m_pBasicInfoView->GetGlobalPos ();
	const UIRECT& rcLeftTopGroup = pControl->GetGlobalPos ();

	const D3DXVECTOR2 vLeftTopGroup ( rcLeftTopGroup.left, rcLeftTopGroup.top );
	const D3DXVECTOR2 vLeftTopGroupDummy ( rcLeftTopGroupDummy.left, rcLeftTopGroupDummy.top );

	const D3DXVECTOR2 vBasicInfoViewDummy (rcBasicInfoViewDummy.left,rcBasicInfoViewDummy.top);
	const D3DXVECTOR2 vBasicInfoView (rcBasicInfoView.left,rcBasicInfoView.top);
	const D3DXVECTOR2 vDist = vBasicInfoViewDummy - vBasicInfoView;
	const float fDist = D3DXVec2LengthSq ( &vDist );

	if ( fDist > 900.0f )	//	컨트롤이 떨어졌나?
	{	
		if ( vBasicInfoViewDummy != vLeftTopGroup ) pControl->SetGlobalPos ( vBasicInfoViewDummy );
	}
	else
	{
		if ( vLeftTopGroupDummy != vLeftTopGroup )		pControl->SetGlobalPos ( vLeftTopGroupDummy );
		if ( vBasicInfoViewDummy != vBasicInfoView )	m_pBasicInfoView->SetGlobalPos( vBasicInfoViewDummy );
	}
}

void CInnerInterface::BasicInfoViewDoubleClick()
{
	CUIControl * pControl(NULL);

//#ifdef CH_PARAM // 중국 인터페이스 변경
//	pControl = m_pBasicQuickSkillSlot;
//#else
	pControl = m_pUILeftTopGroup;
//#endif

	const UIRECT& rcBasicInfoViewDummy = m_pBasicInfoViewDummy->GetGlobalPos ();
	const UIRECT& rcLeftTopGroupDummy = m_pLeftTopGroupDummy->GetGlobalPos ();

	const D3DXVECTOR2 vLeftTopGroupDummy ( rcLeftTopGroupDummy.left, rcLeftTopGroupDummy.top );
	const D3DXVECTOR2 vBasicInfoViewDummy (rcBasicInfoViewDummy.left,rcBasicInfoViewDummy.top);

	pControl->SetGlobalPos ( vLeftTopGroupDummy );
	m_pBasicInfoView->SetGlobalPos( vBasicInfoViewDummy );
}

//************************************
// Method:    IsOpenWindowToMoveBlock
// FullName:  CInnerInterface::IsOpenWindowToMoveBlock
// Access:    public 
// Returns:   BOOL
// Qualifier: 
//************************************
BOOL CInnerInterface::IsOpenWindowToMoveBlock()
{
	// 여기에 추가되는 컨트롤은 반듯이 InitDeviceObjects에서 생성시 SetVisibleSingle( FALSE )를 해줘야 한다.
	if( IsVisibleGroup( TRADE_WINDOW ) || 
		IsVisibleGroup( TRADEINVENTORY_WINDOW ) || 
		IsVisibleGroup( MARKET_WINDOW ) || 
		IsVisibleGroup( STORAGE_WINDOW ) || 
		IsVisibleGroup( ITEMBANK_WINDOW ) || 
		IsVisibleGroup( MAP_REQUIRE_CHECK ) ||
		IsVisibleGroup( REBIRTH_DIALOGUE ) ||
		IsVisibleGroup( DIALOGUE_WINDOW ) || 
		IsVisibleGroup( CLUB_STORAGE_WINDOW ) || 
		IsVisibleGroup( PRIVATE_MARKET_MAKE ) || 
		IsVisibleGroup( GAMBLE_DIALOGUE ) || 
		IsVisibleGroup( GAMBLE_SELECT_DIALOGUE ) || 
		IsVisibleGroup( GAMBLE_AGAIN_DIALOGUE ) ||
		IsVisibleGroup( GAMBLE_ANIMATION_DIALOGUE ) || 
		IsVisibleGroup( GAMBLE_RESULT_EVEN_DIALOGUE ) ||
		IsVisibleGroup( GAMBLE_RESULT_ODD_DIALOGUE ) ||
		IsVisibleGroup( ITEM_REBUILD_WINDOW ) || 
		IsVisibleGroup( REBUILDINVENTORY_WINDOW ) ||	// ITEMREBUILD_MARK
		IsVisibleGroup( ITEM_GARBAGE_WINDOW ) || 
		IsVisibleGroup( GARBAGEINVENTORY_WINDOW ) ||	// 휴지통
		IsVisibleGroup( ITEMSHOP_WINDOW ) ||
		IsVisibleGroup( ITEM_SHOP_SEARCH_WINDOW ) ||
		IsVisibleGroup( ITEM_SEARCH_RESULT_WINDOW ) || // 아이템 검색 창
		IsVisibleGroup( PETSKIN_MIX_IMAGE_WINDOW ) ||
		IsVisibleGroup( ITEM_MIX_WINDOW ) ||
		IsVisibleGroup( ITEM_MIX_INVEN_WINDOW ) )

	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
#ifdef CH_PARAM // 중국 인터페이스 변경
void CInnerInterface::UpdatePotionTrayPosition()
{
	m_pUILeftTopGroup->SetGlobalPos( m_pBasicPotionTrayDummy->GetGlobalPos() );
	m_pQuestAlarm->SetGlobalPos( m_pQuestAlarmDummy->GetGlobalPos() );

	BOOL bVisibleQuest = IsVisibleGroup( QUEST_ALARM);
	BOOL bVisibleMini = IsVisibleGroup( MINIPARTY_OPEN );

	if( bVisibleQuest && bVisibleMini ) return;

	UIRECT rcDummy = m_pUILeftTopGroup->GetGlobalPos();
	rcDummy.left += 41; // 퀘스트, 미니파티 버튼의 가로 크기가 41
	rcDummy.right = rcDummy.left + rcDummy.sizeX;
	m_pUILeftTopGroup->SetGlobalPos( rcDummy );

	CUIControl * pUIControl(NULL);

	if( !bVisibleQuest && !bVisibleMini )
	{
		pUIControl = m_pUILeftTopGroup;
	}
	else if( !bVisibleMini )
	{
		pUIControl = m_pQuestAlarm;
	}

	if( pUIControl != NULL )
	{
		rcDummy = pUIControl->GetGlobalPos();
		rcDummy.left += 41;
		rcDummy.right = rcDummy.left + rcDummy.sizeX;
		pUIControl->SetGlobalPos( rcDummy );
	}
}
#endif
*/
void CInnerInterface::SetArmSwap( BOOL bSwap )
{
	m_pInventoryWindow->SetArmSwapTabButton( bSwap );
}

bool CInnerInterface::ItemShopAuth ()
{

//	ItemShopAuth
#if defined ( JP_PARAM ) 
	if ( m_bItemShopLoad )	return false;

	GLCharacter* pChar = GLGaeaClient::GetInstance().GetCharacter();
	if ( !pChar ) return false;

	CString strData = "";
	strData.Format( "%s?id=%s& uuid=%s", RANPARAM::ItemShopAddress, pChar->m_szUID, RANPARAM::GETJAPANUUID() );

	CCommonWeb::Get()->Navigate( CCommonWeb::ITEMSHOP_ID, strData, true );
	CCommonWeb::Get()->ReSetCompleteLoad( CCommonWeb::ITEMSHOP_ID );

	m_bItemShopLoad = true;

	// 로딩 완료 메세지가 올때까지 기다린다.
	return false;
#endif	
	return true;
}

void CInnerInterface::ItemShopVisible()
{

//	ItemShopAuth
#if defined ( JP_PARAM ) 
	if ( m_bItemShopLoad )
	{
		if ( CCommonWeb::Get()->IsCompleteLoad( CCommonWeb::ITEMSHOP_ID ) )
		{
			ShowGroupFocus ( ITEMSHOP_WINDOW );			
			HideGroup ( HELP_WINDOW );
			m_bItemShopLoad = false;
		}
	}
#endif
	return;
}

void CInnerInterface::SetItemMixResult( CString strMsg, bool bSuccess, bool bFail )
{
	m_pItemMixWindow->SetItemMixResult( strMsg, bSuccess, bFail );
}

void CInnerInterface::SET_GATHER_GAUGE( DWORD dwGaeaID, float fTime )
{
	m_pGatherGauge->SetTime( fTime );
	m_pGatherGauge->SetMaterial ( dwGaeaID );
}

void CInnerInterface::VisibleCDMRanking( bool bVisible )
{
	if ( bVisible )
	{
		HideGroup( QUEST_HELPER_DISPLAY );
		ShowGroupBottom ( CDM_RANKING_DISPLAY );
	}
	else
	{
		HideGroup( CDM_RANKING_DISPLAY );
		ShowGroupBottom ( QUEST_HELPER_DISPLAY );
	}
}

void CInnerInterface::RefreashCDMRanking()
{
	if ( m_pCdmRankingDisplay && m_pCdmRankingDisplay->IsVisible() )
		m_pCdmRankingDisplay->RefreashCdmRanking();
}