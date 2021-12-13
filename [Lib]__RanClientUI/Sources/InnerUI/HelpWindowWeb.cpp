#include "pch.h"
#include "HelpWindowWeb.h"

#include "../[Lib]__EngineUI/Sources/UIWindowBody.h"
#include "SubPath.h"
#include "RanParam.h"
#include "InnerInterface.h"

#include "CommonWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//--------------------------------------------------------------------
// HelpWindowWeb
//--------------------------------------------------------------------
VOID CHelpWindowWeb::CreateWeb()
{
	if( strstr( RANPARAM::HelpAddress, "http" ) )
	{
		CCommonWeb::Get()->Navigate( CCommonWeb::HELP_ID, RANPARAM::HelpAddress, TRUE );
	}
	else
	{
		std::string strFullPath = SUBPATH::APP_ROOT;
		strFullPath += RANPARAM::HelpAddress;
		CCommonWeb::Get()->Navigate( CCommonWeb::HELP_ID, strFullPath.c_str(), TRUE );
	}
}

VOID CHelpWindowWeb::AlignWeb()
{
	if( CCommonWeb::Get()->GetVisible( CCommonWeb::HELP_ID ) )
	{
		const UIRECT& rcBody = GetBody()->GetGlobalPos();
		CCommonWeb::Get()->Move( CCommonWeb::HELP_ID,
								 (INT)rcBody.left + ALIGN_LEFT,
                                 (INT)rcBody.top + ALIGN_LEFT,
                                 (INT)rcBody.sizeX - ALIGN_RIGHT,
                                 (INT)rcBody.sizeY - ALIGN_RIGHT );
	}
	else
	{
		CCommonWeb::Get()->Move( CCommonWeb::HELP_ID, 0, 0, 0, 0, FALSE, FALSE );
	}
}

VOID CHelpWindowWeb::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );

	CCommonWeb::Get()->SetVisible( CCommonWeb::HELP_ID, bVisible );

	AlignWeb();
}

VOID CHelpWindowWeb::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	AlignWeb();
}

void CHelpWindowWeb::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				CInnerInterface::GetInstance().SetDefaultPosInterface( HELP_WINDOW );
			}
		}
		break;	
	}
}