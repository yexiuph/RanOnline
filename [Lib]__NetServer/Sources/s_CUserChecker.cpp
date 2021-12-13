#include "pch.h"
#include "s_CUserChecker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CUserChecker* CUserChecker::SelfInstance = NULL;

CUserChecker::CUserChecker(void)
{
	InitializeCriticalSection(&m_CriticalSection);
}

CUserChecker::~CUserChecker(void)
{
	DeleteAll();
	DeleteCriticalSection(&m_CriticalSection);
}

	
CUserChecker* CUserChecker::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CUserChecker();
	return SelfInstance;
}

void CUserChecker::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		SAFE_DELETE(SelfInstance);
	}
}

bool CUserChecker::IsAvailable(CString strUserID)
{	
	EnterCriticalSection(&m_CriticalSection);
	strUserID.Trim(_T(""));

	if (Find(strUserID) == true) // 이미 UserID 가 있음
	{
		LeaveCriticalSection(&m_CriticalSection);
		return false;
	}
	else
	{
		// 신규유저를 등록한다.
		Insert(strUserID);
		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}
}

bool CUserChecker::Find(CString strUserID)
{
    CString* pTemp = NULL;
	if (m_UserMap.Lookup(strUserID, (void*&) pTemp) == 0) // Can't found
	{		
		return false;
	}
	else // Found
	{
		return true;
	}
}

void CUserChecker::Insert(CString strUserID)
{	
	strUserID.Trim(_T(" "));

	CString* pTemp = new CString(strUserID);	
	m_UserMap.SetAt(strUserID, (CString*) pTemp);
}

void CUserChecker::DeleteAll()
{
	CString	strKey;
	CString* pTemp = NULL;
	POSITION pos   = NULL;

	EnterCriticalSection(&m_CriticalSection);
	
	// Remove the elements with even key values.
	pos = m_UserMap.GetStartPosition();

	if (pos == NULL) 
	{
		LeaveCriticalSection(&m_CriticalSection);
	}
	else
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_UserMap.GetNextAssoc(pos, strKey, (void*&) pTemp);
			delete pTemp;
			m_UserMap.RemoveKey(strKey);
		}
		LeaveCriticalSection(&m_CriticalSection);
	}
}
 
void CUserChecker::RemoveUser(CString strUserID)
{
	CString* pTemp = NULL;	
	strUserID.Trim(_T(" "));

	EnterCriticalSection(&m_CriticalSection);
	if (m_UserMap.Lookup(strUserID, (void*&) pTemp) == 0)
	{
		LeaveCriticalSection(&m_CriticalSection);
	}
	else // Find
	{
        SAFE_DELETE(pTemp);
		m_UserMap.RemoveKey(strUserID);
		LeaveCriticalSection(&m_CriticalSection);
	}
}