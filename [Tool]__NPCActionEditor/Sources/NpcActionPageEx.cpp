#include "pch.h"
#include "NpcAction.h"
#include "NpcActionPage.h"
#include "SheetWithTab.h"

#include "NpcTalk.h"
#include "GLItemMan.h"
#include "GLSkill.h"

// CNpcActionPage ��ȭ �����Դϴ�.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string	CNpcActionPage::GetBoolString ( BOOL bValue )
{
	const static	char szTRUE[] = "TRUE";
	const static	char szFALSE[] = "FALSE";
	
	if ( bValue )	return szTRUE;
	else			return szFALSE;
}