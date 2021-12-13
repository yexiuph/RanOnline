#include "pch.h"
#include "CreateCharacterWeb.h"

#include "../[Lib]__EngineUI/Sources/UIWindowBody.h"
#include "SubPath.h"
#include "RanParam.h"
#include "CommonWeb.h"
#include "OuterInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VOID CCreateCharacterWeb::CreateWeb()
{
	std::string strFullPath = SUBPATH::APP_ROOT;
	std::string strTemp;
	
	strTemp = strFullPath + _T("\\Data\\Gui\\fight.htm");
	CCommonWeb::Get()->Navigate( CCommonWeb::FIGHT_ID, strTemp.c_str(), TRUE);

	strTemp = strFullPath + _T("\\Data\\Gui\\fencing.htm");
	CCommonWeb::Get()->Navigate( CCommonWeb::FENCING_ID, strTemp.c_str(), TRUE );

	strTemp = strFullPath + _T("\\Data\\Gui\\archery.htm");
	CCommonWeb::Get()->Navigate( CCommonWeb::ARCHERY_ID, strTemp.c_str(), TRUE );

	strTemp = strFullPath + _T("\\Data\\Gui\\spirit.htm");
	CCommonWeb::Get()->Navigate( CCommonWeb::SPIRIT_ID, strTemp.c_str(), TRUE );

	//return;

	//if( strstr( RANPARAM::HelpAddress, "http" ) )
	//{
	//	CCommonWeb::Get()->Navigate( WEB_ID, RANPARAM::HelpAddress, TRUE );
	//}
	//else
	//{
	//	std::string strFullPath = SUBPATH::APP_ROOT;
	//	strFullPath += RANPARAM::HelpAddress;
	//	CCommonWeb::Get()->Navigate( WEB_ID, strFullPath.c_str(), TRUE );
	//}
}

VOID CCreateCharacterWeb::AlignWeb()
{
	for( int i=CCommonWeb::FIGHT_ID; i<CCommonWeb::TOTAL_ID; ++i )
	{
		if( CCommonWeb::Get()->GetVisible( i ) )
		{
			const UIRECT& rcBody = GetBody()->GetGlobalPos();
			CCommonWeb::Get()->Move( i,	(INT)rcBody.left + ALIGN_LEFT,
										(INT)rcBody.top + ALIGN_LEFT,
										(INT)rcBody.sizeX - ALIGN_RIGHT,
										(INT)rcBody.sizeY - ALIGN_RIGHT );
		}
		else
		{
			CCommonWeb::Get()->Move( i, 0, 0, 0, 0, FALSE, FALSE );
		}
	}
}

void CCreateCharacterWeb::ShiftCharClass( WORD nIndex )
{
	WORD nClass = nIndex+CCommonWeb::FIGHT_ID;

	if( CCommonWeb::FIGHT_ID <= nClass && nClass <= CCommonWeb::SPIRIT_ID )
	{
		COuterInterface::GetInstance().ShowGroupFocus( GetWndID() );
		
		CCommonWeb::Get()->SetVisible( nClass, TRUE );
		CCommonWeb::Get()->Refresh( (INT)nClass );
	}
	else
	{
		COuterInterface::GetInstance().HideGroup( GetWndID() );

		AlignWeb();
	}
}

void CCreateCharacterWeb::SetVisibleSingle ( BOOL bVisible )
{
	CUIOuterWindow::SetVisibleSingle( bVisible );

	CCommonWeb::Get()->SetVisible( FALSE );

	if( !bVisible ) AlignWeb();
}

void CCreateCharacterWeb::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
}