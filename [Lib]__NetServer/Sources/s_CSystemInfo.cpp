#include "pch.h"
#include "s_CSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace SERVER_UTIL
{

CSystemInfo::CSystemInfo()
{
}
CSystemInfo::~CSystemInfo()
{
}

DWORD CSystemInfo::GetProcessorNumber(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.dwNumberOfProcessors;
}

DWORD CSystemInfo::GetProcessorType(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.dwProcessorType; 
}

WORD CSystemInfo::GetProcessorFamilyCode(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.wProcessorLevel;
}

WORD CSystemInfo::GetProcessorModelNumber(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return HIBYTE(SystemInfo.wProcessorRevision);
}

WORD CSystemInfo::GetProcessorSteppingID(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);		
	return LOBYTE(SystemInfo.wProcessorRevision);
}

// Get CPU name, Support only INTEL CPU
void CSystemInfo::GetProcessorName(char* szPname)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);

	switch(SystemInfo.wProcessorLevel)
	{
	case 6 : // Family code 0110 
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor"));
		// Pentium Celeron or Pentium III
		switch(HIBYTE(SystemInfo.wProcessorRevision))
		{
		case 6 : // 0110
			// Intel Celeron processor, model 6
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Intel Celeron processor, model 6"));
			break;
		case 7 : // 0111
			// Pentium III processor, model 7, and Pentium III Xeon processor, model 7
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor model 7"));
			break;
		case 8 : // 1000
			// Pentium III processor, model 8, Pentium III Xeon processor, model 8, and intel Celeron processor, model 8
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor, model 8"));
			break;
		case 10 : // 1010
			// Pentium III Xeon processor, model A
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III Xeon processor, model A"));
			break;
		case 11 : // 1011
			// Pentium III processor, model B
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor, model B"));
			break;
		case 3 : // 0011
			// Intel Pentium II OverDrive processor
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Intel Pentium II OverDrive processor"));
			break;
		default :
			break;
		}
		break;
	case 15 : // Family code 1111
		// Pentium 4 processor, Mobile Intel Pentium 4 processor, 
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium 4 processor"));
		break;
	default :
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Unknown Processor"));
		break;
	}
}

void CSystemInfo::GetOSName(char* szName)
{
	OSVERSIONINFO OSInfo;
	memset(&OSInfo, 0, sizeof(OSInfo));

	// Set size
	OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	StringCchCopy(szName, MAX_OS_NAME, _T("Unknown OS"));

	if (GetVersionEx((OSVERSIONINFO *) &OSInfo) == FALSE)
	{			
		return;
	}

	switch(OSInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// Microsoft Windows 2003
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 2))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 2003"));

		// Microsoft Windows XP
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 1))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows XP"));

		// Microsoft Windows 2000
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 0))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 2000"));

		// Microsoft Windows NT
		if (OSInfo.dwMajorVersion <= 4)
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows NT"));
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		if((OSInfo.dwMajorVersion == 4) && (!OSInfo.dwMinorVersion)) 
		{		
			// Microsoft Windows 95 OSR2
			if(OSInfo.szCSDVersion[1] == 'C') 
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 95 OSR2"));
			}
			// Microsoft Windows 95
			else
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 95"));
			}			
		}
		if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 10))
		{
			// Microsoft Windows 98 SE
			if(OSInfo.szCSDVersion[1] == 'A')
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 98 SE"));
			}			
			// Microsoft Windows 98
			else
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 98"));
			}
		}
		// Microsoft Windows ME
		if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 90))
			StringCchCopy(szName, MAX_OS_NAME, "Microsoft Windows ME");
		break;
	case VER_PLATFORM_WIN32s:
		// Microsoft Win32s
		StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Win32s"));
		break;
	}
}

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

DWORD CSystemInfo::GetCpuSpeed(void)
{
	const int MAX_TRIES = 10;
	DWORD dwSpeed = 0;
	DWORD dwTestValue = UINT_MAX;
	int   nNumTimes = 0;

	DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	//--------------------------------------------------------
	// Find the first two similarities up to ten times.
	// This should be a maximum of five seconds.
	// 
	while ((dwSpeed != dwTestValue) && (nNumTimes < MAX_TRIES))
	{
		dwTestValue = dwSpeed;
		dwSpeed = CalculateCpuSpeed();
		nNumTimes++;
	}	
	SetPriorityClass(GetCurrentProcess(), dwStartingPriority);
	return dwSpeed;
}

// Stolen from AMD
DWORD CSystemInfo::DetermineTimeStamp(void)
{
	DWORD dwTickVal;
	__asm
	{
		_emit 0Fh
		_emit 31h
		mov   dwTickVal, eax
	}
	return (dwTickVal);
}

DWORD CSystemInfo::CalculateCpuSpeed()
{
	int   nTimeStart = 0;
	int   nTimeStop = 0;
	DWORD dwStartTicks = 0;
	DWORD dwEndTicks = 0;
	DWORD dwTotalTicks = 0;
	DWORD dwCpuSpeed = 0;

	nTimeStart = timeGetTime();
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 1)
		{
			dwStartTicks = DetermineTimeStamp();
			break;
		}
	}
	nTimeStart = nTimeStop;
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 500)    // one-half second
		{
			dwEndTicks = DetermineTimeStamp();
			break;
		}
	}
	dwTotalTicks = dwEndTicks - dwStartTicks;
	dwCpuSpeed = dwTotalTicks / 500000;
	return (dwCpuSpeed);
}

// Retrieves the current directory for the current process
// DWORD nBufferLength : size of directory buffer
// LPTSTR lpBuffer : directory buffer
int CSystemInfo::GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer)
{
	if (::GetCurrentDirectory(nBufferLength, lpBuffer) == 0)
		return -1;
	return 0;
}

CString	CSystemInfo::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
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

MEMORYSTATUSEX CSystemInfo::GetMemoryStatus()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex;
}

DWORD CSystemInfo::GetMemoryUsagePercent()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.dwMemoryLoad;
}

DWORDLONG CSystemInfo::GetMemoryPhysicalTotal()
{
	MEMORYSTATUSEX statex;	
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullTotalPhys; 
}

DWORDLONG CSystemInfo::GetMemoryPhysicalAvail()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullAvailPhys; 
}

DWORDLONG CSystemInfo::GetMemoryPhysicalUse()
{
	MEMORYSTATUSEX statex;	
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return (statex.ullTotalPhys - statex.ullAvailPhys);
}

DWORDLONG CSystemInfo::GetMemoryVirtualTotal()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullTotalVirtual;	
}

DWORDLONG CSystemInfo::GetMemoryVirtualAvail()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullAvailVirtual;
}

DWORDLONG CSystemInfo::GetMemoryVirtualUse()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return (statex.ullTotalVirtual - statex.ullAvailVirtual);
}

//void CSystemInfo::GetMemoryUsage()
//{
//	MEMORYSTATUSEX statex;
//	int nDIV = 1024;
//	char *divisor = "K";
//	// Handle the width of the field in which to print numbers this way to
//	// make changes easier. The asterisk in the print format specifier
//	// "%*I64d" takes an int from the argument list, and uses it to pad 
//	// and right-justify the number being formatted.
//	int nWIDTH = 7;
//
//	statex.dwLength = sizeof (statex);
//
//	GlobalMemoryStatusEx (&statex);
//
//	printf ("%ld percent of memory is in use.\n",
//			statex.dwMemoryLoad);
//	printf ("There are %*I64d total %sbytes of physical memory.\n",
//			nWIDTH, statex.ullTotalPhys/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of physical memory.\n",
//			nWIDTH, statex.ullAvailPhys/nDIV, divisor);
//	printf ("There are %*I64d total %sbytes of paging file.\n",
//			nWIDTH, statex.ullTotalPageFile/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of paging file.\n",
//			nWIDTH, statex.ullAvailPageFile/nDIV, divisor);
//	printf ("There are %*I64x total %sbytes of virtual memory.\n",
//			nWIDTH, statex.ullTotalVirtual/nDIV, divisor);
//	printf ("There are %*I64x free %sbytes of virtual memory.\n",
//			nWIDTH, statex.ullAvailVirtual/nDIV, divisor);
//	// Show the amount of extended memory available.
//	printf ("There are %*I64x free %sbytes of extended memory.\n",
//			nWIDTH, statex.ullAvailExtendedVirtual/nDIV, divisor);
//}
BOOL CSystemInfo::SystemDown(int nMethod)
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{			
		if (os.dwMajorVersion == 4) // Microsoft Windows NT
		{
		}				
		if ((os.dwMajorVersion == 5) && (!os.dwMinorVersion)) // Microsoft Windows 2000
		{
		}				
		if ((os.dwMajorVersion == 5) && (os.dwMinorVersion == 1)) // Microsoft Windows XP
		{
		}				
	}
	else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 95, 98, ME
	{
	}
	else
	{
	}
	return ExitWindowsEx(nMethod ,EWX_FORCE);
}

BOOL CSystemInfo::SystemLogoff()
{
	return SystemDown(EWX_LOGOFF);
}

BOOL CSystemInfo::SystemShutdown()
{
	return SystemDown(EWX_SHUTDOWN);		
}

BOOL CSystemInfo::SystemReboot()
{	
	return SystemDown(EWX_REBOOT);
}

BOOL CSystemInfo::SystemPoweroff()
{
	return SystemDown(EWX_POWEROFF);
}

}; // namespace SERVER_UTIL
