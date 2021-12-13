#ifndef S_CCLIENTCONSOLEMESSAGE_H_
#define S_CCLIENTCONSOLEMESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef C_MSG_DEFAULT
#define C_MSG_DEFAULT		1
#endif

#ifndef C_MSG_CONSOLE
#define C_MSG_CONSOLE		1
#endif

#ifndef C_MSG_FILE
#define C_MSG_FILE			2
#endif

#ifndef C_MSG_FILE_CONSOLE
#define C_MSG_FILE_CONSOLE	3
#endif

#ifndef C_MSG_DB
#define C_MSG_DB			4
#endif

#ifndef C_MSG_DB_FILE
#define C_MSG_DB_FILE		5
#endif

/*
#ifndef C_MSG_DB_CONSOLE
#define C_MSG_DB_CONSOLE	6
#endif
*/

#ifndef C_MSG_ALL
#define C_MSG_ALL			7
#endif

#ifndef C_MAX_QUEUE_SIZE
#define C_MAX_QUEUE_SIZE	100
#endif

#ifndef C_BUFFER_SIZE
#define C_BUFFER_SIZE		1024
#endif

class CClientConsoleMessage 
{
public:
	static CClientConsoleMessage* GetInstance();
	static void	ReleaseInstance();

protected:		
	HWND		m_pEditBox;	
	int			nDefaultType;
	CRITICAL_SECTION m_CriticalSection;

public:
	void Write(int nType, const char* msg, ...);
	void Write(const char* msg, ...);
	void WriteWithTime(const char* msg, ...);

	void UpdateEditControl(char* msgbuf);
	void SetControl(HWND hControl);
	HWND GetControl(void);	
	
	void LockOn();
	void LockOff();

private:	
	CClientConsoleMessage(int nType = C_MSG_DEFAULT);
	~CClientConsoleMessage();
	static CClientConsoleMessage* SelfInstance;
};

#endif // S_CCLIENTCONSOLEMESSAGE_H_


