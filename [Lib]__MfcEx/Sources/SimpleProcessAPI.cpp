/*=========================================================================== 
    (c) Copyright 1999, Emmanuel KARTMANN, all rights reserved                 
  =========================================================================== 
    File           : SimpleProcessAPI.h
    $Header: $
    Author         : Emmanuel KARTMANN
    Creation       : Friday 9/24/99 4:57:06 PM
    Remake         : 
  ------------------------------- Description ------------------------------- 

           Implementation of the CSimpleProcessAPI class

  ------------------------------ Modifications ------------------------------ 
    $Log: $  
  =========================================================================== 
*/

#include "pch.h"
#include "SimpleProcessAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleProcessAPI::CSimpleProcessAPI()
    : m_hModule(NULL)
    , m_pBuildProcessListFunction(NULL)
    , m_pBuildModuleListFunction(NULL)
    , m_pEnumProcessModulesFunction(NULL)
    , m_pGetModuleFileNameExFunction(NULL)
    , m_pCreateToolhelp32SnapshotFunction(NULL)
    , m_pProcess32FirstFunction(NULL)
    , m_pProcess32NextFunction(NULL)
    , m_pModule32FirstFunction(NULL)
    , m_pModule32NextFunction(NULL)
{
    // Load library and map function, based on OS type (Windows NT or Windows 95/98)
    LoadProcessDLL();
}

CSimpleProcessAPI::~CSimpleProcessAPI()
{
    UnLoadProcessDLL();
}

BOOL CSimpleProcessAPI::LoadProcessDLL()
{
    BOOL bReturnCode = FALSE;

    UnLoadProcessDLL();

    if (IsRunningWindowsNT()) {

        m_hModule = LoadLibrary("psapi.dll");
        if (m_hModule) {
            m_pBuildProcessListFunction = &CSimpleProcessAPI::WindowsNTBuildProcessList;
            m_pBuildModuleListFunction = &CSimpleProcessAPI::WindowsNTBuildModuleList;
            m_pEnumProcessModulesFunction = (EnumProcessModulesFunctionPtr)GetProcAddress(m_hModule, "EnumProcessModules");
#           ifdef UNICODE
                m_pGetModuleFileNameExFunction = (GetModuleFileNameExFunctionPtr)GetProcAddress(m_hModule, "GetModuleFileNameExW");
#           else
                m_pGetModuleFileNameExFunction = (GetModuleFileNameExFunctionPtr)GetProcAddress(m_hModule, "GetModuleFileNameExA");
#           endif // UNICODE
        }
       
    } else {

        m_hModule = LoadLibrary("kernel32.dll");
        if (m_hModule) {
            m_pBuildProcessListFunction = &CSimpleProcessAPI::Windows9xBuildProcessList;
            m_pBuildModuleListFunction = &CSimpleProcessAPI::Windows9xBuildModuleList;
            m_pCreateToolhelp32SnapshotFunction = (CreateToolhelp32SnapshotFunctionPtr)GetProcAddress(m_hModule, "CreateToolhelp32Snapshot");
#           ifdef UNICODE
            m_pProcess32FirstFunction = (Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Process32FirstW");
            m_pProcess32NextFunction = (Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Process32NextW");
            m_pModule32FirstFunction = (Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Module32FirstW");
            m_pModule32NextFunction = (Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Module32NextW");
#           else
            m_pProcess32FirstFunction = (Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Process32First");
            m_pProcess32NextFunction = (Process32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Process32Next");
            m_pModule32FirstFunction = (Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Module32First");
            m_pModule32NextFunction = (Module32FirstOrNextFunctionPtr)GetProcAddress(m_hModule, "Module32Next");
#           endif // UNICODE
        }

    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::UnLoadProcessDLL()
{
    BOOL bReturnCode = FALSE;
    if (m_hModule) {
        bReturnCode = FreeLibrary(m_hModule);
        m_hModule = NULL;
    }

    return(bReturnCode);
}


BOOL CSimpleProcessAPI::IsRunningWindowsNT()
{
	OSVERSIONINFO versionInfo;

	// set the size of OSVERSIONINFO, before calling the function

	versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	// Get the version information

	if (::GetVersionEx (&versionInfo)) {

	    if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
            return(TRUE);
	    }

    }

    return(FALSE);
}

BOOL CSimpleProcessAPI::BuildProcessList(CMapStringToString &ProcessPIDNameMap)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pBuildProcessListFunction);

    if (m_pBuildProcessListFunction) {
        bReturnCode = ((*this).*m_pBuildProcessListFunction)(ProcessPIDNameMap);
    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::BuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pBuildProcessListFunction);

    if (m_pBuildModuleListFunction) {
        bReturnCode = ((*this).*m_pBuildModuleListFunction)(nCurrentPID, ModuleFileNameList);
    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::WindowsNTBuildProcessList(CMapStringToString &ProcessPIDNameMap)
{
	// NT Implementation: Uses Registry Performance Data
	// Reset list/map
	ProcessPIDNameMap.RemoveAll();

	//This following was coded using the tlist example

	bool						 bErrorOccured=false;
	DWORD                        rc;
    HKEY                         hKeyNames;
    DWORD                        dwType;
    DWORD                        dwSize;
    LPBYTE                       buf = NULL;
    TCHAR                        szSubKey[1024];
    LANGID                       lid;
    LPSTR                        p;
    LPSTR                        p2;
    PPERF_DATA_BLOCK             pPerf;
    PPERF_OBJECT_TYPE            pObj;
    PPERF_INSTANCE_DEFINITION    pInst;
    PPERF_COUNTER_BLOCK          pCounter;
	PPERF_COUNTER_DEFINITION     pCounterDef;
    DWORD                        i;
    DWORD                        dwProcessIdTitle; 
    DWORD                        dwProcessIdCounter; 
	TCHAR                        szProcessName[MAX_PATH] = {0};;
    DWORD                        dwLimit = 256;
	DWORD dwNumTasks;
    lid = MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL);
    _stprintf_s(szSubKey, _T("%s\\%03x"), REGKEY_PERF, lid);
    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                       szSubKey,
                       0,
                       KEY_READ,
                       &hKeyNames
                     );
    if (rc != ERROR_SUCCESS)
	{
		bErrorOccured=true;
        goto exit;

    }

    //
    // get the buffer size for the counter names
    //
    rc = RegQueryValueEx(hKeyNames,
                          REGSUBKEY_COUNTERS,
                          NULL,
                          &dwType,
                          NULL,
                          &dwSize
                        );

    if (rc != ERROR_SUCCESS)
	{
		bErrorOccured=true;
        goto exit;
    }

    //
    // allocate the counter names buffer
    //
    buf = (LPBYTE) malloc(dwSize);
    if (buf == NULL)
	{
		bErrorOccured=true;
        goto exit;
    }
    memset(buf, 0, dwSize);

    //
    // read the counter names from the registry
    //
    rc = RegQueryValueEx( hKeyNames,
                          REGSUBKEY_COUNTERS,
                          NULL,
                          &dwType,
                          buf,
                          &dwSize
                        );

    if (rc != ERROR_SUCCESS) 
	{
		bErrorOccured=true;
        goto exit;
    }

    //
    // now loop thru the counter names looking for the "Process" counters:
    // the buffer contains multiple null terminated strings and then
    // finally null terminated at the end.  the strings are in pairs of
    // counter number and counter name.
    //

    p =(LPSTR) buf;
    while (*p) 
	{
        if (p > (LPSTR)buf) 
		{
            for( p2=p-2; _istdigit(*p2); p2--)
						;
        }
        if (_tcsicmp(p, PROCESS_COUNTER) == 0)
		{
            // look backwards for the counter number
            for(p2=p-2; _istdigit(*p2); p2--) 
				;
            _tcscpy_s(szSubKey, p2 + 1);
        } else {
			if (_stricmp(p, PROCESSID_COUNTER) == 0) {
				// 
				// look backwards for the counter number
				//
				for( p2=p-2; isdigit(*p2); p2--) 
					; 
				dwProcessIdTitle = atol( p2+1 );
			}
		}
        //
		// next string
		// 
        p += (_tcslen(p) + 1);
    }


    // free the counter names buffer
    free(buf);


    // allocate the initial buffer for the performance data

    dwSize = INITIAL_SIZE;
    buf = (LPBYTE)malloc( dwSize );
    if (buf == NULL)
	{
		bErrorOccured=true;
        goto exit;
    }
    memset(buf, 0, dwSize);
    while (true)
	{

        rc = RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                              szSubKey,
                              NULL,
                              &dwType,
                              buf,
                              &dwSize
                            );

        pPerf = (PPERF_DATA_BLOCK) buf;

        // check for success and valid perf data block signature

        if ((rc == ERROR_SUCCESS) &&
            (dwSize > 0) &&
            (pPerf)->Signature[0] == (WCHAR)'P' &&
            (pPerf)->Signature[1] == (WCHAR)'E' &&
            (pPerf)->Signature[2] == (WCHAR)'R' &&
            (pPerf)->Signature[3] == (WCHAR)'F' )
		{
            break;
        }

        // if buffer is not big enough, reallocate and try again

        if (rc == ERROR_MORE_DATA)
		{
            dwSize += EXTEND_SIZE;
            buf = (LPBYTE)realloc( buf, dwSize );
            memset( buf, 0, dwSize );
        }
        else 
		{
			bErrorOccured=true;
			goto exit;
        }
    }

    // set the perf_object_type pointer

    pObj = (PPERF_OBJECT_TYPE) ((DWORD)pPerf + pPerf->HeaderLength);

    // 
    // loop thru the performance counter definition records looking 
    // for the process id counter and then save its offset 
    // 
    pCounterDef = (PPERF_COUNTER_DEFINITION) ((DWORD)pObj + pObj->HeaderLength); 
    for (i=0; i<(DWORD)pObj->NumCounters; ++i) 
	{ 
        if (pCounterDef->CounterNameTitleIndex == dwProcessIdTitle) { 
            dwProcessIdCounter = pCounterDef->CounterOffset; 
            break; 
        } 
        pCounterDef++; 
    } 

    dwNumTasks = min( dwLimit, (DWORD)pObj->NumInstances );
    pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pObj + pObj->DefinitionLength);

    // loop thru the performance instance data extracting each process name

    for (i=0; i<dwNumTasks; ++i)
	{
        //
        // pointer to the process name
        //
        p = (LPSTR) ((DWORD)pInst + pInst->NameOffset);

        //
        // convert it to ascii
        //
        rc = WideCharToMultiByte( CP_ACP,
                                  0,
                                  (LPCWSTR)p,
                                  -1,
                                  szProcessName,
                                  sizeof(szProcessName),
                                  NULL,
                                  NULL
                                );

		if (rc)
		{
   				//m_strArray.Add(szProcessName);
				TRACE1("%s\t", szProcessName);
		}
        // get the process id
        pCounter = (PPERF_COUNTER_BLOCK) ((DWORD)pInst + pInst->ByteLength);
        DWORD nProcessId = *((LPDWORD) ((DWORD)pCounter + dwProcessIdCounter));
		TRACE1("%u\n", nProcessId);
		// Do not add the _Total instance: it's NOT a process
		if (strcmp("_Total", szProcessName) && nProcessId) {
			CString szMapKey;
			szMapKey.Format("%u", nProcessId);
			ProcessPIDNameMap.SetAt(szMapKey, szProcessName);
		}
        // next process
        pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pCounter + pCounter->ByteLength);
    }

exit:
    if (buf) 
	{
        free(buf);
    }

    RegCloseKey(hKeyNames);
    RegCloseKey(HKEY_PERFORMANCE_DATA);
	return !bErrorOccured;
}

BOOL CSimpleProcessAPI::WindowsNTBuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList)
{
// NT Implementation: Uses PSAPI
	BOOL bReturnCode = FALSE;
	HANDLE pProcessHandle = NULL;

	// Empty the list
	ModuleFileNameList.RemoveAll();

	if (nCurrentPID == 0) {
		// Get current process id
		nCurrentPID = GetCurrentProcessId();
		pProcessHandle = GetCurrentProcess();
	} else {
		// Open the process to get an HANDLE on it
		pProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
									 FALSE, // not inherited
									 nCurrentPID);
	}

	if (pProcessHandle != NULL) {
		HMODULE pLoadedModules[4096];
		DWORD nLoadedBytes = 0;
		// Get array of loaded modules
		if (EnumProcessModules(pProcessHandle,
							   pLoadedModules,
							   (DWORD)sizeof(pLoadedModules),
							   &nLoadedBytes)) {
			// Compute effective number of modules
			int nNumberOfModules = nLoadedBytes/sizeof(HMODULE);
			// Loop on all modules
			for (int i=0; i<nNumberOfModules; ++i) {
				// Fetch module file name
				char pFileName[_MAX_PATH];
				CString cleanFileName;
				if (GetModuleFileNameEx(pProcessHandle,
										pLoadedModules[i],
										pFileName,
										_MAX_PATH) > 0) {
                    // Make the module UPPERCASE
                    // Windows NT seems to use random cases for files...
                    cleanFileName=pFileName;
                    cleanFileName.MakeUpper();
                    // Insert module name in list, in alphabetical order
                    POSITION pos = ModuleFileNameList.GetHeadPosition();
                    BOOL bInsertBefore = FALSE;
                    while ( pos != NULL ) {
					    CString FileName = ModuleFileNameList.GetNext(pos);
                        //TRACE1("File at current pos: \"%s\"\n", FileName);
                        if (FileName >= cleanFileName) {
                            bInsertBefore = TRUE;
                            // Go back one position (or go to last)
                            if (pos==NULL) {
                                pos = ModuleFileNameList.GetTailPosition();
                            } else {
                                ModuleFileNameList.GetPrev(pos);
                            }
                            break;
                        }
                    }
                    if (bInsertBefore) {
                        if (pos == NULL) {
                            ModuleFileNameList.AddHead(cleanFileName);
                        } else {
                            ModuleFileNameList.InsertBefore(pos, cleanFileName);
                        }
                    } else {
                        if (pos == NULL) {
                            ModuleFileNameList.AddTail(cleanFileName);
                        } else {
                            ModuleFileNameList.AddHead(cleanFileName);
                        }
                    }

                    // DUMP THE LIST
                    pos = ModuleFileNameList.GetHeadPosition();

					bReturnCode=TRUE;
				}
			}
		}

		// Close process handle
		CloseHandle(pProcessHandle);

	} else {
		TRACE2("Can't open process %u: %d\n", nCurrentPID, GetLastError());
		bReturnCode=FALSE;
	}


	return(bReturnCode);
}

BOOL CSimpleProcessAPI::Windows9xBuildProcessList(CMapStringToString &ProcessPIDNameMap)
{
	// Windows 98 Implementation: Uses TOOLHELP32
    HANDLE         hProcessSnap = NULL; 
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 

    // Reset list/map
	ProcessPIDNameMap.RemoveAll();
 
    //  Take a snapshot of all processes in the system. 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == (HANDLE)-1) {
        return (FALSE); 
	}
 
    //  Fill in the size of the structure before using it. 

    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    //  Walk the snapshot of the processes, and for each process, 
    //  display information. 

    if (Process32First(hProcessSnap, &pe32)) { 
        BOOL          bGotModule = FALSE; 
 
        do 
        { 
			CString szMapKey;
			szMapKey.Format("%u", pe32.th32ProcessID);
			// Heristic: to get process name, get its executable path, extract the filename,
			// and eventually add the file extension.
			TCHAR lpszProcessName[_MAX_PATH];
			TCHAR lpszExecutableFileName[_MAX_FNAME];
			TCHAR lpszExecutableFileExt[_MAX_EXT];
			_splitpath_s(pe32.szExeFile, NULL, 0, NULL, 0, lpszExecutableFileName, _MAX_FNAME, lpszExecutableFileExt, _MAX_EXT);
			StringCchCopy(lpszProcessName,_MAX_PATH,lpszExecutableFileName);

			if (!_stricmp(lpszExecutableFileExt, ".EXE") || !_stricmp(lpszExecutableFileExt, ".DLL"))
			{
					StringCchCat(lpszProcessName,_MAX_PATH,lpszExecutableFileExt);
			}

			ProcessPIDNameMap.SetAt(szMapKey, lpszProcessName);
        } 
        while (Process32Next(hProcessSnap, &pe32)); 
        bRet = TRUE; 
    } 
    else {
        bRet = FALSE;    // could not walk the list of processes 
	}
 
    // Do not forget to clean up the snapshot object. 
    CloseHandle (hProcessSnap); 

    return (bRet); 
}

BOOL CSimpleProcessAPI::Windows9xBuildModuleList(DWORD nCurrentPID, CStringList &ModuleFileNameList)
{
// Windows 98 Implementation: Uses TOOLHELP32
    BOOL          bReturnCode = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 
 
	// Empty the list
	ModuleFileNameList.RemoveAll();

	if (nCurrentPID == 0) {
		// Get current process id
		nCurrentPID = GetCurrentProcessId();
    }

    // Take a snapshot of all modules in the specified process. 

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nCurrentPID); 
    if (hModuleSnap == (HANDLE)-1) 
        return (FALSE); 
 
    // Fill the size of the structure before using it. 

    me32.dwSize = sizeof(MODULEENTRY32); 
 
    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller. 

    if (Module32First(hModuleSnap, &me32)) { 
        do 
        { 
			// Fetch module file name
			CString cleanFileName;
            // Make the module UPPERCASE
            // Windows NT seems to use random cases for files...
            cleanFileName = me32.szExePath;
            cleanFileName.MakeUpper();
            // Insert module name in list, in alphabetical order
            POSITION pos = ModuleFileNameList.GetHeadPosition();
            BOOL bInsertBefore = FALSE;
            while ( pos != NULL ) {
				CString FileName = ModuleFileNameList.GetNext(pos);
                //TRACE1("File at current pos: \"%s\"\n", FileName);
                if (FileName >= cleanFileName) {
                    bInsertBefore = TRUE;
                    // Go back one position (or go to last)
                    if (pos==NULL) {
                        pos = ModuleFileNameList.GetTailPosition();
                    } else {
                        ModuleFileNameList.GetPrev(pos);
                    }
                    break;
                }
            }
            if (bInsertBefore) {
                if (pos == NULL) {
                    ModuleFileNameList.AddHead(cleanFileName);
                } else {
                    ModuleFileNameList.InsertBefore(pos, cleanFileName);
                }
            } else {
                if (pos == NULL) {
                    ModuleFileNameList.AddTail(cleanFileName);
                } else {
                    ModuleFileNameList.AddHead(cleanFileName);
                }
            }

            // DUMP THE LIST
            pos = ModuleFileNameList.GetHeadPosition();
			bReturnCode=TRUE;
		}
        while (Module32Next(hModuleSnap, &me32)); 
 
    } 
    else {
        bReturnCode = FALSE;           // could not walk module list 
	}
 
    // Do not forget to clean up the snapshot object. 
    CloseHandle (hModuleSnap); 
 
    return (bReturnCode); 
}

///////////////////////////////////////////////////////////////////////////////////////
//// Portable Functions (cross-platform)
///////////////////////////////////////////////////////////////////////////////////////


BOOL CSimpleProcessAPI::GetProcessesLockingModule(LPCTSTR lpszModuleName, CMapStringToString &PIDNameMap, CMapStringToString &oLoadingProcessMap)
{
    BOOL bReturnCode = FALSE;
	CString PIDString;
	DWORD nPID=0;
	POSITION hProcessPosition = NULL;

    oLoadingProcessMap.RemoveAll(); // List of processes which loaded the module
	hProcessPosition = PIDNameMap.GetStartPosition();
	while( hProcessPosition != NULL ){
		CString ProcessName;
		CString PIDString;
        int effectivePosition = 0;
		// Get key ( PIDString ) and value ( ProcessName )
		PIDNameMap.GetNextAssoc( hProcessPosition, PIDString, ProcessName );
		// Convert PID to a number
		nPID = atol(PIDString);
        CStringList ModuleList;
        TCHAR lpszSearchedModuleShortName[_MAX_PATH];
        TCHAR lpszCurrentModuleShortName[_MAX_PATH];
        // Convert the searched module name to a SHORT PATH
		StringCchCopy(lpszSearchedModuleShortName,_MAX_PATH,"");
        GetShortPathName(lpszModuleName, lpszSearchedModuleShortName, sizeof(lpszSearchedModuleShortName));
        if (BuildModuleList(nPID, ModuleList)) {

            POSITION hModulePosition = ModuleList.GetHeadPosition();

            while( hModulePosition != NULL ){

	            // Get file name
	            CString FileName = ModuleList.GetNext(hModulePosition);

                // Convert the current module to a SHORT PATH
                StringCchCopy(lpszCurrentModuleShortName,_MAX_PATH,"");
                GetShortPathName(FileName, lpszCurrentModuleShortName, sizeof(lpszCurrentModuleShortName));
                if (!_stricmp(lpszSearchedModuleShortName, lpszCurrentModuleShortName)) {
                    // Module found: store it in map of processes
                    bReturnCode = TRUE;
                    oLoadingProcessMap.SetAt(PIDString,ProcessName);
                    hModulePosition=NULL;
                    hProcessPosition=NULL;
                }
            }
        }
    }

    return(bReturnCode);
}

DWORD CSimpleProcessAPI::GetFirstProcessLockingModule(LPCTSTR lpszModuleName, CMapStringToString &PIDNameMap)
{
    CMapStringToString oLoadingProcessMap;
    DWORD dwFoundPID = 0;

    if (GetProcessesLockingModule(lpszModuleName, PIDNameMap, oLoadingProcessMap)) {
	    POSITION hProcessPosition = NULL;

	    hProcessPosition = oLoadingProcessMap.GetStartPosition();
	    if ( hProcessPosition != NULL ){
		    CString ProcessName;
		    CString PIDString;
		    // Get key ( PIDString ) and value ( ProcessName )
		    oLoadingProcessMap.GetNextAssoc( hProcessPosition, PIDString, ProcessName );
		    // Convert PID to a number
		    dwFoundPID = atol(PIDString);
        }
    }

    return(dwFoundPID);
}

CString CSimpleProcessAPI::GetProcessExecutableName(DWORD dwProcessID)
{
    // Heuristic: the executable is a module loaded by the process whose file name extension is ".EXE"
    CStringList oModuleList;
    CString szExecutableName;
    if (BuildModuleList(dwProcessID, oModuleList)) {
        POSITION hModulePosition = oModuleList.GetHeadPosition();
        TCHAR lpszCurrentModuleLongName[_MAX_PATH+1];
        TCHAR lpszFileExt[_MAX_EXT];
        LPTSTR lpszFileComponent;

        while( hModulePosition != NULL ){

	        // Get file name
	        CString szFileName = oModuleList.GetNext(hModulePosition);

            _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, lpszFileExt, _MAX_EXT);

            if (!_stricmp(lpszFileExt, ".EXE")) {
                // Convert the current module to a LONG PATH
				StringCchCopy(lpszCurrentModuleLongName,_MAX_PATH,"");
                if (GetFullPathName(szFileName, sizeof(lpszCurrentModuleLongName), lpszCurrentModuleLongName, &lpszFileComponent)) {
                    szExecutableName = lpszCurrentModuleLongName;
                    hModulePosition = NULL;
                }
            }
        }
    }
    return(szExecutableName);
}

///////////////////////////////////////////////////////////////////////////////////////
//// NT Only (PSAPI) Functions
///////////////////////////////////////////////////////////////////////////////////////
BOOL CSimpleProcessAPI::EnumProcessModules(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pEnumProcessModulesFunction);

    if (m_pEnumProcessModulesFunction) {
        bReturnCode = (*m_pEnumProcessModulesFunction)(hProcess, lphModule, cb, lpcbNeeded);
    }

    return(bReturnCode);
}

DWORD CSimpleProcessAPI::GetModuleFileNameEx(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize)
{
    DWORD dwReturnCode = FALSE;

    ASSERT(m_pGetModuleFileNameExFunction);

    if (m_pGetModuleFileNameExFunction) {
        dwReturnCode = (*m_pGetModuleFileNameExFunction)(hProcess, hModule, lpFilename, nSize);
    }

    return(dwReturnCode);
}

///////////////////////////////////////////////////////////////////////////////////////
//// Windows 9x Only (TOOLHELP32) Functions
///////////////////////////////////////////////////////////////////////////////////////
HANDLE CSimpleProcessAPI::CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
{
    HANDLE hReturnCode = FALSE;

    ASSERT(m_pCreateToolhelp32SnapshotFunction);

    if (m_pCreateToolhelp32SnapshotFunction) {
        hReturnCode = (*m_pCreateToolhelp32SnapshotFunction)(dwFlags, th32ProcessID);
    }

    return(hReturnCode);
}

BOOL CSimpleProcessAPI::Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pProcess32FirstFunction);

    if (m_pProcess32FirstFunction) {
        bReturnCode = (*m_pProcess32FirstFunction)(hSnapshot, lppe);
    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pProcess32NextFunction);

    if (m_pProcess32NextFunction) {
        bReturnCode = (*m_pProcess32NextFunction)(hSnapshot, lppe);
    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::Module32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pModule32FirstFunction);

    if (m_pModule32FirstFunction) {
        bReturnCode = (*m_pModule32FirstFunction)(hSnapshot, lpme);
    }

    return(bReturnCode);
}

BOOL CSimpleProcessAPI::Module32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
    BOOL bReturnCode = FALSE;

    ASSERT(m_pModule32NextFunction);

    if (m_pModule32NextFunction) {
        bReturnCode = (*m_pModule32NextFunction)(hSnapshot, lpme);
    }

    return(bReturnCode);
}

