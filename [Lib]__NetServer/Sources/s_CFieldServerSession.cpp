#include "pch.h"
#include "s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Session Server Communication Member Function
// If not use Session Server, this functions are unnecessary
///////////////////////////////////////////////////////////////////////////////

// Session Server message process
void CFieldServer::SessionMsgProcess(
	MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pMsg->Buffer;

	if (nmg == NULL) return;

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
	case NET_MSG_LOGIN_FB :
		SessionMsgLoginFeedBack(nmg);
		break;
	case NET_MSG_CHAT :
		SessionMsgChatProcess(nmg);		
		break;
	case NET_MSG_SVR_CMD :
		SessionMsgSvrCmd(nmg);
		break;	
	case NET_MSG_UPDATE_TRACING_ALL:		
		UpdateTracingCharacter( nmg );
		break;
	default:
		break;
	}
}

BOOL CFieldServer::UpdateTracingCharacter(NET_MSG_GENERIC* nmg )
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
				PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar(dwGaeaID);
				if( pChar != NULL && pChar->m_bTracingUser == FALSE )
				{
					nTracing = CheckTracingUser( pChar->m_szUID, pChar->GetUserID() );
					if( nTracing != -1 )
					{
						m_vecTracingData[nTracing].dwOnLine       = TRUE;
						m_vecTracingData[nTracing].dwUserNum      = pChar->GetUserID();
						m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
						m_vecTracingData[nTracing].strAccount     = pChar->m_szUID;
					
						pChar->m_bTracingUser = TRUE;

						//	Note : 클라이언트에 Tracing 변화 메시지를 보낸다.
						//  1이면 TRUE
						NET_UPDATE_TRACINGCHAR NetMsgNew;
						NetMsgNew.updateNum = 1;
						GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsgNew);

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
				PGLCHAR pChar = GLGaeaServer::GetInstance().GetChar(dwGaeaID);
				if( pChar != NULL && pChar->m_bTracingUser == TRUE )
				{
					nTracing = CheckTracingUser( pChar->m_szUID, pChar->GetUserID() );
					if( nTracing == -1 )
					{
						pChar->m_bTracingUser = FALSE;

						//	Note : 클라이언트에 Tracing 변화 메시지를 보낸다.
						//  0이면 FALSE
						NET_UPDATE_TRACINGCHAR NetMsgNew;
						NetMsgNew.updateNum = 0;
						GLGaeaServer::GetInstance().SENDTOCLIENT(pChar->m_dwClientID,&NetMsgNew);
					}
				}
			}
		}
	}



	return TRUE;
}

void CFieldServer::SessionMsgHeartbeatServerAns()
{
	NET_HEARTBEAT_SERVER_ANS MsgHeartbeatServerAns;
    SendSession( &MsgHeartbeatServerAns );
}

void CFieldServer::SessionMsgSvrCmd(
	NET_MSG_GENERIC* nmg )
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

int CFieldServer::SendSession(
	LPVOID pBuffer )
{
    if (pBuffer == NULL) {
        CConsoleMessage::GetInstance()->Write(_T("SendSession pBuffer == NULL"));
		return NET_ERROR;
	} else {
		return m_pClientManager->SendClient2( NET_RESERVED_SESSION, pBuffer );
	}
}

// 세션서버에서 오는 채팅메시지 처리
void CFieldServer::SessionMsgChatProcess(
	NET_MSG_GENERIC* nmg )
{
    if (nmg == NULL) return;

	NET_CHAT* nc = (NET_CHAT*) nmg;

	switch (nc->emType) 
	{
	case CHAT_TYPE_GLOBAL: // 전체 공지 메시지
		CConsoleMessage::GetInstance()->Write(_T("Sending system message to all users"));
		MsgSndChatGlobal(nc->szChatMsg);
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

// 세션서버 -> 게임서버 : 로그인결과 데이타
void CFieldServer::SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{	
    if (nmg == NULL) return;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	NET_LOGIN_FEEDBACK_DATA2*	nlfd2;
	
	DWORD	dwClient;	
	DWORD	dwSndBytes = 0;
	CHAR	szUserid[USR_ID_LENGTH+1] = {0};
	INT		nUserNum = 0;

	// 서버에서 넘어온 로그인 결과 메시지
	nlfd2 = (NET_LOGIN_FEEDBACK_DATA2*) nmg;
	dwClient = (DWORD) nlfd2->nClient;
	
	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	dwSndBytes = sizeof(NET_LOGIN_FEEDBACK_DATA2);
	nlfd.nmg.dwSize = dwSndBytes;
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	// 온라인이고, ip, id 가 동일할때...
	if ((m_pClientManager->IsOnline(dwClient) == true) &&
		(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
		(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0))
	{
		nlfd.nResult = nlfd2->nResult;
		// 로그인이 정상적으로 되었으면 사용자 번호를 세팅한다.
		if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
		{
			::StringCchCopy(szUserid, USR_ID_LENGTH+1, m_pClientManager->GetUserID(dwClient));
			if (nlfd2->nUserNum < 1)
			{
				CConsoleMessage::GetInstance()->Write(LOG_CONSOLE, _T("SessionMsgLoginFeedBack GetUserNum Failed"));
				return ;
			}
			m_pClientManager->SetUserNum(dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime(dwClient);			
		}
	}
	else
	{
		nlfd.nResult = EM_LOGIN_FB_SUB_FAIL;
	}
	// 로그인 결과 전송	
	SendClient(dwClient, &nlfd);
}

// Field Server -> Session Server 
// Send server info to session server
int CFieldServer::SessionSndSvrInfo()
{
	int nSize;
	NET_SERVER_INFO nsi;
	
	nSize = sizeof(NET_SERVER_INFO);
	::memset(&nsi, 0, nSize);
	
	nsi.nmg.dwSize					= nSize;
	nsi.nmg.nType					= NET_MSG_SND_FULL_SVR_INFO;
	nsi.gsi.nControlPort			= CCfg::GetInstance()->GetControlPort();
	nsi.gsi.nServerCurrentClient	= m_pClientManager->GetCurrentClientNumber();
	nsi.gsi.nServerGroup			= CCfg::GetInstance()->GetServerGroup();
	nsi.gsi.nServerMaxClient		= CCfg::GetInstance()->GetServerMaxClient();
	nsi.gsi.nServerNumber			= CCfg::GetInstance()->GetServerNumber();
	nsi.gsi.nServerType				= CCfg::GetInstance()->GetServerType();
	nsi.gsi.nServerChannel          = CCfg::GetInstance()->GetServerChannel();
	nsi.gsi.nServicePort			= CCfg::GetInstance()->GetServicePort();	
	// ::memcpy(nsi.gsi.szServerIP, m_szAddress, MAX_IP_LENGTH);
	StringCchCopy(nsi.gsi.szServerIP, MAX_IP_LENGTH+1, m_szAddress);
	
	if (SendSession(&nsi) == NET_ERROR)
	{
        CConsoleMessage::GetInstance()->Write(LOG_CONSOLE, _T("ERROR:SessionSndSvrInfo NET_ERROR"));
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(LOG_CONSOLE, _T("INFO:SessionSndSvrInfo %s"), m_szAddress);
		return NET_OK;
	}
}

// Game Server -> Session Server
int CFieldServer::SessionSndSvrCurState()
{	
	// Field->Session:현재 서버상태(간략한)를 전송한다.
	// 현재접속자, 최대접속자
	NET_SERVER_CUR_INFO nsci;
	nsci.nmg.nType				   = NET_MSG_SND_CUR_STATE;	
	nsci.gsci.nServerCurrentClient = (int) GLGaeaServer::GetInstance().GetNumPC();
	nsci.gsci.nServerMaxClient	   = CCfg::GetInstance()->GetServerMaxClient();
	
	return SendSession(&nsci);
}

/*
// Game Server -> Session Server : 사용자 정보 전송
// 사용자 인증을 기다린다.
int CFieldServer::SessionSndLoginInfo(const char* szUsrID, const char* szUsrPass, int nClient)
{
    if (szUsrID == NULL || szUsrPass == NULL) return NET_ERROR;

	NET_LOGIN_DATA2 nld2;
	nld2.nmg.nType	  = NET_MSG_LOGIN_2;
	nld2.nClient	  = nClient;
	StringCchCopy(nld2.szUserid,   USR_ID_LENGTH+1,   szUsrID);
	StringCchCopy(nld2.szPassword, USR_PASS_LENGTH+1, szUsrPass);
	StringCchCopy(nld2.szIp,	   MAX_IP_LENGTH+1,   m_pClientManager->GetClientIP(nClient));
	SendSession(&nld2);

	// 사용자 정보 세팅
	m_pClientManager->SetUserID(nClient, nld2.szUserid);	

	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, 
                           				  _T("SessionSndLoginInfo %s(%s)"),
				                          nld2.szUserid,
				                          nld2.szIp);


	return 0;
}
*/

void CFieldServer::SessionSndLogOutInfoSimple(const char* szUsrID)
{
    if (szUsrID == NULL) return;

	NET_LOGOUT_DATA_SIMPLE    nlods;
	nlods.nmg.nType	= NET_MSG_LOGOUT_G_S_S;	
	StringCchCopy(nlods.szUserID, USR_ID_LENGTH+1, szUsrID);
	SendSession(&nlods);
}

void CFieldServer::SessionSndLogOutInfo(const char* szUsrID, int nUserNum, int nGameTime, int nChaNum)
{	
    if (szUsrID == NULL) return;
	
	NET_LOGOUT_DATA nlod;
	nlod.nmg.nType	= NET_MSG_LOGOUT_G_S;	
	nlod.nUserNum   = nUserNum;
	nlod.nGameTime  = nGameTime;
	nlod.nChaNum    = nChaNum;
	StringCchCopy( nlod.szUserID, USR_ID_LENGTH+1, szUsrID );

	SendSession(&nlod);
}

int CFieldServer::SessionConnectSndSvrInfo(void)
{
	return SessionConnectSndSvrInfo(
				CCfg::GetInstance()->GetSessionServerIP(),
		        CCfg::GetInstance()->GetSessionServerPort() );
}

//! Session 서버에 접속하고 서버의 정보를 전송한다.
int CFieldServer::SessionConnectSndSvrInfo(
	const char* szServerIP,
	int nPort )
{
	if ( SessionConnect( szServerIP, nPort ) == NET_ERROR )
	{
		return NET_ERROR;
	}
	else
	{
		return SessionSndSvrInfo();
	}
}

int	CFieldServer::SessionConnect(
	const char* szServerIP,
	int nPort )
{
	if ( true == m_pClientManager->IsOnline(NET_RESERVED_SESSION) )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("Session Already Connected") );
		return NET_OK;
		// m_pClientManager->CloseClient( NET_RESERVED_SESSION );
	}

	hostent* hst;
	char strRet[MAX_IP_LENGTH+1] = {0};
	unsigned long ulIP;
	struct in_addr inetAddr;

	hst = ::gethostbyname( szServerIP );
	if ( hst == NULL ) // ERROR
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CFieldServer::SessionConnect gethostbyname hst==NULL") );
		return NET_ERROR;
	}
	ulIP = *(DWORD*)(*hst->h_addr_list);
	inetAddr.s_addr = ulIP;
	::StringCchCopy(strRet, MAX_IP_LENGTH+1, ::inet_ntoa(inetAddr));
	if (SessionConnect(::inet_addr(strRet), nPort) == NET_OK)
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

int CFieldServer::SessionConnect(
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
	if ( sSession == INVALID_SOCKET )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:CFieldServer::SessionConnect WSASocket %d"),
			WSAGetLastError() );
		return NET_ERROR;
	}
	
	SOCKADDR_IN Addr;
	::SecureZeroMemory( &Addr, sizeof(SOCKADDR_IN) );
	Addr.sin_family      = AF_INET;
	Addr.sin_port        = ::htons( (u_short) nPort );
	Addr.sin_addr.s_addr = ulServerIP;

	// Connect to server
	nRetCode = ::connect(
					sSession,
					(sockaddr *) &Addr,
					sizeof(SOCKADDR_IN) );
	if ( nRetCode == SOCKET_ERROR )
	{
		CConsoleMessage::GetInstance()->Write(
			_T("CFieldServer::SessionConnect connect %d"),
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
	m_pClientManager->SetSlotFieldAgent ( NET_RESERVED_SESSION, NET_RESERVED_SESSION );
	m_pClientManager->SetSlotFieldClient( NET_RESERVED_SESSION, NET_RESERVED_SESSION );

	// Associate the accepted socket with the completion port	
	LPPER_IO_OPERATION_DATA IOData = NULL;
	DWORD dwRcvBytes = 0;
	DWORD Flags = 0;
	HANDLE hRetCode = NULL;	

	hRetCode = ::CreateIoCompletionPort(
					(HANDLE) sSession, 
					m_hIOServer, 
					(DWORD)NET_RESERVED_SESSION,
					0);
	if (hRetCode == NULL)
	{
		CConsoleMessage::GetInstance()->Write( _T("CreateIoCompletionPort Error") );
		m_pClientManager->CloseClient( NET_RESERVED_SESSION );
		return NET_ERROR;
	}

	//IOData = getRecvIO( m_dwSession );
	IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

	if ( IOData == NULL )
	{
		m_pClientManager->CloseClient( NET_RESERVED_SESSION );
		return NET_ERROR;
	}
	else
	{
		dwRcvBytes = sizeof(NET_MSG_GENERIC);
		IOData->dwRcvBytes		= 0;
		IOData->dwTotalBytes	= dwRcvBytes;
		// Head receive mode
		m_pClientManager->SetNetMode( NET_RESERVED_SESSION, NET_PACKET_HEAD );

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		::WSARecv(
			sSession,
			&(IOData->DataBuf),
			1,
			&dwRcvBytes,
			&Flags,
			&(IOData->Overlapped),
			NULL );
		return NET_OK;
	}
}