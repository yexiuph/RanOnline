#include "pch.h"
#include "BasicLineBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicLineBox::CBasicLineBox () :
	m_pLeft( NULL ),
	m_pRight( NULL ),
	m_pTop( NULL ),
	m_pBottom( NULL )
{
}

CBasicLineBox::~CBasicLineBox ()
{
}

void CBasicLineBox::ResizeControl ( char* szBoxControl )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szBoxControl );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CBasicLineBox::CreateBaseBoxOption ( char* szBoxControl )
{	
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_OPTION" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_OPTION", "BASIC_LINE_BOX_RIGHT_OPTION", "BASIC_LINE_BOX_TOP_OPTION", "BASIC_LINE_BOX_BOTTOM_OPTION" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxSkill ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_SKILL" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_SKILL", "BASIC_LINE_BOX_RIGHT_SKILL", "BASIC_LINE_BOX_TOP_SKILL", "BASIC_LINE_BOX_BOTTOM_SKILL" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxCombo ( char* szBoxControl )
{	
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_COMBO" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_COMBO", "BASIC_LINE_BOX_RIGHT_COMBO", "BASIC_LINE_BOX_TOP_COMBO", "BASIC_LINE_BOX_BOTTOM_COMBO" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxMiniParty ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_MINIPARTY" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_MINIPARTY", "BASIC_LINE_BOX_RIGHT_MINIPARTY", "BASIC_LINE_BOX_TOP_MINIPARTY", "BASIC_LINE_BOX_BOTTOM_MINIPARTY" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxModal ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_MODAL" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_MODAL", "BASIC_LINE_BOX_RIGHT_MODAL", "BASIC_LINE_BOX_TOP_MODAL", "BASIC_LINE_BOX_BOTTOM_MODAL" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxDialogue ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_DIALOGUE" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_DIALOGUE", "BASIC_LINE_BOX_RIGHT_DIALOGUE", "BASIC_LINE_BOX_TOP_DIALOGUE", "BASIC_LINE_BOX_BOTTOM_DIALOGUE" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxOuter ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_OUTER" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_OUTER", "BASIC_LINE_BOX_RIGHT_OUTER", "BASIC_LINE_BOX_TOP_OUTER", "BASIC_LINE_BOX_BOTTOM_OUTER" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxOuterFat ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_OUTER_FAT" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_OUTER_FAT", "BASIC_LINE_BOX_RIGHT_OUTER_FAT", "BASIC_LINE_BOX_TOP_OUTER_FAT", "BASIC_LINE_BOX_BOTTOM_OUTER_FAT" );
	ResizeControl ( szBoxControl );
}

void  CBasicLineBox::CreateBaseBoxQuestList ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_QUEST_LIST" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_QUEST_LIST", "BASIC_LINE_BOX_RIGHT_QUEST_LIST", "BASIC_LINE_BOX_TOP_QUEST_LIST", "BASIC_LINE_BOX_BOTTOM_QUEST_LIST" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxQuestStepDesc ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_QUEST_MAIN_DESC" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_QUEST_MAIN_DESC", "BASIC_LINE_BOX_RIGHT_QUEST_MAIN_DESC", "BASIC_LINE_BOX_TOP_QUEST_MAIN_DESC", "BASIC_LINE_BOX_BOTTOM_QUEST_MAIN_DESC" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxQuestStepItem ( char* szBoxControl )
{
//	CreateBodyImage ( "BASIC_LINE_BOX_BODY_QUEST_MAIN_ITEM" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_QUEST_MAIN_ITEM", "BASIC_LINE_BOX_RIGHT_QUEST_MAIN_ITEM", "BASIC_LINE_BOX_TOP_QUEST_MAIN_ITEM", "BASIC_LINE_BOX_BOTTOM_QUEST_MAIN_ITEM" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxQuestRunDesc ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_QUEST_STEP_DESC" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_QUEST_STEP_DESC", "BASIC_LINE_BOX_RIGHT_QUEST_STEP_DESC", "BASIC_LINE_BOX_TOP_QUEST_STEP_DESC", "BASIC_LINE_BOX_BOTTOM_QUEST_STEP_DESC" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxEditBox ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_EDIT" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_EDIT", "BASIC_LINE_BOX_RIGHT_EDIT", "BASIC_LINE_BOX_TOP_EDIT", "BASIC_LINE_BOX_BOTTOM_EDIT" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxQuestRunItem ( char* szBoxControl )
{
//	CreateBodyImage ( "BASIC_LINE_BOX_BODY_QUEST_STEP_ITEM" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_QUEST_STEP_ITEM", "BASIC_LINE_BOX_RIGHT_QUEST_STEP_ITEM", "BASIC_LINE_BOX_TOP_QUEST_STEP_ITEM", "BASIC_LINE_BOX_BOTTOM_QUEST_STEP_ITEM" );	
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::SetUseRenderLine ( int nLine, BOOL bUseRender )
{
	switch ( nLine )
	{
	case LEFT:
		{
			if ( m_pLeft )	m_pLeft->SetUseRender ( bUseRender );
		}
		break;

	case RIGHT:
		{
			if ( m_pRight )	m_pRight->SetUseRender ( bUseRender );
		}		
		break;

	case TOP:
		{
			if ( m_pTop )	m_pTop->SetUseRender ( bUseRender );
		}
		break;

	case BOTTOM:
		{
			if ( m_pBottom )m_pBottom->SetUseRender ( bUseRender );
		}
		break;
	}
}

void CBasicLineBox::ResetAlignFlagLine ()
{
	if ( m_pLeft )	m_pLeft->ResetAlignFlag ();
	if ( m_pRight )	m_pRight->ResetAlignFlag ();
	if ( m_pTop )	m_pTop->ResetAlignFlag ();
	if ( m_pBottom )m_pBottom->ResetAlignFlag ();
}

void CBasicLineBox::SetAlignFlagLine ( int nLine, WORD wAlignFlag )
{
	switch ( nLine )
	{
	case LEFT:
		{
			if ( m_pLeft )	m_pLeft->SetAlignFlag ( wAlignFlag );
		}
		break;

	case RIGHT:
		{
			if ( m_pRight )	m_pRight->SetAlignFlag ( wAlignFlag );
		}		
		break;

	case TOP:
		{
			if ( m_pTop )	m_pTop->SetAlignFlag ( wAlignFlag );
		}
		break;

	case BOTTOM:
		{
			if ( m_pBottom )m_pBottom->SetAlignFlag ( wAlignFlag );
		}
		break;
	}
}

void CBasicLineBox::CreateLineImage ( char* szLeft, char* szRight, char* szTop, char* szBottom )
{
	m_pLeft = CreateControl ( szLeft, UI_FLAG_YSIZE );
	m_pRight = CreateControl ( szRight, UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	m_pTop = CreateControl ( szTop, UI_FLAG_XSIZE );
	m_pBottom = CreateControl ( szBottom, UI_FLAG_BOTTOM | UI_FLAG_XSIZE );
}

void CBasicLineBox::CreateBodyImage ( char* szBody )
{
	CUIControl* pBody = new CUIControl;
	pBody->CreateSub ( this, szBody, UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	RegisterControl ( pBody );
}


CUIControl*	CBasicLineBox::CreateControl ( char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl, wAlignFlag );
	RegisterControl ( pControl );
	return pControl;
}

void	CBasicLineBox::CreateBaseBoxMapMove ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_MAPMOVE" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_MAPMOVE", "BASIC_LINE_BOX_RIGHT_MAPMOVE", "BASIC_LINE_BOX_TOP_MAPMOVE", "BASIC_LINE_BOX_BOTTOM_MAPMOVE" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxWaitServer ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_WAITSERVER" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_WAITSERVER", "BASIC_LINE_BOX_RIGHT_WAITSERVER", "BASIC_LINE_BOX_TOP_WAITSERVER", "BASIC_LINE_BOX_BOTTOM_WAITSERVER" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxDialogueAnswer ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_WAITSERVER" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_WAITSERVER", "BASIC_LINE_BOX_RIGHT_WAITSERVER", "BASIC_LINE_BOX_TOP_WAITSERVER", "BASIC_LINE_BOX_BOTTOM_WAITSERVER" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxSystemMessage ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_SYSTEM_BODY_MAIN_LIGHTGRAY" );
	CreateLineImage ( "BASIC_LINE_BOX_LEFT_SYSTEM_MESSAGE", "BASIC_LINE_BOX_RIGHT_SYSTEM_MESSAGE", "BASIC_LINE_BOX_TOP_SYSTEM_MESSAGE", "BASIC_LINE_BOX_BOTTOM_SYSTEM_MESSAGE" );
	ResizeControl ( szBoxControl );
}

void CBasicLineBox::CreateBaseBoxSystemMessageUp ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_BODY_MINIPARTY" );
//	CreateLineImage ( "BASIC_LINE_BOX_LEFT_MINIPARTY", "BASIC_LINE_BOX_RIGHT_MINIPARTY", "BASIC_LINE_BOX_TOP_MINIPARTY", "BASIC_LINE_BOX_BOTTOM_MINIPARTY" );
	ResizeControl ( szBoxControl );
}