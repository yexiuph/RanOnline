/**
* \ingroup 
*
* \par requirements
* Windows XP, Windows 2000 Professional, Windows NT Workstation, Windows Me, Windows 98, or Windows 95. \n
* Header Declared in Rpcdce.h; include Rpc.h. \n
* Library Link to Rpcrt4.lib. \n
* DLL Requires Rpcrt4.dll. \n
*
* \version 1.0
*
* \date 2006.04.29
*
* \author Jgkim
*
* \par license
* Copyright(c) Mincoms. All rights reserved.
* 
* \todo 
*
* \bug 
*
*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Rpc.h>

#define UUID_STR_LENGTH 37

namespace SERVER_UTIL
{

enum UUID_RESULT
{
	UUID_ERROR   = -1,
	UUID_SUCCESS =  0,		
};
		
int getUUID( TCHAR* pszUUID, size_t cchUUID );
int getUUID( UUID &_uuid );
int uuidToString( UUID &_uuid, TCHAR* pszUUID );
int stringToUUID( TCHAR* pszUUID, UUID &_uuid );

} // End of namesapce uuid 