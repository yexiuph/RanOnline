#include "pch.h"
#include "BasicEscMenu.h"
#include "BasicLineBoxEx.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"
#include "InnerInterface.h"
#include "ModalWindow.h"
#include "ModalCallerID.h"
#include "DxGlobalStage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicEscMenu::CBasicEscMenu ()
{
}

CBasicEscMenu::~CBasicEscMenu ()
{
}

void CBasicEscMenu::CreateSubControl ()
{
	CBasicLineBoxEx* pBasicLineBox = new CBasicLineBoxEx;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_ESC", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxESCMenu ( "ESC_MENU_BACK" );
	RegisterControl ( pBasicLineBox );

	{	//	버튼 만들기
//		CreateTextButton ( "ESC_MENU_CHARACTER_CHANGE", ESC_MENU_CHARACTER_CHANGE, (char*)ID2GAMEWORD ( "ESC_MENU", 0 ) );
		CreateTextButton ( "ESC_MENU_SERVER_CHANGE", ESC_MENU_SERVER_CHANGE, (char*)ID2GAMEWORD ( "ESC_MENU", 1 ) );
		CreateTextButton ( "ESC_MENU_HWOPTION", ESC_MENU_HWOPTION, (char*)ID2GAMEWORD ( "ESC_MENU", 2 ) );
		CreateTextButton ( "ESC_MENU_HELP", ESC_MENU_HELP, (char*)ID2GAMEWORD ( "ESC_MENU", 3 ) );		
		CreateTextButton ( "ESC_MENU_CLOSEGAME", ESC_MENU_CLOSEGAME, (char*)ID2GAMEWORD ( "ESC_MENU", 4 ) );
		CreateTextButton ( "ESC_MENU_KEYSETTING", ESC_MENU_KEYSETTING, (char*)ID2GAMEWORD ( "ESC_MENU", 5 ) );


		//CreateFlipButton ( "ESC_MENU_CLOSE", "ESC_MENU_CLOSE_F", ESC_MENU_CLOSE );
	}
}

CBasicButton* CBasicEscMenu::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::CLICK_FLIP );
	pButton->SetUseDynamic ( TRUE );
	RegisterControl ( pButton );

	return pButton;
}

void CBasicEscMenu::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	const float fBUTTON_CLICK = CBasicButton::fBUTTON_CLICK;

	switch ( ControlID )
	{
	//case ESC_MENU_CHARACTER_CHANGE:
	//	{
	//		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	//		{
	//			GASSERT ( 0 && "캐릭터 선택" );
	//		}
	//	}
	//	break;

	case ESC_MENU_SERVER_CHANGE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModal ( ID2GAMEINTEXT ( "MODAL_ESC_SERVER_STAGE" ), MODAL_QUESTION, YESNO, MODAL_SERVER_STAGE );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;

	case ESC_MENU_HELP:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//	TODO : 모달 박스 출력 필요.
				//
				CInnerInterface::GetInstance().ShowGroupFocus ( HELP_WINDOW );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;

	case ESC_MENU_HWOPTION:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().ShowGroupFocus ( OPTION_HW_WINDOW );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;

	case ESC_MENU_CLOSEGAME:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				DoModal ( ID2GAMEINTEXT ( "MODAL_ESC_MENU_CLOSE" ), MODAL_QUESTION, YESNO, MODAL_CLOSEGAME );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	case ESC_MENU_KEYSETTING:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().GetShotCutKey();
				CInnerInterface::GetInstance().ShowGroupFocus ( KEY_SETTING_WINDOW );
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	case ESC_MENU_CLOSE:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				CInnerInterface::GetInstance().HideGroup ( GetWndID () );
			}
		}
		break;
	}
}

CBasicTextButton*	 CBasicEscMenu::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON22", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, 
									CBasicTextButton::SIZE22, 
									CBasicButton::CLICK_FLIP, 
									szText,
									_DEFAULT_FONT_SHADOW_EX_FLAG );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	return pTextButton;
}