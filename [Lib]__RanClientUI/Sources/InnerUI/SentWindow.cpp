#include "pch.h"
#include ".\sentwindow.h"
#include "d3dfont.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "FriendWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSentWindow::CSentWindow(void)
	: m_pListText(NULL)
	, m_pListScrollBar(NULL)
	, m_pOpenButton(NULL)
	, m_pDelButton(NULL)
	, m_pReloadButton(NULL)
{
}

CSentWindow::~CSentWindow(void)
{
}

CUIControl*	CSentWindow::CreateControl ( char* szControl, const UIGUID& cID )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl, UI_FLAG_DEFAULT, cID );
	RegisterControl ( pControl );
	return pControl;
}

CBasicTextButton* CSentWindow::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CSentWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{	// 이름
		CreateControl( "SENT_NAMEBACK_L" );
		CreateControl( "SENT_NAMEBACK_R" );

		CBasicTextBox* pStaticText = new CBasicTextBox;
		pStaticText->CreateSub( this, "SENT_NAME_TEXT" );
		pStaticText->SetFont( pFont9 );
		pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X );
		pStaticText->AddText( ID2GAMEWORD("FRIEND_BUTTON",5) );
		RegisterControl ( pStaticText );
		
		// 제목
		CreateControl( "SENT_TITLEBACK_L" );
		CreateControl( "SENT_TITLEBACK_R" );

		pStaticText = new CBasicTextBox;
		pStaticText->CreateSub( this, "SENT_TITLE_TEXT" );
		pStaticText->SetFont( pFont9 );
		pStaticText->SetTextAlign( TEXT_ALIGN_CENTER_X );
		pStaticText->AddText( ID2GAMEWORD("FRIEND_BUTTON",6) );
		RegisterControl ( pStaticText );
	}

	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub( this, "SENT_LIST_TEXTBOX", UI_FLAG_DEFAULT, SENT_LIST_TEXTBOX );
		pTextBox->SetFont( pFont9 );		
		pTextBox->SetLineInterval( 3.0f );
		pTextBox->SetSensitive( true );
		pTextBox->SetLimitLine( 10000 );
		RegisterControl( pTextBox );
		m_pListText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, SENT_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar( "SENT_LIST_SCROLLBAR" );
		pScrollBar->GetThumbFrame()->SetState( 1, nTotalLine );
		RegisterControl( pScrollBar );
		m_pListScrollBar = pScrollBar;
	}

	m_pOpenButton	= CreateTextButton14( "SENT_OPEN_BUTTON", SENT_OPEN_BUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",8) );
	m_pDelButton	= CreateTextButton14( "SENT_DEL_BUTTON", SENT_DEL_BUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",1) );
	m_pReloadButton	= CreateTextButton14( "SENT_RELOAD_BUTTON", SENT_RELOAD_BUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",9) );
}

void CSentWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( cID, dwMsg );

	switch( cID )
	{
	case SENT_LIST_TEXTBOX:
		{
		}
		break;

	case SENT_OPEN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GetParent()->AddMessageEx( UIMSG_RECEIVE_NOTE_VISIBLE );
			}
		}
		break;

	case SENT_DEL_BUTTON:
		{
		}
		break;

	case SENT_RELOAD_BUTTON:
		{
		}
		break;
	}
}