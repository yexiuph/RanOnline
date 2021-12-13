#include "pch.h"
#include "./FriendWindow.h"

#include "FriendWindowBlockPage.h"
#include "FriendWindowNormalPage.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "InnerInterface.h"
#include "BasicLineBox.h"
#include "InBoxWindow.h"
#include "SentWindow.h"
#include "ReceiveNoteWindow.h"
#include "WriteNoteWindow.h"
#include "GLGaeaClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFriendWindow::CFriendWindow ()
	: m_pNORMAL_TAB_BUTTON( NULL )
    , m_pBLOCK_TAB_BUTTON( NULL )
	, m_pRECEIVE_TAB_BUTTON( NULL )
	, m_pSEND_TAB_BUTTON( NULL )
	, m_pBlockPage( NULL )
	, m_pNormalPage( NULL )
	, m_pInBoxPage( NULL )
	, m_pSentPage( NULL )
{
}

CFriendWindow::~CFriendWindow ()
{
}

void CFriendWindow::CreateSubControl ()
{
	m_pNORMAL_TAB_BUTTON = CreateTextButton19 ( "FRIEND_NORMAL_TAB_BUTTON", FRIEND_NORMAL_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",0) );
	m_pBLOCK_TAB_BUTTON	 = CreateTextButton19 ( "FRIEND_BLOCK_TAB_BUTTON", FRIEND_BLOCK_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",1) );
	m_pRECEIVE_TAB_BUTTON = CreateTextButton19 ( "FRIEND_RECEIVE_TAB_BUTTON", RECEIVE_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",2) );
	m_pRECEIVE_TAB_BUTTON->SetVisibleSingle( FALSE );
	m_pSEND_TAB_BUTTON	= CreateTextButton19 ( "FRIEND_SEND_TAB_BUTTON", SEND_TAB_BUTTON, (char*)ID2GAMEWORD("FRIEND_TAB_BUTTON",3) );
	m_pSEND_TAB_BUTTON->SetVisibleSingle( FALSE );

	{	// 텍스트 박스 배경
		CBasicLineBox* pBasicLineBox = new CBasicLineBox;
		pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_QUEST_LIST", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
		pBasicLineBox->CreateBaseBoxQuestList ( "FRIEND_LINEBOX" );
		RegisterControl ( pBasicLineBox );		
	}

	m_pNormalPage = new CFriendWindowNormalPage;
	m_pNormalPage->CreateSub ( this, "FRIEND_NORMAL_PAGE", UI_FLAG_DEFAULT, FRIEND_NORMAL_PAGE );
	m_pNormalPage->CreateSubControl ();
	RegisterControl ( m_pNormalPage );

	m_pBlockPage = new CFriendWindowBlockPage;
	m_pBlockPage->CreateSub ( this, "FRIEND_BLOCK_PAGE", UI_FLAG_DEFAULT, FRIEND_BLOCK_PAGE );
	m_pBlockPage->CreateSubControl ();
	RegisterControl ( m_pBlockPage );

	m_pInBoxPage = new CInBoxWindow;
	m_pInBoxPage->CreateSub( this, "FRIEND_INBOX_PAGE", UI_FLAG_DEFAULT, FRIEND_INBOX_PAGE );
	m_pInBoxPage->CreateSubControl();
	RegisterControl( m_pInBoxPage );

	m_pSentPage = new CSentWindow;
	m_pSentPage->CreateSub( this, "FRIEND_SENT_PAGE", UI_FLAG_DEFAULT, FRIEND_SENT_PAGE );
	m_pSentPage->CreateSubControl();
	RegisterControl( m_pSentPage );

	ChangePage ( FRIEND_NORMAL_TAB_BUTTON );
}

CBasicTextButton* CFriendWindow::CreateTextButton19 ( char* szButton, UIGUID ControlID , char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON191", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, CBasicTextButton::SIZE19_RECT, CBasicButton::RADIO_FLIP, szText );
	pTextButton->SetAlignFlag ( UI_FLAG_BOTTOM );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	return pTextButton;
}

void CFriendWindow::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( FRIEND_WINDOW );
			}
		}
		break;
	case FRIEND_NORMAL_TAB_BUTTON:
	case FRIEND_BLOCK_TAB_BUTTON:
	case RECEIVE_TAB_BUTTON:
	case SEND_TAB_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg ) ChangePage ( cID );
			}
		}
		break;
	}
}

void CFriendWindow::ChangePage ( const UIGUID& cClickButton )
{
	{
		m_pNORMAL_TAB_BUTTON->SetFlip( FALSE );
		m_pBLOCK_TAB_BUTTON->SetFlip( FALSE );
		m_pRECEIVE_TAB_BUTTON->SetFlip( FALSE );
		m_pSEND_TAB_BUTTON->SetFlip( FALSE );
	}

	{
		m_pNormalPage->SetVisibleSingle( FALSE );
		m_pBlockPage->SetVisibleSingle( FALSE );
		m_pInBoxPage->SetVisibleSingle( FALSE );
		m_pSentPage->SetVisibleSingle( FALSE );
	}

	CBasicTextButton* pVisibleButton( NULL );
	CUIControl* pVisiblePage( NULL );

	switch ( cClickButton )
	{
	case FRIEND_NORMAL_TAB_BUTTON:
		{
			pVisibleButton = m_pNORMAL_TAB_BUTTON;
			pVisiblePage = m_pNormalPage;
		}
		break;

	case FRIEND_BLOCK_TAB_BUTTON:
		{
			pVisibleButton = m_pBLOCK_TAB_BUTTON;
			pVisiblePage = m_pBlockPage;
		}
		break;

	case RECEIVE_TAB_BUTTON:
		{
			pVisibleButton = m_pRECEIVE_TAB_BUTTON;
			pVisiblePage = m_pInBoxPage;
		}
		break;

	case SEND_TAB_BUTTON:
		{
			pVisibleButton = m_pSEND_TAB_BUTTON;
			pVisiblePage = m_pSentPage;
		}
		break;
	}

	if ( pVisibleButton && pVisiblePage )
	{
		pVisibleButton->SetFlip( TRUE );
		pVisiblePage->SetVisibleSingle( TRUE );
	}
}

void  CFriendWindow::LoadFriendList ()
{
	if ( m_pNormalPage->IsVisible () )	m_pNormalPage->LoadFriendList ();
	else								m_pBlockPage->LoadBlockList ();
}

void CFriendWindow::ADD_FRIEND_NAME_TO_EDITBOX ( const CString& strName )
{
	m_pNormalPage->ADD_FRIEND_NAME_TO_EDITBOX ( strName );
	m_pBlockPage->ADD_NAME_TO_EDITBOX ( strName );
}

void CFriendWindow::ADD_FRIEND ( const CString& strName )
{
	m_pNormalPage->ADD_FRIEND ( strName );
}

const CString& CFriendWindow::GET_FRIEND_NAME () const
{
	return m_pNormalPage->GET_FRIEND_NAME ();
}

const CString& CFriendWindow::GET_BLOCK_NAME () const
{
	return m_pBlockPage->GET_BLOCK_NAME ();
}

void CFriendWindow::EDIT_END ()
{
	m_pNormalPage->EDIT_END ();
	m_pBlockPage->EDIT_END ();
}

void CFriendWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		GLGaeaClient::GetInstance().GetCharacter()->ReqFriendWindowOpen( true );

		if( m_pNormalPage->IsVisible() )		m_pNormalPage->SetVisibleSingle( TRUE );
		else if( m_pBlockPage->IsVisible() )	m_pBlockPage->SetVisibleSingle( TRUE );
		else if( m_pInBoxPage->IsVisible() )	m_pInBoxPage->SetVisibleSingle( TRUE );
		else if( m_pSentPage->IsVisible() )		m_pSentPage->SetVisibleSingle( TRUE );
	}
	else
	{
		GLGaeaClient::GetInstance().GetCharacter()->ReqFriendWindowOpen( false );
	}
}

SFRIEND & CFriendWindow::GetFriendSMSInfo()
{ 
	return m_pNormalPage->GetFriendInfo(); 
}