#include "pch.h"
#include "DialogueWindow.h"

#include "BasicLineBox.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "BasicTextButton.h"
#include "UITextControl.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "ModalWindow.h"
#include "GLQuestMan.h"
#include "GLQuest.h"
#include "GameTextControl.h"
#include "GLItemMan.h"
#include "ModalCallerID.h"
#include "InventoryWindow.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcDialogue.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcDialogueCase.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcTalkControl.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcTalk.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Npc/NpcTalkCondition.h"

#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	int		CDialogueWindow::nSTARTLINE = 0;
const	int		CDialogueWindow::nSTARTINDEX = 1;
const	float	CDialogueWindow::fANSWER_PART_INTERVAL = 8.0f;

CDialogueWindow::CDialogueWindow ()
: m_dwRandomTime ( 0 )
, m_fRandomTime(0.0f)
{
}

CDialogueWindow::~CDialogueWindow ()
{
}

void CDialogueWindow::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxDialogue ( "DIALOGUE_BACK_BOX" );
		RegisterControl ( pBasicLineBox );
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_DIALOGUE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxDialogueAnswer ( "DIALOGUE_ANSWER_BACK_BOX" );
		RegisterControl ( pBasicLineBox );
	}

	//	텍스트 박스
	{
		m_pQuestionBox = new CBasicTextBox;
		m_pQuestionBox->CreateSub ( this, "DIALOGUE_QUESTION_TEXTBOX" );
		m_pQuestionBox->SetFont ( pFont9 );		
		m_pQuestionBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );		
		RegisterControl ( m_pQuestionBox );

		int nTotalLine = m_pQuestionBox->GetVisibleLine ();

		//	스크롤바
		m_pScrollBar = new CBasicScrollBarEx;
		m_pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_DIALOGUE_SCROLLBAR );
		m_pScrollBar->CreateBaseScrollBar ( "BASIC_DIALOGUE_SCROLLBAR" );
		m_pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( m_pScrollBar );
	}

	{
		//	텍스트 박스
		m_pAnswerBox = new CBasicTextBoxEx;
		m_pAnswerBox->CreateSub ( this, "DIALOGUE_ANSWER_TEXTBOX", UI_FLAG_DEFAULT, DIALOGUE_ANSWER_TEXTBOX );
		m_pAnswerBox->SetFont ( pFont9 );
		m_pAnswerBox->SetPartInterval ( fANSWER_PART_INTERVAL );
		m_pAnswerBox->SetSensitive ( true );
		RegisterControl ( m_pAnswerBox );

		int nVisibleLine = m_pAnswerBox->GetVisibleLine ();
		int nTotalLine = m_pAnswerBox->GetTotalLine ();

		//	스크롤바
		m_pAnswerScrollBar = new CBasicScrollBarEx;
		m_pAnswerScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, BASIC_DIALOGUE_ANSWER_SCROLLBAR );
		m_pAnswerScrollBar->CreateBaseScrollBar ( "BASIC_DIALOGUE_ANSWER_SCROLLBAR" );
		m_pAnswerScrollBar->GetThumbFrame()->SetState ( nTotalLine, nVisibleLine );
		RegisterControl ( m_pAnswerScrollBar );
	}


		// RandomTime
	{
		const int nBUTTONSIZE = CBasicTextButton::SIZE16;
		m_pRandomTimeButton = new CBasicTextButton;
		m_pRandomTimeButton->CreateSub ( this, "BASIC_TEXT_BUTTON18", UI_FLAG_XSIZE, RANDOM_TIME_BUTTON );
		m_pRandomTimeButton->CreateBaseButton ( "RANDOM_TIME_BUTTON", nBUTTONSIZE, CBasicButton::RADIO_FLIP, ID2GAMEWORD( "RANDOM_TIME_BUTTON",0));
		m_pRandomTimeButton->SetFlip ( FALSE );
		RegisterControl ( m_pRandomTimeButton );

		m_pRandomTimeBack = new CBasicLineBox;
		m_pRandomTimeBack->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		m_pRandomTimeBack->CreateBaseBoxEditBox ( "RANDOM_TIME_BACK" );
		RegisterControl ( m_pRandomTimeBack );    


		m_pRandomTimeText = new CBasicTextBox;
		m_pRandomTimeText->CreateSub ( this, "RANDOM_TIME_TEXT", UI_FLAG_DEFAULT, RANDOM_TIME_TEXT );
		m_pRandomTimeText->SetFont ( pFont9 );
		m_pRandomTimeText->SetTextAlign ( TEXT_ALIGN_CENTER_X );	
		RegisterControl ( m_pRandomTimeText );
	}
}

void CDialogueWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pAnswerBox->SetUseOverColor ( FALSE );
	CUIWindowEx::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );	
	
	m_pAnswerBox->SetGlobalPos ( m_pAnswerBox->GetGlobalPos () );

	if ( m_dwRandomTime > 0 )
	{
		m_fRandomTime += fElapsedTime;

		CString strTime;
		strTime.Format( "%d", (int) (m_dwRandomTime - m_fRandomTime) );

		m_pRandomTimeText->SetText( strTime, NS_UITEXTUTIL::WHITE );

		// 시간 오버 
		if ( m_dwRandomTime < m_fRandomTime )
		{
			CInnerInterface::GetInstance().PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT( "NPCTALK_RANDOM_TIME_OVER" ) ); 
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
		}
	}

	if ( bFirstControl )
	{
		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();		
			m_pQuestionBox->SetCurLine ( 0 );
			const int nTotalLine = m_pQuestionBox->GetTotalLine ();
			const int nLinePerOneView = m_pQuestionBox->GetVisibleLine ();
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
				m_pQuestionBox->SetCurLine ( nPos );			
			}
		}
		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pAnswerScrollBar->GetThumbFrame ();		
			m_pAnswerBox->SetCurLine ( 0 );
			const int nTotalLine = m_pAnswerBox->GetTotalLine ();
			const int nLinePerOneView = m_pAnswerBox->GetVisibleLine ();
			if ( nLinePerOneView < nTotalLine )
			{
				const int nMovableLine = nTotalLine - nLinePerOneView;
				float fPercent = pThumbFrame->GetPercent ();
				int nPos = (int)floor(fPercent * nMovableLine);
				if ( nPos < nSTARTLINE ) nPos = nSTARTLINE;
				m_pAnswerBox->SetCurLine ( nPos );			
			}
		}
	}
}

void CDialogueWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case DIALOGUE_ANSWER_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = m_pAnswerBox->GetSelectPos ();
				if ( nIndex < 0 ) return ;

				m_pAnswerBox->SetUseOverColor ( TRUE );		
				m_pAnswerBox->SetOverColor ( nIndex, NS_UITEXTCOLOR::IVORY );

				const DWORD NID = m_pAnswerBox->GetTextData ( nIndex );				
				if ( !m_pDialogueCase ) return ;
				CNpcTalkControl* pNpcTalkControl = m_pDialogueCase->GetTalkControl();
				if ( !pNpcTalkControl ) return ;

				SNpcTalk* pNpcTalk = pNpcTalkControl->GetTalk ( NID );
				if ( !pNpcTalk ) return ;

				if ( dwMsg & UIMSG_LB_UP )
				{	
					TranslateTalkMessage ( pNpcTalk );
				}
				else
				{
					//	퀘스트 보상치 표현
					if ( pNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
					{						
						DWORD dwQuestID = pNpcTalk->m_dwQuestStartID[0];
						GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
						if ( !pQuest )
						{
							CDebugSet::ToLogFile ( "quest id = %d 의 정보를 가져오지 못하였습니다.", dwQuestID );
							return;
						}

						//	보상치가 없으면 아무것도 표시하지 않음
						if ( !(pQuest->m_dwGiftEXP || pQuest->m_dwGiftMONEY ||
							 pQuest->m_dwGiftELEMENT || pQuest->m_dwGiftLIFEPOINT ||
							 pQuest->m_dwGiftSKILLPOINT || pQuest->m_dwGiftSTATSPOINT ||
							 pQuest->m_vecGiftITEM.size () || pQuest->m_vecGiftSKILL.size ()) )
						{
							return ;
						}

						if ( CInnerInterface::GetInstance().BEGIN_COMMON_LINEINFO_MULTI () )
						{
							CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( ID2GAMEWORD("QUEST_GIFT",9), NS_UITEXTCOLOR::GREENYELLOW );

							CString strCombine;

							if ( pQuest->m_dwGiftEXP )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",0), pQuest->m_dwGiftEXP );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftMONEY )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",1), pQuest->m_dwGiftMONEY );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftELEMENT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",2), pQuest->m_dwGiftELEMENT );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftLIFEPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",3), pQuest->m_dwGiftLIFEPOINT );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							//if ( pQuest->m_dwGiftDisPK )
							//{
							//	strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",4), pQuest->m_dwGiftDisPK );
							//	CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							//}
							if ( pQuest->m_dwGiftSKILLPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",5), pQuest->m_dwGiftSKILLPOINT );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}
							if ( pQuest->m_dwGiftSTATSPOINT )
							{
								strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",6), pQuest->m_dwGiftSTATSPOINT );
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strCombine, NS_UITEXTCOLOR::WHITE );
							}

							if ( pQuest->m_vecGiftITEM.size () )
							{
								CString strItem = ID2GAMEWORD("QUEST_GIFT",7);
								strItem += " : ";
								INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
								INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();
								for ( ; iter != iter_end; )
								{
									SITEMCUSTOM& sItemCustom = (*iter);
									SITEM* pItem = GLItemMan::GetInstance().GetItem ( sItemCustom.sNativeID );
									if ( !pItem ) continue;
									strItem += pItem->GetName ();

									++iter;
									if ( iter == iter_end ) break;
									strItem += ", ";
								}
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strItem, NS_UITEXTCOLOR::WHITE );
							}

							if ( pQuest->m_vecGiftSKILL.size () )
							{
								CString strSkill = ID2GAMEWORD("QUEST_GIFT",8);
								strSkill += " : ";
								DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin ();
								DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end ();
								for ( ; iter != iter_end; )
								{
									DWORD dwID = (*iter);									
									PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( SNATIVEID(dwID) );
									if ( !pSKILL ) continue;
									strSkill += pSKILL->GetName();

									++iter;
									if ( iter == iter_end ) break;
									strSkill += ", ";
								}
								CInnerInterface::GetInstance().ADD_COMMON_LINEINFO_MULTI ( strSkill, NS_UITEXTCOLOR::WHITE );
							}

							CInnerInterface::GetInstance().END_COMMON_LINEINFO_MULTI ();
						}
					}
				}
			}
		}
		break;
	}
}

void CDialogueWindow::TranslateTalkMessage ( SNpcTalk* pNpcTalk )
{
	switch ( pNpcTalk->m_nACTION )
	{
	case SNpcTalk::EM_DO_NOTHING:
		{
		}
		break;
	case SNpcTalk::EM_PAGE_MOVE:
		{
			LoadNode ( pNpcTalk->m_dwACTION_NO );
		}
		break;
	case SNpcTalk::EM_BASIC:
		{
			TranslateBasicMessage ( pNpcTalk );
		}
		break;
	case SNpcTalk::EM_QUEST_START:
		{
			LoadNode ( pNpcTalk->m_dwACTION_NO );

			TranslateQuestStartMessage ( pNpcTalk );
		}
		break;
	case SNpcTalk::EM_QUEST_STEP:
		{
			LoadNode ( pNpcTalk->m_dwACTION_NO );

			TranslateQuestStepMessage ( pNpcTalk );
		}
		break;
	};
}

void CDialogueWindow::TranslateBasicMessage ( SNpcTalk* pNpcTalk )
{

	// Need Add New Item Con.
	DWORD dwMsg = pNpcTalk->m_dwACTION_NO;
	DWORD dwGlobalID = pNpcTalk->m_dwGLOB_ID;
	DWORD *pParamA = pNpcTalk->m_dwNeedItem;
	DWORD paramB = pNpcTalk->m_dwACTION_PARAM2;

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return;
	PGLCROWCLIENT pCrow = pLand->GetCrow ( m_sCrowID.dwID );
	if ( !pCrow )	return;

	switch ( dwMsg )
	{
	case SNpcTalk::EM_STORAGE:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			CInnerInterface::GetInstance().SetDefaultPosInterface( INVENTORY_WINDOW );
			CInnerInterface::GetInstance().SetDefaultPosInterface( STORAGE_WINDOW );
			CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
			CInnerInterface::GetInstance().SetStorageWindowOpen ( m_sCrowID.dwID );
		}
		break;
	case SNpcTalk::EM_MARKET:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			CInnerInterface::GetInstance().SetDefaultPosInterface( INVENTORY_WINDOW );
			CInnerInterface::GetInstance().SetDefaultPosInterface( MARKET_WINDOW );
			CInnerInterface::GetInstance().SetMarketWindowOpen ( pCrow->m_pCrowData->sNativeID );
			CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
		}
		break;
	case SNpcTalk::EM_CURE:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			GLGaeaClient::GetInstance().GetCharacter()->ReqCure ( m_sCrowID.dwID, dwGlobalID );
		}
		break;
	case SNpcTalk::EM_STARTPOINT:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			GLGaeaClient::GetInstance().GetCharacter()->ReqReGenGate ( m_sCrowID.dwID );
		}
		break;

	case SNpcTalk::EM_CHAR_RESET:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			GLGaeaClient::GetInstance().GetCharacter()->ReqCharReset ( m_sCrowID.dwID );
		}
		break;

	// Need Add New Item Con.
	case SNpcTalk::EM_ITEM_TRADE:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			GLGaeaClient::GetInstance().GetCharacter()->ReqItemTrade ( m_sCrowID.dwID, dwGlobalID, pParamA, paramB );
		}
		break;

	case SNpcTalk::EM_BUSSTATION:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			
			//	NOTE : 버스 정류장 인터페이스 로드.
			CInnerInterface::GetInstance().SetBusWindowOpen ( m_sCrowID.dwID, pNpcTalk );
		}
		break;

	case SNpcTalk::EM_CLUB_NEW:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );

            //	NOTE : 클럽 만들기 신청
			CInnerInterface::GetInstance().SetClubMake ( m_sCrowID.dwID );
		}
		break;

	case SNpcTalk::EM_CLUB_UP:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			
			GLGaeaClient::GetInstance().GetCharacter()->ReqClubRank(m_sCrowID.dwID);
		}
		break;

	case SNpcTalk::EM_CD_CERTIFY:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			GLGaeaClient::GetInstance().GetCharacter()->ReqCDCertify(m_sCrowID.dwID);
		}
		break;

	case SNpcTalk::EM_COMMISSION:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );

			//	NOTE : 수수료 지정
			//
			DoModal ( ID2GAMEINTEXT("MODAL_COMMISSION"), MODAL_INPUT, EDITBOX, MODAL_COMMISSION );
		}
		break;

	case SNpcTalk::EM_CLUB_STORAGE:
		{
			GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter ();
			GLCLUB& sCLUB = pCharacter->m_sCLUB;
			bool bVALID_CLUB = sCLUB.m_dwID!=CLUB_NULL;
			bool bMASTER = sCLUB.m_dwMasterID==pCharacter->m_dwCharID;

			if ( bVALID_CLUB && bMASTER )
			{
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );

				pCharacter->ReqGetClubStorage ();

				//	NOTE : 창고 띄우기
				//			
				CInnerInterface::GetInstance().SetDefaultPosInterface( INVENTORY_WINDOW );
				CInnerInterface::GetInstance().SetDefaultPosInterface( CLUB_STORAGE_WINDOW );
				CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
				CInnerInterface::GetInstance().SetClubStorageWindowOpen ();
			}
			else
			{
				CInnerInterface::GetInstance().PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("ONLY_CLUB_MASTER_USE_CLUBSTORAGE") );
			}
		}
		break;

	case SNpcTalk::EM_ITEM_REBUILD:	// ITEMREBUILD_MARK
		{
			CInnerInterface::GetInstance().HideGroup( GetWndID() );
			CInnerInterface::GetInstance().OpenItemRebuildWindow();
		}
		break;

	case SNpcTalk::EM_ODDEVEN:
		{
			CInnerInterface::GetInstance().HideGroup( GetWndID() );
			CInnerInterface::GetInstance().SetDefaultPosInterface( GAMBLE_DIALOGUE );
			CInnerInterface::GetInstance().SetDefaultPosInterface( INVENTORY_WINDOW );
			CInnerInterface::GetInstance().ShowGroupFocus ( GAMBLE_DIALOGUE );
			CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
		}
		break;
	case SNpcTalk::EM_RECOVERY_EXP:
		{
			CInnerInterface::GetInstance().HideGroup( GetWndID() );
			GLGaeaClient::GetInstance().GetCharacter()->ReqGetReExpNpc( m_sCrowID.dwID );
		}
		break;
	case SNpcTalk::EM_RANDOM_PAGE:
		{
			int nCnt = MAX_RANDOM_PAGE;

			for ( int i = 0; i < MAX_RANDOM_PAGE; ++i )
			{
				if ( pNpcTalk->m_dwRandomPageID[i] == UINT_MAX )
				{
					nCnt = i;
					break;
				}
			}

			if ( nCnt <= 0 ) 
			{
				CDebugSet::ToLogFile( "Random Page Not Found [%s]",m_DialogueSet.m_strFileName );
				break;
			}
			
			int nChoice = int ( rand() % nCnt );

			LoadNode( pNpcTalk->m_dwRandomPageID[nChoice], true, pNpcTalk->m_dwRandomTime );
		}
		break;
	case SNpcTalk::EM_ITEMSEARCH_PAGE:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			CInnerInterface::GetInstance().HideGroup( ITEM_SEARCH_RESULT_WINDOW );
			CInnerInterface::GetInstance().SetDefaultPosInterface( ITEM_SHOP_SEARCH_WINDOW );
			CInnerInterface::GetInstance().ShowGroupFocus( ITEM_SHOP_SEARCH_WINDOW );
		}
		break;

	case SNpcTalk::EM_ATTENDANCE_BOOK:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			CInnerInterface::GetInstance().ShowGroupFocus ( ATTENDANCE_BOOK_WINDOW );
		}
		break;
	case SNpcTalk::EM_ITEM_MIX:
		{
			CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			CInnerInterface::GetInstance().OpenItemMixWindow( m_sCrowID.dwID );
		}
		break;
	};
}

void CDialogueWindow::TranslateQuestStartMessage ( SNpcTalk* pNpcTalk )
{
	GASSERT(pNpcTalk&&"CDialogueWindow::TranslateQuestStartMessage");

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return;
	PGLCROWCLIENT pCrow = pLand->GetCrow ( m_sCrowID.dwID );
	if ( !pCrow )	return;

	for ( int i = 0; i < MAX_QUEST_START; ++i )
	{
		if ( pNpcTalk->m_dwQuestStartID[i] != UINT_MAX && pNpcTalk->DoTEST(m_pCHAR, i))
		{
			GLGaeaClient::GetInstance().GetCharacter()->ReqQuestStart ( 
				m_sCrowID.dwID, pNpcTalk->m_dwGLOB_ID, pNpcTalk->m_dwQuestStartID[i] );
		}
	}
}

void CDialogueWindow::TranslateQuestStepMessage ( SNpcTalk* pNpcTalk )
{
	GASSERT(pNpcTalk&&"CDialogueWindow::TranslateQuestStepMessage");

	PLANDMANCLIENT pLand = GLGaeaClient::GetInstance().GetActiveMap();
	if ( !pLand )	return;
	PGLCROWCLIENT pCrow = pLand->GetCrow ( m_sCrowID.dwID );
	if ( !pCrow )	return;

	GLGaeaClient::GetInstance().GetCharacter()->ReqQuestStepNpcTalk ( m_sCrowID.dwID,
		pNpcTalk->m_dwGLOB_ID, pNpcTalk->m_dwACTION_PARAM1, pNpcTalk->m_dwACTION_PARAM2 );
}

void CDialogueWindow::LoadBasicTalk ()
{
	if ( !m_pDialogueCase ) return ;
	const std::string str = m_pDialogueCase->GetBasicTalk ();

	m_pQuestionBox->SetText ( str.c_str(), NS_UITEXTCOLOR::BURLYWOOD );

	int nTotal = m_pQuestionBox->GetTotalLine ();
	int nViewPerPage = m_pQuestionBox->GetVisibleLine ();
	CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar->GetThumbFrame ();
	pThumbFrame->SetState ( nTotal, nViewPerPage );
	pThumbFrame->SetPercent ( 0.0f );
}

void CDialogueWindow::LoadButtonTalk ( bool bRandom )
{
	m_pAnswerBox->ClearText ();

	static const int nKEYWORDSIZE = 10;
	static const CString strKeyword[nKEYWORDSIZE] = 
	{
		"DIALOGUE_ICON_HEAL",
		"DIALOGUE_ICON_TRADE",
		"DIALOGUE_ICON_STARTPOINT",
		"DIALOGUE_ICON_STORAGE",
		"DIALOGUE_ICON_MARKET",
		"DIALOGUE_ICON_RESETPOINT",
		"DIALOGUE_ICON_BUS",
		"DIALOGUE_ICON_QUEST_START",
		"DIALOGUE_ICON_QUEST_ING",
		"DIALOGUE_ICON_TALK",
	};

	enum
	{
		ICON_HEAL,
		ICON_TRADE,
		ICON_STARTPOINT,
		ICON_STORAGE,
		ICON_MARKET,
		ICON_RESETPOINT,
		ICON_BUS,
		ICON_QUEST_START,
		ICON_QUEST_ING,
		ICON_QUEST_TALK,
	};

	//	버튼 텍스트 설정
	if ( !m_pDialogueCase ) return ;
	CNpcTalkControl* pNpcTalkControl = m_pDialogueCase->GetTalkControl ();
	if ( pNpcTalkControl )
	{
		CNpcTalkControl::NPCMAP* pMap = pNpcTalkControl->GetTalkMap ();
		CNpcTalkControl::NPCMAP_IT iter = pMap->begin();
		CNpcTalkControl::NPCMAP_IT iter_end = pMap->end();

		vector<SNpcTalk*> vecTalk;
		vecTalk.reserve( pMap->size() );

		for ( ; iter != iter_end; ++iter )
		{
			SNpcTalk* pNpcTalk = (*iter).second;
			vecTalk.push_back( pNpcTalk );
		}
		
		if ( bRandom )
		{
			random_shuffle( vecTalk.begin(), vecTalk.end() );			
		}

		vector<SNpcTalk*>::iterator	pos = vecTalk.begin();
		vector<SNpcTalk*>::iterator	pos_end = vecTalk.end();

		for ( ; pos != pos_end; ++pos )
		{
			SNpcTalk* pNpcTalk = (SNpcTalk*) (*pos);

			//	Note : 조건에 부합하지 못할 경우 추가하지 않음.
			if ( !pNpcTalk->DoTEST(m_pCHAR) )	continue;
			
			CString strAnswerTalk = pNpcTalk->GetTalk();			

            int nImageType = 0;	
			D3DCOLOR dwColor = NS_UITEXTCOLOR::PERU;
			switch ( pNpcTalk->m_nACTION )
			{
			case SNpcTalk::EM_QUEST_START:
				{
					dwColor = NS_UITEXTCOLOR::LIGHTCYAN;
					nImageType = ICON_QUEST_START;

					//	NOTE : 비용이 있을 경우 추가
					DWORD dwQuestID = pNpcTalk->m_dwQuestStartID[0];
					GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
					if ( !pQuest )
					{
						CDebugSet::ToLogFile ( "quest id = %d 의 정보를 가져오지 못하였습니다.", dwQuestID );
						continue;
					}

					const DWORD dwBeginMoney = pQuest->m_dwBeginMoney;
					if ( dwBeginMoney )
					{
						CString strQuestCost;
						strQuestCost.Format ( " [%s:%d]", ID2GAMEWORD ( "QUEST_BEGIN_MONEY" ), dwBeginMoney );
						strAnswerTalk += strQuestCost;
					}					
				}
				break;

			case SNpcTalk::EM_QUEST_STEP:
				{
					dwColor = NS_UITEXTCOLOR::LIGHTCYAN;
					nImageType = ICON_QUEST_START;
				}
				break;

			case SNpcTalk::EM_BASIC:
				{
					switch ( pNpcTalk->m_dwACTION_NO )
					{
					//case SNpcTalk::EM_STORAGE:	nImageType = ICON_STORAGE; break;
					//case SNpcTalk::EM_MARKET:	nImageType = ICON_MARKET; break;
					case SNpcTalk::EM_CURE:		nImageType = ICON_HEAL; break;
					case SNpcTalk::EM_STARTPOINT: nImageType = ICON_STARTPOINT; break;
					//case SNpcTalk::EM_CHAR_RESET: nImageType = ICON_RESETPOINT; break;
					//case SNpcTalk::EM_ITEM_TRADE: nImageType = ICON_TRADE; break;
					default:	nImageType = ICON_QUEST_TALK;		break;
					}
				}
				break;

			case SNpcTalk::EM_PAGE_MOVE:
				{
					nImageType = ICON_QUEST_TALK;
				}
				break;
			}

			nImageType = ( nImageType >= nKEYWORDSIZE )?nKEYWORDSIZE-1:nImageType;

			int nIndex = m_pAnswerBox->AddText ( strAnswerTalk, dwColor );
			m_pAnswerBox->SetTextData ( nIndex, pNpcTalk->m_dwNID );				 
			m_pAnswerBox->SetTextImage ( nIndex, strKeyword[nImageType] );
		}

		int nTotal = m_pAnswerBox->GetTotalLine ();
		int nViewPerPage = m_pAnswerBox->GetVisibleLine ();
		CBasicScrollThumbFrame* const pThumbFrame = m_pAnswerScrollBar->GetThumbFrame ();
		pThumbFrame->SetState ( nTotal, nViewPerPage );
		pThumbFrame->SetPercent ( 0.0f );
	}
}

void CDialogueWindow::LoadNode( DWORD dwNID, bool bRandom, DWORD dwRandomTime )
{
	m_pDialogue = m_DialogueSet.GetDialogue ( dwNID );
	GASSERT(m_pDialogue&&"m_DialogueSet.GetDialogue()");
	if ( !m_pDialogue )			return;
	
	DWORD dwIndex = m_pDialogue->FindConditionSatisfied ( m_pCHAR );
	GASSERT(dwIndex!=UINT_MAX&&"m_pDialogue->FindConditionSatisfied()");
	if ( dwIndex==UINT_MAX )	return;

	CNpcDialogueCase* pCase = m_pDialogue->GetDlgCase(dwIndex);	
	GASSERT(pCase&&"m_pDialogueCase = NULL");
	m_pDialogueCase = pCase;

	m_dwRandomTime = dwRandomTime;
	m_fRandomTime = 0.0f;

    //
	if ( m_dwRandomTime > 0 )
	{
		m_pRandomTimeButton->SetVisibleSingle( TRUE );
		m_pRandomTimeBack->SetVisibleSingle( TRUE );
		m_pRandomTimeText->SetVisibleSingle( TRUE );
		
		CString strTime;
		strTime.Format( "%d", m_dwRandomTime );

		m_pRandomTimeText->SetText( strTime, NS_UITEXTUTIL::WHITE );

	}
	else
	{
		m_pRandomTimeButton->SetVisibleSingle( FALSE );
		m_pRandomTimeBack->SetVisibleSingle( FALSE );
		m_pRandomTimeText->SetVisibleSingle( FALSE );
	}

	LoadBasicTalk ();
	LoadButtonTalk ( bRandom );
}

BOOL CDialogueWindow::SetDialogueData ( STARGETID sCrowID, GLCHARLOGIC *pCHAR )
{
	GASSERT ( pCHAR );
	if ( sCrowID.emCrow != CROW_NPC )	return FALSE;	

	m_sCrowID = sCrowID;
	m_pCHAR = pCHAR;

	PGLCROWCLIENT pCrow = GLGaeaClient::GetInstance().GetActiveMap()->GetCrow(sCrowID.dwID);
	if ( !pCrow )						return FALSE;

	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pCrow->m_pCrowData->sNativeID );
	if ( m_DialogueSet.Load ( pCrowData->GetTalkFile() ) )
	{
		LoadNode ( nSTARTINDEX );
		return TRUE;
	}

	return FALSE;
}