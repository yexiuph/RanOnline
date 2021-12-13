#ifndef DXSERVERINSTANCE_H_
#define DXSERVERINSTANCE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"

class DxMsgServer;
class DxConsoleMsg;
class GLDBMan;
struct F_SERVER_INFO;

namespace DxFieldInstance
{
	HRESULT Create ( const char *szAppPath, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan *pDBMan, int nServiceProvider, const char* szMapList=NULL, DWORD dwFieldSID=FIELDSERVER_MAX, DWORD dwMaxClient=1000, bool bPK_MODE=true, int nChannel=0, BOOL bPKLess=FALSE, BOOL bUseIntelTBB = FALSE, DWORD dwLangSet = 0 );

	HRESULT FrameMove ();

	void CleanUp ();
};

namespace DxAgentInstance
{
	HRESULT Create ( const char *szAppPath, DxMsgServer *pMsgServer, DxConsoleMsg* pConsoleMsg, GLDBMan *pDBMan, char* szMapList=NULL, DWORD dwMaxClient=1000, F_SERVER_INFO* pFieldInfo=NULL, bool bPK_MODE=false, BOOL bPKLess=FALSE, DWORD dwLangSet = 0  );
	
	HRESULT FrameMove();

	void CleanUp ();
};

namespace GMTOOL
{
    HRESULT Create ( const char* szAppPath );

	void CleanUp ();	
};

#endif // DXSERVERINSTANCE_H_