//! Copyright (c) Min Communications. All rights reserved.
#include "pch.h"
#include "s_CLoginServer.h"
#include "s_CRandomNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLoginServer::CLoginServer(
	HWND hWnd,
	HWND hEditBox,
	HWND hEditBoxInfo )
	: CServer( hWnd, hEditBox, hEditBoxInfo )
	, m_pClientManager( NULL )
	, m_dwLastGameSvrUpdateTime( 0 )
	, m_bInspecting( false )
{	
	for ( int i=0; i<MAX_SERVER_GROUP; i++ )
	{
		for ( int j=0; j<MAX_SERVER_NUMBER; j++ )
		{
			::SecureZeroMemory( &m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN) );
		}
	}
}

CLoginServer::~CLoginServer()
{
	SAFE_DELETE( m_pClientManager );
}

// Ŭ���̾�Ʈ �������� Ŭ���� ����
int CLoginServer::StartClientManager()
{	
	if (m_pClientManager == NULL)
	{
		//m_pClientManager = new CClientLogin( m_nMaxClient, m_hIOServer );
		m_pClientManager = new CClientLogin(m_nMaxClient, m_pSendIOCP, m_pRecvIOCP, m_hIOServer);
	}

	if (!m_pClientManager) 
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

int CLoginServer::Start()
{
	int		nRetCode;	
	
	m_bIsRunning = true;	
	
	if( StartCfg(1)			== NET_ERROR ) return NET_ERROR;	
	if( StartAllowIPCfg()	== NET_ERROR ) return NET_ERROR;
	if( StartIOCPList()		== NET_ERROR ) return NET_ERROR;    	
	if( StartMsgManager()	== NET_ERROR ) return NET_ERROR;
	if( StartIOCP()			== NET_ERROR ) return NET_ERROR;
	if( StartClientManager()== NET_ERROR ) return NET_ERROR;	
	if( StartWorkThread()	== NET_ERROR ) return NET_ERROR;
	if( StartUpdateThread() == NET_ERROR ) return NET_ERROR;	
	if( StartListenThread() == NET_ERROR ) return NET_ERROR;	 

	// ����Ű ����
	GenerateEncrypt();
		
	// Connect to Session Server
	nRetCode = SessionConnect(CCfg::GetInstance()->GetSessionServerIP(), CCfg::GetInstance()->GetSessionServerPort());
	if (nRetCode != NET_OK)
	{		
		m_bIsRunning = false;
		return NET_ERROR;
	}	

	// ���Ӽ����� ������ ��û�Ѵ�.
	SessionReqSvrInfo(); 

	m_dwLastGameSvrUpdateTime = timeGetTime();

	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	CConsoleMessage::GetInstance()->Write(_T("Server Start OK"));
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	
	m_nStatus = S_SERVER_RUNING;

	

	// SERVER_UTIL::CAddressChecker::GetInstance()->AddKnownAddress(CCfg::GetInstance()->GetServerIP());
	// SERVER_UTIL::CAddressChecker::GetInstance()->AddKnownAddress(CCfg::GetInstance()->GetSessionServerIP());

	return NET_OK;
}

int CLoginServer::Stop()
{
	CConsoleMessage::GetInstance()->Write(_T("== Please wait until server stop "));

	// Stop All Thread and exit
	DWORD dwExitCode = 0;	

	//////////////////////////////////////////////////////////////////////////////
	// Disconnect Session Server
	// SessionCloseConnect();

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
	
	// Ŭ���̾�Ʈ ���� Ŭ���� ����
	SAFE_DELETE(m_pSendIOCP);
	SAFE_DELETE(m_pRecvIOCP);	
	CConsoleMessage::GetInstance()->Write(_T("Stop IOCP"));
	// Message Queue ����	
	SAFE_DELETE(m_pRecvMsgManager);
	CConsoleMessage::GetInstance()->Write(_T("Stop Meessage Queue"));
	// DB ��������
	COdbcManager::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->Write(_T("Stop DataBase"));
	// CFG class ����
	CCfg::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->Write(_T("Stop CFG"));
	
	// Put message to console
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	CConsoleMessage::GetInstance()->Write(_T("Server Stop OK"));
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int	CLoginServer::Pause()
{
	::closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	CConsoleMessage::GetInstance()->Write(_T("Server Pause OK"));
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));

	return NET_OK;	
}

int CLoginServer::Resume()
{
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		CConsoleMessage::GetInstance()->Write(_T("======================================================="));
		CConsoleMessage::GetInstance()->Write(_T("Server Resume OK"));
		CConsoleMessage::GetInstance()->Write(_T("======================================================="));

		return NET_OK;
	}
	else
	{
		// Put message to console
		CConsoleMessage::GetInstance()->Write(_T("======================================================="));
		CConsoleMessage::GetInstance()->Write(_T("Server Resume Failed"));
		CConsoleMessage::GetInstance()->Write(_T("======================================================="));

		return NET_ERROR;
	}
}

int	CLoginServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}

//bool CLoginServer::CloseClient(
//	DWORD dwClient )
//{
//	if (dwClient<0) return false;
//
//	bool bReturn = false;
//
//	LockOn();
//	
//	if ( dwClient < NET_RESERVED_SLOT )
//	{
//		if ( dwClient == m_dwSession )
//		{
//			// Session ������ �������
//			if ( true == m_pClientManager->CloseClient( m_dwSession ) )
//			{
//				bReturn = true;
//				// ������ ������ ��� �ٷ� �ٽ� ���� �õ� ���� �ʴ´�.
//				// Heart Beat���� �ٽ� üũ�ϱ� �����̴�.
//				/*
//				if ( true == m_bIsRunning )
//				{
//					// ������ �������̶�� �ٽ� ���� �õ�.
//					SessionConnect();
//				}
//				*/
//				CConsoleMessage::GetInstance()->Write(_T("================================"));
//				CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
//				CConsoleMessage::GetInstance()->Write(_T("================================"));
//			}
//		}
//	}
//	else
//	{
//		if ( true == m_pClientManager->CloseClient(dwClient) )
//		{
//			bReturn = true;
//		}
//	}
//
//	LockOff();
//
//	return bReturn;
//}

void CLoginServer::CloseClient(DWORD dwClient)
{	
	LockOn();

	if ( m_pClientManager->IsOnline(dwClient) )
	{
		if ((dwClient >= 0) && (dwClient < NET_RESERVED_SLOT))
		{
			if ( dwClient == NET_RESERVED_SESSION )

			{
				// Session ������ �������
				m_pClientManager->CloseClient( NET_RESERVED_SESSION );
				// ������ ������ ��� �ٷ� �ٽ� ���� �õ� ���� �ʴ´�. Heart Beat���� �ٽ� üũ�ϱ� �����̴�.
				/*
				if ( true == m_bIsRunning )
				{
				// ������ �������̶�� �ٽ� ���� �õ�.
				SessionConnect();
				}
				*/
				CConsoleMessage::GetInstance()->Write(_T("================================"));
				CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
				CConsoleMessage::GetInstance()->Write(_T("================================"));
			}
		}
		else
		{
			// ��ϵ� IP ����Ʈ�� �����Ѵ�.
			// SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));
			m_pClientManager->CloseClient(dwClient);
		}
	}

	LockOff();
}

// close all client connections
void CLoginServer::CloseAllClient()
{	
	CConsoleMessage::GetInstance()->Write(_T("CloseAllClient"));
	for (DWORD dwNum=NET_RESERVED_SLOT; dwNum<(DWORD) m_nMaxClient; ++dwNum)
	{
		CloseClient(dwNum);
	}
}

void CLoginServer::GenerateEncrypt()
{
	SERVER_UTIL::CRandomNumber random;
	CString str;
	random.GenerateRandomString( str, ENCRYPT_KEY +1 , 1, 2, 3 );

	::StringCchCopy( m_szEncrypt, ENCRYPT_KEY +1 , str );
}

bool CLoginServer::CheckAllowIP( SOCKET socket )
{

	if( !CCfg::GetInstance()->GetAllowFileLoad() ) return TRUE;
	if( !GetAllowIP() ) return TRUE;

	int	nSize = 0;
	sockaddr_in	sAddrIn;
	CString strIp;

	::SecureZeroMemory(&sAddrIn, sizeof(sockaddr_in));
	nSize = sizeof(sockaddr_in);

	// Get client ip address and port
	::getpeername(socket, (sockaddr *) &sAddrIn, &nSize);

	strIp.Format( _T( "%s" ), ::inet_ntoa(sAddrIn.sin_addr) );

	for( int i = 0; i < ( int ) m_vecAllowIPType1.size(); i++ )
	{
		if( m_vecAllowIPType1[ i ] == strIp )
		{
			return TRUE;
		}
	}

	{
		CString strType2;
		for(int i = 0; i <= 2; i++ )
		{
			CString strTmp;
			AfxExtractSubString( strTmp, strIp, i, _T( '.' ) );
			strType2 += strTmp;
			strType2 += _T( '.' );
		}

		for( int i = 0; i < ( int ) m_vecAllowIPType2.size(); i++ )
		{
			CString str1234 = m_vecAllowIPType2[ i ];
			if( m_vecAllowIPType2[ i ] == strType2 )
			{
				return TRUE;
			}
		}
	}

	{
		for( int i = 0; i < ( int ) m_vecAllowIPType3.size(); i++ )
		{
			CString str1234 = m_vecAllowIPType3[ i ];

			CString strTemp;

			AfxExtractSubString( strTemp, m_vecAllowIPType3[ i ], 3, _T( '.' ) );

			CString strFirst;
			CString strSecond;
			int nFirst;
			int nSecond;
			
			AfxExtractSubString( strFirst, strTemp, 0, _T( '/' ) );
			AfxExtractSubString( strSecond, strTemp, 1, _T( '/' ) );
			nFirst = _ttoi( strFirst );
			nSecond = _ttoi( strSecond );

			CString strFront;
			for(int j = 0; j <= 2; j++ )
			{
				CString strTmp;
				AfxExtractSubString( strTmp, m_vecAllowIPType3[ i ], j, _T( '.' ) );
				strFront += strTmp;
				strFront += _T( '.' );
			}

			CString strCmp;
			for(int i = 0; i <= 2; i++ )
			{
				CString strTmp;
				AfxExtractSubString( strTmp, strIp, i, _T( '.' ) );
				strCmp += strTmp;
				strCmp += _T( '.' );
			}
			int nCmpLast;
			CString strCmpLast;
			AfxExtractSubString( strCmpLast, strIp, 3, _T( '.' ) );

			nCmpLast = _ttoi( strCmpLast );

			if( strFront == strCmp )
			{
				if( nFirst <= nCmpLast && nCmpLast <= nSecond )
				{
					return TRUE;
				}
			}
		}		
	}

	return FALSE;
}

int CLoginServer::ListenProc()
{
	LPPER_IO_OPERATION_DATA pIOData = NULL;
	SOCKET Accept;
	DWORD  dwRecvNumBytes = 0;
	DWORD  dwFlags        = 0;
	HANDLE hRetCode       = NULL;
	int    nClientNumber  = 0;
	DWORD  dwClient       = 0;
	int	   nRetCode       = 0;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
			CConsoleMessage::GetInstance()->Write(_T("ERROR:WSAAccept %d"), nRetCode);
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
			{	
				// CConsoleMessage::GetInstance()->Write("Listen Thread Stopped");
				break;
			}
			else
			{
				continue;
			}
		}

		if( !CheckAllowIP( Accept ) ) continue;

		///////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(); // Get free client slot number
		if (nClientNumber == NET_ERROR)
		{
			/*
			LINGER      lingerStruct;     
			lingerStruct.l_onoff  = 1;  
			lingerStruct.l_linger = 5;
			nRetCode = ::setsockopt(Accept, 
									SOL_SOCKET, 
									SO_LINGER, 
									(char *) &lingerStruct, 
									sizeof(lingerStruct));
			*/

			::closesocket(Accept);
			CConsoleMessage::GetInstance()->Write(_T("Reached Max Client Number, Not available client slot"));
			CloseAllClient();
			continue;
		}

		if( m_pClientManager->GetCurrentClientNumber() % 100 == 0  )
		{
			CConsoleMessage::GetInstance()->Write( _T("Current Connect Client Number %d"), m_pClientManager->GetCurrentClientNumber() );
		}


		dwClient = (DWORD) nClientNumber;
		
		// Set client information
		if (m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR)
		{
			m_pClientManager->ReleaseClientID(dwClient);
			continue;
		}

		// IP �� ����Ʈ�� ����Ѵ�.
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient));

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort((HANDLE) Accept, 
										    m_hIOServer, 
										    (DWORD) nClientNumber, 
										    0);

		if (hRetCode == NULL) 
		{
			CConsoleMessage::GetInstance()->Write(_T("CreateIoCompletionPort Error"));
			CloseClient((DWORD) nClientNumber);
			continue;
		}

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// Client ���� ������ �޴´�.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
        pIOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);
		//pIOData = getRecvIO( nClientNumber );

		if (pIOData == NULL) 
		{
			CloseClient((DWORD) nClientNumber);
			continue;
		}
		
		pIOData->dwRcvBytes   = 0;
		pIOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);
		
		::WSARecv(Accept,
		  		  &(pIOData->DataBuf), 
				  1,
				  &dwRecvNumBytes,
				  &dwFlags ,
				  &(pIOData->Overlapped),
				  NULL);

		if( !m_bUseEventThread ) Sleep( 0 );
	}
	return 0;	
}

int CLoginServer::UpdateProc()
{
	// ������ ������ ���� ������
	DWORD dwTimeS          = timeGetTime();
	DWORD dwHeartBeatStart1 = dwTimeS; // ���� ���� ���� �ð�����
	DWORD dwHeartBeatStart2 = dwTimeS; // ���� ���� ���� �ð�����
	bool  bHeartBeat	   = false;
	DWORD dwMaxClient      = m_nMaxClient;
	DWORD dwN              = 0;
	DWORD dwTimeTemp       = timeGetTime();

	float fFrame = 0;
	float fCount = 0;
	
	MSG_LIST* pMsg = NULL;

	// Event ����� Thread�� ���� ����
	DWORD dwNewWaitTime = 0;
	DWORD dwReturn = 0;
	DWORD dwNewTime = 0;
	DWORD dwUpdateFrameTime = 0;

	double fBeforeTime = 0.0f;
	double fCurrentTime = 0.0f;
	double fTimeDelta = 0.0f;

	while (m_bIsRunning)
	{

		if( m_bUseEventThread )	
		{
			dwNewTime = ::GetTickCount();
			dwUpdateFrameTime = dwNewTime - m_dwUpdateOldTime;
			m_dwUpdateOldTime = dwNewTime;

			// ����ð� üũ
			int nUpdateElspTime = (int) (m_dwUpdateWaitTime - dwUpdateFrameTime );
			if ( 0 >= nUpdateElspTime ) 
			{
				dwNewWaitTime = 0;
			} else {
				dwNewWaitTime = nUpdateElspTime;
			}
			dwReturn = ::WaitForSingleObject( m_hUpdateQuitEvent, dwNewWaitTime );

			if ( WAIT_OBJECT_0 == dwReturn )
			{
				break;
			}else if ( WAIT_TIMEOUT != dwReturn )
			{
				continue;
			}
		}else{
			// ���� �ð��� ����
			fCurrentTime = DXUtil_Timer( TIMER_GETAPPTIME );


			// ���� ������ �� ������ ����
			fTimeDelta = fBeforeTime+0.001f;

			// ���� ���簡 ù �������̳� timeGetTime()�� �������� ������ �ð����� ũ�� ������
			if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
			{
				// ���η��� ó��
				// ���� �ð��� g_BeforeTime�� ����
				fBeforeTime = fCurrentTime;
			}
			else
			{
				Sleep(0);
				continue;
			}
		}

        m_pClientManager->ResetPreSleepCID ();
		m_pRecvMsgManager->MsgQueueFlip();	// Flip Messge Queue
		pMsg = m_pRecvMsgManager->GetMsg();// Get Message	

		while (pMsg != NULL)
		{
			MsgProcess(pMsg);
			pMsg = m_pRecvMsgManager->GetMsg();
		}


		// HeartBeat Check
		dwTimeTemp = timeGetTime();
		
		/////////////////////////////////////////////////////////////////////////////
		// Session Server State Check
		if ((dwTimeTemp - dwHeartBeatStart1) > SESSION_CHECK_TIME)
		{
			if( !m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
			{
				SessionConnectSndSvrInfo();
			}
			dwHeartBeatStart1 = timeGetTime();
		}

		if ( (dwTimeTemp - dwHeartBeatStart2) > HEARTBEAT_TIME )
		{
			if (bHeartBeat) // Send Heartbeat
			{
				CConsoleMessage::GetInstance()->WriteInfo(
					_T("Send Heartbeat Request") );

				// Check Session Server
				if ( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
				{
					m_pClientManager->ResetHeartBeat( NET_RESERVED_SESSION );
                    NET_HEARTBEAT_SERVER_REQ HeartBeatMsgReqServer;
					SendClient( NET_RESERVED_SESSION, &HeartBeatMsgReqServer );
				}
				else
				{
					CConsoleMessage::GetInstance()->Write(_T("================================"));
					CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
					CConsoleMessage::GetInstance()->Write(_T("Reconnecting Session Server."));
					CConsoleMessage::GetInstance()->Write(_T("================================"));

					//SessionConnect();
					//SessionConnectSndSvrInfo();
				}

				// Check Client
				NET_HEARTBEAT_CLIENT_REQ HeartBeatMsgReq;
				for ( dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; dwN++ )
				{
					if ( m_pClientManager->IsOnline(dwN) )
					{
						m_pClientManager->ResetHeartBeat( dwN );
						SendClient( dwN, &HeartBeatMsgReq );
					}
				}
			}
			else // Check Heartbeat
			{
				CConsoleMessage::GetInstance()->WriteInfo(_T("Check Heartbeat"));
				// Check Sessein Server
				if ( true == m_pClientManager->IsOnline( NET_RESERVED_SESSION ) &&
					 false == m_pClientManager->CheckHeartBeat( NET_RESERVED_SESSION ) )
				{
					CloseClient( NET_RESERVED_SESSION );
				}

				// Check Client
				for (dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; dwN++)
				{					
					if ( true == m_pClientManager->IsOnline(dwN) && 
						 false == m_pClientManager->CheckHeartBeat(dwN) )
					{
						CloseClient( dwN );
					}
				}
			}
			bHeartBeat = !bHeartBeat;
			dwHeartBeatStart2 = dwTimeTemp;
		}

		// ������ ����
		if ( (dwTimeTemp - dwTimeS) >= FPS_UPDATE_TIME )
		{			
			fFrame = ( (float)((fCount * FPS_UPDATE_TIME) /(float)(dwTimeTemp - dwTimeS)) );

			// ���� ����ڼ�/������ ���� ���÷���
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Update %d FPS Avg %d msec User %d/%d"),
				(int) (fFrame/(FPS_UPDATE_TIME/1000)),
				(int) (FPS_UPDATE_TIME/fFrame),
				(int) m_pClientManager->GetCurrentClientNumber(),
				(int) m_pClientManager->GetMaxClient() );
			
			// ��Ŷ ó���� ���÷��� (������)
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("In Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getInPacketCount() / (FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getInPacketSize() / (FPS_UPDATE_TIME/1000)) );

			// ��Ŷ ó���� ���÷��� (������)
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Out Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getOutPacketCount() / (FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getOutPacketSize() / (FPS_UPDATE_TIME/1000)) );

			// Message Queue �� IOCP �޸� ��뷮 ���÷���
			//CConsoleMessage::GetInstance()->WriteInfo(
			//	_T("Msg Queue Size %d Send %d Recv %d"),
			//	m_pRecvMsgManager->m_pMsgBack->GetCount(),
			//	m_pClientManager->getSendUseCount(),
			//	m_pClientManager->getRecvUseCount() );
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Msg Queue Size %d Send %d/%d Recv %d/%d"),
				m_pRecvMsgManager->m_pMsgBack->GetCount(),
				m_pSendIOCP->GetUseCount(),
				m_pSendIOCP->GetUnUseCount(),
				m_pRecvIOCP->GetUseCount(),
				m_pRecvIOCP->GetUnUseCount() );

			m_pClientManager->resetPacketCount();
			
			dwTimeS = dwTimeTemp;
			fCount = 0;
		}
		else
		{
			fCount++;
		}
	}
	return 0;
}

int CLoginServer::WorkProc()
{
	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes  = 0;
	DWORD dwRcvBytes  = 0;
	DWORD dwByteTrans = 0;
	DWORD dwFlags     = 0;
	DWORD dwClient    = -1;
	int	  nRetCode    = 0;
	int   nLastError  = 0;
	DWORD dwMaxClient = (DWORD) m_pClientManager->GetMaxClient();
	NET_MSG_GENERIC* pNmg = NULL;

	while (m_bIsRunning)
	{
		/////////////////////////////////////////////////////////////////////////////
		// Wait for I/O to complete on any socket
		nRetCode = GetQueuedCompletionStatus(m_hIOServer,
											 &dwByteTrans, 
											 (LPDWORD) &dwClient,
											 (LPOVERLAPPED *) &pPerIoData,
											 INFINITE);
		
		// ������ �����ϱ� ���ؼ� ����ó�� ��ƾ�� ȣ��������...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
		{			
			break;
		}		
		
        // Illegal Message Skip
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{		
			ReleaseOperationData(pPerIoData);
			continue;
		}
		
		if (nRetCode == 0) // ��������
		{
			// 64 �� �����̸� ������ ����
			nLastError = GetLastError();
			//releaseIO( dwClient, pPerIoData );
			CloseClient(dwClient);
			ReleaseOperationData(pPerIoData);
			continue;
		}

		if (nRetCode != 0 && pPerIoData == NULL)
		{
			CloseClient(dwClient);
			continue;
		}		
		
		if (dwByteTrans == 0) // ��������
		{
			//releaseIO( dwClient, pPerIoData );
			CloseClient( dwClient );
			ReleaseOperationData(pPerIoData);
			continue;
		}
		
		switch (pPerIoData->OperationType)
		{
		// �б� �Ϸ� �뺸�϶�...
		case NET_RECV_POSTED :			
			{
                m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);				
				// Get one Message
				while (true) {
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient,FALSE);
					if (NULL == pNmg) {
						break;
					}
					else {
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->dwSize);
					}
				}

				//releaseRecvIO( dwClient , pPerIoData );
				ReleaseOperationData(pPerIoData);
				
				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				//pPerIoData = getRecvIO( dwClient );
				pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

				if (pPerIoData == NULL) continue;
				
				nRetCode = ::WSARecv(m_pClientManager->GetSocket(dwClient),
									 &(pPerIoData->DataBuf), 
									 1, 
									 &dwRcvBytes,
									 &dwFlags,
									 &(pPerIoData->Overlapped),
									 NULL);
				if ((nRetCode == SOCKET_ERROR) && ((nLastError=WSAGetLastError()) != WSA_IO_PENDING))
				{
					//releaseRecvIO( dwClient, pPerIoData );
					ReleaseOperationData(pPerIoData);
					CloseClient(dwClient);
					CConsoleMessage::GetInstance()->Write(
							_T("ERROR:WorkProc WSARecv Client(%d) ERR(%d)"), 
							dwClient,
							nLastError );
				}
			}
			break;
		// ������ �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED :
			{				
				dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // �� ���� ����Ʈ��
				if (dwSndBytes >= pPerIoData->dwTotalBytes) // ������ �Ϸ��
				{
					//releaseSendIO( dwClient, pPerIoData );
					ReleaseOperationData(pPerIoData);
				}
				else // ������ �Ϸ���� ���� ���� ����Ʈ�� ����
				{
					CConsoleMessage::GetInstance()->Write( 
						_T("ERROR:WorkProc have bytes, not send completed"));

					pPerIoData->dwSndBytes  = dwSndBytes; // ���� ����Ʈ�� ������Ʈ
					pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // �������� ����Ÿ ������ ������Ʈ
					dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // �������� ����Ʈ�� ������Ʈ
					pPerIoData->DataBuf.len = (u_long) dwSndBytes; // ���۱��� ������Ʈ

					// ���� �޽��� �����ۿ�û
					nRetCode = m_pClientManager->SendClient2( dwClient, pPerIoData, dwSndBytes );
					if (nRetCode == NET_ERROR)
					{
						//releaseSendIO( dwClient, pPerIoData );
						CloseClient( dwClient );			
						ReleaseOperationData(pPerIoData);
					}
				}
			}
			break;
		default:
			{
				CloseClient(dwClient);
			}
			break;
		}
		if( !m_bUseEventThread ) Sleep( 0 );
	}
	// ServerWorkerThread Running End
	CConsoleMessage::GetInstance()->Write(_T("WorkProc End"));
	return 0;
}

void CLoginServer::Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{	
}

int CLoginServer::SendClient(DWORD dwClient, LPVOID pBuffer)
{
    if (pBuffer == NULL) return NET_ERROR;

	return m_pClientManager->SendClient2(dwClient, pBuffer);
}

/*
void CLoginServer::MsgSndPingAnswer(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	NET_MSG_PING*	nmpr = NULL;	
	NET_MSG_PING	nmpa;
	DWORD			dwClient = pMsg->dwClient;
	
	nmpr			= (NET_MSG_PING*) pMsg->Buffer;	
	nmpa.nmg.dwSize	= sizeof(NET_MSG_PING);;
	nmpa.nmg.nType	= NET_MSG_PING_ANSWER;
	nmpa.stime		= nmpr->stime;
	SendClient(dwClient, (char*) &nmpa);	
}
*/

/// ���� ������ ���� ����
void CLoginServer::SetVersion(int nGameVer, int nPatchVer)
{
	m_nVersion      = nGameVer;
	m_nPatchVersion = nPatchVer;

	CCfg::GetInstance()->SetServerVersion(nGameVer);
	CCfg::GetInstance()->SetPatchVersion(nPatchVer);
}

// ������/������� ���� ��ȭ
void CLoginServer::SetInspecting(bool bStatus)
{
	m_bInspecting = bStatus;

	if (m_bInspecting)
		CConsoleMessage::GetInstance()->Write(_T("Inspecting Start"));
	else
		CConsoleMessage::GetInstance()->Write(_T("Inspecting End"));
}

// ���� ���� ��ȯ ������/�������
bool CLoginServer::GetInspection()
{
	return m_bInspecting;
}

// �������϶� ����IP��� ��뿩��
void CLoginServer::SetAllowIP( bool bAllowIP )
{
	m_bAllowIP = bAllowIP;
}

bool CLoginServer::GetAllowIP()
{
    return m_bAllowIP;
}

int CLoginServer::ExecuteCommand(char* strCmd)
{
    if (strCmd == NULL) return NET_ERROR;

	// ��ū���� �и��� �и���.
	// space
	char seps[]   = " ";
	char *token;
	char* nToken = NULL;
//	char strTemp[100];

	// Establish string and get the first token
	token = strtok_s(strCmd, seps, &nToken);
	while (token != NULL)
	{
		if (strcmp(token, "print_ip") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			return 0;
		}
		// Request
		if (strcmp(token, "req") == 0)
		{
			token = strtok_s(NULL, seps, &nToken);
			if (token)
			{
				if (strcmp(token, "info") == 0)
				{
					// SessionReqSvrInfo();
				}
				return 0;
			}
			else
			{
				return 0;
			}
		}
		//	Memo :	PROFILE ���.
		if( strcmp(token, "profile_on") == 0 )
		{
			g_bProfile = TRUE;
			CConsoleMessage::GetInstance()->Write("PROFILE ON");
			return 0;
		}
		//	Memo :	PROFILE ������.
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

//! -----------------------------------------------------------------------
//! I/O operation memory
//int CLoginServer::StopIOCPList()
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
PER_IO_OPERATION_DATA* CLoginServer::getSendIO(
	DWORD dwClient )
{
	return m_pClientManager->getSendIO( dwClient );
}

PER_IO_OPERATION_DATA* CLoginServer::getRecvIO(
	DWORD dwClient )
{
	return m_pClientManager->getRecvIO( dwClient );
}

void CLoginServer::releaseSendIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseSendIO( dwClient, pData );
}

void CLoginServer::releaseRecvIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseRecvIO( dwClient, pData );
}

void CLoginServer::releaseIO(
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

//      _(,__           __),
//  (_,d888888888b,d888888888b
//   d888888888888/888888888888b_)
//(_8888888P'""'`Y8Y`'""'"Y88888b
//  Y8888P.-'     `      '-.Y8888b_)
// ,_Y88P (_(_(        )_)_) d88Y_,
//  Y88b,  (o  )      (o  ) d8888P
//  `Y888   '-'        '-'  `88Y`
//  ,d/O\         c         /O\b,
//    \_/'.,______w______,.'\_/
//       .-`             `-.
//      /   , d88b  d88b_   \
//     /   / 88888bd88888`\  \
//    /   / \ Y88888888Y   \  \
//    \  \   \ 88888888    /  /
//     `\ `.  \d8888888b, /\\/
//       `.//.d8888888888b; |
//         |/d888888888888b/
//         d8888888888888888b
//      ,_d88p""q88888p""q888b,
//      `""'`\    "`|    /`'""`
//            `.    |===/
//              >   |   |
//              /   |   |
//             |    |   |
//             |    Y  /
//             \   /  /
//              | /| /
//             / / / |
//            /=/  |=/
//           `"`   `"`
