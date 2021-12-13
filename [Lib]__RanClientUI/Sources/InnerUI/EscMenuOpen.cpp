#include "pch.h"
#include "EscMenuOpen.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEscMenuOpen::CEscMenuOpen ()
{
}

CEscMenuOpen::~CEscMenuOpen ()
{
}

void CEscMenuOpen::CreateSubControl ()
{
	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "ESCMENU_OPEN_BUTTON", UI_FLAG_DEFAULT, ESCMENU_OPEN_BUTTON );
	pButton->CreateFlip ( "ESCMENU_OPEN_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );
}

void CEscMenuOpen::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ESCMENU_OPEN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( !CInnerInterface::GetInstance().IsVisibleGroup ( ESC_MENU ) )
				{
					CInnerInterface::GetInstance().ShowGroupFocus ( ESC_MENU );
				}
				else
				{
					CInnerInterface::GetInstance().HideGroup ( ESC_MENU );
				}
			}
		}
		break;
	}
}
