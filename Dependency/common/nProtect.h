#ifndef _NPROTECT_H_
#define _NPROTECT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	DWORD dwIndex;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

#endif // _NPROTECT_H_
