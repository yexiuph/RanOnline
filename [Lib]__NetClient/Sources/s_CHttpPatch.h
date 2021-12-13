#pragma once

#include <afxinet.h>
#include <Urlmon.h>

// Header Urlmon.h 
// Import library Urlmon.lib 
// Minimum availability Internet Explorer 3.0 
// Minimum operating systems Windows NT 4.0, Windows 95 
#pragma comment (lib,"Urlmon.lib")
//#pragma message ("Auto linking Urlmon library")
#pragma comment (lib,"Wininet.lib")
//#pragma message ("Auto linking Wininet.lib")

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	if(p) { delete (p); (p) = NULL; }
#endif

#ifndef NET_OK
#define NET_OK	1
#endif

#ifndef NET_ERROR
#define NET_ERROR -1
#endif

#ifndef ERROR_MSG_LENGTH 
#define ERROR_MSG_LENGTH 256
#endif

#ifndef RECEIVE_BUF_LENGTH
#define RECEIVE_BUF_LENGTH 1024
#endif

/**
* \ingroup NetClientLib
*
* \par requirements
* win98 or later\n
* win2k or later\n
* MFC\n
*
* \version 1.0
* first version
*
* \date 2002-07-04
*
* \author jgkim
*
* \par license
* Copyright 2002-2006 (c) Mincoms. All rights reserved.
* 
* \todo 
*
* \bug 
*
*/
class CHttpPatch
{
public:
	CHttpPatch(void);
	~CHttpPatch(void);
	
	int		SetBaseURL(CString strBaseURL);
	void	SetPort( int nPort ) { m_nPort = nPort; }
	void	SetServer( const TCHAR* szServer ) { m_strServer = szServer; }
	
	HRESULT GetFileSimple(LPCTSTR szURL, LPCTSTR szFileName);
	int		GetFile      (CString strPathFile, CString strLocalFile);
	int		GetFileSize  (CString szURL);
	char*   GetErrMsg();

	void	SetForceTerminateToggle(BOOL* pForceTerminate );
	void	SetProgressValue(ULONGLONG* pReceivedUL, ULONGLONG* pFileSizeUL);
	
	// Send Data Post
	bool	SendData( CString strPathFile, CString& strData, CString& strReceive );

protected:
	CString m_strBaseURL;
	CString m_strServer;
	CString m_strObject;
	DWORD   m_dwServerType;
	char    m_szError[ERROR_MSG_LENGTH];

	INTERNET_PORT m_nPort;

	BOOL*		m_pbForceTerminate;	//	강제종료
	ULONGLONG*	m_pFileSizeUL;		//	받을 전체 용량
	ULONGLONG*	m_pReceivedUL;		//	현재 받은 용량

	LONG	GetLength(CString strHeader);
	int		GetFile2     (CString szURL, CString strLocalFile);
	CString GetErrorMsg  (DWORD dwErrCode);
};

/******************************************************************************
 * Example code
 ******************************************************************************
#include "pch.h"
#include "HttpPatch.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "s_CHttpPatch.h"

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
		CHttpPatch cPatch;

		if (cPatch.SetBaseURL(_T("http://patch.ran-online.co.kr")))
		{
			cPatch.GetErrMsg();
		}
		if (cPatch.GetFile("/dx.zip", "d:\\temp\\d.zip"))
		{
			cPatch.GetErrMsg();
		}
		if (cPatch.SetBaseURL(_T("http://www.gamerworld.org")))
		{
			cPatch.GetErrMsg();
		}
		if (cPatch.GetFile("/test/dy.zip", "d:\\temp\\d2.zip"))
		{
			cPatch.GetErrMsg();
		}
	}
	return nRetCode;
}
*/