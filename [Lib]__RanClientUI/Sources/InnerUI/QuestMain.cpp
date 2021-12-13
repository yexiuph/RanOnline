#include "pch.h"
#include "QuestMain.h"
#include "DxFontMan.h"
#include "BasicLineBox.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "ItemSlot.h"
#include "BasicTextButton.h"
#include "GLQuestPlay.h"
#include "GLQuest.h"
#include "GLQuestMan.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"
#include "QuestGiftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CQuestMain::fANSWER_LINE_INTERVAL = 5.0f;
const	DWORD	CQuestMain::dwMINITE_BY_HOUR = 60;

CQuestMain::CQuestMain ()
{
}

CQuestMain::~CQuestMain ()
{
}

void	CQuestMain::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_DESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestStepDesc ( "QUEST_MAIN_DESC_BACK" );
	RegisterControl ( pBasicLineBox );

	//	텍스트 박스
	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "QUEST_MAIN_DESC_TEXTBOX", UI_FLAG_DEFAULT, QUEST_MAIN_DESC_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pDescText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_DESC_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_MAIN_DESC_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pDescScrollBar = pScrollBar;
	}

	//	조건
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "QUEST_MAIN_CONDITION", UI_FLAG_DEFAULT, QUEST_MAIN_CONDITION );
		pTextBox->SetFont ( pFont9 );
		RegisterControl ( pTextBox );
		m_pCondition = pTextBox;
	}

	{
		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "QUEST_MAIN_TEXTBOX", UI_FLAG_DEFAULT, QUEST_MAIN_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pMainText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_MAIN_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_MAIN_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pMainScrollBar = pScrollBar;
	}

	{	//	아이템 타이틀 BACK 이미지
		CreateControl ( "QUEST_MAIN_ITEM_TITLE_BACK_L" );
		CreateControl ( "QUEST_MAIN_ITEM_TITLE_BACK_R" );

		CBasicTextBox* pTitle = CreateStaticControl ( "QUEST_MAIN_ITEM_TITLE", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X );
		pTitle->AddText ( ID2GAMEWORD("QUEST_MAIN_ITEM_TITLE") );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_MAIN_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestStepItem ( "QUEST_MAIN_ITEM_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pItemSlot = CreateItemSlot ( "QUEST_MAIN_ITEMSLOT", QUEST_MAIN_ITEMSLOT );
	}

	{
		m_pButtonL = CreateTextButton ( "QUEST_MAIN_BUTTON_L", QUEST_MAIN_BUTTON_L, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",0) );        
		m_pButtonM = CreateTextButton ( "QUEST_MAIN_BUTTON_M", QUEST_MAIN_BUTTON_M, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",1) );		
		m_pButtonR = CreateTextButton ( "QUEST_MAIN_BUTTON_R", QUEST_MAIN_BUTTON_R, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",2) );
        
		m_pButtonCOMPLETE = CreateTextButton ( "QUEST_MAIN_BUTTON_COMPLETE", QUEST_MAIN_BUTTON_COMPLETE, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",3) );
		m_pButtonGIFT = CreateTextButton ( "QUEST_MAIN_BUTTON_GIFT", QUEST_MAIN_BUTTON_GIFT, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",4) );
	}

	{
		CQuestGiftView*	pGiftView = new CQuestGiftView;
		pGiftView->CreateSub ( this, "QUEST_GIFT_VIEW", UI_FLAG_DEFAULT, QUEST_MAIN_GIFT_VIEW );
		pGiftView->CreateSubControl ();
		pGiftView->SetVisibleSingle ( FALSE );
		RegisterControl ( pGiftView );
		m_pGiftView = pGiftView;
	}
}

CItemSlot* CQuestMain::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot;
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( GLQUESTPROG::INVEN_SIZEX, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestMain::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestMain::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestMain::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CQuestMain::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible ) m_pGiftView->SetVisibleSingle ( FALSE );
}

void CQuestMain::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	//	NOTE
	//		퀘스트 보상치를 보여줄때는 퀘스트메인에 다른 컨트롤은 업데이트 하지 않는다.
	if ( m_pGiftView->IsVisible () )
	{
		m_pGiftView->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		return ;
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pDescScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pDescText->GetTotalLine ();
		const int nLinePerOneView = m_pDescText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pDescText->SetCurLine ( nPos );			
		}
	}
	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pMainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pMainText->GetTotalLine ();
		const int nLinePerOneView = m_pMainText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );

		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pMainText->SetCurLine ( nPos );			
		}
	}
}

void CQuestMain::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_MAIN_BUTTON_L:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_GIVEUP );
			}
		}
		break;
	case QUEST_MAIN_BUTTON_M:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_GO_QUESTSTEP );
			}
		}
		break;
	case QUEST_MAIN_BUTTON_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_BACK );
			}
		}
		break;
	
	case QUEST_MAIN_BUTTON_COMPLETE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_COMPLETE );
			}
		}
		break;

	case QUEST_MAIN_ITEMSLOT:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nPosX = m_pItemSlot->GetItemIndex ();
				if ( nPosX < 0 ) return ;
				SINVENITEM sInvenItem = GetItem ( nPosX );
				{				
					if ( sInvenItem.sItemCustom.sNativeID != NATIVEID_NULL () )
					{
						CInnerInterface::GetInstance().SHOW_ITEM_INFO_SIMPLE ( sInvenItem.sItemCustom );
					}
				}
			}
		}
		break;

	case QUEST_MAIN_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DUP & dwMsg )
				{
					AddMessageEx ( UIMSG_BUTTON_GO_QUESTSTEP );
				}
			}
		}
		break;

	case QUEST_MAIN_BUTTON_GIFT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pGiftView->SetVisibleSingle ( TRUE );
				m_pGiftView->SetQuestID ( m_dwQuestID );
			}
		}
		break;
	}
}

bool	CQuestMain::LoadQuestMain ( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	m_dwQuestID = dwQuestID;
	
	GLQuestPlay& cQuestPlay = GLGaeaClient::GetInstance().GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );		

	//	설명
	{	
		m_pDescText->SetText( pQuest->m_strCOMMENT.c_str(), NS_UITEXTCOLOR::WHITE );
		m_pDescScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
	}

	//	조건
	{
		m_pCondition->ClearText ();				
		CString strCombine;		
		CString strConditionLine1, strConditionLine2;

		if ( pQuest->m_dwLimitTime )
		{
			const DWORD dwLimitTime = pQuest->m_dwLimitTime;
			const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
			const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

			const float fLapsTime = pQuestProg->m_fLAPSTIME;
			const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// 초->분단위
            const DWORD dwLapsHour = dwLapsTime / dwMINITE_BY_HOUR;
			const DWORD dwLapsMinute = dwLapsTime % dwMINITE_BY_HOUR;

			CString strLimitTime;
			if ( dwLimitHour )
			{
				strLimitTime.Format ( "%d%s %d%s",
                    dwLimitHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
					dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
			}
			else
			{
				strLimitTime.Format ( "%d%s",
					dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
			}

			CString strLapsTime;
			if ( dwLapsHour )
			{
				strLapsTime.Format ( "%d%s %d%s",
                    dwLapsHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
					dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
			}
			else
			{
				strLapsTime.Format ( "%d%s",
					dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
			}

			strConditionLine1.Format ( "%s : [%s/%s]", 
				ID2GAMEWORD("QUEST_MAIN_CONDITION",0), strLapsTime, strLimitTime );			
		}

		if ( pQuest->m_dwLimitPartyMemNum )
		{
			if ( strConditionLine1.GetLength () )
				strCombine.Format ( ", %s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
			else
				strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
			strConditionLine1 += strCombine;			
		}

		//	첫번째 줄
		if ( strConditionLine1.GetLength () )
		{
			strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine1 );
			m_pCondition->AddText ( strCombine, NS_UITEXTCOLOR::ORANGE );
		}

		//	두번째 줄
		if ( pQuest->m_bNonDie )
		{
			strConditionLine2 = ID2GAMEWORD("QUEST_MAIN_CONDITION",2);
			if ( strConditionLine1.GetLength () )	//	윗 라인이 있을 경우
			{
				std::string strCondition = ID2GAMEWORD("QUEST_CONDITION");
				std::string strBlank ( strCondition.size(), ' ' );

				strCombine.Format ( "%s - %s", strBlank.c_str(), strConditionLine2 );				
			}
			else
			{
				strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine2 );				
			}

			m_pCondition->AddText ( strCombine, NS_UITEXTCOLOR::ORANGE );
		}
	}

	//	진행 상황
	{
		enum
		{
			QUEST_CHECK = 0,
			QUEST_CHECK_F
		};

		static	CString strCheckKeyword[] =
		{
			"QUEST_CHECK",
			"QUEST_CHECK_F"
		};

		m_pMainText->ClearText ();
		GLQUEST::VECQSTEP& vecProgStep = pQuest->m_vecProgStep;
		for ( int i = 0; i < (int)vecProgStep.size(); ++i )
		{
			const GLQUEST_STEP& sQuestStep = vecProgStep[i];

			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( i < (int)dwSTEP )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			else if ( i == (int)dwSTEP )
			{
				dwColor = NS_UITEXTCOLOR::GOLD;
			}

			int nIndex = m_pMainText->AddText ( sQuestStep.GetTITLE (), dwColor );
			m_pMainText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		CBasicScrollThumbFrame* const pThumbFrame = m_pMainScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pMainText->GetTotalLine ();
		const int nLinePerOneView = m_pMainText->GetVisibleLine ();		
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );


		m_pMainText->SetCurLine ( 0 );
		pThumbFrame->SetPercent ( 0.0f );
		const int nCount = m_pMainText->GetCount () - 1;
		if ( nLinePerOneView < nTotalLine )
		{			
			float fPercent = (float)dwSTEP / (float)nCount;
			pThumbFrame->SetPercent ( fPercent );
		}
	}

	//	아이템 설정
	{
		//	클리어
		for ( int i = 0; i < GLQUESTPROG::INVEN_SIZEX; ++i )
		{
			UnLoadItem ( i );
		}

		GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList ();

		if ( ItemList->size () > (GLQUESTPROG::INVEN_SIZEX * GLQUESTPROG::INVEN_SIZEY) )
		{
			GASSERT ( 0 && "인터페이스 사이즈보다, 데이타크기가 큽니다." );
			return false;
		}

		GLInventory::CELL_MAP_ITER iter = ItemList->begin();
		GLInventory::CELL_MAP_ITER iter_end = ItemList->end();

		for ( ; iter!=iter_end; ++iter )
		{
			SINVENITEM* pInvenItem = (*iter).second;
			SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;
			SITEM* pItemData = GLItemMan::GetInstance().GetItem ( ref_ItemCustom.sNativeID );
			if ( !pItemData )	continue;

			LoadItem ( *pInvenItem );
		}
	}

	//	우선순위가 중요함.
	//	완료가 포기에 우선한다.
	//	만약, 퀘스트가 완료되었다면, 포기는 뿌릴 필요가 없다.
	m_pButtonCOMPLETE->SetVisibleSingle ( FALSE );
	m_pButtonL->SetVisibleSingle ( FALSE );
	
	if ( pQuestProg->CheckCOMPLETE() )	//	완료버튼을 눌러야 하는 퀘스트
	{		
		m_pButtonCOMPLETE->SetVisibleSingle ( TRUE );
		return true;
	}	
	else if ( pQuest->IsGIVEUP () )	//	포기 퀘스트
	{
		m_pButtonL->SetVisibleSingle ( TRUE );
		return true;
	}

	return true;
}

void CQuestMain::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
//	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlot->SetItemImage ( nPosX, ref_InvenItem );
}


SINVENITEM& CQuestMain::GetItem ( int nPosX )
{
	return m_pItemSlot->GetItemImage ( nPosX );
}

void CQuestMain::UnLoadItem ( int nPosX )
{
	m_pItemSlot->ResetItemImage ( nPosX );
}