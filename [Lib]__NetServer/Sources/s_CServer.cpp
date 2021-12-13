#include "pch.h"
#include <process.h>
#include "s_CServer.h"

// #pragma comment (lib,"Winmm.lib")
// #pragma message ("Auto linking Winmm library")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CServer::CServer(
	HWND hWnd,
	HWND hEditBox,
	HWND hEditBoxInfo )
	: m_hWnd                 (hWnd)
	, m_hEditBox             (hEditBox)
	, m_hEditBoxInfo         (hEditBoxInfo)
	, m_hIOServer		     (NULL)

	, m_hAcceptThread        (NULL)
	, m_hUpdateThread	     (NULL)

	, m_hUpdateQuitEvent     (INVALID_HANDLE_VALUE)
	, m_dwUpdateWaitTime     (0)
	, m_dwUpdateOldTime      (0)
	, m_dwUpdateFrame        (10)

	, m_pRecvIOCP			 (NULL)
	, m_pSendIOCP            (NULL)

	, m_pRecvMsgManager      (NULL)
	, m_sServer		         (INVALID_SOCKET) // Server Socket	
	, m_nStatus		         (S_SERVER_STOP)

	, m_dwCompKey		     (0)
	, m_nPort			     (0)	 // Server port
	, m_nVersion		     (0)     // Server version

	, m_nServerGroup	     (0)     // Server group
	, m_nServerChannel       (0)     // Server Channel
	, m_nServerChannelNumber (1)     // Agent, Server Channel Number
	, m_nServiceProvider     (0)

	, m_dwLastDropClient     (0)
	, m_bIsRunning	         (false)
	, m_bUpdateEnd	         (false)
	, m_bTestServer	         (false)
	, m_bAdult		         (false)
	, m_bPK                  (true)
	, m_bPKLess              (false)	

	, m_bUseEventThread		 (false)

	, m_dwWorkerThreadNumber (2) // Worker Thread 의 갯수
	, m_bGameGuardInit       (false) // nProtect GameGuard 초기화 성공여부
	, m_bGameGuardAuth       (false) // nProtect GameGuard 인증 사용여부	
	, m_dwCpuCount           (0)
	, m_dwLangSet(0)
	, m_dwHeartBeatTime(0)
	, m_dwFrameTime(0)
	, m_dwFrameCount(0)

	, m_bAllowIP( false )
{
	//m_dwSession = NET_SESSION_START;

	// Create Console Manager
	CConsoleMessage::GetInstance()->SetControl(hEditBox, hEditBoxInfo);
	// Set global value
	g_pConsole = CConsoleMessage::GetInstance();
	// Create IP address checker instance
	SERVER_UTIL::CAddressChecker::GetInstance();
	
	// Create Lzo compress library
	CMinLzo::GetInstance().init();
	
	::SecureZeroMemory( m_szAddress, sizeof(char) * (MAX_IP_LENGTH+1) );

	// Set work thread handle to invaild 
	for (int i=0; i < MAX_WORKER_THREAD; i++)
	{
		m_hWorkerThread[i] = INVALID_HANDLE_VALUE;
	}

	NET_InitializeSocket();

	::SecureZeroMemory( m_szEncrypt, sizeof( TCHAR ) * (ENCRYPT_KEY+1) );
}

CServer::~CServer()
{
	CCfg::GetInstance()->ReleaseInstance();
	g_pConsole = NULL;

	// 받기/보내기 버퍼 종료
	SAFE_DELETE(m_pRecvIOCP);
	SAFE_DELETE(m_pSendIOCP);

	// Message Queue 종료
	SAFE_DELETE(m_pRecvMsgManager);
	COdbcManager::GetInstance()->ReleaseInstance();
	CWhiteRock::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->ReleaseInstance();	
	// Clear Winsock2
	NET_CloseSocket();
	// Clear Address Checker
	SERVER_UTIL::CAddressChecker::GetInstance()->ReleaseInstance();
}

// Create CFG file loader and open cfg file
int CServer::StartCfg(BYTE nType)
{	
	int	nRetCode;

	//nRetCode = CCfg::GetInstance()->Load(_T("server.cfg"));

	switch (nType)
	{
	case 0:
		nRetCode = CCfg::GetInstance()->Load(_T("session.cfg"));
		break;
	case 1:
		nRetCode = CCfg::GetInstance()->Load(_T("login.cfg"));
		break;
	case 2:
		nRetCode = CCfg::GetInstance()->Load(_T("field_0.cfg"));
		break;
	case 3:
		nRetCode = CCfg::GetInstance()->Load(_T("agent.cfg"));
		break;
	default:
		nRetCode = CCfg::GetInstance()->Load(_T("server.cfg"));
		break;
	}
	if (nRetCode != 0)
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:CFG file load error"));
		m_bIsRunning = false;
		return NET_ERROR;
	}	

	m_nVersion		   = CCfg::GetInstance()->GetServerVersion();	// 버전저장
	m_nPatchVersion    = CCfg::GetInstance()->GetPatchVersion();	// 패치프로그램 버전
	
	m_nServerGroup	   = CCfg::GetInstance()->GetServerGroup();		// 서버 그룹
	m_nServerNum       = CCfg::GetInstance()->GetServerNumber();    // 서버 번호
	m_nMaxClient	   = CCfg::GetInstance()->GetServerMaxClient(); // 최대 접속자수
	
	// 서버 최대 인원이 올바른지 체크한다.
	if (m_nMaxClient <= (NET_RESERVED_SLOT+30))
	{
		CConsoleMessage::GetInstance()->Write(_T("ERROR:MaxClient > %d"), NET_RESERVED_SLOT+30);
		return NET_ERROR;
	}
	
	m_nServerChannel       = CCfg::GetInstance()->GetServerChannel();          // 서버 채널
	m_nServerChannelNumber = CCfg::GetInstance()->GetServerChannelNumber();    // Agent 가 가진 서버 채널 갯수
	m_nMaxChannelUser      = CCfg::GetInstance()->GetServerChannelMaxClient(); // 채널당 최대 접속자
	
	m_nPort			   = CCfg::GetInstance()->GetServicePort();		// 포트 저장	
	
	m_dwCompKey		   = (DWORD) ((m_nMaxClient * 2) + 1);          // Completion Key
	
	m_bTestServer      = CCfg::GetInstance()->IsTestServer();	
	m_nServiceProvider = CCfg::GetInstance()->GetServiceProvider(); // 서비스제공업자
	m_bAdult           = CCfg::GetInstance()->AdultCheck();         // Server for adult ?
	m_bPK              = CCfg::GetInstance()->IsPKServer();         // PK On, Off
	m_bPKLess          = CCfg::GetInstance()->IsPKLessServer();     // All attack between character is off

	m_bUseEventThread  = CCfg::GetInstance()->IsUserEventThread();

	m_dwLangSet		   = CCfg::GetInstance()->GetLangSet();

	StringCchCopy(m_szAddress, MAX_IP_LENGTH+1, CCfg::GetInstance()->GetServerIP());
	
	switch (m_nServiceProvider)
	{
	case SP_KOREA :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:KOREA"));
		break;
	case SP_TAIWAN :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:TAIWAN"));
		break;
	case SP_CHINA :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:CHINA"));
		break;
	case SP_JAPAN :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:JAPAN"));
		break;
	case SP_TERRA :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:Philippines/Vietnam"));
		break;
	case SP_THAILAND :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:THAILAND"));
		break;
	case SP_GLOBAL:
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:Global Service Platform"));
		break;
	case SP_MALAYSIA :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:MALAYSIA"));
		break;
	case SP_GS :
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:Global Service"));
		break;
	default:
		CConsoleMessage::GetInstance()->Write(_T("Service Provider:MINCOMS KOREA"));
		break;
	}

	return NET_OK;
}

int CServer::StartAllowIPCfg()
{
	int	nRetCode;

	nRetCode = CCfg::GetInstance()->Load( _T("allow_ip.cfg") );
	if (nRetCode != 0)
	{
		CCfg::GetInstance()->SetAllowFileLoad( false );
		//CConsoleMessage::GetInstance()->Write( _T("ERROR:CFG file load error") );
		//m_bIsRunning = false;
		return NET_OK;
	}	
	else
	{
        CCfg::GetInstance()->SetAllowFileLoad( true );
	}

	for( int i = 0; i < CCfg::GetInstance()->GetAllowIPType1Size(); i++ )
	{
		CString strAllowIP;
		CCfg::GetInstance()->GetAllowIPType1( i, strAllowIP );
		m_vecAllowIPType1.push_back( strAllowIP );
	}

	for( int i = 0; i < CCfg::GetInstance()->GetAllowIPType2Size(); i++ )
	{
		CString strAllowIP;
		CCfg::GetInstance()->GetAllowIPType2( i, strAllowIP );
		m_vecAllowIPType2.push_back( strAllowIP );
	}

	for( int i = 0; i < CCfg::GetInstance()->GetAllowIPType3Size(); i++ )
	{
		CString strAllowIP;
		CCfg::GetInstance()->GetAllowIPType3( i, strAllowIP );
		m_vecAllowIPType3.push_back( strAllowIP );
	}

	return NET_OK;
}

// IOCP 리스트를 생성한다.
int CServer::StartIOCPList()
{	
	if (m_pRecvIOCP == NULL) 
	{
		m_pRecvIOCP = new COverlapped(m_nMaxClient);
	} 
	else 
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}

	if (m_pSendIOCP == NULL) 
	{
		m_pSendIOCP = new COverlapped(m_nMaxClient * 9);
	}
	else
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}

	return NET_OK;
}

// Create message queue
int CServer::StartMsgManager()
{
	if (m_pRecvMsgManager == NULL)
	{
		m_pRecvMsgManager = new CSMsgManager(m_nMaxClient * 3);
		return NET_OK;
	}
	else
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
}

// Create I/O completion port
int CServer::StartIOCP(void)
{
	if (m_hIOServer == NULL)
		m_hIOServer = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	if(m_hIOServer == NULL) 
	{
		CConsoleMessage::GetInstance()->Write(_T("CreateIoCompletionPort Error"));
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("CreateIoCompletionPort OK"));
		return NET_OK;
	}
}


// Create IOCP Work Thread
int CServer::StartWorkThread(void)
{
	char szPName[100] = {0};
	char szSName[100] = {0};

	SERVER_UTIL::CSystemInfo pSInfo;

	pSInfo.GetOSName(szSName);
	pSInfo.GetProcessorName(szPName);
	
	// Determine how many processors are on the system
	// Calcurate number of threads	
	CConsoleMessage::GetInstance()->Write(_T("--------------- System Information -------------------"));
	CConsoleMessage::GetInstance()->Write(_T("%s"), szSName);
	CConsoleMessage::GetInstance()->Write(_T("%s %u Mhz"), szPName, pSInfo.GetCpuSpeed());
	CConsoleMessage::GetInstance()->Write(_T("%u System Processor Detected"), pSInfo.GetProcessorNumber());
	CConsoleMessage::GetInstance()->Write(_T("------------------------------------------------------"));
	
	// Work Thread 갯수 결정
	// CPU 갯수 X 2 가 기본값이다.
	m_dwCpuCount = pSInfo.GetProcessorNumber();
	m_dwWorkerThreadNumber = m_dwCpuCount * S_HEURISTIC_NUM; // Processor * 2
	
	// 설정파일의 최대값 보다 크면 설정파일에 적힌 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > (DWORD) CCfg::GetInstance()->GetMaxThread())
	{
		m_dwWorkerThreadNumber = (DWORD) CCfg::GetInstance()->GetMaxThread();
	}

	// 미리 정의된 최대값보다 크면 프로그램의 정의된 최대값으로 제한한다.
	if (m_dwWorkerThreadNumber > MAX_WORKER_THREAD)
	{
		m_dwWorkerThreadNumber = MAX_WORKER_THREAD;
	}

	// Creaate worker threads based on the number of processors	
	DWORD dwThreadId;
	CConsoleMessage::GetInstance()->Write(_T("%d Worker Thread"), m_dwWorkerThreadNumber);
	
	for (DWORD dwCPU=0; dwCPU < m_dwWorkerThreadNumber; dwCPU++)
    { 		
		HANDLE hThread;
		/*
		hThread = ::CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE) CServerWorkProc, 
								this, 
								0, 
								&dwThreadId);
		*/
		hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CServerWorkProc,
								this, 
								0, 
								(unsigned int*) &dwThreadId);
		if (hThread == NULL)  
		{ 
			CConsoleMessage::GetInstance()->Write(_T("%d Worker Thread Create Error code : %d"), dwCPU+1, GetLastError());
			return NET_ERROR;
        } 
		else 
		{
			CConsoleMessage::GetInstance()->Write(_T("%d st Worker Thread Create OK"), dwCPU+1);
			// The system schedules threads on their preferred processors whenever possible.
			::SetThreadIdealProcessor(hThread, dwCPU % m_dwCpuCount);
        }		
		// store thread handle
		m_hWorkerThread[dwCPU] = hThread;
		::CloseHandle(hThread);
	} 
	return NET_OK;
}


void CServer::setUpdateFrame( DWORD dwFrame )
{
	m_dwUpdateFrame = dwFrame;
	m_dwUpdateWaitTime = 1000/m_dwUpdateFrame;
}

// Start game update thread	
int	CServer::StartUpdateThread()
{
	SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );

	if( m_bUseEventThread )	m_hUpdateQuitEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	DWORD	dwThreadId = 0;	

	m_hUpdateThread = (HANDLE) ::_beginthreadex(
		NULL,
		0,
		CServerUpdateProc,
		this,
		CREATE_SUSPENDED,
		(unsigned int*) &dwThreadId );

	if ( m_hUpdateThread == NULL )
	{ 
		CConsoleMessage::GetInstance()->Write(_T("Server Update Thread Create Failed Error code : %d"), GetLastError()); 
		m_bIsRunning = false;

		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );

		return NET_ERROR;
	} 
	else 
	{
		if (m_dwCpuCount > 2)
		{
			// Update Thread 의 수행레벨을 높여 성능을 향상시킨다.
			// 서버의 CPU 갯수가 2개 이상일때만 실제로 효과가 있었다.
			::SetThreadPriority(m_hUpdateThread, THREAD_PRIORITY_HIGHEST);
		}
		CConsoleMessage::GetInstance()->Write(_T("Server Update Thread Create OK"));

		if( m_bUseEventThread )	
		{
			setUpdateFrame( 100 );
			m_dwUpdateOldTime = ::GetTickCount();
		}

		::ResumeThread( m_hUpdateThread );

		return NET_OK;
	}
}

int	CServer::StartDatabaseThread()
{
	HANDLE hThread;
	DWORD dwThreadId;
	/*
	hThread = ::CreateThread(NULL, 
							0, 
							(LPTHREAD_START_ROUTINE) CServerDatabaseProc, 
							this, 
							0, 
							&dwThreadId);
	*/
	hThread = (HANDLE) ::_beginthreadex(
							NULL, 
							0, 
							CServerDatabaseProc,
							this,
							0,
							(unsigned int*) &dwThreadId );
	if (hThread == NULL)  
	{
		CConsoleMessage::GetInstance()->Write(_T("Database Thread Create Error code : %d"), GetLastError());
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(_T("Database Thread Create OK"));
	}
	::CloseHandle(hThread);
	return NET_OK;
}

// Log database thread function
int	CServer::StartLogDatabaseThread()
{
	HANDLE hThread;
	DWORD dwThreadId;
	/*
	hThread = ::CreateThread(NULL, 
							0, 
							(LPTHREAD_START_ROUTINE) CServerLogDatabaseProc, 
							this, 
							0, 
							&dwThreadId);
	*/
	hThread = (HANDLE) ::_beginthreadex(
							NULL, 
							0, 
							CServerLogDatabaseProc,
							this,
							0,
							(unsigned int*) &dwThreadId );
	if (hThread == NULL)  
	{ 
		CConsoleMessage::GetInstance()->Write(_T("Log Database Thread Create Error code : %d"), GetLastError());
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("Log Database Thread Create OK"));
	}
	::CloseHandle(hThread);
	return NET_OK;
}

// User database thread function
int	CServer::StartUserDatabaseThread()
{
	HANDLE hThread;
	DWORD dwThreadId;
	/*
	hThread = ::CreateThread(NULL, 
							 0, 
							 (LPTHREAD_START_ROUTINE) CServerUserDatabaseProc, 
							 this, 
							 0, 
							 &dwThreadId);
	*/
	hThread = (HANDLE) ::_beginthreadex(
							NULL, 
							0, 
							CServerUserDatabaseProc, 
							this, 
							0, 
							(unsigned int*) &dwThreadId );
	if (hThread == NULL)
	{ 
		CConsoleMessage::GetInstance()->Write(_T("User Database Thread Create Error code : %d"), GetLastError());
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("User Database Thread Create OK"));
	}
	::CloseHandle(hThread);
	return NET_OK;
}

// Web database thread function
int	CServer::StartWebDatabaseThread()
{
	HANDLE hThread;
	DWORD dwThreadId;

	hThread = (HANDLE) ::_beginthreadex(
							NULL, 
							0, 
							CServerWebDatabaseProc, 
							this, 
							0, 
							(unsigned int*) &dwThreadId );
	if (hThread == NULL)
	{ 
		CConsoleMessage::GetInstance()->Write(_T("Web Database Thread Create Error code : %d"), GetLastError());
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("Web Database Thread Create OK"));
	}
	::CloseHandle(hThread);
	return NET_OK;
}

int	CServer::StartListenThread()
{
	int nRetCode;
	DWORD	dwThreadId;
	///////////////////////////////////////////////////////////////////////////
	// Prepare server socket for TCP
	m_sServer = ::WSASocket(
					PF_INET,
					SOCK_STREAM,
					IPPROTO_TCP,
					NULL,
					0,
					WSA_FLAG_OVERLAPPED );
	if(m_sServer == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(_T("CServer::StartListenThread WSASocket failed. Error code : %d"), WSAGetLastError());
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("WSASocket OK"));
	}
	
	// ////////////////////////////////////////////////////////////////////////////
	// Set the socket option
    // Disable send buffering on the socket.  Setting SO_SNDBUF 
    // to 0 causes winsock to stop bufferring sends and perform 
    // sends directly from our buffers, thereby reducing CPU usage. 
	// Winsock 의 버퍼링을 줄이고 cpu 사용량을 줄이려면 아래 옵션을 활성하 시키면 됨.
	// Set SO_SNDBUF to 0
	// Set SO_RCVBUF to 0
	// ///////////////////////////////////////////////////////////////////////////
	
	// ///////////////////////////////////////////////////////////////////////////
    /*
	int nZero = 0; 
	::setsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char *) &nZero, sizeof(nZero)); 
	::setsockopt(m_sServer, SOL_SOCKET, SO_RCVBUF, (char *) &nZero, sizeof(nZero)); 
	*/
	// ///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////
	// Set the exclusive address option, must be called before bind()	
	/*
	int optval = 1;
	int rc = ::setsockopt(m_sServer, 
					SOL_SOCKET, 
					SO_EXCLUSIVEADDRUSE,
					(char *) &optval, 
					sizeof(optval));
	if (rc == SOCKET_ERROR)
	{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, 
				"CServer::StartListenThread setsockopt SO_EXCLUSIVEADDRUSE Error(%d)", 
				::WSAGetLastError());
			return NET_ERROR;
	}
	*/
	
	//////////////////////////////////////////////////////////////////////////////
	// bind socket
	// 바인드될 로칼어드레스, 2개 이상의 랜카드일때..
	// 주의 : Ipv4 에 대응, ipv6 으로 변경시 코딩변경 필수~!
	struct sockaddr_in local;
	// char szAddress[20]; 
	// BOOL bInterface = false;	
	
	//if (bInterface) 
	//{
	//	local.sin_addr.s_addr = inet_addr(szAddress);
	//	if (local.sin_addr.s_addr == INADDR_NONE)
	//		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "inet_addr error");
	//	//	
	//} 
	//else 
	//{
	//	local.sin_addr.s_addr = htonl(INADDR_ANY);
	//}

	
	// The inet_addr function converts a string 
	// containing an (Ipv4) Internet Protocol dotted address 
	// into a proper address for the IN_ADDR structure.
	const char* a = CCfg::GetInstance()->GetServerIP();

	local.sin_addr.s_addr = ::inet_addr(CCfg::GetInstance()->GetServerIP());
	
	if (local.sin_addr.s_addr == INADDR_NONE)
	{
		CConsoleMessage::GetInstance()->Write(_T("inet_addr error"));
		local.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}

	local.sin_family = AF_INET;
	local.sin_port = ::htons(m_nPort);

	nRetCode = ::bind(m_sServer, (struct sockaddr *)&local, sizeof(local));
	if (nRetCode == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("bind Error. Error Code:%d"),
											  WSAGetLastError());
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;		
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("Bind OK"));
	}

	//////////////////////////////////////////////////////////////////////////////
	// Server 의 ip Address 를 얻는다.
	char szHostName[128] = {0};	
	if (::gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;
		int i; 
		pHost = ::gethostbyname(szHostName); 
		for (i=0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
 		{
			char szIP[MAX_IP_LENGTH+1] = "";
 			int j; 
 			for (j=0; j<pHost->h_length; j++)
 			{
				char addr[MAX_IP_LENGTH+1] = {0};
 				if (j>0)
					::StringCchCat(szIP, MAX_IP_LENGTH, "."); 
				::StringCchPrintf(addr, MAX_IP_LENGTH, _T("%u"), (unsigned int)((unsigned char*) pHost->h_addr_list[i])[j]); 				
				::StringCchCat(szIP, MAX_IP_LENGTH, addr);
 			}
 		}
	}
	CConsoleMessage::GetInstance()->Write(_T("%s:%d"), m_szAddress, m_nPort);

	/*
	LINGER      lingerStruct;     
	lingerStruct.l_onoff = 1; 
	lingerStruct.l_linger = 0;
	nRetCode = ::setsockopt(m_sServer, SOL_SOCKET, SO_LINGER, 
				(char *) &lingerStruct, sizeof(lingerStruct) ); 
	if (SOCKET_ERROR == nRetCode)  
	{         
		m_bIsRunning = false;
		return NET_ERROR;
	}
	*/

	// Prepare socket for listening
	nRetCode = ::listen(m_sServer, NET_CLIENT_LISTEN);
//	nRetCode = ::listen(m_sServer, SOMAXCONN );
	if (nRetCode == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("Listen error. Error code:%d"),
											  WSAGetLastError());
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = false;
		return NET_ERROR;
	}	
	
	//////////////////////////////////////////////////////////////////////////////
	// Create Listen (Accept) Thread
	/*
	m_hAcceptThread = ::CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE) CServerListenProc, 
								this, 
								0, &dwThreadId);
	*/
	m_hAcceptThread = (HANDLE) ::_beginthreadex(
									NULL, 
									0, 
									CServerListenProc,
									this,
									0,
									(unsigned int*) &dwThreadId );


	if (m_hAcceptThread == NULL)  
	{ 
		CConsoleMessage::GetInstance()->Write(_T("Server Accept Thread Create Failed Error code : %d"), GetLastError()); 
		m_bIsRunning = false;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(_T("Server Accept Thread Create OK"));
		return NET_OK;
	}
}


int	CServer::StopListenThread()
{	
	// Close socket, will close Listen Thread	
	int nRetCode = ::closesocket(m_sServer);
	
	CloseHandle( m_hAcceptThread );
	m_hAcceptThread = NULL;

	if (nRetCode == SOCKET_ERROR)
	{
		TRACE("StopListenThread closesocket ERROR \n");
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Delete iocp queue
int CServer::StopIOCPList()
{	
	int nCount = 0;
	if (m_pRecvIOCP)
	{
		nCount = m_pRecvIOCP->GetUseCount();
		for (int i=0; i<nCount; ++i) 
		{
			::PostQueuedCompletionStatus(m_hIOServer,
								0,
								m_dwCompKey, 
								NULL);
		}
		Sleep( 1000 );
	}
	if (m_pSendIOCP)
	{
		nCount = m_pSendIOCP->GetUseCount();
		for (int i=0; i<nCount; i++) 
		{
			::PostQueuedCompletionStatus(m_hIOServer,
								0,
								m_dwCompKey, 
								NULL);
		}
		Sleep( 1000 );
	}
	return NET_OK;
}

// Stop Work Thread	
int CServer::StopWorkThread()
{
	for (DWORD dwNum=0; dwNum<m_dwWorkerThreadNumber; dwNum++)
	{
		::PostQueuedCompletionStatus(m_hIOServer, 
							0, 
							m_dwCompKey, 
							NULL);
	}
	// Wait until all worker thread exit...	
	Sleep( 1000 );
	return NET_OK;
}

int	CServer::StopIOCP()
{	
	DWORD dwExitCode;
	if (m_hIOServer) 
	{
		dwExitCode = ::WaitForSingleObject(m_hIOServer, 2000);
		if (dwExitCode == WAIT_FAILED)
			::CloseHandle(m_hIOServer);
		m_hIOServer = NULL;
	}
	return NET_OK;
}

void CServer::ReleaseOperationData(PER_IO_OPERATION_DATA* pData)
{
	if (pData == NULL) return;

	if (NET_SEND_POSTED == pData->OperationType)
	{
		m_pSendIOCP->Release(pData);
	}
	else
	{
		m_pRecvIOCP->Release(pData);
	}	
}

LPPER_IO_OPERATION_DATA CServer::GetFreeOverIO(int nType)
{
    if (NET_SEND_POSTED == nType)
	{
		return m_pSendIOCP->GetFreeOverIO(nType);
	}
	else
	{
		return m_pRecvIOCP->GetFreeOverIO(nType);
	}
}

