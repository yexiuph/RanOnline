//! 2002.05.30 jgkim Create
//! Copyright (c) Min Communications. All rights reserved.
#include "pch.h"
//#include "./DbExecuter.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// #import "EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	IEGameEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#elif defined ( MY_PARAM )
	IMyRossoEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#endif

CAgentServer* g_pAgentServer = NULL;

CAgentServer::CAgentServer(
	HWND hWnd,
	HWND hEditBox,
	HWND hEditBoxInfo )
	: CServer(
		hWnd,
		hEditBox,
		hEditBoxInfo )
	, m_pClientManager (NULL)
	, m_hCheckStrDLL( NULL )
	, m_pCheckString( NULL ) 
{
	m_bHeartBeat = false;

	for (int i=0; i<MAX_CHANNEL_NUMBER; ++i)
	{
		m_ChannelUser[i] = 0;
	}
}

CAgentServer::~CAgentServer()
{
	SAFE_DELETE( m_pClientManager );
}

int	CAgentServer::StartClientManager()
{	
	// Ŭ���̾�Ʈ �������� Ŭ���� ����
	SAFE_DELETE(m_pClientManager);	
	//m_pClientManager = new CClientAgent(
	//						m_nMaxClient,
	//						m_hIOServer );
	m_pClientManager = new CClientAgent(m_nMaxClient, m_pSendIOCP, m_pRecvIOCP, m_hIOServer);

	if (!m_pClientManager)
	{
		m_bIsRunning = false;
		return NET_ERROR;
	}
	return NET_OK;
}

int	CAgentServer::StartDbManager()
{
	int nRetCode;
	///////////////////////////////////////////////////////////////////////////
	// Create DB manager and Open DB
	CDbExecuter::GetInstance()->SetServer( (CServer*) this );
	CLogDbExecuter::GetInstance()->SetServer( (CServer*) this );
	CUserDbExecuter::GetInstance()->SetServer( (CServer*) this );
	CWebDbExecuter::GetInstance()->SetServer( (CServer*) this );

	COdbcManager::GetInstance()->SetServiceProvider(m_nServiceProvider);

	// User DB
	nRetCode = COdbcManager::GetInstance()->OpenUserDB(CCfg::GetInstance()->GetUserOdbcName(),
													   CCfg::GetInstance()->GetUserOdbcUser(),
													   CCfg::GetInstance()->GetUserOdbcPass(),
													   CCfg::GetInstance()->GetUserOdbcDatabase(),
													   CCfg::GetInstance()->GetUserOdbcPoolSize(),
													   CCfg::GetInstance()->GetUserOdbcResponseTime());
	if (nRetCode == DB_ERROR)
	{
		//DB_Shutdown();
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:User DB Open Error") );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("User DB Open OK") );
	}
	
	// Game DB
	nRetCode = COdbcManager::GetInstance()->OpenGameDB(CCfg::GetInstance()->GetGameOdbcName(),
													   CCfg::GetInstance()->GetGameOdbcUser(),
													   CCfg::GetInstance()->GetGameOdbcPass(),
													   CCfg::GetInstance()->GetGameOdbcDatabase(),
													   CCfg::GetInstance()->GetGameOdbcPoolSize(),
													   CCfg::GetInstance()->GetGameOdbcResponseTime());
	if (nRetCode == DB_ERROR)
	{
		//DB_Shutdown();
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Game DB Open Error") );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("Game DB Open OK") );
	}	

	// Log DB
	nRetCode = COdbcManager::GetInstance()->OpenLogDB(CCfg::GetInstance()->GetLogOdbcName(),
													  CCfg::GetInstance()->GetLogOdbcUser(),
													  CCfg::GetInstance()->GetLogOdbcPass(),
													  CCfg::GetInstance()->GetLogOdbcDatabase(),
													  CCfg::GetInstance()->GetLogOdbcPoolSize(),
													  CCfg::GetInstance()->GetLogOdbcResponseTime());
	if (nRetCode == DB_ERROR)
	{
		//DB_Shutdown();
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Log DB Open Error") );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("Log DB Open OK") );
	}

	// Shop DB
	nRetCode = COdbcManager::GetInstance()->OpenShopDB(CCfg::GetInstance()->GetShopOdbcName(),
													   CCfg::GetInstance()->GetShopOdbcUser(),
													   CCfg::GetInstance()->GetShopOdbcPass(),
													   CCfg::GetInstance()->GetShopOdbcDatabase(),
													   CCfg::GetInstance()->GetShopOdbcPoolSize(),
													   CCfg::GetInstance()->GetShopOdbcResponseTime());

	if (nRetCode == DB_ERROR)
	{
		//DB_Shutdown();
		CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Shop DB Open Error") );
		m_bIsRunning = false;
		return NET_ERROR;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase( _T("Shop DB Open OK") );
	}

	// Terra User Database (�ʸ���/��Ʈ��)	
	if (m_nServiceProvider == SP_TERRA)
	{
		nRetCode = COdbcManager::GetInstance()->OpenTerraDB(CCfg::GetInstance()->GetTerraOdbcName(),
													        CCfg::GetInstance()->GetTerraOdbcUser(),
													        CCfg::GetInstance()->GetTerraOdbcPass(),
													        CCfg::GetInstance()->GetTerraOdbcDatabase(),
													        CCfg::GetInstance()->GetTerraOdbcPoolSize(),
													        CCfg::GetInstance()->GetTerraOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Terra DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Terra DB Open OK") );
		}		
	}

	if (m_nServiceProvider == SP_MALAYSIA)
	{
		nRetCode = COdbcManager::GetInstance()->OpenTerraDB(CCfg::GetInstance()->GetTerraOdbcName(),
													        CCfg::GetInstance()->GetTerraOdbcUser(),
													        CCfg::GetInstance()->GetTerraOdbcPass(),
													        CCfg::GetInstance()->GetTerraOdbcDatabase(),
													        CCfg::GetInstance()->GetTerraOdbcPoolSize(),
													        CCfg::GetInstance()->GetTerraOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Terra DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Terra DB Open OK") );
		}

		nRetCode = COdbcManager::GetInstance()->OpenMyDB(CCfg::GetInstance()->GetMyOdbcName(),
													        CCfg::GetInstance()->GetMyOdbcUser(),
													        CCfg::GetInstance()->GetMyOdbcPass(),
													        CCfg::GetInstance()->GetMyOdbcDatabase(),
													        CCfg::GetInstance()->GetMyOdbcPoolSize(),
													        CCfg::GetInstance()->GetMyOdbcResponseTime());

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:My DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("My DB Open OK") );
		}
	}

	// GSP User Database
	if (m_nServiceProvider == SP_GLOBAL)
	{
		nRetCode = COdbcManager::GetInstance()->OpenGspDB(
													CCfg::GetInstance()->GetGspOdbcName(),
													CCfg::GetInstance()->GetGspOdbcUser(),
													CCfg::GetInstance()->GetGspOdbcPass(),
													CCfg::GetInstance()->GetGspOdbcDatabase(),
													CCfg::GetInstance()->GetGspOdbcPoolSize(),
													CCfg::GetInstance()->GetGspOdbcResponseTime() );

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Gsp DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Gsp DB Open OK") );
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

	// Korea Web User DataBase ( Korea )	
	if (m_nServiceProvider == SP_KOREA)
	{
		nRetCode = COdbcManager::GetInstance()->OpenKorDB(
													CCfg::GetInstance()->GetKorOdbcName(),
													CCfg::GetInstance()->GetKorOdbcUser(),
													CCfg::GetInstance()->GetKorOdbcPass(),
													CCfg::GetInstance()->GetKorOdbcDatabase(),
													CCfg::GetInstance()->GetKorOdbcPoolSize(),
													CCfg::GetInstance()->GetKorOdbcResponseTime() );

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Kor DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Kor DB Open OK") );
		}
	}

	if ( m_nServiceProvider == SP_GS ) 
	{
/*
		nRetCode = COdbcManager::GetInstance()->OpenKorDB(
													CCfg::GetInstance()->GetKorOdbcName(),
													CCfg::GetInstance()->GetKorOdbcUser(),
													CCfg::GetInstance()->GetKorOdbcPass(),
													CCfg::GetInstance()->GetKorOdbcDatabase(),
													CCfg::GetInstance()->GetKorOdbcPoolSize(),
													CCfg::GetInstance()->GetKorOdbcResponseTime() );

		if (nRetCode == DB_ERROR)
		{
			//DB_Shutdown();
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Kor DB Open Error") );
			m_bIsRunning = false;
			return NET_ERROR;
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("Kor DB Open OK") );
		}
*/
	}

	if (StartDatabaseThread()     == NET_ERROR) return NET_ERROR;
	if (StartLogDatabaseThread()  == NET_ERROR) return NET_ERROR;
	if (StartUserDatabaseThread() == NET_ERROR) return NET_ERROR;
	if (StartWebDatabaseThread()  == NET_ERROR) return NET_ERROR;

	return NET_OK;
}

int CAgentServer::Start()
{
	int nRetCode = 0;
	HRESULT hResult = S_OK;

	m_bIsRunning = true;

	// ./cfg/server.cfg ������ �о� ���δ�.
	// ���� �ʱ�ȭ�� �����Ѵ�.
	if (StartCfg(3) == NET_ERROR) return NET_ERROR;

	// �� ������ �α��ο� ���õ� ��Ʈ�ѵ��� �ʱ�ȭ �Ѵ�.
	switch (m_nServiceProvider)
	{
	case SP_KOREA :
		// �������� ����Ű �ʱ�ȭ
		m_DaumAuth.Init("Ran72pCsA1yXge3psD63Uu0o3GqpAzyBjm7pcyeODYKefE37fNadf2ng34ejiPja");
		break;
	case SP_JAPAN : 
		// Excite Japan, ��ȣŰ �ʱ�ȭ
//		m_MinMd5.ExciteSetSecretKey("XQ1ftLGWZ");
		if ( !(CWhiteRock::GetInstance()->SetWhiteRockXML()) )
		{
			CConsoleMessage::GetInstance()->WriteFile(
													_T("INFO:WhiteRock XML File Load Failed") );
			return NET_ERROR;
		}
		// CFG ���Ͽ� �����ص� WhiteRock Server�̸��� Port ����
		CWhiteRock::GetInstance()->SetWhiteRockInfo( CCfg::GetInstance()->GetWhiteRockName(), CCfg::GetInstance()->GetWhiteRockPort() );
		CConsoleMessage::GetInstance()->WriteFile(
			_T("WhiteRock Address : %s, Port : %d"),CWhiteRock::GetInstance()->m_sWhiteRockName.GetString(), CWhiteRock::GetInstance()->m_nWhiteRockPort );
		break;
	case SP_TERRA :
		// EGameEncrypt.dll �ʱ�ȭ
		CoInitialize ( NULL );
#if defined ( PH_PARAM ) || defined ( VN_PARAM ) 
		hResult = m_pMyRossoEncrypto.CreateInstance(__uuidof(EGameEncrypt));
#endif

		if (FAILED(hResult))
		{
			CConsoleMessage::GetInstance()->WriteFile( _T("ERROR:m_pMyRossoEncrypto.CreateInstance FAILED") );
			return NET_ERROR;
		} // if (FAILED(hResult))
		else
		{
			CConsoleMessage::GetInstance()->WriteFile( _T("INFO:m_pMyRossoEncrypto CreateInstance SUCCESS") );
		}
		break;

	case SP_MALAYSIA:
		// Malaysia (TERRA) / EGameEncrypt.dll �ʱ�ȭ
		CoInitialize ( NULL );
#if defined ( MY_PARAM )
		hResult = m_pMyRossoEncrypto.CreateInstance(__uuidof(MyRossoEncrypt));
#endif

		if (FAILED(hResult))
		{
			CConsoleMessage::GetInstance()->WriteFile( _T("ERROR:m_pMyRossoEncrypto.CreateInstance FAILED") );
			return NET_ERROR;
		} // if (FAILED(hResult))
		else
		{
			CConsoleMessage::GetInstance()->WriteFile( _T("INFO:m_pMyRossoEncrypto CreateInstance SUCCESS") );
		}
		break;
	case SP_CHINA:
	case SP_TAIWAN:
		{
#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) // || defined ( _RELEASED ) // Apex ����
			m_ApexClose.clear();

			g_pAgentServer = this;
			APEX_SERVER::StartApexProxy();
#endif
		}
		break;
	case SP_GS:
		break;
	default :
		break;
	} // switch (m_nServiceProvider)

#ifdef TH_PARAM
	{
		m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

		if ( m_hCheckStrDLL )
		{
			m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
		}
	}
#endif

	// nProtect GameGuard dll �� �ʱ�ȭ �Ѵ�.
	DWORD dwGGErrCode = ERROR_SUCCESS;
	dwGGErrCode = InitGameguardAuth ("", 1000, FALSE, 0x03);
	if (dwGGErrCode != ERROR_SUCCESS)
	{	
		m_bGameGuardInit = false;
		CConsoleMessage::GetInstance()->WriteFile(
			_T("INFO:nProtect GameGuard server DLL load failed") );
	}
	else
	{
		m_bGameGuardInit = true;		
		CConsoleMessage::GetInstance()->WriteFile(
			_T("INFO:nProtect GameGuard server DLL load success") );
		m_bGameGuardAuth = CCfg::GetInstance()->GemeGuardAuth();
		if (m_bGameGuardAuth == true)
		{
			CConsoleMessage::GetInstance()->WriteFile(
				_T("INFO:nProtect GameGuard Auth ON") );
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteFile(
				_T("INFO:nProtect GameGuard Auth OFF") );
		}
	}
	
	// ä�� ������ �ùٸ��� üũ�Ѵ�.
	if (m_nMaxClient != (m_nMaxChannelUser * m_nServerChannelNumber))
	{
		CConsoleMessage::GetInstance()->WriteFile(
			_T("ERROR:MaxChannelUser X ServerChannelNumber = MaxClient") );
		return NET_ERROR;
	}
	
	if (StartIOCPList()		== NET_ERROR) return NET_ERROR;    	
	if (StartMsgManager()	== NET_ERROR) return NET_ERROR;
	if (StartIOCP()			== NET_ERROR) return NET_ERROR;
	if (StartClientManager()== NET_ERROR) return NET_ERROR;	
	if (StartDbManager()    == NET_ERROR) return NET_ERROR;
	if (StartWorkThread()	== NET_ERROR) return NET_ERROR;

	///////////////////////////////////////////////////////////////////////////
	//	Note : �� ����Ʈ �о��.
	DWORD dwMaxClient = CCfg::GetInstance()->GetMaxClient();
	F_SERVER_INFO* pFieldInfo = CCfg::GetInstance()->GetFieldServerArray();
	
	SERVER_UTIL::CSystemInfo SysInfo;
	DxAgentInstance::Create(SysInfo.GetAppPath(), 
		                    this,
							CConsoleMessage::GetInstance(),
							COdbcManager::GetInstance(),
							NULL,
							dwMaxClient,
							pFieldInfo,
							(bool) m_bPK,
							m_bPKLess, 
							m_dwLangSet );
	
	if (StartUpdateThread() == NET_ERROR) return NET_ERROR;
	if (StartListenThread() == NET_ERROR) return NET_ERROR;	
	
	//////////////////////////////////////////////////////////////////////////////
	// Connect to Session server
	if (SessionConnectSndSvrInfo() == NET_ERROR)
	{	
		m_bIsRunning = false;
		return NET_ERROR;
	}

	/*
	nRetCode = SessionConnect(CCfg::GetInstance()->GetSessionServerIP(), CCfg::GetInstance()->GetSessionServerPort());
	if (nRetCode != NET_OK)
	{
		CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
		m_bIsRunning = false;
		return NET_ERROR;
	}
	CConsoleMessage::GetInstance()->Write(_T("Session Server Connection OK"));

	// Send Server info to Session Server
	SessionSndSvrInfo();	
	*/
	
	//////////////////////////////////////////////////////////////////////////////
	// Connect to all field server
	FieldConnectAll();

	CConsoleMessage::GetInstance()->Write( _T("NET_MSG_BASE %d"), NET_MSG_BASE );

	CConsoleMessage::GetInstance()->WriteFile(_T("======================================================="));
	CConsoleMessage::GetInstance()->WriteFile(_T("Server Start OK"));
	CConsoleMessage::GetInstance()->WriteFile(_T("======================================================="));

	m_nStatus = S_SERVER_RUNING;

	// �˰� �ִ� �ּҿ� �ڽ��� �ּ�, Session ������ �ּҸ� ����Ѵ�.
	SERVER_UTIL::CAddressChecker::GetInstance()->addKnownAddress(CCfg::GetInstance()->GetServerIP());
	SERVER_UTIL::CAddressChecker::GetInstance()->addKnownAddress(CCfg::GetInstance()->GetSessionServerIP());

	// �˰� �ִ� �ּҿ� �ʵ弭�� �ּҸ� ����Ѵ�.
	if (pFieldInfo != NULL)
	{
		for (int nCh=0; nCh<MAX_CHANNEL_NUMBER; ++nCh)
		{
			for (int nSvrNum=0; nSvrNum<FIELDSERVER_MAX; ++nSvrNum)
			{
				SERVER_UTIL::CAddressChecker::GetInstance()->addKnownAddress(pFieldInfo[nCh*MAX_CHANNEL_NUMBER+nSvrNum].szServerIP);
			}
		}
	}

	// UserDB�� UserInfo Table�� UserLoginState���� �ʱ�ȭ
	InitUserLoginState();

	return NET_OK;
}

int CAgentServer::Stop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;	

	CConsoleMessage::GetInstance()->WriteFile(_T("== Please wait until server stop "));

    CloseAllClient();
	FieldCloseConnectAll();

	LockOn();
	m_bIsRunning = false;
	LockOff();

	if( m_bUseEventThread )	
	{
		::SetEvent( m_hUpdateQuitEvent );

		::WaitForSingleObject( m_hUpdateThread, INFINITE );
		SAFE_CLOSE_HANDLE( m_hUpdateQuitEvent );
	}else{
		while ( m_hUpdateThread )	
		{
			Sleep( 0 );
		}
	}


	CConsoleMessage::GetInstance()->WriteFile(_T("== Update Thread end"));

	//CDbExecuter::GetInstance()->endThread();
	//CLogDbExecuter::GetInstance()->endThread();
	//CUserDbExecuter::GetInstance()->endThread();

	DxAgentInstance::CleanUp();

	StopListenThread();
	StopIOCPList();
	StopWorkThread();
	StopIOCP();
	
	SessionCloseConnect();

	// Ŭ���̾�Ʈ ���� Ŭ���� ����
	SAFE_DELETE(m_pClientManager);
	CConsoleMessage::GetInstance()->WriteFile(_T("== Client Manager closed"));
	// IOCP ����
	SAFE_DELETE(m_pSendIOCP);
	SAFE_DELETE(m_pRecvIOCP);
	// Message Queue ����
	SAFE_DELETE(m_pRecvMsgManager);
	CConsoleMessage::GetInstance()->WriteFile(_T("== Message Manager closed "));
	// DB ��������
	COdbcManager::GetInstance()->ReleaseInstance();
	CConsoleMessage::GetInstance()->WriteFile(_T("== Database Manager closed OK"));
	// CFG class ����
	CCfg::GetInstance()->ReleaseInstance();	

	// nProtect GameGuard ����
	if (m_bGameGuardInit == true)
	{
		CleanupGameguardAuth();
	}

#ifdef TH_PARAM
	if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif	

#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex ����	
	// Apex ����
	if( m_nServiceProvider == SP_CHINA || m_nServiceProvider == SP_TAIWAN )
//		|| m_nServiceProvider == SP_MINCOMS )
	{
		APEX_SERVER::StopApexProxy();
	}
#endif

	// Put message to console
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	CConsoleMessage::GetInstance()->Write(_T("Server Stop OK"));
	CConsoleMessage::GetInstance()->Write(_T("======================================================="));
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int	CAgentServer::Pause()
{
	::closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	CConsoleMessage::GetInstance()->Write( "=======================================================");	
	CConsoleMessage::GetInstance()->Write( "Server Pause OK");	
	CConsoleMessage::GetInstance()->Write( "=======================================================");

	return NET_OK;	
}

int CAgentServer::Resume()
{
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		CConsoleMessage::GetInstance()->Write( "=======================================================");	
		CConsoleMessage::GetInstance()->Write( "Server Resume OK");	
		CConsoleMessage::GetInstance()->Write( "=======================================================");

		return NET_OK;
	}
	else
	{
		// Put message to console
		CConsoleMessage::GetInstance()->Write( "=======================================================");	
		CConsoleMessage::GetInstance()->Write( "Server Resume Failed");	
		CConsoleMessage::GetInstance()->Write( "=======================================================");

		return NET_ERROR;
	}
}

int	CAgentServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}

// close all client connections
void CAgentServer::CloseAllClient()
{
	if (m_pClientManager)
	{
		for (DWORD i=NET_RESERVED_SLOT; i < (DWORD) m_pClientManager->GetMaxClient(); i++)
		{
			if (m_pClientManager->IsOnline(i))
				CloseClient(i);
		}		
	}
}

int CAgentServer::SendClient(DWORD dwClient, LPVOID pBuffer)
{
	if (dwClient >= (DWORD) m_nMaxClient || pBuffer == NULL)
	{		
		return NET_ERROR;	
	}
	else
	{
		if (m_pClientManager->IsOnline(dwClient))
		{				
			return m_pClientManager->SendClient(dwClient, pBuffer);
		}
		else
		{			
			return NET_ERROR;
		}
	}
}

// dwClient : Agent's client slot number
int CAgentServer::SendField(
	DWORD dwClient,
	LPVOID pBuffer )
{
	if (dwClient >= (DWORD) m_nMaxClient || pBuffer == NULL)
	{
		return NET_ERROR;
	}
	else
	{
		if (m_pClientManager->IsOnline(dwClient))
		{
			return m_pClientManager->SendField( dwClient+(DWORD) m_nMaxClient, pBuffer );
		}
		else
		{
			return NET_ERROR;
		}
	}
}

void CAgentServer::SendAllClient(LPVOID pBuffer)
{
	DWORD dwMaxClient;
	DWORD dwStartClient;

	dwStartClient = NET_RESERVED_SLOT;
	dwMaxClient = (DWORD) m_nMaxClient;

	CConsoleMessage::GetInstance()->WriteConsole(_T("Message to all player"));

	for (dwStartClient; dwStartClient < dwMaxClient; ++dwStartClient)
	{
		// �������̸� �޽����� ������
		if (m_pClientManager->IsGaming(dwStartClient))		
			SendClient(dwStartClient, pBuffer);
	}
}

void CAgentServer::SendAllClientChannel( LPVOID pBuffer, int nServerChannel)
{
	if ((pBuffer == NULL) || 
		(nServerChannel < 0) || 
		(nServerChannel >= m_nServerChannelNumber))
	{
		CConsoleMessage::GetInstance()->WriteConsole(_T("ERROR:SendAllClientChannel CH:%d"), nServerChannel);
		return;
	}

	DWORD dwMaxClient;
	DWORD dwStartClient;

	dwStartClient = NET_RESERVED_SLOT;
	dwMaxClient = (DWORD) m_nMaxClient;

	CConsoleMessage::GetInstance()->WriteConsole(_T("Message to CH %d players"), nServerChannel);

	for (dwStartClient; dwStartClient < dwMaxClient; dwStartClient++ )
	{
		// �������̰� ä���� ��ġ�Ѵٸ� �޽����� ������.
		if (m_pClientManager->IsGaming(dwStartClient) && 
			m_pClientManager->GetChannel(dwStartClient) == nServerChannel)		
			SendClient(dwStartClient, pBuffer);
	}
}

// ����ڸ� �α׾ƿ� ��Ų��.
//bool CAgentServer::CloseClient(
//	DWORD dwClient )
//{
//	LockOn();
//
//	if (dwClient<0) return false;
//	
//	int		nUserNum = 0;
//	int		nGameTime= 0;
//	int		nChaNum  = 0;
//	DWORD	dwGaeaID = 0;
//	int     nChannel = 0;
//	char*	szUserID = "";
//	bool    bReturn = false;
//
//	// ������ �������
//	if ( dwClient < NET_RESERVED_SLOT )
//	{
//		if ( dwClient == m_dwSession && m_bIsRunning == true )
//		{
//			if ( true == m_pClientManager->CloseClient( m_dwSession ) )
//			{
//				bReturn = true;
//				// Session �������� ����� ����Ǿ���.
//				// �Ϲ����� ��찡 �ƴϴ�.
//				// Session ������ �ٽ� ������ �õ��Ѵ�.
//				CConsoleMessage::GetInstance()->Write(
//					_T("INFO:Session Server Connection was closed. try to reconect") );
//				
//				// ������ ������ ��� �ٷ� �ٽ� ���� �õ� ���� �ʴ´�. Heart Beat���� �ٽ� üũ�ϱ� �����̴�.
//				SessionConnectSndSvrInfo();
//
//				CConsoleMessage::GetInstance()->Write(_T("================================"));
//				CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Closed"));
//				CConsoleMessage::GetInstance()->Write(_T("================================"));				
//			}
//		}
//		else
//		{
//			if ( true == m_pClientManager->CloseClient( dwClient ) )
//			{
//				bReturn = true;
//				// �ʵ弭���� ���� �����
//				CConsoleMessage::GetInstance()->Write(
//					_T("INFO:Field Server Connection was closed. %d"),
//					dwClient );
//				
//				// ������ ����̶�� �ʵ弭���� �ٽ� ����õ��� �Ѵ�.
//				if ( m_bIsRunning == true )
//				{
//					FieldConnectAll();
//				}
//			}
//		}
//	}
//	else if ( dwClient >= NET_RESERVED_SLOT &&
//		      dwClient < (DWORD) m_nMaxClient ) // client ���� ����
//	{
//		bool bAccountPass = false;
//		TCHAR szUserID[GSP_USERID] = {0};
//
//		nUserNum = m_pClientManager->GetUserNum(dwClient);
//		::StringCchCopy( szUserID, GSP_USERID, m_pClientManager->GetUserID(dwClient) );
//		nChannel = m_pClientManager->GetChannel(dwClient);
//
//		if ( nUserNum > 0 )
//		{				
//			nGameTime = m_pClientManager->GetLoginTime(dwClient);
//			dwGaeaID  = m_pClientManager->GetGaeaID(dwClient);
//			nChaNum = m_pClientManager->GetChaNum(dwClient);
//			bAccountPass = m_pClientManager->IsAccountPass(dwClient);
//		}
//
//		if ( true == m_pClientManager->CloseClient(dwClient) )
//		{
//			bReturn = true;
//
//			// ä�� �����ڼ��� ���ҽ�Ų��.
//			DecreaseChannelUser(nChannel);
//			SessionSndSvrCurState();
//
//			if ( nUserNum > 0 )
//			{
//				///////////////////////////////////////////////////////////////////////////
//				// ����ڸ� �α׾ƿ� ��Ų��.
//				// ������ ����� ���¿��� Close �������� logout ��Ų��.
//				if ( bAccountPass  == true)
//				{
//					CUserLogout* pAction5 = new CUserLogout(
//													szUserID, 
//													nUserNum, 
//													nGameTime, 
//													nChaNum,
//													m_nServerGroup,
//													m_nServerNum );
//					COdbcManager::GetInstance()->AddUserJob( (CDbAction*) pAction5 );
//
//					if( m_nServiceProvider == SP_CHINA )
//					{
//						APEX_SERVER::NoticeApexProxy_UserLogout( dwClient, szUserID );
//					}
//				}
//
//				///////////////////////////////////////////////////////////////////////////
//				// ĳ���͸� �����Ѵ�				
//				if (dwGaeaID != GAEAID_NULL)
//				{
//					// ���̾Ƽ��� ĳ���� ��� ����
//					GLAgentServer::GetInstance().ReserveDropOutPC(dwGaeaID); // lock
//					
//					// ĳ���� ���� ��������
//					CSetCharacterOffline* pDBAction3 = new CSetCharacterOffline(nChaNum);
//					COdbcManager::GetInstance()->AddJob(pDBAction3);
//				}
//				else
//				{
//					CConsoleMessage::GetInstance()->WriteConsole(
//															_T("CloseClient[ID:%d] Invalid Gaea ID"),
//															dwClient );
//				}				
//			}
//			
//			// �ʵ弭���� ��������
//			m_pClientManager->CloseField( dwClient );
//		}
//	}
//	else if ( dwClient >= (DWORD) m_nMaxClient &&
//		      dwClient < (DWORD) (m_nMaxClient*2) ) // Field �������� �������
//	{
//		// CConsoleMessage::GetInstance()->Write(_T("INFO:%d Field Server Connection was closed"), dwClient);
//		// m_pClientManager->CloseField(dwClient); // �ʵ弭���� ��������
//
//		if( true == m_pClientManager->CloseField( dwClient ) )
//		{
//			bReturn = true;
//
//			DWORD dwRealID = (DWORD)(dwClient - m_nMaxClient );
//			DWORD dwGaeaID = m_pClientManager->GetClientGaeaID( dwRealID );
//
//			if( dwGaeaID == GAEAID_NULL)
//			{
//				//CConsoleMessage::GetInstance()->Write(
//				//	_T("CloseClient dwGaeaID == GAEAID_NULL!!!") );
//			}
//			else
//			{
//				NET_MSG_GENERIC NetMsg;
//				memcpy( &NetMsg, m_pClientManager->GetNetMsg( dwRealID ), sizeof(NET_MSG_GENERIC) );
//
//				m_pClientManager->ResetConnectionFieldInfo( dwRealID );
//
//				PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar ( dwGaeaID );
//
//				//CConsoleMessage::GetInstance()->Write(
//				//	_T("CloseClient ClientID[%u], GaeaID[%u]"), dwRealID, dwGaeaID );
//
//				if ( pChar && pChar->m_dwNextFieldSvr != FIELDSERVER_MAX )
//				{
//					//	���ο� �ʵ� ������ ������ �Ѵ�.
//					if ( ConnectFieldSvr(	dwRealID, 
//											pChar->m_dwNextFieldSvr, 
//											dwGaeaID,
//											pChar->m_nChannel) == NET_OK )
//					{
//						//	������Ʈ ������ �޽��� ����.
//						GLAgentServer::GetInstance().MsgProcess( &NetMsg, dwRealID, dwGaeaID );
//						//pChar->ResetNextFieldSvr (); //	������ �ʵ弭�� ������ ����.
//					}
//				}
//			}
//		}
//	}
//	else // ��������!
//	{
//		CConsoleMessage::GetInstance()->Write(
//			_T("ERROR:Unknown connection closed") );
//	}
//
//	LockOff();
//	return bReturn;
//}

void CAgentServer::CloseClient( DWORD dwClient )
{
	int		nUserNum = 0;
	int		nGameTime= 0;
	int		nChaNum  = 0;
	DWORD	dwGaeaID = 0;
	int     nChannel = 0;
	char*	szUserID = "";
	int		nTotalGameTime = 0;
	int		nOfflineTime = 0;
	int		temp = 0;

	LockOn();
	{
		if ((m_pClientManager->IsOnline(dwClient) == false) || (dwClient < 0))
		{
			LockOff();
			return;
		}


		// ������ �������
		if ((dwClient < NET_RESERVED_SLOT) && (dwClient >= 0))
		{
			if (dwClient == NET_RESERVED_SESSION && m_bIsRunning == true)
			{
				// Session �������� ����� ����Ǿ���.
				// �Ϲ����� ��찡 �ƴϴ�.
				// Session ������ �ٽ� ������ �õ��Ѵ�.
				CConsoleMessage::GetInstance()->Write(
					_T("INFO:Session Server Connection was closed. try to reconect") );
				m_pClientManager->CloseClient( NET_RESERVED_SESSION );
				
				// ������ ������ ��� �ٷ� �ٽ� ���� �õ� ���� �ʴ´�. Heart Beat���� �ٽ� üũ�ϱ� �����̴�.
				//SessionConnectSndSvrInfo();

				CConsoleMessage::GetInstance()->Write(_T("================================"));
				CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
				CConsoleMessage::GetInstance()->Write(_T("================================"));

			}
			else
			{
				// �ʵ弭���� ���� �����
				CConsoleMessage::GetInstance()->Write(
					_T("INFO:Field Server Connection was closed. try to reconect %d"),
					dwClient );
				m_pClientManager->CloseClient( dwClient );
				// ������ ����̶�� �ʵ弭���� �ٽ� ����õ��� �Ѵ�.
				//if ( m_bIsRunning == true )
				//{
				//	FieldConnectAll();
				//}

			}
		}
		else if ((dwClient >= NET_RESERVED_SLOT) && (dwClient < (DWORD) m_nMaxClient)) // client ���� ����
		{
			nUserNum = m_pClientManager->GetUserNum(dwClient);
			szUserID = m_pClientManager->GetUserID(dwClient);
			nChannel = m_pClientManager->GetChannel(dwClient);

			// ä�� �����ڼ��� ���ҽ�Ų��.
			DecreaseChannelUser(nChannel);

			// ��ϵ� IP ����Ʈ�� �����Ѵ�.
			// SERVER_UTIL::CAddressChecker::GetInstance()->Delete(m_pClientManager->GetClientIP(dwClient));

			// ������ ����� ���¿��� Close �������� logout ��Ų��.
			/*
			if (m_pClientManager->IsAccountPass(dwClient) == true)
			{
				// ����ڸ� �α׾ƿ� ��Ų��.		
				CUserLogoutSimple* pAction1 = new CUserLogoutSimple(szUserID);
				COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction1);

				CUserLogoutSimple2* pAction2 = new CUserLogoutSimple2(nUserNum);
				COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction2);				
			}
			*/

			if ( nUserNum > 0 )
			{
#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
				int nTracing = CheckTracingUser( m_pClientManager->GetUserID(dwClient), m_pClientManager->GetUserNum(dwClient) );
				if( nTracing != -1 )
				{
					CDebugSet::ToTracingFile( m_vecTracingData[nTracing].strAccount.c_str(), "Account: [%s], UserNum: [%u], IP: [%s]", 
											  m_pClientManager->GetUserID(dwClient), m_pClientManager->GetUserNum(dwClient), 
											  m_pClientManager->GetClientIP(dwClient) );
					CDebugSet::ToTracingFile( m_vecTracingData[nTracing].strAccount.c_str(), "############ Tracing User Logoff ############" );

					NET_UPDATE_TRACINGCHAR msg;
					m_vecTracingData[nTracing].dwOnLine = FALSE;
					msg.tracingData = m_vecTracingData[nTracing];
					msg.updateNum   = nTracing;
					SendSession( &msg );
				}
#endif
				nGameTime = m_pClientManager->GetLoginTime(dwClient);
				dwGaeaID  = m_pClientManager->GetGaeaID(dwClient);
				nChaNum   = m_pClientManager->GetChaNum(dwClient);
#if defined(VN_PARAM) //vietnamtest%%%
				// ������ �α׿��� �ð�
				CTime lastOffTime = m_pClientManager->GetLastLoginDate(dwClient);
				CTime crtTime	  = CTime::GetCurrentTime();
				// �⵵, ��, ��¥�� �ٸ��� �ʱ�ȭ �Ѵ�.
				if( lastOffTime.GetYear() != crtTime.GetYear() ||
					lastOffTime.GetMonth() != crtTime.GetMonth() ||
					lastOffTime.GetDay() != crtTime.GetDay()  )
				{
					m_pClientManager->SetVTGameTime(dwClient, 0 );
//					CConsoleMessage::GetInstance()->Write( _T("INFO:Vietnam Accumulation Time Init ###Close Client### Last Off Time Month %d, Day %d" ), 
//													lastOffTime.GetMonth(), lastOffTime.GetDay());

					temp = 0;
				}else{
					temp  = m_pClientManager->GetVTGameTime(dwClient);
				}

				CTime tLogin = m_pClientManager->GetLoginTimeEx( dwClient );

				if( tLogin.GetYear() != crtTime.GetYear() ||
					tLogin.GetMonth() != crtTime.GetMonth() ||
					tLogin.GetDay() != crtTime.GetDay()  )

				{
					// �������� ���� �ð� ( �� ) 
					int nMidTime =  ( crtTime.GetHour() * 60 ) + crtTime.GetMinute();

					// nGameTime�� nMidTime���� ũ�ٸ� ����� �ʱ�ȭ�� �̷������ �ʾҴٴ� ����̴�.
					if ( nGameTime > nMidTime )
					{	
						nGameTime = nMidTime;				
					}	

				}

#else
				temp  = m_pClientManager->GetChinaGameTime(dwClient);
#endif 				
				nTotalGameTime = nGameTime+temp;
				nOfflineTime = m_pClientManager->GetChinaOfflineTime(dwClient);

				///////////////////////////////////////////////////////////////////////////
				// ����ڸ� �α׾ƿ� ��Ų��.
				// ������ ����� ���¿��� Close �������� logout ��Ų��.
				if (m_pClientManager->IsAccountPass(dwClient) == true)
				{
					CUserLogout* pAction5 = new CUserLogout(szUserID, 
						nUserNum, 
						nGameTime, 
						nChaNum,
						m_nServerGroup,
						m_nServerNum,
						nTotalGameTime,
						nOfflineTime);
					COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction5);

#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex ����
					if( m_nServiceProvider == SP_CHINA || m_nServiceProvider == SP_TAIWAN )
//						|| m_nServiceProvider == SP_MINCOMS )
					{
						APEX_SERVER::NoticeApexProxy_UserLogout( dwClient, szUserID );
					}
#endif

#if defined ( JP_PARAM ) //  || defined( _RELEASED ) // �Ϻ� WhiteRock���� ���
					//m_CWhiteRock.SendLogoutXML( m_pClientManager->JPGetUUID(dwClient) );
						CAgentJapnaWRLogout* pAction = new CAgentJapnaWRLogout(
																	m_pClientManager->JPGetUUID(dwClient) );

					COdbcManager::GetInstance()->AddWebJob((CDbAction*) pAction);
#endif
				}

				///////////////////////////////////////////////////////////////////////////
				// ĳ���͸� �����Ѵ�				
				if (dwGaeaID != GAEAID_NULL)
				{
					// ���̾Ƽ��� ĳ���� ��� ����
					GLAgentServer::GetInstance().ReserveDropOutPC(dwGaeaID); // lock

					// ĳ���� ���� �������� 
					CSetCharacterOffline* pDBAction3 = new CSetCharacterOffline(m_pClientManager->GetChaNum(dwClient));
					COdbcManager::GetInstance()->AddJob(pDBAction3);
				}
				else
				{
					CConsoleMessage::GetInstance()->WriteConsole( _T("ERROR:%d Invalid Gaea ID"), dwClient );
//					CConsoleMessage::GetInstance()->Write( _T("ERROR:%d Invalid Gaea ID"), dwClient );
				}				

			}
			m_pClientManager->CloseClient(dwClient); // Ŭ���̾�Ʈ�� ��������
			m_pClientManager->CloseField (dwClient); // �ʵ弭���� ��������			

			SessionSndSvrCurState();

			// ���� ���� Reached Max Client �����̸�...
			if( m_bReachedMAX )
			{
				CConsoleMessage::GetInstance()->Write( _T("########### CloseClient Run2 %d ########### "), dwClient );
				m_bReachedMAX = FALSE;
			}

		}
		else if ((dwClient >= (DWORD) m_nMaxClient) && (dwClient < (DWORD) (m_nMaxClient*2))) // Field �������� �������
		{
			// CConsoleMessage::GetInstance()->Write(_T("INFO:%d Field Server Connection was closed"), dwClient);
			// m_pClientManager->CloseField(dwClient); // �ʵ弭���� ��������
		}
		else // ��������!
		{
			CConsoleMessage::GetInstance()->Write(_T("ERROR:Unknown reason connection close"));
		}


		

	}

	LockOff();
}

bool CAgentServer::IsFieldOnline(DWORD dwClient)
{
    return m_pClientManager->IsFieldOnline(dwClient);
}

bool CAgentServer::IsSlotOnline ( DWORD dwClient )
{
	return m_pClientManager->IsOnline(dwClient);
}

void CAgentServer::CloseField(
	DWORD dwClient )
{
    if ((dwClient >= (DWORD) m_nMaxClient) && (dwClient < (DWORD) (m_nMaxClient*2)))
    {
		m_pClientManager->CloseField(dwClient-m_nMaxClient);
    }
    else
    {
        m_pClientManager->CloseField(dwClient);
    }
}

int CAgentServer::ConnectFieldSvr(
	DWORD dwClient,
	int nFieldServer,
	DWORD dwGaeaID,
	int nServerChannel )
{
	return m_pClientManager->ConnectFieldServer ( dwClient, nFieldServer, dwGaeaID, nServerChannel );
}

int CAgentServer::ExecuteCommand(char* strCmd)
{
	// ��ū���� �и��� �и���.
	// space
	char seps[]   = " ";
	char *token;
	char* nToken = NULL;
	
	// Establish string and get the first token
	token = strtok_s(strCmd, seps, &nToken);
	while (token != NULL)
	{
		if (strcmp(token, "print_ip") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			return 0;
		}
		if (strcmp(token, "print_freeslot") == 0)
		{
			m_pClientManager->SaveFreeSlot();			
			return 0;
		}
		if (strcmp(token, "print_info") == 0)
		{
			// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
			m_pClientManager->SaveFreeSlot();
			return 0;
		}		
		// ���� ä�� �޽���, global
		if (strcmp(token, "chat") == 0)
		{
			token = strtok_s(NULL, seps, &nToken);
			if (token)
			{
				MsgSndChatGlobal(token);
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

void CAgentServer::InitUserLoginState()
{
	int nServerGroup	= 0;
//	int nServerNumber	= 0;

	nServerGroup	= CCfg::GetInstance()->GetServerGroup();
//	nServerNumber	= CCfg::GetInstance()->GetServerNumber();

//	CUserLogoutSvr* pAction = new CUserLogoutSvr( nServerGroup, nServerNumber );
//		COdbcManager::GetInstance()->AddLogJob((CDbAction*) pAction);
	COdbcManager::GetInstance()->AllServerUserLogout(m_nServerGroup);
}

int CAgentServer::CheckTracingUser( const char *pUserAccount, const int nUserNum )
{

	int i;
	for( i = 0; i < (int)m_vecTracingData.size(); i++ )
	{
		STracingData data = m_vecTracingData[i];
		if( _strcmpi( data.strAccount.c_str(), pUserAccount ) == 0 ||
			data.dwUserNum  == nUserNum )
		{
			return i;
		}
	}

	return -1;
}

void CAgentServer::RecvMsgProcess()
{
	MSG_LIST* pMsg = NULL;	

	// Flip Messge Queue
	m_pRecvMsgManager->MsgQueueFlip();

	// Get Messages and Processing message
	pMsg = m_pRecvMsgManager->GetMsg();
	while (pMsg != NULL)
	{
		// �޽��� ó��
		MsgProcess(pMsg);					
		pMsg = m_pRecvMsgManager->GetMsg(); // �����޽��� ��������
	}

}

void CAgentServer::AgentSrvHeartBeatCheck( DWORD dwUdpateTime )
{
	DWORD dwCurrentTime	= timeGetTime();
	DWORD dwN               = 0;	           // ���޻���� ���Ӳ��⸦ ���� ���� ����
	/////////////////////////////////////////////////////////////////////////////
	// User Connection Check
	if ((dwCurrentTime - m_dwHeartBeatTime) > SESSION_CHECK_TIME)
	{
		if ( m_bHeartBeat )
		{	
			NET_HEARTBEAT_SERVER_REQ HeartbeatServerRequest;

			// Session Server Connection Check
			if( m_pClientManager->IsOnline( NET_RESERVED_SESSION ) )
			{
				m_pClientManager->ResetHeartBeat( NET_RESERVED_SESSION );
				SendSession( &HeartbeatServerRequest );
			}
			else
			{
				CConsoleMessage::GetInstance()->Write(_T("================================"));
				CConsoleMessage::GetInstance()->Write(_T("Session Server Connection Failed"));
				CConsoleMessage::GetInstance()->Write(_T("Reconnecting Session Server."));
				CConsoleMessage::GetInstance()->Write(_T("================================"));

				//SessionReconnect();
				SessionConnectSndSvrInfo();
			}

			// Field Server Connection Check
			// ������ ä�� ������ �ʵ弭�� ������ �����´�.
			int nMaxFiledNumber = CCfg::GetInstance()->GetMaxFieldNumber();
			for ( int nChannel=0; nChannel<m_nServerChannelNumber; nChannel++ )
			{
				for ( int nFieldNumber=0; nFieldNumber <= nMaxFiledNumber; nFieldNumber++ )
				{
					DWORD dwFieldSlotNumber = nChannel*MAX_CHANNEL_NUMBER+nFieldNumber;
					m_pClientManager->ResetHeartBeat( dwFieldSlotNumber );
					SendClient( dwFieldSlotNumber, &HeartbeatServerRequest );
				}
			}
		}
		else
		{
			// Session Check
			if ( true == m_pClientManager->IsOnline( NET_RESERVED_SESSION ) &&
				false == m_pClientManager->CheckHeartBeat( NET_RESERVED_SESSION ) )
			{
				CloseClient( NET_RESERVED_SESSION );
			}

			// Field Server Connection Check
			// ������ ä�� ������ �ʵ弭�� ������ �����´�.
			int nMaxFiledNumber = CCfg::GetInstance()->GetMaxFieldNumber();
			for ( int nChannel=0; nChannel<m_nServerChannelNumber; nChannel++ )
			{
				for ( int nFieldNumber=0; nFieldNumber <= nMaxFiledNumber; nFieldNumber++ )
				{
					DWORD dwFieldSlotNumber = nChannel*MAX_CHANNEL_NUMBER+nFieldNumber;
					if ( true == m_pClientManager->IsOnline( dwFieldSlotNumber ) &&
						false == m_pClientManager->CheckHeartBeat( dwFieldSlotNumber ) )
					{
						CloseClient( dwFieldSlotNumber );
					}							
				}
			}
		}

		if (m_bGameGuardInit == true && m_bGameGuardAuth ==true)
		{
			// GameGuard Check
			if (m_bHeartBeat)
			{
				CConsoleMessage::GetInstance()->WriteInfo(_T("Send GameGuardAuth Request"));
				// ���Ӱ��� ������ �޽���
				NET_GAMEGUARD_AUTH nga;
				for (dwN = NET_RESERVED_SLOT; dwN < (DWORD)m_nMaxClient; ++dwN)
				{
					m_pClientManager->ResetHeartBeat( dwN );
					MsgSndGameGuard( dwN, nga );
				}
			}
			else
			{
				CConsoleMessage::GetInstance()->WriteInfo(_T("Check GameGuardAuth"));
				DWORD dwResult = 0;
				for (dwN = NET_RESERVED_SLOT; dwN < (DWORD)m_nMaxClient; ++dwN)
				{
					if (m_pClientManager->IsOnline(dwN))
					{
						dwResult = m_pClientManager->nProtectCheckAuthAnswer( dwN );
						if (dwResult != 0 && false == m_pClientManager->CheckHeartBeat(dwN) )
						{

							WriteNprotectLog( dwN, dwResult );
							CloseClient(dwN);
						}
					}
				}
			}
		}
		else
		{
			// Hearbeat Check
			if (m_bHeartBeat)
			{	
				CConsoleMessage::GetInstance()->WriteInfo(_T("Send Heartbeat Request"));

				NET_HEARTBEAT_CLIENT_REQ HeartbeatClientRequest;

				for ( dwN = NET_RESERVED_SLOT; dwN < (DWORD)m_nMaxClient; ++dwN )
				{
					// �¶��� �����ΰ��
					if (m_pClientManager->IsOnline( dwN ))
					{
						m_pClientManager->ResetHeartBeat( dwN );
						SendClient( dwN, &HeartbeatClientRequest );								
						/*
						if (m_pClientManager->IsFieldOnline(dwN))
						SendField(dwN, &nmg);
						*/
					}
				}
			}
			else
			{
				CConsoleMessage::GetInstance()->WriteInfo(_T("Check Heartbeat"));
				for ( dwN = NET_RESERVED_SLOT; dwN < (DWORD)m_nMaxClient; ++dwN )
				{	
					if ( true == m_pClientManager->IsOnline(dwN) &&
						false == m_pClientManager->CheckHeartBeat(dwN) )
					{
						CloseClient(dwN);
					}
				}
			} // bHeartBeat
		}
		m_bHeartBeat = !m_bHeartBeat;
		m_dwHeartBeatTime = dwCurrentTime;
	} // HeartBeat check time

}

void CAgentServer::UpdateClientState()
{
	///////////////////////////////////////////////////////////////////////
	// ��Ʈ�� �� 12�ð� ������ ��ü �ð��� �ʱ�ȭ ��
#if defined(VN_PARAM) //vietnamtest%%%
	{
		CTime crtTime = CTime::GetCurrentTime();
		if( m_VietnamPresetTime.GetDay() != crtTime.GetDay() ||
			m_VietnamPresetTime.GetMonth() != crtTime.GetMonth() ||
			m_VietnamPresetTime.GetYear() != crtTime.GetYear()  )
		{
			DWORD dwGaeaID;
			m_VietnamPresetTime	= CTime::GetCurrentTime();

			for (DWORD dwVNCount = NET_RESERVED_SLOT; dwVNCount < m_nMaxClient; ++dwVNCount)
			{
				if( m_pClientManager->IsOnline(dwVNCount) )
				{
					dwGaeaID = m_pClientManager->GetGaeaID(dwVNCount);
					PGLCHARAG pChar = GLAgentServer::GetInstance().GetChar(dwGaeaID);

					// �κ� ������� pChar�� ���̴�. 
					m_pClientManager->SetVTGameTime(dwVNCount,0);
					m_pClientManager->SetLoginTime(dwVNCount);

					if( pChar == NULL )
					{
						CConsoleMessage::GetInstance()->Write( _T("ERROR:pChar Is Null.. dwGaeaID %d, dwVNCount %d VTGameTime %d"), 
							dwGaeaID, dwVNCount, m_pClientManager->GetVTGameTime(dwVNCount) );
						continue;
					}

					pChar->m_sVietnamSystem.gameTime   = 0;
					pChar->m_sVietnamSystem.loginTime  = m_VietnamPresetTime.GetTime();
				}
			} 

			GLMSG::SNETPC_VIETNAM_ALLINITTIME NetMsg;
			NetMsg.initTime = m_VietnamPresetTime.GetTime();
			//					GLAgentServer::GetInstance().MsgProcess( &NetMsg );
			SendAllChannel ( &NetMsg );
			CConsoleMessage::GetInstance()->Write( _T("INFO:-=-=-=-=Init Vietnam Time=-=-=-=-=-") );
		}	
	}

#endif
}

void CAgentServer::PrintDebugMsg( DWORD dwUdpateTime )
{
	DWORD dwCurrentTime = timeGetTime();
	///////////////////////////////////////////////////////////////////////
	// ������ ����
	if ((dwCurrentTime - m_dwFrameTime) >= dwUdpateTime )
	{		
		// DB ��������				
		// COdbcManager::GetInstance()->CheckConnection();

		float fFrame = ((float)((m_dwFrameCount * dwUdpateTime) /(float)(timeGetTime() - m_dwFrameTime)));			
		m_dwFrameCount = 0;				

		// ���� ����ڼ� ���÷���

		// ������ ���� ���÷���
		CConsoleMessage::GetInstance()->WriteInfo(
			_T("Update %d FPS Avg %d msec User %d/%d"),
			(int) (fFrame/(dwUdpateTime/1000)),
			(int) (dwUdpateTime/fFrame),
			(int) m_pClientManager->GetCurrentClientNumber(),
			(int) m_pClientManager->GetMaxClient() );

		// ��Ŷ ó���� ���÷���
		CConsoleMessage::GetInstance()->WriteInfo(
			_T("In Packet Count %d/sec Traffic %d(bytes)/sec"),
			(int) (m_pClientManager->getInPacketCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getInPacketSize() / (dwUdpateTime/1000)) );

		CConsoleMessage::GetInstance()->WriteInfo(
			_T("Out Packet Count %d/sec Traffic %d(bytes)/sec"),
			(int) (m_pClientManager->getOutPacketCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getOutPacketSize() / (dwUdpateTime/1000)) );

		CConsoleMessage::GetInstance()->WriteInfo(
			_T("Compress Count %d/sec Traffic %d(bytes)/sec"),
			(int) (m_pClientManager->getOutCompressCount() / (dwUdpateTime/1000)),
			(int) (m_pClientManager->getOutCompressSize() / (dwUdpateTime/1000)) );

		// Message Queue / IOCP �޸� ��뷮 ���÷���
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

		// ä�κ� ����ڼ� ���÷���
		CConsoleMessage::GetInstance()->WriteInfo(
			_T("CH[%d %d %d %d %d %d %d %d %d %d]"),
			m_ChannelUser[0],
			m_ChannelUser[1],
			m_ChannelUser[2],
			m_ChannelUser[3],
			m_ChannelUser[4],
			m_ChannelUser[5],
			m_ChannelUser[6],
			m_ChannelUser[7],
			m_ChannelUser[8],
			m_ChannelUser[9] );

		//				CConsoleMessage::GetInstance()->Write( _T("INFO:Channel User Number: %d deqIDSize : %d CurrentClientNum : %d "), 
		//														m_ChannelUser[0],
		//														m_pClientManager->m_deqSleepCID.size(),	m_pClientManager->GetCurrentClientNumber() );


		// ��Ŷī���� ����
		m_pClientManager->resetPacketCount();

		m_dwFrameTime = timeGetTime();
	}
	else
	{
		m_dwFrameCount++;
	}	
}

void CAgentServer::EndUpdateThread()
{
	// Last db action
	COdbcManager::GetInstance()->SetAllCharacterOffline(); // ��� ĳ���͸� ������������ �����.
	COdbcManager::GetInstance()->AllServerUserLogout(m_nServerGroup);
	CDbExecuter::GetInstance()->ExecuteJobLast();
	CLogDbExecuter::GetInstance()->ExecuteJobLast();
	CUserDbExecuter::GetInstance()->ExecuteJobLast();
	CWebDbExecuter::GetInstance()->ExecuteJobLast();
	CConsoleMessage::GetInstance()->Write(_T("= Last Database Job Executed"));

	TRACE ( "[UpdateProc End]\n" );

	//	������Ʈ ������ �����.
	CloseHandle( m_hUpdateThread );
	m_hUpdateThread = NULL;
	m_bUpdateEnd	= true;
}

//! -----------------------------------------------------------------------
//! I/O operation memory
//int CAgentServer::StopIOCPList()
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
PER_IO_OPERATION_DATA* CAgentServer::getSendIO(
	DWORD dwClient )
{
	return m_pClientManager->getSendIO( dwClient );
}

PER_IO_OPERATION_DATA* CAgentServer::getRecvIO(
	DWORD dwClient )
{
	return m_pClientManager->getRecvIO( dwClient );
}

void CAgentServer::releaseSendIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseSendIO( dwClient, pData );
}

void CAgentServer::releaseRecvIO(
	DWORD dwClient,
	PER_IO_OPERATION_DATA* pData )
{
	m_pClientManager->releaseRecvIO( dwClient, pData );
}

void CAgentServer::releaseIO(
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
