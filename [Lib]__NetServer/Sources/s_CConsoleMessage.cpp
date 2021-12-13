#include "pch.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConsoleMessage* CConsoleMessage::SelfInstance = NULL;

CConsoleMessage::CConsoleMessage(int nType) 
	: m_pLog( NULL )
	, m_pEditBox( NULL )
	, m_pEditBoxInfo( NULL )
	, m_pLogNPROTECT( NULL )
	, m_pLogDB( NULL )
	, m_pLogAPEX( NULL )
	, m_pLogChar( NULL )
{		
	m_pLog         = new SERVER_UTIL::CLogSystem( _T("NETWORK") );
	m_pLogNPROTECT = new SERVER_UTIL::CLogSystem( _T("NPROTECT") );
	m_pLogDB       = new SERVER_UTIL::CLogSystem( _T("DB") );
	m_pLogAPEX     = new SERVER_UTIL::CLogSystem( _T("APEX") );
	m_pLogChar	   = new SERVER_UTIL::CLogSystem( _T("CharSave") );
	m_nDefaultType = nType;
}

CConsoleMessage::~CConsoleMessage()
{
	SAFE_DELETE( m_pLog );
	SAFE_DELETE( m_pLogNPROTECT );
	SAFE_DELETE( m_pLogDB );
	SAFE_DELETE( m_pLogAPEX );
	SAFE_DELETE( m_pLogChar );
}

CConsoleMessage* CConsoleMessage::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CConsoleMessage();
	return SelfInstance;
}

void CConsoleMessage::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		SAFE_DELETE(SelfInstance);
	}
}

void CConsoleMessage::SetControl(HWND hControl, HWND hEditBoxInfo)
{
	m_pEditBox = hControl;
	m_pEditBoxInfo = hEditBoxInfo;
	// only accept n k of text
	// n * 1024 = n k
	SendMessage( m_pEditBox, EM_LIMITTEXT, (WPARAM) (64 * 1024), (LPARAM) 0 );
	SendMessage( m_pEditBoxInfo, EM_LIMITTEXT, (WPARAM) (32 * 1024), (LPARAM) 0 );	
}

HWND CConsoleMessage::GetControl(void)
{
	return m_pEditBox;
}

void CConsoleMessage::WriteDatabase( const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};

	va_list ap;
	va_start( ap, msg );
	StringCchVPrintf( szBuf, C_BUFFER_SIZE+1, msg, ap );
	va_end( ap );
	Write( LOG_TEXT_DATABASE, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteAPEX( const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);	
	va_end(ap);	
	Write( LOG_TEXT_APEX, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteNPROTECT( const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);	
	va_end(ap);	
	Write( LOG_TEXT_NPROTECT, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteFile(
	const TCHAR* msg,
	... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);
	// StringCbVPrintf ( szBuf, C_BUFFER_SIZE, msg, ap );
	va_end(ap);	

	Write( LOG_TEXT_CONSOLE, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteCharSave( const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);	
	va_end(ap);	
	Write( LOG_TEXT_CHARSAVE, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteConsole(
	const TCHAR* msg,
	... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);
	// StringCbVPrintf ( szBuf, C_BUFFER_SIZE, msg, ap );
	va_end(ap);	

	Write( LOG_CONSOLE, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::Write(
	const TCHAR* msg,
	... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);
	// StringCbVPrintf ( szBuf, C_BUFFER_SIZE, msg, ap );
	va_end(ap);	

	Write( LOG_TEXT_CONSOLE, C_MSG_NORMAL, szBuf);
}

void CConsoleMessage::Write( int nType, const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start( ap, msg );
	StringCchVPrintf( szBuf, C_BUFFER_SIZE+1, msg, ap );
	// StringCbVPrintf ( szBuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);

	Write( nType, C_MSG_NORMAL, szBuf );
}

void CConsoleMessage::WriteInfo( const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start( ap, msg );
	StringCchVPrintf( szBuf, C_BUFFER_SIZE, msg, ap );	
	va_end(ap);

	Write( LOG_CONSOLE, C_MSG_INFO, szBuf );
}

void CConsoleMessage::WriteInfo( int nType, const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	

	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf( szBuf, C_BUFFER_SIZE+1, msg, ap );	
	va_end(ap);	

	Write(nType, C_MSG_INFO, szBuf);
}


void CConsoleMessage::Write( int nType, int nPosition, const TCHAR* msg, ... )
{
	if (msg == NULL) return;

	// Memory allocation for buffer
	TCHAR szBuf[C_BUFFER_SIZE+1] = {0};	
	
	LockOn();
	va_list ap;
	va_start(ap, msg);
	StringCchVPrintf(szBuf, C_BUFFER_SIZE+1, msg, ap);	
	va_end(ap);	

	switch (nType)
	{
	case LOG_TEXT_NPROTECT:
		if (m_pLogNPROTECT) m_pLogNPROTECT->Write(szBuf); // Write to file
		break;		
	case LOG_TEXT_APEX:
		if (m_pLogAPEX) m_pLogAPEX->Write(szBuf); // Write to file
		break;
	case LOG_TEXT_DATABASE:
		if (m_pLogDB) m_pLogDB->Write(szBuf); // Write to file
		UpdateEditControl( szBuf, nPosition );
		break;
	case LOG_TEXT_CONSOLE:
		if (m_pLog)	m_pLog->Write(szBuf); // Write to file
		UpdateEditControl( szBuf, nPosition );
		break;		
	case LOG_TEXT: 
		if (m_pLog)	m_pLog->Write(szBuf); // Write to file
		break;
	case LOG_CONSOLE:
		UpdateEditControl( szBuf, nPosition );
		break;
	case LOG_TEXT_CHARSAVE:
		if (m_pLogChar) m_pLogChar->Write(szBuf);
		break;
	default:
		break;
	}
	LockOff();
}

void CConsoleMessage::UpdateEditControl( TCHAR* szMsg, int nPosition )
{
	switch (nPosition)
	{
	case C_MSG_NORMAL:
        UpdateEditControl( szMsg ); // Write to Console
		break;
	case C_MSG_INFO:
		UpdateEditControlInfo( szMsg );
		break;
	default:
		break;
	};
}

void CConsoleMessage::UpdateEditControl( TCHAR* sbuf )
{
	if ( !sbuf ) return;
	if ( !m_pEditBox ) return;

	DWORD dwRetCode = 0;
	
	// Update Control and Scroll to end of the line
	TCHAR TempBuffer[C_BUFFER_SIZE+3] = {0};
	::StringCchCopy( TempBuffer, C_BUFFER_SIZE,   sbuf );	
	::StringCchCat ( TempBuffer, C_BUFFER_SIZE+3, _T("\r\n") );	

	/*
	::SendMessageTimeout( m_pEditBox, EM_SETSEL,      (WPARAM) UINT_MAX-1,  (LPARAM) UINT_MAX,   SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBox, EM_SCROLL,	  (WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBox, EM_SCROLLCARET, (WPARAM) 0,			(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBox, EM_REPLACESEL,  (WPARAM) FALSE,		(LPARAM) TempBuffer, SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBox, EM_LINESCROLL,  (WPARAM) 0,			(LPARAM) 1,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBox, EM_SETSEL,	  (WPARAM) -1,			(LPARAM) UINT_MAX,	 SMTO_BLOCK, 15, &dwRetCode );
	*/
	
	::SendMessageCallback( m_pEditBox, EM_SETSEL,      (WPARAM) UINT_MAX-1,  (LPARAM) UINT_MAX, NULL, (ULONG_PTR) &dwRetCode );
	::SendMessageCallback( m_pEditBox, EM_SCROLL,      (WPARAM) SB_PAGEDOWN, (LPARAM) 0,	    NULL, (ULONG_PTR) &dwRetCode );	
	::SendMessageCallback( m_pEditBox, EM_SCROLLCARET, (WPARAM) 0,			 (LPARAM) 0,		NULL, (ULONG_PTR) &dwRetCode );	
	::SendMessageTimeout(  m_pEditBox, EM_REPLACESEL,  (WPARAM) FALSE,		 (LPARAM) TempBuffer, SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageCallback( m_pEditBox, EM_LINESCROLL,  (WPARAM) 0,			 (LPARAM) 1,		NULL, (ULONG_PTR) &dwRetCode );
	::SendMessageCallback( m_pEditBox, EM_SETSEL,	   (WPARAM) -1,		     (LPARAM) UINT_MAX, NULL, (ULONG_PTR) &dwRetCode );
	
}

void CConsoleMessage::UpdateEditControlInfo( TCHAR* sbuf )
{
	if ( !sbuf ) return;
	if ( !m_pEditBoxInfo ) return;

	DWORD dwRetCode = 0;
	
	// Update Control and Scroll to end of the line	
	TCHAR TempBuffer[C_BUFFER_SIZE+3] = {0};
	::StringCchCopy( TempBuffer, C_BUFFER_SIZE,   sbuf );
	::StringCchCat ( TempBuffer, C_BUFFER_SIZE+3, _T("\r\n") );

	/*
	::SendMessageTimeout( m_pEditBoxInfo, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX,	 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBoxInfo, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBoxInfo, EM_SCROLLCARET,   (WPARAM) 0,				(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBoxInfo, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) TempBuffer, SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBoxInfo, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1,			 SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageTimeout( m_pEditBoxInfo, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX,	 SMTO_BLOCK, 15, &dwRetCode );
	*/

	::SendMessageCallback( m_pEditBoxInfo, EM_SETSEL,      (WPARAM) UINT_MAX-1,  (LPARAM) UINT_MAX, NULL, (ULONG_PTR) &dwRetCode );
	::SendMessageCallback( m_pEditBoxInfo, EM_SCROLL,      (WPARAM) SB_PAGEDOWN, (LPARAM) 0,	    NULL, (ULONG_PTR) &dwRetCode );	
	::SendMessageCallback( m_pEditBoxInfo, EM_SCROLLCARET, (WPARAM) 0,			 (LPARAM) 0,		NULL, (ULONG_PTR) &dwRetCode );	
	::SendMessageTimeout(  m_pEditBoxInfo, EM_REPLACESEL,  (WPARAM) FALSE,		 (LPARAM) TempBuffer, SMTO_BLOCK, 15, &dwRetCode );
	::SendMessageCallback( m_pEditBoxInfo, EM_LINESCROLL,  (WPARAM) 0,			 (LPARAM) 1,		NULL, (ULONG_PTR) &dwRetCode );
	::SendMessageCallback( m_pEditBoxInfo, EM_SETSEL,	   (WPARAM) -1,		     (LPARAM) UINT_MAX, NULL, (ULONG_PTR) &dwRetCode );
}