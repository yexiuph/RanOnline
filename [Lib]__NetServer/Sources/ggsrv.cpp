#include "pch.h"
#include "ggsrv.h"
#include "./s_CConsoleMessage.h"

GGAUTHS_API void NpLog(int mode, char* msg)
{
	if(mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode. 
#ifdef WIN32
		OutputDebugString(msg);
#else
		//printf(msg);
		CConsoleMessage::GetInstance()->WriteNPROTECT(msg);
#endif
};

GGAUTHS_API void GGAuthUpdateCallback(PGG_UPREPORT report)
{
	//printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
	//		report->nType==1?"GameGuard Ver":"Protocol Num", 
	//		report->dwBefore,
	//		report->dwNext);

	CConsoleMessage::GetInstance()->WriteNPROTECT( "GGAuth version update [%s] : [%ld] -> [%ld] \n", 
													report->nType==1?"GameGuard Ver":"Protocol Num", 
													report->dwBefore,
													report->dwNext );
};