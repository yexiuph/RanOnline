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

		// IP �� ����Ʈ�� ����Ѵ�.
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
		// ���� ������ �޴´�.
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
	float fUpdateSec	   = 60000; // FPS ǥ�� ���Žð� 1 ��
	DWORD dwUsrChkTime	   = 1800000; // 30 �и��� �ѹ��� ����� ���Ӳ��� üũ
	DWORD dwUsrChkStart	   = timeGetTime(); // �ƹ��͵� ���� �ʴ� ����� ���Ӳ��⸦ ���� �ð�
	DWORD dwUsrSaveTime	   = 60000 * 30; // ����� ����ð� 30��
	DWORD dwUsrSaveStart   = timeGetTime(); // ����� ����ð� üũ�� ���� �ð�����
	nTimeS				   = timeGetTime();
	m_bUpdateEnd		   = false;
    DWORD dwHeartBeatStart = timeGetTime(); // ������ ���󰡵������� üũ�Ѵ�.
    bool  bHeartBeat	   = false; // ���󰡵� üũ �÷���
    DWORD dwN              = 0;
    DWORD dwMaxClient      = (DWORD) m_nMaxClient;
	MSG_LIST* pMsg         = NULL;

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
					// �¶��� �����ΰ��
					if (m_pClientManager->IsOnline(dwN))
					{
						MsgHeartbeatServerReq;
						m_pClientManager->ResetHeartBeat(dwN);// Set all hearbeat false
						SendClient( dwN, &MsgHeartbeatServerReq );
                        /*
						G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo(dwN);
                        // ����Ÿ���� �α���, �ʵ�, ������Ʈ �϶�
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
						// ������� ������ ���´�.
						CloseClient( dwN );
						
						// ������ ��������� �˸���.
                        G_SERVER_INFO gsi = m_pClientManager->GetSvrInfo( dwN );
                        // ����Ÿ���� �α���, �ʵ�, ������Ʈ �϶�
                        if (gsi.nServerType == NET_SERVER_LOGIN ||
                            gsi.nServerType == NET_SERVER_FIELD ||
                            gsi.nServerType == NET_SERVER_AGENT)
                        {						    
                            // ��� ���� �޴����� ���� �ٿ��� �˸���.
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
		// ������ ����
		if ((timeGetTime() - nTimeS) >= fUpdateSec )
		{			
			// DB ��������
			// COdbcManager::GetInstance()->CheckConnection();

			fFrame = ( (float)((fCount * fUpdateSec) /(float)(timeGetTime() - nTimeS)));
			//nFrame = Count; 
			nTimeS = timeGetTime();
			fCount = 0;

			// ���� ����ڼ� ���÷���
			// ������ ���� ���÷���
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("Update %d FPS Avg %d msec User %d/%d"),
				(int) (fFrame/(FPS_UPDATE_TIME/1000)),
				(int) (FPS_UPDATE_TIME/fFrame),
				(int) m_pClientManager->GetCurrentClientNumber(),
				(int) m_pClientManager->GetMaxClient() );

			// ��Ŷ ó���� ���÷���			
			CConsoleMessage::GetInstance()->WriteInfo(
				_T("In Packet Count %d/sec Traffic %d(bytes)/sec"),
				(int) (m_pClientManager->getInPacketCount()/(FPS_UPDATE_TIME/1000)),
				(int) (m_pClientManager->getInPacketSize()/(FPS_UPDATE_TIME/1000)) );

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

			// ���� ���� �����ڼ� DB �� ����
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
		
		// ������ �����ϱ� ���ؼ� ����ó�� ��ƾ�� ȣ��������...
		if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
		{
			break;
		}
		
        // Illegal Message Skip
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{
			continue;
		}		
		
		if (nRetCode == 0) // ��������
		{
			// 64 �� �����̸� ������ ����
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
		
		if (dwByteTrans == 0) // ��������
		{
			//releaseIO( dwClient, pPerIoData );
			//CloseClient( dwClient );
			CloseClient(dwClient);
			ReleaseOperationData(pPerIoData);
			continue;
		}

		switch (pPerIoData->OperationType)
		{
		// �б� �Ϸ� �뺸�϶�...
		case NET_RECV_POSTED :
			{
				m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);
				while (true) {
					// �� ���� �޽����� �ִ��� Ȯ���Ѵ�.
					pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient,FALSE);
					if (pNmg == NULL) {
						break;
					}
					else {
						// �� ���� �޽����� �޽��� queue �� �ִ´�.
						m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->dwSize);
					}
				}
				
				// �Ϸᰡ �Ǿ��� ������ �޸𸮸� ��ȯ�Ѵ�.
				//releaseRecvIO( dwClient, pPerIoData );
				ReleaseOperationData(pPerIoData);

				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				// ���Ӱ� �ޱ� ��û�� �ϱ� ���ؼ� �޸𸮸� �����´�.
				//pPerIoData = getRecvIO( dwClient );
				pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

				if (pPerIoData == NULL) continue;
				
				// ���Ӱ� �ޱ� ��û�� �Ѵ�.
				// WSARecv ��û
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
					// ġ���� ����, �α׿� ��ϳ���
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
		// ���� �Ϸ� �뺸 �϶�...
		case NET_SEND_POSTED :
			// �� ���� ����Ʈ��
			dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes;
			// ���� �Ϸ��
			if (dwSndBytes >= pPerIoData->dwTotalBytes)
			{				
				//releaseSendIO( dwClient, pPerIoData );
				ReleaseOperationData(pPerIoData);
			}
			else // ���� �Ϸ���� ���� ���� ����Ʈ�� ����
			{
				CConsoleMessage::GetInstance()->Write( 
					_T("ERROR:WorkProc have bytes, not send completed"));

				pPerIoData->dwSndBytes  = dwSndBytes; // ���� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // �������� ����Ÿ ������ ������Ʈ				
				dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // �������� ����Ʈ�� ������Ʈ				
				pPerIoData->DataBuf.len = (u_long)dwSndBytes; // ���۱��� ������Ʈ				
				// ���ۿ�û
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