#include "pch.h"
#include "s_CPatch.h"
#include "s_NetGlobal.h"
#include "s_CClientConsoleMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPatch::CPatch() 
	: m_pIsession(NULL)
	, m_pConnect(NULL)
	, m_pFileSizeUL(NULL)
	, m_pReceivedUL(NULL)
	, m_pbForceTerminate(NULL)
	, m_bPassive(FALSE)
{	
	::memset ( LocalFullPath, 0, sizeof(char) * (MAX_PATH+1) );
	::memset ( ServerName, 0, sizeof(char) * (MAX_FTP_SERVER_NAME+1));
	::memset ( UserID, 0, sizeof(char) * (MAX_FTP_USER_ID+1));			// ftp server user id
	::memset ( UserPass, 0, sizeof(char) *(MAX_FTP_PASSWORD+1));			// ftp server user password
}

CPatch::~CPatch()
{		
	if (m_pConnect)
		delete m_pConnect;
	if (m_pIsession)
		delete m_pIsession;
}

void CPatch::SetProgressValue(ULONGLONG* pReceivedUL, ULONGLONG* pFileSizeUL)
{	
	m_pReceivedUL = pReceivedUL;
	m_pFileSizeUL = pFileSizeUL;
}

void CPatch::SetForceTerminateToggle ( BOOL* pForceTerminate )
{
	m_pbForceTerminate = pForceTerminate;
}

void CPatch::SetServerName(const char* strName)
{
	StringCchCopy(ServerName, MAX_FTP_SERVER_NAME+1, strName);
}

void CPatch::SetServerPort(int nPort)
{
	ServerPort = nPort;
}

void CPatch::SetUserID(const char* strID)
{
	StringCchCopy(UserID, MAX_FTP_USER_ID+1, strID);
}

void CPatch::SetUserPass(const char* strPass)
{
	StringCchCopy(UserPass, MAX_FTP_PASSWORD+1, strPass);
}

void CPatch::SetServer(const char* strName, int nPort, const char* strID, const char* strPass)
{
	SetServerName(strName);
	SetServerPort(nPort);	
	SetUserID(strID);
	SetUserPass(strPass);
}

int CPatch::Connect(const char* strName, int nPort, const char* strID, const char* strPass, BOOL bPassive)
{
	SetServerName(strName);
	SetServerPort(nPort);	
	SetUserID(strID);
	SetUserPass(strPass);

	m_bPassive = bPassive;

	m_pIsession = new CInternetSession("Patch Program");
	if (m_pIsession == NULL)
	{			
		CDebugSet::ToLogFile("CInternetSession NULL");
		return NET_ERROR;
	}
	
	try 
	{
		if (m_bPassive)
		{		
			// Passive Mode 
			m_pConnect = m_pIsession->GetFtpConnection(
					ServerName, 
					UserID, 
					UserPass, 
					ServerPort,
					TRUE);
		}
		else 
		{
			// Normal Mode
			m_pConnect = m_pIsession->GetFtpConnection(
				ServerName, 
				UserID, 
				UserPass, 
				ServerPort,
				FALSE);
		}
	}
	catch(CInternetException* pEx)
	{
		//TCHAR szCause[255];
		//pEx->GetErrorMessage(szCause, 255);
		//CDebugSet::ToLogFile(szCause);
		pEx->Delete(); 
		m_pIsession->Close();
		SAFE_DELETE(m_pIsession);
		return NET_ERROR;		
	}
	m_pConnect->GetCurrentDirectory(strFirstDir);
	strCrtDir = strFirstDir;

	return NET_OK;
}

void CPatch::DisConnect()
{
	if ( m_pConnect )
	{
		m_pConnect->Close();
		SAFE_DELETE(m_pConnect);
	}
}

// ftp 의 현재 디렉토리를 변경한다.
int CPatch::SetCurrentDirectory(LPCTSTR pstrDirName)
{
	int nRetCode = 0;
	if (m_pConnect)
	{

		CString strTemp(pstrDirName);
		strTemp = strTemp.MakeLower();
		CString strFullPath = strFirstDir + "/" + strTemp + "/";
		
		strFullPath.Replace("\\", "/");
		strFullPath.Replace("//", "/");		

		CString strToken;
		int nPos=0;

		strToken = strFullPath.Tokenize("/", nPos);
		while (strToken != "")
		{
			m_pConnect->SetCurrentDirectory(strToken);
			strToken = strFullPath.Tokenize("/", nPos);
		}		
		strCrtDir = strFullPath + "/";
		strCrtDir.Replace("//", "/");
	}

	if (nRetCode == 0)
		return NET_ERROR;
	else
		return NET_OK;
}

// 현재 ftp 디렉토리 위치를 반환한다.
int CPatch::GetCurrentDirectory( LPTSTR pstrDirName, LPDWORD lpdwLen)
{
	int nRetCode = 0;
	if (m_pConnect)
		nRetCode = GetCurrentDirectory(pstrDirName, lpdwLen);

	if (nRetCode == 0)
		return NET_ERROR;
	else
		return NET_OK;
}

// ftp 의 현재 디렉토리에서 해당 파일을 가져온다.
int CPatch::GetFile(const char* filename)
{
	int			nRetCode = 0;	// Return code
	int			nPercent = 0;
	ULONGLONG	m_filesize = 0; // File Size	
	HANDLE		hFile = NULL;	//New File	
	UINT		m_iReadBufSize = 0;	
	DWORD		dwWritten;		// Received Bytes
	ULONGLONG	dwTbytes = 0;	// Received Total Bytes
	CString		str;			// Temp String
	UINT		nBufSize = 1024;
	UCHAR		m_rBuf[1024] = {0};	// Must be same nBufSize, Receive Buffer


	if ( !m_pbForceTerminate )
	{
		return NET_ERROR;
	}

	
	//Open From FTP Site.
	CInternetFile* pInternetFile = NULL;
	CFtpFileFind finder(m_pConnect);
	
	CString strFullPath;
	strFullPath = strCrtDir + filename;

	nRetCode = finder.FindFile(strFullPath, INTERNET_FLAG_RELOAD);
	// nRetCode = finder.FindFile(strFullPath, INTERNET_FLAG_EXISTING_CONNECT);
	if (nRetCode == 0)
	{		
		finder.Close();
		return NET_ERROR;
	}
	finder.FindNextFile();

	try 
	{	
		pInternetFile = m_pConnect->OpenFile(strFullPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 1);
	}
	catch (CInternetException* pEx)
	{	
		TCHAR szCause[255] = {0};
		pEx->GetErrorMessage(szCause, 255);		
		finder.Close();
		delete m_pIsession;
		m_pIsession = NULL;
		return NET_ERROR;
	}

	if (pInternetFile == NULL)
	{	
		finder.Close();
		return NET_ERROR;	
	}
	
	m_filesize = finder.GetLength();
	if ( m_pFileSizeUL ) *m_pFileSizeUL = m_filesize;
	
	// Create NewFile
	char szFullPathFile[MAX_PATH+1] = {0};
	StringCchCopy ( szFullPathFile, MAX_PATH+1, LocalFullPath );
	StringCchCat  ( szFullPathFile, MAX_PATH+1, filename );
	hFile = ::CreateFile(szFullPathFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	try
	{
		while ( 1 )
		{
			//	강제 종료
			if ( m_pbForceTerminate && *m_pbForceTerminate ) return NET_OK;

			pInternetFile->SetWriteBufferSize(nBufSize);
			pInternetFile->SetReadBufferSize(nBufSize);		
					
			// Read File to Buffer step by nBufSize.
			m_iReadBufSize = pInternetFile->Read(m_rBuf, nBufSize);		
			
			if (m_iReadBufSize == 0)
			{
				// end of file.
				break;
			}
			else
			{
				// Write file from buffer.
				WriteFile(hFile, m_rBuf, m_iReadBufSize, &dwWritten, NULL);
				dwTbytes += dwWritten;			
				// Move Progress Bar.
				if ( m_pReceivedUL ) *m_pReceivedUL = dwTbytes;
			}
			Sleep( 0 );
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR szCause[255] = {0};
		pEx->GetErrorMessage(szCause, 255);		
	}

	// Close File
	CloseHandle(hFile);
	pInternetFile->Close();
	finder.Close();
	delete pInternetFile;

	return NET_OK;
}

void CPatch::SetLocalDir( char* szLocalFullPath )
{
	StringCchCopy ( LocalFullPath, MAX_PATH+1, szLocalFullPath );
}