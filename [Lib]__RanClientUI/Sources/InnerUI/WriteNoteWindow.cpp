#include "pch.h"
#include "./writenotewindow.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIMultiEditBox.h"
#include "d3dfont.h"
#include "GameTextControl.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "FriendWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CWriteNoteWindow::nLIMIT_CHAR = 30;

CWriteNoteWindow::CWriteNoteWindow(void)
	: m_pSubjectEditBox(NULL)
	, m_pMultiEditBox(NULL)
	, m_pRewriteButton(NULL)
	, m_pSendButton(NULL)
	, m_pNameTextBox(NULL)
{
}

CWriteNoteWindow::~CWriteNoteWindow(void)
{
}

void CWriteNoteWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub( this, "WRITENOTE_TO_TEXTBOX" );
		pTextBox->SetFont( pFont9 );
		pTextBox->AddText( ID2GAMEWORD("NOTE_WINDOW",1) );
		RegisterControl( pTextBox );

		pTextBox = new CBasicTextBox;
		pTextBox->CreateSub( this, "WRITENOTE_TO_NAME_TEXTBOX" );
		pTextBox->SetFont( pFont9 );		
		RegisterControl( pTextBox );
		m_pNameTextBox = pTextBox;
	}

	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub( this, "WRITENOTE_SUBJECT_TEXTBOX" );
		pTextBox->SetFont( pFont9 );
		pTextBox->AddText( ID2GAMEWORD("NOTE_WINDOW",2) );
		RegisterControl( pTextBox );

		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxEditBox ( "WRITE_NOTE_BOX_BACK" );
		RegisterControl ( pBasicLineBox );

		CUIEditBox* pEditBox = new CUIEditBox;
		pEditBox->CreateSub( this, "WRITENOTE_SUBJECT_EDITBOX", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, WRITENOTE_SUBJECT_EDITBOX );
		pEditBox->CreateCarrat( "WRITE_NOTE_CARRAT", TRUE, UINT_MAX );
		pEditBox->SetLimitInput( nLIMIT_CHAR );
		pEditBox->SetFont( pFont9 );		
		RegisterControl( pEditBox );
		m_pSubjectEditBox = pEditBox;
	}

	{
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp( "WRITE_NOTE_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl( pLineBox );
	}

	{
		CUIMultiEditBox* pMultiEditBox = new CUIMultiEditBox;
		pMultiEditBox->CreateSub( this, "WRITE_NOTE_EDITBOX" );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L01, "WRITENOTE_EDITBOX_L01", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L02, "WRITENOTE_EDITBOX_L02", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L03, "WRITENOTE_EDITBOX_L03", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L04, "WRITENOTE_EDITBOX_L04", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L05, "WRITENOTE_EDITBOX_L05", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L06, "WRITENOTE_EDITBOX_L06", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L07, "WRITENOTE_EDITBOX_L07", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L08, "WRITENOTE_EDITBOX_L08", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L09, "WRITENOTE_EDITBOX_L09", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( WRITENOTE_EDITBOX_L10, "WRITENOTE_EDITBOX_L10", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		RegisterControl( pMultiEditBox );
		m_pMultiEditBox = pMultiEditBox;
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, WRITENOTE_REWRITE_BUTTON );
		pButton->CreateBaseButton( "WRITENOTE_REWRITE_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",11) );
		RegisterControl( pButton );
		m_pRewriteButton = pButton;

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, WRITENOTE_SEND_BUTTON );
		pButton->CreateBaseButton( "WRITENOTE_SEND_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",10) );
		RegisterControl( pButton );
		m_pSendButton = pButton;
	}
}

void CWriteNoteWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case WRITENOTE_SUBJECT_EDITBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					CString strName = m_pSubjectEditBox->GetEditString ();
					EDIT_BEGIN( strName );				
				}
			}
			else
			{
				if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
				{
					EDIT_END();
				}
			}
		}
		break;

	case WRITENOTE_REWRITE_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pMultiEditBox->ClearEdit();
				m_pMultiEditBox->EndEdit();
				m_pMultiEditBox->Init();
				m_pMultiEditBox->BeginEdit();
			}
		}
		break;

	case WRITENOTE_SEND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
			}
		}
		break;
	}
}

void CWriteNoteWindow::EDIT_BEGIN( const CString& strName )
{
	if( m_pSubjectEditBox )
	{
		m_pSubjectEditBox->EndEdit ();
		m_pSubjectEditBox->ClearEdit ();

		m_pSubjectEditBox->SetEditString ( strName );
		m_pSubjectEditBox->BeginEdit ();	
	}
}

void CWriteNoteWindow::EDIT_END()
{
	if( m_pSubjectEditBox )
		m_pSubjectEditBox->EndEdit();
}

void CWriteNoteWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		const CString& strName = CInnerInterface::GetInstance().GetFriendWindow()->GET_FRIEND_NAME();
		m_pNameTextBox->AddText( strName );
	}
	else
	{
		m_pNameTextBox->ClearText();
		EDIT_END();
	}
}