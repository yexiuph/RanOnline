#pragma once

#define RAN_MSG_START   WM_USER + 500
#define RAN_MSG_STOP    WM_USER + 501
#define RAN_MSG_EXIT    WM_USER + 502
#define RAN_MSG_RESTART WM_USER + 503

#define SERVER_CONTROLLER_TIMER_ID          101
#define SERVER_CONTROLLER_TIMER_MILLISECOND 2000

namespace SERVER_CONTROLLER
{
bool bStart = false;
bool bStop = false;
bool bExit = false;
bool bRestart = false;
bool bInit = false;

TCHAR* szServerFileName[] = { 
		{_T("AgentServer.exe")},
		{_T("FieldServer.exe")},
		{_T("SessionServer.exe")},
		{_T("LoginServer.exe")}};

TCHAR* szServerTitle[] = {
		{_T("AgentServer")},
		{_T("FieldServer")},
		{_T("SessionServer")},
		{_T("LoginServer")}};

TCHAR* szCommandOption[] = {
		{_T("start")},
		{_T("stop")},
		{_T("exit")},
		{_T("restart")}};

void checkCmdParameter(LPSTR lpCmdLine)
{
#ifdef UNICODE
	wchar_t strTemp[1024] = {0};
	ansiToUnicode(lpCmdLine, strTemp, 1024);
#else
	char strTemp[1024] = {0};
	strcpy_s(strTemp, lpCmdLine);
#endif // UNICODE

	TCHAR seps[] = _T(" ");
	TCHAR* token = NULL;
	TCHAR* nexttoken = NULL;
	token = ::_tcstok_s(strTemp, seps, &nexttoken);
	while (token != NULL) {
		if (_tcscmp(token, SERVER_CONTROLLER::szCommandOption[0]) == 0) {
			SERVER_CONTROLLER::bStart = true;
			break;
		}
		else if (_tcscmp(token, SERVER_CONTROLLER::szCommandOption[1]) == 0) {
			SERVER_CONTROLLER::bStop = true;
			break;
		}
		else if (_tcscmp(token, SERVER_CONTROLLER::szCommandOption[2]) == 0) {
			SERVER_CONTROLLER::bExit = true;
			break;
		}
		else if (_tcscmp(token, SERVER_CONTROLLER::szCommandOption[3]) == 0) {
			SERVER_CONTROLLER::bRestart = true;
			break;
		}
		else {
		}
		token = ::_tcstok_s(NULL, seps, &nexttoken);
	}	
}

CString	getAppPath()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if (!strCommandLine.IsEmpty())
	{
		DWORD dwFind = strCommandLine.ReverseFind('\\');
		if (dwFind != -1)
		{
			strFullPath = strCommandLine.Left(dwFind);
			
			if (!strFullPath.IsEmpty())
			if (strFullPath.GetAt(0) == '"')
				strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
		}
	}
	
	if (strFullPath.GetLength() < 4)
	{
		TCHAR szTest[MAX_PATH] = {0};
		::GetCurrentDirectory(MAX_PATH, szTest);		
		CString strTest(szTest);
		return strTest;
	}
	else
	{
		return strFullPath;
	}
}

}; // SERVER_CONTROLLER