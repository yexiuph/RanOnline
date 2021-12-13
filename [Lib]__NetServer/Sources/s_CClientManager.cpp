///////////////////////////////////////////////////////////////////////////////
// s_CClientManager.cpp
//
// * History
// 2002.05.30 jgkim Create
// 2003.02.12 jgkim Message buffering
//
// Copyright(c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CClientManager.h"
#include "s_CConsoleMessage.h"
#include "GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientManager::CClientManager(
	int nMaxClient,
	COverlapped* pSendIOCP,
	COverlapped* pRecvIOCP,
	HANDLE hIOServer)
	: m_pClient(NULL)
	, m_nMaxClient(nMaxClient)
	, m_hIOServer(hIOServer)
	//, m_pSendIOCP(NULL)
	//, m_pRecvIOCP(NULL)
	, m_pSendIOCP(pSendIOCP)
	, m_pRecvIOCP(pRecvIOCP)
	, m_dwInPacketCount(0) //< ���ŵ� �� ��Ŷ ����
	, m_dwInPacketSize(0) //< ���ŵ� �� ��Ŷ ������
	, m_dwOutPacketCount(0) //< �۽��� �� ��Ŷ ����
	, m_dwOutPacketSize(0) //< �۽��� �� ��Ŷ ������
	, m_dwOutCompressCount(0) //< �۽��� �� ���� ��Ŷ ����
	, m_dwOutCompressSize(0) //< �۽��� �� ���� ��Ŷ ũ��
{
	//m_pRecvIOCP = new COverlapped(m_nMaxClient);
	//m_pSendIOCP = new COverlapped(m_nMaxClient * 3);

	srand((unsigned) time(NULL));
	m_pClient = new CNetUser[m_nMaxClient];

	for ( int i=NET_RESERVED_SLOT; i<m_nMaxClient; ++i )	
	{
		m_deqSleepCID.push_back(i);
		m_mapSleepCID[i] = TRUE;
	}

	m_vecSleepCID.reserve( m_nMaxClient );

	m_vecConnectIP.clear();
}

CClientManager::~CClientManager()
{
	// close all client socket
	// CloseAllClient();
	// Memory Free Client information
	if ( m_pClient != NULL )
	{	
		LockOn();
		SAFE_DELETE_ARRAY(m_pClient);
		LockOff();
	}


	//SAFE_DELETE( m_pRecvIOCP );
	//SAFE_DELETE( m_pSendIOCP );
}

CRYPT_KEY CClientManager::GetNewCryptKey()
{
	CRYPT_KEY ck;
	// ck.nKeyDirection	= rand() % 2 + 1; // Direction Left or Right	
	// ck.nKey				= rand() % 5 + 2; // Shift amount	

	ck.nKeyDirection	= 1; // Direction Left or Right	
	ck.nKey				= 1; // Shift amount
	return ck;
}

bool CClientManager::IsAccountPass (DWORD dwClient) // ���� ����(login) ����Ͽ����� ����.
{
	return m_pClient[dwClient].IsAccountPass();
}

void CClientManager::SetAccountPass (DWORD dwClient, bool bPASS) // ���� ����(login) ��� ����.
{
	m_pClient[dwClient].SetAccountPass(bPASS);
}


CRYPT_KEY CClientManager::GetCryptKey(DWORD dwClient)
{
	//assert(dwClient<(DWORD)m_nMaxClient);

	return m_pClient[dwClient].GetCryptKey();
}

void CClientManager::SetCryptKey(DWORD dwClient, CRYPT_KEY ck)
{
	//assert(dwClient<(DWORD)m_nMaxClient);

	m_pClient[dwClient].SetCryptKey(ck);
}

void CClientManager::ResetUserID(DWORD dwClient)
{
	m_pClient[dwClient].ResetUserID();
}

bool CClientManager::IsOnline(DWORD dwClient)
{
	// assert(dwClient<(DWORD)m_nMaxClient);
	return m_pClient[dwClient].IsOnline();
}

bool CClientManager::IsOffline(DWORD dwClient)
{
	// assert(dwClient<(DWORD)m_nMaxClient);
	return m_pClient[dwClient].IsOffline();
}

// GetFreeClientID
// Return the free client slot
int	CClientManager::GetFreeClientID(int nType)
{	
	LockOn();
	if (m_deqSleepCID.empty())
	{		
		LockOff();
		// Reached Max Client Error �޼����� ȣ���ϱ��� m_vecSleepCID queue�� ���¸� üũ
		ResetPreSleepCID();
		return NET_ERROR;
	}
	else
	{		
		int nClient = m_deqSleepCID.front();
		m_deqSleepCID.pop_front();		
		// assert ( !m_pClient[nClient].IsOnline() );
		m_mapSleepCID.erase( m_mapSleepCID.find(nClient) );
		m_pClient[nClient].SetOnLine();	

		LockOff();
		return nClient;		
	}
}

//	Note : ���� �������� ������ üũ�Ѵ�.
//
void CClientManager::ConnectIPCheck(char *pszIP)
{
	LockOn();
	UINT i, iSameIPCount = 0;
	for( i = 0; i < m_vecConnectIP.size(); i++ )
	{
		if( pszIP == m_vecConnectIP[i] )
		{
			iSameIPCount++;
		}
	}

	if( iSameIPCount >= 10 )
	{
		CConsoleMessage::GetInstance()->Write( _T( "ERROR:Connect Same IP %s"), pszIP );

		for( i = 0; i < m_vecConnectIP.size(); i++ )
		{
			if( pszIP == m_vecConnectIP[i] )
			{
				m_vecConnectIP.erase( m_vecConnectIP.begin() + i );
				i--;
			}
		}
	}else{
		m_vecConnectIP.push_back(pszIP);
		if( m_vecConnectIP.size() > 30 )
			m_vecConnectIP.erase( m_vecConnectIP.begin() );
	}


	LockOff();

	//return NET_OK;
	

}

//	Note : ���� Ŭ���̾�Ʈ ID�� ��Ƽ� �Ѳ����� ���. ( Ŭ���̾�Ʈ ID�� ���� ����� ����. )
//
void CClientManager::ResetPreSleepCID()
{
	LockOn();
	int nTemp = -1;
	size_t vecSize = m_vecSleepCID.size();
	for ( size_t i = 0; i < vecSize; ++i )
	{
		if (nTemp != m_vecSleepCID[i] )
		{
			if( m_mapSleepCID.count(m_vecSleepCID[i]) == 0 )
			{
				m_deqSleepCID.push_back(m_vecSleepCID[i]);
				m_mapSleepCID.insert( std::make_pair( m_vecSleepCID[i], TRUE ) );

				nTemp = m_vecSleepCID[i];
			}else{
				CConsoleMessage::GetInstance()->Write( _T( "ERROR:Already CloseClient %d"), m_vecSleepCID[i] );
			}
		}else{
			CConsoleMessage::GetInstance()->Write( _T( "INFO:ResetPreSleepCID Error SleepCID %d" ), m_vecSleepCID[i] );
		}
	}
	m_vecSleepCID.clear();	
	LockOff();
}

int CClientManager::GetMaxClient()
{
	return m_nMaxClient;
}

// Ŭ���̾�Ʈ�� ó�� ���������� ȣ���
// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
int CClientManager::SetAcceptedClient(DWORD dwClient, SOCKET sSocket)
{
	int nRetCode = 0;

	CNetUser* pData = (CNetUser*) (m_pClient+dwClient);
	
	if (pData == NULL)	return NET_ERROR;

	if (pData->SetAcceptedClient(sSocket) == NET_OK)
	{
		CConsoleMessage::GetInstance()->WriteConsole(
			_T("(Client ID:%d) (%s:%d)"),
			dwClient,
			pData->GetIP(),
			pData->GetPort() );
		return NET_OK;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(
			_T("Accept Client Failed (Client ID:%d) (%s:%d)"),
			dwClient,
			pData->GetIP(),
			pData->GetPort() );
		return NET_ERROR;
	}
}

void CClientManager::CloseAllClient()
{
	int nMaxClient = 0;
	int i = 0;
	nMaxClient = m_nMaxClient;

	for (i=0; i<nMaxClient; i++)
	{
		CloseClient(i);
	}
}


//! CloseClient
//! Close client socket 
//bool CClientManager::CloseClient(
//	DWORD dwClient )
//{
//	int nResult = 0;
//
//	if ( true == m_pClient[dwClient].CloseClient() )
//	{
//		if ( dwClient >= NET_RESERVED_SLOT )
//		{
//			LockOn();
//			m_vecSleepCID.push_back( dwClient );
//			LockOff();			
//		}
//
//		return true;
//	}
//
//	return false;
//}

//! CloseClient
//! Close client socket 
int CClientManager::CloseClient(DWORD dwClient, bool bOnlineCheck)
{	
	int nResult = 0;

	LockOn();
	if ( m_pClient[dwClient].IsOnline() || bOnlineCheck == FALSE )
	{
		// ������ ���´�.
		nResult = m_pClient[dwClient].CloseClient();

		// Ŭ���̾�Ʈ ��ȣ�� ���޽��Կ� ���� �ִ´�.
		if ( dwClient >= NET_RESERVED_SLOT )
		{
			m_vecSleepCID.push_back( dwClient );
		}
	}
	LockOff();

	return nResult;
}

int CClientManager::ReleaseClientID( DWORD dwClient )
{
	LockOn();
	if (dwClient >= NET_RESERVED_SLOT)
	{
		m_vecSleepCID.push_back(dwClient);

		LockOff();
		return NET_OK;
	}
	else
	{
		LockOff();
		return NET_ERROR;
	}
}

void CClientManager::Reset(DWORD dwClient)
{
	LockOn();
	m_pClient[dwClient].Reset();
	LockOff();
}

//! Return current client number
int	CClientManager::GetCurrentClientNumber(void)
{
	return m_nMaxClient - (int) m_deqSleepCID.size();
}

int CClientManager::GetNetMode(DWORD dwClient)
{
	return m_pClient[dwClient].GetNetMode();
}

void CClientManager::SetNetMode(DWORD dwClient, int nMode)
{
	m_pClient[dwClient].SetNetMode(nMode);
}

char* CClientManager::GetClientIP(DWORD dwClient)
{
	return m_pClient[dwClient].GetIP();
}

USHORT CClientManager::GetClientPort(DWORD dwClient)
{
	return m_pClient[dwClient].GetPort();
}

int	CClientManager::addRcvMsg(DWORD dwClient, 
	                          void* pMsg, 
				              DWORD dwSize)
{
	ASSERT(pMsg);
	// ���Ź��� ũ�� �� ī���� ���
	m_dwInPacketCount++;
	m_dwInPacketSize += dwSize;
	return m_pClient[dwClient].addRcvMsg(pMsg, dwSize);
}

void* CClientManager::getRcvMsg(DWORD dwClient,bool bClient)

{
	return m_pClient[dwClient].getRcvMsg(bClient);
}

// �޽����� ���� ���۸� �����Ѵ�
void CClientManager::ResetRcvBuffer(DWORD dwClient)
{
	m_pClient[dwClient].ResetRcvBuffer();
}

SOCKET CClientManager::GetSocket(DWORD dwClient)
{
	return m_pClient[dwClient].GetSOCKET();
}

void CClientManager::CloseSocket(DWORD dwClient)
{
	m_pClient[dwClient].CloseSocket();
}

void CClientManager::SendClientFinal()
{
	DWORD dwSendSize = 0;
	for (DWORD dwClient=0; dwClient < (DWORD) m_nMaxClient; ++dwClient)
	{
		dwSendSize = m_pClient[dwClient].getSendSize();
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, m_pClient[dwClient].getSendBuffer());
		}
	}
}

int	CClientManager::SendClient(DWORD dwClient, LPVOID pBuffer)
{
	if (NULL == pBuffer)	return NET_ERROR;
	
	NET_MSG_GENERIC* pNmg = (NET_MSG_GENERIC*) pBuffer;	

	DWORD dwSendSize = pNmg->dwSize;
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSendSize;

	int nResult = m_pClient[dwClient].addSendMsg(pNmg, dwSendSize);
	
	switch (nResult) 
	{
	case CSendMsgBuffer::BUFFER_ERROR:
		return NET_ERROR;
		break;

	case CSendMsgBuffer::BUFFER_ADDED:		
		break;

	case CSendMsgBuffer::BUFFER_SEND:
		dwSendSize = m_pClient[dwClient].getSendSize();
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, m_pClient[dwClient].getSendBuffer());			
		}
		break;

	case CSendMsgBuffer::BUFFER_SEND_ADD:		
		dwSendSize = m_pClient[dwClient].getSendSize();		
		if (dwSendSize > 0) 
		{
			// ����� ������ ����
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, m_pClient[dwClient].getSendBuffer());			
			m_pClient[dwClient].addSendMsg(pBuffer, pNmg->dwSize);
		}
		break;

	default:
		return NET_ERROR;
		break;
	}

	return NET_OK;
}

int	CClientManager::SendClient2(DWORD dwClient, LPVOID pBuffer)
{
	if (pBuffer == NULL)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:SendClient pBuffer NULL") );
		return NET_ERROR;
	}

	LPPER_IO_OPERATION_DATA pIoWrite   = NULL;
	NET_MSG_GENERIC*		pNmg       = NULL;
	DWORD					dwSndBytes = 0;

	assert(pBuffer);
	pNmg = (NET_MSG_GENERIC*) pBuffer;		
	dwSndBytes = pNmg->dwSize;
	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:SendClient dwSndBytes > NET_DATA_BUFSIZE") );
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
			//incSendCount( dwClient );
			CopyMemory( pIoWrite->Buffer, pNmg, dwSndBytes );
			pIoWrite->dwTotalBytes	= dwSndBytes;
			return SendClient2( dwClient, pIoWrite, dwSndBytes );
		}
	}
}

int CClientManager::SendClient2(
	DWORD dwClient, 
	LPPER_IO_OPERATION_DATA PerIoData, 
	DWORD dwSize )
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode  = 0;
	DWORD	dwFlags   = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len   = (u_long) dwSize;
	
	SOCKET sSocket = GetSocket(dwClient);
	if (sSocket == INVALID_SOCKET)
	{
		ReleaseOperationData(PerIoData);
		CloseClient(dwClient);
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
		// WSA_IO_PENDING is not error.
		// IOCP �� ��� ����/���� ����� ���߿� �뺸�Ǳ� ������
		// �����ڵ尡 WSA_IO_PENDING �� ��� ������ �ƴϴ�.
		// �� ���� ���߿� ����� �뺸�ǰ� ����� ����� �����ϰڴٴ� ���̴�.		
		if (nRetCode != WSA_IO_PENDING) 
		{
			// ����� WSA_IO_PENDING �̿��� �����ÿ��� ������ ������ѹ�����.
			::PostQueuedCompletionStatus(
				m_hIOServer,
				0,
				dwClient,
				&(PerIoData->Overlapped) );

			CConsoleMessage::GetInstance()->Write(
				_T("ERROR:CClientManager::SendClient Client(%d)ERR(%d)"),
				dwClient,
				nRetCode );

			return NET_ERROR;
		}
	}

	return NET_OK;
}

// Agent ���������� Client ���� ��� ����
DWORD CClientManager::GetSlotAgentClient(DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotAgentClient();
}

// Agent ���������� Field �������� ��� ����
DWORD CClientManager::GetSlotAgentField (DWORD dwClient)
{
	return m_pClient[dwClient].GetSlotAgentField();
}

// Field ���������� Agent �������� ��� ����
DWORD CClientManager::GetSlotFieldAgent (DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotFieldAgent();
}

// Field ���������� Client ���� ��� ����
DWORD CClientManager::GetSlotFieldClient(DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotFieldClient();
}

// Agent ���������� Client ���� ��� ����
void CClientManager::SetSlotAgentClient(DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotAgentClient(dwSlot);
}

// Agent ���������� Field �������� ��� ����
void CClientManager::SetSlotAgentField (DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotAgentField(dwSlot);
}

// Field ���������� Agent �������� ��� ����
void CClientManager::SetSlotFieldAgent (DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotFieldAgent(dwSlot);
}

// Field ���������� Client ���� ��� ����
void CClientManager::SetSlotFieldClient(DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotFieldClient(dwSlot);
}

void CClientManager::SetSlotType(DWORD dwClient, DWORD dwType)	
{
	m_pClient[dwClient].SetSlotType(dwType);
}

bool CClientManager::IsClientSlot(DWORD dwClient)
{
	return m_pClient[dwClient].IsClientSlot();
}

bool CClientManager::IsAgentSlot(DWORD dwClient)
{
	return m_pClient[dwClient].IsAgentSlot();
}

bool CClientManager::IsFieldSlot(DWORD dwClient)
{
	return m_pClient[dwClient].IsFieldSlot();
}

DWORD CClientManager::GetSlotType(DWORD dwClient)
{
	return m_pClient[dwClient].GetSlotType();
}

void CClientManager::ResetHeartBeat(DWORD dwClient)
{
	m_pClient[dwClient].ResetHeartBeat();
}

void CClientManager::SetHeartBeat(DWORD dwClient)
{
	m_pClient[dwClient].SetHeartBeat();
}

bool CClientManager::CheckHeartBeat(DWORD dwClient)
{
	return m_pClient[dwClient].CheckHeartBeat();
}

void CClientManager::GspSetUserID( DWORD dwClient, const TCHAR* szGspUserID )
{
	m_pClient[dwClient].GspSetUserID( szGspUserID );
}

TCHAR* CClientManager::GspGetUserID( DWORD dwClient )
{
	return m_pClient[dwClient].GspGetUserID();
}

void CClientManager::SetUserID( DWORD dwClient, const TCHAR* szUserID )
{	
	m_pClient[dwClient].SetUserID( szUserID );
}

TCHAR* CClientManager::GetUserID( DWORD dwClient )
{
	return m_pClient[dwClient].GetUserID();
}

void CClientManager::SetUserNum(DWORD dwClient, INT nUserNum)
{
	m_pClient[dwClient].SetUserNum(nUserNum);
}

void CClientManager::SetLoginTime(DWORD dwClient)
{
	m_pClient[dwClient].SetLoginTime();
}

void CClientManager::SetGaeaID(DWORD dwClient, DWORD dwGaeaID)
{
	m_pClient[dwClient].SetGaeaID(dwGaeaID);
}

/*
void CClientManager::SetLastMsgTime(DWORD dwClient, DWORD dwTime)
{
	m_pClient[dwClient].SetLastMsgTime(dwTime);
}
*/

/*
DWORD CClientManager::GetLastMsgTime(DWORD dwClient)
{
	return m_pClient[dwClient].GetLastMsgTime();
}
*/

/*
void CClientManager::SetLastSaveTime(DWORD dwClient, DWORD dwTime)
{
	m_pClient[dwClient].SetLastSaveTime(dwTime);
}
*/

/*
DWORD CClientManager::GetLastSaveTime(DWORD dwClient)
{
	return m_pClient[dwClient].GetLastSaveTime();
}
*/

INT CClientManager::GetLoginTime(DWORD dwClient)
{
	CTime crtTime;
	crtTime = CTime::GetCurrentTime();
	CTimeSpan ts = crtTime - m_pClient[dwClient].GetLoginTime();
	return (INT) ts.GetTotalMinutes();
}

CTime CClientManager::GetLoginTimeEx(DWORD dwClient)
{
	return m_pClient[dwClient].GetLoginTime();	    
}

int CClientManager::GetUserNum(DWORD dwClient) 
{
	if ( dwClient >= (DWORD)m_nMaxClient ) {
		return -1;
	} else {
		return m_pClient[dwClient].GetUserNum();
	}
}

DWORD CClientManager::GetGaeaID(DWORD dwClient)
{
	return m_pClient[dwClient].GetGaeaID();
}




// �����ڰ� �÷��� �������� �˷��ش�.
bool CClientManager::IsGaming(DWORD dwClient)
{

	// �¶��� �����̰� ���̾� ID �� ������ �������̴�
	if (m_pClient[dwClient].IsOnline() && m_pClient[dwClient].GetGaeaID() != GAEAID_NULL && m_pClient[dwClient].GetSlotType() == NET_SLOT_CLIENT)
		return true;
/*
	if (m_pClient[dwClient].IsOnline() && m_pClient[dwClient].GetGaeaID() != GAEAID_NULL )
		return true;
*/
	else
		return false;
}

void CClientManager::SetChaNum(DWORD dwClient, int nChaNum)
{
	m_pClient[dwClient].SetChaNum(nChaNum);
}

int	CClientManager::GetChaNum(DWORD dwClient)
{
	return m_pClient[dwClient].GetChaNum();
}

void CClientManager::SetUserType(DWORD dwClient, int nType)
{
	m_pClient[dwClient].SetUserType(nType);
}

int CClientManager::GetUserType(DWORD dwClient)
{
	return m_pClient[dwClient].GetUserType();
}

void CClientManager::SetChaRemain(DWORD dwClient, WORD wChaNum)
{
	m_pClient[dwClient].SetChaRemain(wChaNum);
}

void CClientManager::SetChaTestRemain(DWORD dwClient, WORD wChaNum)
{
	m_pClient[dwClient].SetChaTestRemain(wChaNum);
}

WORD CClientManager::GetChaRemain(DWORD dwClient)
{
	return m_pClient[dwClient].GetChaRemain();
}

WORD CClientManager::GetChaTestRemain(DWORD dwClient)
{
	return m_pClient[dwClient].GetChaTestRemain();
}

__time64_t CClientManager::GetPremiumDate(DWORD dwClient)
{
    return m_pClient[dwClient].GetPremiumDate();
}

void CClientManager::SetPremiumDate(DWORD dwClient, __time64_t tTime)
{
    m_pClient[dwClient].SetPremiumDate(tTime);
}

__time64_t CClientManager::GetChatBlockDate(DWORD dwClient)
{
	return m_pClient[dwClient].GetChatBlockDate();
}

void CClientManager::SetChatBlockDate(DWORD dwClient, __time64_t tTime)
{
    m_pClient[dwClient].SetChatBlockDate(tTime);
}

void CClientManager::SetChannel(DWORD dwClient, int nChannel)
{
	m_pClient[dwClient].SetChannel(nChannel);
}

int CClientManager::GetChannel(DWORD dwClient)
{
	return m_pClient[dwClient].GetChannel();
}

//! nProtect GameGuard ������ ������ �������� ���� ȣ��
bool CClientManager::nProtectSetAuthQuery(DWORD dwClient)
{
	return m_pClient[dwClient].nProtectSetAuthQuery();
}
    	
//! nProtect GameGuard ������ ���� ��������
GG_AUTH_DATA CClientManager::nProtectGetAuthQuery(DWORD dwClient)
{
	return m_pClient[dwClient].nProtectGetAuthQuery();
}

//! nProtect GameGuard ���� ���� ��������
GG_AUTH_DATA CClientManager::nProtectGetAuthAnswer( DWORD dwClient )
{
	return m_pClient[dwClient].nProtectGetAuthAnswer();
}

//! nProtect GameGuard ���� ���� ����
void CClientManager::nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad)
{
	return m_pClient[dwClient].nProtectSetAuthAnswer(ggad);
}

//! nProtect GameGuard ���� ��� ��������
DWORD CClientManager::nProtectCheckAuthAnswer(DWORD dwClient)
{
	return m_pClient[dwClient].nProtectCheckAuthAnswer();
}

//! nProtect GameGuard ���� ���¸� ���½�Ų��.
void CClientManager::nProtectResetAuth(DWORD dwClient)
{
	m_pClient[dwClient].nProtectResetAuth();
}

void CClientManager::SetRandomPassNumber(DWORD dwClient, int nNumber)
{
	m_pClient[dwClient].SetRandomPassNumber(nNumber);
}

int CClientManager::GetRandomPassNumber(DWORD dwClient)
{
	return m_pClient[dwClient].GetRandomPassNumber();
}

LPPER_IO_OPERATION_DATA CClientManager::GetFreeOverIO(int nType)
{
    if (NET_SEND_POSTED == nType)
	{
		return m_pSendIOCP->GetFreeOverIO(nType);
	}
	else
	{
		return m_pRecvIOCP->GetFreeOverIO(nType);
	}
}

void CClientManager::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
	if (pData == NULL) return;

	if (NET_SEND_POSTED == pData->OperationType)
	{
		m_pSendIOCP->Release(pData);
	}
	else
	{
		m_pRecvIOCP->Release(pData);
	}
}

//! ��� ��Ŷ ī���͸� �ʱ�ȭ �Ѵ�.
void CClientManager::resetPacketCount()
{
	m_dwInPacketCount    = 0;
	m_dwInPacketSize     = 0;	
	m_dwOutPacketCount   = 0;
	m_dwOutPacketSize    = 0;
	m_dwOutCompressCount = 0;
	m_dwOutCompressSize  = 0;
}

// �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCreateM( DWORD dwClient, int nExtremeM )
{	
	m_pClient[dwClient].SetExtremeCreateM( nExtremeM );
}

// �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCreateW( DWORD dwClient, int nExtremeW )
{	
	m_pClient[dwClient].SetExtremeCreateW( nExtremeW );
}

// �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeM( DWORD dwClient )
{
	return m_pClient[dwClient].GetExtremeM();
}

// �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeW( DWORD dwClient )
{
	return m_pClient[dwClient].GetExtremeW();
}

// �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCheckM( DWORD dwClient, int nExtremeCheckM )
{
	m_pClient[dwClient].SetExtremeCheckM( nExtremeCheckM );
}

// �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCheckW( DWORD dwClient, int nExtremeCheckW )
{
	m_pClient[dwClient].SetExtremeCheckW( nExtremeCheckW );
}

// �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeCheckM( DWORD dwClient )
{
	return m_pClient[dwClient].GetExtremeCheckM();
}

// �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeCheckW( DWORD dwClient )
{
	return m_pClient[dwClient].GetExtremeCheckW();
}

// ���� ���� ī�忡 ���� DB���� ����� �ذ��� ���� ���� ����

// DB�� ����� �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCreateDBM( DWORD dwClient, int nExtremeDBM )
{
	m_pClient[dwClient].SetExtremeCreateDBM( nExtremeDBM );
}

// DB�� ����� �ذ��� ���� ���� ������ �����Ѵ�.
void CClientManager::SetExtremeCreateDBW( DWORD dwClient, int nExtremeDBW )
{
	m_pClient[dwClient].SetExtremeCreateDBW( nExtremeDBW );
	
}

// DB�� ����� �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeDBM(DWORD dwClient)
{
	return m_pClient[dwClient].GetExtremeDBM();
}

// DB�� ����� �ذ��� ���� ���� ������ �����´�.
int CClientManager::GetExtremeDBW(DWORD dwClient)
{
	return m_pClient[dwClient].GetExtremeDBW();
}

//// ������ �ʵ� ������ ������ �����Ѵ�
//void CClientManager::SetConnectionFieldInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
//{
//	m_pClient[dwClient].SetConnectionFieldInfo( dwGaeaID, nmg );
//}
//	
//// ������ �ʵ� ������ ������ ��������� �ʱ�ȭ�Ѵ�.
//void CClientManager::ResetConnectionFieldInfo( DWORD dwClient)
//{
//	m_pClient[dwClient].ResetConnectionFieldInfo();
//}
//
//// ������ �ʵ弭���� �� Ŭ���̾�Ʈ�� GaeaID�� �����´�.
//DWORD CClientManager::GetClientGaeaID( DWORD dwClient )
//{
//	return m_pClient[dwClient].GetClientGaeaID();
//}
//
//// �޼����� ������ �״ٰ� �����´�.
//NET_MSG_GENERIC * CClientManager::GetNetMsg( DWORD dwClient )
//{
//	return m_pClient[dwClient].GetNetMsg();
//}

// �±� User Class Type�� �����Ѵ�.
void CClientManager::SetThaiClass( DWORD dwClient, int nThaiCC_Class )
{
	m_pClient[dwClient].SetThaiClass( nThaiCC_Class );
}

// �±� User Class Type�� �����´�.
int CClientManager::GetThaiClass( DWORD dwClient )
{
	return m_pClient[dwClient].GetThaiClass();
}

// �����̽þ� PC�� �̺�Ʈ
// �����̽þ� User Class Type�� �����Ѵ�.
void CClientManager::SetMyClass( DWORD dwClient, int nMyCC_Class )
{
	m_pClient[dwClient].SetMyClass( nMyCC_Class );
}

// �����̽þ� User Class Type�� �����´�.
int CClientManager::GetMyClass( DWORD dwClient )
{
	return m_pClient[dwClient].GetMyClass();
}

// �߱� GameTime�� ������ �д�.
void CClientManager::SetChinaGameTime( DWORD dwClient, int nChinaGameTime )
{
	m_pClient[dwClient].SetChinaGameTime( nChinaGameTime );
}

// �߱� GameTime�� ������ �´�.
int CClientManager::GetChinaGameTime( DWORD dwClient )
{
	return m_pClient[dwClient].GetChinaGameTime();
}

// �߱� OfflineTime�� ������ �д�.
void CClientManager::SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime )
{
	m_pClient[dwClient].SetChinaOfflineTime( nChinaOfflineTime );
}

// �߱� OfflineTime�� ������ �´�.
int CClientManager::GetChinaOfflineTime( DWORD dwClient )
{
	return m_pClient[dwClient].GetChinaOfflineTime();
}

// �߱� LastLoginDate�� �����ϰ� ������ �´�.
void CClientManager::SetLastLoginDate( DWORD dwClient, __time64_t tTime )
{
	m_pClient[dwClient].SetLastLoginDate( tTime );
}

__time64_t CClientManager::GetLastLoginDate( DWORD dwClient )
{
	return m_pClient[dwClient].GetLastLoginDate();
}

// �߱� UserAge�� ������ �д�.
void CClientManager::SetChinaUserAge( DWORD dwClient, int nChinaUserAge )
{
	m_pClient[dwClient].SetChinaUserAge( nChinaUserAge );
}

// �߱� UserAge�� ������ �´�.
int CClientManager::GetChinaUserAge( DWORD dwClient )
{
	return m_pClient[dwClient].GetChinaUserAge();
}

void CClientManager::SetVTGameTime( DWORD dwClient, int nVTGameTime )
{
	m_pClient[dwClient].SetVTGameTime( nVTGameTime );
}

// ��Ʈ�� GameTime�� ������ �´�.
int CClientManager::GetVTGameTime( DWORD dwClient )
{
	return m_pClient[dwClient].GetVTGameTime();
}


/*
//! �ޱ� ��û Ƚ�� ī����
int CClientManager::incRecvCount( DWORD dwClient )
{
	return m_pClient[dwClient].incRecvCount();
}

int CClientManager::decRecvCount( DWORD dwClient )
{
	return m_pClient[dwClient].decRecvCount();
}

int CClientManager::getRecvCount( DWORD dwClient )
{
	return m_pClient[dwClient].getRecvCount();
}

//! ������ ��û Ƚ�� ī����
int CClientManager::incSendCount( DWORD dwClient )
{
	//CConsoleMessage::GetInstance()->Write("CClientManager::incSendCount");
	return m_pClient[dwClient].incSendCount();
}

int CClientManager::decSendCount( DWORD dwClient )
{
	return m_pClient[dwClient].decSendCount();
}

int CClientManager::getSendCount( DWORD dwClient )
{
	return m_pClient[dwClient].getSendCount();
}

PER_IO_OPERATION_DATA* CClientManager::getSendIO(
	DWORD dwClient )
{
	PER_IO_OPERATION_DATA* pData = m_pSendIOCP->GetFreeOverIO( NET_SEND_POSTED );
	if (pData != NULL)
	{
		//CConsoleMessage::GetInstance()->Write("CClientManager::getSendIO");
		m_pClient[dwClient].incSendCount();
	}
	return pData;
}

PER_IO_OPERATION_DATA* CClientManager::getRecvIO(
	DWORD dwClient )
{
	PER_IO_OPERATION_DATA* pData = m_pRecvIOCP->GetFreeOverIO( NET_RECV_POSTED );
	if (pData != NULL)
	{
		m_pClient[dwClient].incRecvCount();
	}
	return pData;
}

void CClientManager::releaseIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
    if (pData == NULL) return;

	if (NET_SEND_POSTED == pData->OperationType)
	{
		releaseSendIO( dwClient, pData );
	}
	else
	{
		releaseRecvIO( dwClient, pData );
	}
}

void CClientManager::releaseSendIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClient[dwClient].decSendCount();
	m_pSendIOCP->Release( pData );
}

void CClientManager::releaseRecvIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClient[dwClient].decRecvCount();
	m_pRecvIOCP->Release( pData );
}
*/