#include "pch.h"
#include "PassKeyboard.h"

#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "LoginPage.h"
#include "SecPassCheckPage.h"
#include "SecPassSetPage.h"
#include "OuterInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPassKeyboard::CPassKeyboard()
	: m_pPassKeyboardUpChar(NULL)
	, m_pPassKeyboardLowChar(NULL)
	, m_pKeyCapsLockLed(NULL)
	, m_bUpperCase(FALSE)
{
}

CPassKeyboard::~CPassKeyboard()
{
}

void CPassKeyboard::CreateSubControl ()
{
	CUIControl* pPassKeyboard = new CUIControl;
	pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_FORM", UI_FLAG_DEFAULT, PASS_KEYBOARD_FORM );		
	RegisterControl( pPassKeyboard );

	CString strCombine;
	for( int i=0; i<10; ++i )
	{
		strCombine.Format( "KEY_BUTTON_%d", i );
		CreateKeyButton( strCombine.GetString(), "KEY_BUTTON_FLIP", KEY_BUTTON_0 + i );
	}

	for( int i=0; i<26; ++i )
	{
		strCombine.Format( "KEY_BUTTON_%c", KEY_BUTTON_A + i );
		CreateKeyButton( strCombine.GetString(), "KEY_BUTTON_FLIP", KEY_BUTTON_A + i );
	}

	{ // Note : 기능키
		CreateKeyButton( "KEY_BUTTON_BACK", "KEY_BUTTON_FLIP", KEY_BUTTON_BACK );
		CreateKeyButton( "KEY_BUTTON_TAB", "KEY_BUTTON_L_FLIP", KEY_BUTTON_TAB );
		CreateKeyButton( "KEY_BUTTON_CAPSL", "KEY_BUTTON_L_FLIP", KEY_BUTTON_CAPSL );
	}

	{ // Note : Caps Lock LED
		CBasicButton* pButton = new CBasicButton;
		pButton->CreateSub( this, "KEY_BUTTON_CAPSL_LED", UI_FLAG_DEFAULT, KEY_BUTTON_CAPSL_LED );
		pButton->CreateFlip( "KEY_BUTTON_CAPSL_LED_FLIP", CBasicButton::RADIO_FLIP );
		RegisterControl( pButton );
		m_pKeyCapsLockLed = pButton;
	}

	{ // Note : 문자 스킨
		pPassKeyboard = new CUIControl;
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_NUMBER", UI_FLAG_DEFAULT, PASS_KEYBOARD_NUMBER );		
		RegisterControl( pPassKeyboard );

		pPassKeyboard = new CUIControl;
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_LOW_CHAR", UI_FLAG_DEFAULT, PASS_KEYBOARD_LOW_CHAR );
		pPassKeyboard->SetVisibleSingle( FALSE );
		RegisterControl( pPassKeyboard );
		m_pPassKeyboardLowChar = pPassKeyboard;

		pPassKeyboard = new CUIControl;
		pPassKeyboard->CreateSub( this, "PASS_KEYBOARD_UP_CHAR", UI_FLAG_DEFAULT, PASS_KEYBOARD_UP_CHAR );
		pPassKeyboard->SetVisibleSingle( FALSE );
		RegisterControl( pPassKeyboard );
		m_pPassKeyboardUpChar = pPassKeyboard;
	}
}

void CPassKeyboard::CreateKeyButton( const char * szControlKey, const char * szFlipKey, UIGUID nID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub( this, szControlKey, UI_FLAG_DEFAULT, nID );
	pButton->CreateFlip( szFlipKey, CBasicButton::MOUSEIN_FLIP );
	RegisterControl( pButton );		
}

void CPassKeyboard::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( (m_bUpperCase && !(::GetKeyState(VK_CAPITAL) & 1)) ||
		(!m_bUpperCase && (::GetKeyState(VK_CAPITAL) & 1)) )
	{
		m_bUpperCase = !m_bUpperCase;
		SetVisibleKeyChar( m_bUpperCase );
	}
}

void CPassKeyboard::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	if( ControlID >= KEY_BUTTON_0 && ControlID <= KEY_BUTTON_9 )
	{
		TranslateKeyMsg( ControlID, dwMsg, TRUE );
	}
	else if( ControlID >= KEY_BUTTON_A && ControlID <= KEY_BUTTON_Z )
	{
		TranslateKeyMsg( ControlID, dwMsg );
	}
	else
	{
		switch( ControlID )
		{
		case KEY_BUTTON_BACK:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					TranslateBackKey();					
				}
			}
			break;

		case KEY_BUTTON_TAB:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					TranslateTabKey();
				}
			}
			break;

		case KEY_BUTTON_CAPSL:
			{
				if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
				{
					m_bUpperCase = !m_bUpperCase;
					SetVisibleKeyChar( m_bUpperCase );

					if( (m_bUpperCase && !(::GetKeyState(VK_CAPITAL) & 1)) ||
						(!m_bUpperCase && (::GetKeyState(VK_CAPITAL) & 1)) )
					{
						keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
						keybd_event( VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
					}
				}
			}
			break;
		}
	}
}

void CPassKeyboard::TranslateKeyMsg( UIGUID ControlID, DWORD dwMsg, BOOL bNumber )
{

	TCHAR Key; 

	if( m_bUpperCase || bNumber ) // Note : 대문자 or 숫자
	{
		Key =  _TCHAR(ControlID);
	}
	else 
	{
		Key =   _TCHAR(ControlID + 32);
	}


	if( CHECK_MOUSEIN_LBUPLIKE( dwMsg ) )
	{

		if( COuterInterface::GetInstance().IsVisibleGroup( LOGIN_PAGE ) )
		{
			CLoginPage * pLoginPage = COuterInterface::GetInstance().GetLoginPage();
			if( !pLoginPage ) return;
			pLoginPage->SetCharToEditBox(Key);
		}
		else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_SETPAGE ) )
		{
			CSecPassSetPage * pSecPassSetPage = COuterInterface::GetInstance().GetSecPassSetPage();
			if( !pSecPassSetPage ) return;
			pSecPassSetPage->SetCharToEditBox(Key);
		}
		else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_CHECKPAGE ) )
		{
			CSecPassCheckPage * pSecPassCheckPage = COuterInterface::GetInstance().GetSecPassCheckPage();
			if( !pSecPassCheckPage ) return;
			pSecPassCheckPage->SetCharToEditBox(Key);
		}
	}
}

void CPassKeyboard::SetVisibleKeyChar( BOOL bCase )
{
	m_pPassKeyboardUpChar->SetVisibleSingle( FALSE );
	m_pPassKeyboardLowChar->SetVisibleSingle( FALSE );

	if( bCase )	m_pPassKeyboardUpChar->SetVisibleSingle( TRUE );
	else		m_pPassKeyboardLowChar->SetVisibleSingle( TRUE );

	m_pKeyCapsLockLed->SetFlip( bCase );
}

void CPassKeyboard::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle( bVisible );

	if( bVisible )
	{
		if( ::GetKeyState(VK_CAPITAL) & 1 )	m_bUpperCase = TRUE;
		else								m_bUpperCase = FALSE;

		SetVisibleKeyChar( m_bUpperCase );
	}
}

void CPassKeyboard::TranslateBackKey()
{
	if( COuterInterface::GetInstance().IsVisibleGroup( LOGIN_PAGE ) )
	{
		CLoginPage * pLoginPage = COuterInterface::GetInstance().GetLoginPage();
		if( !pLoginPage ) return;
		pLoginPage->DelCharToEditBox();
	}
	else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = COuterInterface::GetInstance().GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->DelCharToEditBox();
	}
	else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = COuterInterface::GetInstance().GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->DelCharToEditBox();
	}
}

void CPassKeyboard::TranslateTabKey()
{

	if( COuterInterface::GetInstance().IsVisibleGroup( LOGIN_PAGE ) )
	{
		CLoginPage * pLoginPage = COuterInterface::GetInstance().GetLoginPage();
		if( !pLoginPage ) return;
		pLoginPage->GoNextTab();
	}
	else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_SETPAGE ) )
	{
		CSecPassSetPage * pSecPassSetPage = COuterInterface::GetInstance().GetSecPassSetPage();
		if( !pSecPassSetPage ) return;
		pSecPassSetPage->GoNextTab();
	}
	else if( COuterInterface::GetInstance().IsVisibleGroup( SECPASS_CHECKPAGE ) )
	{
		CSecPassCheckPage * pSecPassCheckPage = COuterInterface::GetInstance().GetSecPassCheckPage();
		if( !pSecPassCheckPage ) return;
		pSecPassCheckPage->GoNextTab();
	}

}