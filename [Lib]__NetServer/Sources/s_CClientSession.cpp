///////////////////////////////////////////////////////////////////////////////
// s_CClientSession.cpp
// 
// class CClientSession
//
// * History
// 2002.05.30 jgkim Create
// 2003.01.23 jgkim Add member function IsGameServer() 
// 2003.02.12 jgkim 에러처리 강화
// 2005.01.31 jgkim m_pUsrMap 처리 에러 수정
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

// 게임서버 -> 세션서버 : 게임서버의 상태 전송
void CClientSession::SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi)
{
	::CopyMemory(&m_pServerInfo[dwClient], (LPG_SERVER_INFO) gsi, sizeof(G_SERVER_INFO));
}

// 게임서버 -> 세션서버 : 게임서버의 현재상태 전송 (현재사용자수/최대사용자수)
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
		if (m_pClient[dwClient].IsOnline()) // 온라인이라면
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
	return NET_ERROR; // 찾지못하면 에러리턴
}

// 주소와 포트를 이용해서 서버 번호를 찾는다.
DWORD CClientSession::FindServer(const char* szServerIP, int nServicePort)
{
	int nClient;
	
	CConsoleMessage::GetInstance()->Write("서버 찾기 시작");
	CConsoleMessage::GetInstance()->Write(szServerIP);

	for (nClient=0; nClient < m_nMaxClient; nClient++)
	{
		if (m_pClient[nClient].IsOnline()) // 온라인이라면
		{
			if (strcmp(GetSvrIP((DWORD) nClient), szServerIP) == 0) // 주소조사
			{			
				if (GetSvrServicePort((DWORD) nClient) == nServicePort) // 포트조사
				{
					// 주소 포트가 동일하면 동일 서버로 간주한다
					return (DWORD) nClient; // 찾은 서버번호 리턴
				}
			}
		}
	}
	return NET_ERROR; // 찾지못하면 에러리턴
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

	// 기존에 접속되어 있는지 검사한다.
	pExistData = UserFind(strUsrID);	
	if (pExistData == NULL) // 사용자가 없음
		return;

	// 사용자가 있으면 정보 업데이트
	pExistData->nUsrNum = nUsrNum;
	pExistData->nChaNum = nChaNum;
	pExistData->dwGaeaID = dwGaeaID;	
	::StringCchCopy(pExistData->szChaName, CHR_ID_LENGTH+1, szChaName); // 플레이중인 캐릭터명
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

// 새로 접속한 유저를 등록한다.
void CClientSession::UserInsert(CString strUsrID, LPCNT_USR_INFO pData)
{	
	if (pData == NULL) return;

	LPCNT_USR_INFO pExistData = NULL;	
	
	// 기존에 접속되어 있는지 검사한다.
	pExistData = UserFind(strUsrID);	
	if (pExistData != NULL) // 이미 접속되어 있으면 기존접속자를 제거한다.
		UserErase(strUsrID);

	LockOn();
	LPCNT_USR_INFO pDataTemp = new CNT_USR_INFO;

	pDataTemp->dwGameClient	= pData->dwGameClient;
	pDataTemp->dwSvrClient  = pData->dwSvrClient;
	pDataTemp->nSvrGrp      = pData->nSvrGrp;
	pDataTemp->nSvrNum      = pData->nSvrNum;
	pDataTemp->nUsrNum      = pData->nUsrNum;
	::StringCchCopy(pDataTemp->szUserID, USR_ID_LENGTH+1, pData->szUserID); // 사용자 ID
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