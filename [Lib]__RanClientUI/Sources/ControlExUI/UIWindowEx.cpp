#include "pch.h"
#include "./UIWindowEx.h"

#include "./InnerInterface.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "UITextControl.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIWindowEx::CUIWindowEx ()
{
}

CUIWindowEx::~CUIWindowEx ()
{
}

void CUIWindowEx::CreateBaseWindowBlack ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );

#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined(JP_PARAM) || defined(CH_PARAM) || defined ( HK_PARAM )
	CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
#endif

	CreateCloseButton ( "BASIC_WINDOW_CLOSE", "BASIC_WINDOW_CLOSE_F", "BASIC_WINDOW_CLOSE_OVER" );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_BLACK", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowLightGray ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT", "BASIC_WINDOW_TITLE_MID", "BASIC_WINDOW_TITLE_RIGHT", "BASIC_WINDOW_TEXTBOX", szWindowName );

#if defined(RZ_PARAM) || defined(KRT_PARAM) || defined(KR_PARAM) || defined(TW_PARAM) || defined(JP_PARAM) || defined(CH_PARAM) || defined ( HK_PARAM ) 
	CreateTitleFocus ( "BASIC_WINDOW_TITLE", "BASIC_WINDOW_TITLE_LEFT_F", "BASIC_WINDOW_TITLE_MID_F", "BASIC_WINDOW_TITLE_RIGHT_F", "BASIC_WINDOW_TEXTBOX", szWindowName );
#endif

	CreateCloseButton ( "BASIC_WINDOW_CLOSE", "BASIC_WINDOW_CLOSE_F", "BASIC_WINDOW_CLOSE_OVER" );
	CreateBody ( "BASIC_WINDOW_BODY", "BASIC_WINDOW_BODY_LEFT", "BASIC_WINDOW_BODY_UP", "BASIC_WINDOW_BODY_MAIN_LIGHTGRAY", "BASIC_WINDOW_BODY_DOWN", "BASIC_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::CreateBaseWindowMiniParty ( char* szWindowKeyword, char* szWindowName )
{
	CreateTitle ( "MINIPARTY_WINDOW_TITLE", "MINIPARTY_WINDOW_TITLE_LEFT", "MINIPARTY_WINDOW_TITLE_MID", "MINIPARTY_WINDOW_TITLE_RIGHT", "MINIPARTY_WINDOW_TEXTBOX", szWindowName );
	CreateCloseButton ( "MINIPARTY_WINDOW_CLOSE", "MINIPARTY_WINDOW_CLOSE_F", "MINIPARTY_WINDOW_CLOSE_OVER" );
	CreateBody ( "MINIPARTY_WINDOW_BODY", "MINIPARTY_WINDOW_BODY_LEFT", "MINIPARTY_WINDOW_BODY_UP", "MINIPARTY_WINDOW_BODY_MAIN", "MINIPARTY_WINDOW_BODY_DOWN", "MINIPARTY_WINDOW_BODY_RIGHT" );
	ResizeControl ( szWindowKeyword );

    WORD wFlag = GetAlignFlag ();
	wFlag &= ~UI_FLAG_XSIZE;
	wFlag &= ~UI_FLAG_YSIZE;
	SetAlignFlag ( wFlag );
}

void CUIWindowEx::ResizeControl ( char* szWindowKeyword )
{
    CUIControl TempControl; 
	TempControl.Create ( 1, szWindowKeyword ); // MEMO
	const UIRECT& rcParentOldPos = GetLocalPos ();
	const UIRECT& rcParentNewPos = TempControl.GetLocalPos ();
	AlignSubControl ( rcParentOldPos, rcParentNewPos );

	SetLocalPos ( D3DXVECTOR2 ( rcParentNewPos.left, rcParentNewPos.top ) );
}

CUIControl*	CUIWindowEx::CreateControl ( const char* szControl, const UIGUID& cID )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl, UI_FLAG_DEFAULT, cID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicButton* CUIWindowEx::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, wFlipType );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CUIWindowEx::CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox;
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CUIWindowEx::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindow::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;
	}
}