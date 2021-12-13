#include "pch.h"
#include "Psapi.h"
#pragma comment( lib, "Psapi.lib" )
//#include "./DbExecuter.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int CAgentServer::ListenProc(void)
{
	__try
	{
		TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::ListenProc\n"));

		LPPER_IO_OPERATION_DATA IOData = NULL;
		SOCKET	Accept;
		DWORD	dwRecvNumBytes = 0;
		DWORD	Flags = 0;
		HANDLE	hRetCode = NULL;
		int		nClientNumber = 0;
		DWORD   dwClient = 0;
		int		nRetCode = 0;

		int		nReachedErrorCnt = 0;
		int		nServerStateLog  = 0;
		bool	bReachedError = FALSE;
		int		nTest = 0;
		m_bReachedMAX = FALSE;

		while (m_bIsRunning)
		{
			Accept = ::WSAAccept(m_sServer, NULL, NULL, NULL, 0);
			// WSAAccept Error
			if (Accept == INVALID_SOCKET) 
			{
				nRetCode = ::WSAGetLastError();
				CConsoleMessage::GetInstance()->Write(_T("ERROR:WSAAccept %d"), nRetCode);
				if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
				{
					break;
				}
				else
				{
					continue;
				}
			}

	
			/////////////////////////////////////////////////////////////////////////////
			sockaddr_in	sAddrIn;
			int nSize = sizeof(sockaddr_in);
			char szIp[MAX_IP_LENGTH+1] = {0};

			
			// Get client ip address and port
			::getpeername(Accept, (sockaddr *) &sAddrIn, &nSize);
			::StringCchCopy(szIp, MAX_IP_LENGTH+1, ::inet_ntoa(sAddrIn.sin_addr));

		

			// Check block ip address
			if (SERVER_UTIL::CAddressChecker::GetInstance()->isBlock(szIp, timeGetTime()) == true)
			{
				//::shutdown(Accept, SB_BOTH);
				::closesocket(Accept);
				continue;
			}
			
			int nType = 0;
			m_bReachedMAX = FALSE;

		

			/////////////////////////////////////////////////////////////////////////////
			// Create per-handle data information structure to associate with the socket

			nClientNumber = m_pClientManager->GetFreeClientID(nType); // Get free client slot number	

			char szTempIp[256];
			strcpy_s( szTempIp, szIp );
			m_pClientManager->ConnectIPCheck( szTempIp );
			if (nClientNumber == NET_ERROR) 
			{
/*
				int nSocketError = 0;
				int nSocketFlag = MSG_DONTROUTE;

				NET_MSG_GENERIC nmg;
				nmg.dwSize = sizeof(NET_MSG_GENERIC);
				nmg.nType = MET_MSG_SVR_FULL;  // 서버 Full

                // 최대 동접인원 도달...
				nSocketError = ::send(Accept,
					                  (const char *) &nmg,
					                  nmg.dwSize,
					                  nSocketFlag);

				if (nSocketError == SOCKET_ERROR)
				{
					CConsoleMessage::GetInstance()->Write(
						                                  _T("ERROR:ListenProc send %d"),
						                                  WSAGetLastError());
				}
*/
				//::shutdown(Accept, SB_BOTH);
				::closesocket(Accept);

				if( nReachedErrorCnt % 500 == 0 )
				{
					CConsoleMessage::GetInstance()->Write( _T("INFO:Reached Max Client Number vecIDSize : %d deqIDSize : %d MaxClientNum : %d CurrentClientNum : %d ReachedMaxCnt : %d"), 
															   m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
															   m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber(), nReachedErrorCnt );
				}

				nReachedErrorCnt++;
				
				m_bReachedMAX = TRUE;
				bReachedError = TRUE;
				continue;
			}
			if( bReachedError )
			{
				CConsoleMessage::GetInstance()->Write( _T("###### INFO:End reached Max Client Number vecIDSize : %d deqIDSize : %d MaxClientNum : %d CurrentClientNum : %d ######"), 
														m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
														m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber() );
				nReachedErrorCnt = 0;
			}

			bReachedError = FALSE;

			if( nServerStateLog >= 500 )
			{
				CConsoleMessage::GetInstance()->Write( _T("LISTEN_SERVER_INFO: vecIDSize %d deqIDSize %d MaxClientNum %d CurrentClientNum %d"),
															m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
															m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber() );
				nServerStateLog = 0;
			}else{
				nServerStateLog++;
			}

			


			//m_bReachedMAX = TRUE;


			dwClient = (DWORD) nClientNumber;

			
			// Set client information
			// 클라이언트의 ip, port, 접속시간을 기록함.
			if (m_pClientManager->SetAcceptedClient(dwClient, Accept) == NET_ERROR)
			{
				//CloseClient( dwClient );
				//::shutdown(Accept, SB_BOTH);
				//int nRetCode = ::closesocket(Accept);
				//if (nRetCode == SOCKET_ERROR)
				//{
				//	nRetCode = ::WSAGetLastError();
				//	CConsoleMessage::GetInstance()->Write(
				//									_T("CNetUser::SetAcceptedClient closesocket %d"),
				//									nRetCode);
				//}
				m_pClientManager->ReleaseClientID( dwClient );
				continue;
			}

			// IP 를 리스트에 등록하고 block Address 인지 검사한다.
			/*
			if (SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient)) == true)
			{
                CloseClient(dwClient);
				continue;
			}
			*/
			// SERVER_UTIL::CAddressChecker::GetInstance()->IsBlock(m_pClientManager->GetClientIP(dwClient));
			
		
			// Associate the accepted socket with the completion port
			hRetCode = ::CreateIoCompletionPort((HANDLE) Accept, 
											    m_hIOServer, 
											    (DWORD) nClientNumber, 
											    0);
			if (hRetCode == NULL) 
			{
				CConsoleMessage::GetInstance()->Write(_T("CreateIoCompletionPort Error"));				
				CloseClient(dwClient);
				continue;
			}

			// 세션서버에 현재 접속자수 전송
			SessionSndSvrCurState();

			// Start processing I/O on ther accepted socket
			// First WSARecv, TCP/IP Send 8 bytes (ignored byte)			
			dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
	        
			//IOData = getRecvIO( dwClient );
			IOData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);

			if (IOData == NULL)
			{
				CloseClient(dwClient);
				continue;
			}
			
			IOData->dwRcvBytes		= 0;
			IOData->dwTotalBytes	= dwRecvNumBytes;
			// Head receive mode
			m_pClientManager->SetNetMode(dwClient, NET_PACKET_HEAD);
			
			// 클라이언트 메시지 받기 준비
			::WSARecv(Accept,					
					  &(IOData->DataBuf), 
					  1,
					  &dwRecvNumBytes,
					  &Flags ,
					  &(IOData->Overlapped),
					  NULL);

			MsgSndCryptKey(dwClient); // Send crypt key
			MsgSndRandomNumber(dwClient); // Send random password key

			// nProtect 서버 인증을 사용한다면 접속후 바로 클라이언트에게 인증용 메시지를 보낸다.
			MsgSndGameGuardFirstCheck( dwClient );			
			
			nClientNumber = NET_ERROR;

			if( !m_bUseEventThread )	
				Sleep( 0 );
		}

		// ServerAcceptThread Running End
		CConsoleMessage::GetInstance()->Write(_T("== ListenProc End"));
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

int CAgentServer::UpdateProc()
{
	__try
	{
		TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::UpdateProc\n"));


		DWORD nServerStateTime  = timeGetTime(); // 서버 상태 로그 기록용 시간 변수
		m_dwHeartBeatTime		= nServerStateTime;
		float fFPSUdateTime		= /*3000*/FPS_UPDATE_TIME;
		m_bUpdateEnd			= false;

		HANDLE hProcess			= GetCurrentProcess(); // 메모리 체크용 변수
		if( hProcess != NULL )
		{				
			PROCESS_MEMORY_COUNTERS pmc;
			if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
			{
				CConsoleMessage::GetInstance()->Write( _T("SERVER_INFO: Memory %d vecIDSize %d deqIDSize %d MaxClientNum %d CurrentClientNum %d"),
					pmc.WorkingSetSize, m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
					m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber() );
			}
		}

		// Event 방식의 Thread를 위한 변수
		DWORD dwNewWaitTime = 0;
		DWORD dwReturn = 0;
		DWORD dwNewTime = 0;
		DWORD dwUpdateFrameTime = 0;

		// Sleeep 방식의 Thread를 위한 변수
		double fBeforeTime = 0.0f;
		double fCurrentTime = 0.0f;
		double fTimeDelta = 0.0f;

		m_VietnamPresetTime = CTime::GetCurrentTime();


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
				//현재 시간을 얻어옴
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

            
			///////////////////////////////////////////////////////////////////////
			// 유휴 클라이언트 ID 정리.
			m_pClientManager->ResetPreSleepCID ();		


			///////////////////////////////////////////////////////////////////////
			// 받은 메시지들을 처리한다.
			RecvMsgProcess();

			///////////////////////////////////////////////////////////////////////
			// Update Gaea Server

			DxAgentInstance::FrameMove();

			m_pClientManager->SendClientFinal();


#if defined( CH_PARAM ) || defined ( HK_PARAM )  || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex 적용
			// Apex 클라이언트 종료
			if( m_nServiceProvider == SP_CHINA || m_nServiceProvider == SP_TAIWAN )
//				|| m_nServiceProvider == SP_MINCOMS )
			{
				ApexCloseClientFinal();
			}
#endif

			///////////////////////////////////////////////////////////////////////
			// 하트비트 체크를 한다.
			AgentSrvHeartBeatCheck( HEARTBEAT_TIME );

			///////////////////////////////////////////////////////////////////////
			// 클라이언트 상태 업데이트 
			UpdateClientState();

			///////////////////////////////////////////////////////////////////////
			// 현재 서버의 상태를 프린트 한다.
			PrintDebugMsg( (DWORD)fFPSUdateTime );

			///////////////////////////////////////////////////////////////////////
			// 서버 상태 로그 기록
			DWORD dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - nServerStateTime) >= SERVER_STATE_CHK_TIME )
			{					
				if( hProcess != NULL )
				{				
					PROCESS_MEMORY_COUNTERS pmc;
					if( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
					{
						CConsoleMessage::GetInstance()->Write( _T("SERVER_INFO: Memory %d vecIDSize %d deqIDSize %d MaxClientNum %d CurrentClientNum %d"),
							pmc.WorkingSetSize, m_pClientManager->m_vecSleepCID.size(), m_pClientManager->m_deqSleepCID.size(),	
							m_pClientManager->GetMaxClient(), m_pClientManager->GetCurrentClientNumber() );
					}
				}
				nServerStateTime = dwCurrentTime;

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


int CAgentServer::WorkProc()
{
	__try
	{
		TRACE(_T("in s_CAgentServerThread.cpp  CAgentServer::WorkProc\n"));

		PER_IO_OPERATION_DATA* pPerIoData = NULL;
		DWORD dwSndBytes   = 0;
		DWORD dwRcvBytes   = 0;
		DWORD dwByteTrans  = 0;
		DWORD Flags        = 0;
		DWORD dwClient     = -1;
		int	  nRetCode     = 0;		
		DWORD dwMaxClient  = m_pClientManager->GetMaxClient();
		DWORD dwMaxClient2 = dwMaxClient*2;
		int   nLastErr(0);
		NET_MSG_GENERIC* pNmg = NULL;

		while (m_bIsRunning)
		{	
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
			
			// Illegal Message Skip. case 1
			if ((dwClient < 0) || (dwClient >= dwMaxClient2))
			{
				ReleaseOperationData(pPerIoData);
				CConsoleMessage::GetInstance()->Write( _T("WorkProc case 1 %d"), dwClient);
				continue;
			}

			/*
			if (nRetCode == 0)
			{	
				if (PerIoData == NULL) // Time to exit, stop thread. case 2
				{
					CConsoleMessage::GetInstance()->Write( _T("WorkProc case 2 %d"), dwClient);
					return 0;
				}
				else // 강제종료 PerIoData != NULL && nRetCode == 0 && dwByteTrans == 0. case 3
				{
					m_pIOCP->Release(PerIoData);
					// CConsoleMessage::GetInstance()->Write( _T("WorkProc case 3 %d"), dwClient);
					// if ( dwClient < dwMaxClient )
					CloseClient(dwClient);
					continue;
				}
			}
			else // (nRetCode != 0)
			{
				if (PerIoData == NULL) // 클라이언트가 강제 종료했을때.... case 4
				{ 
					// PostQueuedCompletionStatus post an I/O packet with 
					// a NULL CompletionKey (or if we get one for any reason).
					// It is time to exit.
					CloseClient(dwClient);
					CConsoleMessage::GetInstance()->Write( _T("WorkProc case 4 %d"), dwClient);
					continue;
				}				
			}
			*/
			
			
			
			if (nRetCode == 0) // 강제종료
			{
				// 64 번 에러이면 무조건 종료
				nLastErr = GetLastError();
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
			if (dwByteTrans == 0)
			{
				if (dwClient < dwMaxClient) // case 5
				{
					CloseClient(dwClient);
					CConsoleMessage::GetInstance()->Write( _T("WorkProc case 5 %d"), dwClient);
				}
				else // case 6
				{
					CConsoleMessage::GetInstance()->Write( _T("WorkProc case 6 %d"), dwClient);
				}
				// Client Closed. Bytes Transferred 0
				m_pIOCP->Release(PerIoData);
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
					// Insert to client buffer
					m_pClientManager->addRcvMsg(dwClient, pPerIoData->Buffer, dwByteTrans);					
					
					while (true) {
						// Get one Message
						pNmg = (NET_MSG_GENERIC*) m_pClientManager->getRcvMsg(dwClient);
						if (NULL == pNmg) {
							break;
						}
						else {
							// Insert to message queue
							m_pRecvMsgManager->MsgQueueInsert(dwClient, pNmg, pNmg->dwSize);
						}
					}

					// OverlappedIO 메모리 반환
					//releaseRecvIO( dwClient, pPerIoData );
					ReleaseOperationData(pPerIoData);
					// WSARecv 요청
					Flags = 0;
					dwRcvBytes = sizeof(NET_MSG_GENERIC);
					//pPerIoData = getRecvIO( dwClient );
					pPerIoData = (LPPER_IO_OPERATION_DATA) GetFreeOverIO(NET_RECV_POSTED);
					
					if (pPerIoData == NULL) 	continue;

					SOCKET sSOCKET = m_pClientManager->GetSocket(dwClient);
					nRetCode = ::WSARecv(
									sSOCKET,
									&(pPerIoData->DataBuf), 
									1, 
									&dwRcvBytes,
									&Flags,
									&(pPerIoData->Overlapped),
									NULL );
					
					if ((nRetCode == SOCKET_ERROR) && ((nLastErr=WSAGetLastError()) != WSA_IO_PENDING))
					{						
						// 치명적 에러, 로그에 기록남김. case 8
						//releaseRecvIO( dwClient, pPerIoData );
						ReleaseOperationData(pPerIoData);
						CloseClient( dwClient );
						CConsoleMessage::GetInstance()->Write( 
							                    _T("ERROR:case 8 WorkProc WSARecv Client(%d) ERR(%d)"), 
												dwClient, 
												nLastErr );
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
					dwSndBytes              = (DWORD) pPerIoData->dwTotalBytes - dwSndBytes; // 보내야할 바이트수 업데이트				
					pPerIoData->DataBuf.len = (u_long) dwSndBytes; // 버퍼길이 업데이트
					
					// 전송요청
					nRetCode = m_pClientManager->Send(dwClient, pPerIoData, dwSndBytes);
					if (nRetCode == NET_ERROR) // case 9
					{
						CloseClient(dwClient);
						CConsoleMessage::GetInstance()->Write( _T("WorkProc case 9 %d"), dwClient);
					}
				}
				break;
			default : // case 10				
				CloseClient(dwClient);
				CConsoleMessage::GetInstance()->Write( _T("WorkProc case 10 %d"), dwClient);
				break;
			}
			if( !m_bUseEventThread ) 
				Sleep( 0 );
		}
		// ServerWorkerThread Running End
		CConsoleMessage::GetInstance()->Write(_T("WorkProc End"));
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

// Database thread function
int CAgentServer::DatabaseProc()
{
	__try
	{
		while (m_bUpdateEnd == false)
		{
			CDbExecuter::GetInstance()->ExecuteJob();
			if( !m_bUseEventThread ) 
				Sleep( 0 );
		}

		TRACE(_T("CAgentServer::DatabaseProc\n"));
		CConsoleMessage::GetInstance()->Write(_T("== Database Thread Stopped"));
		return NET_OK;

	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
		return NET_ERROR;
	}	
}

// Log Database thread function
int CAgentServer::LogDatabaseProc()
{
	__try
	{
		while (m_bUpdateEnd == false)
		{	
			CLogDbExecuter::GetInstance()->ExecuteJob();
			if( !m_bUseEventThread ) 
				Sleep( 0 );
		}

		TRACE(_T("CAgentServer::LogDatabaseProc\n"));
		CConsoleMessage::GetInstance()->Write(_T("== LogDatabase Thread Stopped"));
		return NET_OK;

	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
		return NET_ERROR;
	}
}

// User Database thread function
int CAgentServer::UserDatabaseProc()
{
	__try
	{
		while (m_bUpdateEnd == false)
		{	
			CUserDbExecuter::GetInstance()->ExecuteJob();
			if( !m_bUseEventThread ) 
				Sleep( 0 );
		}

		TRACE(_T("CAgentServer::UserDatabaseProc\n"));
		CConsoleMessage::GetInstance()->Write(_T("== UserDatabase Thread Stopped"));
		return NET_OK;

	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
		return NET_ERROR;
	}
}


// User Database thread function
int CAgentServer::WebDatabaseProc()
{
	__try
	{
		while (m_bUpdateEnd == false)
		{
			CWebDbExecuter::GetInstance()->ExecuteJob();
			if( !m_bUseEventThread )
				Sleep( 0 );
		}

		TRACE(_T("CAgentServer::WebDatabaseProc\n"));
		CConsoleMessage::GetInstance()->Write(_T("== WebDatabase Thread Stopped"));
		return NET_OK;

	}
	__except(RecordExceptionInfo(GetExceptionInformation(), 
				_T("ExceptionAttacher.cpp - AfxWinMain")))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
		return NET_ERROR;
	}
}