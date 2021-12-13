#include "pch.h"
//#include "./DbExecuter.h"
#include "s_CSessionServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSessionServer::CSessionServer(
	HWND hWnd,
	HWND hEditBox,
	HWND hEditBoxInfo )
	: CServer(
		hWnd,
		hEditBox,
		hEditBoxInfo )
	, m_pClientManager(NULL)
{
	m_dwLoginServerID = -1;
	for (int nGroup=0; nGroup < MAX_SERVER_GROUP; ++nGroup)
	{
		for (int nChannel=0; nChannel < MAX_CHANNEL_NUMBER; ++nChannel)
		{
			// m_sServerChannel[nGroup][nChannel].szServerIP;
			m_sServerChannel[nGroup][nChannel].nServicePort         = 0;
			m_sServerChannel[nGroup][nChannel].nServerGroup         = 0;
			m_sServerChannel[nGroup][nChannel].nServerNumber        = 0;
			m_sServerChannel[nGroup][nChannel].nServerCurrentClient = 0;
			m_sServerChannel[nGroup][nChannel].nServerMaxClient     = 0;
			m_bServerChannelFull[nGroup][nChannel]					= FALSE;
		}
	}
}

CSessionServer::~CSessionServer()
{
	SAFE_DELETE(m_pClientManager);
}

// 클라이언트 정보저장 클래스 생성
int	CSessionServer::StartClientManager()
{	
	SAFE_DELETE(m_pClientManager);
	//m_pClientManager = new CClientSession(m_nMaxClient, m_hIOServer);
	m_pClientManager = new CClientSession(m_nMaxClient, m_pSendIOCP, m_pRecvIOCP, m_hIOServer);
	if (!m_pClientManager) 
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:StartClientManager m_pClientManager NULL"));
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Create DB manager and Open DB
int	CSessionServer::StartDbManager()
{
	int	nRetCode = 0;
	
	CDbExecuter::GetInstance()->SetServer((CServer*) this);

	COdbcManager::GetInstance()->SetServiceProvider(m_nServiceProvider);

	nRetCode = COdbcManager::GetInstance()->OpenUserDB(CCfg::GetInstance()->GetUserOdbcName(),
							                           CCfg::GetInstance()->GetUserOdbcUser(),
							                           CCfg::GetInstance()->GetUserOdbcPass(),
							                           CCfg::GetInstance()->GetUserOdbcDatabase(),
							                           CCfg::GetInstance()->GetUserOdbcPoolSize(),
							                           CCfg::GetInstance()->GetUserOdbcResponseTime());
	if (nRetCode == DB_ERROR)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:User db open failed"));
		m_bIsRunning = false;
		return NET_ERROR;
	}

	nRetCode = COdbcManager::GetInstance()->OpenLogDB(CCfg::GetInstance()->GetLogOdbcName(),
													  CCfg::GetInstance()->GetLogOdbcUser(),
													  CCfg::GetInstance()->GetLogOdbcPass(),
													  CCfg::GetInstance()->GetLogOdbcDatabase(),
													  CCfg::GetInstance()->GetLogOdbcPoolSize(),
													  CCfg::GetInstance()->GetLogOdbcResponseTime());

	if (nRetCode == DB_ERROR)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:Log db open failed"));
		m_bIsRunning = false;
		return NET_ERROR;
	}

	// Terra User Database (Malaysia)
	if (m_nServiceProvider == SP_TERRA || m_nServiceProvider == SP_MALAYSIA )
	{
		nRetCode = COdbcManager::GetInstance()->OpenTerraDB(CCfg::GetInstance()->GetTerraOdbcName(),
													        CCfg::GetInstance()->GetTerraOdbcUser(),
													        CCfg::GetInstance()->GetTerraOdbcPass(),
													        CCfg::GetInstance()->GetTerraOdbcDatabase(),
													        CCfg::GetInstance()->GetTerraOdbcPoolSize(),
													        CCfg::GetInstance()->GetTerraOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{			
			CConsoleMessage::GetInstance()->Write(
				                                  _T("ERROR:Terra db open failed"));
			m_bIsRunning = false;
			return NET_ERROR;
		}
	}

	// GSP User Database (Malaysia)
	if (m_nServiceProvider == SP_GLOBAL)
	{
		nRetCode = COdbcManager::GetInstance()->OpenGspDB(
													CCfg::GetInstance()->GetGspOdbcName(),
													CCfg::GetInstance()->GetGspOdbcUser(),
													CCfg::GetInstance()->GetGspOdbcPass(),
													CCfg::GetInstance()->GetGspOdbcDatabase(),
													CCfg::GetInstance()->GetGspOdbcPoolSize(),
													CCfg::GetInstance()->GetGspOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{			
			CConsoleMessage::GetInstance()->Write(
				                                _T("ERROR:Gsp db open failed"));
			m_bIsRunning = false;
			return NET_ERROR;
		}
	}

	// Thailand User Database (Thailand)
	if (m_nServiceProvider == SP_THAILAND)
	{
		nRetCode = COdbcManager::GetInstance()->OpenThaiDB(CCfg::GetInstance()->GetTerraOdbcName(),
													        CCfg::GetInstance()->GetTerraOdbcUser(),
													        CCfg::GetInstance()->GetTerraOdbcPass(),
													        CCfg::GetInstance()->GetTerraOdbcDatabase(),
													        CCfg::GetInstance()->GetTerraOdbcPoolSize(),
													        CCfg::GetInstance()->GetTerraOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Thai DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Thai DB Open OK") );
		}
	}

	return StartDatabaseThread();
	//return NET_OK;
}

int CSessionServer::Start()
{
	m_bIsRunning = true;

	if (StartCfg(0)			== NET_ERROR) return NET_ERROR;	
	if (StartIOCPList()		== NET_ERROR) return NET_ERROR;    	
	if (StartMsgManager()	== NET_ERROR) return NET_ERROR;
	if (StartIOCP()			== NET_ERROR) return NET_ERROR;
	if (StartClientManager()== NET_ERROR) return NET_ERROR;	
	if (StartDbManager()    == NET_ERROR) return NET_ERROR;
	if (StartWorkThread()	== NET_ERROR) return NET_ERROR;
	if (StartUpdateThread() == NET_ERROR) return NET_ERROR;	
	if (StartListenThread() == NET_ERROR) return NET_ERROR;	
	
	CConsoleMessage::GetInstance()->Write("=======================================================");	
	CConsoleMessage::GetInstance()->Write("Server Start OK");	
	CConsoleMessage::GetInstance()->Write("=======================================================");

	m_nStatus = S_SERVER_RUNING;

	return NET_OK;
}

int CSessionServer::Stop()
{
	// Stop All Thread and exit
	CConsoleMessage::GetInstance()->Write(_T("== Please wait until server stop "));

	DWORD dwExitCode = 0;	
	
	EnterCriticalSection(&m_CriticalSection);
	m_bIsRunning = false;
	LeaveCriticalSection(&m_CriticalSection);

	if( m_bUseEventThread )	
	{
		::SetEvent( m_hUpdateQuitEvent );

		::WaitForSingleObject( m_hUpdateThread, INFINITE );
		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );
	}else{
		Sleep( 1000 );
	}


	StopListenThread();
	StopIOCPList();
	StopWorkThread();
	StopIOCP();

	CloseAllClient(); 	// close all client connections

	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	// IOCP 종료
	SAFE_DELETE(m_pRecvIOCP);
	SAFE_DELETE(m_pSendIOCP);
	CConsoleMessage::GetInstance()->Write("Stop IOCP");
	// Message Queue 종료	
	SAFE_DELETE(m_pRecvMsgManager);
	CConsoleMessage::GetInstance()->Write("Stop Message Queue");
	// DB 연결종료
	//CDbExecuter::GetInstance()->endThread();
	COdbcManager::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->Write("Stop DataBase");
	// CFG class 종료
	CCfg::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->Write("Stop CFG");	
	// Put message to console
	CConsoleMessage::GetInstance()->Write("=======================================================");	
	CConsoleMessage::GetInstance()->Write("Server Stop OK");	
	CConsoleMessage::GetInstance()->Write("=======================================================");
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int	CSessionServer::Pause()
{
	::closesocket(m_sServer);	
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	CConsoleMessage::GetInstance()->Write("=======================================================");	
	CConsoleMessage::GetInstance()->Write("Server Pause OK");	
	CConsoleMessage::GetInstance()->Write("=======================================================");

	return NET_OK;	
}

int CSessionServer::Resume()
{	
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		CConsoleMessage::GetInstance()->Write("=======================================================");	
		CConsoleMessage::GetInstance()->Write("Server Resume OK");	
		CConsoleMessage::GetInstance()->Write("=======================================================");

		return NET_OK;
	}
	else
	{
		// Put message to console
		CConsoleMessage::GetInstance()->Write("=======================================================");	
		CConsoleMessage::GetInstance()->Write("Server Resume Failed");	
		CConsoleMessage::GetInstance()->Write("=======================================================");

		return NET_ERROR;
	}
}

int	CSessionServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}

void CSessionServer::CloseAllClient()
{
	for (DWORD dwNum=0; dwNum<(DWORD) m_nMaxClient; ++dwNum)
	{
		CloseClient(dwNum);
	}
}

//// 클라이언트의 연결을 해제한다.
//bool CSessionServer::CloseClient(
//	DWORD dwClient )
//{
//	if ( true  == m_pClientManager->CloseClient(dwClient) )
//	{
//		if (m_pClientManager->IsGameServer(dwClient) == true) // Agent 서버라면
//		{
//			// 사용자리스트를 삭제한다.
//			m_pClientManager->UserEraseServer(m_pClientManager->GetSvrGrp(dwClient),
//											m_pClientManager->GetSvrNum(dwClient));
//			// 사용자를 로그아웃 시킨다.
//			CUserLogoutSvr* pAction = new CUserLogoutSvr(m_pClientManager->GetSvrGrp(dwClient), 
//														m_pClientManager->GetSvrNum(dwClient));
//			COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pAction );
//		}
//
//		// 등록된 IP 리스트를 제거한다.
//		// SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));
//		// 연결종료
//		
//		// 서버상태 재전송
//		MsgSndCurSvrInfoToAll();
//		return true;
//	}
//	return false;
//}

// 클라이언트의 연결을 해제한다.
void CSessionServer::CloseClient(DWORD dwClient)
{

	if( m_pClientManager->GetSvrType(dwClient) == SERVER_AGENT ||
		m_pClientManager->GetSvrType(dwClient) == SERVER_FIELD ||
		m_pClientManager->GetSvrType(dwClient) == SERVER_LOGIN )
	{
		G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo( dwClient );

		for (DWORD dwL = NET_RESERVED_SLOT; dwL < (DWORD)m_nMaxClient; dwL++)
		{			
			if (m_pClientManager->IsOnline(dwL) && (m_pClientManager->GetSvrType(dwL) == 0))
			{
				NET_SERVER_INFO nsi;
				nsi.nmg.nType = NET_MSG_SVR_CLOSECLIENT;
				nsi.gsi       = gsi;
				SendClient( dwL, &nsi );
				CConsoleMessage::GetInstance()->Write("(%s)RAN %d SERVER CLOSE CLIENT Type : %d", gsi.szServerIP, gsi.nServerGroup, gsi.nServerType);
			}
		}
	}

	if (m_pClientManager->IsGameServer(dwClient) == true) // Agent 서버라면
	{
		// 사용자리스트를 삭제한다.
		m_pClientManager->UserEraseServer(m_pClientManager->GetSvrGrp(dwClient),
			m_pClientManager->GetSvrNum(dwClient));
		// 사용자를 로그아웃 시킨다.
		CUserLogoutSvr* pAction = new CUserLogoutSvr(m_pClientManager->GetSvrGrp(dwClient), 
			m_pClientManager->GetSvrNum(dwClient));
		COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
	}

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	if( m_vecTracingData.size() != 0 && 
		m_pClientManager->GetSvrType(dwClient) != SERVER_AGENT &&
		m_pClientManager->GetSvrType(dwClient) != SERVER_FIELD )
	{
		for (int i=0; i < m_pClientManager->GetMaxClient(); ++i)
		{
			// 서버메니져가 CloseClient되면 초기화한다.
			if (m_pClientManager->IsOnline(i) == true )
			{
				NET_UPDATE_TRACINGCHAR_ALL msg;	
				SendClient(i, &msg);

				CConsoleMessage::GetInstance()->Write(_T("INFO:Tracing User All Clear"));
			}
		}
	}

#endif

	// 등록된 IP 리스트를 제거한다.
	// SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));
	// 연결종료
	m_pClientManager->CloseClient(dwClient);
	// 서버상태 재전송
	MsgSndCurSvrInfoToAll();
}

void CSessionServer::Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
}

void CSessionServer::UserInsert(CString strUsrID, LPCNT_USR_INFO pData)
{
    if (pData == NULL) 
	{
		return;
	}
	else
	{
		m_pClientManager->UserInsert(strUsrID, pData);
	}
}

int CSessionServer::SendClient(DWORD dwClient, LPVOID pBuffer)
{
    if (pBuffer == NULL)
	{
		return NET_ERROR;
	}
	else
	{
		return m_pClientManager->SendClient2(dwClient, pBuffer);
	}
}
int CSessionServer::ExecuteCommand(char* strCmd)
{
    if (strCmd == NULL) return NET_ERROR;

	// 토큰으로 분리할 분리자.
	// space
	char seps[]   = " ";
	char *token = NULL;
	char* nToken = NULL;

	// Establish string and get the first token
	token = strtok_s(strCmd, seps, &nToken);
	while (token)
	{
		// Request Server Info
		// command : req svrfullinfo
		if (strcmp(token, "req") == 0) 
		{
			token = strtok_s(NULL, seps, &nToken);
			if (token) 
			{
				// 서버 정보 요청
				if (strcmp(token, "svrfullinfo") == 0) 
					MsgReqAllSvrFullInfo();
				else if (strcmp(token, "svrcurinfo") == 0) 
					MsgReqAllSvrCurInfo();
				return 0;
			} 
			else 
			{
				return 0;
			}
		} 
		if (strcmp(token, "print_ip") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			return 0;
		}
		if (strcmp(token, "print_user") == 0)
		{
			// 사용자 리스트 출력
			token = strtok_s(NULL, seps, &nToken);
			if (token)
			{
				if (strcmp(token, "user") == 0)
					m_pClientManager->PrintUsrList();
				else
					return 0;
			}
			else
			{
				return 0;
			}
		}
		if (strcmp(token, "chat") == 0)
		{
			// 채팅메시지
			token = strtok_s(NULL, seps, &nToken);
			if (token)
			{
				// 시스템 메시지
				if (strcmp(token, "system") == 0)
				{
					char str[] = "chat system ";
					char *pdest;
					int  result;
					pdest = strstr(strCmd, str);
					result = (int) (pdest - strCmd + 1);
					if ( pdest != NULL )
						MsgSndChatGlobal(pdest);
					else
						return 0;
				}
				// 특정사용자에게 보내는 메시지
				else if (strcmp(token, "user") == 0)
				{
				}
				else
				{
					return 0;
				}	
			}
			else
			{
				return 0;
			}
		}
		//	Memo :	PROFILE 사용.
		if( strcmp(token, "profile_on") == 0 )
		{
			g_bProfile = TRUE;
			CConsoleMessage::GetInstance()->Write("PROFILE ON");
			return 0;
		}
		//	Memo :	PROFILE 사용안함.
		if( strcmp(token, "profile_off") == 0 )
		{
			g_bProfile = FALSE;
			CConsoleMessage::GetInstance()->Write("PROFILE OFF");
			return 0;
		}

		// Get next token
		token = strtok_s(NULL, seps, &nToken);
	}	
	return 0;
}

int CSessionServer::MsgLoginMaintenance( MSG_LIST* pMsg )
{
	SNET_SERVER_LOGINMAINTENANCE_SESSION* pnsll = reinterpret_cast < SNET_SERVER_LOGINMAINTENANCE_SESSION* > (pMsg->Buffer);

	SNET_SERVER_LOGINMAINTENANCE_LOGIN nsll;
	nsll.bOn = pnsll->bOn;	

	SendClient( m_dwLoginServerID ,&nsll );
	return 0;
}


//! -----------------------------------------------------------------------
//! I/O operation memory
//int CSessionServer::StopIOCPList()
//{
//	/*
//	DWORD dwCount = 0;
//	
//	dwCount = m_pClientManager->getSendUseCount();
//
//	for (DWORD i=0; i<dwCount; ++i)
//	{
//		::PostQueuedCompletionStatus(
//			m_hIOServer,
//			0,
//			m_dwCompKey,
//			NULL );
//	}
//	Sleep( 1000 );
//
//	nCount = m_pSendIOCP->GetUseCount();
//	for (int i=0; i<nCount; i++) 
//	{
//		::PostQueuedCompletionStatus(m_hIOServer,
//							0,
//							m_dwCompKey, 
//							NULL);
//	}
//	Sleep( 1000 );
//	*/
//
//	return NET_OK;
//}

/*
PER_IO_OPERATION_DATA* CSessionServer::getSendIO(
	DWORD dwClient )
{
	return m_pClientManager->getSendIO( dwClient );
}

PER_IO_OPERATION_DATA* CSessionServer::getRecvIO(
	DWORD dwClient )
{
	return m_pClientManager->getRecvIO( dwClient );
}

void CSessionServer::releaseSendIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseSendIO( dwClient, pData );
}

void CSessionServer::releaseRecvIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseRecvIO( dwClient, pData );
}

void CSessionServer::releaseIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	if (NULL != pData)
	{
		if (NET_SEND_POSTED == pData->OperationType)
		{
			releaseSendIO( dwClient, pData );
		}
		else
		{
			releaseRecvIO( dwClient, pData );
		}
	}
}
*/

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