///////////////////////////////////////////////////////////////////////////////
// s_CClientAgent.cpp
//
// class CClientAgent
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright(c) Min Communications. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CClientAgent.h"
#include "s_CConsoleMessage.h"

#include "s_CCsvFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientAgent::CClientAgent(
	int nMaxClient,
	COverlapped* pSendIOCP,
	COverlapped* pRecvIOCP,
	HANDLE	hIOServer )
: CClientManager(
	nMaxClient,
	pSendIOCP,
	pRecvIOCP,
	hIOServer)
{
	m_pField = NULL;
	m_pField = new CNetUser[m_nMaxClient];
}

CClientAgent::~CClientAgent()
{
	if (m_pField != NULL) 
	{	
		LockOn();
		delete[] m_pField;
		m_pField = NULL;
		LockOff();
	}	
}

int CClientAgent::SetAcceptedClient(DWORD dwClient, SOCKET sSocket)
{
	if (CClientManager::SetAcceptedClient(dwClient, sSocket) == NET_OK)
	{
		m_pClient[dwClient].SetSlotAgentClient(dwClient);
		return NET_OK;
	}
	else
	{
		return NET_ERROR;
	}
}

// 맵 ID 를 이용해서 필드서버로 접속한다.
// dwClient : Agent's client number
int	CClientAgent::ConnectFieldServer(
	DWORD dwClient,
	int nFieldServer,
	DWORD dwGaeaID,
	int nServerChannel )
{
	if (nServerChannel < 0 || nServerChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:ConnectFieldServer CH:%d"),
			nServerChannel );
        return NET_ERROR;
	}

	F_SERVER_INFO* pField = NULL;
	DWORD dwSlot = 0;
	int   nRetCode=0;
	
	if (dwClient > (DWORD) m_nMaxClient)
		dwSlot = dwClient-m_nMaxClient;
	else
		dwSlot = dwClient;

	if (m_pField[dwSlot].IsOnline())
	{
		m_pField[dwSlot].CloseClient();
	}

	pField = CCfg::GetInstance()->GetFieldServer(nFieldServer, nServerChannel);

	if (pField == NULL)
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:ConnectFieldServer FIELD:%d CH:%d"),
			nFieldServer,
			nServerChannel );
		return NET_ERROR;
	}

	nRetCode = ConnectFieldServer(
					dwClient, 
					(unsigned long) pField->ulServerIP,
					pField->nServicePort, 
					dwGaeaID,
					nServerChannel );
	if (nRetCode == NET_OK)
	{	
		return NET_OK;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:CClientAgent::ConnectFieldServer %d, %d, %d, %d"), 
			dwClient,
			pField->ulServerIP, 
			pField->nServicePort, 
			dwGaeaID );
		return NET_ERROR;
	}
}

int CClientAgent::ConnectFieldServer(DWORD dwClient, unsigned long lServerIP, int nPort, DWORD dwGaeaID, int nServerChannel)
{
	int			nRetCode = 0;
	SOCKET		fSocket;
	SOCKADDR_IN	inAddr;
	DWORD dwClientNum = dwClient + m_nMaxClient;
	
	fSocket = ::WSASocket(
					PF_INET,
					SOCK_STREAM,
					IPPROTO_TCP,
					NULL,
					0,
					WSA_FLAG_OVERLAPPED );
	if (fSocket == INVALID_SOCKET)
	{
		CConsoleMessage::GetInstance()->Write(
											  _T("ERROR:CClientAgent::ConnectFieldServer socket ERR(%d)"),
											  ::WSAGetLastError());
		return NET_ERROR;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Set the socket option
    // Disable send buffering on the socket.  Setting SO_SNDBUF 
    // to 0 causes winsock to stop bufferring sends and perform 
    // sends directly from our buffers, thereby reducing CPU usage. 
	// Winsock 의 버퍼링을 줄이고 cpu 사용량을 줄이려면 아래 옵션을 활성하 시키면 됨.
	// Set SO_SNDBUF to 0
	// Set SO_RCVBUF to 0
	// Exam)
	// int nZero = 0; 
	// ::setsockopt(fSocket, SOL_SOCKET, SO_SNDBUF, (char *) &nZero, sizeof(nZero)); 
	// ::setsockopt(fSocket, SOL_SOCKET, SO_RCVBUF, (char *) &nZero, sizeof(nZero)); 	
	/////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////////////////////
	// 소켓 Re Use 옵션을 사용하려면 아래 코드를 사용하면 됨
    // Exam)
	// int optval = TRUE;
	// ::setsockopt(fSocket,
	// 	            SOL_SOCKET,
	//              SO_REUSEADDR,
	//              (char*) &optval,
	//              sizeof(optval));
	/////////////////////////////////////////////////////////////////////////////
	
	::SecureZeroMemory(&inAddr, sizeof(SOCKADDR_IN));
	inAddr.sin_family = AF_INET;
	inAddr.sin_port = ::htons((u_short) nPort);
	inAddr.sin_addr.s_addr = lServerIP;

	// Connect to server
	nRetCode = ::connect(fSocket, (sockaddr *) &inAddr, sizeof(SOCKADDR_IN));
	if (nRetCode == SOCKET_ERROR)
	{		
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:CClientAgent::ConnectFieldServer connect ERR(%d)"), 
	                                          ::WSAGetLastError());
		::closesocket(fSocket);
		return NET_ERROR;
	}	
	// Associate the accepted socket with the completion port	
	LPPER_IO_OPERATION_DATA IOData = NULL;
	DWORD dwRcvBytes = 0;
	DWORD Flags = 0;
	HANDLE hRetCode = NULL;	

	hRetCode = ::CreateIoCompletionPort((HANDLE) fSocket, 
									    m_hIOServer, 
									    (DWORD) dwClientNum,
									    0);

	if (hRetCode == NULL ) 
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("CreateIoCompletionPort Error"));
		::closesocket(fSocket);
		return NET_ERROR;
	}

	// Start processing I/O on ther accepted socket
	// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
	// Client 버전 정보를 받는다.
	dwRcvBytes = sizeof(NET_MSG_GENERIC);

	IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

	if (IOData == NULL) return NET_ERROR;

	// IOData->DataBuf.buf = IOData->Buffer;
	IOData->dwRcvBytes		= 0;
	IOData->dwTotalBytes	= dwRcvBytes;
	// Head receive mode
	m_pField[dwClient].SetNetMode(NET_PACKET_HEAD);

	::WSARecv(fSocket,
		      &(IOData->DataBuf), 
		      1,
		      &dwRcvBytes,
		      &Flags ,
		      &(IOData->Overlapped),
		      NULL);

	// Set agent server slot type and GaeaID
	m_pField[dwClient].SetAcceptedClient (fSocket);
	m_pField[dwClient].SetGaeaID         (dwGaeaID);

	m_pField[dwClient].SetSlotAgentClient(dwClient);
	m_pField[dwClient].SetSlotAgentField (dwClient);
	m_pField[dwClient].SetSlotType       (NET_SLOT_FIELD);
	m_pField[dwClient].SetChannel        (nServerChannel);

	m_pClient[dwClient].SetSlotAgentClient(dwClient);
	m_pClient[dwClient].SetSlotAgentField (dwClient);
	m_pClient[dwClient].SetSlotType       (NET_SLOT_CLIENT);
	m_pClient[dwClient].SetChannel        (nServerChannel);

	return NET_OK;
}

// CloseField
// Close field socket 
int CClientAgent::CloseField(
	DWORD dwClient )
{	
	int nResult = 0;
	LockOn();

    if (dwClient >= (DWORD) m_nMaxClient)
    {
        dwClient = (DWORD) (dwClient - m_nMaxClient );
    }
	/*
	if (dwClient >=0 && dwClient < NET_RESERVED_SLOT)
	{
		// 서버간 통신 종료됨
		CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "INFO:%d 필드서버와 연결을 끊습니다", dwClient);
		nResult = m_pClient[dwClient].CloseClient();
	}
	else
	{
		nResult = m_pField[dwClient].CloseClient();	
	}
	*/

	nResult = m_pField[dwClient].CloseClient();
	LockOff();

	return nResult;
}

SOCKET CClientAgent::GetFieldSocket(DWORD dwClient)
{
	return m_pField[dwClient].GetSOCKET();
}

void CClientAgent::SendClientFinal()
{
	CClientManager::SendClientFinal();

	DWORD dwSendSize = 0;
	
	for (DWORD dwClient=0; dwClient < (DWORD) m_nMaxClient; ++dwClient)
	{
		dwSendSize = m_pField[dwClient].getSendSize();
		if (dwSendSize > 0)
		{
			// 압축된 데이터 전송
			m_dwOutCompressCount++;
			m_dwOutCompressSize += dwSendSize;
			SendClient2(dwClient, m_pField[dwClient].getSendBuffer());
		}
	}
}

int CClientAgent::Send(DWORD dwClient, LPVOID pBuffer)
{
	ASSERT(pBuffer);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return SendField(dwClient, pBuffer);
	}
	else
	{
		return SendClient(dwClient, pBuffer);
	}
}

int CClientAgent::Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
	ASSERT(PerIoData);
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return SendField(dwClient, PerIoData, dwSize);
	}
	else
	{
		return SendClient2(dwClient, PerIoData, dwSize);
	}
}

int	CClientAgent::SendField(DWORD dwClient, LPVOID pBuffer)
{
	if (pBuffer == NULL) return NET_ERROR;

	LPPER_IO_OPERATION_DATA pIoWrite = NULL;
	
	DWORD dwSndBytes = 0;

	assert(pBuffer);

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pBuffer;

	if (nmg == NULL)
	{
		CConsoleMessage::GetInstance()->Write("ERROR:SendField nmg == NULL");
		return 0;
	}

	dwSndBytes = nmg->dwSize;
	m_dwOutPacketCount++;
	m_dwOutPacketSize += dwSndBytes;

	if (dwSndBytes > NET_DATA_BUFSIZE)
	{
		CConsoleMessage::GetInstance()->Write("ERROR:SendField %d dwSndBytes > NET_DATA_BUFSIZE", dwClient);
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
			return SendField(dwClient, pIoWrite, dwSndBytes);
		}
	}
}

static int nSendFieldError = 0;

int CClientAgent::SendField(
	DWORD dwClient,
	LPPER_IO_OPERATION_DATA PerIoData,
	DWORD dwSize )
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len   = static_cast<u_long> (dwSize);
	
	if (dwClient < (DWORD) m_nMaxClient) 
	{
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
    
	SOCKET sSocket = GetFieldSocket(dwClient-m_nMaxClient);

	if (sSocket == INVALID_SOCKET)
	{
		if ( (dwClient-m_nMaxClient) >= NET_RESERVED_SLOT)
		{
			CConsoleMessage::GetInstance()->Write(
                _T("ERROR:SendField GetFieldSocket %d"),
				dwClient-m_nMaxClient );
		}
		ReleaseOperationData(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(
					sSocket,
					&(PerIoData->DataBuf),
					1,
					&dwSndSize,
					dwFlags,
					&(PerIoData->Overlapped),
					NULL );

	if (nRetCode == SOCKET_ERROR) 
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSA_IO_PENDING) // WSA_IO_PENDING is not error.
		{	
			// 현재는 WSA_IO_PENDING 이외의 에러시에는 연결을 종료시켜버린다.
			// CloseField(dwClient);
			::PostQueuedCompletionStatus(
								m_hIOServer,
							    0,
							    dwClient, 
							    &PerIoData->Overlapped );
			if (nSendFieldError < 100)
			{
				NET_MSG_GENERIC* pMsg = (NET_MSG_GENERIC*) PerIoData->Buffer;

				if ( (pMsg != NULL) && (pMsg->nType > 0) )
				{
					CConsoleMessage::GetInstance()->Write(
												_T("ERROR:SendField Client(%d) Error(%d) nType(%d)"),
												dwClient,
												nRetCode,
												pMsg->nType );
					nSendFieldError++;
				}
			}
			return NET_ERROR;
		}
	}

//	NetOutCountIncrease(dwClient);
	return NET_OK;
}

int CClientAgent::addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize)
{
	ASSERT(pMsg);
	if (dwSize > NET_DATA_BUFSIZE)	return NET_ERROR;

	// 수신받은 크기 및 카운터 기록
	m_dwInPacketCount++;
	m_dwInPacketSize += dwSize;

	if (dwClient >= (DWORD) m_nMaxClient)
		return m_pField[dwClient-m_nMaxClient].addRcvMsg(pMsg, dwSize);
	else
		return m_pClient[dwClient].addRcvMsg(pMsg, dwSize);
}

void CClientAgent::ResetRcvBuffer(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField[dwClient-m_nMaxClient].ResetRcvBuffer();
	else									m_pClient[dwClient].ResetRcvBuffer();
}

void* CClientAgent::getRcvMsg(DWORD dwClient)
{
	// 필드로 부터 온 메시지
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].getRcvMsg(FALSE);
	// 클라이언트로 부터 온 메시지
	else									return m_pClient[dwClient].getRcvMsg(dwClient<NET_RESERVED_SLOT ? FALSE : TRUE);
}

/*
char* CClientAgent::GetMsg(DWORD dwClient)
{
	// 필드로 부터 온 메시지
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].GetMsg();
	// 클라이언트로 부터 온 메시지
	else									return m_pClient[dwClient].GetMsg();
}
*/

SOCKET CClientAgent::GetSocket(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].GetSOCKET();
	else									return m_pClient[dwClient].GetSOCKET();
}

int	CClientAgent::GetCurrentClientNumber(void)
{
	return m_nMaxClient - (int) m_deqSleepCID.size() - NET_RESERVED_SLOT;
}

void CClientAgent::SetHeartBeat(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField[dwClient-m_nMaxClient].SetHeartBeat();
    else							        m_pClient[dwClient].SetHeartBeat();
}

void CClientAgent::ResetHeartBeat(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField[dwClient-m_nMaxClient].ResetHeartBeat();
	else						  		    m_pClient[dwClient].ResetHeartBeat();
}

bool CClientAgent::CheckHeartBeat(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].CheckHeartBeat();
	else									return m_pClient[dwClient].CheckHeartBeat();
}

/*
void CClientAgent::SetLastMsgTime(DWORD dwClient, DWORD dwTime)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].SetLastMsgTime(dwTime);
	else									m_pClient[dwClient].SetLastMsgTime(dwTime);
}
*/

/*
DWORD CClientAgent::GetLastMsgTime(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].GetLastMsgTime();
	else									return m_pClient[dwClient].GetLastMsgTime();
}
*/

/*
void CClientAgent::SetLastSaveTime(DWORD dwClient, DWORD dwTime)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField[dwClient-m_nMaxClient].SetLastSaveTime(dwTime);
	else									m_pClient[dwClient].SetLastSaveTime(dwTime);
}
*/

/*
DWORD CClientAgent::GetLastSaveTime(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].GetLastSaveTime();
	else									return m_pClient[dwClient].GetLastSaveTime();
}
*/

bool CClientAgent::IsOnline(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].IsOnline();
	else									return m_pClient[dwClient].IsOnline();
}

bool CClientAgent::IsFieldOnline(DWORD dwClient)
{
    if (dwClient >= (DWORD) m_nMaxClient)	return m_pField[dwClient-m_nMaxClient].IsOnline();
    else							        return m_pField[dwClient].IsOnline();
}

DWORD CClientAgent::GetGaeaID(DWORD dwClient)
{	
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField[dwClient-m_nMaxClient].GetGaeaID();		
	else									return m_pClient[dwClient].GetGaeaID();		
}

int CClientAgent::GetChaNum(DWORD dwClient)
{	
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField[dwClient-m_nMaxClient].GetChaNum();
	else									return m_pClient[dwClient].GetChaNum();
}

DWORD CClientAgent::GetSlotType(DWORD dwClient)
{
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField [dwClient-m_nMaxClient].GetSlotType();
	else									return m_pClient[dwClient].GetSlotType();
}

TCHAR* CClientAgent::GetClientIP(DWORD dwClient)
{
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField [dwClient-m_nMaxClient].GetIP();
	else									return m_pClient[dwClient].GetIP();
}

USHORT CClientAgent::GetClientPort(DWORD dwClient)
{
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField [dwClient-m_nMaxClient].GetPort();
	else									return m_pClient[dwClient].GetPort();
}

__time64_t CClientAgent::GetPremiumDate(DWORD dwClient)
{
   	if (dwClient > (DWORD)m_nMaxClient)		return m_pField [dwClient-m_nMaxClient].GetPremiumDate();
	else									return m_pClient[dwClient].GetPremiumDate();
}

void CClientAgent::SetPremiumDate(DWORD dwClient, __time64_t tTime)
{
    if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].SetPremiumDate(tTime);
	else									m_pClient[dwClient].SetPremiumDate(tTime);
}

__time64_t CClientAgent::GetChatBlockDate(DWORD dwClient)
{
	if (dwClient > (DWORD)m_nMaxClient)		return m_pField [dwClient-m_nMaxClient].GetChatBlockDate();
	else									return m_pClient[dwClient].GetChatBlockDate();
}

void CClientAgent::SetChatBlockDate(DWORD dwClient, __time64_t tTime)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].SetChatBlockDate(tTime);
	else									m_pClient[dwClient].SetChatBlockDate(tTime);
}

void CClientAgent::KorSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].KorSetUUID( szUUID );
	else									m_pClient[dwClient].KorSetUUID( szUUID );
}

TCHAR* CClientAgent::KorGetUUID( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].KorGetUUID();
	else									return m_pClient[dwClient].KorGetUUID();
}

void CClientAgent::JPSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].JPSetUUID( szUUID );
	else									m_pClient[dwClient].JPSetUUID( szUUID );
}

TCHAR* CClientAgent::JPGetUUID( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].JPGetUUID();
	else									return m_pClient[dwClient].JPGetUUID();
}

void CClientAgent::GspSetUUID( DWORD dwClient, const TCHAR* szUUID )
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].GspSetUUID( szUUID );
	else									m_pClient[dwClient].GspSetUUID( szUUID );
}

TCHAR* CClientAgent::GspGetUUID( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].GspGetUUID();
	else									return m_pClient[dwClient].GspGetUUID();
}

void    CClientAgent::SetEncKey( DWORD dwClient, const TCHAR* szEncKey )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return;
	else									m_pClient[dwClient].SetEncKey( szEncKey );
}

TCHAR*  CClientAgent::GetEncKey( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].GetEncKey();
	else									return m_pClient[dwClient].GetEncKey();
}

void CClientAgent::TerraSetDecodedTID( DWORD dwClient, const TCHAR* szDecodedTID )
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].TerraSetDecodedTID(szDecodedTID);
	else									m_pClient[dwClient].TerraSetDecodedTID(szDecodedTID);
}

TCHAR* CClientAgent::TerraGetDecodedTID( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].TerraGetDecodedTID();
	else									return m_pClient[dwClient].TerraGetDecodedTID();
}

void CClientAgent::SetChannel(DWORD dwClient, int nChannel)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pField [dwClient-m_nMaxClient].SetChannel(nChannel);
	else									m_pClient[dwClient].SetChannel(nChannel);
}

int CClientAgent::GetChannel(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pField [dwClient-m_nMaxClient].GetChannel();
	else									return m_pClient[dwClient].GetChannel();
}

bool CClientAgent::IsAccountPass(DWORD dwClient) // 계정 인증(login) 통과하였는지 점검.
{
	if (dwClient >= (DWORD) m_nMaxClient)   return m_pClient[dwClient-m_nMaxClient].IsAccountPass();
	else                                    return m_pClient[dwClient].IsAccountPass();
}

void CClientAgent::SetAccountPass (DWORD dwClient, bool bPASS) // 계정 인증(login) 통과 설정.
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pClient[dwClient-m_nMaxClient].SetAccountPass(bPASS);
	else                                    m_pClient[dwClient].SetAccountPass(bPASS);
}

// 계정 인증중인지 검사
bool CClientAgent::IsAccountPassing(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)   return m_pClient[dwClient-m_nMaxClient].IsAccountPassing();
	else                                    return m_pClient[dwClient].IsAccountPassing();
}

// 계정 인증중 세팅
void CClientAgent::SetAccountPassing(DWORD dwClient, bool bPassing)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pClient[dwClient-m_nMaxClient].SetAccountPassing(bPassing);
	else                                    m_pClient[dwClient].SetAccountPassing(bPassing);
}

//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
//! 사용자의 접속을 끊음...
//! 로그인 요청 횟수 증가시킴
void CClientAgent::IncLoginAttempt(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pClient[dwClient-m_nMaxClient].IncLoginAttempt();
	else                                    m_pClient[dwClient].IncLoginAttempt();
}

//! 로그인 요청 횟수 가져옴
WORD CClientAgent::GetLoginAttempt(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pClient[dwClient-m_nMaxClient].GetLoginAttempt();
	else                                    return m_pClient[dwClient].GetLoginAttempt();
}

/**
* Thailand 의 사용자 플래그를 세팅한다.
* \param dwClient
* \param wFlag 
*/
void CClientAgent::ThaiSetFlag (DWORD dwClient, WORD wFlag)
{
	if (dwClient >= (DWORD) m_nMaxClient)	m_pClient[dwClient-m_nMaxClient].ThaiSetFlag (wFlag);
	else                                    m_pClient[dwClient].ThaiSetFlag (wFlag);
}

WORD CClientAgent::ThaiGetFlag (DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pClient[dwClient-m_nMaxClient].ThaiGetFlag();
	else                                    return m_pClient[dwClient].ThaiGetFlag();
}

void CClientAgent::ThaiSetTodayPlayTime (DWORD dwClient, WORD wMinutes)
{
	if (dwClient >= (DWORD) m_nMaxClient)	return m_pClient[dwClient-m_nMaxClient].ThaiSetTodayPlayTime (wMinutes);
	else                                    return m_pClient[dwClient].ThaiSetTodayPlayTime (wMinutes);
}

bool CClientAgent::ThaiIs3HourOver (DWORD dwClient, CTime& currentTime)
{
	if (dwClient >= (DWORD) m_nMaxClient)	
	{
		return m_pClient[dwClient-m_nMaxClient].ThaiIs3HourOver(currentTime);
	}
	else
	{
		return m_pClient[dwClient].ThaiIs3HourOver(currentTime);
	}
}

void CClientAgent::SaveFreeSlot()
{
	SERVER_UTIL::CCsvFile SaveFile(_T("dequeSleepCID"));
	LockOn();	
	for (int nSize=0; nSize < (int) m_deqSleepCID.size(); nSize++)
	{
		SaveFile.Write(_T("%d"), m_deqSleepCID[nSize]);          
	}
	LockOff();
}

/*
//! -----------------------------------------------------------------------
//! Virtual Function
//! 받기 요청 횟수 카운터
int CClientAgent::incRecvCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) {
		return m_pField [dwClient-m_nMaxClient].incRecvCount();
	}
	else {
		return m_pClient[dwClient].incRecvCount();
	}
}

int CClientAgent::decRecvCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) {
		return m_pField [dwClient-m_nMaxClient].decRecvCount();
	}
	else {
		return m_pClient[dwClient].decRecvCount();
	}
}

int CClientAgent::getRecvCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) {
		return m_pField [dwClient-m_nMaxClient].getRecvCount();
	}
	else {
		return m_pClient[dwClient].getRecvCount();
	}
}

//! 보내기 요청 횟수 카운터
int CClientAgent::incSendCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) 
	{
		//CConsoleMessage::GetInstance()->Write("CClientAgent::incSendCount");
		return m_pField [dwClient-m_nMaxClient].incSendCount();
	}
	else 
	{
		//CConsoleMessage::GetInstance()->Write("CClientAgent::incSendCount");
		return m_pClient[dwClient].incSendCount();
	}
}

int CClientAgent::decSendCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) {
		return m_pField [dwClient-m_nMaxClient].decSendCount();
	}
	else {
		return m_pClient[dwClient].decSendCount();
	}
}

int CClientAgent::getSendCount( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient) {
		return m_pField [dwClient-m_nMaxClient].getSendCount();
	}
	else {
		return m_pClient[dwClient].getSendCount();
	}
}

PER_IO_OPERATION_DATA* CClientAgent::getSendIO(
	DWORD dwClient )
{
	PER_IO_OPERATION_DATA* pData = m_pSendIOCP->GetFreeOverIO( NET_SEND_POSTED );
	if (pData != NULL)
	{
		if (dwClient >= (DWORD) m_nMaxClient) 
		{
			m_pField [dwClient-m_nMaxClient].incSendCount();
			//CConsoleMessage::GetInstance()->Write("CClientAgent::getSendIO");
		}
		else
		{
			m_pClient[dwClient].incSendCount();
			//CConsoleMessage::GetInstance()->Write("CClientAgent::getSendIO");
		}
	}
	return pData;
}

PER_IO_OPERATION_DATA* CClientAgent::getRecvIO(
	DWORD dwClient )
{
	PER_IO_OPERATION_DATA* pData = m_pRecvIOCP->GetFreeOverIO( NET_RECV_POSTED );
	if (pData != NULL)
	{
		if (dwClient >= (DWORD) m_nMaxClient) 
		{
			m_pField[dwClient-m_nMaxClient].incRecvCount();
		}
		else
		{
			m_pClient[dwClient].incRecvCount();
		}
	}
	return pData;
}

void CClientAgent::releaseSendIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	if (dwClient >= (DWORD) m_nMaxClient) 
	{
		m_pField[dwClient-m_nMaxClient].decSendCount();
	}
	else
	{
		m_pClient[dwClient].decSendCount();;
	}	
	m_pSendIOCP->Release( pData );
}

void CClientAgent::releaseRecvIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	if (dwClient >= (DWORD) m_nMaxClient) 
	{
		m_pField[dwClient-m_nMaxClient].decRecvCount();
	}
	else
	{
		m_pClient[dwClient].decRecvCount();;
	}
	m_pRecvIOCP->Release( pData );
}
*/