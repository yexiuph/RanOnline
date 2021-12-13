#include "pch.h"
#include "./SMSSendWindow.h"

#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "./BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIMultiEditBox.h"
#include "./GameTextControl.h"
#include "./BasicLineBox.h"
#include "./InnerInterface.h"
#include "./FriendWindow.h"

#include "../[Lib]__Engine/Sources/DxTools/d3dfont.h"

#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlCharJoinMsg.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Client/GLCharacter.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Client/GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSMSSendWindow::CSMSSendWindow(void)
	: m_pMultiEditBox(NULL)
	, m_pNameTextBox(NULL)
{
}

CSMSSendWindow::~CSMSSendWindow(void)
{
}

void CSMSSendWindow::CreateSubControl()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	{
		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub( this, "SMS_SEND_TO_TEXTBOX" );
		pTextBox->SetFont( pFont9 );
		pTextBox->AddText( (char*)ID2GAMEWORD("FRIEND_BUTTON",14) );
		RegisterControl( pTextBox );

		pTextBox = new CBasicTextBox;
		pTextBox->CreateSub( this, "SMS_SEND_TO_NAME_TEXTBOX" );
		pTextBox->SetFont( pFont9 );		
		RegisterControl( pTextBox );
		m_pNameTextBox = pTextBox;
	}

	{
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "SMS_SEND_LINEBOX" );
		RegisterControl ( pBasicLineBox );		

		CBasicTextBox* pTextBox = new CBasicTextBox;
		pTextBox->CreateSub ( this, "SMS_SEND_STATIC" );
		pTextBox->SetFont ( pFont9 );
		RegisterControl ( pTextBox );
		pTextBox->AddText( ID2GAMEINTEXT( "SMS_SEND_STATIC" ) );
	}

	{
		CBasicLineBox* pLineBox = new CBasicLineBox;
		pLineBox->CreateSub( this, "BASIC_LINE_BOX_MINIPARTY", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pLineBox->CreateBaseBoxSystemMessageUp( "SMS_SEND_LINE_TEXTBOX" );
		pLineBox->ResetAlignFlag ();
		RegisterControl( pLineBox );
	}

	{
		CUIMultiEditBox* pMultiEditBox = new CUIMultiEditBox;
		pMultiEditBox->CreateSub( this, "SMS_SEND_EDITBOX" );
		pMultiEditBox->CreateEditBox( SMS_SEND_EDITBOX_L01, "SMS_SEND_EDITBOX_L01", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		pMultiEditBox->CreateEditBox( SMS_SEND_EDITBOX_L02, "SMS_SEND_EDITBOX_L02", "WRITE_NOTE_CARRAT", TRUE, UINT_MAX, pFont9 );
		RegisterControl( pMultiEditBox );
		m_pMultiEditBox = pMultiEditBox;
	}

	{
		CBasicTextButton* pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, SMS_SEND_BUTTON );
		pButton->CreateBaseButton( "SMS_SEND_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",12) );
		RegisterControl( pButton );

		pButton = new CBasicTextButton;
		pButton->CreateSub( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, SMS_CANCEL_BUTTON );
		pButton->CreateBaseButton( "SMS_CANCEL_BUTTON", CBasicTextButton::SIZE14, CBasicButton::CLICK_FLIP, (char*)ID2GAMEWORD("FRIEND_BUTTON",13) );
		RegisterControl( pButton );
	}
}

void CSMSSendWindow::TranslateUIMessage( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( cID, dwMsg );

	switch ( cID )
	{
	case SMS_SEND_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CString strNotice;
				m_pMultiEditBox->GetText( strNotice );

				const SFRIEND & sFriend = CInnerInterface::GetInstance().GetFriendWindow()->GetFriendSMSInfo();

				GLGaeaClient::GetInstance().GetCharacter()->ReqSendSMS( sFriend.nCharID, sFriend.szPhoneNumber, strNotice );

				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;

	case SMS_CANCEL_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup( GetWndID() );
			}
		}
		break;
	}
}

void CSMSSendWindow::EDIT_BEGIN()
{
	m_pMultiEditBox->ClearEdit();
	m_pMultiEditBox->Init ();
	m_pMultiEditBox->EndEdit ();
	m_pMultiEditBox->BeginEdit ();
}

void CSMSSendWindow::EDIT_END()
{
	m_pMultiEditBox->EndEdit();
}

void CSMSSendWindow::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		const CString& strName = CInnerInterface::GetInstance().GetFriendWindow()->GET_FRIEND_NAME();
		m_pNameTextBox->AddText( strName );

		EDIT_BEGIN();
	}
	else
	{
		m_pNameTextBox->ClearText();

		EDIT_END();
	}
}