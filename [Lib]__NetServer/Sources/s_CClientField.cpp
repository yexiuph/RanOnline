///////////////////////////////////////////////////////////////////////////////
// s_CClientField.cpp
//
// class CClientField
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CClientField.h"
#include "GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientField::CClientField(
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
	memset(m_szAgentIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
}

CClientField::~CClientField()
{
}

// 클라이언트가 처음 접속했을때 호출됨
// 클라이언트의 ip, port, 접속시간을 기록함.
int CClientField::SetAcceptedClient(DWORD dwClient, SOCKET sSocket)
{
	CNetUser* pData = NULL;
	
	pData = (CNetUser*) (m_pClient+dwClient);
	
	if (pData == NULL) return NET_ERROR;

	if (pData->SetAcceptedClient(sSocket) == NET_OK)
	{
		// 접속한 ip 와 Agent 서버 ip 와 동일한지 검사한다.	
		if (::strcmp(pData->GetIP(), m_szAgentIP) == 0)
		{
			pData->SetSlotType( NET_SLOT_AGENT );
		}
		else
		{
			pData->SetSlotType( NET_SLOT_CLIENT );
		}
		return NET_OK;
	}	
	else
	{
		return NET_ERROR;
	}
}

/*
bool CClientField::CloseClient(
	DWORD dwClient )
{
	bool bReturn = false;
	static DWORD dwPreID(0);

 	if ( dwClient < NET_RESERVED_SLOT )
	{
		// 서버간의 연결이 끊긴것이다.
		if ( true == m_pClient[dwClient].CloseClient() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	DWORD dwClosetSlot = dwClient;

	if (dwClosetSlot != 0 && m_pClient[dwClosetSlot].IsOnline())
	{
		if ( true == m_pClient[dwClosetSlot].CloseClient() )
		{
			if (dwClosetSlot >= NET_RESERVED_SLOT)
			{
				LockOn();
				m_vecSleepCID.push_back(dwClosetSlot);
				//m_pClient[dwAgentSlot].SetSlotFieldClient(0);	
				LockOff();
				bReturn = true;

				if( dwPreID == dwClosetSlot )
				{
					CConsoleMessage::GetInstance()->Write("CClientField::CloseClient dwClientSlot[%u]", dwClosetSlot );
				}
				else
				{
					dwPreID = dwClosetSlot;
				}
			}
		}
	}

	return bReturn;
}
*/

void CClientField::CloseClient( DWORD dwClient )
{
	DWORD dwClientSlot = 0;
	DWORD dwAgentSlot  = 0;

	LockOn();

	if (m_pClient[dwClient].IsOnline())
	{
		if ( dwClient == NET_RESERVED_SESSION )
		{
			// Session 서버와 연결이 끊긴것이다.
			m_pClient[NET_RESERVED_SESSION].CloseClient();			
			LockOff();
			return;
		}

		if (m_pClient[dwClient].IsClientSlot())
		{
			dwAgentSlot  = m_pClient[dwClient].GetSlotFieldAgent();
			dwClientSlot = dwClient;
		}
		else
		{
			dwAgentSlot  = dwClient;
			dwClientSlot = m_pClient[dwClient].GetSlotFieldClient();
		}

		if (dwClient >= NET_RESERVED_SLOT)
		{
			if (dwAgentSlot != dwClientSlot)
			{
				if (dwAgentSlot != 0)
				{
					if( m_pClient[dwAgentSlot].CloseClient() == NET_OK )
					{
						if (dwAgentSlot >= NET_RESERVED_SLOT) m_vecSleepCID.push_back(dwAgentSlot);						
					}else{
						CConsoleMessage::GetInstance()->Write( _T( "INFO:Already CloseClient Agent %d"), dwAgentSlot );
					}
				}

				if (dwClientSlot != 0)
				{
					if( m_pClient[dwClientSlot].CloseClient() == NET_OK )
					{
						if (dwClientSlot >= NET_RESERVED_SLOT) m_vecSleepCID.push_back(dwClientSlot);
					}else{
						CConsoleMessage::GetInstance()->Write( _T( "INFO:Already CloseClient Client %d"), dwClientSlot );
					}
				}
			}
			else
			{
				if (dwClient != 0)
				{
					CConsoleMessage::GetInstance()->Write(_T("CClientField::CloseClient dwClient != 0"));
					if( m_pClient[dwClient].CloseClient() == NET_OK )
					{
						if (dwClient >= NET_RESERVED_SLOT) m_vecSleepCID.push_back(dwClient);
					}else{
						CConsoleMessage::GetInstance()->Write( _T( "INFO:Already CloseClient dwClient %d"), dwClient );
					}
				}
			}
		}
	}

	LockOff();
}

void CClientField::SetAgentIP(const char* szIP)
{
	if (szIP == NULL) return;
	::StringCchCopy(m_szAgentIP, MAX_IP_LENGTH+1, szIP);
}

int	CClientField::SendAgent(DWORD dwClient, LPVOID pBuffer)
{
	assert(pBuffer);

	if (pBuffer == NULL) return NET_ERROR;
	
	LPPER_IO_OPERATION_DATA pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;
	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pBuffer;
	if (nmg == NULL)
	{
		CConsoleMessage::GetInstance()->Write("ERROR:SendAgent nmg == NULL");
		return NET_ERROR;
	}
	dwSndBytes = nmg->dwSize;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		CConsoleMessage::GetInstance()->Write("ERROR:SendAgent %d dwSndBytes > NET_DATA_BUFSIZE", dwClient);
		return NET_ERROR;
	}
	else
	{
		pIoWrite = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_SEND_POSTED);
		if (pIoWrite == NULL)
		{
			return NET_ERROR;
		}
		else
		{
			::CopyMemory(pIoWrite->Buffer, nmg, dwSndBytes);
			pIoWrite->dwTotalBytes	= dwSndBytes;        
			m_dwOutPacketCount++;
			m_dwOutPacketSize += dwSndBytes;
			return SendAgent(dwClient, pIoWrite, dwSndBytes);
		}
	}
}

int	CClientField::SendAgent(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
	DWORD dwRealClient;
	if (m_pClient[dwClient].IsAgentSlot())
	{
		dwRealClient = dwClient;		
	}
	else
	{
		dwRealClient = m_pClient[dwClient].GetSlotFieldAgent();
	}
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = (u_long) dwSize;
	
	SOCKET sSocket = GetSocket(dwRealClient);
	if (sSocket == INVALID_SOCKET)
	{
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(sSocket,
					&(PerIoData->DataBuf),
					1,
					&dwSndSize,
					dwFlags,
					&(PerIoData->Overlapped),
					NULL);

	if (nRetCode == SOCKET_ERROR) 
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSA_IO_PENDING) // WSA_IO_PENDING is not error.
		{
			::PostQueuedCompletionStatus(m_hIOServer,
								0,
								dwRealClient, 
								&PerIoData->Overlapped);				
			CConsoleMessage::GetInstance()->Write("ERROR:SendAgent Client(%d)ERR(%d)", dwClient, nRetCode);
			return NET_ERROR;		
		}
	}
	return NET_OK;
}
	
int	CClientField::SendClient(DWORD dwClient, LPVOID pBuffer)
{
	assert(pBuffer);

	if (pBuffer == NULL) return NET_ERROR;
	
	DWORD dwRealClient;
	if (m_pClient[dwClient].IsClientSlot()) {
		dwRealClient = dwClient;
	}
	else {
		dwRealClient = m_pClient[dwClient].GetSlotFieldClient();
	}
	return CClientManager::SendClient(dwRealClient, pBuffer);
}

/*
int	CClientField::SendClient(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = (u_long) dwSize;
	
	SOCKET sSocket = GetSocket(dwClient);
	if (sSocket == INVALID_SOCKET)
	{
		// 기존에 보내기 받기 요청이 되어 있는 상황에서 클라이언트가 접속이 종료되어 버리면
		// INVALID_SOCKET 에러가 발생한다.
		// 에러라고 보기 에매한 상황이다.
		// CConsoleMessage::GetInstance()->Write("ERROR:CClientField::SendClient INVALID_SOCKET");
		m_pIOCP->Release(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(sSocket,
					&(PerIoData->DataBuf),
					1,
					&dwSndSize,
					dwFlags,
					&(PerIoData->Overlapped),
					NULL);

	if (nRetCode == SOCKET_ERROR) 
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSA_IO_PENDING) // WSA_IO_PENDING is not error.
		{	
			::PostQueuedCompletionStatus(m_hIOServer,
							0,
							dwClient, 
							&PerIoData->Overlapped);
			CConsoleMessage::GetInstance()->Write(_T("ERROR:SendClient Client(%d)ERR(%d)"),
				                                  dwClient,
												  nRetCode);
			return NET_ERROR;
		}
	}
	return NET_OK;
}
*/