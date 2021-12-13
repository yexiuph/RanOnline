///////////////////////////////////////////////////////////////////////////////
// s_CClientSession.cpp
// 
// class CClientSession
//
// * History
// 2002.05.30 jgkim Create
// 2003.01.23 jgkim Add member function IsGameServer() 
// 2003.02.12 jgkim ����ó�� ��ȭ
// 2005.01.31 jgkim m_pUsrMap ó�� ���� ����
//
// Copyright(c) Mincoms. All rights reserved.                 
// 
// * Note
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "s_CClientSession.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientSession::CClientSession(
	int nMaxClient,
	COverlapped* pSendIOCP,
	COverlapped* pRecvIOCP,
	HANDLE	hIOServer )
	: CClientManager(
		nMaxClient,
		pSendIOCP, 
		pRecvIOCP, 
		hIOServer )
{
	m_pServerInfo = NULL;
	m_pServerInfo = (LPG_SERVER_INFO) HeapAlloc(GetProcessHeap(), 
										        HEAP_ZERO_MEMORY, 
										        sizeof(G_SERVER_INFO) * nMaxClient);
	m_pUsrMap = new CMapStringToPtr (USER_MAP_START_SIZE);
}

CClientSession::~CClientSession()
{
	if (m_pServerInfo) 
	{		
		UserEraseAll();
		SAFE_DELETE(m_pUsrMap);
		HeapFree(GetProcessHeap(), 0, m_pServerInfo);
		m_pServerInfo = NULL;
	}
}

int CClientSession::CloseClient(
	DWORD dwClientNum )
{	
	// Clear client information	
	//if ( true == CClientManager::CloseClient(dwClientNum) )
	//{
	//	::SecureZeroMemory((void *) &m_pServerInfo[dwClientNum], sizeof(G_SERVER_INFO));
	//	return true;
	//}
	//return false;

	::SecureZeroMemory((void *) &m_pServerInfo[dwClientNum], sizeof(G_SERVER_INFO));
	return CClientManager::CloseClient(dwClientNum);
}

// ���Ӽ��� -> ���Ǽ��� : ���Ӽ����� ���� ����
void CClientSession::SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi)
{
	::CopyMemory(&m_pServerInfo[dwClient], (LPG_SERVER_INFO) gsi, sizeof(G_SERVER_INFO));
}

// ���Ӽ��� -> ���Ǽ��� : ���Ӽ����� ������� ���� (�������ڼ�/�ִ����ڼ�)
void CClientSession::SetSvrCurInfo(DWORD dwClient, G_SERVER_CUR_INFO *nsi)
{
	m_pServerInfo[dwClient].nServerCurrentClient = nsi->nServerCurrentClient;
	m_pServerInfo[dwClient].nServerMaxClient     = nsi->nServerMaxClient;
}

DWORD CClientSession::FindServer(int nType, int nSvrGrp, int nSvrNum, int nField)
{
	DWORD dwClient = 0;
	for (dwClient=0; dwClient < (DWORD) m_nMaxClient; dwClient++)
	{
		if (m_pClient[dwClient].IsOnline()) // �¶����̶��
		{
			if( nType == NET_SERVER_AGENT )
			{
				if (m_pServerInfo[dwClient].nServerType == (DWORD) nType &&
					m_pServerInfo[dwClient].nServerGroup == (DWORD) nSvrGrp )
					return dwClient;
			}else{
				if (m_pServerInfo[dwClient].nServerType == (DWORD) nType &&
					m_pServerInfo[dwClient].nServerGroup == (DWORD) nSvrGrp &&
					m_pServerInfo[dwClient].nServerNumber == (DWORD) nSvrNum &&
					m_pServerInfo[dwClient].nServerField == (DWORD) nField)
					return dwClient;
			}
		}
	}
	return NET_ERROR; // ã�����ϸ� ��������
}

// �ּҿ� ��Ʈ�� �̿��ؼ� ���� ��ȣ�� ã�´�.
DWORD CClientSession::FindServer(const char* szServerIP, int nServicePort)
{
	int nClient;
	
	CConsoleMessage::GetInstance()->Write("���� ã�� ����");
	CConsoleMessage::GetInstance()->Write(szServerIP);

	for (nClient=0; nClient < m_nMaxClient; nClient++)
	{
		if (m_pClient[nClient].IsOnline()) // �¶����̶��
		{
			if (strcmp(GetSvrIP((DWORD) nClient), szServerIP) == 0) // �ּ�����
			{			
				if (GetSvrServicePort((DWORD) nClient) == nServicePort) // ��Ʈ����
				{
					// �ּ� ��Ʈ�� �����ϸ� ���� ������ �����Ѵ�
					return (DWORD) nClient; // ã�� ������ȣ ����
				}
			}
		}
	}
	return NET_ERROR; // ã�����ϸ� ��������
}

int CClientSession::GetSvrGrp(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerGroup; 
}

int CClientSession::GetSvrNum(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerNumber;
}

int CClientSession::GetSvrField(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerField;
}

int CClientSession::GetSvrType(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerType;
}

int	CClientSession::GetSvrServicePort(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServicePort;
}

int CClientSession::GetSvrCrtClient(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerCurrentClient;
}

int CClientSession::GetSvrMaxClient(DWORD dwClient)
{
	return m_pServerInfo[dwClient].nServerMaxClient;
}

char* CClientSession::GetSvrIP(DWORD dwClient)
{
	return m_pServerInfo[dwClient].szServerIP;
}

G_SERVER_INFO CClientSession::GetSvrInfo(DWORD dwClient)
{
	return m_pServerInfo[dwClient];
}

void CClientSession::UpdateChaJoinOK (CString strUsrID, 							
							int nUsrNum, 
							int nChaNum, 
							DWORD dwGaeaID,
							const char* szChaName)
{
	if (szChaName == NULL) return;

	LPCNT_USR_INFO pExistData = NULL;	

	// ������ ���ӵǾ� �ִ��� �˻��Ѵ�.
	pExistData = UserFind(strUsrID);	
	if (pExistData == NULL) // ����ڰ� ����
		return;

	// ����ڰ� ������ ���� ������Ʈ
	pExistData->nUsrNum = nUsrNum;
	pExistData->nChaNum = nChaNum;
	pExistData->dwGaeaID = dwGaeaID;	
	::StringCchCopy(pExistData->szChaName, CHR_ID_LENGTH+1, szChaName); // �÷������� ĳ���͸�
}

///////////////////////////////////////////////////////////////////////////////
// User management function
///////////////////////////////////////////////////////////////////////////////

LPCNT_USR_INFO CClientSession::UserFind(CString strUsrID)
{
	BOOL bRetCode = 0;
	LPCNT_USR_INFO pData = NULL;
	LockOn();
	bRetCode = m_pUsrMap->Lookup(strUsrID, (void*&) pData);
	if (bRetCode == 0)
	{
		LockOff();
		return NULL;
	}
	else
	{
		LockOff();
		return pData;
	}
}

// ���� ������ ������ ����Ѵ�.
void CClientSession::UserInsert(CString strUsrID, LPCNT_USR_INFO pData)
{	
	if (pData == NULL) return;

	LPCNT_USR_INFO pExistData = NULL;	
	
	// ������ ���ӵǾ� �ִ��� �˻��Ѵ�.
	pExistData = UserFind(strUsrID);	
	if (pExistData != NULL) // �̹� ���ӵǾ� ������ ���������ڸ� �����Ѵ�.
		UserErase(strUsrID);

	LockOn();
	LPCNT_USR_INFO pDataTemp = new CNT_USR_INFO;

	pDataTemp->dwGameClient	= pData->dwGameClient;
	pDataTemp->dwSvrClient  = pData->dwSvrClient;
	pDataTemp->nSvrGrp      = pData->nSvrGrp;
	pDataTemp->nSvrNum      = pData->nSvrNum;
	pDataTemp->nUsrNum      = pData->nUsrNum;
	::StringCchCopy(pDataTemp->szUserID, USR_ID_LENGTH+1, pData->szUserID); // ����� ID
	m_pUsrMap->SetAt(strUsrID, (LPCNT_USR_INFO) pDataTemp);
	LockOff();
}

BOOL CClientSession::UserErase(CString strUsrID)
{	
	BOOL bFind;
	LPCNT_USR_INFO pData;
	LockOn();
	bFind = m_pUsrMap->Lookup(strUsrID, (void*&) pData);
	if (bFind == 0)	
	{
		LockOff();
		return FALSE; // Can't find, return.
	}
	else
	{
		// Find and delete, erase.
		delete pData;
		m_pUsrMap->RemoveKey(strUsrID);
		LockOff();
		return TRUE;
	}
}

void CClientSession::UserEraseServer(int nSvrGrp, int nSvrNum)
{
	CString			strKey;
	LPCNT_USR_INFO	pData;
	POSITION		pos;
	LockOn();
	// Remove the elements with even key values.
	pos = m_pUsrMap->GetStartPosition();

	if (pos == NULL) 
	{
		LockOff();
		return;
	}
		
	// Loop all elements and find nSvrGrp, nSvrNum, delete element	
	while (pos != NULL)
	{
		m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);

		if (pData->nSvrGrp == nSvrGrp && pData->nSvrNum == nSvrNum)
		{
			delete pData;
			m_pUsrMap->RemoveKey(strKey);
		}
	}
	LockOff();
}

void CClientSession::UserEraseAll()
{
	CString			strKey;
	LPCNT_USR_INFO	pData;
	POSITION		pos;

	LockOn();
	// Remove the elements with even key values.
	pos = m_pUsrMap->GetStartPosition();

	if (pos == NULL) 
	{
		LockOff();
		return;
	}
	else
	{
		// Loop all elements and delete, erase.		
		while (pos != NULL)
		{
			m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);
			delete pData;
			m_pUsrMap->RemoveKey(strKey);			
		}
		LockOff();
		return;
	}
}

void CClientSession::PrintUsrList()
{
	CString			strKey;
	LPCNT_USR_INFO	pData;	
	POSITION		pos = m_pUsrMap->GetStartPosition();
	
	if (pos == NULL) return;
	
	// Loop all elements and delete, erase.
	while (pos != NULL)
	{		
		m_pUsrMap->GetNextAssoc(pos, strKey, (void*&) pData);
		CConsoleMessage::GetInstance()->Write(
			                                  "(%s:%d)(%d:%d)(%d:%d)",
							                  strKey, 
							                  pData->nUsrNum,
							                  pData->nSvrGrp,
							                  pData->nSvrNum,							
							                  pData->dwSvrClient,
							                  pData->dwGameClient);
	}
}

bool CClientSession::IsGameServer(DWORD dwClient)
{
	// server type [type]
	// 1 : Login server
    // 2 : Session server
    // 3 : Field server
    // 4 : Agent server	
	if (m_pServerInfo[dwClient].nServerType == SERVER_AGENT)
		return true;
	else
		return false;
}