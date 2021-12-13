///////////////////////////////////////////////////////////////////////////////
// s_CSessionServerMsg.cpp
//
// * History
// 2002.05.30 jgkim Create
// 2003.10.20 jgkim 
//
// Copyright 2002-2006 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CSessionServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CSessionServer::MsgProcess(MSG_LIST* pMsg)
{	
    if (pMsg == NULL) return NET_ERROR;

	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);

	switch (nmg->nType)
	{
	case NET_MSG_HEARTBEAT_SERVER_ANS: // All server->Session:Heartbeat Answer
		m_pClientManager->SetHeartBeat( pMsg->dwClient );
		break;
	// Agent, Filed, Login->Session:Hearbeat Requst
    case NET_MSG_HEARTBEAT_SERVER_REQ:
        MsgServerHeartbeatAnswer( pMsg );
        break;
	// Login, Agent, Field->Session : 서버 전체 정보를 Session 서버에 전송한다.
	case NET_MSG_SND_FULL_SVR_INFO :
		MsgServerInfo(pMsg);
		break;
	// Server Manager -> Session : 전체 서버 정보 요청
	case NET_MSG_REQ_ALL_SVR_INFO :
		MsgSndAllSvrInfo(pMsg);
		break;
	// Login->Session : 게임서버의 상태정보를 요청한다.
	case NET_MSG_REQ_SVR_INFO :
		MsgSndCurSvrInfo(pMsg->dwClient);
		break;
	// Login, Agent, Field->Session : 현재 서버상태(간략한)를 전송한다.
	case NET_MSG_SND_CUR_STATE :
		MsgServerCurInfo(pMsg);
		break;
	// Agent->Session : 채널의 유저정보
	case NET_MSG_SND_CHANNEL_STATE :
		MsgServerChannelInfo(pMsg);
		break;
	// Agent->Session : 캐릭터가 게임에 조인했음 
	case NET_MSG_GAME_JOIN_OK :
		MsgGameJoinOK(pMsg);
		break;
	// 채팅메시지 처리
	case NET_MSG_CHAT : 
		MsgChatProcess(pMsg);
		break;
	case NET_MSG_SVR_CMD :
		MsgSvrCmdProcess(pMsg);
		break;
	case NET_MSG_SND_ENCRYPT_KEY :
		MsgEncryptKey(pMsg);
		break;

	// Agent->Session LoginMaintenance On/Off
	case NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_SESSION :
		MsgLoginMaintenance( pMsg );
		break;

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	case NET_MSG_UPDATE_TRACING_ALL:
		MsgTracingChar(pMsg,TRUE);
		break;
	case NET_MSG_UPDATE_TRACING_CHARACTER:
		MsgTracingChar(pMsg,FALSE);
		break;
#endif
	default:
		break;
	}
	return 0;
}

void CSessionServer::MsgServerHeartbeatAnswer(
	MSG_LIST* pMsg )
{
	DWORD dwClient = pMsg->dwClient;
	NET_HEARTBEAT_SERVER_ANS msg;
	SendClient( dwClient, &msg );
}

int CSessionServer::FindServer(CString strIP, int nPort)
{
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{		
		if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
		    (strcmp(strIP, m_pClientManager->GetClientIP((DWORD)i)) == 0) &&
			(nPort == m_pClientManager->GetSvrServicePort((DWORD)i)))
		{					
			return i;
		}
	}
	return NET_ERROR;
}

int CSessionServer::FindServer(int nSvrType, int nSvrGrp, int nSvrNum, int nFieldNum)
{
	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		if (nSvrType == NET_SERVER_FIELD)
		{
			if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
				(m_pClientManager->GetSvrGrp((DWORD) i) == nSvrGrp) &&
				(m_pClientManager->GetSvrNum((DWORD) i) == nSvrNum) &&
				(m_pClientManager->GetSvrField((DWORD) i) == nFieldNum))
			{
				CConsoleMessage::GetInstance()->Write(_T("Find Server %d"), i);
				return i;
			}
		}
		else
		{
			if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
				(m_pClientManager->GetSvrGrp((DWORD) i) == nSvrGrp) &&
				(m_pClientManager->GetSvrNum((DWORD) i) == nSvrNum) &&
				(m_pClientManager->GetSvrType((DWORD) i) == nSvrType))
			{
				CConsoleMessage::GetInstance()->Write(_T("Find Server %d"), i);
				return i;
			}
		}
	}
	CConsoleMessage::GetInstance()->Write(_T("Can't Find Server"));
	return NET_ERROR;
}

int CSessionServer::MsgSndServerStateChange(int nClient, int nState)
{
	NET_SVR_CMD_CHANGE_STATE sTemp;
	sTemp.nCmdType = nState;
	return SendClient(static_cast <DWORD> (nClient), static_cast <LPVOID> (&sTemp));
}

void CSessionServer::MsgTracingChar(MSG_LIST* pMsg, bool bAll )
{
	if( pMsg == NULL ) return;

	if( bAll )
	{
		m_vecTracingData.clear();
		NET_UPDATE_TRACINGCHAR_ALL* msg = reinterpret_cast <NET_UPDATE_TRACINGCHAR_ALL*> (pMsg->Buffer);
		BYTE i;
		for( i = 0; i < MAX_TRACING_NUM; i++ )
		{
			if( msg->tracingData[i].dwUserNum == 0 && msg->tracingData[i].strAccount.size() == 0 ) break;
			m_vecTracingData.push_back(msg->tracingData[i]);
		}

		// 전 서버에 현재 추가된 캐릭터들에 대한 정보를 보낸다.
		int nMaxClient = m_pClientManager->GetMaxClient();
		for (int i=0; i<nMaxClient; ++i)
		{
			if( i == pMsg->dwClient ) continue;
			// 온라인이고 게임서버라면 채팅메시지를 보낸다.
			if( m_pClientManager->IsOnline((DWORD) i) == true  )
			{
				SendClient((DWORD) i, msg);
			}
		}

	}else{
		NET_UPDATE_TRACINGCHAR* msg = reinterpret_cast <NET_UPDATE_TRACINGCHAR*> (pMsg->Buffer);

		m_vecTracingData[msg->updateNum] = msg->tracingData;

		// 전 서버에 현재 추가된 캐릭터들에 대한 정보를 보낸다.
		int nMaxClient = m_pClientManager->GetMaxClient();
		for (int i=0; i<nMaxClient; ++i)
		{
			// 온라인이고 게임서버라면 채팅메시지를 보낸다.
			if( m_pClientManager->IsOnline((DWORD) i) == true &&
				m_pClientManager->GetSvrType(i) != SERVER_AGENT &&
				m_pClientManager->GetSvrType(i) != SERVER_FIELD  )
			{
				SendClient((DWORD) i, msg);
			}
		}
	}

	

}



void CSessionServer::MsgChatProcess(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_CHAT* nc = (NET_CHAT*) pMsg->Buffer;

	CConsoleMessage::GetInstance()->WriteConsole(_T("Chat message"));

	switch (nc->emType) 
	{
	case CHAT_TYPE_NORMAL: // 일반적 채팅 메시지
		break;
	case CHAT_TYPE_PARTY: // 파티원에게 전달되는 메시지
		break;
	case CHAT_TYPE_PRIVATE: // 개인적인 메시지, 귓말, 속삭임
		break;
	case CHAT_TYPE_GUILD: // 길드원에게 전달되는 메시지
		break;
	// 관리프로그램에서 오는 전체메시지
	case CHAT_TYPE_CTRL_GLOBAL2: 
		MsgSndChatCtrlGlobal2(pMsg);
		break;
	case CHAT_TYPE_GLOBAL:		
	case CHAT_TYPE_CTRL_GLOBAL:		
		MsgSndChatCtrlGlobal(pMsg);
		break;
	default:
		break;
	}	

	int test;
	if( nc->emType == CHAT_TYPE_GLOBAL  )
	{
		test = 0;
	}
}

// 관리프로그램에서 오는 전체 공지 채팅 메시지를 해당서버로 전송한다
void CSessionServer::MsgSndChatCtrlGlobal(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	CConsoleMessage::GetInstance()->Write(_T("Notice Message"));

	NET_CHAT_CTRL* ncc = reinterpret_cast<NET_CHAT_CTRL*> (pMsg->Buffer);    

	// Find server	
	DWORD dwClient = m_pClientManager->FindServer(ncc->szServerIP, ncc->nServicePort);
	if (dwClient == NET_ERROR) // 해당 서버를 찾지 못함
	{
        CConsoleMessage::GetInstance()->Write("ERROR:Can't Find Server");
		return;
	}

	// Send Global message
	NET_CHAT_CTRL nc;
	nc.nmg.nType = NET_MSG_CHAT;
	nc.emType    = CHAT_TYPE_GLOBAL;
	::StringCchCopy(nc.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, ncc->szChatMsg);
	SendClient(dwClient , &nc);

	CConsoleMessage::GetInstance()->Write(nc.szChatMsg);    
}

// 관리프로그램에서 오는 전체 공지 채팅 메시지를 해당서버로 전송한다
void CSessionServer::MsgSndChatCtrlGlobal2(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

    CConsoleMessage::GetInstance()->Write(_T("Notice Message"));

	NET_CHAT_CTRL2* ncc = reinterpret_cast <NET_CHAT_CTRL2*> (pMsg->Buffer);
	
    // Find server	
	DWORD dwClient = m_pClientManager->FindServer(NET_SERVER_AGENT, ncc->nSvrGrp, 0, 0);	
	if (dwClient == NET_ERROR) // 해당 서버를 찾지 못함
	{
		CConsoleMessage::GetInstance()->Write(_T("INFO:Can't Find Server"));
		return;
	}
	
	// Send Global message
	NET_CHAT_CTRL nc;
	nc.nmg.nType = NET_MSG_CHAT;
	nc.emType    = CHAT_TYPE_GLOBAL;
	::StringCchCopy(nc.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, ncc->szChatMsg);
	SendClient(dwClient , &nc);

	CConsoleMessage::GetInstance()->Write(nc.szChatMsg);    
}

void CSessionServer::MsgSndSvrInfoReset(DWORD dwClient)
{		
	NET_MSG_GENERIC	nmg;	
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_SND_SVR_INFO_RESET;
	SendClient(dwClient, &nmg);
}

/// 세션서버->로그인서버 : 게임서버의 상태정보를 전송한다.
void CSessionServer::MsgSndCurSvrInfo(DWORD dwClient)
{
	// server type [type]
	// [type]
	// 1 : login server
	// 2 : session server
	// 3 : field server
	// 4 : agent server

	// LoginServer의 ClientID를 셋팅한다.
	m_dwLoginServerID = dwClient;

	NET_CUR_INFO_LOGIN ncil;
	ncil.nmg.nType = NET_MSG_SND_SVR_INFO;

	for (int nGroup=0; nGroup < MAX_SERVER_GROUP; ++nGroup)
	{
		for (int nChannel=0; nChannel < MAX_CHANNEL_NUMBER; ++nChannel)
		{
			if (m_sServerChannel[nGroup][nChannel].nServerMaxClient > 0)
			{
				ncil.gscil               = m_sServerChannel[nGroup][nChannel];
				ncil.gscil.nServerNumber = nChannel; // Channel number

				SendClient(dwClient, &ncil);

				CConsoleMessage::GetInstance()->WriteConsole(
					_T("MsgSndCurSvrInfo %s"),
					m_sServerChannel[nGroup][nChannel].szServerIP );
			}
		}
	}
}

void CSessionServer::MsgSndChatGlobal(char* szChatMsg)
{
    if (szChatMsg == NULL) return;

	NET_CHAT nc;	
	nc.nmg.nType	= NET_MSG_CHAT;
	nc.emType		= CHAT_TYPE_GLOBAL;
	::StringCchCopy(nc.szChatMsg, CHAT_MSG_SIZE+1, szChatMsg);

	int nMaxClient = m_pClientManager->GetMaxClient();

	for (int i=0; i<nMaxClient; ++i)
	{
		// 온라인이고 게임서버라면 채팅메시지를 보낸다.
		if ((m_pClientManager->IsOnline((DWORD) i) == true) && 
			(m_pClientManager->IsGameServer((DWORD) i) == true)) 
		{
			SendClient((DWORD) i, &nc);
		}
	}
}

// Session->ServerManager:온라인 상태인 모든 서버의 상태정보를 전송해 준다
void CSessionServer::MsgSndAllSvrInfo(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	int nMax = m_pClientManager->GetMaxClient();

	// Start of server info
	NET_MSG_GENERIC nmg;
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_SND_ALL_SVR_INFO_S;
	SendClient(dwClient, &nmg);
	
	// server info
	NET_SERVER_INFO nsi;
	int nSize = sizeof(NET_SERVER_INFO);

	for (DWORD dwSvr=0; dwSvr<(DWORD) nMax; ++dwSvr)
	{
		if (m_pClientManager->IsOnline(dwSvr) == true)
		{	
			memset(&nsi, 0, nSize);
			nsi.nmg.dwSize	= nSize;
			nsi.nmg.nType	= NET_MSG_SND_ALL_SVR_INFO;
			nsi.gsi			= m_pClientManager->GetSvrInfo(dwSvr);
			SendClient(dwClient, &nsi);

			CConsoleMessage::GetInstance()->WriteConsole(
				_T("MsgSndAllSvrInfo %s %d %d"),
				nsi.gsi.szServerIP,
				nsi.gsi.nControlPort,
				nsi.gsi.nServerType );
		}
	}

	// End of server info	
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_SND_ALL_SVR_INFO_E;
	SendClient(dwClient, &nmg);


#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	// 접속한 서버에 현재 TracingUser 정보를 전송한다.
	if( m_pClientManager->IsOnline(dwClient) == true  )
	{
		NET_UPDATE_TRACINGCHAR_ALL msg;	

		size_t i;
		for( i = 0; i < m_vecTracingData.size(); i++ )
		{
			msg.tracingData[i] = m_vecTracingData[i];
		}
		SendClient(dwClient, &msg);
	}
#endif
}

// Agent, Field, Login -> Session
// Heartbeat
/*
void CSessionServer::MsgHeartBeat(MSG_LIST* pMsg)
{    
    if (pMsg == NULL) return;

    m_pClientManager->SetHeartBeat(pMsg->dwClient);
}
*/

void CSessionServer::SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi)
{
	if (gsi == NULL) return;

	int nServerGroup         = gsi->nServerGroup;
	int nServerChannelNumber = gsi->nServerChannelNumber;
	int nPort                = gsi->nServicePort;

	if (gsi->nServerType == SERVER_AGENT)
	{
		for (int nChannel=0; nChannel < nServerChannelNumber; ++nChannel)
		{
			::StringCchCopy(m_sServerChannel[nServerGroup][nChannel].szServerIP, MAX_IP_LENGTH+1, gsi->szServerIP);			
			m_sServerChannel[nServerGroup][nChannel].nServicePort         = nPort;
			m_sServerChannel[nServerGroup][nChannel].nServerGroup         = nServerGroup;
			m_sServerChannel[nServerGroup][nChannel].nServerNumber        = nChannel;
			m_sServerChannel[nServerGroup][nChannel].nServerCurrentClient = gsi->nServerCurrentClient;
			m_sServerChannel[nServerGroup][nChannel].nServerMaxClient     = gsi->nServerChannellMaxClient;
			m_sServerChannel[nServerGroup][nChannel].bPK                  = gsi->bPk;
		}
	}

	m_pClientManager->SetSvrInfo(dwClient, gsi);

	CConsoleMessage::GetInstance()->WriteConsole(
		_T("INFO:SetSvrInfo %s %d %d %d %d"),
		gsi->szServerIP,
		gsi->nServicePort,
		gsi->nServerChannellMaxClient,
		gsi->nServerCurrentClient,
		gsi->nServerType );
}

// 서버->세션서버
// 서버의 Full 정보
void CSessionServer::MsgServerInfo(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_SERVER_INFO* nsi = reinterpret_cast <NET_SERVER_INFO*> (pMsg->Buffer);
	SetSvrInfo(pMsg->dwClient, &nsi->gsi);
    
	// 서버 정보를 로그인 서버에 전송한다.
	MsgSndCurSvrInfoToAll();

	// 필드서버라면
	if (nsi->gsi.nServerType == NET_SERVER_FIELD)
	{
		for (int i=0; i < m_pClientManager->GetMaxClient(); ++i)
		{
			// 필드서버 다운시 재연결이 이루어 지면 Agent 에게 다시 Field 로 연결하라고 알려야 한다.
			if (m_pClientManager->IsOnline(i)   == true &&
				m_pClientManager->GetSvrType(i) == SERVER_AGENT &&
				m_pClientManager->GetSvrGrp(i)  == nsi->gsi.nServerGroup &&
				m_pClientManager->GetSvrNum(i)  == nsi->gsi.nServerNumber)
			{
				// Agent 에 자신에게 연결하라고 알린다.
				NET_RECONNECT_FILED	nrf;
				SendClient(i, &nrf);
				CConsoleMessage::GetInstance()->Write(_T("INFO:Inform Agent, Reconnect To Field"));
			}
		}
	}

	// Agent서버 라면...
	if (nsi->gsi.nServerType == NET_SERVER_AGENT)
	{
		DWORD dwClient = pMsg->dwClient;
		if ( m_pClientManager->IsOnline(dwClient) == true )
		{
			MsgSndEncryptKey( dwClient );
		}
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	// 접속한 서버에 현재 TracingUser 정보를 전송한다.
	DWORD dwClient = pMsg->dwClient;
	if( m_pClientManager->IsOnline(dwClient) == true  )
	{
		NET_UPDATE_TRACINGCHAR_ALL msg;	

		size_t i;
		for( i = 0; i < m_vecTracingData.size(); i++ )
		{
			msg.tracingData[i] = m_vecTracingData[i];
		}
		SendClient(dwClient, &msg);
	}
#endif
}

void CSessionServer::MsgEncryptKey(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	NET_ENCRYPT_KEY* nmg = reinterpret_cast <NET_ENCRYPT_KEY*> (pMsg->Buffer);

	::StringCchCopy( m_szEncrypt, ENCRYPT_KEY+1, nmg->szEncryptKey );


	for (DWORD dwClient=0; dwClient < (DWORD) m_pClientManager->GetMaxClient(); ++dwClient)
	{
		if (m_pClientManager->IsOnline(dwClient) == true &&
			m_pClientManager->GetSvrType(dwClient) == NET_SERVER_AGENT)
		{
			MsgSndEncryptKey( dwClient );
		}
	}
}

void CSessionServer::MsgSndEncryptKey( DWORD dwClient )
{
	NET_ENCRYPT_KEY nmg;
	::StringCchCopy(nmg.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt );
	SendClient(dwClient, &nmg);
}

// 모든 연결에 게임서버의 상태정보를 전송한다.
// NET_SERVER_FIELD	3
// NET_SERVER_AGENT	4
void CSessionServer::MsgSndCurSvrInfoToAll(void)
{	
	for (DWORD dwClient=0; dwClient < (DWORD) m_pClientManager->GetMaxClient(); ++dwClient)
	{
		if (m_pClientManager->IsOnline(dwClient) == true &&
			m_pClientManager->GetSvrType(dwClient) != NET_SERVER_FIELD)
		{
			MsgSndSvrInfoReset(dwClient);
			MsgSndCurSvrInfo(dwClient);
		}
	}
}

// 게임서버->세션서버 
// 현재 서버 상태 전송처리
void CSessionServer::MsgServerCurInfo(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_SERVER_CUR_INFO* nsci = reinterpret_cast <NET_SERVER_CUR_INFO*> (pMsg->Buffer);
	m_pClientManager->SetSvrCurInfo(pMsg->dwClient, &nsci->gsci);
}

// Agent->Session
// 현재 채널 상태정보
void CSessionServer::MsgServerChannelInfo(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

	NET_SERVER_CHANNEL_INFO* pNsci = reinterpret_cast <NET_SERVER_CHANNEL_INFO*> (pMsg->Buffer);
	SetChannelInfo(pNsci);
}

void CSessionServer::SetChannelInfo(NET_SERVER_CHANNEL_INFO* pMsg)
{
	if (pMsg == NULL) return;

	int nServerGroup        = pMsg->nServerGroup;
	int nChannel            = pMsg->nChannel;	
	int nChannelCurrentUser = pMsg->nChannelCurrentUser;
	int nChannelMaxUser     = pMsg->nChannelMaxUser;
	bool bPk                = pMsg->bPk;
	
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER || nServerGroup < 0 || nServerGroup >= MAX_SERVER_GROUP)
	{
		return;
	}
	else
	{
		m_sServerChannel[nServerGroup][nChannel].nServerCurrentClient = nChannelCurrentUser;
		m_sServerChannel[nServerGroup][nChannel].nServerMaxClient     =	nChannelMaxUser;
		m_sServerChannel[nServerGroup][nChannel].bPK                  = bPk;

		if( m_dwLoginServerID != -1 )
		{
			if( (float)nChannelMaxUser * 0.95f < (float)nChannelCurrentUser )
			{
				if( m_bServerChannelFull[nServerGroup][nChannel] == FALSE )
				{
					m_bServerChannelFull[nServerGroup][nChannel] = TRUE;
					// LoginServer에 메시지를 보냄
					NET_SERVER_CHANNEL_FULL_INFO nmg;
					nmg.nServerGroup = nServerGroup;
					nmg.nChannel	 = nChannel;
					nmg.bChannelFull = TRUE;
					nmg.nChannelCurrentUser = nChannelCurrentUser;

					SendClient(m_dwLoginServerID, &nmg);					
				}
			}else 
			{
				if( m_bServerChannelFull[nServerGroup][nChannel] == TRUE )
				{
					m_bServerChannelFull[nServerGroup][nChannel] = FALSE;
					// LoginServer에 메시지를 보냄
					NET_SERVER_CHANNEL_FULL_INFO nmg;
					nmg.nServerGroup = nServerGroup;
					nmg.nChannel	 = nChannel;
					nmg.bChannelFull = FALSE;
					nmg.nChannelCurrentUser = nChannelCurrentUser;

					SendClient(m_dwLoginServerID, &nmg);
				}			
			}
		}
	}
}

void CSessionServer::MsgReqAllSvrFullInfo(void)
{
	for (int i=0; i<m_pClientManager->GetMaxClient(); ++i)
	{
		if (m_pClientManager->IsOnline(i) == true)
		{
			MsgReqSvrFullInfo(i);
		}
	}
}

void CSessionServer::MsgReqSvrFullInfo(DWORD dwClient)
{
	NET_MSG_GENERIC		nmg;
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_REQ_FULL_SVR_INFO;
	SendClient(dwClient, &nmg);
}

void CSessionServer::MsgReqAllSvrCurInfo(void)
{	
	for (int i=0; i<m_pClientManager->GetMaxClient(); ++i) 
	{
		if (m_pClientManager->IsOnline(i) == true)
		{
			MsgReqSvrCurInfo(i);
		}
	}
}

void CSessionServer::MsgReqSvrCurInfo(DWORD dwClient)
{
	NET_MSG_GENERIC		nmg;
	nmg.dwSize = sizeof(NET_MSG_GENERIC);
	nmg.nType = NET_MSG_REQ_CUR_STATE;
	SendClient(dwClient, &nmg);
}

// 게임서버 -> 세션서버 : 캐릭터가 게임에 조인했음 
void CSessionServer::MsgGameJoinOK(MSG_LIST* pMsg)
{	
	/*
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAME_JOIN_OK* ngjo = (NET_GAME_JOIN_OK*) pMsg->Buffer;

	// CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "게임서버->세션서버:(%s)캐릭터가 게임에 조인했음", ngjo->szChaName);
	m_pClientManager->UpdateChaJoinOK(ngjo->szUserID,
									ngjo->nUsrNum,
									ngjo->nChaNum,
									ngjo->dwGaeaID,
									ngjo->szChaName);

	// DB Update	
	CUserUpdateCha* pAction = new CUserUpdateCha(ngjo->nUsrNum, ngjo->szChaName);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
}

// Agent->Session : 캐릭터 생성, 갯수 감소
int CSessionServer::MsgChaDecrease(MSG_LIST* pMsg) 
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	
	CUserChaNumDecrease* pAction = new CUserChaNumDecrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob(pAction);
	*/
	return NET_OK;
}

// Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
int CSessionServer::MsgTestChaDecrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);

	CUserTestChaNumDecrease* pAction = new CUserTestChaNumDecrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session : 캐릭터 생성, 갯수 감소
int CSessionServer::DaumMsgChaDecrease(MSG_LIST* pMsg) 
{
	/*
	if (pMsg == NULL) return NET_ERROR;

	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	
	CUserChaNumDecrease* pAction = new CUserChaNumDecrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob(pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
int CSessionServer::DaumMsgTestChaDecrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);
	CUserTestChaNumDecrease* pAction = new CUserTestChaNumDecrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Agent->Session:캐릭터 삭제, 갯수 증가
int CSessionServer::MsgChaIncrease(MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserChaNumIncrease* pAction = new CUserChaNumIncrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Agent->Session:TEST 서버 캐릭터 삭제, 갯수 증가
int	CSessionServer::MsgTestChaIncrease      (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session : 캐릭터 삭제, 갯수 증가
int CSessionServer::DaumMsgChaIncrease      (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserChaNumIncrease* pAction = new CUserChaNumIncrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// DAUM:Agent->Session :TEST 서버  캐릭터 삭제, 갯수 증가
int CSessionServer::DaumMsgTestChaIncrease  (MSG_LIST* pMsg)
{
	/*
	if (pMsg == NULL) return NET_ERROR;
	NET_CHARACTER_INC_DEC* pTemp = reinterpret_cast <NET_CHARACTER_INC_DEC*> (pMsg->Buffer);	
	CUserTestChaNumIncrease* pAction = new CUserTestChaNumIncrease(pTemp->nUserNum);
	COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);
	*/
	return NET_OK;
}

// Control program->Session server : command message
// NET_MSG_SVR_PAUSE	서버를 잠시 멈춘다.
// NET_MSG_SVR_RESUME	멈추어진 서버를 다시 가동시킨다.	
// NET_MSG_SVR_RESTART	서버를 완전히 멈추고 새롭게 가동시킨다.
// NET_MSG_SVR_STOP		서버를 완전히 멈춘다.
void CSessionServer::MsgSvrCmdProcess(MSG_LIST* pMsg)
{
    if (pMsg == NULL) return;

    NET_SVR_CMD_CHANGE_STATE* pTemp = (NET_SVR_CMD_CHANGE_STATE*) pMsg->Buffer;
	int nClient = FindServer(pTemp->nSvrType, pTemp->nSvrGrp, pTemp->nSvrNum, pTemp->nSvrField);
	CConsoleMessage::GetInstance()->Write("INFO:Server state change command %d", nClient);
	if (nClient != NET_ERROR)
	{
		switch(pTemp->nCmdType)
		{
		case NET_MSG_SVR_PAUSE : // 서버 잠시 중단
			CConsoleMessage::GetInstance()->Write("INFO:Server state change 'PAUSE' command %d", nClient);
            MsgSndServerStateChange(nClient, NET_MSG_SVR_PAUSE);
			break;
		case NET_MSG_SVR_RESUME : // 서버 시작
			CConsoleMessage::GetInstance()->Write("INFO:Server state change 'RESUME' command %d", nClient);
			MsgSndServerStateChange(nClient, NET_MSG_SVR_RESUME);
			break;
		case NET_MSG_SVR_RESTART : // 서버 재시작
			CConsoleMessage::GetInstance()->Write("INFO:Server state change 'RESTART' command %d", nClient);
			MsgSndServerStateChange(nClient, NET_MSG_SVR_RESTART);
			break;
		case NET_MSG_SVR_STOP : // 서버 가동 중단
			CConsoleMessage::GetInstance()->Write("INFO:Server state change 'STOP' command %d", nClient);
			MsgSndServerStateChange(nClient, NET_MSG_SVR_STOP);
			break;
		default:
			break;
		}
	}
}

//    _.--"""""--._
//  .'             '.
// /                 \
//;                   ;
//|                   |
//|                   |
//;                   ;
// \ (`'--,    ,--'`) /
//  \ \  _ )  ( _  / /
//   ) )(')/  \(')( (
//  (_ `""` /\ `""` _)
//   \`"-, /  \ ,-"`/
//    `\ / `""` \ /`
//     |/\/\/\/\/\|
//     |\        /|
//     ; |/\/\/\| ;
//      \`-`--`-`/
//       \      /
//        ',__,'
//         q__p
//         q__p
//         q__p
//         q__p
// jgkim
