#include "pch.h"
#include "regfunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//GetRegValue ( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer", "Version", strVer );

BOOL GetRegValue ( HKEY hRoot, LPCSTR sPost, LPCSTR sName, CString& sValue )
{
	HKEY hKey;
	DWORD dwType;
	DWORD dwSize = 250;
	char  sbuf[250];

	if (RegOpenKeyEx(hRoot, (LPSTR)sPost, 0, KEY_QUERY_VALUE, &hKey)!= ERROR_SUCCESS)	return false;

	if (RegQueryValueEx (hKey, (LPSTR)sName, NULL,  &dwType, (BYTE *)sbuf, &dwSize)!=ERROR_SUCCESS)	return false;
	RegCloseKey(hKey);

	sValue = sbuf;
	return true;
}