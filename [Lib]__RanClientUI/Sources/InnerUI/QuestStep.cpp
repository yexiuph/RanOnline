#include "pch.h"
#include "QuestStep.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicLineBox.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "ItemSlot.h"
#include "BasicTextButton.h"
#include "GLQuestMan.h"
#include "GLQuest.h"
#include "GLQuestPlay.h"
#include "GLGaeaClient.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const	float	CQuestStep::fANSWER_LINE_INTERVAL = 5.0f;

CQuestStep::CQuestStep ()
{
}

CQuestStep::~CQuestStep ()
{
}

void	CQuestStep::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	
	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_STEP_DESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxQuestRunDesc ( "QUEST_STEP_DESC_BACK" );
	RegisterControl ( pBasicLineBox );

	//	텍스트 박스
	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "QUEST_STEP_DESC_TEXTBOX", UI_FLAG_DEFAULT, QUEST_STEP_DESC_TEXTBOX );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
		RegisterControl ( pTextBox );
		m_pDescText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_STEP_DESC_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_STEP_DESC_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pDescScrollBar = pScrollBar;
	}

	{
		//	텍스트 박스
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "QUEST_STEP_TEXTBOX", UI_FLAG_DEFAULT, QUEST_STEP_TEXTBOX );
		pTextBox->SetFont ( pFont9 );
		pTextBox->SetLineInterval ( fANSWER_LINE_INTERVAL );
		RegisterControl ( pTextBox );
		m_pRunText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, QUEST_STEP_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "QUEST_STEP_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pRunScrollBar = pScrollBar;
	}

	{	//	아이템 타이틀 BACK 이미지
		CreateControl ( "QUEST_STEP_ITEM_TITLE_BACK_L" );
		CreateControl ( "QUEST_STEP_ITEM_TITLE_BACK_R" );

		CBasicTextBox* pTitle = CreateStaticControl ( "QUEST_STEP_ITEM_TITLE", pFont9, NS_UITEXTCOLOR::DEFAULT, TEXT_ALIGN_CENTER_X );
		pTitle->AddText ( ID2GAMEWORD("QUEST_STEP_ITEM_TITLE") );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_STEP_ITEM", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestRunItem ( "QUEST_STEP_ITEM_BACK" );
		RegisterControl ( pBasicLineBox );

		m_pItemSlot = CreateItemSlot ( "QUEST_STEP_ITEMSLOT", QUEST_STEP_ITEMSLOT );
	}

	{
//		m_pButtonL = CreateTextButton ( "QUEST_STEP_BUTTON_L", QUEST_STEP_BUTTON_L, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",0) );
		m_pButtonR = CreateTextButton ( "QUEST_STEP_BUTTON_R", QUEST_STEP_BUTTON_R, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",1) );
		m_pButtonPrint = CreateTextButton ( "QUEST_STEP_BUTTON_PRINT", QUEST_STEP_BUTTON_PRINT, (char*)ID2GAMEWORD("QUEST_STEP_BUTTON",2) );

		m_pButtonCOMPLETE = CreateTextButton ( "QUEST_MAIN_BUTTON_COMPLETE", QUEST_MAIN_BUTTON_COMPLETE, (char*)ID2GAMEWORD("QUEST_MAIN_BUTTON",3) );
	}
}

CItemSlot* CQuestStep::CreateItemSlot ( CString strKeyword, UIGUID ControlID )
{
	CItemSlot* pItemSlot = new CItemSlot;
	pItemSlot->CreateSub ( this, strKeyword.GetString (), UI_FLAG_DEFAULT, ControlID );
	pItemSlot->CreateSubControl ( 8, TRUE );
	RegisterControl ( pItemSlot );
	return pItemSlot;
}

CBasicTextButton* CQuestStep::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

CUIControl*	CQuestStep::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


CBasicTextBox* CQuestStep::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

bool	CQuestStep::LoadQuestStep ( DWORD dwQuestID )
{
	if ( dwQuestID == NATIVEID_NULL().dwID ) return false;

	GLQuestPlay& cQuestPlay = GLGaeaClient::GetInstance().GetCharacter ()->m_cQuestPlay;
	GLQUESTPROG* pQuestProg = cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg ) return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) return false;	

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

	//	설명
	{
		m_pDescText->SetText ( pQuestStep->GetCOMMENT (), NS_UITEXTCOLOR::WHITE );
		m_pDescScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
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

		m_pRunText->ClearText ();
		if ( pQuestStep->IsNEED_NPCTALK() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bTALK )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}

			int nIndex = m_pRunText->AddText ( pQuestStep->m_strOBJ_NPCTALK.c_str (), dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}
		
		if ( pQuestStep->IsNEED_QITEM() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bQITEM )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			int nIndex = m_pRunText->AddText ( pQuestStep->m_strOBJ_MOBGEN_QITEM.c_str (), dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		if ( pQuestStep->IsNEED_MOBKILL() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::GOLD;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bMOBKILL_ALL )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}

			CString strCombine;
			strCombine.Format ( "%s [%d/%d]",
				pQuestStep->m_strOBJ_MOBKILL.c_str (),
				sSTEP_PROG.m_dwNUM_MOBKILL,
				pQuestStep->m_dwNUM_MOBKILL );
			int nIndex = m_pRunText->AddText ( strCombine, dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		if ( pQuestStep->IsNEED_NPCGUARD() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bNPCGUARD_DEATH )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			int nIndex = m_pRunText->AddText ( pQuestStep->m_strOBJ_NPCGUARD.c_str (), dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}
		
		if ( pQuestStep->IsNEED_REACHZONE() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bREACH_ZONE )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			int nIndex = m_pRunText->AddText ( pQuestStep->m_strOBJ_REACH_ZONE.c_str (), dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
		}

		if ( pQuestStep->IsNEED_DEFENSEZONE() )
		{
			D3DCOLOR dwColor = NS_UITEXTCOLOR::WHITE;
			int nImageType = QUEST_CHECK;
			if ( sSTEP_PROG.m_bDEFENSE_ZONE_COMPLETE )
			{
				dwColor = NS_UITEXTCOLOR::DARKGRAY;
				nImageType = QUEST_CHECK_F;
			}
			int nIndex = m_pRunText->AddText ( pQuestStep->m_strOBJ_DEFENSE_ZONE.c_str (), dwColor );
			m_pRunText->SetTextImage ( nIndex, strCheckKeyword[nImageType] );
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
	m_pButtonCOMPLETE->SetVisibleSingle ( FALSE );	
	
	if ( pQuestProg->CheckCOMPLETE() )	//	완료버튼을 눌러야 하는 퀘스트
	{		
		m_pButtonCOMPLETE->SetVisibleSingle ( TRUE );
		m_pButtonPrint->SetVisibleSingle( FALSE );
		return true;
	}
	else
	{
		if( pQuestStep->IsNEED_QITEM() || pQuestStep->IsNEED_MOBKILL() ) m_pButtonPrint->SetVisibleSingle( TRUE );
		else	m_pButtonPrint->SetVisibleSingle( FALSE );

	}

	return true;
}

void	CQuestStep::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case QUEST_STEP_BUTTON_R:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_BACK_LBUPLIKE );
			}
		}
		break;

	case QUEST_MAIN_BUTTON_COMPLETE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_COMPLETE_STEP );
			}
		}
		break;

	case QUEST_STEP_BUTTON_PRINT:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				AddMessageEx ( UIMSG_BUTTON_PRINT_STEP );
			}
		}
		break;
	case QUEST_STEP_ITEMSLOT:
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
	}
}


void CQuestStep::LoadItem ( SINVENITEM& ref_InvenItem )
{	
	int nPosX = ref_InvenItem.wPosX;
//	int nPosY = ref_InvenItem.wPosY;
	m_pItemSlot->SetItemImage ( nPosX, ref_InvenItem );
}

SINVENITEM& CQuestStep::GetItem ( int nPosX )
{
	return m_pItemSlot->GetItemImage ( nPosX );
}


void CQuestStep::UnLoadItem ( int nPosX )
{
	m_pItemSlot->ResetItemImage ( nPosX );
}