#include "pch.h"
#include "s_CCfg.h"
#include "s_CSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCfg* CCfg::SelfInstance = NULL;

CCfg::CCfg()
{
	SetDefault();
}

CCfg::CCfg(const TCHAR* filename)
{
	SetDefault();
	Load(filename);
}

CCfg* CCfg::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CCfg();

	return SelfInstance;
}

void CCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CCfg::SetDefault(void)
{
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	m_nServerVersion		= 0;	// 서버 버전, 클라이언트 버전과 동일
	m_nPatchVersion			= 0;	// 패치프로그램 버전
	
	m_nPortService			= 5001;	// 서비스용 포트
	m_nPortControl			= 6001;	// 컨트롤용 포트
	
	m_nServerType			= 0;	// 서버 유형
	m_nServerGroup			= 0;	// 서버 그룹
	m_nServerNumber			= 0;	// 서버 번호
	m_nServerField			= 0;	// 서버 필드 번호.
	m_nServerChannel        = 0;    // 서버 채널 번호
	m_nServerChannelNumber  = 1;    // Agent 가 가진 채널 갯수
	
	m_nServerMaxClient		= 1000;	// 최대 접속 가능 클라이언트 수
	m_nMaxChannelUser       = 1000; // 채널당 최대 접속 가능 클라이언트 수

	m_nMaxFieldNumber = 0;

	m_nGAMEServerNumber		= 0;
	m_nLOGINServerNumber	= 0;
	m_nFTPServerNumber		= 0;
	m_nSESSIONServerNumber	= 0;

	m_bHeartBeat			= true;  // Heart beat check
	m_nServiceProvider		= 0;     // Service provider
	m_nMaxThread			= 8;	 // 최대 work thread 갯수
	m_bTestServer			= false; // Test 서버 유무
	m_bAdultCheck			= false; // 성인서버 테스트 유무
	m_bUseIntelTBB			= false; // TBB 사용 여부
	m_bUseEventThread		= false; // 이벤트 쓰레드 사용 여부
	m_bPK                   = true;  // true:PK 서버, false:Non PK 서버
	m_bPkLess               = false;
	m_bGameGuardAuth        = false; // nProtect GameGuard 인증 사용여부

	m_bAllowFileLoad		= false; // allow_ip.cfg파일 사용여부

	for (int i=0; i<MAX_CHANNEL_NUMBER; i++)
	{
		m_sChannelInfo[i].bPK = true;
	}
}

CCfg::~CCfg()
{
}

int CCfg::Load(const TCHAR* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	SERVER_UTIL::CSystemInfo SysInfo;
	CString strAppPath;	

	strAppPath = SysInfo.GetAppPath();
	strAppPath += _T("\\cfg\\");
	strAppPath += filename;
	
	FILE *oFile = NULL;
	TCHAR line[300];

	// Open for read
	::_tfopen_s(&oFile, strAppPath.GetString(), _T("r"));
	if (oFile == NULL)
	{
		/*
		MessageBox(
			NULL,
			szAppPath,
			_T("ERROR"),
			MB_OK);
		*/
		return -1;
	}	

	// Read a line and process
	while (::_fgetts(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
	{
		return -1;
	}	
	return 0;
}

int CCfg::Process(TCHAR* szLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	TCHAR seps[]   = _T(" ,\t\n");
	TCHAR* token;
	TCHAR* nToken = NULL;

	// Establish string and get the first token
	token = _tcstok_s(szLine, seps, &nToken);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (_tcscmp(token, _T("//")) == 0) 
		{
			return 0;
		} 		
		//////////////////////////////////////////////////////////////////////
		// server setting value
		
		
		else if (_tcscmp(token, "pkless") == 0)
		{
			// 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
			token = _tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (_tstoi(token)) m_bPkLess = true;
				else             m_bPkLess = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "pk") == 0)
		{
			// PK 만 Disable
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bPK = true;
				else					m_bPK = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "adult") == 0)
		{
			// Adust check
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bAdultCheck = true;
				else					m_bAdultCheck = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "use_intel_tbb") == 0)
		{
			// use inteltbb
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bUseIntelTBB = true;
				else					m_bUseIntelTBB = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "use_event_thread") == 0)
		{
			// use event Thread
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bUseEventThread = true;
				else					m_bUseEventThread = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "testserver") == 0)
		{
			// testserver
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bTestServer = true;
				else					m_bTestServer = false;
			}
			else
			{
				return 0;
			}
		}		
		else if (_tcscmp(token, "heartbeat") == 0)
		{
			// HeartBeat
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bHeartBeat = true;
				else					m_bHeartBeat = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "gameguard") == 0)
		{
			// HeartBeat
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				if (::_tstoi(token))		m_bGameGuardAuth = true;
				else					m_bGameGuardAuth = false;
			}
			else
			{
				return 0;
			}
		}
		else if (_tcscmp(token, "server_version") == 0) 
		{
			// 서버 버전, 클라이언트 버전과 동일
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerVersion = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "patch_version") == 0)
		{
			// 패치프로그램 버전
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nPatchVersion = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_name") == 0) 
		{
			// 서버명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szServerName, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_max_client") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerMaxClient = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_max_channel_client") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nMaxChannelUser = _tstoi(token);
			else
				return 0;
		}
			
		else if (_tcscmp(token, "server_ip") == 0) 
		{
			// 서버 IP, 프로그램에서 자동으로 결정될수 있다
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szServerIP, MAX_IP_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_service_port") == 0) 
		{
			// 서비스용 포트
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nPortService = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_control_port") == 0) 
		{
			// 컨트롤용 포트
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nPortControl = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_type") == 0)
		{
			// server type [type]
			// [type]
			// 1 : login server
			// 2 : session server
			// 3 : game server
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerType = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_group") == 0)
		{
			// server group
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerGroup = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_number") == 0)
		{
			// Server number
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerNumber = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_field") == 0)
		{
			// Server field
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerField = _tstoi(token);
			else
				return 0;
		}		 
		else if (_tcscmp(token, "server_channel") == 0)
		{
			// Server field
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerChannel = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "server_channel_number") == 0) 
		{
			// Agent 서버가 가진 채널의 갯수
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServerChannelNumber = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "service_provider") == 0)
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nServiceProvider = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "whiterock_name") == 0) 
		{
			// 일본 WhiteRock 서버명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szWhiteRockServerName, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "whiterock_port") == 0)
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nWhiteRockServerPort = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "max_thread") == 0)
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nMaxThread = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "language_set") == 0)
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nLangSet = ::_tstoi(token);
			else
				return 0;
		}
		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE1" ) == 0 )
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if ( token )
			{
				CString strTemp;
				strTemp.Format( _T( "%s" ), token );
				//::StringCchCopy( strTemp.GetBuffer( 0 ), DB_SVR_NAME_LENGTH + 1, token );
				m_vecAllowIPType1.push_back( strTemp );
			}
			else
				return 0;
		}

		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE2" ) == 0 )
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if ( token )
			{
				CString strTemp;
				strTemp.Format( _T( "%s" ), token );
				m_vecAllowIPType2.push_back( strTemp );
			}
			else
				return 0;
		}

		else if( _tcscmp( token, "ALLOW_IP_INFO_TYPE3" ) == 0 )
		{
			token = ::_tcstok_s(NULL, seps, &nToken);
			if ( token )
			{
				CString strTemp;
				strTemp.Format( _T( "%s" ), token );
				m_vecAllowIPType3.push_back( strTemp );
			}
			else
				return 0;
		}



		//////////////////////////////////////////////////////////////////////
		// DB-LIB C User database setting value 
		else if (_tcscmp(token, "user_db_server") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szUserDBServer, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szUserDBUser, USR_ID_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szUserDBPass, USR_PASS_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szUserDBDatabase, DB_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nUserDBPoolSize = ::_tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "user_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nUserDBResponseTime = _tstoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// DB-LIB C Game database setting value 
		else if (_tcscmp(token, "game_db_server") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szGameDBServer, DB_SVR_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szGameDBUser, USR_ID_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szGameDBPass, USR_PASS_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				::StringCchCopy(m_szGameDBDatabase, DB_NAME_LENGTH+1, token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nGameDBPoolSize = _tstoi(token);
			else
				return 0;
		}
		else if (_tcscmp(token, "game_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
				m_nGameDBResponseTime = _tstoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC User database setting value 
		else if (_tcscmp(token, "user_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szUserOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "user_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szUserOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szUserOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "user_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szUserOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nUserOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "user_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nUserOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Game database setting value 
		else if (_tcscmp(token, "game_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szGameOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "game_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szGameOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szGameOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "game_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szGameOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nGameOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "game_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nGameOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Board database setting value 
		else if (_tcscmp(token, "board_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szBoardOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "board_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szBoardOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szBoardOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "board_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szBoardOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nBoardOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "board_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nBoardOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Log database setting value 
		else if (_tcscmp(token, "log_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szLogOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "log_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szLogOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szLogOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "log_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szLogOdbcDatabase, DB_SVR_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nLogOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "log_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nLogOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Shop database setting value 
		else if (_tcscmp(token, "shop_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szShopOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "shop_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szShopOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szShopOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "shop_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szShopOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nShopOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "shop_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nShopOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Terra database setting value 
		else if (_tcscmp(token, "terra_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szTerraOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "terra_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szTerraOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szTerraOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "terra_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szTerraOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nTerraOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "terra_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nTerraOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC GSP database setting value 
		else if (_tcscmp(token, "gsp_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szGspOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szGspOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szGspOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szGspOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nGspOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "gsp_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nGspOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Korea database setting value 
		else if (_tcscmp(token, "kor_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szKorOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "kor_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szKorOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szKorOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "kor_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szKorOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nKorOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "kor_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nKorOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// ODBC Malaysia database setting value 
		else if (_tcscmp(token, "my_odbc_name") == 0) 
		{
			// database server name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szMyOdbcName, DB_SVR_NAME_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "my_odbc_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szMyOdbcUser, USR_ID_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_pass") == 0) 
		{
			// 비밀번호
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	StringCchCopy(m_szMyOdbcPass, USR_PASS_LENGTH+1, token);
			else		return 0;
		}
		else if (_tcscmp(token, "my_odbc_database") == 0) 
		{
			// database 명
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		StringCchCopy(m_szMyOdbcDatabase, DB_NAME_LENGTH+1, token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nMyOdbcPoolSize = ::_tstoi(token);
			else			return 0;
		}
		else if (_tcscmp(token, "my_odbc_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)		m_nMyOdbcResponseTime = ::_tstoi(token);
			else			return 0;
		}

		/////////////////////////////////////////////////////////////////////////////
		// Other server information

		// Login Server
		else if (_tcscmp(token, "login_server") == 0) 
		{
			// login_server login1.ran-online.co.kr 211.203.233.100 5001 6001 ran ran
			// login_server [server_name] [ip] [service port] [control port] [userid] [userpass]
			// [server_name]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy( m_sLOGINServer[m_nLOGINServerNumber].szServerName, SERVER_NAME_LENGTH+1, token );
			else       return 0;
			// [ip]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				StringCchCopy( m_sLOGINServer[m_nLOGINServerNumber].szServerIP, MAX_IP_LENGTH+1, token );
				// [string ip -> binary Internet address]
				unsigned long ulAddr = ::inet_addr( m_sLOGINServer[m_nLOGINServerNumber].szServerIP );
				if ( ulAddr==INADDR_NONE )
				{
					return 0;
				}
				else
				{
					m_sLOGINServer[m_nLOGINServerNumber].ulServerIP = ulAddr;
				}
			}
			else
			{
				return 0;
			}
			// [service port]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) m_sLOGINServer[m_nLOGINServerNumber].nServicePort = _tstoi(token);
			else       return 0;
			// [control port]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) m_sLOGINServer[m_nLOGINServerNumber].nControlPort = _tstoi(token);
			else       return 0;
			// [userid]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(m_sLOGINServer[m_nLOGINServerNumber].szUserID, USR_ID_LENGTH+1, token);
			else       return 0;
			// [userpass]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(m_sLOGINServer[m_nLOGINServerNumber].szUserPass, USR_PASS_LENGTH+1, token);
			else       return 0;
			m_nLOGINServerNumber++;
			return 0;
		}
		///////////////////////////////////////////////////////////////////////
		// Session Server
		else if ( _tcscmp(token, "session_server") == 0 )
		{
			// session_server [server_name] [ip] [port] [userid] [password]
			// server_name
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szServerName, SERVER_NAME_LENGTH+1, token);
			else       return 0;
			// ip
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)
			{
				StringCchCopy( m_sSESSIONServer[m_nSESSIONServerNumber].szServerIP, MAX_IP_LENGTH+1, token );
				unsigned long ulAddr = ::inet_addr( m_sSESSIONServer[m_nSESSIONServerNumber].szServerIP );
				if ( ulAddr==INADDR_NONE )
				{
					return 0;
				}
				else
				{
					m_sSESSIONServer[m_nSESSIONServerNumber].ulServerIP = ulAddr;
				}
			}
			else
			{
				return 0;
			}
			// port
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) m_sSESSIONServer[m_nSESSIONServerNumber].nServicePort = _tstoi(token);
			else       return 0;
			// userid
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserID, USR_ID_LENGTH+1, token);
			else       return 0;
			// password
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserPass, USR_PASS_LENGTH+1, token);
			else       return 0;
			m_nSESSIONServerNumber++;
			return 0;
		}
		///////////////////////////////////////////////////////////////////////
		// Channel
		else if (_tcscmp(token, "channel") == 0)
		{
			// channel [nubmer] [pk on/off]
			int nChannelPK = 1;
			int nPK;

			token = ::_tcstok_s(NULL, seps, &nToken);
			
			if (token) nChannelPK = ::_tstoi(token);
			else return 0;

			if (nChannelPK < 0 || nChannelPK >= MAX_CHANNEL_NUMBER)	return 0;

			token = ::_tcstok_s(NULL, seps, &nToken);

			if (token) nPK = ::_tstoi(token);
			else return 0;

			if (nPK == 1) m_sChannelInfo[nChannelPK].bPK = true;
			else          m_sChannelInfo[nChannelPK].bPK = false;
		}
		///////////////////////////////////////////////////////////////////////
		// Agent Server
		else if (_tcscmp(token, "agent_server") == 0) 
		{			
			// agent_server [server_name] [ip]
			// server_name			 
			token = ::_tcstok_s(NULL, seps, &nToken);
			if ( !token )
			{
				return 0;
			}
			// ip
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy( m_szAgentIP, MAX_IP_LENGTH+1, token );
			else return 0;
		}
		///////////////////////////////////////////////////////////////////////
		// Field Server		
		else if (_tcscmp(token, "field_server") == 0) 
		{				
			// field_server [Channel] [serverID] [server_name] [ip] [port]			
			int nServerID = 0;
			int nChannel  = 0;

			// [Channel]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	nChannel = _tstoi(token);
			else 		return 0;

			if (nChannel >= MAX_CHANNEL_NUMBER)
			{
				return 0;
			}
			
			// [Server id]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token)	nServerID = _tstoi(token);
			else 		return 0;

			if (nServerID >= FIELDSERVER_MAX)
			{
				return 0;
			}
			
			// 0 번 채널의 필드서버 갯수를 세팅해 놓는다.
            if ( 0 == nChannel )
			{
				if ( nServerID > m_nMaxFieldNumber )
				{
					m_nMaxFieldNumber = nServerID;
				}
			}

			F_SERVER_INFO sFieldSVR;

			// [server name]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) 	StringCchCopy(sFieldSVR.szServerName, SERVER_NAME_LENGTH+1, token);
			else 		return 0;

			// [ip]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) StringCchCopy(sFieldSVR.szServerIP, MAX_IP_LENGTH+1, token);
			else return 0;

			// [string ip -> binary Internet address]
			unsigned long ulAddr = ::inet_addr( sFieldSVR.szServerIP );
			if ( ulAddr==INADDR_NONE )
			{
				return 0;
			}
			else
			{
				sFieldSVR.ulServerIP = ulAddr;
			}

			// [port]
			token = ::_tcstok_s(NULL, seps, &nToken);
			if (token) {
				sFieldSVR.nServicePort = _tstoi(token);
			} else {
				return 0;
			}			

			for ( int i=0; i<FIELDSERVER_MAX; i++ )
			{
				if ( (m_sFIELDServer[nChannel][i].ulServerIP == sFieldSVR.ulServerIP) &&
					 (m_sFIELDServer[nChannel][i].nServicePort == sFieldSVR.nServicePort) )
				{
					MessageBox(
						NULL,
						"field_server [Channel] [serverID] [server_name] [ip] [port]\r\n"
						"Same IP/PORT Field Server Detected",
						"ERROR",
						MB_OK );
					return 0;
				}
			}

			m_sFIELDServer[nChannel][nServerID] = sFieldSVR;

			return 0;
		}

		// Get next token
		token = ::_tcstok_s( NULL, seps, &nToken );
	}
	return 0;
}	

F_SERVER_INFO* CCfg::GetFieldServer(int nServerID, int nServerChannel)
{
	if ( nServerID < 0 || 
		 nServerID >= FIELDSERVER_MAX ||
		 nServerChannel < 0 ||
		 nServerChannel >= MAX_CHANNEL_NUMBER)
	{
		return NULL;
	}
	else
	{
		return &m_sFIELDServer[nServerChannel][nServerID];
	}
}

void CCfg::SetServerVersion(int nVer) {	m_nServerVersion = nVer; }
void CCfg::SetPatchVersion(int nVer)  { m_nPatchVersion = nVer;  }

int	CCfg::GetSessionServerPort(){ return m_sSESSIONServer[0].nServicePort; }
int CCfg::GetServerVersion()	{ return m_nServerVersion; }
int CCfg::GetPatchVersion()		{ return m_nPatchVersion; }

int CCfg::GetServicePort()		{ return m_nPortService; }
int CCfg::GetControlPort()		{ return m_nPortControl; }

int CCfg::GetServerGroup()		{ return m_nServerGroup; }
int CCfg::GetServerNumber()		{ return m_nServerNumber; }
int CCfg::GetServerField()		{ return m_nServerField; }
int CCfg::GetServerType()		{ return m_nServerType; }
int CCfg::GetServerMaxClient()	{ return m_nServerMaxClient; }

int CCfg::GetServerChannel()    { return m_nServerChannel; }
int CCfg::GetServerChannelNumber() { return m_nServerChannelNumber; }
int CCfg::GetServerChannelMaxClient() { return m_nMaxChannelUser; }

int	CCfg::GetServiceProvider()  { return m_nServiceProvider; }
int	CCfg::GetMaxThread(void)    { return m_nMaxThread; }

const TCHAR* CCfg::GetServerIP() { return m_szServerIP; }
TCHAR* CCfg::GetAgentIP()		{ return m_szAgentIP; }
TCHAR* CCfg::GetSessionServerIP(void) { return &m_sSESSIONServer[0].szServerIP[0] ; }

bool CCfg::HeartBeatCheck(void) { return m_bHeartBeat; }
bool CCfg::GemeGuardAuth(void)  { return m_bGameGuardAuth; }
bool CCfg::IsTestServer(void)   { return m_bTestServer; }
bool CCfg::AdultCheck(void)     { return m_bAdultCheck; }
bool CCfg::IsPKServer(void)     { return m_bPK; }

BOOL CCfg::IsUseIntelTBB(void)		{ return m_bUseIntelTBB; }
BOOL CCfg::IsUserEventThread(void)  { return m_bUseEventThread; }
int	 CCfg::GetLangSet(void) { return m_nLangSet; }

//-- 서버점검시 로그인서버에 접속할 수 있는 IP목록

int CCfg::GetAllowIPType1Size( void ) { return int( m_vecAllowIPType1.size() ); }
int CCfg::GetAllowIPType2Size( void ) { return int( m_vecAllowIPType2.size() ); }
int CCfg::GetAllowIPType3Size( void ) { return int( m_vecAllowIPType3.size() ); }

void CCfg::GetAllowIPType1( int nCount, CString& strAllowIP ) { strAllowIP = m_vecAllowIPType1[ nCount ]; }
void CCfg::GetAllowIPType2( int nCount, CString& strAllowIP ) { strAllowIP = m_vecAllowIPType2[ nCount ]; }
void CCfg::GetAllowIPType3( int nCount, CString& strAllowIP ) { strAllowIP = m_vecAllowIPType3[ nCount ]; }

bool CCfg::GetAllowFileLoad( void ) { return m_bAllowFileLoad; }
void CCfg::SetAllowFileLoad( bool bAllowFileLoad ) { m_bAllowFileLoad = bAllowFileLoad; }

//-- 일본 WhiteRock System 설정 변수-------------------------------------//
TCHAR* CCfg::GetWhiteRockName(void)		{ return m_szWhiteRockServerName; }
int	CCfg::GetWhiteRockPort(void)		{ return m_nWhiteRockServerPort; }

///////////////////////////////////////////////////////////////////////////
// DB-Lib C
// User
TCHAR*	CCfg::GetUserDBServer(void)			{ return m_szUserDBServer; }
TCHAR*	CCfg::GetUserDBUser(void)			{ return m_szUserDBUser; }
TCHAR*	CCfg::GetUserDBPass(void)			{ return m_szUserDBPass; }
TCHAR*	CCfg::GetUserDBDatabase(void)		{ return m_szUserDBDatabase; }
int		CCfg::GetUserDBResponseTime(void)	{ return m_nUserDBResponseTime; }
int		CCfg::GetUserDBPoolSize()			{ return m_nUserDBPoolSize; }

// Game
TCHAR*	CCfg::GetGameDBServer(void)			{ return m_szGameDBServer; }
TCHAR*	CCfg::GetGameDBUser(void)			{ return m_szGameDBUser; }
TCHAR*	CCfg::GetGameDBPass(void)			{ return m_szGameDBPass; }
TCHAR*	CCfg::GetGameDBDatabase(void)		{ return m_szGameDBDatabase; }
int		CCfg::GetGameDBResponseTime(void)	{ return m_nGameDBResponseTime; }
int		CCfg::GetGameDBPoolSize()			{ return m_nGameDBPoolSize; }

///////////////////////////////////////////////////////////////////////////
// ODBC
// User
TCHAR*	CCfg::GetUserOdbcName(void)			{ return m_szUserOdbcName; }
TCHAR*	CCfg::GetUserOdbcUser(void)			{ return m_szUserOdbcUser; }
TCHAR*	CCfg::GetUserOdbcPass(void)			{ return m_szUserOdbcPass; }
TCHAR*	CCfg::GetUserOdbcDatabase(void)		{ return m_szUserOdbcDatabase; }
int		CCfg::GetUserOdbcResponseTime(void)	{ return m_nUserOdbcResponseTime; }
int		CCfg::GetUserOdbcPoolSize()			{ return m_nUserOdbcPoolSize; }

// Game
TCHAR*	CCfg::GetGameOdbcName(void)			{ return m_szGameOdbcName; }
TCHAR*	CCfg::GetGameOdbcUser(void)			{ return m_szGameOdbcUser; }
TCHAR*	CCfg::GetGameOdbcPass(void)			{ return m_szGameOdbcPass; }
TCHAR*	CCfg::GetGameOdbcDatabase(void)		{ return m_szGameOdbcDatabase; }
int		CCfg::GetGameOdbcResponseTime(void)	{ return m_nGameOdbcResponseTime; }
int		CCfg::GetGameOdbcPoolSize()			{ return m_nGameOdbcPoolSize; }

// Board
TCHAR*	CCfg::GetBoardOdbcName(void)		{ return m_szBoardOdbcName; }
TCHAR*	CCfg::GetBoardOdbcUser(void)		{ return m_szBoardOdbcUser; }
TCHAR*	CCfg::GetBoardOdbcPass(void)		{ return m_szBoardOdbcPass; }
TCHAR*	CCfg::GetBoardOdbcDatabase(void)	{ return m_szBoardOdbcDatabase; }
int		CCfg::GetBoardOdbcResponseTime(void){ return m_nBoardOdbcResponseTime; }
int		CCfg::GetBoardOdbcPoolSize()		{ return m_nBoardOdbcPoolSize; }

// Log
TCHAR*	CCfg::GetLogOdbcName(void)		    { return m_szLogOdbcName; }
TCHAR*	CCfg::GetLogOdbcUser(void)		    { return m_szLogOdbcUser; }
TCHAR*	CCfg::GetLogOdbcPass(void)		    { return m_szLogOdbcPass; }
TCHAR*	CCfg::GetLogOdbcDatabase(void)	    { return m_szLogOdbcDatabase; }
int		CCfg::GetLogOdbcResponseTime(void)  { return m_nLogOdbcResponseTime; }
int		CCfg::GetLogOdbcPoolSize()		    { return m_nLogOdbcPoolSize; }

// Shop
TCHAR*	CCfg::GetShopOdbcName(void)		    { return m_szShopOdbcName; }
TCHAR*	CCfg::GetShopOdbcUser(void)		    { return m_szShopOdbcUser; }
TCHAR*	CCfg::GetShopOdbcPass(void)		    { return m_szShopOdbcPass; }
TCHAR*	CCfg::GetShopOdbcDatabase(void)	    { return m_szShopOdbcDatabase; }
int		CCfg::GetShopOdbcResponseTime(void) { return m_nShopOdbcResponseTime; }
int		CCfg::GetShopOdbcPoolSize()		    { return m_nShopOdbcPoolSize; }

// Terra
TCHAR*	CCfg::GetTerraOdbcName(void)		{ return m_szTerraOdbcName; }
TCHAR*	CCfg::GetTerraOdbcUser(void)		{ return m_szTerraOdbcUser; }
TCHAR*	CCfg::GetTerraOdbcPass(void)		{ return m_szTerraOdbcPass; }
TCHAR*	CCfg::GetTerraOdbcDatabase(void)	{ return m_szTerraOdbcDatabase; }
int		CCfg::GetTerraOdbcResponseTime(void){ return m_nTerraOdbcResponseTime; }
int		CCfg::GetTerraOdbcPoolSize()		{ return m_nTerraOdbcPoolSize; }

// GSP
TCHAR*	CCfg::GetGspOdbcName(void)		{ return m_szGspOdbcName; }
TCHAR*	CCfg::GetGspOdbcUser(void)		{ return m_szGspOdbcUser; }
TCHAR*	CCfg::GetGspOdbcPass(void)		{ return m_szGspOdbcPass; }
TCHAR*	CCfg::GetGspOdbcDatabase(void)	{ return m_szGspOdbcDatabase; }
int		CCfg::GetGspOdbcResponseTime(void){ return m_nGspOdbcResponseTime; }
int		CCfg::GetGspOdbcPoolSize()		{ return m_nGspOdbcPoolSize; }

// Korea
TCHAR*	CCfg::GetKorOdbcName(void)		{ return m_szKorOdbcName; }
TCHAR*	CCfg::GetKorOdbcUser(void)		{ return m_szKorOdbcUser; }
TCHAR*	CCfg::GetKorOdbcPass(void)		{ return m_szKorOdbcPass; }
TCHAR*	CCfg::GetKorOdbcDatabase(void)	{ return m_szKorOdbcDatabase; }
int		CCfg::GetKorOdbcResponseTime(void){ return m_nKorOdbcResponseTime; }
int		CCfg::GetKorOdbcPoolSize()		{ return m_nKorOdbcPoolSize; }

// Malaysia
TCHAR*	CCfg::GetMyOdbcName(void)		{ return m_szMyOdbcName; }
TCHAR*	CCfg::GetMyOdbcUser(void)		{ return m_szMyOdbcUser; }
TCHAR*	CCfg::GetMyOdbcPass(void)		{ return m_szMyOdbcPass; }
TCHAR*	CCfg::GetMyOdbcDatabase(void)	{ return m_szMyOdbcDatabase; }
int		CCfg::GetMyOdbcResponseTime(void){ return m_nMyOdbcResponseTime; }
int		CCfg::GetMyOdbcPoolSize()		{ return m_nMyOdbcPoolSize; }

bool CCfg::IsPkChannel(int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER)
	{
		return true;
	}
	return m_sChannelInfo[nChannel].bPK;
}

//! 선도클럽, PK, 학원간 PK 등 모든 적대행위 Disable
bool CCfg::IsPKLessServer(void)
{
	return m_bPkLess;
}
