#include "pch.h"
#include "./gassert.h"
#include "../[Client]__BugTrap/Sources/BugTrap.h"
#include "./MinBugTrap.h"
#include "../[Lib]__Engine/Sources/Common/BugTrapStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace BUG_TRAP;

void BUG_TRAP::BugTrapInstall(
	std::string& strAppName,
	bool bUseEmail,
	bool bUseScreenCapture )
{
#ifndef _DEBUG
//	// Setup InvalidParameterHandler
//	SetMinInvalidParameterHandler();

	// Setup exception handler
	BT_InstallSehFilter();

	std::string strApp = strAppName;
	strApp += BUG_TRAP::szCountry;
	
	BT_SetAppName( strApp.c_str() );

	// e-mail 보고를 받을 것인가?
	if ( true == bUseEmail )
	{
		BT_SetSupportEMail( BUG_TRAP::szEMail );
	}
	
	// 스크린 캡쳐를 사용할 것이지 설정
	if ( true == bUseScreenCapture )
	{
		BT_SetFlags( BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT | BTF_SCREENCAPTURE );
	}
	else
	{
		BT_SetFlags( BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT );
	}

	BT_SetSupportURL( BUG_TRAP::szSupportURL );

	// = BugTrapServer ===========================================
	BT_SetSupportServer( BUG_TRAP::szServerAddr, BUG_TRAP::nPort );
#endif
}
