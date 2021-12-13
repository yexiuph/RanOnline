#include "pch.h"
#include "./UIOuterWindow.h"
#include "./OuterInterface.h"
#include "./BasicLineBox.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIOuterWindow::CUIOuterWindow ()
{
}

CUIOuterWindow::~CUIOuterWindow ()
{
}

void CUIOuterWindow::CreateBaseWidnow ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "OUTER_WINDOW_TITLE", "OUTER_WINDOW_TITLE_LEFT", "OUTER_WINDOW_TITLE_MID", "OUTER_WINDOW_TITLE_RIGHT", "OUTER_WINDOW_TEXTBOX", szWindowName );
	CreateBody ( "OUTER_WINDOW_BODY", "OUTER_WINDOW_BODY_LEFT", "OUTER_WINDOW_BODY_UP", "OUTER_WINDOW_BODY_MAIN", "OUTER_WINDOW_BODY_DOWN", "OUTER_WINDOW_BODY_RIGHT" );	
	
	ResizeControl ( szWindowKeyword );

//	SetTitleAlign ( CBasicTextBox::CENTER );
}

CBasicTextBox*	CUIOuterWindow::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

CUIControl*	CUIOuterWindow::CreateControl ( char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );
	return pControl;
}


void CUIOuterWindow::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl;
	TempControl.Create ( 1, szWindowKeyword );
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

void CUIOuterWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				COuterInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;
	}
}