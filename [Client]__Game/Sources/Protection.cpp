#include "pch.h"
#include ".\Protection.h"
//#include "GlobalDefine.h"
//#include "DEBUG_NEW.h"


CProtection::CProtection(void)
{
	SecureZeroMemory(&hKey, sizeof(HKEY)); 
	SecureZeroMemory(&dwData, sizeof(DWORD));
	SecureZeroMemory(&lRet, sizeof(LONG));
	SecureZeroMemory(&lpValueName,sizeof(LPCSTR));
	SecureZeroMemory(&CONTEXTDebug, sizeof(CONTEXT));
	SecureZeroMemory(&DR_7, sizeof(DebugReg7*));

	CONTEXTDebug.ContextFlags = CONTEXT_DEBUG_REGISTERS;
}

CProtection::~CProtection(void)
{
	RegistryDelete();	//레지스트리 삭제
}


void CProtection::Init()
{
	RegistryRecord();	//레지스트리 입력
}


CProtection& CProtection::GetInstance()
{
	static CProtection preservationObj;
	return preservationObj;
}

void CProtection::RegistryRecord()
{
	lRet = RegCreateKey(HKEY_LOCAL_MACHINE, "", &hKey);
	if( lRet != ERROR_SUCCESS )//실패
	{
		//		MessageBox(Main_hWnd, "생성 실패", "알림", MB_OK );
		CProtection::WindowsSyStemCompulsoryDown();//레지스트리에 생성 실패시 운영체제 종료
	}
	else//성공  
	{
		//		MessageBox(Main_hWnd, "생성 성공", "알림", MB_OK );
		lRet = RegSetValueEx(hKey, lpValueName, 0,
			REG_DWORD, (LPBYTE) &dwData, sizeof(DWORD));
		if( lRet != ERROR_SUCCESS )//실패
		{
			//			MessageBox(Main_hWnd, "레지스트리 쓰기 실패", "알림", MB_OK );
			CProtection::WindowsSyStemCompulsoryDown();//레지스트리에 쓰기 실패시 운영체제 종료
		}
		else//성공
		{
			//			MessageBox(Main_hWnd, "레지스트리 쓰기 성공", "알림", MB_OK );
		}
	}
	RegCloseKey(hKey);
	/*
	HKEY MyKey = 0;
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Ryu&Soft", 0, KEY_ALL_ACCESS, &MyKey );
	if ( lRet != ERROR_SUCCESS )//실패
	{
	//		MessageBox(Main_hWnd, "RegOpenKeyEx 오픈 실패", "알림", MB_OK );
	}
	else//성공
	{
	//		MessageBox(Main_hWnd, "RegOpenKeyEx 오픈 성공", "알림", MB_OK );
	char buffer[256] = {NULL};
	DWORD data_size = sizeof(buffer); // 사용할 메모리공간의 크기를 저장한다.
	DWORD data_type;
	RegQueryValueEx(MyKey, "Pos", 0, &data_type, NULL, NULL);
	if(data_type == REG_SZ)
	{
	// "Pos" 형식이 문자열이라면 배열을 이용해서 값을 얻는다.
	RegQueryValueEx(MyKey, "Pos", 0, NULL, (LPBYTE)buffer, &data_size);
	}
	}
	RegCloseKey(MyKey);
	*/
}

void CProtection::RegistryDelete()
{
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "", 0, KEY_ALL_ACCESS, &hKey);
	if ( lRet != ERROR_SUCCESS )//실패
	{

		//		MessageBox(Main_hWnd, "RegOpenKeyEx 오픈 실패", "알림", MB_OK );
	}
	else//성공
	{
		//		MessageBox(Main_hWnd, "RegOpenKeyEx 오픈 성공", "알림", MB_OK );
		lRet = RegDeleteValue(HKEY_LOCAL_MACHINE, lpValueName);
		if ( lRet != ERROR_SUCCESS)//실패
		{	
			//			MessageBox(Main_hWnd, "레지스트리 삭제 실패", "알림", MB_OK );
		}
		else//성공
		{
			//			MessageBox(Main_hWnd, "레지스트리 삭제 성공", "알림", MB_OK );
		}
	}	
	RegCloseKey(hKey);  
}

BOOL CProtection::Pesudo_IsDebuggerPresent()
{
	BOOL Retval = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x18]
		mov eax,dword ptr ds:[eax+0x30]
		movzx eax,byte ptr ds:[eax+0x2]
		mov Retval,eax
		pop eax
	}
	return Retval;
}

void CProtection::WindowsSyStemCompulsoryDown()
{
	// Fixed shutdown when not runned as administrator - YeXiuPH
	/*
	DWORD dwVersion = GetVersion();
	if ( dwVersion < 0x80000000)// 윈도우 2000 이상일때 설정해줄 프리빌리지
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES TokenPrivileges;

		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &TokenPrivileges.Privileges[0].Luid);

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	}
	//==========================================================================
	//	ExitWindowsEx API를 사용하면 됩니다. 이 함수의 원형은 다음과 같습니다.
	//	BOOL ExitWindowsEx(UINT uFlags, DWORD dwReserved); 
	//	uFlags로 종료방법을 지정할 수 있습니다. 다음과 같은 값이 가능합니다.
	//	EWX_LOGOFF 현재 사용자를 로그오프한다. 
	//	EWX_POWEROFF 시스템을 종료하고 파워오프한다. 파워오프는 이를 지원하는 하드웨어에서만 가능하다. 
	//	EWX_REBOOT 시스템을 종료하고 시스템을 재시동한다. 
	//	EWX_SHUTDOWN 시스템을 종료한다. 
	//	EWX_FORCE WM_QUERYSESSION이나 WM_ENDQUERYSESSION을 보내지 않고 실행중인 모든 프로세스를 종료한다. 
	//	위의 네 가지 플래그들과 함께 사용할 수 있다. 
	//==========================================================================
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN | EWX_POWEROFF,0); */
}