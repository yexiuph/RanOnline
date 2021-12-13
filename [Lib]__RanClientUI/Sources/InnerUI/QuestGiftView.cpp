#include "pch.h"
#include "QuestGiftView.h"
#include "BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "GLQuestMan.h"
#include "GLQuest.h"
#include "GameTextControl.h"
#include "UITextControl.h"
#include "GLItemMan.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CQuestGiftView::CQuestGiftView ()
{
}

CQuestGiftView::~CQuestGiftView ()
{
}

void	CQuestGiftView::CreateSubControl ()
{
	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessage ( "QUEST_GIFT_LINE_BOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );

		// MEMO : 너무 투명해서 하나를 더 만듬. 이 부분은 수정이 필요함.
		pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_SYSTEM_MESSAGE", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessage ( "QUEST_GIFT_LINE_BOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

	pLineBox = new CBasicLineBox;
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessageUp ( "QUEST_GIFT_LINE_TEXTBOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl ( pLineBox );

		pLineBox = new CBasicLineBox;
		pLineBox->CreateSub ( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp ( "QUEST_GIFT_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl ( pLineBox );

	CBasicTextBox* pMapMoveTextBox = new CBasicTextBox;
	pMapMoveTextBox->CreateSub ( this, "QUEST_GIFT_TEXTBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pMapMoveTextBox->SetFont ( pFont );
	pMapMoveTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X );
	pMapMoveTextBox->ResetAlignFlag ();
	RegisterControl ( pMapMoveTextBox );
	m_pTextBox = pMapMoveTextBox;

	m_pOK = CreateTextButton ( "QUEST_GIFT_VIEW_OK", QUEST_GIFT_VIEW_OK, (char*)ID2GAMEWORD ( "MODAL_BUTTON", 0 ) );
}

void	CQuestGiftView::SetQuestID ( const DWORD& dwQuestID )
{
	m_pTextBox->ClearText ();

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

	{
		m_pTextBox->AddText ( ID2GAMEWORD("QUEST_GIFT",9), NS_UITEXTCOLOR::GREENYELLOW );

		CString strCombine;

		if ( pQuest->m_dwGiftEXP )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",0), pQuest->m_dwGiftEXP );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
		if ( pQuest->m_dwGiftMONEY )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",1), pQuest->m_dwGiftMONEY );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
		if ( pQuest->m_dwGiftELEMENT )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",2), pQuest->m_dwGiftELEMENT );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
		if ( pQuest->m_dwGiftLIFEPOINT )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",3), pQuest->m_dwGiftLIFEPOINT );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
		//if ( pQuest->m_dwGiftDisPK )
		//{
		//	strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",4), pQuest->m_dwGiftDisPK );
		//	m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		//}
		if ( pQuest->m_dwGiftSKILLPOINT )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",5), pQuest->m_dwGiftSKILLPOINT );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
		}
		if ( pQuest->m_dwGiftSTATSPOINT )
		{
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_GIFT",6), pQuest->m_dwGiftSTATSPOINT );
			m_pTextBox->AddText ( strCombine, NS_UITEXTCOLOR::WHITE );
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
			m_pTextBox->AddText ( strItem, NS_UITEXTCOLOR::WHITE );
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
			m_pTextBox->AddText ( strSkill, NS_UITEXTCOLOR::WHITE );
		}
	}
}

CBasicTextButton* CQuestGiftView::CreateTextButton ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void	CQuestGiftView::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CQuestGiftView::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case QUEST_GIFT_VIEW_OK:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				SetVisibleSingle ( FALSE );
			}
		}
		break;
	}
}