#include "pch.h"
//#include "./DbExecuter.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "s_CFieldServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef _RELEASED
#define fRESERVATIONTIME 30.0f
#else
#define fRESERVATIONTIME 0.0f
#endif


int CFieldServer::ListenProc(void)
{
	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp  CFieldServer::ListenProc\n"));
	
		LPPER_IO_OPERATION_DATA IOData = NULL;
		HANDLE	hRetCode       = NULL;		
		DWORD	dwRecvNumBytes = 0;
		DWORD	Flags          = 0;		
		int		nClientNumber  = 0;
		DWORD   dwClient       = 0;
		int		nRetCode       = 0;
		SOCKET	Accept;

		while (m_bIsRunning)
		{
			Accept = ::WSAAccept( m_sServer, NULL, NULL, NULL, 0 );
			if ( Accept == INVALID_SOCKET )
			{
				nRetCode = ::WSAGetLastError();
				CConsoleMessage::GetInstance()->Write( _T("ERROR:WSAAccept %d"), nRetCode );
				if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				{
					break;
				}
				else
				{
					continue;
				}
			}

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
				CConsoleMessage::GetInstance()->Write(_T("INFO:Reached Max Client Number"));
				//::shutdown(Accept, SB_BOTH);
				::closesocket(Accept);
				continue;
			}
			
			dwClient = (DWORD) nClientNumber;

			// Set client information
			// Ŭ���̾�Ʈ�� ip, port, ���ӽð��� �����.
			if ( m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR )
			{
				//CloseClient( dwClient );
				//::shutdown(Accept, SB_BOTH);
				//int nRetCode = ::closesocket(Accept);
				//if (nRetCode == SOCKET_ERROR)
				//{
				//	nRetCode = ::WSAGetLastError();
				//	CConsoleMessage::GetInstance()->Write(
				//		                               _T("CNetUser::SetAcceptedClient closesocket %d"),
				//		                               nRetCode);
				//}
				m_pClientManager->ReleaseClientID( dwClient );
				continue;
			}
			
			// Block �� �ּҶ��
			// ������ �����Ų��.
			if ( SERVER_UTIL::CAddressChecker::GetInstance()->isBlock( m_pClientManager->GetClientIP(dwClient), timeGetTime() ) == true )
			{
				CloseClient(dwClient);
				continue;
			}

			// Associate the accepted socket with the completion port
			hRetCode = ::CreateIoCompletionPort(
							(HANDLE) Accept, 
							m_hIOServer, 
							(DWORD) nClientNumber, 
							0 );

			if (hRetCode == NULL) 
			{
				CConsoleMessage::GetInstance()->Write(_T("CreateIoCompletionPort Error"));
				CloseClient(dwClient);
				continue;
			}

			//if( !m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
			//{
			//	SessionConnectSndSvrInfo();
			//}

			//if( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
			//{
				// ���Ǽ����� ���� �����ڼ� ����
				SessionSndSvrCurState();
			//}
			
			// Start processing I/O on ther accepted socket
			// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
			// Client ���� ������ �޴´�.
			dwRecvNumBytes = sizeof( NET_MSG_GENERIC );
	        
			//IOData = getRecvIO( nClientNumber );
			IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

			if (IOData == NULL)
			{
				CloseClient( (DWORD) nClientNumber );
				continue;
			}

			IOData->dwRcvBytes		= 0;
			IOData->dwTotalBytes	= dwRecvNumBytes;
			// Head receive mode
			m_pClientManager->SetNetMode( (DWORD) nClientNumber, NET_PACKET_HEAD );
			
			// Agent ������ ��ſ��� ��ȣȭ ����
			//if (!m_pClientManager->IsAgentSlot(nClientNumber))
			//	MsgSndCryptKey(nClientNumber); // Send crypt key
					
			::WSARecv(
				Accept,
				&(IOData->DataBuf), 
				1,
				&dwRecvNumBytes,
				&Flags ,
				&(IOData->Overlapped),
				NULL );

			if( !m_bUseEventThread ) Sleep( 0 );
		}
		// ServerAcceptThread Running End
		CConsoleMessage::GetInstance()->Write(_T("ListenProc End"));
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;	
}

namespace PROFILE
{
	void DumpOutputToNon ();
};

int CFieldServer::Update()
{
	// CString strLog;

	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp  CFieldServer::UpdateProc\n"));

		DWORD dwMaxClient       = 0;

		DWORD dwTimeTemp		= timeGetTime();
		DWORD dwTimeS           = timeGetTime();
		DWORD dwFPSUdateTime		= /*3000*/FPS_UPDATE_TIME;

		m_dwHeartBeatStart1 = dwTimeS;
		m_dwHeartBeatStart2 = dwTimeS;
		m_dwHeartBeatTime   = dwTimeS;
		m_dwFrameTime		= dwTimeS;
		m_bUpdateEnd		= false;

		m_dwFrameCount		= 0;

		float fOldAppTime = 0, fAppTime = 0, fElapsTime = 0;
		fOldAppTime = DXUtil_Timer ( TIMER_GETAPPTIME );

		// Event ����� Thread�� ���� ����
		DWORD dwNewWaitTime = 0;
		DWORD dwReturn = 0;
		DWORD dwNewTime = 0;
		DWORD dwUpdateFrameTime = 0;

		// Sleep ����� Thread�� ���� ����
		double fBeforeTime = 0.0f;
		double fCurrentTime = 0.0f;
		double fTimeDelta = 0.0f;

		//	DEBUG : �������ϸ� �ʱ�ȭ.
		PROFILE_INIT();

		
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
				fTimeDelta = fBeforeTime+GLGaeaServer::GetInstance().GetLimitFPS();

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

			PROFILE_BLOCKSTART();
			PROFILE_BEGIN("PROFILE_BLOCKSTART");

			///////////////////////////////////////////////////////////////////////
			// ���� Ŭ���̾�Ʈ ID ����.
			m_pClientManager->ResetPreSleepCID ();

			
			PROFILE_BEGIN("Messge Queue");

			///////////////////////////////////////////////////////////////////////
			// ���� �޽������� ó���Ѵ�.
			RecvMsgProcess();
			PROFILE_END("Messge Queue");
			

			PROFILE_BEGIN("DxFieldInstance::FrameMove");
			{				
				// Update Gaea Server
				HRESULT hr = DxFieldInstance::FrameMove();
				if ( hr!=S_FALSE ) m_dwFrameCount++;
			}
			PROFILE_END("DxFieldInstance::FrameMove");

			PROFILE_BEGIN("Message Send Final");
			{
				m_pClientManager->SendClientFinal();
			}
			PROFILE_END("Message Send Final");

			PROFILE_END("PROFILE_BLOCKSTART");
			PROFILE_BLOCKEND();

			///////////////////////////////////////////////////////////////////////
			// ��Ʈ��Ʈ üũ�� �Ѵ�.
			FieldSrvHeartBeatCheck( dwFPSUdateTime );

			///////////////////////////////////////////////////////////////////////
			// ���� ������ ���¸� ����Ʈ �Ѵ�.
			PrintDebugMsg( dwFPSUdateTime );

			fAppTime    = DXUtil_Timer ( TIMER_GETAPPTIME ); //	���� �ð�.
			fElapsTime  = fAppTime - fOldAppTime;			 //	�޽��� ó�� �ִ� ���� �ð�.
			fOldAppTime = fAppTime;							 //	�����ð� ����.

			

			if ( m_bReservationServerStop )
			{
				m_fReservationtime += fElapsTime;

				if ( m_fReservationtime > fRESERVATIONTIME )
				{
					PostMessage( m_hWnd, WM_FINALCLEANUPFINISH, 0, 0 );
					m_bReservationServerStop = false;
				}
			}
		}

		///////////////////////////////////////////////////////////////////////
		// UpdateThread ����
		EndUpdateThread();
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
		_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;
}

int CFieldServer::UpdateProc()
{
	int returnValue = 0;
	BOOL bUseIntelTBB = GLGaeaServer::GetInstance().GetUseIntelTBB();
	if( bUseIntelTBB )
	{
		// �ڵ����� cpu ���� ������ �κ�
		int threads = tbb::task_scheduler_init::automatic;
		tbb::task_scheduler_init Init(threads);
		returnValue = Update();
	}else{
		returnValue = Update();
	}


	return returnValue;
}

int CFieldServer::WorkProc()
{
	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp  CFieldServer::WorkProc\n"));

		PER_IO_OPERATION_DATA* pPerIoData = NULL;
		DWORD dwSndBytes                  = 0;
		DWORD dwRcvBytes                  = 0;
		DWORD dwByteTrans                 = 0;
		DWORD dwFlags                     = 0;
		DWORD dwClient                    = -1;
		int	  nRetCode                    = 0;
		DWORD nLastError                  = 0;
		
		DWORD dwMaxClient = m_pClientManager->GetMaxClient();
		NET_MSG_GENERIC* pNmg = NULL;

		while (m_bIsRunning)
		{
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
			
			// Illegal Message Skip, �߸��� �޽���
			if ((dwClient < 0) || (dwClient >= dwMaxClient))
			{
				CConsoleMessage::GetInstance()->Write(_T("WorkProc case 1 %d"), dwClient);
				ReleaseOperationData(pPerIoData);
				continue;
			}
			
			/*
			if (nRetCode == 0)
			{
				if (pPerIoData == NULL) // Time to exit, stop thread
				{
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, _T("WorkProc case 2 %d"), dwClient);
					return 0;
				}
				else // �������� PerIoData != NULL && nRetCode == 0 && dwByteTrans == 0
				{
					m_pIOCP->Release(pPerIoData);
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, _T("WorkProc case 3 %d"), dwClient);
					CloseClient(dwClient);				
					continue;
				}
			}
			else // (nRetCode != 0)
			{
				if (pPerIoData == NULL) // Ŭ���̾�Ʈ�� ���� ����������...
				{ 
					// PostQueuedCompletionStatus post an I/O packet with 
					// a NULL CompletionKey (or if we get one for any reason).
					// It is time to exit.
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, _T("WorkProc case 4 %d"), dwClient);
					CloseClient(dwClient);					
					continue;
				}
			}
			*/
			if (nRetCode == 0) // ��������
			{
				// 64 �� �����̸� ������ ����
				nLastError = GetLastError();
				//releaseIO( dwClient, pPerIoData );
				CloseClient( dwClient );
				ReleaseOperationData(pPerIoData);
				continue;
			}

			if (nRetCode != 0 && pPerIoData == NULL)
			{
				CloseClient(dwClient);
				continue;
			}

			
			/*
			// Ŭ���̾�Ʈ�ʿ��� ���������� socket �� close ������...
			if (dwByteTrans == 0 &&
				(pPerIoData->OperationType == NET_RECV_POSTED || 
				pPerIoData->OperationType == NET_SEND_POSTED))
			{
				// Client Closed. Bytes Transferred 0
				m_pIOCP->Release(pPerIoData);
				CloseClient(dwClient);
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, _T("WorkProc case 5 %d"), dwClient);
				continue;
			}
			*/

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
					// Insert to client recv buffer
					m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);
					 
					while (true) {
						pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient,dwClient<NET_RESERVED_SLOT ? FALSE : TRUE);
						if (NULL == pNmg || 0 == dwClient) {
							break;
						} else {
							m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->dwSize);
						}
					}
					//releaseRecvIO( dwClient, pPerIoData ); // OverlappedIO �޸� ��ȯ
					ReleaseOperationData(pPerIoData); // OverlappedIO �޸� ��ȯ
					
					dwRcvBytes = sizeof(NET_MSG_GENERIC);
					//pPerIoData = getRecvIO( dwClient );
					pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

					if (pPerIoData == NULL) continue;
					
					// WSARecv ��û
					nRetCode = ::WSARecv(
									m_pClientManager->GetSocket(dwClient),
									&(pPerIoData->DataBuf), 
									1, 
									&dwRcvBytes,
									&dwFlags,
									&(pPerIoData->Overlapped),
									NULL );
					
					if ((nRetCode == SOCKET_ERROR) && ((nLastError=WSAGetLastError()) != WSA_IO_PENDING))
					{
						//releaseRecvIO( dwClient, pPerIoData );
						ReleaseOperationData(pPerIoData);
						CloseClient(dwClient);
						CConsoleMessage::GetInstance()->Write(
							_T("ERROR:case 8 WorkProc WSARecv Client(%d) ERR(%d)"), 
							dwClient, 
							nLastError );
					}
				}
				break;
			// ���� �Ϸ� �뺸 �϶�...
			case NET_SEND_POSTED :
				dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // �� ���� ����Ʈ��				
				if (dwSndBytes >= pPerIoData->dwTotalBytes) // ���� �Ϸ��
				{	
					//releaseSendIO( dwClient, pPerIoData );
					ReleaseOperationData(pPerIoData);
				}
				else // ���� �Ϸ���� ���� ���� ����Ʈ�� ����
				{
					CConsoleMessage::GetInstance()->Write("ERROR:WorkProc Write Incompleted");
					
					pPerIoData->dwSndBytes  = dwSndBytes; // ���� ����Ʈ�� ������Ʈ					
					pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // �������� ����Ÿ ������ ������Ʈ					
					dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // �������� ����Ʈ�� ������Ʈ					
					pPerIoData->DataBuf.len = (u_long) dwSndBytes; // ���۱��� ������Ʈ
					// ���ۿ�û					
					nRetCode = m_pClientManager->SendClient2(dwClient, pPerIoData, dwSndBytes);
					if (nRetCode == NET_ERROR)
					{
						CloseClient(dwClient);
						CConsoleMessage::GetInstance()->Write(_T("WorkProc case 9 %d"), dwClient);
					}
				}
				break;
			default :
				CloseClient(dwClient);
				CConsoleMessage::GetInstance()->Write(_T("WorkProc case 10 %d"), dwClient);
				break;
			}	
			if( !m_bUseEventThread ) Sleep( 0 );
		}
		// ServerWorkerThread Running End
		CConsoleMessage::GetInstance()->Write("WorkProc End");
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CFieldServer::DatabaseProc()
{
	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp CFieldServer::DatabaseProc\n"));
		while (m_bIsRunning)
		{	
			CDbExecuter::GetInstance()->ExecuteJob();
			if( m_bUseEventThread )	Sleep( 0 );
		}

		TRACE("CFieldServer::DatabaseProc() End \n");
		CConsoleMessage::GetInstance()->Write("== Database Theread stooped");
		return NET_OK;
		
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
int CFieldServer::LogDatabaseProc()
{
	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp CFieldServer::LogDatabaseProc\n"));
		while (m_bIsRunning)
		{	
			CLogDbExecuter::GetInstance()->ExecuteJob();
			if( !m_bUseEventThread ) Sleep( 0 );
		}

		TRACE("CFieldServer::LogDatabaseProc() End \n");
		CConsoleMessage::GetInstance()->Write("== Log Database Theread stopped");
		return NET_OK;
		
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int CFieldServer::UserDatabaseProc()
{
	return 0;
	/*
	__try
	{
		TRACE(_T("in s_CFieldServerThread.cpp CFieldServer::UserDatabaseProc\n"));
		while (m_bIsRunning)
		{	
			CUserDbExecuter::GetInstance()->ExecuteJob();
			Sleep( 1 );
		}

		TRACE("CFieldServer::UserDatabaseProc() End \n");
		CConsoleMessage::GetInstance()->Write("== User Database Theread stopped");
		return NET_OK;
		
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return 0;
	*/
}