#include "pch.h"
#include "./receivenotewindow.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "BasicTextButton.h"
#include "d3dfont.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "BasicLineBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CReceiveNoteWindow::CReceiveNoteWindow(void)
	: m_pFromNameTextBox(NULL)
	, m_pSubjectNameTextBox(NULL)
	, m_pTextTextBox(NULL)
	, m_pReplyButton(NULL)
{
}

CReceiveNoteWindow::~CReceiveNoteWindow(void)
{
}

void CReceiveNoteWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "RECEIVENOTE_FROM_TEXTBOX" );
	pTextBox->SetFont( pFont9 );
	pTextBox->AddText( ID2GAMEWORD("NOTE_WINDOW",0) );
	RegisterControl( pTextBox );

	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "RECEIVENOTE_FROM_NAME_TEXTBOX", UI_FLAG_DEFAULT, RECEIVENOTE_FROM_NAME_TEXTBOX );
	pTextBox->SetFont( pFont9 );
	RegisterControl( pTextBox );
	m_pFromNameTextBox = pTextBox;

	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "RECEIVENOTE_SUBJECT_TEXTBOX" );
	pTextBox->SetFont( pFont9 );
	pTextBox->AddText( ID2GAMEWORD("NOTE_WINDOW",2) );
	RegisterControl( pTextBox );

	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "RECEIVENOTE_SUBJECT_NAME_TEXTBOX", UI_FLAG_DEFAULT, RECEIVENOTE_SUBJECT_NAME_TEXTBOX );
	pTextBox->SetFont( pFont9 );
	RegisterControl( pTextBox );
	m_pSubjectNameTextBox = pTextBox;

	CBasicLineBox* pLineBox = new CBasicLineBox;
	pLineBox->CreateSub( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxSystemMessageUp( "RECEIVE_NOTE_LINE_TEXTBOX" );
	pLineBox->ResetAlignFlag ();
	RegisterControl( pLineBox );

	pTextBox = new CBasicTextBox;
	pTextBox->CreateSub( this, "RECEIVENOTE_TEXT_TEXTBOX", UI_FLAG_DEFAULT, RECEIVENOTE_TEXT_TEXTBOX );
	pTextBox->SetFont( pFont9 );
	RegisterControl( pTextBox );
	m_pTextTextBox = pTextBox;

	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, RECEIVENOTE_REPLY_BUTTON );
	pButton->CreateBaseButton( "RECEIVENOTE_REPLY_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",7) );
	RegisterControl( pButton );
	m_pReplyButton = pButton;
}

void CReceiveNoteWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case RECEIVENOTE_REPLY_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
				CInnerInterface::GetInstance().ShowGroupFocus( WRITE_NOTE_WINDOW );
			}
		}
		break;
	}
}