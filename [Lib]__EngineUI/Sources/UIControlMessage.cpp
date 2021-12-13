#include "pch.h"
#include "UIControlMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD MAKE_UIMSG ( WORD HIGHMSG, WORD LOWMSG )
{		
	DWORD HighMsg= ((HIGHMSG) & USHRT_MAX) << BIT_16;
	DWORD LowMsg = ((LOWMSG) & USHRT_MAX);

	return DWORD(HighMsg | LowMsg);
}

DWORD MAKE_UIMSG ( BYTE HIGHMSG1, BYTE HIGHMSG2, BYTE LOWMSG1, BYTE LOWMSG2 )
{
	DWORD HighMsg1	= ((HIGHMSG1) & UCHAR_MAX)	<< BIT_24;
	DWORD HighMsg2	= ((HIGHMSG2) & UCHAR_MAX)	<< BIT_16;
	DWORD LowMsg1	= ((LOWMSG1) & UCHAR_MAX)	<< BIT_8;
	DWORD LowMsg2	= ((LOWMSG2) & UCHAR_MAX);

	return DWORD(HighMsg1 | HighMsg2 | LowMsg1 | LowMsg2);
}

BOOL CHECK_LB_DOWN_LIKE ( DWORD dwMsg )
{
	DWORD dwDownedMsg = UIMSG_LB_DOWN | UIMSG_LB_PRESEED;// | UIMSG_LB_DRAG;
	return (dwMsg & dwDownedMsg)?TRUE:FALSE;
}

BOOL CHECK_LB_UP_LIKE ( DWORD dwMsg )
{
	DWORD dwUpMsg = UIMSG_LB_UP | UIMSG_LB_DUP;
	return (dwMsg & dwUpMsg)?TRUE:FALSE;
}

BOOL CHECK_MB_DOWN_LIKE ( DWORD dwMsg )
{
	DWORD dwDownedMsg = UIMSG_MB_DOWN | UIMSG_MB_PRESSED | UIMSG_MB_DRAG;
	return (dwMsg & dwDownedMsg)?TRUE:FALSE;
}

BOOL CHECK_MB_UP_LIKE ( DWORD dwMsg )
{
	DWORD dwUpMsg = UIMSG_MB_UP | UIMSG_MB_DUP;
	return (dwMsg & dwUpMsg)?TRUE:FALSE;
}

BOOL CHECK_RB_DOWN_LIKE ( DWORD dwMsg )
{
	DWORD dwDownedMsg = UIMSG_RB_DOWN | UIMSG_RB_PRESSED | UIMSG_RB_DRAG;
	return (dwMsg & dwDownedMsg)?TRUE:FALSE;
}

BOOL CHECK_RB_UP_LIKE ( DWORD dwMsg )
{
	DWORD dwUpMsg = UIMSG_RB_UP | UIMSG_RB_DUP;
	return (dwMsg & dwUpMsg)?TRUE:FALSE;
}

BOOL CHECK_MOUSE_IN ( DWORD dwMsg )
{
	return (dwMsg & UIMSG_MOUSEIN)?TRUE:FALSE;
}

BOOL CHECK_MODAL_ON ( DWORD dwMsg )
{
	return (dwMsg & UIMSG_MODAL_ON)?TRUE:FALSE;
}

BOOL CHECK_FOCUSED ( DWORD dwMsg )
{
	return (dwMsg & UIMSG_FOCUSED)?TRUE:FALSE;
}

BOOL CHECK_KEYFOCUSED ( DWORD dwMsg )
{
	return (dwMsg & UIMSG_KEY_FOCUSED)?TRUE:FALSE;
}

BOOL CHECK_MOUSEIN_LBUPLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_LB_UP_LIKE ( dwMsg ));
}

BOOL CHECK_MOUSE_IN_LBDOWNLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_LB_DOWN_LIKE ( dwMsg ));
}

BOOL CHECK_MOUSEIN_MBUPLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_MB_UP_LIKE ( dwMsg ));
}

BOOL CHECK_MOUSE_IN_MBDOWNLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_MB_DOWN_LIKE ( dwMsg ));
}

BOOL CHECK_MOUSEIN_RBUPLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_RB_UP_LIKE ( dwMsg ));
}

BOOL CHECK_MOUSE_IN_RBDOWNLIKE ( DWORD dwMsg )
{
	return (CHECK_MOUSE_IN ( dwMsg ) && CHECK_RB_DOWN_LIKE ( dwMsg ));
}

BOOL CHECK_TOPARENT ( DWORD dwMsg )
{
	return ( dwMsg & UIMSG_TOTOPPARENT ) ? TRUE : FALSE;
}

DWORD CUT_LOW24BIT ( DWORD dwMsg )
{
	DWORD dwHIGH8BIT = MAKE_UIMSG ( 0xFF, 0x80, 0x00, 0x01 );
	return dwHIGH8BIT & dwMsg;
}