#include "pch.h"

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

#include <Urlmon.h>
//#include <afxisapi.h>

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "strsafe.h" // Safe string function

#include "s_CHttpPatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CHttpPatch::CHttpPatch(void) 
	: m_nPort(80)
	, m_pbForceTerminate ( NULL )
	, m_pReceivedUL ( NULL )
	, m_pFileSizeUL ( NULL )
{	
	memset(m_szError, 0, sizeof(char) * (ERROR_MSG_LENGTH));
}

CHttpPatch::~CHttpPatch(void)
{	
}

void CHttpPatch::SetForceTerminateToggle(BOOL* pForceTerminate )
{
	m_pbForceTerminate = pForceTerminate;
}

void CHttpPatch::SetProgressValue(ULONGLONG* pReceivedUL, ULONGLONG* pFileSizeUL)
{	
	m_pReceivedUL = pReceivedUL;
	m_pFileSizeUL = pFileSizeUL;
}

int	CHttpPatch::SetBaseURL(CString strBaseURL)
{
	if (!::AfxParseURL(strBaseURL, m_dwServerType, m_strServer, m_strObject, m_nPort))
	{
		// 잘못된 URL
		StringCchCopy(m_szError, ERROR_MSG_LENGTH, _T("AfxParserURL ERROR"));
		return NET_ERROR;
	}
	else
	{
		m_strBaseURL = strBaseURL;
		return NET_OK;
	}
}

char* CHttpPatch::GetErrMsg()
{
	return m_szError;
}

int CHttpPatch::GetFile(CString strPathFile, CString strLocalFile)
{	
	CInternetSession Session("HTTP Patch program");
	CHttpConnection* pHttp = NULL;
	CHttpFile* pFile = NULL;
	// DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_DONT_CACHE;

	// const TCHAR szHeaders[] = _T("Accept: text/*\r\nUser-Agent: Ran Launcher\r\n");
	const TCHAR szHeaders[] = _T("Accept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\n");

	try
	{
		// www.ran-online.com (0) http://www.ran-online.com (X)
		// nPort : 80 (default)
		pHttp = Session.GetHttpConnection(m_strServer, INTERNET_FLAG_DONT_CACHE, m_nPort);

        CString strFile = m_strObject + strPathFile;
        strFile.Replace(_T("//"), _T("/"));

		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_GET, 
								   strFile, 
								   NULL, 
								   1,
								   NULL, 
								   NULL,
								   dwHttpRequestFlags);

		pFile->AddRequestHeaders(szHeaders);
		pFile->SendRequest();

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);	

		if ( dwRet != HTTP_STATUS_OK)
		{
			StringCchCopy(m_szError, ERROR_MSG_LENGTH, GetErrorMsg(dwRet).GetString());
			pFile->Close();		SAFE_DELETE(pFile);
			pHttp->Close();		SAFE_DELETE(pHttp);			
			return NET_ERROR;
		}

		CString strNewLocation;
		pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
		
		if ( m_pFileSizeUL ) *m_pFileSizeUL = (ULONGLONG) GetLength(strNewLocation);

		pFile->Close();		SAFE_DELETE(pFile);
		pHttp->Close();		SAFE_DELETE(pHttp);		
		Session.Close();

		strPathFile.Trim(_T(" "));
		strPathFile.Replace(_T("\\"), _T("/"));

		CString strUrlFull = m_strBaseURL + strPathFile;
		return GetFile2(strUrlFull, strLocalFile);
	}
	catch (CInternetException* pEx)
	{		
		pEx->GetErrorMessage(m_szError, ERROR_MSG_LENGTH);
		pEx->Delete();
		pFile->Close();		SAFE_DELETE(pFile);
		pHttp->Close();		SAFE_DELETE(pHttp);		
		Session.Close();
		return NET_ERROR;
	}	
	return NET_OK;
}

int	CHttpPatch::GetFile2(CString szURL, CString strLocalFile)
{
	CInternetSession mySession("Ran Patch GetFile2");
	// mysession.SetCookie(m_szSessionURL, "Ran-Online", (LPCTSTR) m_szSessionID );

	// DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_TRANSFER_BINARY;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_TRANSFER_BINARY;

    HANDLE hFile = NULL;

	try
	{
		// http://www.gamerworld.org/test/d.zip
		CHttpFile *remotefile = (CHttpFile*) mySession.OpenURL((LPCTSTR) szURL,
																1,
																dwHttpRequestFlags); 
		HANDLE hFile = NULL;
		char szBuff[RECEIVE_BUF_LENGTH] = {0};	
		hFile = ::CreateFile(strLocalFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
		DWORD dwWritten;
		if ( m_pReceivedUL ) *m_pReceivedUL = 0;
		UINT nRead = remotefile->Read(szBuff, RECEIVE_BUF_LENGTH);
		while (nRead > 0)
		{
			//	강제 종료
			if ( m_pbForceTerminate && *m_pbForceTerminate )
			{
				::CloseHandle(hFile);
                remotefile->Close(); 
				SAFE_DELETE(remotefile);
				
				return NET_OK;
			}

			// Write file from buffer.
			if ( m_pReceivedUL ) *m_pReceivedUL += (ULONGLONG) nRead;
			::WriteFile(hFile, szBuff, nRead, &dwWritten, NULL);
			nRead = remotefile->Read(szBuff, RECEIVE_BUF_LENGTH);
			Sleep( 0 );
		}		
		::CloseHandle(hFile);

		remotefile->Close(); 
		SAFE_DELETE(remotefile);

		return NET_OK;
	}
	catch (CInternetException* pEx)
	{
        ::CloseHandle(hFile);        
		pEx->GetErrorMessage(m_szError, ERROR_MSG_LENGTH);
		pEx->Delete();
		return NET_ERROR;
	}
}

// szURL : Full path of URL
// szFileName : Full path of local disk
// Return value
// E_OUTOFMEMORY : The buffer length is invalid or there was insufficient memory to complete the operation. 
// S_OK : The operation succeeded.
HRESULT CHttpPatch::GetFileSimple(LPCTSTR szURL, LPCTSTR szFileName)
{	
	return URLDownloadToFile(NULL, szURL, szFileName, 0, NULL);
}

LONG CHttpPatch::GetLength(CString strHeader)
{
	// Linux / Apache Server
	//
	// HTTP/1.1 200 OK
	// ETag: "16d4d3-d4bb-8d1fd540"
	// Content-Length: 54459
	// Content-Type: application/zip
	// Content-Language: kr
	// Last-Modified: Thu, 18 Mar 2004 11:58:37 GMT

	// Windows 2k / IIS Server
	//
	// HTTP/1.1 200 OK
	// X-Powered-By: ASP.NET
	// Content-Type: application/x-zip-compressed
	// ETag: "a06b22f69e10c41:1468"
	// Content-Length: 54459
	// Last-Modified: Tue, 23 Mar 2004 06:20:39 GMT
	CString strToken;
	int curPos= 0;

	strToken= strHeader.Tokenize(_T(" \r\n"), curPos);
	while (strToken != "")
	{
		if (strToken.CompareNoCase(_T("Content-Length:")) == 0)
		{
			strToken= strHeader.Tokenize(_T(" \r\n"),curPos);
			return ::atol(strToken.GetString());
		}
		strToken= strHeader.Tokenize(_T(" \r\n"),curPos);
		//Sleep( 0 );
	}
	return 0;
}

bool CHttpPatch::SendData( CString strPathFile, CString& strData, CString& strReceive )
{
	if ( strData.IsEmpty() ) return false;
	
	CHttpConnection* pHttp = NULL;
	CInternetSession Session("WhiteRock XML Send");
	CHttpFile* pFile = NULL;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_DONT_CACHE;

	try
	{        
		pHttp = Session.GetHttpConnection( m_strServer, INTERNET_FLAG_DONT_CACHE, m_nPort);

		if ( !pHttp ) 
		{
			Session.Close();
			return false;
		}

		CString strFile = strPathFile;
		strFile.Replace(_T("//"), _T("/"));

		CString strHeader = _T("Content-Type: application/x-www-form-urlencoded");
		
		pFile = pHttp->OpenRequest(CHttpConnection::HTTP_VERB_POST,
									strFile, 
									NULL, 
									1,
									NULL, 
									NULL,
									dwHttpRequestFlags);

		if ( !pFile ) 
		{
			pHttp->Close();	SAFE_DELETE(pHttp);
			Session.Close();
			return false;
		}

		if ( !pFile->SendRequest ( strHeader, (LPVOID)strData.GetBuffer(strData.GetLength()),
							strData.GetLength() ) )
		{
			pFile->Close();	SAFE_DELETE(pFile);
			pHttp->Close();	SAFE_DELETE(pHttp);
			Session.Close();
			return false;
		}

		char szBuff[1025];
		UINT nRead; 	
	  
		int nPacket = 1024;

		// 패킷 단위로 데이터를 받아 파일에 저장
		while (true) 
		{ 
			// 데이터 수신
			nRead = pFile->Read(szBuff, nPacket);
			if(nRead == 0)  break;

			szBuff[nRead] = 0x00;
			strReceive += szBuff;           
		} 

		pFile->Close();		SAFE_DELETE(pFile);
		pHttp->Close();		SAFE_DELETE(pHttp);	

		Session.Close();
		return true;
	}
	catch( CInternetException* pEx)
	{
		pEx->GetErrorMessage(m_szError, ERROR_MSG_LENGTH);
		pEx->Delete();
		pFile->Close();		SAFE_DELETE(pFile);
		pHttp->Close();		SAFE_DELETE(pHttp);
		Session.Close();
		return false;
	}
}

CString CHttpPatch::GetErrorMsg(DWORD dwErrCode)
{
	CString strError;

	switch (dwErrCode)
	{
	case HTTP_STATUS_CONTINUE :
		strError.Format(_T("%d HTTP_STATUS_CONTINUE OK to continue with request"), dwErrCode);
		break;
	case HTTP_STATUS_SWITCH_PROTOCOLS :
		strError.Format(_T("%d HTTP_STATUS_SWITCH_PROTOCOLS server has switched protocols in upgrade header"), dwErrCode);
		break;
	case HTTP_STATUS_OK :
		strError.Format(_T("%d HTTP_STATUS_OK request completed"), dwErrCode);
		break;
	case HTTP_STATUS_CREATED :
		strError.Format(_T("%d HTTP_STATUS_CREATED object created, reason = new URI"), dwErrCode);
		break;
	case HTTP_STATUS_ACCEPTED :
		strError.Format(_T("%d HTTP_STATUS_ACCEPTED async completion (TBS)"), dwErrCode);
		break;
	case HTTP_STATUS_PARTIAL : // 203
		strError.Format(_T("%d HTTP_STATUS_PARTIAL partial completion"), dwErrCode);
		break;
	case HTTP_STATUS_NO_CONTENT : // 204 
		strError.Format(_T("%d HTTP_STATUS_NO_CONTENT no info to return"), dwErrCode);
		break;
	case HTTP_STATUS_RESET_CONTENT : // 205 
		strError.Format(_T("%d HTTP_STATUS_RESET_CONTENT request completed, but clear form"), dwErrCode);
		break;
	case HTTP_STATUS_PARTIAL_CONTENT : // 206 
		strError.Format(_T("%d HTTP_STATUS_PARTIAL_CONTENT partial GET furfilled"), dwErrCode);
		break;
	case HTTP_STATUS_AMBIGUOUS : // 300 
		strError.Format(_T("%d HTTP_STATUS_AMBIGUOUS server couldn't decide what to return"), dwErrCode);
		break;
	case HTTP_STATUS_MOVED : // 301 
		strError.Format(_T("%d HTTP_STATUS_MOVED object permanently moved"), dwErrCode);
		break;
	case HTTP_STATUS_REDIRECT : // 302 
		strError.Format(_T("%d HTTP_STATUS_REDIRECT object temporarily moved"), dwErrCode);
		break; 
	case HTTP_STATUS_REDIRECT_METHOD : // 303 
		strError.Format(_T("%d HTTP_STATUS_REDIRECT_METHOD redirection w/ new access method"), dwErrCode);
		break;
	case HTTP_STATUS_NOT_MODIFIED : // 304 
		strError.Format(_T("%d HTTP_STATUS_NOT_MODIFIED if-modified-since was not modified"), dwErrCode);
		break;
	case HTTP_STATUS_USE_PROXY : // 305 
		strError.Format(_T("%d HTTP_STATUS_USE_PROXY redirection to proxy, location header specifies proxy to use"), dwErrCode);
		break;
	case HTTP_STATUS_REDIRECT_KEEP_VERB : // 307 
		strError.Format(_T("%d HTTP_STATUS_REDIRECT_KEEP_VERB HTTP/1.1: keep same verb"), dwErrCode);
		break;
	case HTTP_STATUS_BAD_REQUEST : // 400 
		strError.Format(_T("%d HTTP_STATUS_BAD_REQUEST invalid syntax"), dwErrCode);
		break;
	case HTTP_STATUS_DENIED : // 401 
		strError.Format(_T("%d HTTP_STATUS_DENIED access denied"), dwErrCode);
		break;
	case HTTP_STATUS_PAYMENT_REQ : // 402 
		strError.Format(_T("%d HTTP_STATUS_PAYMENT_REQ payment required"), dwErrCode);
		break;
	case HTTP_STATUS_FORBIDDEN : // 403 
		strError.Format(_T("%d HTTP_STATUS_FORBIDDEN request forbidden"), dwErrCode);
		break;
	case HTTP_STATUS_NOT_FOUND : // 404 
		strError.Format(_T("%d HTTP_STATUS_NOT_FOUND object not found"), dwErrCode);
		break;
	case HTTP_STATUS_BAD_METHOD : // 405 
		strError.Format(_T("%d HTTP_STATUS_BAD_METHOD method is not allowed"), dwErrCode);
		break;
	case HTTP_STATUS_NONE_ACCEPTABLE : // 406 
		strError.Format(_T("%d HTTP_STATUS_NONE_ACCEPTABLE no response acceptable to client found"), dwErrCode);
		break;
	case HTTP_STATUS_PROXY_AUTH_REQ : // 407 
		strError.Format(_T("%d HTTP_STATUS_PROXY_AUTH_REQ proxy authentication required"), dwErrCode);
		break;
	case HTTP_STATUS_REQUEST_TIMEOUT : // 408 
		strError.Format(_T("%d HTTP_STATUS_REQUEST_TIMEOUT server timed out waiting for request"), dwErrCode);
		break;
	case HTTP_STATUS_CONFLICT : // 409 
		strError.Format(_T("%d HTTP_STATUS_CONFLICT user should resubmit with more info"), dwErrCode);
		break;
	case HTTP_STATUS_GONE : // 410 
		strError.Format(_T("%d HTTP_STATUS_GONE the resource is no longer available"), dwErrCode);
		break;
	case HTTP_STATUS_LENGTH_REQUIRED : // 411 
		strError.Format(_T("%d HTTP_STATUS_LENGTH_REQUIRED the server refused to accept request w/o a length"), dwErrCode);
		break;
	case HTTP_STATUS_PRECOND_FAILED : // 412 
		strError.Format(_T("%d HTTP_STATUS_PRECOND_FAILED precondition given in request failed"), dwErrCode);
		break;
	case HTTP_STATUS_REQUEST_TOO_LARGE : // 413 
		strError.Format(_T("%d HTTP_STATUS_REQUEST_TOO_LARGE request entity was too large"), dwErrCode);
		break;
	case HTTP_STATUS_URI_TOO_LONG : // 414 
		strError.Format(_T("%d HTTP_STATUS_URI_TOO_LONG request URI too long"), dwErrCode);
		break;
	case HTTP_STATUS_UNSUPPORTED_MEDIA : // 415 
		strError.Format(_T("%d HTTP_STATUS_UNSUPPORTED_MEDIA unsupported media type"), dwErrCode);
		break;
	case HTTP_STATUS_RETRY_WITH : // 449 
		strError.Format(_T("%d HTTP_STATUS_RETRY_WITH retry after doing the appropriate action."), dwErrCode);
		break;
	case HTTP_STATUS_SERVER_ERROR : // 500 
		strError.Format(_T("%d HTTP_STATUS_SERVER_ERROR internal server error"), dwErrCode);
		break;
	case HTTP_STATUS_NOT_SUPPORTED : // 501 
		strError.Format(_T("%d HTTP_STATUS_NOT_SUPPORTED required not supported"), dwErrCode);
		break;
	case HTTP_STATUS_BAD_GATEWAY : // 502 
		strError.Format(_T("%d HTTP_STATUS_BAD_GATEWAY error response received from gateway"), dwErrCode);
		break;
	case HTTP_STATUS_SERVICE_UNAVAIL : // 503 
		strError.Format(_T("%d HTTP_STATUS_SERVICE_UNAVAIL temporarily overloaded"), dwErrCode);
		break;
	case HTTP_STATUS_GATEWAY_TIMEOUT : // 504 
		strError.Format(_T("%d HTTP_STATUS_GATEWAY_TIMEOUT timed out waiting for gateway"), dwErrCode);
		break;
	case HTTP_STATUS_VERSION_NOT_SUP : // 505 
		strError.Format(_T("%d HTTP_STATUS_VERSION_NOT_SUP HTTP version not supported"), dwErrCode);
		break;	
	default :
		strError.Format(_T("%d Unknown Error"), dwErrCode);
		break;
	}
	return strError;
}