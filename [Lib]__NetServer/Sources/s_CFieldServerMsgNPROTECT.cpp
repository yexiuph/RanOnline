#include "pch.h"
#include "s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CFieldServer::WriteNprotectLog( DWORD dwClient, DWORD dwResult )
{
	GG_AUTH_DATA Query = m_pClientManager->nProtectGetAuthQuery( dwClient );
	GG_AUTH_DATA Answer = m_pClientManager->nProtectGetAuthAnswer( dwClient );
    
	TCHAR szMsg[MAX_PATH] = {0};
	StringCchPrintf(
		szMsg,
		MAX_PATH,
		_T("ID(%s) Result(%d) Query(%X %X %X %X) Answer(%X %X %X %X)"),
		m_pClientManager->GetUserID( dwClient ),
		dwResult,
		Query.dwIndex,
		Query.dwValue1,
		Query.dwValue2,
		Query.dwValue3,
		Answer.dwIndex,
		Answer.dwValue1,
		Answer.dwValue2,
		Answer.dwValue3 );

	CConsoleMessage::GetInstance()->WriteNPROTECT(szMsg);

	// 2006-08-16 대만요청
	// DB 에도 로그를 남긴다.	
	CLogHackProgram *pDbAction = new CLogHackProgram(
										m_pClientManager->GetUserNum( dwClient ),
										m_pClientManager->GetChaNum( dwClient ),
										NPROTECT_HACK_NUM,
										szMsg );

	COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pDbAction );
}