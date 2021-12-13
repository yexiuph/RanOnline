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
	RegistryDelete();	//������Ʈ�� ����
}


void CProtection::Init()
{
	RegistryRecord();	//������Ʈ�� �Է�
}


CProtection& CProtection::GetInstance()
{
	static CProtection preservationObj;
	return preservationObj;
}

void CProtection::RegistryRecord()
{
	lRet = RegCreateKey(HKEY_LOCAL_MACHINE, "", &hKey);
	if( lRet != ERROR_SUCCESS )//����
	{
		//		MessageBox(Main_hWnd, "���� ����", "�˸�", MB_OK );
		CProtection::WindowsSyStemCompulsoryDown();//������Ʈ���� ���� ���н� �ü�� ����
	}
	else//����  
	{
		//		MessageBox(Main_hWnd, "���� ����", "�˸�", MB_OK );
		lRet = RegSetValueEx(hKey, lpValueName, 0,
			REG_DWORD, (LPBYTE) &dwData, sizeof(DWORD));
		if( lRet != ERROR_SUCCESS )//����
		{
			//			MessageBox(Main_hWnd, "������Ʈ�� ���� ����", "�˸�", MB_OK );
			CProtection::WindowsSyStemCompulsoryDown();//������Ʈ���� ���� ���н� �ü�� ����
		}
		else//����
		{
			//			MessageBox(Main_hWnd, "������Ʈ�� ���� ����", "�˸�", MB_OK );
		}
	}
	RegCloseKey(hKey);
	/*
	HKEY MyKey = 0;
	lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Ryu&Soft", 0, KEY_ALL_ACCESS, &MyKey );
	if ( lRet != ERROR_SUCCESS )//����
	{
	//		MessageBox(Main_hWnd, "RegOpenKeyEx ���� ����", "�˸�", MB_OK );
	}
	else//����
	{
	//		MessageBox(Main_hWnd, "RegOpenKeyEx ���� ����", "�˸�", MB_OK );
	char buffer[256] = {NULL};
	DWORD data_size = sizeof(buffer); // ����� �޸𸮰����� ũ�⸦ �����Ѵ�.
	DWORD data_type;
	RegQueryValueEx(MyKey, "Pos", 0, &data_type, NULL, NULL);
	if(data_type == REG_SZ)
	{
	// "Pos" ������ ���ڿ��̶�� �迭�� �̿��ؼ� ���� ��´�.
	RegQueryValueEx(MyKey, "Pos", 0, NULL, (LPBYTE)buffer, &data_size);
	}
	}
	RegCloseKey(MyKey);
	*/
}

void CProtection::RegistryDelete()
{
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "", 0, KEY_ALL_ACCESS, &hKey);
	if ( lRet != ERROR_SUCCESS )//����
	{

		//		MessageBox(Main_hWnd, "RegOpenKeyEx ���� ����", "�˸�", MB_OK );
	}
	else//����
	{
		//		MessageBox(Main_hWnd, "RegOpenKeyEx ���� ����", "�˸�", MB_OK );
		lRet = RegDeleteValue(HKEY_LOCAL_MACHINE, lpValueName);
		if ( lRet != ERROR_SUCCESS)//����
		{	
			//			MessageBox(Main_hWnd, "������Ʈ�� ���� ����", "�˸�", MB_OK );
		}
		else//����
		{
			//			MessageBox(Main_hWnd, "������Ʈ�� ���� ����", "�˸�", MB_OK );
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
	if ( dwVersion < 0x80000000)// ������ 2000 �̻��϶� �������� ����������
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
	//	ExitWindowsEx API�� ����ϸ� �˴ϴ�. �� �Լ��� ������ ������ �����ϴ�.
	//	BOOL ExitWindowsEx(UINT uFlags, DWORD dwReserved); 
	//	uFlags�� �������� ������ �� �ֽ��ϴ�. ������ ���� ���� �����մϴ�.
	//	EWX_LOGOFF ���� ����ڸ� �α׿����Ѵ�. 
	//	EWX_POWEROFF �ý����� �����ϰ� �Ŀ������Ѵ�. �Ŀ������� �̸� �����ϴ� �ϵ������� �����ϴ�. 
	//	EWX_REBOOT �ý����� �����ϰ� �ý����� ��õ��Ѵ�. 
	//	EWX_SHUTDOWN �ý����� �����Ѵ�. 
	//	EWX_FORCE WM_QUERYSESSION�̳� WM_ENDQUERYSESSION�� ������ �ʰ� �������� ��� ���μ����� �����Ѵ�. 
	//	���� �� ���� �÷��׵�� �Բ� ����� �� �ִ�. 
	//==========================================================================
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN | EWX_POWEROFF,0); */
}