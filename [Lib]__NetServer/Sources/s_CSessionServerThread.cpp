#include "pch.h"
//#include "./DbExecuter.h"
#include "s_CSessionServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int CSessionServer::DatabaseProc()
{
	while (m_bUpdateEnd == false)
	{	
		CDbExecuter::GetInstance()->ExecuteJob();
		if( !m_bUseEventThread ) Sleep( 0 );
	}

	if (m_bUpdateEnd)
	{
		// Last db action
		CDbExecuter::GetInstance()->ExecuteJobLast();
		CConsoleMessage::GetInstance()->Write(_T("CSessionServer::DatabaseProc ExecuteJobLast"));
	}	
	CConsoleMessage::GetInstance()->Write(_T("CSessionServer::DatabaseProc() End"));
	return NET_OK;
}

int CSessionServer::ListenProc()
{
	LPPER_IO_OPERATION_DATA IOData = NULL;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	dwFlags = 0;
	HANDLE	hRetCode = NULL;
	int		nClientNumber = 0;
	int		nRetCode = 0;

	while (m_bIsRunning)
	{
		Accept = ::WSAAccept( m_sServer, NULL, NULL, NULL, 0 );
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
			CConsoleMessage::GetInstance()->Write(
                _T("ERROR:WSAAccept %d"),
				nRetCode );
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
			{
				break;
			}
			else
			{
				continue;
			}
		}
		
		// Get free client slot number
		nClientNumber = m_pClientManager->GetFreeClientID();
		if (nClientNumber == NET_ERROR) 
		{
			closesocket(Accept);
			CConsoleMessage::GetInstance()->Write(
				_T("CSessionServer::ListenProc Not available client slot") );
			continue;
		}
		else
		{
			// Set client information
			m_pClientManager->SetAcceptedClient( nClientNumber, Accept );
		}

		// IP 를 리스트에 등록한다.
		// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP((DWORD) nClientNumber));

		// Associate the accepted socket with the completion port
		hRetCode = ::CreateIoCompletionPort(
						(HANDLE) Accept,
						m_hIOServer, 
						(DWORD) nClientNumber, 
						0 );
		if (hRetCode == NULL)
		{
			CConsoleMessage::GetInstance()->Write(
				_T("CSessionServer::ListenProc CreateIoCompletionPort Error"));
			::closesocket(Accept);
			continue;
		}

		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// 서버 정보를 받는다.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
		//IOData = getRecvIO( nClientNumber );
		IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

		if (IOData == NULL) continue;

		IOData->dwRcvBytes = 0;
		IOData->dwTotalBytes = dwRecvNumBytes;
		// Head receive mode
		m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);

		// Request server information
		if( !m_bUseEventThread ) Sleep( 0 );
		MsgReqSvrFullInfo((DWORD) nClientNumber);
		
		::WSARecv(
			Accept,
			&(IOData->DataBuf), 
			1,
			&dwRecvNumBytes,
			&dwFlags,
			&(IOData->Overlapped),
			NULL );
		if( !m_bUseEventThread ) Sleep(0);
	}
	CConsoleMessage::GetInstance()->Write(_T("Listen Thread Stopped"));
	return 0;
}

int CSessionServer::UpdateProc()
{
	DWORD nTimeS           = 0;
	float fFrame		   = 0;
	float fCount		   = 0;	
	float fUpdateSec	   = 60000; // FPS 표시 갱신시간 1 분
	DWORD dwUsrChkTime	   = 1800000; // 30 분마다 한번씩 사용자 접속끊기 체크
	DWORD dwUsrChkStart	   = timeGetTime(); // 아무것도 하지 않는 사용자 접속끊기를 위한 시간
	DWORD dwUsrSaveTime	   = 60000 * 30; // 사용자 저장시간 30분
	DWORD dwUsrSaveStart   = timeGetTime(); // 사용자 저장시간 체크를 위한 시간변수
	nTimeS				   = timeGetTime();
	m_bUpdateEnd		   = false;
    DWORD dwHeartBeatStart = timeGetTime(); // 서버가 정상가동중인지 체크한다.
    bool  bHeartBeat	   = false; // 정상가동 체크 플래그
    DWORD dwN              = 0;
    DWORD dwMaxClient      = (DWORD) m_nMaxClient;
	MSG_LIST* pMsg         = NULL;

	// Event 방식의 Thread를 위한 변수
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

			// 경과시간 체크
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
			// 현재 시간을 얻어옴
			fCurrentTime = DXUtil_Timer( TIMER_GETAPPTIME );


			// 다음 랜더링 될 시점을 구함
			fTimeDelta = fBeforeTime+0.001f;

			// 만약 현재가 첫 프레임이나 timeGetTime()이 랜더링될 시점의 시간보다 크면 랜더링
			if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
			{
				// 메인루프 처리
				// 현재 시간을 g_BeforeTime에 넣음
				fBeforeTime = fCurrentTime;
			}
			else
			{
				Sleep(0);
				continue;
			}
		}

		m_pClientManager->ResetPreSleepCID();
		// Flip Messge Queue
		m_pRecvMsgManager->MsgQueueFlip();	
		// Get Message
		pMsg = m_pRecvMsgManager->GetMsg();
		
		while (pMsg != NULL)
		{
			MsgProcess(pMsg);
			pMsg = m_pRecvMsgManager->GetMsg();
		}
		/////////////////////////////////////////////////////////////////////////////
		// Additional Update procedure
		/////////////////////////////////////////////////////////////////////////////
        if ((timeGetTime() - dwHeartBeatStart) > HEARTBEAT_TIME)
        {
            if ( bHeartBeat ) // Send Heartbeat
			{
				NET_HEARTBEAT_SERVER_REQ MsgHeartbeatServerReq;

				CConsoleMessage::GetInstance()->WriteInfo(
					_T("Send Heartbeat Request") );
				for ( dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; ++dwN )
				{
					// 온라인 상태인경우
					if (m_pClientManager->IsOnline(dwN))
					{
						MsgHeartbeatServerReq;
						m_pClientManager->ResetHeartBeat(dwN);// Set all hearbeat false
						SendClient( dwN, &MsgHeartbeatServerReq );
                        /*
						G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo(dwN);
                        // 서버타입이 로그인, 필드, 에이전트 일때
                        if (gsi.nServerType == NET_SERVER_LOGIN ||
                            gsi.nServerType == NET_SERVER_FIELD ||
                            gsi.nServerType == NET_SERVER_AGENT)
                        {
						    m_pClientManager->ResetHeartBeat(dwN);// Set all hearbeat false
						    SendClient(dwN, &nmg);
                        }
						*/
					}
				}
			}
			else // Check Heartbeat
			{
				CConsoleMessage::GetInstance()->WriteInfo("Check Heartbeat");
				for ( dwN = NET_RESERVED_SLOT; dwN < dwMaxClient; dwN++ )
				{
					if ( true  == m_pClientManager->IsOnline( dwN ) &&
						 false == m_pClientManager->CheckHeartBeat( dwN ) )
					{
						// 응답없는 연결은 끊는다.
						CloseClient( dwN );
						
						// 서버가 응답없음을 알린다.
                        G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo( dwN );
                        // 서버타입이 로그인, 필드, 에이전트 일때
                        if (gsi.nServerType == NET_SERVER_LOGIN ||
                            gsi.nServerType == NET_SERVER_FIELD ||
                            gsi.nServerType == NET_SERVER_AGENT)
                        {						    
                            // 모든 서버 메니저에 서버 다운을 알린다.
                            for (DWORD dwL = NET_RESERVED_SLOT; dwL < dwMaxClient; dwL++)
                            {
                                if (m_pClientManager->IsOnline(dwL) && (m_pClientManager->GetSvrType(dwL) == 0))
                                {
                                    NET_SERVER_INFO nsi;
                                    nsi.nmg.nType = NET_MSG_SVR_DOWN;
                                    nsi.gsi       = gsi;
                                    SendClient( dwL, &nsi );
									CConsoleMessage::GetInstance()->Write("(%s)RAN %d SERVER ERROR DETECTED Type : %d", gsi.szServerIP, gsi.nServerGroup, gsi.nServerType);
                                }
                            }
                        }
					}
				}
			}
            bHeartBeat = !bHeartBeat;
            dwHeartBeatStart = timeGetTime();
        }

		///////////////////////////////////////////////////////////////////////
		// 프레임 측정
		if ((timeGetTime() - nTimeS) >= fUpdateSec )
		{			
			// DB 연결유지
			// COdbcManager::GetInstance()->CheckConnection();

			fFrame = ( (float)((fCount * fUpdateSec) /(float)(timeGetTime() - nTimeS)));
			//nFrame = Count; 
			nTimeS = timeGetTime();
			fCount = 0;

			// 현재 사용자수 디스플레이
			// 프레임 측정 디스플레이
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Update %d FPS Avg %d msec User %d/%d"),
				(int) (fFrame/(FPS_UPDATE_TIME/1000)),
				(int) (FPS_UPDATE_TIME/fFrame),
				(int) m_pClientManager->GetCurrentClientNumber(),
				(int) m_pClientManager->GetMaxClient() );

			// 패킷 처리량 디스플레이			
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("In Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getInPacketCount()/(FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getInPacketSize()/(FPS_UPDATE_TIME/1000)) );

			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Out Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getOutPacketCount() / (FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getOutPacketSize() / (FPS_UPDATE_TIME/1000)) );


			// Message Queue 및 IOCP 메모리 사용량 디스플레이
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

			// 현재 서버 접속자수 DB 에 저장
			for (int nLoop=0; nLoop < m_pClientManager->GetMaxClient(); nLoop++)
			{
				if (m_pClientManager->IsOnline(nLoop) == true &&
					m_pClientManager->GetSvrType((DWORD) nLoop) == SERVER_AGENT)
				{
					CAddLogServerState* pAction = new CAddLogServerState(
						m_pClientManager->GetSvrGrp((DWORD) nLoop),
						m_pClientManager->GetSvrNum((DWORD) nLoop),
						m_pClientManager->GetSvrCrtClient((DWORD) nLoop),
						m_pClientManager->GetSvrMaxClient((DWORD) nLoop) );
					
					COdbcManager::GetInstance()->AddJob((CDbAction*) pAction);					
				}
			}
			m_pClientManager->resetPacketCount();
		}
		else
		{
			fCount++;
		}
	}

	// Last db action
	COdbcManager::GetInstance()->AllUserLogout();
	CDbExecuter::GetInstance()->ExecuteJobLast();
	CConsoleMessage::GetInstance()->Write(_T("= Last Database Job Executed"));
	TRACE("CSessionServer::UpdateProc() End \n");
	m_bUpdateEnd = true;
	return 0;
}

int CSessionServer::WorkProc()
{
	PER_IO_OPERATION_DATA* pPerIoData = NULL;
	DWORD dwSndBytes  = 0;
	DWORD dwRcvBytes  = 0;
	DWORD dwByteTrans = 0;
	DWORD dwFlag      = 0;
	DWORD dwClient    = -1;
	int	  nRetCode    = 0;
	int   nLastError  = 0;	
	DWORD dwMaxClient = m_pClientManager->GetMaxClient();
	NET_MSG_GENERIC* pNmg = NULL;

	while (m_bIsRunning)
	{		
		// Wait for I/O to complete on any socket
		nRetCode = GetQueuedCompletionStatus(
						m_hIOServer,
			            &dwByteTrans,
			            (LPDWORD) &dwClient,
			            (LPOVERLAPPED *) &pPerIoData,
			            INFINITE );
		
		// 서버를 종료하기 위해서 종료처리 루틴을 호출했을때...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
		{
			break;
		}
		
        // Illegal Message Skip
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{
			continue;
		}		
		
		if (nRetCode == 0) // 강제종료
		{
			// 64 번 에러이면 무조건 종료
			nLastError = GetLastError();
			//releaseIO( dwClient, pPerIoData );
			//CloseClient( dwClient );
			CloseClient(dwClient);
			ReleaseOperationData(pPerIoData);
			continue;
		}

		if (nRetCode != 0 && pPerIoData == NULL)
		{
			CloseClient(dwClient);
			continue;
		}		
		
		if (dwByteTrans == 0) // 정상종료
		{
			//releaseIO( dwClient, pPerIoData );
			//CloseClient( dwClient );
			CloseClient(dwClient);
			ReleaseOperationData(pPerIoData);
			continue;
		}

		switch (pPerIoData->OperationType)
		{
		// 읽기 완료 통보일때...
		case NET_RECV_POSTED :
			{
				m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);
				while (true) {
					// 다 받은 메시지가 있는지 확인한다.
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient,FALSE);
					if (pNmg == NULL) {
						break;
					}
					else {
						// 다 받은 메시지를 메시지 queue 에 넣는다.
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->dwSize);
					}
				}
				
				// 완료가 되었기 때문에 메모리를 반환한다.
				//releaseRecvIO( dwClient, pPerIoData );
				ReleaseOperationData(pPerIoData);

				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				// 새롭게 받기 요청을 하기 위해서 메모리를 가져온다.
				//pPerIoData = getRecvIO( dwClient );
				pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

				if (pPerIoData == NULL) continue;
				
				// 새롭게 받기 요청을 한다.
				// WSARecv 요청
				nRetCode = ::WSARecv(
								m_pClientManager->GetSocket(dwClient),
					            &(pPerIoData->DataBuf), 
					            1, 
					            &dwRcvBytes,
					            &dwFlag,
					            &(pPerIoData->Overlapped),
					            NULL );
				
				if ((nRetCode == SOCKET_ERROR) && ((nLastError=WSAGetLastError()) != WSA_IO_PENDING))
				{
					// 치명적 에러, 로그에 기록남김
					CConsoleMessage::GetInstance()->Write(
						                  _T("WorkProc WSARecv %d ERROR"),
										nLastError);
					//releaseRecvIO( dwClient, pPerIoData );
					//CloseClient( dwClient );
					CloseClient(dwClient);
					ReleaseOperationData(pPerIoData);
				}
			}
			break;
		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED :
			// 총 보낸 바이트수
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes;
			// 쓰기 완료됨
			if (dwSndBytes >= pPerIoData->dwTotalBytes)
			{				
				//releaseSendIO( dwClient, pPerIoData );
				ReleaseOperationData(pPerIoData);
			}
			else // 쓰기 완료되지 않음 남은 바이트를 전송
			{
				CConsoleMessage::GetInstance()->Write( 
					_T("ERROR:WorkProc have bytes, not send completed"));

				pPerIoData->dwSndBytes  = dwSndBytes; // 보낸 바이트수 업데이트				
				pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // 보내야할 데이타 포인터 업데이트				
				dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // 보내야할 바이트수 업데이트				
				pPerIoData->DataBuf.len = (u_long)dwSndBytes; // 버퍼길이 업데이트				
				// 전송요청
				Send(dwClient, pPerIoData, dwSndBytes);
			}
			break;
		default :
			CConsoleMessage::GetInstance()->Write(_T("Client %d Unknown State"), dwClient);			
			CloseClient(dwClient);
			break;
		}
		if( !m_bUseEventThread ) Sleep( 0 );
	}
	// ServerWorkerThread Running End
	TRACE("CSessionServer::WorkProc() End \n");
	return 0;
}