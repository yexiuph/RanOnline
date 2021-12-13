#include "pch.h"
#include "./ClientApex.h"
#include "./DxGlobalStage.h"
#include "../Dependency/common/ApexClient.h"

#ifdef	CH_PARAM 
#pragma comment(lib, "../Dependency/commonlib/AHClientInterface.lib")
#else	
#pragma comment(lib, "../Dependency/commonlib/AHClientInterface.lib")
#endif


_FUNC_C_REC pfRec = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Apex Àû¿ë
namespace APEX_CLIENT
{
	long NetSendToGameServer( const char * pBuffer,int nLen )
	{
		if ( (nLen <= 0) || (nLen > MAX_APEX_PACKET_LENGTH) )
			return -1;

		NET_APEX_DATA nmg;
		nmg.nmg.dwSize = sizeof(NET_MSG_GENERIC) + nLen;
		memcpy( nmg.szData, pBuffer, nLen );

		NETSEND( (NET_MSG_GENERIC*) &nmg );
		return 0;
	}

	int StartApexClient()
	{
		return CHCStart( NetSendToGameServer, pfRec );
	}

	int StopApexClient()
	{
		CHCEnd();
		return 0;
	}

	int NoticeApex_UserData(const char * pBuf,int nBufLen)
	{
		if(pfRec)
		{
			pfRec(pBuf,nBufLen);			
		}
		return 0;
	}
};