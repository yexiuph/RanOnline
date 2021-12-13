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
			// 클라이언트의 ip, port, 접속시간을 기록함.
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
			
			// Block 된 주소라면
			// 연결을 종료시킨다.
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
				// 세션서버에 현재 접속자수 전송
				SessionSndSvrCurState();
			//}
			
			// Start processing I/O on ther accepted socket
			// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
			// Client 버전 정보를 받는다.
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
			
			// Agent 서버와 통신에는 암호화 제거
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

		// Event 방식의 Thread를 위한 변수
		DWORD dwNewWaitTime = 0;
		DWORD dwReturn = 0;
		DWORD dwNewTime = 0;
		DWORD dwUpdateFrameTime = 0;

		// Sleep 방식의 Thread를 위한 변수
		double fBeforeTime = 0.0f;
		double fCurrentTime = 0.0f;
		double fTimeDelta = 0.0f;

		//	DEBUG : 프로파일링 초기화.
		PROFILE_INIT();

		
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
				fTimeDelta = fBeforeTime+GLGaeaServer::GetInstance().GetLimitFPS();

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

			PROFILE_BLOCKSTART();
			PROFILE_BEGIN("PROFILE_BLOCKSTART");

			///////////////////////////////////////////////////////////////////////
			// 유휴 클라이언트 ID 정리.
			m_pClientManager->ResetPreSleepCID ();

			
			PROFILE_BEGIN("Messge Queue");

			///////////////////////////////////////////////////////////////////////
			// 받은 메시지들을 처리한다.
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
			// 하트비트 체크를 한다.
			FieldSrvHeartBeatCheck( dwFPSUdateTime );

			///////////////////////////////////////////////////////////////////////
			// 현재 서버의 상태를 프린트 한다.
			PrintDebugMsg( dwFPSUdateTime );

			fAppTime    = DXUtil_Timer ( TIMER_GETAPPTIME ); //	현재 시간.
			fElapsTime  = fAppTime - fOldAppTime;			 //	메시지 처리 최대 지연 시간.
			fOldAppTime = fAppTime;							 //	이전시간 저장.

			

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
		// UpdateThread 종료
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
		// 자동으로 cpu 갯수 얻어오는 부분
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
			
			// 서버를 종료하기 위해서 종료처리 루틴을 호출했을때...
			if (dwClient == m_dwCompKey && pPerIoData == NULL && dwByteTrans == 0)
			{				
				break;
			}
			
			// Illegal Message Skip, 잘못된 메시지
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
				else // 강제종료 PerIoData != NULL && nRetCode == 0 && dwByteTrans == 0
				{
					m_pIOCP->Release(pPerIoData);
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, _T("WorkProc case 3 %d"), dwClient);
					CloseClient(dwClient);				
					continue;
				}
			}
			else // (nRetCode != 0)
			{
				if (pPerIoData == NULL) // 클라이언트가 강제 종료했을때...
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
			if (nRetCode == 0) // 강제종료
			{
				// 64 번 에러이면 무조건 종료
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
			// 클라이언트쪽에서 정상적으로 socket 을 close 했을때...
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

			if (dwByteTrans == 0) // 정상종료
			{
				//releaseIO( dwClient, pPerIoData );
				CloseClient( dwClient );
				ReleaseOperationData(pPerIoData);
				continue;
			}

			switch (pPerIoData->OperationType)
			{
			// 읽기 완료 통보일때...
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
					//releaseRecvIO( dwClient, pPerIoData ); // OverlappedIO 메모리 반환
					ReleaseOperationData(pPerIoData); // OverlappedIO 메모리 반환
					
					dwRcvBytes = sizeof(NET_MSG_GENERIC);
					//pPerIoData = getRecvIO( dwClient );
					pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

					if (pPerIoData == NULL) continue;
					
					// WSARecv 요청
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
			// 쓰기 완료 통보 일때...
			case NET_SEND_POSTED :
				dwSndBytes = dwByteTrans + pPerIoData->dwSndBytes; // 총 보낸 바이트수				
				if (dwSndBytes >= pPerIoData->dwTotalBytes) // 쓰기 완료됨
				{	
					//releaseSendIO( dwClient, pPerIoData );
					ReleaseOperationData(pPerIoData);
				}
				else // 쓰기 완료되지 않음 남은 바이트를 전송
				{
					CConsoleMessage::GetInstance()->Write("ERROR:WorkProc Write Incompleted");
					
					pPerIoData->dwSndBytes  = dwSndBytes; // 보낸 바이트수 업데이트					
					pPerIoData->DataBuf.buf = pPerIoData->Buffer + dwSndBytes; // 보내야할 데이타 포인터 업데이트					
					dwSndBytes              = pPerIoData->dwTotalBytes - dwSndBytes; // 보내야할 바이트수 업데이트					
					pPerIoData->DataBuf.len = (u_long) dwSndBytes; // 버퍼길이 업데이트
					// 전송요청					
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