#include "pch.h"
#include "ItemShopWindowWeb.h"

#include "../[Lib]__EngineUI/Sources/UIWindowBody.h"
#include "SubPath.h"
#include "RanParam.h"

#include "CommonWeb.h"
#include "GLGaeaClient.h"
#include "GLCharacter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const CString strBlank = "about:blank";

VOID CItemShopWindowWeb::CreateWeb()
{
	if( strstr( RANPARAM::ItemShopAddress, "http" ) )
	{
#if defined( CH_PARAM ) 
		CCommonWeb::Get()->Navigate( CCommonWeb::ITEMSHOP_ID, RANPARAM::China_Region[RANPARAM::nChinaRegion].strItemShop, TRUE );
#else
		CCommonWeb::Get()->Navigate( CCommonWeb::ITEMSHOP_ID, RANPARAM::ItemShopAddress, TRUE );
#endif
	}
	else
	{
		std::string strFullPath = SUBPATH::APP_ROOT;
#if defined( CH_PARAM )
		strFullPath += RANPARAM::China_Region[RANPARAM::nChinaRegion].strItemShop;
#else
		strFullPath += RANPARAM::ItemShopAddress;
#endif
		CCommonWeb::Get()->Navigate( CCommonWeb::ITEMSHOP_ID, strFullPath.c_str(), TRUE );
	}
}

VOID CItemShopWindowWeb::AlignWeb()
{
	if( CCommonWeb::Get()->GetVisible( CCommonWeb::ITEMSHOP_ID ) )
	{
		const UIRECT& rcBody = GetBody()->GetGlobalPos();
		CCommonWeb::Get()->Move( CCommonWeb::ITEMSHOP_ID,
								 (INT)rcBody.left + ALIGN_LEFT,
                                 (INT)rcBody.top + ALIGN_LEFT,
                                 (INT)rcBody.sizeX - ALIGN_RIGHT,
                                 (INT)rcBody.sizeY - ALIGN_RIGHT );
	}
	else
	{
		CCommonWeb::Get()->Move( CCommonWeb::ITEMSHOP_ID, 0, 0, 0, 0, FALSE, FALSE );
	}
}

VOID CItemShopWindowWeb::SetVisibleSingle( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );	

	CCommonWeb::Get()->SetVisible( CCommonWeb::ITEMSHOP_ID, bVisible );		

	GLGaeaClient::GetInstance().GetCharacter()->ReqItemShopOpen( bVisible );

	AlignWeb();

#if defined ( JP_PARAM )	// itemShopAuth
	if ( !bVisible )	CCommonWeb::Get()->Navigate( CCommonWeb::ITEMSHOP_ID, strBlank, bVisible );
#endif

}

VOID CItemShopWindowWeb::Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	AlignWeb();
}