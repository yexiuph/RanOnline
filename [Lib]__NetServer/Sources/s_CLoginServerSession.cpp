///////////////////////////////////////////////////////////////////////////////
// s_CLoginServerSession.cpp
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright(c) Min Communications. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CLoginServer.h"

//#include "../LoginServer/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CLoginServer::SessionConnect()
{
	return SessionConnect(
				CCfg::GetInstance()->GetSessionServerIP(),
				CCfg::GetInstance()->GetSessionServerPort() );
}

int	CLoginServer::SessionConnect(
	const char* szServerIP,
	int nPort )
{
	if ( true == m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
	{	
		CConsoleMessage::GetInstance()->Write(
			_T("INFO:CLoginServer::SessionConnect already connected") );
		return NET_OK;
	}

	if ( NET_OK != SessionConnect( ::inet_addr(szServerIP), nPort ) )
	{
		CConsoleMessage::GetInstance()->Write(_T("================================"));
		CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
		CConsoleMessage::GetInstance()->Write(_T("================================"));
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

int CLoginServer::SessionConnect(
	unsigned long ulServerIP,
	int nPort )
{
	int nRetCode = 0;

	SOCKET sSession = ::WSASocket(
							PF_INET,
							SOCK_STREAM,
							IPPROTO_TCP,
							NULL,
							0,
							WSA_FLAG_OVERLAPPED );
	if ( INVALID_SOCKET == sSession )
	{
		CConsoleMessage::GetInstance()->Write(
            _T("ERROR:WSASocket sSession==INVALID_SOCKET %d"),
			WSAGetLastError());
		return NET_ERROR;
	}

	SOCKADDR_IN Addr;
	::SecureZeroMemory( &Addr, sizeof(SOCKADDR_IN) );
	Addr.sin_family      = AF_INET;
	Addr.sin_port        = ::htons( (u_short) nPort );
	Addr.sin_addr.s_addr = ulServerIP;

	// Connect to server
	nRetCode = ::connect( sSession, (sockaddr *)&Addr, sizeof(SOCKADDR_IN) );
	if ( nRetCode == SOCKET_ERROR )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CLoginServer::SessionConnect connect %d"),
			WSAGetLastError() );
		::closesocket(sSession);
		return NET_ERROR;
	}

	//m_dwSession++;
	//if (m_dwSession >= NET_SESSION_END)
	//{
	//	m_dwSession = NET_SESSION_START;
	//}

	m_pClientManager->SetAcceptedClient( NET_RESERVED_SESSION, sSession );

	// Associate the accepted socket with the completion port	
	LPPER_IO_OPERATION_DATA IOData = NULL;
	DWORD dwRcvBytes = 0;
	DWORD Flags = 0;
	HANDLE hRetCode = NULL;	

	hRetCode = CreateIoCompletionPort(
					(HANDLE) sSession, 
					m_hIOServer, 
					(DWORD) NET_RESERVED_SESSION,
					0 );
	if (hRetCode == NULL) 
	{
		CConsoleMessage::GetInstance()->Write( _T("CreateIoCompletionPort Error") );
		::closesocket(sSession);
		return NET_ERROR;
	}	
	
	//IOData = getRecvIO( m_dwSession );
	IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);	

	if (IOData == NULL) 
	{
		return NET_ERROR;
	}
	else
	{		
		dwRcvBytes = sizeof(NET_MSG_GENERIC);
		IOData->dwRcvBytes = 0;
		IOData->dwTotalBytes = dwRcvBytes;
		// Head receive mode
		m_pClientManager->SetNetMode( NET_RESERVED_SESSION, NET_PACKET_HEAD );

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		WSARecv(
			sSession,
			&(IOData->DataBuf), 
			1,
			&dwRcvBytes,
			&Flags ,
			&(IOData->Overlapped),
			NULL );
		
		SessionReqSvrInfo(); // 서버리스트를 요청한다.
		SessionSndSvrInfo(); // 서버정보를 전송한다.
		SessionSndKey();	 // 키값을 전송한다.

		CConsoleMessage::GetInstance()->Write(_T("============================"));
		CConsoleMessage::GetInstance()->Write(_T("Session Server Connection OK"));
		CConsoleMessage::GetInstance()->Write(_T("============================"));

		return NET_OK;
	}
}

void CLoginServer::SessionCloseConnect(void)
{	
	CloseClient( NET_RESERVED_SESSION );
}

// Session Server message process
void CLoginServer::SessionMsgProcess(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pMsg->Buffer;
	switch ( nmg->nType )
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ: // Session->Login:Heartbeat check request
		SessionMsgHeartbeatServerAns();
		break;
	case NET_MSG_HEARTBEAT_SERVER_ANS: // Session->Login:Heartbeat check answer
		m_pClientManager->SetHeartBeat( pMsg->dwClient );
		break;
	// 세션서버->로그인서버 : 게임서버의 상태정보를 전송한다.
	case NET_MSG_SND_SVR_INFO :
		SessionMsgSvrInfo(nmg);
		break;
	// 세션서버->로그인서버 : 게임서버의 전체정보를 요청한다.
	case NET_MSG_REQ_FULL_SVR_INFO :
        SessionSndSvrInfo();
		break;
	case NET_MSG_SND_SVR_INFO_RESET :
		SessionMsgSvrInfoReset(nmg);
		break;
	case NET_MSG_SVR_CMD:
		SessionMsgSvrCmd(nmg);
		break;
	// Channel Full 상태가 바뀌었을 때
	case NET_MSG_SND_CHANNEL_FULL_STATE:
		SessionSndSvrChannelFullInfo(nmg);
		break;

	case NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_LOGIN:
		SessionSndSvrLoginMaintenance( nmg );
		break;

	default:
		break;
	}
}

void CLoginServer::SessionMsgHeartbeatServerAns()
{
	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession( &MsgHeartbeatServerAns );
}

void CLoginServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	NET_SVR_CMD_CHANGE_STATE* pTemp = reinterpret_cast <NET_SVR_CMD_CHANGE_STATE*> (nmg);

	CConsoleMessage::GetInstance()->Write(_T("INFO:Received Server Command From Session Server"));

	switch (pTemp->nCmdType)
	{
	case NET_MSG_SVR_PAUSE: // Server pause message
		CConsoleMessage::GetInstance()->Write(_T("INFO:Server Will Pause"));
		Pause();
		break;
	case NET_MSG_SVR_RESUME: // Server resume message
		CConsoleMessage::GetInstance()->Write(_T("INFO:Server Will Resume"));
		Resume();
		break;
	case NET_MSG_SVR_RESTART: // Server restart message
		CConsoleMessage::GetInstance()->Write(_T("INFO:Server Will Restart"));
		Stop();
		Start();
		break;
	default:
		break;
	}
}

int CLoginServer::SendSession(
	LPVOID pBuffer )
{
	if (pBuffer == NULL) {
		return NET_ERROR;
	}
	else {
		return m_pClientManager->SendClient2( NET_RESERVED_SESSION, pBuffer );
		// return SendClient(NET_RESERVED_SESSION, pBuffer);
	}
}

void CLoginServer::SessionMsgSvrInfoReset(
	NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;

	int i, j;

	for (i=0; i<MAX_SERVER_GROUP; i++)
	{
		for (j=0; j<MAX_SERVER_NUMBER; j++)
		{
			SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
		}
	}
}

/*
void CLoginServer::SessionSndHeartbeat()
{
    NET_MSG_GENERIC nmg;
    nmg.dwSize = sizeof(NET_MSG_GENERIC);
    nmg.nType = NET_MSG_HEARTBEAT_SVR;
    SendSession(&nmg);
}
*/

// 세션서버에서 넘어온 게임서버 정보를 세팅한다.
void CLoginServer::SessionMsgSvrInfo(
	NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;
	
	NET_CUR_INFO_LOGIN* ncil;
	ncil = reinterpret_cast<NET_CUR_INFO_LOGIN*> (nmg);		
	if ((ncil->gscil.nServerGroup >= MAX_SERVER_GROUP) || (ncil->gscil.nServerNumber >= MAX_SERVER_NUMBER))
	{
		return;
	}
	else
	{
		LockOn();
		m_sGame[ncil->gscil.nServerGroup][ncil->gscil.nServerNumber] = ncil->gscil;
		CConsoleMessage::GetInstance()->WriteInfo(
			_T("SessionMsgSvrInfo %s"), 
			m_sGame[ncil->gscil.nServerGroup][ncil->gscil.nServerNumber].szServerIP );
		LockOff();
	}
}

void CLoginServer::SessionSndSvrChannelFullInfo(NET_MSG_GENERIC* nmg)
{
	NET_SERVER_CHANNEL_FULL_INFO* ncil = reinterpret_cast<NET_SERVER_CHANNEL_FULL_INFO*> (nmg);		
	
	LockOn();
	m_sGame[ncil->nServerGroup][ncil->nChannel].nServerCurrentClient = ncil->nChannelCurrentUser;

	if( ncil->bChannelFull )
	{
		CConsoleMessage::GetInstance()->Write( "Group %d Channel %d is Full", ncil->nServerGroup, ncil->nChannel   );
	}else{
		CConsoleMessage::GetInstance()->Write( "Group %d Channel %d is not Full", ncil->nServerGroup, ncil->nChannel   );
	}
	LockOff();
}

void CLoginServer::SessionSndSvrLoginMaintenance( NET_MSG_GENERIC* nmg )
{
	SNET_SERVER_LOGINMAINTENANCE_LOGIN* nsll = reinterpret_cast< SNET_SERVER_LOGINMAINTENANCE_LOGIN* > ( nmg );
	
    bool bOK = nsll->bOn;
	if( bOK )
	{
		SetAllowIP( false );
		CConsoleMessage::GetInstance()->Write(_T("LoginMaintenance AllowIP : False"));
	}
	else
	{
		SetAllowIP( true );
		CConsoleMessage::GetInstance()->Write(_T("LoginMaintenance AllowIP : True"));
	}
}

// Session Server 에 Game 서버의 현재상태를 요청한다.
void CLoginServer::SessionReqSvrInfo(void)
{
	NET_MSG_GENERIC nmg;
	nmg.dwSize = sizeof(NET_MSG_GENERIC);	
	nmg.nType = NET_MSG_REQ_SVR_INFO;
	SendSession(&nmg);
}

void CLoginServer::SessionSndKey(void)
{
	NET_ENCRYPT_KEY nmg;
	::StringCchCopy(nmg.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );
	SendSession(&nmg);	
}

void CLoginServer::SessionSndSvrInfo(void)
{
	NET_SERVER_INFO nsi;
	int nSize;
	
	nSize = sizeof(NET_SERVER_INFO);
	::memset(&nsi, 0, nSize);
	nsi.nmg.dwSize = nSize;
	nsi.nmg.nType = NET_MSG_SND_FULL_SVR_INFO;
	
	nsi.gsi.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
	nsi.gsi.nServerGroup			= m_nServerGroup;
	nsi.gsi.nServerChannel          = m_nServerChannel;
	nsi.gsi.nServerMaxClient		= m_pClientManager->GetMaxClient();	
	nsi.gsi.nServicePort			= m_nPort;
	nsi.gsi.nServerType				= SERVER_LOGIN;
	::StringCchCopy(nsi.gsi.szServerIP, MAX_IP_LENGTH+1, m_szAddress);

	SendSession(&nsi);
}

int CLoginServer::SessionConnectSndSvrInfo()
{
	return SessionConnect(
				CCfg::GetInstance()->GetSessionServerIP(),
		        CCfg::GetInstance()->GetSessionServerPort() );
}
//                              _-`.
//                           ,-"` |-`.
//                           |   | _-|
//                           |\  | _-|
//                          | \\|   |
//                          |  `| _-|
//                         |   | _-|
//                        |   |   |
//            ,===========;...'.-|
//          ,;||||||||||||||||\ |
//         /\""""""""/"\"`-.||||
//        //\\------/ /\\. `\||||
//       //  \       /  \ `. `.||
//      //  dP;     /   ,\  ` .\|.-.
//  ,-'' --^,'     /__  d#\   |;/   |
// |   _   Y          "-^;#\  `i   /
//  `. \"-.;      )               `.
//     \\ | `--""""""""""""`.      |
//      `:. |  |   |  |  |  |    _/
//        `:.  |   |  |  ' ,'_,-'
//          `:-._      _.-','
//            `--_"""""_.-'  
//                """""
