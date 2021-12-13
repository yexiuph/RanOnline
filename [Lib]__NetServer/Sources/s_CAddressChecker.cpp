#include "pch.h"
#include "s_CAddressChecker.h"
#include "s_CCsvFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace SERVER_UTIL;

CAddressChecker* CAddressChecker::SelfInstance = NULL;

CAddressChecker::CAddressChecker(int nBlockCount)
{
	InitializeCriticalSection(&m_CriticalSection); 

	m_nBlockCount = nBlockCount;
}

CAddressChecker::~CAddressChecker(void)
{
	deleteAll();
	deleteKnownAddressAll();
	deleteBlockAddressAll();

	DeleteCriticalSection(&m_CriticalSection);
}

CAddressChecker* CAddressChecker::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CAddressChecker(10);

	return SelfInstance;
}

void CAddressChecker::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		SAFE_DELETE(SelfInstance);
	}
}

// 몇번의 잘못된 접속시도시 접속을 거부할 것인지 정한다.
void CAddressChecker::setBlockCount(int nBlockCount)
{
    if (nBlockCount < 1) return;

	m_nBlockCount = nBlockCount;
}

bool CAddressChecker::isBlock(const _TCHAR* szAddress, DWORD dwCurrentTime)
{
	if (szAddress == NULL) return false;

	int nRefCount = 0;
	CString strTemp(szAddress);
	strTemp.Trim(_T(""));

	if (isBlockAddress(strTemp, dwCurrentTime) == true) 
	{
		return true;
	}

	if (isKnownAddress(strTemp) == true)
	{
		return false;
	}

	return false;
	// IP 주소 관리기능 삭제
	/*
	else
	{
		nRefCount = insertAddress(strTemp.GetString());
		if (nRefCount > m_nBlockCount)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	*/
}

// 등록된 Block Address 를 모두 삭제한다.
int CAddressChecker::resetBlockAddress()
{
	return deleteBlockAddressAll();
}

int CAddressChecker::deleteBlockAddressAll()
{
    CString			strKey;
	LPADDRESS_INFO	pData = NULL;
	POSITION		pos   = NULL;
	
	EnterCriticalSection(&m_CriticalSection);

	// Remove the elements with even key values.
	pos = m_BlockAddress.GetStartPosition();

	if (pos == NULL) 
	{		
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_BlockAddress.GetNextAssoc(pos, strKey, (void*&) pData);
			delete pData;
			m_BlockAddress.RemoveKey(strKey);
		}
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
}

int CAddressChecker::deleteKnownAddressAll()
{
	CString			strKey;
	LPADDRESS_INFO	pData = NULL;
	POSITION		pos   = NULL;

	EnterCriticalSection(&m_CriticalSection);
	// Remove the elements with even key values.
	pos = m_KnownAddress.GetStartPosition();

	if (pos == NULL) 
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_KnownAddress.GetNextAssoc(pos, strKey, (void*&) pData);
			delete pData;
			m_KnownAddress.RemoveKey(strKey);
		}
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
}

bool CAddressChecker::isKnownAddress(CString strAddress)
{
	LPADDRESS_INFO pData = NULL;

	EnterCriticalSection(&m_CriticalSection);
	if (m_KnownAddress.Lookup(strAddress, (void*&) pData) == 0) // Can't found
	{
		LeaveCriticalSection(&m_CriticalSection);
		return false;
	}
	else // Found address
	{
		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}
}

bool CAddressChecker::isBlockAddress(CString strAddress, DWORD dwCurrentTime)
{
	LPADDRESS_INFO pData = NULL;
	bool bBlock = true;

	EnterCriticalSection(&m_CriticalSection);
	if (m_BlockAddress.Lookup(strAddress, (void*&) pData) == 0) // Can't found
	{
		bBlock = false;		
	}
	else // Found address
	{
		// 시간체크를 한다는 뜻이다
		if (dwCurrentTime != 0) 
		{
			// Block 종료 시간이 현재시간보다 같거나 작으면
			if (pData->dwTime <= dwCurrentTime)
			{
				// Block 이 풀린것이므로 true 를 리턴한다.
				bBlock = false;
			}
			else
			{
				// 아직 Block 이 풀릴 시간이 아니다.
				bBlock = true;
			}
		}
		else // 시간체크를 하지 않을때...
		{
			bBlock = true;
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
	return bBlock;
}

//! Block 되는 주소를 등록한다.\n
//! Block Address 는 KnownAddress 보다 우선한다.
//! \param szAddress Block 될 IP 주소
//! \param dwBlockTime Block 이 풀리는 시간
//! \return
int CAddressChecker::addBlockAddress(const _TCHAR* szAddress, DWORD dwBlockTime)
{
	if (szAddress == NULL) return 0;

	CString strTemp(szAddress);
	strTemp.Trim(_T(""));

	// 이미 알고 있는 주소라면 block 주소에 등록하지 않는다.
	if (isKnownAddress(strTemp) == true)
	{
		return 0;
	}

	LPADDRESS_INFO pData = NULL;

	EnterCriticalSection(&m_CriticalSection);

	if (m_BlockAddress.Lookup(strTemp, (void*&) pData) == 0) // Can't found
	{
		LPADDRESS_INFO pDataTemp = new ADDRESS_INFO;
		pDataTemp->nRefCount = 1;
		pDataTemp->dwTime = dwBlockTime;
		StringCchCopy(pDataTemp->szAddress, MAX_IP_LENGTH+1, strTemp.GetString());
		m_BlockAddress.SetAt(strTemp, (LPADDRESS_INFO) pDataTemp);
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else // Found address
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}
}

//! Block 되지 않는 주소를 등록한다 이 IP 주소는 신뢰하는 주소이다.
//! \param szAddress 등록할 IP 주소
//! \return
int CAddressChecker::addKnownAddress(const _TCHAR* szAddress)
{
	if (szAddress == NULL) return 0;

	CString strTemp(szAddress);
	strTemp.Trim(_T(""));

	LPADDRESS_INFO pData = NULL;

	EnterCriticalSection(&m_CriticalSection);
	if (m_KnownAddress.Lookup(strTemp, (void*&) pData) == 0) // Can't found
	{	
		LPADDRESS_INFO pDataTemp = new ADDRESS_INFO;
		pDataTemp->nRefCount = 1;
		StringCchCopy(pDataTemp->szAddress, MAX_IP_LENGTH+1, strTemp.GetString());
		m_KnownAddress.SetAt(strTemp, (LPADDRESS_INFO) pDataTemp);
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else // Found address
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}
}

//! 알고있는 주소나 Block 주소와 관계없이 IP 를 등록한다.
//! \param szAddress 등록할 주소
int CAddressChecker::insertAddress(const _TCHAR* szAddress)
{
	if (szAddress == NULL) return 0;

	CString strTemp(szAddress);
	strTemp.Trim(_T(" "));

	EnterCriticalSection(&m_CriticalSection);

	LPADDRESS_INFO pData = findAddress(strTemp);
	if (pData == NULL) // Can't find address
	{
		LPADDRESS_INFO pDataTemp = new ADDRESS_INFO;
		pDataTemp->nRefCount = 1;
		pDataTemp->dwTime = 0;
		StringCchCopy(pDataTemp->szAddress, MAX_IP_LENGTH+1, strTemp.GetString());
		m_AddressMap.SetAt(strTemp, (LPADDRESS_INFO) pDataTemp);
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}
	else // Find address
	{
		pData->nRefCount++;
		LeaveCriticalSection(&m_CriticalSection);
		return pData->nRefCount;
	}
}

int CAddressChecker::deleteAddress(const _TCHAR* szAddress)
{
	if (szAddress == NULL) return 0;

	LPADDRESS_INFO pData = NULL;
	CString strTemp(szAddress);
	strTemp.Trim(_T(" "));

	EnterCriticalSection(&m_CriticalSection);
	if (m_AddressMap.Lookup(strTemp, (void*&) pData) == 0) // Can't find
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else // Find address
	{
		pData->nRefCount--;
		if (pData->nRefCount <= 0)
		{
			pData->nRefCount = 0;
			LeaveCriticalSection(&m_CriticalSection);
			return 0;
		}
		else
		{
			LeaveCriticalSection(&m_CriticalSection);
			return pData->nRefCount;
		}
	}
}

int CAddressChecker::deleteAll()
{
	CString			strKey;
	LPADDRESS_INFO	pData = NULL;
	POSITION		pos   = NULL;

	EnterCriticalSection(&m_CriticalSection);
	// Remove the elements with even key values.
	pos = m_AddressMap.GetStartPosition();

	if (pos == NULL) 
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
	else
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_AddressMap.GetNextAssoc(pos, strKey, (void*&) pData);
			delete pData;
			m_AddressMap.RemoveKey(strKey);
		}
		LeaveCriticalSection(&m_CriticalSection);
		return 0;
	}
}

bool CAddressChecker::deleteBlockAddress(const _TCHAR* szAddress)
{
	CString strKey;
	LPADDRESS_INFO pData = NULL;
	POSITION pos = NULL;

	EnterCriticalSection(&m_CriticalSection);
	if (m_BlockAddress.Lookup(szAddress, (void*&) pData) != 0)
	{
		// 주소를 찾았음
		SAFE_DELETE(pData);
		m_BlockAddress.RemoveKey(szAddress);
		LeaveCriticalSection(&m_CriticalSection);
		return true;
	}
	else
	{
		LeaveCriticalSection(&m_CriticalSection);
		return false;
	}
}

int CAddressChecker::getRefCount(const _TCHAR* szAddress)
{
	if (szAddress == NULL) return 0;

	CString strTemp(szAddress);
	strTemp.Trim(_T(" "));
	
	LPADDRESS_INFO pData = findAddress(strTemp);
	if (pData == NULL) // Can't find address
	{
		return 0;
	}
	else // Find address
	{
		return pData->nRefCount; 
	}
}

int CAddressChecker::setRefCount(const _TCHAR* szAddress, int nCount)
{
	if (szAddress == NULL || nCount < 0) return 0;

	CString strTemp(szAddress);
	strTemp.Trim(_T(" "));
	
	LPADDRESS_INFO pData = findAddress(strTemp);
	if (pData == NULL) // Can't find address
	{
		return 0;
	}
	else // Find address
	{
		pData->nRefCount = nCount;
		return pData->nRefCount;
	}
}

LPADDRESS_INFO CAddressChecker::findAddress(CString strAddress)
{
	LPADDRESS_INFO pData = NULL;
	if (m_AddressMap.Lookup(strAddress, (void*&) pData) == 0) // Can't found
	{		
		return NULL;
	}
	else // Found address
	{
		return pData;
	}
}

int CAddressChecker::save()
{
	CString			strKey;
	LPADDRESS_INFO	pData = NULL;
	POSITION		pos   = NULL;

	CCsvFile SaveFile(_T("ConnectList"));
	
	SaveFile.Write(_T("Known Address"));
	
	EnterCriticalSection(&m_CriticalSection);

	pos = m_KnownAddress.GetStartPosition();

	if (pos != NULL)
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_KnownAddress.GetNextAssoc(pos, strKey, (void*&) pData);
			SaveFile.Write(_T("%s, %d"), pData->szAddress, pData->nRefCount);
		}
	}

	SaveFile.Write(_T("Block Address"));
	
	pos = m_BlockAddress.GetStartPosition();

	if (pos != NULL)
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_BlockAddress.GetNextAssoc(pos, strKey, (void*&) pData);
			SaveFile.Write(_T("%s, %d"), pData->szAddress, pData->nRefCount);
		}
	}

	SaveFile.Write(_T("Connected Address"));

	pos = m_AddressMap.GetStartPosition();

	if (pos != NULL)
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_AddressMap.GetNextAssoc(pos, strKey, (void*&) pData);
			SaveFile.Write(_T("%s, %d"), pData->szAddress, pData->nRefCount);
		}
	}

	LeaveCriticalSection(&m_CriticalSection);
	return 0;
}