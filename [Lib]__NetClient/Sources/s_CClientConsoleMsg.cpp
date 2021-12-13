#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

#include "pch.h"

#include <stdio.h>
#include "s_CClientConsoleMsg.h"

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include "strsafe.h" // Safe string function

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientConsoleMessage* CClientConsoleMessage::SelfInstance = NULL;

CClientConsoleMessage::CClientConsoleMessage(int nType)
{
	::InitializeCriticalSection(&m_CriticalSection);
	m_pEditBox		= NULL;
	nDefaultType	= nType;
}

CClientConsoleMessage::~CClientConsoleMessage()
{
	::DeleteCriticalSection(&m_CriticalSection);
}


CClientConsoleMessage* CClientConsoleMessage::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CClientConsoleMessage();
	return SelfInstance;
}

void CClientConsoleMessage::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}


void CClientConsoleMessage::LockOn()
{
	::EnterCriticalSection(&m_CriticalSection);
}

void CClientConsoleMessage::LockOff()
{
	::LeaveCriticalSection(&m_CriticalSection);
}

void CClientConsoleMessage::SetControl(HWND hControl)
{
	m_pEditBox = hControl;	
	// only accept 10k of text
	// 10 * 1024 = 10k
	::SendMessage(m_pEditBox, EM_LIMITTEXT, (WPARAM) (10 * 1024), (LPARAM) 0);	
}

HWND CClientConsoleMessage::GetControl(void)
{
	return m_pEditBox;
}

void CClientConsoleMessage::Write(const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE] = {0};

	// va_start            va_arg              va_end
    // va_list             va_dcl (UNIX only)
	va_list ap;
	va_start(ap, msg);
	// StringCbVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	StringCchVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	
	
	Write(C_MSG_CONSOLE, sbuf);
}

void CClientConsoleMessage::WriteWithTime(const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE] = {0};

	// va_start            va_arg              va_end
    // va_list             va_dcl (UNIX only)
	va_list ap;
	va_start(ap, msg);
	// StringCbVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	StringCchVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);

	CString strTemp(sbuf);
	CTime cTime = CTime::GetCurrentTime();
	CString strTime = cTime.Format("(%H:%M:%S)");
	CString strWrite = strTime + strTemp;
	Write(C_MSG_CONSOLE, strWrite.GetString());
}

void CClientConsoleMessage::Write(int nType, const char* msg, ...)
{
	// Memory allocation for buffer
	char sbuf[C_BUFFER_SIZE] = {0};	
	
	LockOn();	

	// va_start            va_arg              va_end
    // va_list             va_dcl (UNIX only)
	va_list ap;
	va_start(ap, msg);
	// StringCbVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	StringCchVPrintf(sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	switch(nType)
	{
	case C_MSG_CONSOLE :		
		UpdateEditControl(sbuf); // Write to Console
		break;
	case C_MSG_FILE : 
		// if(m_pLog != NULL)	m_pLog->Write(sbuf); // Write to file
		break;	
	case C_MSG_FILE_CONSOLE	:
		// if(m_pLog != NULL)	m_pLog->Write(sbuf); // Write to file
		UpdateEditControl(sbuf); // Write to Console
		break;
	case C_MSG_DB : // todo
		// todo : write to db
		break;	
	case C_MSG_DB_FILE :
		// todo : write to db
		// if(m_pLog != NULL)	m_pLog->Write(sbuf); // Write to file			
		break;
	/*
	case C_MSG_DB_CONSOLE :
		// todo : write to db
		UpdateEditControl(sbuf); // Write to Console
		break;
	*/
	case C_MSG_ALL :
		// todo : write to db
		// if(m_pLog != NULL)	m_pLog->Write(sbuf); // Write to file
		UpdateEditControl(sbuf); // Write to Console
		break;
	default:
		break;
	}		
	LockOff();
}

void CClientConsoleMessage::UpdateEditControl(char* sbuf)
{
	if (!m_pEditBox) return;

	DWORD dwRetCode;
	// Update Control and Scroll to end of the line	
	char TempBuffer[C_BUFFER_SIZE+3] = {0};
	::StringCchCopy(TempBuffer, C_BUFFER_SIZE,   sbuf);
	::StringCchCat (TempBuffer, C_BUFFER_SIZE+3, "\r\n");	
	
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX,	 SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SCROLLCARET,(WPARAM) 0,				(LPARAM) 0,			 SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) TempBuffer, SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1,			 SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX,	 SMTO_BLOCK, 15, &dwRetCode);
}