#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 로그인 메시지
* Thailand
* id / pwd 입력시
*/
void CAgentServer::ThaiMsgLogin (MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_THAILAND || pMsg == NULL) return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing (pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing (pMsg->dwClient, true);
	}

	THAI_NET_LOGIN_DATA* tnld = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	tnld = reinterpret_cast<THAI_NET_LOGIN_DATA *> (pMsg->Buffer);

	/// Tea 알고리듬으로 교체
	/// 2005-11-15 Jgkim
	
	/*
	char* pBuffer = (char*) tnld;
	CRYPT_KEY ck;
	ck = m_pClientManager->GetCryptKey (dwClient);
	m_Bit.buf_decode (pBuffer+sizeof(NET_MSG_GENERIC), 
					  (tnld->nmg.dwSize)-sizeof(NET_MSG_GENERIC),
					  ck.nKeyDirection,
					  ck.nKey);
	*/
	m_Tea.decrypt( tnld->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( tnld->szPassword, USR_PASS_LENGTH+1 );
	
	// 접속하려는 채널 검사
	int nChannel = tnld->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL; // 채널이 Full 이라고 메시지를 보낸다.
		SendClient (dwClient, &nlfd); // 결과전송
//		CloseClient (dwClient); // 접속종료

		CConsoleMessage::GetInstance()->WriteConsole(
			_T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			nChannel );
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull (nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient (dwClient, &nlfd); // 결과전송
//		CloseClient (dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel (dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser (nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel (dwClient, nChannel);
		}
		else // 기존접속자
		{
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID (dwClient, tnld->szUserid);		
	
	// 인증통과 실패 세팅
	m_pClientManager->SetAccountPass (dwClient, false); 

	// DB 에 인증요청
	CAgentThaiUserCheck* pAction = new CAgentThaiUserCheck (tnld->szUserid, 
										                    tnld->szPassword,
												            m_pClientManager->GetClientIP(dwClient),
												            m_nServerGroup,
										                    m_nServerNum,
										                    0,
										                    dwClient);
	
	COdbcManager::GetInstance()->AddUserJob ((CDbAction*) pAction);
}

void CAgentServer::ThaiMsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA	nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD dwClient = (DWORD) nlfd2->nClient;
	DWORD dwSndBytes = 0;
	int	nUserNum   = 0;		

    // 로그인 성공
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			 (strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0))
		{
			nlfd.nResult    = nlfd2->nResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.uChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  ( dwClient, true );
			m_pClientManager->SetUserNum      ( dwClient, nlfd2->nUserNum );       // 유저번호
			m_pClientManager->SetLoginTime    ( dwClient );                        // 로그인 시간
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // 유저타입세팅
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // 채팅 블록 기간
			m_pClientManager->ThaiSetFlag     ( dwClient, nlfd2->wThaiFlag );      // Thailand 사용자 플래그
			m_pClientManager->SetThaiClass	  ( dwClient, nlfd2->nThaiCC_Class );  // Thailand 사용자 Class Type

			
			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
			
			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}
			
			// 극강부 추가
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			// 18 세 이하일때는 오늘 게임한 플레이 시간(분)을 가져와야 한다.
			if ( nlfd2->wThaiFlag == THAI_UNDER18 )
			{
				// 오늘 플레이한 시간 가져오기
				WORD wTodayGameTime = COdbcManager::GetInstance()->ThaiGetTodayPlayTime( nlfd2->nUserNum );
				// 오늘 게임플레이 시간(분) 세팅
				m_pClientManager->ThaiSetTodayPlayTime( dwClient, wTodayGameTime );
			}

			m_pClientManager->SetAccountPass( dwClient, true ); // 인증통과했음

			SendClient( dwClient, &nlfd ); // 결과전송
			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// 로그아웃 시킨다.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
				_T("MsgLoginBack User Already Offline") );
	
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
                _T("MsgLoginBack User IP/ID Wrong(%s)"),
                nlfd2->szUserid );

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass (dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline (dwClient) == true) &&
			(strcmp (m_pClientManager->GetClientIP (dwClient), nlfd2->szIp) == 0) &&
			(strcmp (m_pClientManager->GetUserID (dwClient), nlfd2->szUserid) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient (dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline (dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
                _T("MsgLoginBack User Login Fail And Offline(%s)"),
				nlfd2->szUserid );
		
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			CConsoleMessage::GetInstance()->Write (
		                                          _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}
}

//! China 로그인 메시지
void CAgentServer::ChinaMsgLogin( MSG_LIST* pMsg )
{
	

	// 중국 클라이언트가 보낸 메시지 인지 확인한다.
	if (m_nServiceProvider != SP_CHINA || pMsg == NULL) return;	

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing( pMsg->dwClient ) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( pMsg->dwClient, true );
	}

	CHINA_NET_LOGIN_DATA* pNml = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	pNml = reinterpret_cast<CHINA_NET_LOGIN_DATA *> (pMsg->Buffer);

	if (sizeof(CHINA_NET_LOGIN_DATA) != pNml->nmg.dwSize)
	{
		CConsoleMessage::GetInstance()->Write(
			
			_T("ERROR:CHINA_NET_LOGIN_DATA Wrong Message Size") );
		return;
	}

	// 복호화
	m_Tea.decrypt( pNml->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( pNml->szPassword, USR_PASS_LENGTH );
	m_Tea.decrypt( pNml->szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};

	StringCchCopy( szUserid, USR_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_PASS_LENGTH, pNml->szPassword );
	StringCchCopy( szRandomPassword, USR_RAND_PASS_LENGTH+1, pNml->szRandomPassword );
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			
			_T("ERROR:ChinaMsgLogin Invalid Channel Number (Channel:%d)"),
			nChannel );
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull( nChannel ) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료
		CConsoleMessage::GetInstance()->Write(
			
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient, nChannel );
		}
		else // 기존접속자
		{
			
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB 에 인증요청
	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패 세팅
	
	CAgentUserCheck* pAction = new CAgentUserCheck(
		szUserid, 
		szPassword,
		m_pClientManager->GetClientIP( dwClient ),
		szRandomPassword,
		m_pClientManager->GetRandomPassNumber( dwClient ),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient,
		static_cast<DWORD> (m_nServiceProvider) );
	
	COdbcManager::GetInstance()->AddUserJob( (CDbAction*) pAction );
}

void CAgentServer::ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

    // 로그인 성공
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ),   nlfd2->szUserid ) == 0))
		{
			nlfd.nResult    = nlfd2->nResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.uChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  ( dwClient, true );
			m_pClientManager->SetUserNum      ( dwClient, nlfd2->nUserNum );       // 유저번호
			m_pClientManager->SetLoginTime    ( dwClient );                        // 로그인 시간
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // 유저타입세팅
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // 채팅 블록 기간

			m_pClientManager->SetChinaGameTime( dwClient, nlfd2->nChinaGameTime ); // 중국 게임시간 세팅
			m_pClientManager->SetChinaOfflineTime( dwClient, nlfd2->nChinaOfflineTime ); // 중국 오프라인 시간 세팅
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 중국 LastLoginDate 세팅
			m_pClientManager->SetChinaUserAge( dwClient, nlfd2->nChinaUserAge ); // 중국 UserAge 세팅

			m_pClientManager->SetAccountPass( dwClient, true ); // 인증통과했음

			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
			
			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// 극강부 남/여 생성 가능 숫자를 넘겨준다.

			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;			

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient( dwClient, &nlfd ); // 결과전송

			// Apex 적용
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

//			char* szIp = m_pClientManager->GetClientIP( dwClient );
//			int nIp  = inet_addr(szIp);
			
//			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			
			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			// 로그아웃 시킨다.
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szUserid );			
			CConsoleMessage::GetInstance()->Write(
				
		        _T("ChinaMsgLoginBack User Already Offline"));
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szUserid );			

			CConsoleMessage::GetInstance()->Write(
				
		        _T("MsgLoginBack User IP/ID Wrong(%s)"),
				nlfd2->szUserid );

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패
		m_pClientManager->SetAccountPassing( dwClient, false );

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ), nlfd2->szUserid ) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient( dwClient, &nlfd );
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			CConsoleMessage::GetInstance()->Write(				
		        _T("MsgLoginBack User Login Fail And Offline(%s)"),
				nlfd2->szUserid );

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			CConsoleMessage::GetInstance()->Write(
				
		        _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
				nlfd2->szUserid );
			return;
		}
	}
}

/**
* 일반 로그인 : id / pwd 입력시
*/
void CAgentServer::MsgLogIn(MSG_LIST* pMsg)
{		
	if (pMsg == NULL)
		return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	NET_LOGIN_DATA* pNml = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	pNml = reinterpret_cast<NET_LOGIN_DATA *> (pMsg->Buffer);

	if (sizeof(NET_LOGIN_DATA) != pNml->nmg.dwSize)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

	/// 암호화 알고리듬 교체
	/// 2005-11-16 Jgkim
	
	/*
	char* pBuffer = (char*) nml;
	CRYPT_KEY ck;
	ck = m_pClientManager->GetCryptKey(dwClient);	
	m_Bit.buf_decode(pBuffer+sizeof(NET_MSG_GENERIC), 
					 (nml->nmg.dwSize)-sizeof(NET_MSG_GENERIC),
					 ck.nKeyDirection,
					 ck.nKey);
	*/
	m_Tea.decrypt (pNml->szUserid, USR_ID_LENGTH+1);
	m_Tea.decrypt (pNml->szPassword, USR_PASS_LENGTH+1);
	m_Tea.decrypt (pNml->szRandomPassword, USR_RAND_PASS_LENGTH+1);
	m_Tea.decrypt (pNml->szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};


	StringCchCopy (szUserid, USR_ID_LENGTH+1, pNml->szUserid);
	StringCchCopy (szPassword, USR_PASS_LENGTH+1, pNml->szPassword);
	StringCchCopy (szRandomPassword, USR_RAND_PASS_LENGTH+1, pNml->szRandomPassword);
	StringCchCopy (szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt);

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{

		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:MsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

//		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);

		/*if( m_ChannelUser[nChannel] != (m_pClientManager->GetCurrentClientNumber() - 1) )
		{
			CConsoleMessage::GetInstance()->Write( _T("INFO:Different CHUserNum(%d) and ClientNum(%d)"), m_ChannelUser[nChannel],
				m_pClientManager->GetCurrentClientNumber() );
			LockOn();
			m_ChannelUser[nChannel] = m_pClientManager->GetCurrentClientNumber() - 1;
			LockOff();

		}*/

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);

			/*CConsoleMessage::GetInstance()->Write(
				_T("### INCREASE_CHANNEL_USER_CLIENT_ID %d"), dwClient );*/
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}
/*
	// WhiteRock과의 통신 하는 부분을 Thread 처리
	CAgentJapnaWRLogin* pAction = new CAgentJapnaWRLogin(
													szUserid,
													szPassword,
													m_pClientManager->GetClientIP(dwClient),
													m_nServerGroup,
													m_nServerNum,
													dwClient
													);

	COdbcManager::GetInstance()->AddWebJob((CDbAction*) pAction);
*/

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	CAgentUserCheck* pAction = new CAgentUserCheck( 
		szUserid, 
		szPassword,
		m_pClientManager->GetClientIP(dwClient),
		szRandomPassword,
		m_pClientManager->GetRandomPassNumber(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient,
		static_cast<DWORD> (m_nServiceProvider) );
	
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);

}

void CAgentServer::GsMsgLogin( MSG_LIST* pMsg )
{
	if (pMsg == NULL)
		return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	GS_NET_LOGIN_DATA* pNml = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	pNml = reinterpret_cast<GS_NET_LOGIN_DATA *> (pMsg->Buffer);

	if (sizeof(GS_NET_LOGIN_DATA) != pNml->nmg.dwSize)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:GS_NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

	m_Tea.decrypt (pNml->szUserid, GS_USER_ID+1);
	m_Tea.decrypt (pNml->szPassword, GS_USER_PASS+1);
	m_Tea.decrypt (pNml->szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};


	StringCchCopy (szUserid, USR_ID_LENGTH+1, pNml->szUserid);
	StringCchCopy (szPassword, USR_PASS_LENGTH+1, pNml->szPassword);
	StringCchCopy (szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt);

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{

		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = GS_NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:GsMsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:GsMsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

//		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);

			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	CAgentGsUserCheck* pAction = new CAgentGsUserCheck( 
		szUserid, 
		szPassword,
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient,
		static_cast<DWORD> (m_nServiceProvider) );
	
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

void CAgentServer::GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	//int		nTestNum   = 100;

    // 로그인 성공
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0) /*&&
			nTestNum != 100*/ )
		{
			nlfd.nResult    = nlfd2->nResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.uChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
			
			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			// 로그아웃 시킨다.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Already Offline"));			

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szUserid);			

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) /*&&
			nTestNum != 100*/  )
		{
			if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			{

				bool bFind = FALSE;
				int i;
				for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
				{
					// 온라인 상태인경우
					if ( /*(strcmpi(m_pClientManager->GetUserID(i), nlfd2->szUserid) == 0) ||*/
						  m_pClientManager->GetUserNum(i) == nlfd2->nUserNum )
					{
						if( i != dwClient )
						{
							CloseClient(i);
							bFind = TRUE;
							CConsoleMessage::GetInstance()->Write(_T("GsMsgLoginBack Duplication Login CloseClient(ID: %s ClientID: %d )"),
																	nlfd2->szUserid, dwClient );
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);

					CConsoleMessage::GetInstance()->Write(_T("GsMsgLoginBack Duplication Login UserLogoutSimple(ID: %s ClientID: %d )"),
															nlfd2->szUserid, dwClient );
				}
			}


			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->szUserid);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}

}

/*
* 일반 로그인 결과 : id / pwd 입력시
*/
void CAgentServer::MsgLogInBack (NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	//int		nTestNum   = 100;

    // 로그인 성공
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0) /*&&
			nTestNum != 100*/ )
		{
			nlfd.nResult    = nlfd2->nResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.uChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			
/*
			// 내부 테스트를 위해서 말레이시아 User Class Type을 추가한다.
			m_pClientManager->SetMyClass	  ( dwClient, nlfd2->nMyCC_Class );  // 말레이시아 사용자 Class Type
*/
//			m_pClientManager->SetThaiClass	  ( dwClient, nlfd2->nThaiCC_Class ); // 테스트용

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 중국, 베트남 LastLoginDate 세팅
#if defined( VN_PARAM ) || defined(_RELEASED) //vietnamtest%%%
			m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // 베트남 게임시간 세팅
#endif

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
			
			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// 극강부 남/여 생성 가능 숫자를 넘겨준다.
// 극강부 추가
#if	defined(RZ_PARAM) || defined(_RELEASED) || defined(KRT_PARAM) || defined(KR_PARAM) || defined ( TW_PARAM ) || defined(HK_PARAM) 
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif
			nlfd.nCheckFlag = nlfd2->nCheckFlag; // 한국 CheckFlag Test를 위해서 추가

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
			int nTracing = CheckTracingUser( nlfd2->szUserid, nlfd2->nUserNum );
			if( nTracing != -1 )
			{
				m_vecTracingData[nTracing].dwOnLine       = TRUE;
				m_vecTracingData[nTracing].dwUserNum      = nlfd2->nUserNum;
				m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
				m_vecTracingData[nTracing].strAccount     = nlfd2->szUserid;

				CDebugSet::ToTracingFile( nlfd2->szUserid, "############ Tracing User Login ############" );
				CDebugSet::ToTracingFile( nlfd2->szUserid, "Account: [%s], UserNum: [%u], IP: [%s]", nlfd2->szUserid, nlfd2->nUserNum, nlfd2->szIp );
				NET_UPDATE_TRACINGCHAR msg;
				msg.tracingData = m_vecTracingData[nTracing];
				msg.updateNum   = nTracing;
							
				SendSession( &msg );
			}

#endif

#if defined ( HK_PARAM )  || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex 적용( 홍콩 )
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

			char* szIp = m_pClientManager->GetClientIP( dwClient );
			int nIp  = inet_addr(szIp);

			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
#endif

			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			// 로그아웃 시킨다.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Already Offline"));			

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

				return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szUserid);			

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) /*&&
			nTestNum != 100*/  )
		{
			if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			{

				bool bFind = FALSE;
				int i;
				for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
				{
					// 온라인 상태인경우
					if ( /*(strcmpi(m_pClientManager->GetUserID(i), nlfd2->szUserid) == 0) ||*/
						  m_pClientManager->GetUserNum(i) == nlfd2->nUserNum )
					{
						if( i != dwClient )
						{
							CloseClient(i);
							bFind = TRUE;
							CConsoleMessage::GetInstance()->Write(_T("MsgLoginBack Duplication Login CloseClient(ID: %s ClientID: %d )"),
																	nlfd2->szUserid, dwClient );
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);

					CConsoleMessage::GetInstance()->Write(_T("MsgLoginBack Duplication Login UserLogoutSimple(ID: %s ClientID: %d )"),
															nlfd2->szUserid, dwClient );
				}
			}


			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->szUserid);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// 다음 : 로그인
/*
void CAgentServer::DaumMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_KOREA) return;

	if (pMsg == NULL) return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	char szDaumGID[DAUM_MAX_GID_LENGTH+1] = {0};
	char szDaumUID[DAUM_MAX_UID_LENGTH+1] = {0};
	char szDaumSSNHEAD[DAUM_MAX_SSNHEAD_LENGTH+1] = {0};
	char szDaumSEX[DAUM_MAX_SEX_LENGTH+1] = {0};

	CString strDaumGID;
	CString strDaumUID;
	CString strDaumSSNHEAD;
	CString strDaumSEX;	

	DWORD dwClient=0;

	DAUM_NET_LOGIN_DATA* pLogin = NULL;
    pLogin = reinterpret_cast<DAUM_NET_LOGIN_DATA*> (pMsg->Buffer);	

	if (pLogin == NULL) return;

	dwClient = pMsg->dwClient;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:DaumMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	try
	{
		if (m_DaumAuth.SetSource(pLogin->szParameter) == false)
		{
			// 정상적이지 않은 데이터이기 때문에 잘못된 사용자임.
			nlfd.nResult = EM_LOGIN_FB_SUB_UNCON;
			SendClient(dwClient, &nlfd);
			CConsoleMessage::GetInstance()->Write(
				                                  _T("DaumMsgLogin SetSource Failed (%s)"),
												  pLogin->szParameter);
			return;
		}
		else
		{
			// "GID": GameID   (사용자가 직접 입력했던 ID) - e.g. test1     16 자리
			// "UID": UniqueID (시스템이 부여한 사용자 ID) - e.g. 15ftgr     6 자리
			// "SSNHEAD": 생년월일 - e.g. 731201                             6 자리
			// "SEX": 성별(M:남자, F:여자)                                   1 자리
			// "NAMECHECK": 실명확인 여부(1=했음, 0=안했음)                  1 자리
			// "IP": 접속한 IP
			// "STATUS": 사용자 상태(A: 정회원, W: 준회원(기획예정), D: 탈퇴신청회원, B: 불량유저)
			
			if (m_DaumAuth.IsTimeExpired() == TRUE) 
			{
				// 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다.			
				nlfd.nResult = EM_LOGIN_FB_SUB_EXPIRED;
				SendClient(dwClient, &nlfd);				
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:Login time expired"));
				return;
			}
			// GID를 얻어온다.
			if (m_DaumAuth.GetData("GID", szDaumGID, DAUM_MAX_GID_LENGTH) == FALSE)
			{
				// GID가 없으니 에러처리.			
				nlfd.nResult = EM_LOGIN_FB_SUB_GID_ERR;
				SendClient(dwClient, &nlfd);
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:GID is not exist"));
				return;
			}		
			// UID를 얻어온다.
			if (m_DaumAuth.GetData("UID", szDaumUID, DAUM_MAX_UID_LENGTH) == FALSE)
			{
				// UID가 없으니 에러처리.			
				nlfd.nResult = EM_LOGIN_FB_SUB_UID_ERR;
				SendClient(dwClient, &nlfd);
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:UID is not exist"));
				return;
			}

			if (m_DaumAuth.GetData("SSNHEAD", szDaumSSNHEAD, DAUM_MAX_SSNHEAD_LENGTH) == FALSE)
			{
				// 주민번호 에러처리.			
				nlfd.nResult = EM_LOGIN_FB_SUB_SSNHEAD;
				SendClient(dwClient, &nlfd);
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:SSNHEAD is not exist"));
				return;
			}
			m_DaumAuth.GetData("SEX", szDaumSEX, DAUM_MAX_SEX_LENGTH);

			strDaumGID = szDaumGID;
			strDaumUID = szDaumUID;
			strDaumSSNHEAD = szDaumSSNHEAD;
			strDaumSEX = szDaumSEX;

			strDaumGID.Trim(_T(" "));
			strDaumUID.Trim(_T(" "));
			strDaumSSNHEAD.Trim(_T(" "));
			strDaumSEX.Trim(_T(" "));

			// 성인인증
			if (m_bAdult)
			{
				if (SERVER_UTIL::CheckAdult(strDaumSSNHEAD,18) == false)
				{
					// 성인인증 에러처리.			
					nlfd.nResult = EM_LOGIN_FB_SUB_ADULT;
					SendClient(dwClient, &nlfd);
					CConsoleMessage::GetInstance()->Write(
						                                  _T("Daum:Adult authorize failed")); // 성인인증 실패
					return;
				}
			}

			CConsoleMessage::GetInstance()->WriteConsole(
				_T("(UID:%s)(GID:%s)"),
				strDaumUID.GetString(), 
				strDaumGID.GetString() );

			// 사용자 UID 세팅
			m_pClientManager->SetUserID(dwClient, strDaumUID.GetString());	

			// 사용자 UID 를 비교, 그에 맞는 로그인 피드백 데이타를 전송한다.
			CAgentDaumUserCheck* pAction = new CAgentDaumUserCheck(strDaumGID.GetString(),
														           strDaumUID.GetString(),
									                               strDaumSSNHEAD.GetString(),
														           strDaumSEX.GetString(),
														           m_pClientManager->GetClientIP(dwClient),
														           m_nServerGroup,
											 			           m_nServerNum,
														           0,
														           dwClient);
			COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
		}
	}
	catch (_com_error &e)
	{
		CConsoleMessage::GetInstance()->Write( 
			                                  _T("DAUM:DLL Exception %s"),
											  e.ErrorMessage());
	}
	catch (HRESULT hr)
	{
		_com_error error(hr);
		CConsoleMessage::GetInstance()->Write(
			                                  _T("DAUM:DLL Exception %s"),
											  error.ErrorMessage());
	}
}
*/

void CAgentServer::DaumMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_KOREA) return;

	if (pMsg == NULL) return;

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			
			_T("WARNING:User Login Attempt is over 5 times %s"),
			m_pClientManager->GetClientIP(pMsg->dwClient) );
		CloseClient( pMsg->dwClient ); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	DWORD dwClient=0;

	DAUM_NET_LOGIN_DATA* pLogin = NULL;
    pLogin = reinterpret_cast<DAUM_NET_LOGIN_DATA*> (pMsg->Buffer);	

	if (pLogin == NULL) return;

	dwClient = pMsg->dwClient;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:DaumMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	// UUID Check
	// UUID copy
	TCHAR szUUID[UUID_STR_LENGTH] = {0};
	StringCchCopy( szUUID, UUID_STR_LENGTH, pLogin->szUUID);

	// 로그인 과정이 끝나고 계정을 체크하기 위해서 UUID 를 세팅한다.
	// 해킹을 통해서 다른 UUID 인것처럼 속일 가능성이 있다.
	m_pClientManager->KorSetUUID( dwClient, szUUID );

	// UUID를 이용해서 사용자 정보를 체크한다.
	CAgentDaumUserCheck* pAction = new CAgentDaumUserCheck(
		szUUID,
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient );

	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);	
}

///////////////////////////////////////////////////////////////////////////////
// 다음 : 로그인 결과
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	m_pClientManager->SetUserID(dwClient, nlfd2->szDaumGID);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->KorGetUUID(dwClient), nlfd2->szUUID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			StringCchCopy( nlfd.szDaumGID, DAUM_MAX_GID_LENGTH+1, nlfd2->szDaumGID );

			nlfd.nResult    = nlfd2->nResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

            if (CCfg::GetInstance()->IsTestServer()) 
            {
                nlfd.uChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.uChaRemain = nlfd2->uChaRemain;
            }

			
			// Daum 패스워드를 받기위한 Check Flag 값
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업

			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}
			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

// 극강부 남/여 생성 가능 숫자를 넘겨준다.
// 극강부 추가
#if	defined(RZ_PARAM) || defined(_RELEASED) || defined(KRT_PARAM) || defined(KR_PARAM) || defined ( TW_PARAM ) || defined(HK_PARAM) 
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd);
		}
		// 이미 오프라인
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumGID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szDaumGID);
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumGID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szDaumGID);
			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false); // 인증중 false

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szDaumGID) == 0))
		{

			//if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			//{

			//	bool bFind = FALSE;
			//	int i;
			//	for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
			//	{
			//		// 온라인 상태인경우
			//		if ( (strcmpi(m_pClientManager->GetUserID(i), nlfd2->szDaumGID) == 0)  )
			//		{
			//			if( i != dwClient )
			//			{
			//				CloseClient(i);
			//				CConsoleMessage::GetInstance()->Write(_T("DaumMsgLoginBack Duplication Login CloseClient(ID: %s ClientID: %d )"),
			//														nlfd2->szDaumGID, dwClient );
			//				bFind = TRUE;
			//				break;
			//			}
			//		}
			//	}

			//	if( bFind == FALSE )
			//	{
			//		CConsoleMessage::GetInstance()->Write(_T("DaumMsgLoginBack Duplication Login UserLogoutSimple(ID: %s ClientID: %d )"),
			//												nlfd2->szDaumGID, dwClient );
			//		COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szDaumGID);
			//	}
			//}

			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szDaumGID);
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szDaumGID);
			// 현재사용자는 대기
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// 다음 : 로그인 결과
/*
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szDaumUID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			StringCchCopy( nlfd.szDaumGID, DAUM_MAX_GID_LENGTH+1, nlfd2->szDaumGID );

			nlfd.nResult    = nlfd2->nResult;
            
            if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH) nlfd2->uChaRemain     = 0;
            if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH) nlfd2->uChaTestRemain = 0;

            if (CCfg::GetInstance()->IsTestServer()) 
            {
                nlfd.uChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.uChaRemain = nlfd2->uChaRemain;
            }

			// 극강부 남/여 생성 가능 숫자를 넘겨준다.
#if	defined(_RELEASED) || defined(KRT_PARAM)
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif
			// Daum 패스워드를 받기위한 Check Flag 값
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd);
		}
		// 이미 오프라인
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumUID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szDaumUID);
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumUID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szDaumUID);
			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false); // 인증중 false

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szDaumUID) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szDaumUID);
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szDaumUID);
			// 현재사용자는 대기
			return;
		}
	}
}
*/

void CAgentServer::GspMsgLogin( MSG_LIST* pMsg )
{
	if (m_nServiceProvider != SP_GLOBAL) return;

	if (pMsg == NULL) return;
	
	m_pClientManager->IncLoginAttempt( pMsg->dwClient );

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			
			_T("WARNING:User Login Attempt is over 5 times %s"),
			m_pClientManager->GetClientIP(pMsg->dwClient) );
		CloseClient( pMsg->dwClient ); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing( pMsg->dwClient ) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( pMsg->dwClient, true );
	}	
	
	DWORD dwClient=0;

	GSP_NET_LOGIN_DATA* pLogin = NULL;
    pLogin = reinterpret_cast<GSP_NET_LOGIN_DATA*> (pMsg->Buffer);	

	if (pLogin == NULL) return;

	dwClient = pMsg->dwClient;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			
			_T("ERROR:GspMsgLogin Invalid Channel Number (Channel:%d)"),
			nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull( nChannel ) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료
		CConsoleMessage::GetInstance()->Write(
			
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient, nChannel );
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}	

	// UUID Check
	// UUID copy	
	TCHAR szUUID[UUID_STR_LENGTH] = {0};
	StringCchCopy( szUUID, UUID_STR_LENGTH, pLogin->szUUID);

	// 로그인 과정이 끝나고 계정을 체크하기 위해서 UUID 를 세팅한다.
	// 해킹을 통해서 다른 UUID 인것처럼 속일 가능성이 있다.
	m_pClientManager->GspSetUUID( dwClient, szUUID );

	// UUID를 이용해서 사용자 정보를 체크한다.
	CAgentGspUserCheck* pAction = new CAgentGspUserCheck(
		szUUID,
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient );
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

void CAgentServer::GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;	

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	// GspUserID
	m_pClientManager->GspSetUserID( dwClient, nlfd2->szGspUserID );

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, Decoded TID 가 동일할때...
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			 (strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0) )
		{
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			nlfd.nResult    = nlfd2->nResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

            if (CCfg::GetInstance()->IsTestServer()) 
            {
                nlfd.uChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.uChaRemain = nlfd2->uChaRemain;
            }

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			SendClient(dwClient, &nlfd);
		}
		// 이미 오프라인
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패

			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szGspUserID );
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Already Offline(%s)"),
				nlfd2->szGspUserID );
			return;
		}
		// 로그인성공, 온라인이지만 ip, DecodedTID 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szGspUserID );
			CConsoleMessage::GetInstance()->Write(
				
				_T("GspMsgLoginBack User IP/UUID Wrong(%s:%s)"),
				nlfd2->szGspUserID,
				nlfd2->szUUID );

			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패
		m_pClientManager->SetAccountPassing( dwClient, false );

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			 (strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID ) == 0) )
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient( dwClient, &nlfd );
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Offline AND Login Fail(%s)"),
				nlfd2->szGspUserID );
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Online Wrong(%s)"),
				nlfd2->szGspUserID );
			// 현재사용자는 대기
			return;
		}
	}
}

/**
 * Terra Login message
 * \param pMsg Login message
 */
void CAgentServer::TerraMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_TERRA && m_nServiceProvider != SP_MALAYSIA ) return;

	if (pMsg == NULL) return;

	m_pClientManager->IncLoginAttempt(pMsg->dwClient);

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("WARNING:User Login Attempt is over 5 times %s"),
											  m_pClientManager->GetClientIP(pMsg->dwClient));
		CloseClient(pMsg->dwClient); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	CString strTID;
	
	DWORD dwClient=0;

	TERRA_NET_LOGIN_DATA* pLogin = NULL;
    pLogin = reinterpret_cast<TERRA_NET_LOGIN_DATA*> (pMsg->Buffer);	

	if (pLogin == NULL) return;

	dwClient = pMsg->dwClient;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:TerraMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write( _T("CH(%d) is FULL"), nChannel );

		/*if( m_ChannelUser[nChannel] != m_pClientManager->GetCurrentClientNumber() )
		{
			CConsoleMessage::GetInstance()->Write( _T("INFO:Different CHUserNum(%d) and ClientNum(%d)"), m_ChannelUser[nChannel],
				m_pClientManager->GetCurrentClientNumber() );
			LockOn();
			m_ChannelUser[nChannel] = m_pClientManager->GetCurrentClientNumber();
			LockOff();

		}*/

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	char szDecodedTID[TERRA_TID_DECODE+1] = {0}; // Decoded TID

	// Encoding 된 문자열을 Decoding 한다.
	if (TerraGetDecodedTID(pLogin->szTID, szDecodedTID) == NET_ERROR) 
	{ 		
		// Decoding 실패시 코딩
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
                                              _T("ERROR:TerraGetDecodedTID %s"),
											  pLogin->szTID);
		return;
	}
	else
	{
		// 로그인 과정이 끝나고 계정을 체크하기 위해서 TID 를 세팅한다.
		m_pClientManager->TerraSetDecodedTID(dwClient, szDecodedTID);

		// Decoding 된 TID 를 이용해서 사용자 정보를 체크한다.
		CAgentTerraUserCheck* pAction = new CAgentTerraUserCheck(szDecodedTID,
																 m_pClientManager->GetClientIP(dwClient),
																 m_nServerGroup,
											 					 m_nServerNum,
																 0,
																 dwClient);
		COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
	}
}

/**
* Terra Login message back
* \param nlfd2 로그인 Feedback data
*/
void CAgentServer::TerraMsgLoginBack(TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	// TLoginName 
	m_pClientManager->SetUserID(dwClient, nlfd2->szTerraTLoginName);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, Decoded TID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			StringCchCopy( nlfd.szDaumGID, DAUM_MAX_GID_LENGTH+1, nlfd2->szTerraTLoginName );

			nlfd.nResult    = nlfd2->nResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

            if (CCfg::GetInstance()->IsTestServer()) 
            {
                nlfd.uChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.uChaRemain = nlfd2->uChaRemain;
            }

			// 말레이지아 패스워드를 받기위한 Check Flag 값
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간
#if	defined( MYE_PARAM ) || defined( MY_PARAM ) 
			m_pClientManager->SetMyClass	  ( dwClient, nlfd2->nMyCC_Class );  // 말레이시아 사용자 Class Type
#endif
			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업
			
			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// 극강부 추가
#if	defined( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM )
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

#if defined( VN_PARAM ) //vietnamTEst%%%
			m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // 베트남 게임시간 세팅
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 베트남 LastLoginDate 세팅
#endif			
			SendClient(dwClient, &nlfd);
		}
		// 이미 오프라인
		else if ( m_pClientManager->IsOnline(dwClient) == false )
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szTerraTLoginName);

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 로그인 실패 전송
	
			return;
		}
		// 로그인성공, 온라인이지만 ip, DecodedTID 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User IP/DecodedTID Wrong(%s)"),
												  nlfd2->szTerraTLoginName);

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			CloseClient( dwClient );

			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			{

				bool bFind = FALSE;
				int i;
				for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
				{
					// 온라인 상태인경우
					if (m_pClientManager->GetUserNum(i) == nlfd2->nUserNum )
					{
						if( i != dwClient )
						{
							CloseClient(i);
							bFind = TRUE;
							CConsoleMessage::GetInstance()->Write(_T("TerraMsgLoginBack Duplication Login CloseClient(ID: %s UserNum: %d ClientID: %d )"),
								nlfd2->szTerraTLoginName, nlfd2->nUserNum, dwClient );
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szTerraTLoginName);

					CConsoleMessage::GetInstance()->Write(_T("TerraMsgLoginBack Duplication Login UserLogoutSimple(ID: %s UserNum: %d ClientID: %d )"),
						nlfd2->szTerraTLoginName, nlfd2->nUserNum, dwClient );
				}
			}

			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Offline AND Login Fail(%s)"),
												  nlfd2->szTerraTLoginName);

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 로그인 실패 전송

		
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Online Wrong(%s)"),
												  nlfd2->szTerraTLoginName);
			// 현재사용자는 대기
			return;
		}
	}
}

/**
* Encoding 된 TID 에서 Decoding 된 TID 를 얻는다.
* \param szEncodedTID Encoding 된 TID
* \param szDecodedTID Decoding 된 TID
* \return
*/
int CAgentServer::TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID)
{
#if defined (MY_PARAM)
	if (szEncodedTID == NULL || szDecodedTID == NULL || m_pMyRossoEncrypto == NULL)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:szEncodedTID == NULL || szDecodedTID == NULL || m_pMyRossoEncrypto == NULL"));
		return NET_ERROR;
	} // if (szEncodedTID == NULL || szDecodedTID == NULL || m_pMyRossoEncrypto == NULL)

	_bstr_t bstrEncodedTID = szEncodedTID;
	_bstr_t bstrRestore;
	_bstr_t bstrDest;
	HRESULT hResult;

	hResult = m_pMyRossoEncrypto->Decode(bstrEncodedTID, bstrDest.GetAddress());

	if (FAILED(hResult))
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:m_pMyRossoEncrypto->Decode()"));
		return NET_ERROR;
	}
	else
	{
		StringCchCopy ( szDecodedTID, TERRA_TID_DECODE+1, (char*) bstrDest );
		// Test Code Start
		/*
		FILE* fFile = fopen("TerraTid.txt", "a+");
		fprintf(fFile, "Encoded TID:%s \n", szEncodedTID);
		fprintf(fFile, "Decoded TID:%s \n", szDecodedTID);	
		fclose(fFile);
		*/
		// Test Code End
		return NET_OK;
	}
#else
	return NET_OK;
#endif
}

void CAgentServer::ExciteMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_JAPAN) return;

	if (pMsg == NULL) return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	DWORD dwClient=0;

	EXCITE_NET_LOGIN_DATA* pLogin = NULL;
    pLogin = reinterpret_cast<EXCITE_NET_LOGIN_DATA*> (pMsg->Buffer);	

	if (pLogin == NULL) return;

	dwClient = pMsg->dwClient;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:ExciteMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	CString strUserID = pLogin->szUID;	
	CString strTDate = pLogin->szTDATE;	

	bool bMd5Result = false;
	bMd5Result = m_MinMd5.ExciteGetMd5(pLogin->szUID,
		                               pLogin->szTDATE,
						               pLogin->szMD5);
	if (bMd5Result == false)
	{
		// 정상적이지 않은 데이터이기 때문에 잘못된 사용자임.
		nlfd.nResult = EM_LOGIN_FB_SUB_UNCON;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ExciteMsgLogin ExciteGetMd5 false (%s)"),
											  pLogin->szUID);
		return;
	} // if (bMd5Result == FALSE)

	bool bDateCheck = false;
	bDateCheck = m_MinMd5.ExciteCheckDate(strTDate);
	if (bDateCheck == false)
	{
		// 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다.			
		nlfd.nResult = EM_LOGIN_FB_SUB_EXPIRED;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ExciteMsgLogin time expired (%s)"),
											  pLogin->szTDATE);
		return;
	} // if (bDateCheck == FALSE)	

	// 사용자 ID 세팅
	m_pClientManager->SetUserID(dwClient, strUserID.GetString());

	// 사용자 UID 를 비교, 그에 맞는 로그인 피드백 데이타를 전송한다.
	CAgentExciteUserCheck* pAction = new CAgentExciteUserCheck(strUserID.GetString(),
														    m_pClientManager->GetClientIP(dwClient),
														    m_nServerGroup,
											 			    m_nServerNum,
														    0,
														    dwClient);
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

void CAgentServer::ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szExciteUserID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			nlfd.nResult    = nlfd2->nResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

            if (CCfg::GetInstance()->IsTestServer()) 
            {
                nlfd.uChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.uChaRemain = nlfd2->uChaRemain;
            }

			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			SendClient(dwClient, &nlfd);
		}
		// 이미 오프라인
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szExciteUserID);
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szExciteUserID);
			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false); // 인증중 false

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szExciteUserID) == 0))
		{

			if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			{

				bool bFind = FALSE;
				int i;
				for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
				{
					// 온라인 상태인경우
					if (m_pClientManager->GetUserNum(i) == nlfd2->nUserNum )
					{
						if( i != dwClient )
						{
							CloseClient(i);
							bFind = TRUE;
							CConsoleMessage::GetInstance()->Write(_T("ExciteMsgLoginBack Duplication Login CloseClient(ID: %s UserNum: %d ClientID: %d )"),
								nlfd2->szExciteUserID, nlfd2->nUserNum, dwClient );
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szExciteUserID);

					CConsoleMessage::GetInstance()->Write(_T("ExciteMsgLoginBack Duplication Login UserLogoutSimple(ID: %s UserNum: %d ClientID: %d )"),
						nlfd2->szExciteUserID, nlfd2->nUserNum, dwClient );
				}
			}


			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szExciteUserID);
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szExciteUserID);
			// 현재사용자는 대기
			return;
		}
	}
}


/**
* 일본Gonzo 로그인 : id / pwd 입력시
*/
void CAgentServer::JapanMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_JAPAN) return;

	if (pMsg == NULL)
		return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(pMsg->dwClient, true);
	}

	JAPAN_NET_LOGIN_DATA* pNml = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	pNml = reinterpret_cast<JAPAN_NET_LOGIN_DATA *> (pMsg->Buffer);

	if (sizeof(JAPAN_NET_LOGIN_DATA) != pNml->nmg.dwSize)
	{
		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:JAPAN_NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

	// Tea 알고리즘으로 암호화된 패킷 복호화
	m_Tea.decrypt (pNml->szUserid, JAPAN_USER_ID+1);
	m_Tea.decrypt (pNml->szPassword, JAPAN_USER_PASS+1);
	m_Tea.decrypt (pNml->szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};

	StringCchCopy (szUserid, JAPAN_USER_ID+1, pNml->szUserid);
	StringCchCopy (szPassword, JAPAN_USER_PASS+1, pNml->szPassword);
	StringCchCopy (szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt);

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{

		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:MsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient); // 접속종료

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

		CConsoleMessage::GetInstance()->Write(_T("CH(%d) is FULL"), nChannel);

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}

	// WhiteRock과의 통신 하는 부분을 Thread 처리
	CAgentJapnaWRLogin* pAction = new CAgentJapnaWRLogin(
													szUserid,
													szPassword,
													m_pClientManager->GetClientIP(dwClient),
													m_nServerGroup,
													m_nServerNum,
													dwClient
													);

	COdbcManager::GetInstance()->AddWebJob((CDbAction*) pAction);
/*
	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	CAgentJapanUserCheck* pAction = new CAgentJapanUserCheck( 
		szUserid, 
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient);
	
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
*/
}

// 일본 Gonzo Japan WhiteRock 시스템과의 통신 실패
void CAgentServer::JapanWRFailed( DWORD dwClient, int nResult )
{
	DWORD dwClientNum = 0;
	dwClientNum = dwClient;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	if( HTTP_STATUS_BAD_GATEWAY == nResult ) nlfd.nResult = EM_LOGIN_FB_SUB_INCORRECT; // ID, PW 불일치로 로그인 실패
	else if( HTTP_STATUS_ALREADY_LOGIN == nResult ) nlfd.nResult = EM_LOGIN_FB_SUB_DUP; // 중복로그인
	else nlfd.nResult = EM_LOGIN_FB_SUB_SYSTEM; // 시스템 에러로 로그인 실패

	SendClient(dwClientNum, &nlfd); // 결과전송
	m_pClientManager->SetAccountPassing (dwClientNum, false); // 인증통과중 상태 false
	return;
}

// 일본 Gonzo Japan WhiteRock 시스템과의 통신 성공
void CAgentServer::JapanWRSuccess( DWORD dwClient, CString strUserID,
									int nServerGroup, int nServerNum, CString strUUID, int nUserNum )
{
	TCHAR szUserID[JAPAN_USER_ID+1] = {0};
	int nSGroup = 0;
	int nSNum = 0;
	int nUserNumber = 0;
	DWORD dwClientNum = 0;
	TCHAR szUUID[UUID_STR_LENGTH] = {0};

	StringCchCopy( szUserID, JAPAN_USER_ID+1, strUserID);
	nSGroup = nServerGroup;
	nSNum = nServerNum;
	dwClientNum = dwClient;
	nUserNumber = nUserNum;
	StringCchCopy( szUUID, UUID_STR_LENGTH, strUUID);

	m_pClientManager->JPSetUUID( dwClientNum, szUUID );

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClientNum, szUserID);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClientNum, false); // 인증통과 실패 세팅
	
	CAgentJapanUserCheck* pAction = new CAgentJapanUserCheck( 
		szUserID, 
		m_pClientManager->GetClientIP(dwClientNum),
		nSGroup,
		nSNum,
		0,
		dwClientNum,
		nUserNumber);
	
	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

/*
* 일본 로그인 결과 : id / pwd 입력시
*/
void CAgentServer::JapanMsgLoginBack (JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // 로그인 성공
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->JapanUserID) == 0) )
		{
			nlfd.nResult    = nlfd2->nResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaRemain Error : %hu"),nlfd2->uChaRemain);	
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				CConsoleMessage::GetInstance()->Write(_T("uChaTestRemain Error : %hu"),nlfd2->uChaTestRemain);	
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.uChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			// DB에서 연산된 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (남여 성별 변경카드에 따른 추가)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// 성별 변경 카드 추가에 따른 극강부 생성 조건 추가 체크 작업

			// 극강부 남자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}
			// 극강부 여자 생성조건이 0이하일 경우..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// 극강부 남/여 생성 가능 숫자를 넘겨준다.
			// 극강부 추가가 되면 이 코드를 넣는다.
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;

			// 극강부 남/여 생성 가능 숫자를 서버가 가지고 있는다. (해킹방지)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd); // 결과전송

			// UUID를 클라이언트에 넘겨준다.
			JAPAN_NET_LOGIN_UUID netMsg;

			StringCchCopy( netMsg.szUUID, UUID_STR_LENGTH, m_pClientManager->JPGetUUID( dwClient ) ); 
			SendClient(dwClient, &netMsg );	

			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if ( m_pClientManager->IsOnline(dwClient) == false )
		{
			// 로그아웃 시킨다.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->JapanUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Already Offline"));

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->JapanUserID);			

			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->JapanUserID);

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->JapanUserID) == 0) )
		{
			if( nlfd2->nResult == EM_LOGIN_FB_SUB_DUP )
			{

				bool bFind = FALSE;
				int i;
				for ( i = NET_RESERVED_SLOT; i < m_nMaxClient; ++i )
				{
					// 온라인 상태인경우
					if ( m_pClientManager->GetUserNum(i) == nlfd2->nUserNum )
					{
						if( i != dwClient )
						{
							CloseClient(i);
							bFind = TRUE;
							CConsoleMessage::GetInstance()->Write(_T("MsgLoginBack Duplication Login CloseClient(ID: %s ClientID: %d )"),
																	nlfd2->JapanUserID, dwClient );
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->JapanUserID);

					CConsoleMessage::GetInstance()->Write(_T("MsgLoginBack Duplication Login UserLogoutSimple(ID: %s ClientID: %d )"),
															nlfd2->JapanUserID, dwClient );
				}
			}

			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->JapanUserID);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->JapanUserID);
			return;
		}
	}
}


/**
 * Excite Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::ExciteMsgPassCheck(MSG_LIST* pMsg)
{
	if (pMsg == NULL) return;

	DWORD dwClient=0;

	EXCITE_NET_PASSCHECK_DATA* pPassCheck = NULL;
    pPassCheck = reinterpret_cast<EXCITE_NET_PASSCHECK_DATA*> (pMsg->Buffer);

	if (pPassCheck == NULL) return;

	dwClient = pMsg->dwClient;

	TCHAR szExciteUserID[EXCITE_SEC_ID+1];
	TCHAR szUserPass[EXCITE_SEC_PASS+1];

	int nCheckFlag = pPassCheck->nCheckFlag;

	StringCchCopy( szExciteUserID, EXCITE_SEC_ID+1, pPassCheck->szExciteUserID );
	StringCchCopy( szUserPass, EXCITE_SEC_PASS+1, pPassCheck->szUserPass );	

	// UserID를 이용해서 사용자 패스워드를 체크한다.
	CAgentExcitePassCheck* pAction = new CAgentExcitePassCheck( m_pClientManager->GetUserID(dwClient),
															  szExciteUserID,
															  szUserPass,
															  nCheckFlag,
															  0,
															  dwClient );

	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}



/**
 * Excite Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::ExciteMsgPassCheckBack(EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
		CConsoleMessage::GetInstance()->Write( _T("ExciteMsgPassCheckBack User Already Offline(%s)"),
													nlfd2->szExciteUserID);
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("ExciteMsgPassCheckBack Login Error(%s)"),
													nlfd2->szExciteUserID);
		return;
	}
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_SECID_ALREADY )	// 아뒤 중복
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;	
	}	

	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;
	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("ExciteMsgPassCheckBack Login Fail(%s)"),
													nlfd2->szExciteUserID);
		return;	
	}
}



/**
 * Terra Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::TerraMsgPassCheck(MSG_LIST* pMsg)
{
	//	Memo :	말레이지아, 한국에서 사용된다.
	//if (m_nServiceProvider != SP_TERRA) return;

	if (pMsg == NULL) return;

	DWORD dwClient=0;

	TERRA_NET_PASSCHECK_DATA* pPassCheck = NULL;
    pPassCheck = reinterpret_cast<TERRA_NET_PASSCHECK_DATA*> (pMsg->Buffer);

	if (pPassCheck == NULL) return;

	dwClient = pMsg->dwClient;

	TCHAR szTLoginName[TERRA_TLOGIN_NAME+1];
	TCHAR szUserPass[TERRA_USERPASS+1];

	int nCheckFlag = pPassCheck->nCheckFlag;

	StringCchCopy( szTLoginName, TERRA_TLOGIN_NAME+1, pPassCheck->szTLoginName );
	StringCchCopy( szUserPass, TERRA_USERPASS+1, pPassCheck->szUserPass );	

	// TLoginName을 이용해서 사용자 패스워드를 체크한다.
	CAgentTerraPassCheck* pAction = new CAgentTerraPassCheck( szTLoginName,
															  szUserPass,
															  nCheckFlag,
															  0,
															  dwClient );

	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

/**
 * Terra Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::TerraMsgPassCheckBack(TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
		CConsoleMessage::GetInstance()->Write( _T("TerraMsgPassCheckBack User Already Offline(%s)"),
													nlfd2->szTerraTLoginName);
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("TerraMsgPassCheckBack Login Error(%s)"),
													nlfd2->szTerraTLoginName);
		return;
	}
	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("TerraMsgPassCheckBack Login Fail(%s)"),
													nlfd2->szTerraTLoginName);
		return;	
	}
}



/**
 * Daum Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::DaumMsgPassCheck(MSG_LIST* pMsg)
{
	//	Memo :	말레이지아, 한국에서 사용된다.
	if (m_nServiceProvider != SP_KOREA) return;

	if (pMsg == NULL) return;

	DAUM_NET_PASSCHECK_DATA* pPassCheck = NULL;
    pPassCheck = reinterpret_cast<DAUM_NET_PASSCHECK_DATA*> (pMsg->Buffer);

	if (pPassCheck == NULL) return;

	// TLoginName을 이용해서 사용자 패스워드를 체크한다.
	CAgentDaumPassCheck* pAction = new CAgentDaumPassCheck(	pPassCheck->szDaumGID,
															pPassCheck->szUserPass,
															pPassCheck->nCheckFlag,
															0,
															pMsg->dwClient );

	COdbcManager::GetInstance()->AddUserJob((CDbAction*) pAction);
}

/**
 * Daum Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD dwClient = static_cast<DWORD> (nlfd2->nClient);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		COdbcManager::GetInstance()->UserLogoutSimple(m_pClientManager->GetUserID(dwClient));
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack User Already Offline(%s)"), nlfd2->szDaumGID );
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack Login Error(%s)"),nlfd2->szDaumGID );
		return;
	}
	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack Login Fail(%s)"), nlfd2->szDaumGID);
		return;	
	}
}

