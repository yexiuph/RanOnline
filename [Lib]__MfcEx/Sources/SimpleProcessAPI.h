/*=========================================================================== 
    (c) Copyright 1999, Emmanuel KARTMANN, all rights reserved                 
  =========================================================================== 
    File           : SimpleProcessAPI.h
    $Header: $
    Author         : Emmanuel KARTMANN
    Creation       : Friday 9/24/99
    Remake         : 
  ------------------------------- Description ------------------------------- 

           Declaration of the CSimpleProcessAPI class

  ------------------------------ Modifications ------------------------------ 
    $Log: $  
  =========================================================================== 
*/

#if !defined(AFX_SIMPLEPROCESSAPI_H__6CA809B6_ABB6_11D3_BFE5_0010E3B966CE__INCLUDED_)
#define AFX_SIMPLEPROCESSAPI_H__6CA809B6_ABB6_11D3_BFE5_0010E3B966CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// NT Implementation: Uses Registry PERF DATA and PSAPI
#   include "winperf.h"
#   include "psapi.h"

#   define INITIAL_SIZE        51200
#   define EXTEND_SIZE         25600
#   define REGKEY_PERF         _T("software\\microsoft\\windows nt\\currentversion\\perflib")
#   define REGSUBKEY_COUNTERS  _T("Counters")
#   define PROCESS_COUNTER     _T("process")
#   define PROCESSID_COUNTER   _T("id process") 


// Windows 98 Implementation: Uses TOOLHELP32
#   include "tlhelp32.h"


/* -----------------------------------------------------------------
CLASS

    CSimpleProcessAPI  

    Provide simple and cross-platform API to processes and modules.

DESCRIPTION

    This class provides a simple and cross-platform (Windows NT, 
    Windows 98) access to processes and modules (DLLs).

USAGE

    To use this class:
    <UL>
        <LI>Create an instance of the class
        <LI>Call any process/module function:
        <UL>
            <LI>BuildProcessList()
            <LI>BuildModuleList()
            <LI>GetFirstProcessLockingModule()
            <LI>GetProcessesLockingModule()
            <LI>GetProcessExecutableName()
        </UL>
    </UL>

EXAMPLE

<PRE>

    // Create instance of the class
    CSimpleProcessAPI oProcessAPI;

    // Build list of running processes
    CMapStringToString oProcessPIDNameMap;
    if (oProcessAPI.BuildProcessList(oProcessPIDNameMap)) {

        POSITION hProcessPosition = NULL;

        // Loop on all processes and print their names
        hProcessPosition = PIDNameMap.GetStartPosition();

        if (hProcessPosition != NULL) {
            cout << "PID\t\tProcess Name\n";
            while (hProcessPosition != NULL) {
		        CString ProcessName;
		        CString PIDString;
		        PIDNameMap.GetNextAssoc( hProcessPosition, PIDString, ProcessName );
                cout << PIDString << "\t\t" << ProcessName;
            }
        }
    }
</PRE>

ADMINISTRATIVE

  Author     Emmanuel KARTMANN

  Date       Monday 12/6/99

SEE ALSO

    

----------------------------------------------------------------- */
class CSimpleProcessAPI  
{
public:

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> create an instance of the class
//
// <U>Parameters:</U> none (C++ constructor)
//
// <U>Return value :</U> none (C++ constructor)
//
// <U>Description  :</U> This function loads the system DLL
//                       (PSAPI.DLL for Windows NT and TOOLHLP32.DLL
//                       for Windows 9x) needed to build processes 
//                       and module lists.
//
    CSimpleProcessAPI();

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> delete an instance of the class
//
// <U>Parameters:</U> none (C++ destructor)
//
// <U>Return value :</U> none (C++ destructor)
//
// <U>Description  :</U> 
//
    virtual ~CSimpleProcessAPI();

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> builds the list of running processes
//
// <U>Parameters:</U> 
//
//       [out] ProcessPIDNameMap
//                MFC Map containing all processes (one entry consists
//                of a PID and its associated process name).
//
// <U>Return value :</U> BOOL = TRUE for success, FALSE otherwise
//
// <U>Description  :</U> 
//
    BOOL BuildProcessList(CMapStringToString &ProcessPIDNameMap);

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> builds the list of modules (DLLs) loaded by
//                 a given process.
//
// <U>Parameters:</U> 
//
//       [in] nCurrentPID
//                Unique process identifier (PID)
//       [out] ModuleFileNameList
//                List of loaded module file names (full path)
//
// <U>Return value :</U> BOOL = TRUE for success, FALSE otherwise
//
// <U>Description  :</U> 
//
    BOOL BuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList);

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> find the process(es) who is (are) locking a given
//                 module (DLL).
//
// <U>Parameters:</U> 
//
//       [in] lpszModuleName
//                name of the module (DLL) to find (full path)
//       [in] PIDNameMap
//                list of all processes (PID/Name map)
//       [out] oLoadingProcessMap
//                list of processes who are locking the given DLL
//
// <U>Return value :</U> BOOL = TRUE for success (at least one process 
//                              has been found), FALSE otherwise
//
// <U>Description  :</U> 
//
    BOOL GetProcessesLockingModule(LPCTSTR lpszModuleName, CMapStringToString &PIDNameMap, CMapStringToString &oLoadingProcessMap);

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> find the first process who is locking a given
//                 module (DLL).
//
// <U>Parameters:</U> 
//
//       [in] lpszModuleName
//                name of the module (DLL) to find (full path)
//       [in] PIDNameMap
//                list of all processes (PID/Name map)
//
// <U>Return value :</U> DWORD = PID of process who's locking the DLL
//
// <U>Description  :</U> Implemented via GetProcessesLockingModule()
//
    DWORD GetFirstProcessLockingModule(LPCTSTR lpszModuleName, CMapStringToString &PIDNameMap);

/////////////////////////////////////////////////////////////////////
//
// <U>Purpose:</U> returns the name of the process's executable
//
// <U>Parameters:</U> 
//
//       [in] dwProcessID
//                Unique process identifier (PID)
//
// <U>Return value :</U> CString = executable name
//
// <U>Description  :</U> This function considers that the executable 
//                       is a module loaded by the process whose file 
//                       name extension is ".EXE"
//
    CString GetProcessExecutableName(DWORD dwProcessID);

protected:
	BOOL IsRunningWindowsNT(void);
	BOOL LoadProcessDLL(void);
    BOOL UnLoadProcessDLL();
    typedef BOOL (CSimpleProcessAPI::*BuildProcessListFunctionPtr)(CMapStringToString &ProcessPIDNameMap);
    typedef BOOL (CSimpleProcessAPI::*BuildModuleListFunctionPtr)(DWORD nCurrentPID, CStringList &ModuleFileNameList);
    BuildProcessListFunctionPtr m_pBuildProcessListFunction;
    BuildModuleListFunctionPtr m_pBuildModuleListFunction;
	BOOL WindowsNTBuildProcessList(CMapStringToString &ProcessPIDNameMap);
	BOOL WindowsNTBuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList);
	BOOL Windows9xBuildProcessList(CMapStringToString &ProcessPIDNameMap);
	BOOL Windows9xBuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList);

private:
    HINSTANCE m_hModule;
    // NT Only (PSAPI)
    BOOL EnumProcessModules(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
    DWORD GetModuleFileNameEx(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);
    typedef BOOL (WINAPI *EnumProcessModulesFunctionPtr)(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
    EnumProcessModulesFunctionPtr m_pEnumProcessModulesFunction;
    typedef DWORD (WINAPI *GetModuleFileNameExFunctionPtr)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);
    GetModuleFileNameExFunctionPtr m_pGetModuleFileNameExFunction;

    // Windows 9x Only (TOOLHELP32)
    HANDLE CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
    BOOL Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
    BOOL Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
    BOOL Module32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    BOOL Module32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    typedef HANDLE (WINAPI *CreateToolhelp32SnapshotFunctionPtr)(DWORD dwFlags, DWORD th32ProcessID);
    CreateToolhelp32SnapshotFunctionPtr m_pCreateToolhelp32SnapshotFunction;
    typedef BOOL (WINAPI *Process32FirstOrNextFunctionPtr)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
    Process32FirstOrNextFunctionPtr m_pProcess32FirstFunction;
    Process32FirstOrNextFunctionPtr m_pProcess32NextFunction;
    typedef BOOL (WINAPI *Module32FirstOrNextFunctionPtr)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    Module32FirstOrNextFunctionPtr m_pModule32FirstFunction;
    Module32FirstOrNextFunctionPtr m_pModule32NextFunction;
};

#endif // !defined(AFX_SIMPLEPROCESSAPI_H__6CA809B6_ABB6_11D3_BFE5_0010E3B966CE__INCLUDED_)
