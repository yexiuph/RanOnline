#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CAgentServer::IncreaseChannelUser(
	int nChannel )
{
	LockOn();
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		LockOff();
		return NET_ERROR;
	}
	else
	{
		++m_ChannelUser[nChannel];
		LockOff();
		SessionSndChannelState(nChannel);



		/*if( m_ChannelUser[nChannel] >= (m_pClientManager->GetCurrentClientNumber() + NET_RESERVED_SLOT) )
		{
			CConsoleMessage::GetInstance()->WriteConsole(_T("INFO:Channel(%d) CHUserNum(%d) >= CurrentNumber+100(%d)"),	
														     nChannel, m_ChannelUser[nChannel], (m_pClientManager->GetCurrentClientNumber() + NET_RESERVED_SLOT));
		}*/

		/*CConsoleMessage::GetInstance()->Write(
			_T("### INCREASE_CHANNEL_USER   CHUserNum : %d deqIDSize : %d CurrentClientNum : %d"), 
			m_ChannelUser[nChannel], m_pClientManager->m_deqSleepCID.size(), m_pClientManager->GetCurrentClientNumber() );*/


		return NET_OK;
	}
}

int CAgentServer::DecreaseChannelUser(
	int nChannel )
{	
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->WriteConsole(
			_T("ERROR:DecreaseChannelUser Channel Number Wrong(%d)"),
			nChannel);
		return NET_ERROR;
	}
	else
	{
		--m_ChannelUser[nChannel];
		SessionSndChannelState(nChannel);
		return NET_OK;
	}
}

int CAgentServer::GetChannelUser(
	int nChannel )
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->WriteConsole(
            _T("ERROR:GetChannelUser Channel Number Wrong(%d)"),
			nChannel );
		return NET_ERROR;
	}
	else
	{
		return m_ChannelUser[nChannel];
	}
}

bool CAgentServer::IsChannelFull(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:IsChannelFull nChannel:%d"),
			nChannel );
		return true;
	}
	else
	{
		// if (m_ChannelUser[nChannel] > (m_nMaxChannelUser-NET_RESERVED_SLOT))
		if (m_ChannelUser[nChannel] >= m_nMaxChannelUser)
		{
			CConsoleMessage::GetInstance()->Write(
				_T("INFO:Channel(%d) is full, CHUserNum : %d vecIDSize : %d deqIDSize : %d MaxClientNum : %d CurrentClientNum : %d "), 					
				nChannel, m_ChannelUser[nChannel], m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
				m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber() );

			

			return true;
		}
		else
		{
			return false;
		}
	}
}

/******************************************************************************
* Field Server Communication Member Function
* If you not use Field Server, this functions are unnecessary
*/

// 필드서버와 연결한다.
// Field 다운후 Field 가 재 가동되었을때도 호출된다.
void CAgentServer::FieldConnectAll()
{
	// 서버의 채널 갯수와 필드서버 갯수를 가져온다.
	bool bReconnect = false;
	int nMaxFiledNumber = CCfg::GetInstance()->GetMaxFieldNumber();
	for (int nChannel=0; nChannel<m_nServerChannelNumber; nChannel++)
	{
		for (int nFieldNumber=0; nFieldNumber <= nMaxFiledNumber; nFieldNumber++)
		{
			if ( false == m_pClientManager->IsOnline( (DWORD) nChannel*MAX_CHANNEL_NUMBER+nFieldNumber ) )
			{
				bReconnect = true;
				FieldConnect( nFieldNumber, nChannel );
			}
		}
	}

    if ( true == bReconnect )
	{
		// 필드를 다시 연결했기 때문에 동기화를 위해서 리셋해준다.
		// 필드 서버에 PERIOD time update
		GLMSG::SNET_PERIOD NetPeriod;
		NetPeriod.sPRERIODTIME = GLPeriod::GetInstance().GetPeriod ();
		SendAllChannel( &NetPeriod );

		// 무효화되는 정보 리샛을 필드에 요청.		
		GLMSG::SNET_FIELDINFO_RESET NetMsgInfoReset;
		SendAllChannel( &NetPeriod );

		// 무효화되는 정보 리셋.
		GLAgentServer::GetInstance().ResetFieldInfo ();
	}
}

int CAgentServer::FieldConnect(
	int nFieldNum,
	int nChannel )
{
	if ( nChannel < 0 || nChannel >= m_nServerChannelNumber )
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:FieldConnect %d"),
			nChannel );
		return NET_ERROR;
	}

	F_SERVER_INFO* pField = CCfg::GetInstance()->GetFieldServerArray();
	if ( pField == NULL )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentServer::FieldConnect pField==NULL") );
		return NET_ERROR;
	}

	if ( pField[nChannel*MAX_CHANNEL_NUMBER+nFieldNum].IsValid())
	{
		if ( true == m_pClientManager->IsOnline( nChannel*MAX_CHANNEL_NUMBER+nFieldNum ) )
		{
			return NET_OK;
		}
		else
		{
			return FieldConnect(
						pField[nChannel*MAX_CHANNEL_NUMBER+nFieldNum].ulServerIP,
				        pField[nChannel*MAX_CHANNEL_NUMBER+nFieldNum].nServicePort,
						nFieldNum,
						nChannel );
		}
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:FieldConnect CH:%d FIELD:%d"),
			nChannel,
			nFieldNum );
		return NET_ERROR;
	}
}

int	CAgentServer::FieldConnect(
	unsigned long ulServerIP,
	int nPort,
	int nFieldNum,
	int nChannel )
{	
	LPPER_IO_OPERATION_DATA pIOData = NULL;
	HANDLE hRetCode  = NULL;
	DWORD dwRcvBytes = 0;
	DWORD Flags      = 0;
	int nRetCode     = 0;
	
	SOCKET sField = ::WSASocket(
						PF_INET,
						SOCK_STREAM,
						IPPROTO_TCP,
						NULL,
						0,
						WSA_FLAG_OVERLAPPED );
	if ( INVALID_SOCKET == sField )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CAgentServer::FieldConnect INVALID_SOCKET==sField %d"),
			WSAGetLastError() );
		return NET_ERROR;
	}

	SOCKADDR_IN Addr;
	::SecureZeroMemory(&Addr, sizeof(SOCKADDR_IN));
	Addr.sin_family      = AF_INET;
	Addr.sin_addr.s_addr = ulServerIP;
	Addr.sin_port        = ::htons( (u_short) nPort );

	// Connect to server
	nRetCode = ::connect( sField, (sockaddr *) &Addr, sizeof(SOCKADDR_IN) );
	if ( nRetCode == SOCKET_ERROR )
	{	
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CAgentServer::FieldConnect connect CH(%d) FIELD(%d)"),
			nChannel,
			nFieldNum );
		return NET_ERROR;
	}

	DWORD dwID = (DWORD) (MAX_CHANNEL_NUMBER * nChannel + nFieldNum);

	// Associate the accepted socket with the completion port
	hRetCode = ::CreateIoCompletionPort(
					(HANDLE) sField,
					m_hIOServer,
					dwID,
					0 );

	if ( hRetCode == NULL )
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:CreateIoCompletionPort Error"));
		return NET_ERROR;
	}

	// Start processing I/O on ther accepted socket
	// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
	// Client 버전 정보를 받는다.
	dwRcvBytes = sizeof(NET_MSG_GENERIC);

	//pIOData = getRecvIO( dwID );
	pIOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);
	if (pIOData == NULL) return NET_ERROR;
	
	pIOData->dwRcvBytes		= 0;
	pIOData->dwTotalBytes	= dwRcvBytes;
	// Head receive mode
	m_pClientManager->SetNetMode( dwID, NET_PACKET_HEAD );

	nRetCode = ::WSARecv(
					sField,
		            &(pIOData->DataBuf), 
		            1,
		            &dwRcvBytes,
		            &Flags ,
		            &(pIOData->Overlapped),
		            NULL );
	if ( nRetCode == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:FieldConnect WSARecv"));		
		//releaseRecvIO( dwID, pIOData );
		ReleaseOperationData(pIOData);
		return NET_ERROR;
	}
	else
	{
		// Type is FIELD Server, all slots are itself.
		m_pClientManager->SetAcceptedClient( dwID, sField );
		m_pClientManager->SetSlotType( dwID, NET_SLOT_FLDSVR );
		m_pClientManager->SetSlotAgentField( dwID, dwID );
		m_pClientManager->SetSlotAgentClient( dwID, dwID );
		m_pClientManager->SetChannel( dwID, nChannel );

		CConsoleMessage::GetInstance()->Write(
			_T("INFO:Field Server CH(%d) FIELD(%d) ID(%d) Connection OK"), 
			nChannel,
			nFieldNum,
			dwID );

		// 필드서버에 접속후 자신이 Agent 서버임을 알리고 서버간 통신을 위한 정보를 전송한다.	
		NET_MSG_GENERIC nmg;
		nmg.dwSize = sizeof(NET_MSG_GENERIC);
		nmg.nType = NET_MSG_I_AM_AGENT;
		return SendClient((DWORD) dwID, &nmg);
	}
}

void CAgentServer::BroadcastToField(LPVOID pBuffer, int nServerChannel)
{
	if (pBuffer == NULL ||
		nServerChannel < 0 ||
		nServerChannel >= m_nServerChannelNumber) 
	{
		CConsoleMessage::GetInstance()->Write( 
			                                  _T("ERROR:BroadcastToField CH:%d"),
											  nServerChannel);
		return;
	}

	for (DWORD dwN=0; dwN<NET_RESERVED_SLOT; dwN++)
	{
		if (m_pClientManager->GetSlotType(dwN) == NET_SLOT_FLDSVR &&
			m_pClientManager->IsOnline(dwN) &&
			m_pClientManager->GetChannel(dwN) == nServerChannel)
		{
			if (BroadcastToField(dwN, pBuffer) == NET_ERROR)
			{
				CConsoleMessage::GetInstance()->Write( 
					                                  _T("ERROR:BroadcastToField:%d CH:%d"), 
													  dwN, 
													  nServerChannel);
			}
		}
	}
}

void CAgentServer::SendAllChannel( LPVOID pBuffer )
{ 
	if (pBuffer == NULL) return;

	for (DWORD dwN=0; dwN<NET_RESERVED_SLOT; dwN++)
	{
		if (m_pClientManager->GetSlotType(dwN) == NET_SLOT_FLDSVR &&
			m_pClientManager->IsOnline(dwN))
		{
			if (BroadcastToField(dwN, pBuffer) == NET_ERROR)
			{
				CConsoleMessage::GetInstance()->Write( 
					                                  _T("ERROR:SendAllChannel:%d"), 
													  dwN);
			}
		}
	}
}

int CAgentServer::BroadcastToField(
	DWORD dwClient,
	LPVOID pBuffer )
{
	return SendClient( dwClient, pBuffer );
}

int	CAgentServer::BroadcastToField(
	int nSvrNum,
	LPVOID pBuffer,
	int nServerChannel )
{	
	DWORD dwClient = (DWORD) (nServerChannel * MAX_CHANNEL_NUMBER + nSvrNum);
	return SendClient(dwClient, pBuffer);
}

int CAgentServer::FieldCloseConnect(int nSvrNum)
{	
	int nRetCode = 0;
	SOCKET sTemp = INVALID_SOCKET;
	sTemp = m_pClientManager->GetSocket((DWORD) nSvrNum);
	if (sTemp != INVALID_SOCKET)
	{
		/*
        LINGER      lingerStruct;     
		lingerStruct.l_onoff  = 1;  
		lingerStruct.l_linger = 5;
		nRetCode = ::setsockopt(sTemp, 
						        SOL_SOCKET, 
							    SO_LINGER, 
							    (char *) &lingerStruct, 
							    sizeof(lingerStruct));
		*/

		// ::shutdown(sTemp, SD_SEND);
		nRetCode = ::closesocket(sTemp);
		
		if (nRetCode == SOCKET_ERROR)
		{
			CConsoleMessage::GetInstance()->Write(
				                                  _T("INFO:FieldCloseConnect %d closesocket %d"),
												  nSvrNum,
												  WSAGetLastError());
		}
		m_pClientManager->Reset((DWORD) nSvrNum);
		return NET_OK;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write( _T("ERROR:FieldCloseConnect INVALID_SOCKET"));
		return NET_OK;
	}
}

void CAgentServer::FieldCloseConnectAll()
{
	if ( !m_pClientManager )	return;

	for (DWORD i=0; i<NET_RESERVED_SLOT; i++)
	{
		if (m_pClientManager->GetSlotType(i) == NET_SLOT_FLDSVR &&
			m_pClientManager->IsOnline(i))
		{
            FieldCloseConnect((INT) i);
		}
	}
}
