#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//! Session Server message process
void CAgentServer::SessionMsgProcess(MSG_LIST* pMsg)
{	
	if (pMsg == NULL) return;

	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);

	switch ( nmg->nType )
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ: // Session->Login:Heartbeat check request
		SessionMsgHeartbeatServerAns();
		break;
	case NET_MSG_HEARTBEAT_SERVER_ANS:
		m_pClientManager->SetHeartBeat( NET_RESERVED_SESSION );
		break;
	case NET_MSG_REQ_FULL_SVR_INFO :	
		SessionSndSvrInfo();
		break;
	case NET_MSG_REQ_CUR_STATE :
		SessionSndSvrCurState();
		break;
	case NET_MSG_CHAT :
		SessionMsgChatProcess(nmg);		
		break;
	case NET_MSG_RECONNECT_FIELD:
		CConsoleMessage::GetInstance()->Write(_T("INFO:Reconnect to field server"));
		FieldConnectAll();
		break;
	case NET_MSG_SVR_CMD:
		SessionMsgSvrCmd(nmg);
		break;	
	case NET_MSG_UPDATE_TRACING_ALL:		
		UpdateTracingCharacter( nmg );
		break;
	case NET_MSG_SND_ENCRYPT_KEY:
		SessionMsgEncryptKey( nmg );
		break;
	default:
		break;
	}
}

BOOL CAgentServer::UpdateTracingCharacter(NET_MSG_GENERIC* nmg )
{
	NET_UPDATE_TRACINGCHAR_ALL* pNetMsg = (NET_UPDATE_TRACINGCHAR_ALL*) nmg;
	if( pNetMsg == NULL ) return FALSE;

	size_t nPreSize = m_vecTracingData.size();

	m_vecTracingData.clear();
	int i, nTracing;
	for( i = 0; i < MAX_TRACING_NUM; i++ )
	{
		if( pNetMsg->tracingData[i].dwUserNum == 0 && pNetMsg->tracingData[i].strAccount.size() == 0 ) break;
		m_vecTracingData.push_back(pNetMsg->tracingData[i]);
	}
	size_t nPostSize = m_vecTracingData.size();

	// 추가 되었을 경우
	int iMAX_CLIENT = m_pClientManager->GetMaxClient();
	if( nPreSize < nPostSize )
	{	
		NET_UPDATE_TRACINGCHAR msg;		
		DWORD dwGaeaID;
		for (i = NET_RESERVED_SLOT; i < iMAX_CLIENT; ++i)
		{
			if( m_pClientManager->IsOnline(i) )
			{
				dwGaeaID = m_pClientManager->GetGaeaID(i);
				PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar(dwGaeaID);
				if( pChar != NULL && pChar->m_bTracingUser == FALSE )
				{
					nTracing = CheckTracingUser( pChar->m_szUserName, pChar->m_dwUserID );
					if( nTracing != -1 )
					{
						m_vecTracingData[nTracing].dwOnLine       = TRUE;
						m_vecTracingData[nTracing].dwUserNum      = pChar->m_dwUserID;
						m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
						m_vecTracingData[nTracing].strAccount     = pChar->m_szUserName;

						msg.tracingData = m_vecTracingData[nTracing];
						msg.updateNum   = nTracing;

						SendSession( &msg );

						pChar->m_bTracingUser = TRUE;

						CDebugSet::ToTracingFile( pChar->m_szUserName, "Tracing User Already Login..., [%s][%s], UserNum:[%u], IP:[%s]", 
					 							  pChar->m_szUserName, pChar->m_szName, pChar->m_dwUserID,pChar->m_szIp );

					}
				}
			}
		}		
		// 삭제 되었을 경우
	}else if( nPreSize > nPostSize )
	{
		DWORD dwGaeaID;
		for (i = NET_RESERVED_SLOT; i < iMAX_CLIENT; ++i)
		{
			if( m_pClientManager->IsOnline(i) )
			{
				dwGaeaID = m_pClientManager->GetGaeaID(i);
				PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar(dwGaeaID);
				if( pChar != NULL && pChar->m_bTracingUser == TRUE )
				{
					nTracing = CheckTracingUser( pChar->m_szUserName, pChar->m_dwUserID );
					if( nTracing == -1 )
					{
						pChar->m_bTracingUser = FALSE;
						CDebugSet::ToTracingFile( pChar->m_szUserName, "Delete Tracing User List, [%s][%s], UserNum:[%u], IP:[%s]", 
												  pChar->m_szUserName, pChar->m_szName, pChar->m_dwUserID,pChar->m_szIp );


					}
				}
			}
		}
	}

	

	return TRUE;
}

void CAgentServer::SessionMsgHeartbeatServerAns()
{
	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession( &MsgHeartbeatServerAns );
}

void CAgentServer::SessionMsgSvrCmd(NET_MSG_GENERIC* nmg)
{
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

void CAgentServer::SessionMsgEncryptKey(NET_MSG_GENERIC* pMsg)
{
	if (pMsg == NULL) return;

	NET_ENCRYPT_KEY* nmg = (NET_ENCRYPT_KEY*) pMsg;

	::StringCchCopy( m_szEncrypt, ENCRYPT_KEY+1, nmg->szEncryptKey );
}

int CAgentServer::SendSession(
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

// 세션서버에서 오는 채팅메시지 처리
void CAgentServer::SessionMsgChatProcess(NET_MSG_GENERIC* nmg)
{   
    if (nmg == NULL) return;
	NET_CHAT_CTRL* ncc;
	ncc = reinterpret_cast<NET_CHAT_CTRL*> (nmg);

	switch (ncc->emType) 
	{
	case CHAT_TYPE_GLOBAL: // 전체 공지 메시지
		CConsoleMessage::GetInstance()->Write(_T("Sending system message to all clients"));		
		MsgSndChatGlobal(ncc->szChatMsg);
		break;
	case CHAT_TYPE_NORMAL:
		break;
	case CHAT_TYPE_PARTY:
		break;
	case CHAT_TYPE_PRIVATE:
		break;
	case CHAT_TYPE_GUILD:
		break;
	case CHAT_TYPE_CTRL_GLOBAL:
		break;
	default:
		break;
	}	
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int CAgentServer::SessionConnectSndSvrInfo(void)
{
	return SessionConnectSndSvrInfo(
				CCfg::GetInstance()->GetSessionServerIP(),
		        CCfg::GetInstance()->GetSessionServerPort() );
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int CAgentServer::SessionConnectSndSvrInfo(
	const char* szServerIP,
	int nPort )
{
	if (SessionConnect(szServerIP, nPort) == NET_ERROR)
	{
		return NET_ERROR;
	}
	else
	{
		return SessionSndSvrInfo();
	}
}

// Agent Server -> Session Server 
// Send server info to session server
int CAgentServer::SessionSndSvrInfo(void)
{	
	NET_SERVER_INFO nsi;
	nsi.nmg.nType					= NET_MSG_SND_FULL_SVR_INFO;
	nsi.gsi.nControlPort			= CCfg::GetInstance()->GetControlPort();
	nsi.gsi.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
	nsi.gsi.nServerGroup			= CCfg::GetInstance()->GetServerGroup();
	nsi.gsi.nServerMaxClient		= CCfg::GetInstance()->GetServerMaxClient();
	nsi.gsi.nServerNumber			= CCfg::GetInstance()->GetServerNumber();
	nsi.gsi.nServerType				= CCfg::GetInstance()->GetServerType();	
	nsi.gsi.nServicePort			= CCfg::GetInstance()->GetServicePort();
	nsi.gsi.nServerChannel          = CCfg::GetInstance()->GetServerChannel();
	nsi.gsi.nServerChannelNumber    = CCfg::GetInstance()->GetServerChannelNumber();
	nsi.gsi.nServerChannellMaxClient= CCfg::GetInstance()->GetServerChannelMaxClient();
	StringCchCopy( nsi.gsi.szServerIP, MAX_IP_LENGTH+1, m_szAddress );

	CConsoleMessage::GetInstance()->Write(
		_T("SessionSndSvrInfo %s CH:%d"), 
		m_szAddress,
		nsi.gsi.nServerChannelNumber );
	
	if (SendSession(&nsi) == NET_ERROR)
	{
		return NET_ERROR;
	}
	else
	{
		for (int i=0; i<CCfg::GetInstance()->GetServerChannelNumber(); ++i)
		{
			SessionSndChannelState(i);
		}
		return NET_OK;
	}
}

// Agent Server -> Session Server
int CAgentServer::SessionSndSvrCurState()
{	
	NET_SERVER_CUR_INFO nsci;

	// 로그인서버, 게임서버 -> 세션서버 : 현재 서버상태(간략한)를 전송한다.
	// 현재접속자, 최대접속자
	nsci.nmg.nType					= NET_MSG_SND_CUR_STATE;
	nsci.gsci.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
	nsci.gsci.nServerMaxClient		= m_nMaxClient;
	
	return SendSession(&nsci);
}

int CAgentServer::SessionSndChannelState(int nChannel)
{
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		return NET_ERROR;
	}

	NET_SERVER_CHANNEL_INFO nsci;
	nsci.nChannel            = nChannel;
	nsci.nChannelCurrentUser = m_ChannelUser[nChannel];
	nsci.nChannelMaxUser     = m_nMaxChannelUser;
	nsci.nServerGroup        = m_nServerGroup;
	nsci.bPk                 = CCfg::GetInstance()->IsPkChannel(nChannel);

	return SendSession(&nsci);
}

int	CAgentServer::SessionConnect(
	const char* szServerIP,
	int nPort )
{
	if ( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR::CAgentServer::SessionConnect IsOnline true") );
		return NET_ERROR;
	}
	else
	{		
		if ( SessionConnect(::inet_addr(szServerIP), nPort) == NET_OK )
		{		
			CConsoleMessage::GetInstance()->Write(_T("============================"));
			CConsoleMessage::GetInstance()->Write(_T("Session Server Connection OK"));
			CConsoleMessage::GetInstance()->Write(_T("============================"));
			return NET_OK;
		}
		else
		{
			CConsoleMessage::GetInstance()->Write(_T("================================"));
			CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
			CConsoleMessage::GetInstance()->Write(_T("================================"));
			return NET_ERROR;
		}
	}
}

int CAgentServer::SessionConnect(
	unsigned long lServerIP,
	int nPort )
{
	if ( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR::Session server is already connected") );
		return NET_ERROR;
	}

	int nRetCode = 0;

	SOCKET sSession = WSASocket(
						PF_INET,
						SOCK_STREAM,
						IPPROTO_TCP,
						NULL,
						0,
						WSA_FLAG_OVERLAPPED );
	if ( sSession == INVALID_SOCKET )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CAgentServer::SessionConnect WSASocket sSession==INVALID_SOCKET %d"),
			WSAGetLastError() );
		return NET_ERROR;
	}

	SOCKADDR_IN Addr;
	::SecureZeroMemory( &Addr, sizeof(SOCKADDR_IN) );
	Addr.sin_family      = AF_INET;
	Addr.sin_port        = ::htons( (u_short) nPort );
	Addr.sin_addr.s_addr = lServerIP;

	// Connect to server
	nRetCode = ::connect(
					sSession,
					(sockaddr *) &Addr,
					sizeof(SOCKADDR_IN) );
	if ( nRetCode == SOCKET_ERROR )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CAgentServer::SessionConnect connect %d"),
			WSAGetLastError() );
		::closesocket(sSession);
		return NET_ERROR;
	}

	//m_dwSession++;
	//if (m_dwSession >= NET_SESSION_END)
	//{
	//	m_dwSession = NET_SESSION_START;
	//}
	
	// If type is session, all slots are itself.
	m_pClientManager->SetAcceptedClient ( NET_RESERVED_SESSION, sSession );
	m_pClientManager->SetSlotType       ( NET_RESERVED_SESSION, NET_SLOT_SESSION );
	m_pClientManager->SetSlotAgentField ( NET_RESERVED_SESSION, NET_RESERVED_SESSION );
	m_pClientManager->SetSlotAgentClient( NET_RESERVED_SESSION, NET_RESERVED_SESSION );

	// Associate the accepted socket with the completion port
	
	LPPER_IO_OPERATION_DATA IOData = NULL;
	DWORD dwRcvBytes = 0;
	DWORD Flags = 0;
	HANDLE hRetCode = NULL;

	hRetCode = ::CreateIoCompletionPort(
					(HANDLE) sSession,
					m_hIOServer,
					(DWORD) NET_RESERVED_SESSION,
					0 );

	if (hRetCode == NULL)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentServer::SessionConnect CreateIoCompletionPort Error") );
		m_pClientManager->CloseClient( NET_RESERVED_SESSION );
		return NET_ERROR;
	}

	//IOData = getRecvIO( m_dwSession );
	IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

	if (IOData == NULL) 
	{
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentServer::SessionConnect IOData==NULL"));
		m_pClientManager->CloseClient( NET_RESERVED_SESSION );
		return NET_ERROR;
	}
	else
	{
		dwRcvBytes = sizeof(NET_MSG_GENERIC);
		IOData->dwRcvBytes = 0;
		IOData->dwTotalBytes = dwRcvBytes;
		
		m_pClientManager->SetNetMode( NET_RESERVED_SESSION, NET_PACKET_HEAD );

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		::WSARecv(
			sSession,
			&(IOData->DataBuf), 
			1,
			&dwRcvBytes,
			&Flags ,
			&(IOData->Overlapped),
			NULL );
		return NET_OK;
	}
}

int CAgentServer::SessionCloseConnect(void)
{

	if ( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
	{
		return m_pClientManager->CloseClient( NET_RESERVED_SESSION );
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(
			_T("INFO:CAgentServer::SessionCloseConnect SessionCloseConnect already offline") );
		return NET_OK;
	}
}

/**
* Session 서버와 통신을 체크하고 이상이 있으면 다시 연결한다.
*/
int CAgentServer::SessionReconnect()
{
	if ( false ==  m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
	{
		return SessionConnectSndSvrInfo();
	}
	else
	{
		return NET_OK;
	}
}

//                        ,     
//                   ,   /^\     ___
//                  /^\_/   `...'  /`
//               ,__\    ,'     ~ (
//            ,___\ ,,    .,       \
//             \___ \\\ .'.'   .-.  )
//               .'.-\\\`.`.  '.-. (
//              / (==== ."".  ( o ) \
//            ,/u  `~~~'|  /   `-'   )
//           "")^u ^u^|~| `""".  ~_ /
//             /^u ^u ^\~\     ".  \\
//     _      /u^  u ^u  ~\      ". \\
//    ( \     )^ ^U ^U ^U\~\      ". \\
//   (_ (\   /^U ^ ^U ^U  ~|       ". `\
//  (_  _ \  )U ^ U^ ^U ^|~|        ". `\.
// (_  = _(\ \^ U ^U ^ U^ ~|          ".`.;
//(_ -(    _\_)U ^ ^ U^ ^|~|            ""
//(_    =   ( ^ U^ U^ ^ U ~|
//(_ -  ( ~  = ^ U ^U U ^|~/
// (_  =     (_^U^ ^ U^ U /
//  (_-   ~_(/ \^ U^ ^U^,"
//   (_ =  _/   |^ u^u."  
//    (_  (/    |u^ u.(   
//     (__/     )^u^ u/ 
//             /u^ u^(  
//            |^ u^ u/   
//            |u^ u^(       ____   
//            |^u^ u(    .-'    `-,
//             \^u ^ \  / ' .---.  \
//              \^ u^u\ |  '  `  ;  |
//               \u^u^u:` . `-'  ;  |
//                `-.^ u`._   _.'^'./
//                   "-.^.-```_=~._/
//                      `"------"'
// jgkim