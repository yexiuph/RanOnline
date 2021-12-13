#ifndef S_CPATCH_H_
#define S_CPATCH_H_

// This patch process use MFC
// StdAfx.h must have below line
// #include "pch.h"
// Windows NT Use version 4.0. Implemented as ANSI and Unicode functions. 
// Windows Use Windows 95 and later. Implemented as ANSI and Unicode functions. 
// Header Wininet.h 
// Import library Wininet.lib 
// Minimum availability Internet Explorer 3.0 (ANSI only), 5 (ANSI and Unicode) 
#pragma comment (lib,"Wininet.lib")
// #pragma message ("Auto linking Windows Internet library")
// #pragma message ("Client must have Internet Explorer 3.0 (ANSI only), 5 (ANSI and Unicode)")

#define MAX_FTP_SERVER_NAME 30
#define MAX_FTP_USER_ID     30
#define MAX_FTP_PASSWORD    30

#ifndef MAX_PATH
#define MAX_PATH          260
#endif

class CPatch
{
public :	
	CPatch();
	~CPatch();
protected :
	char	ServerName[MAX_FTP_SERVER_NAME+1]; // ftp server name
	int		ServerPort;			// ftp server port, default 21
	char	UserID[MAX_FTP_USER_ID+1];			// ftp server user id
	char	UserPass[MAX_FTP_PASSWORD+1];			// ftp server user password

	char	LocalFullPath[MAX_PATH+1];		// ftp local path
	CString strFirstDir;
	CString strCrtDir;

	CInternetSession*	m_pIsession;
	CFtpConnection*		m_pConnect;
	BOOL				m_bPassive;
public :
	ULONGLONG*			m_pFileSizeUL;
	ULONGLONG*			m_pReceivedUL;

private:
    BOOL*	m_pbForceTerminate;

public :
	int		Connect(const char* strName = "ftp.ran-online.co.kr", 
					int nPort = 21, 
					const char* strID = "Anonymous", 
					const char* strPass = "Anonymous@ran-online.co.kr",
					BOOL bPassive=FALSE);
	
	void	DisConnect();
	
	void	SetServer(const char* strName = "ftp.ran-online.co.kr", 
					  int nPort = 21, 
					  const char* strID = "Anonymous", 
					  const char* strPass = "Anonymous@ran-online.co.kr");
	void	SetServerName(const char* strName = "ftp.ran-online.co.kr");
	void	SetServerPort(int nPort = 21);
	void	SetUserID    (const char* strID = "Anonymous");
	void	SetUserPass  (const char* strPass = "Anonymous@ran-online.co.kr");	
	int		SetCurrentDirectory(LPCTSTR pstrDirName);
	void	SetLocalDir  (char* szLocalFullPath );
	void	SetProgressValue(ULONGLONG* pReceivedUL, ULONGLONG* pFileSizeUL);
	void	SetForceTerminateToggle(BOOL* pForceTerminate );
	
	int		GetCurrentDirectory(LPTSTR pstrDirName, LPDWORD lpdwLen);
	int		GetFile(const char* filename);	
};

#endif // S_CPATCH_H_