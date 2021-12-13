#include "pch.h"
#include "BasicGameMenu.h"

#include "InnerInterface.h"
#include "BasicVarTextBox.h"
#include "UITextControl.h"
#include "GameTextControl.h"
#include "RANPARAM.h"
#include "ModalWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicGameMenu::CBasicGameMenu ()
	: m_pRunButton(NULL)
	, m_pInfo(NULL)
	, m_nOverMenu( NO_ID )
{
}

CBasicGameMenu::~CBasicGameMenu ()
{
}

void CBasicGameMenu::CreateSubControl ()
{
	InitShotCutString();
/*
#ifdef CH_PARAM // 중국 인터페이스 변경
	CUIControl * pControl = new CUIControl;
	pControl->CreateSub ( this, "GAME_MENU_TOP" );
	RegisterControl ( pControl );
#endif
*/
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_INVENTORY_BUTTON", UI_FLAG_DEFAULT, MENU_INVENTORY_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_CHARACTER_BUTTON", UI_FLAG_DEFAULT, MENU_CHARACTER_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_SKILL_BUTTON", UI_FLAG_DEFAULT, MENU_SKILL_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_PARTY_BUTTON", UI_FLAG_DEFAULT, MENU_PARTY_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_GUILD_BUTTON", UI_FLAG_DEFAULT, MENU_GUILD_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_QUEST_BUTTON", UI_FLAG_DEFAULT, MENU_QUEST_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_FRIEND_BUTTON", UI_FLAG_DEFAULT, MENU_FRIEND_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_LARGEMAP_BUTTON", UI_FLAG_DEFAULT, MENU_LARGEMAP_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_CHATMACRO_BUTTON", UI_FLAG_DEFAULT, MENU_CHATMACRO_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_ITEMBANK_BUTTON", UI_FLAG_DEFAULT, MENU_ITEMBANK_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_ITEMSHOP_BUTTON", UI_FLAG_DEFAULT, MENU_ITEMSHOP_BUTTON );
	pButton->CreateFlip ( "MENU_OVER_IMAGE", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );

	m_pRunButton = new CBasicButton;
	m_pRunButton->CreateSub ( this, "MENU_RUN_BUTTON", UI_FLAG_DEFAULT, MENU_RUN_BUTTON );
	m_pRunButton->CreateFlip ( "MENU_RUN_BUTTON_F", CBasicButton::RADIO_FLIP );
	m_pRunButton->CreateMouseOver( "MENU_OVER_IMAGE" );
	m_pRunButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( m_pRunButton );

// #ifndef CH_PARAM // 중국 인터페이스 변경
	pButton = new CBasicButton;
	pButton->CreateSub ( this, "MENU_CLOSE_BUTTON", UI_FLAG_DEFAULT, MENU_CLOSE_BUTTON );
	pButton->CreateFlip ( "MENU_CLOSE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );
//#endif

	m_pInfo = new CBasicVarTextBox;
	m_pInfo->CreateSub ( this, "BASIC_VAR_TEXT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	m_pInfo->CreateSubControl ();
	m_pInfo->SetMousePointGap ( D3DXVECTOR2(-40.0f,-40.0f) );
	m_pInfo->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pInfo );
}

void CBasicGameMenu::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	m_pInfo->SetVisibleSingle( FALSE );
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CBasicGameMenu::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case MENU_INVENTORY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[0], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( INVENTORY_WINDOW )
						&&  !CInnerInterface::GetInstance().IsVisibleGroup ( TRADEINVENTORY_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( INVENTORY_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( INVENTORY_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_CHARACTER_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[1], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( CHARACTER_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( CHARACTER_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( CHARACTER_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_SKILL_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[2], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( SKILL_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( SKILL_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( SKILL_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_PARTY_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[3], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( PARTY_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( PARTY_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( PARTY_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_QUEST_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[4], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( QUEST_WINDOW ) )
					{
						DWORD dwEventQuestID = CInnerInterface::GetInstance().GetEventQuestID ();
						if ( NATIVEID_NULL().dwID == dwEventQuestID )
						{
							CInnerInterface::GetInstance().ShowGroupFocus ( QUEST_WINDOW );
							CInnerInterface::GetInstance().REFRESH_QUEST_WINDOW ();							
						}
						else
						{
							CInnerInterface::GetInstance().SetQuestWindowOpen ( dwEventQuestID );
						}
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( QUEST_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_GUILD_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[5], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( CLUB_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( CLUB_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( CLUB_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_FRIEND_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[6], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( FRIEND_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( FRIEND_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( FRIEND_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_LARGEMAP_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[7], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( LARGEMAP_WINDOW ) )
					{
						CInnerInterface::GetInstance().ShowGroupFocus ( LARGEMAP_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( LARGEMAP_WINDOW );
					}
				}
			}
		}
		break;
	case MENU_CHATMACRO_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[11], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( CHATMACRO_WINDOW ) )
					{
						CInnerInterface::GetInstance().GetChatMacro();
						CInnerInterface::GetInstance().ShowGroupFocus ( CHATMACRO_WINDOW );
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( CHATMACRO_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_ITEMBANK_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[8], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( ITEMBANK_WINDOW ) )
					{
						CInnerInterface::GetInstance().SetItemBankWindowOpen ();
					}
					/*else
					{
						CInnerInterface::GetInstance().HideGroup ( ITEMBANK_WINDOW );
					}*/
				}
			}
		}
		break;

	case MENU_ITEMSHOP_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[10], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( dwMsg & UIMSG_LB_UP )
				{
					if ( !CInnerInterface::GetInstance().IsVisibleGroup ( ITEMSHOP_WINDOW ) )
					{
						// itemShopAuth
						if ( CInnerInterface::GetInstance().ItemShopAuth() )
						{
							CInnerInterface::GetInstance().ShowGroupFocus ( ITEMSHOP_WINDOW );
						}
						
						CInnerInterface::GetInstance().HideGroup ( HELP_WINDOW );								
					}
					else
					{
						CInnerInterface::GetInstance().HideGroup ( ITEMSHOP_WINDOW );
					}
				}
			}
		}
		break;

	case MENU_RUN_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( m_nOverMenu != ControlID )
				{
					m_pInfo->SetTextNoSplit ( m_ShotcutText[9], NS_UITEXTCOLOR::SILVER );
				}
				m_pInfo->SetVisibleSingle ( TRUE );
				m_nOverMenu = ControlID;

				if ( UIMSG_LB_UP & dwMsg )
				{
					CInnerInterface::GetInstance().ReqToggleRun ();
				}
			}
		}
		break;
		
	case MENU_CLOSE_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_UP_LIKE ( dwMsg ) )
				{
					CInnerInterface::GetInstance().HideGroup ( GetWndID () );
					CInnerInterface::GetInstance().ShowGroupBottom ( GAME_MENU_OPEN_BUTTON, true );
				}
			}
		}
		break;
	}
}

void CBasicGameMenu::InitShotCutString()
{
	int i=0;
	int nIndex = 0;
	CString strTemp;
	
	for ( i=0; i< BASIC_MENU_NUM; ++i){
		m_ShotcutText[i] = ID2GAMEWORD ("GAMEMENU",i);
		
		nIndex = RANPARAM::BasicMenuToRanparam[i];
		strTemp = CInnerInterface::GetInstance().GetdwKeyToString(RANPARAM::MenuShotcut[nIndex]);
		
		SetShotcutText(i,strTemp);
	}
 
}

void CBasicGameMenu::SetShotcutText ( DWORD nID, CString& strTemp )
{
	CString ShotcutTemp = m_ShotcutText[nID];

	ShotcutTemp = ShotcutTemp.Left(ShotcutTemp.Find("("));
	ShotcutTemp += "(" + strTemp + ")";
	m_ShotcutText[nID] = ShotcutTemp;	
}

