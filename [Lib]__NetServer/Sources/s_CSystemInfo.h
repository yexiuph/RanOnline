///////////////////////////////////////////////////////////////////////////////
/// s_CSystemInfo.h
///
/// * History
/// 2002.05.30 jgkim First Coding
/// 2003.01.18 jgkim Add memory function
///
/// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
/// 
/// * Note 
/// Some of class member functions do not support Windows 95/98/Me.
/// Use Windows 2000 or Higher OS
///
/// This class need winmm lib, must link with Winmm.lib
///
/// #pragma comment (lib,"Winmm.lib")
/// #pragma message ("Auto linking Windows mm library")
///
///////////////////////////////////////////////////////////////////////////////
#ifndef S_CSYSTEM_INFO_H_
#define S_CSYSTEM_INFO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <Mmsystem.h>


namespace SERVER_UTIL
{

#define MAX_PROCESSOR_NAME 50
#define MAX_OS_NAME        50

	/**
	* \ingroup NetServerLib
	*
	* \par requirements
	* win98 or later\n
	* win2k or later\n
	* MFC\n
	*
	* \version 1.0
	* first version
	*
	* \date 2002.05.30
	*
	* \author jgkim
	*
	* \par license
	* Copyright 2002-2005 (c) Mincoms. All rights reserved.
	* 
	* \todo 
	*
	* \bug 
	*
	*/
	class CSystemInfo
	{
	public:
		CSystemInfo();
		~CSystemInfo();
	public:
		DWORD	GetProcessorNumber(void);
		DWORD	GetProcessorType(void);
		WORD	GetProcessorFamilyCode(void);
		WORD	GetProcessorModelNumber(void);
		WORD	GetProcessorSteppingID(void);
		void	GetProcessorName(char* szPname);

		DWORD	CalculateCpuSpeed();
		
		DWORD	GetCpuSpeed(void);
		void	GetOSName(char* szName);
		int		GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);
		CString	GetAppPath ();

		DWORD	DetermineTimeStamp(void);

		MEMORYSTATUSEX	GetMemoryStatus();

		/**
		* Percentage of system memory usage
		* \return Percentage
		*/
		DWORD			GetMemoryUsagePercent(); 
		DWORDLONG		GetMemoryPhysicalTotal(); // Total bytes of physical memory
		DWORDLONG		GetMemoryPhysicalAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryPhysicalUse();  // Total bytes of useage physical memory
		DWORDLONG		GetMemoryVirtualTotal(); // Total bytes of virtual memory
		DWORDLONG		GetMemoryVirtualAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryVirtualUse(); // Total bytes of useage physical memory


		BOOL	SystemLogoff();
		BOOL	SystemShutdown();
		BOOL	SystemReboot();
		BOOL	SystemPoweroff();

	protected:
		BOOL	SystemDown(int nMethod);
	};	
}
#endif // S_CSYSTEM_INFO_H_