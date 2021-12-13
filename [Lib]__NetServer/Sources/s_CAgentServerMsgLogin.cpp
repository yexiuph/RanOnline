#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* �α��� �޽���
* Thailand
* id / pwd �Է½�
*/
void CAgentServer::ThaiMsgLogin (MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_THAILAND || pMsg == NULL) return;

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing (pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing (pMsg->dwClient, true);
	}

	THAI_NET_LOGIN_DATA* tnld = NULL;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient   = 0;

	dwClient = pMsg->dwClient;

	tnld = reinterpret_cast<THAI_NET_LOGIN_DATA *> (pMsg->Buffer);

	/// Tea �˰������� ��ü
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
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = tnld->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL; // ä���� Full �̶�� �޽����� ������.
		SendClient (dwClient, &nlfd); // �������
//		CloseClient (dwClient); // ��������

		CConsoleMessage::GetInstance()->WriteConsole(
			_T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			nChannel );
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull (nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient (dwClient, &nlfd); // �������
//		CloseClient (dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel (dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser (nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel (dwClient, nChannel);
		}
		else // ����������
		{
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID (dwClient, tnld->szUserid);		
	
	// ������� ���� ����
	m_pClientManager->SetAccountPass (dwClient, false); 

	// DB �� ������û
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

    // �α��� ����
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
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
			m_pClientManager->SetUserNum      ( dwClient, nlfd2->nUserNum );       // ������ȣ
			m_pClientManager->SetLoginTime    ( dwClient );                        // �α��� �ð�
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // ä�� ��� �Ⱓ
			m_pClientManager->ThaiSetFlag     ( dwClient, nlfd2->wThaiFlag );      // Thailand ����� �÷���
			m_pClientManager->SetThaiClass	  ( dwClient, nlfd2->nThaiCC_Class );  // Thailand ����� Class Type

			
			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
			
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}
			
			// �ذ��� �߰�
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			// 18 �� �����϶��� ���� ������ �÷��� �ð�(��)�� �����;� �Ѵ�.
			if ( nlfd2->wThaiFlag == THAI_UNDER18 )
			{
				// ���� �÷����� �ð� ��������
				WORD wTodayGameTime = COdbcManager::GetInstance()->ThaiGetTodayPlayTime( nlfd2->nUserNum );
				// ���� �����÷��� �ð�(��) ����
				m_pClientManager->ThaiSetTodayPlayTime( dwClient, wTodayGameTime );
			}

			m_pClientManager->SetAccountPass( dwClient, true ); // �����������

			SendClient( dwClient, &nlfd ); // �������
			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// �α׾ƿ� ��Ų��.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
				_T("MsgLoginBack User Already Offline") );
	
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
                _T("MsgLoginBack User IP/ID Wrong(%s)"),
                nlfd2->szUserid );

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass (dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline (dwClient) == true) &&
			(strcmp (m_pClientManager->GetClientIP (dwClient), nlfd2->szIp) == 0) &&
			(strcmp (m_pClientManager->GetUserID (dwClient), nlfd2->szUserid) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient (dwClient, &nlfd);
			return;
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline (dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
                _T("MsgLoginBack User Login Fail And Offline(%s)"),
				nlfd2->szUserid );
		
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			CConsoleMessage::GetInstance()->Write (
		                                          _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}
}

//! China �α��� �޽���
void CAgentServer::ChinaMsgLogin( MSG_LIST* pMsg )
{
	

	// �߱� Ŭ���̾�Ʈ�� ���� �޽��� ���� Ȯ���Ѵ�.
	if (m_nServiceProvider != SP_CHINA || pMsg == NULL) return;	

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing( pMsg->dwClient ) == true)
	{
		return;
	}
	else // ���������� ����
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

	// ��ȣȭ
	m_Tea.decrypt( pNml->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( pNml->szPassword, USR_PASS_LENGTH );
	m_Tea.decrypt( pNml->szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};

	StringCchCopy( szUserid, USR_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_PASS_LENGTH, pNml->szPassword );
	StringCchCopy( szRandomPassword, USR_RAND_PASS_LENGTH+1, pNml->szRandomPassword );
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������

		CConsoleMessage::GetInstance()->Write(
			
			_T("ERROR:ChinaMsgLogin Invalid Channel Number (Channel:%d)"),
			nChannel );
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull( nChannel ) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������
		CConsoleMessage::GetInstance()->Write(
			
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient, nChannel );
		}
		else // ����������
		{
			
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB �� ������û
	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ���� ����
	
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

    // �α��� ����
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
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
			m_pClientManager->SetUserNum      ( dwClient, nlfd2->nUserNum );       // ������ȣ
			m_pClientManager->SetLoginTime    ( dwClient );                        // �α��� �ð�
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // ä�� ��� �Ⱓ

			m_pClientManager->SetChinaGameTime( dwClient, nlfd2->nChinaGameTime ); // �߱� ���ӽð� ����
			m_pClientManager->SetChinaOfflineTime( dwClient, nlfd2->nChinaOfflineTime ); // �߱� �������� �ð� ����
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // �߱� LastLoginDate ����
			m_pClientManager->SetChinaUserAge( dwClient, nlfd2->nChinaUserAge ); // �߱� UserAge ����

			m_pClientManager->SetAccountPass( dwClient, true ); // �����������

			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
			
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.

			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;			

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient( dwClient, &nlfd ); // �������

			// Apex ����
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

//			char* szIp = m_pClientManager->GetClientIP( dwClient );
//			int nIp  = inet_addr(szIp);
			
//			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			
			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			// �α׾ƿ� ��Ų��.
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szUserid );			
			CConsoleMessage::GetInstance()->Write(
				
		        _T("ChinaMsgLoginBack User Already Offline"));
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szUserid );			

			CConsoleMessage::GetInstance()->Write(
				
		        _T("MsgLoginBack User IP/ID Wrong(%s)"),
				nlfd2->szUserid );

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����
		m_pClientManager->SetAccountPassing( dwClient, false );

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ), nlfd2->szUserid ) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient( dwClient, &nlfd );
			return;
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			CConsoleMessage::GetInstance()->Write(				
		        _T("MsgLoginBack User Login Fail And Offline(%s)"),
				nlfd2->szUserid );

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			CConsoleMessage::GetInstance()->Write(
				
		        _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
				nlfd2->szUserid );
			return;
		}
	}
}

/**
* �Ϲ� �α��� : id / pwd �Է½�
*/
void CAgentServer::MsgLogIn(MSG_LIST* pMsg)
{		
	if (pMsg == NULL)
		return;

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	/// ��ȣȭ �˰��� ��ü
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:MsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

//		CloseClient(dwClient); // ��������
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
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);

			/*CConsoleMessage::GetInstance()->Write(
				_T("### INCREASE_CHANNEL_USER_CLIENT_ID %d"), dwClient );*/
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}
/*
	// WhiteRock���� ��� �ϴ� �κ��� Thread ó��
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

	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:GsMsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:GsMsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

//		CloseClient(dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);

			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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

    // �α��� ����
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
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
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
			
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd); // �������
			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			// �α׾ƿ� ��Ų��.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Already Offline"));			

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szUserid);			

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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
					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->szUserid);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			CConsoleMessage::GetInstance()->Write(
		                                          _T("GsMsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}

}

/*
* �Ϲ� �α��� ��� : id / pwd �Է½�
*/
void CAgentServer::MsgLogInBack (NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	//int		nTestNum   = 100;

    // �α��� ����
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
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
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			
/*
			// ���� �׽�Ʈ�� ���ؼ� �����̽þ� User Class Type�� �߰��Ѵ�.
			m_pClientManager->SetMyClass	  ( dwClient, nlfd2->nMyCC_Class );  // �����̽þ� ����� Class Type
*/
//			m_pClientManager->SetThaiClass	  ( dwClient, nlfd2->nThaiCC_Class ); // �׽�Ʈ��

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // �߱�, ��Ʈ�� LastLoginDate ����
#if defined( VN_PARAM ) || defined(_RELEASED) //vietnamtest%%%
			m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // ��Ʈ�� ���ӽð� ����
#endif

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
			
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
// �ذ��� �߰�
#if	defined(RZ_PARAM) || defined(_RELEASED) || defined(KRT_PARAM) || defined(KR_PARAM) || defined ( TW_PARAM ) || defined(HK_PARAM) 
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif
			nlfd.nCheckFlag = nlfd2->nCheckFlag; // �ѱ� CheckFlag Test�� ���ؼ� �߰�

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
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

#if defined ( HK_PARAM )  || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex ����( ȫ�� )
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

			char* szIp = m_pClientManager->GetClientIP( dwClient );
			int nIp  = inet_addr(szIp);

			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
#endif

			SendClient(dwClient, &nlfd); // �������
			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			// �α׾ƿ� ��Ų��.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Already Offline"));			

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

				return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->szUserid);
			// COdbcManager::GetInstance()->UserLogoutSimple2(nlfd2->nUserNum);

			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szUserid);			

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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
					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false /*|| nTestNum == 100*/ )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->szUserid);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)"),
												  nlfd2->szUserid);
			return;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ���� : �α���
/*
void CAgentServer::DaumMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_KOREA) return;

	if (pMsg == NULL) return;

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:DaumMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}

	try
	{
		if (m_DaumAuth.SetSource(pLogin->szParameter) == false)
		{
			// ���������� ���� �������̱� ������ �߸��� �������.
			nlfd.nResult = EM_LOGIN_FB_SUB_UNCON;
			SendClient(dwClient, &nlfd);
			CConsoleMessage::GetInstance()->Write(
				                                  _T("DaumMsgLogin SetSource Failed (%s)"),
												  pLogin->szParameter);
			return;
		}
		else
		{
			// "GID": GameID   (����ڰ� ���� �Է��ߴ� ID) - e.g. test1     16 �ڸ�
			// "UID": UniqueID (�ý����� �ο��� ����� ID) - e.g. 15ftgr     6 �ڸ�
			// "SSNHEAD": ������� - e.g. 731201                             6 �ڸ�
			// "SEX": ����(M:����, F:����)                                   1 �ڸ�
			// "NAMECHECK": �Ǹ�Ȯ�� ����(1=����, 0=������)                  1 �ڸ�
			// "IP": ������ IP
			// "STATUS": ����� ����(A: ��ȸ��, W: ��ȸ��(��ȹ����), D: Ż���ûȸ��, B: �ҷ�����)
			
			if (m_DaumAuth.IsTimeExpired() == TRUE) 
			{
				// �����ð��� �ʰ��� ������̱� ������, ���� �������� �϶�� �޽����� ������.			
				nlfd.nResult = EM_LOGIN_FB_SUB_EXPIRED;
				SendClient(dwClient, &nlfd);				
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:Login time expired"));
				return;
			}
			// GID�� ���´�.
			if (m_DaumAuth.GetData("GID", szDaumGID, DAUM_MAX_GID_LENGTH) == FALSE)
			{
				// GID�� ������ ����ó��.			
				nlfd.nResult = EM_LOGIN_FB_SUB_GID_ERR;
				SendClient(dwClient, &nlfd);
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:GID is not exist"));
				return;
			}		
			// UID�� ���´�.
			if (m_DaumAuth.GetData("UID", szDaumUID, DAUM_MAX_UID_LENGTH) == FALSE)
			{
				// UID�� ������ ����ó��.			
				nlfd.nResult = EM_LOGIN_FB_SUB_UID_ERR;
				SendClient(dwClient, &nlfd);
				CConsoleMessage::GetInstance()->Write(
					                                  _T("Daum:UID is not exist"));
				return;
			}

			if (m_DaumAuth.GetData("SSNHEAD", szDaumSSNHEAD, DAUM_MAX_SSNHEAD_LENGTH) == FALSE)
			{
				// �ֹι�ȣ ����ó��.			
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

			// ��������
			if (m_bAdult)
			{
				if (SERVER_UTIL::CheckAdult(strDaumSSNHEAD,18) == false)
				{
					// �������� ����ó��.			
					nlfd.nResult = EM_LOGIN_FB_SUB_ADULT;
					SendClient(dwClient, &nlfd);
					CConsoleMessage::GetInstance()->Write(
						                                  _T("Daum:Adult authorize failed")); // �������� ����
					return;
				}
			}

			CConsoleMessage::GetInstance()->WriteConsole(
				_T("(UID:%s)(GID:%s)"),
				strDaumUID.GetString(), 
				strDaumGID.GetString() );

			// ����� UID ����
			m_pClientManager->SetUserID(dwClient, strDaumUID.GetString());	

			// ����� UID �� ��, �׿� �´� �α��� �ǵ�� ����Ÿ�� �����Ѵ�.
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

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			
			_T("WARNING:User Login Attempt is over 5 times %s"),
			m_pClientManager->GetClientIP(pMsg->dwClient) );
		CloseClient( pMsg->dwClient ); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:DaumMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}

	// UUID Check
	// UUID copy
	TCHAR szUUID[UUID_STR_LENGTH] = {0};
	StringCchCopy( szUUID, UUID_STR_LENGTH, pLogin->szUUID);

	// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� UUID �� �����Ѵ�.
	// ��ŷ�� ���ؼ� �ٸ� UUID �ΰ�ó�� ���� ���ɼ��� �ִ�.
	m_pClientManager->KorSetUUID( dwClient, szUUID );

	// UUID�� �̿��ؼ� ����� ������ üũ�Ѵ�.
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
// ���� : �α��� ���
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	m_pClientManager->SetUserID(dwClient, nlfd2->szDaumGID);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->KorGetUUID(dwClient), nlfd2->szUUID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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

			
			// Daum �н����带 �ޱ����� Check Flag ��
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
// �ذ��� �߰�
#if	defined(RZ_PARAM) || defined(_RELEASED) || defined(KRT_PARAM) || defined(KR_PARAM) || defined ( TW_PARAM ) || defined(HK_PARAM) 
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd);
		}
		// �̹� ��������
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumGID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szDaumGID);
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumGID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szDaumGID);
			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false); // ������ false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
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
			//		// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szDaumGID);
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szDaumGID);
			// �������ڴ� ���
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// ���� : �α��� ���
/*
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szDaumUID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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

			// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
#if	defined(_RELEASED) || defined(KRT_PARAM)
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif
			// Daum �н����带 �ޱ����� Check Flag ��
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd);
		}
		// �̹� ��������
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumUID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szDaumUID);
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szDaumUID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szDaumUID);
			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false); // ������ false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szDaumUID) == 0))
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient(dwClient, &nlfd);
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szDaumUID);
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("DaumMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szDaumUID);
			// �������ڴ� ���
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

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			
			_T("WARNING:User Login Attempt is over 5 times %s"),
			m_pClientManager->GetClientIP(pMsg->dwClient) );
		CloseClient( pMsg->dwClient ); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing( pMsg->dwClient ) == true)
	{
		return;
	}
	else // ���������� ����
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

	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			
			_T("ERROR:GspMsgLogin Invalid Channel Number (Channel:%d)"),
			nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull( nChannel ) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������
		CConsoleMessage::GetInstance()->Write(
			
			_T("CH(%d) is FULL"),
			nChannel );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient, nChannel );
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}	

	// UUID Check
	// UUID copy	
	TCHAR szUUID[UUID_STR_LENGTH] = {0};
	StringCchCopy( szUUID, UUID_STR_LENGTH, pLogin->szUUID);

	// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� UUID �� �����Ѵ�.
	// ��ŷ�� ���ؼ� �ٸ� UUID �ΰ�ó�� ���� ���ɼ��� �ִ�.
	m_pClientManager->GspSetUUID( dwClient, szUUID );

	// UUID�� �̿��ؼ� ����� ������ üũ�Ѵ�.
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

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	// GspUserID
	m_pClientManager->GspSetUserID( dwClient, nlfd2->szGspUserID );

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, Decoded TID �� �����Ҷ�...
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			 (strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0) )
		{
			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			SendClient(dwClient, &nlfd);
		}
		// �̹� ��������
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����

			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szGspUserID );
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Already Offline(%s)"),
				nlfd2->szGspUserID );
			return;
		}
		// �α��μ���, �¶��������� ip, DecodedTID Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple( nlfd2->szGspUserID );
			CConsoleMessage::GetInstance()->Write(
				
				_T("GspMsgLoginBack User IP/UUID Wrong(%s:%s)"),
				nlfd2->szGspUserID,
				nlfd2->szUUID );

			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����
		m_pClientManager->SetAccountPassing( dwClient, false );

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (strcmp( m_pClientManager->GetClientIP( dwClient ), nlfd2->szIp ) == 0) &&
			 (strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID ) == 0) )
		{
			nlfd.nResult = nlfd2->nResult;
			SendClient( dwClient, &nlfd );
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Offline AND Login Fail(%s)"),
				nlfd2->szGspUserID );
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			CConsoleMessage::GetInstance()->Write(
				
		        _T("GspMsgLoginBack User Online Wrong(%s)"),
				nlfd2->szGspUserID );
			// �������ڴ� ���
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

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(pMsg->dwClient) > 5)
	{
		CConsoleMessage::GetInstance()->Write(
			                                  _T("WARNING:User Login Attempt is over 5 times %s"),
											  m_pClientManager->GetClientIP(pMsg->dwClient));
		CloseClient(pMsg->dwClient); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:TerraMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
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
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}

	char szDecodedTID[TERRA_TID_DECODE+1] = {0}; // Decoded TID

	// Encoding �� ���ڿ��� Decoding �Ѵ�.
	if (TerraGetDecodedTID(pLogin->szTID, szDecodedTID) == NET_ERROR) 
	{ 		
		// Decoding ���н� �ڵ�
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
                                              _T("ERROR:TerraGetDecodedTID %s"),
											  pLogin->szTID);
		return;
	}
	else
	{
		// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� TID �� �����Ѵ�.
		m_pClientManager->TerraSetDecodedTID(dwClient, szDecodedTID);

		// Decoding �� TID �� �̿��ؼ� ����� ������ üũ�Ѵ�.
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
* \param nlfd2 �α��� Feedback data
*/
void CAgentServer::TerraMsgLoginBack(TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	// TLoginName 
	m_pClientManager->SetUserID(dwClient, nlfd2->szTerraTLoginName);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, Decoded TID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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

			// ���������� �н����带 �ޱ����� Check Flag ��
			nlfd.nCheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ
#if	defined( MYE_PARAM ) || defined( MY_PARAM ) 
			m_pClientManager->SetMyClass	  ( dwClient, nlfd2->nMyCC_Class );  // �����̽þ� ����� Class Type
#endif
			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�
			
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// �ذ��� �߰�
#if	defined( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM )
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;
#else
			nlfd.nExtremeM = nlfd2->nExtremeM = 0;
			nlfd.nExtremeW = nlfd2->nExtremeW = 0;
#endif

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

#if defined( VN_PARAM ) //vietnamTEst%%%
			m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // ��Ʈ�� ���ӽð� ����
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // ��Ʈ�� LastLoginDate ����
#endif			
			SendClient(dwClient, &nlfd);
		}
		// �̹� ��������
		else if ( m_pClientManager->IsOnline(dwClient) == false )
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szTerraTLoginName);

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �α��� ���� ����
	
			return;
		}
		// �α��μ���, �¶��������� ip, DecodedTID Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User IP/DecodedTID Wrong(%s)"),
												  nlfd2->szTerraTLoginName);

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			CloseClient( dwClient );

			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
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
					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Offline AND Login Fail(%s)"),
												  nlfd2->szTerraTLoginName);

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �α��� ���� ����

		
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("TerraMsgLoginBack User Online Wrong(%s)"),
												  nlfd2->szTerraTLoginName);
			// �������ڴ� ���
			return;
		}
	}
}

/**
* Encoding �� TID ���� Decoding �� TID �� ��´�.
* \param szEncodedTID Encoding �� TID
* \param szDecodedTID Decoding �� TID
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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:ExciteMsgLogin Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
		CConsoleMessage::GetInstance()->Write(
											  _T("CH(%d) is FULL"),
			                                  nChannel);
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
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
		// ���������� ���� �������̱� ������ �߸��� �������.
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
		// �����ð��� �ʰ��� ������̱� ������, ���� �������� �϶�� �޽����� ������.			
		nlfd.nResult = EM_LOGIN_FB_SUB_EXPIRED;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write(
			                                  _T("ExciteMsgLogin time expired (%s)"),
											  pLogin->szTDATE);
		return;
	} // if (bDateCheck == FALSE)	

	// ����� ID ����
	m_pClientManager->SetUserID(dwClient, strUserID.GetString());

	// ����� UID �� ��, �׿� �´� �α��� �ǵ�� ����Ÿ�� �����Ѵ�.
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

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.	
	nlfd.nmg.nType       = NET_MSG_LOGIN_FB;
	nlfd.nGameProgramVer = m_nVersion;
	nlfd.nPatchProgramVer= m_nPatchVersion;

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(strcmp(m_pClientManager->GetClientIP(dwClient), nlfd2->szIp) == 0) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szExciteUserID) == 0))
		{
			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			SendClient(dwClient, &nlfd);
		}
		// �̹� ��������
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Already Offline(%s)"),
												  nlfd2->szExciteUserID);
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->szExciteUserID);
			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false); // ������ false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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
					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Offline AND ID/PWD Wrong(%s)"),
												  nlfd2->szExciteUserID);
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("ExciteMsgLoginBack User Online ID/PWD Wrong(%s)"),
												  nlfd2->szExciteUserID);
			// �������ڴ� ���
			return;
		}
	}
}


/**
* �Ϻ�Gonzo �α��� : id / pwd �Է½�
*/
void CAgentServer::JapanMsgLogin(MSG_LIST* pMsg)
{
	if (m_nServiceProvider != SP_JAPAN) return;

	if (pMsg == NULL)
		return;

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(pMsg->dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	// Tea �˰������� ��ȣȭ�� ��Ŷ ��ȣȭ
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		CConsoleMessage::GetInstance()->Write(
			_T("ERROR:MsgLogIn Not Correct Encrypt UserID : %s"), pNml->szUserid );

		return;
	}
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient); // ��������

		CConsoleMessage::GetInstance()->Write(
			                                  _T("ERROR:MsgLogIn Invalid Channel Number (Channel:%d)"),
			                                  nChannel);
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nmg.nType = NET_MSG_LOGIN_FB;
		nlfd.nResult   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

		CConsoleMessage::GetInstance()->Write(_T("CH(%d) is FULL"), nChannel);

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}

	// WhiteRock���� ��� �ϴ� �κ��� Thread ó��
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
	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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

// �Ϻ� Gonzo Japan WhiteRock �ý��۰��� ��� ����
void CAgentServer::JapanWRFailed( DWORD dwClient, int nResult )
{
	DWORD dwClientNum = 0;
	dwClientNum = dwClient;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	if( HTTP_STATUS_BAD_GATEWAY == nResult ) nlfd.nResult = EM_LOGIN_FB_SUB_INCORRECT; // ID, PW ����ġ�� �α��� ����
	else if( HTTP_STATUS_ALREADY_LOGIN == nResult ) nlfd.nResult = EM_LOGIN_FB_SUB_DUP; // �ߺ��α���
	else nlfd.nResult = EM_LOGIN_FB_SUB_SYSTEM; // �ý��� ������ �α��� ����

	SendClient(dwClientNum, &nlfd); // �������
	m_pClientManager->SetAccountPassing (dwClientNum, false); // ��������� ���� false
	return;
}

// �Ϻ� Gonzo Japan WhiteRock �ý��۰��� ��� ����
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

	// ����� id ����
	m_pClientManager->SetUserID(dwClientNum, szUserID);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClientNum, false); // ������� ���� ����
	
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
* �Ϻ� �α��� ��� : id / pwd �Է½�
*/
void CAgentServer::JapanMsgLoginBack (JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nmg.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // �α��� ����
	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
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
			m_pClientManager->SetUserNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			// DB���� ����� �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (���� ���� ����ī�忡 ���� �߰�)
			m_pClientManager->SetExtremeCreateDBM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateDBW( dwClient, nlfd2->nExtremeW );

			// ���� ���� ī�� �߰��� ���� �ذ��� ���� ���� �߰� üũ �۾�

			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeM < 0 )
			{
				nlfd2->nExtremeW += nlfd2->nExtremeM;
				nlfd2->nExtremeM = 0;
			}
			// �ذ��� ���� ���������� 0������ ���..
			if( nlfd2->nExtremeW < 0 )
			{
				nlfd2->nExtremeM += nlfd2->nExtremeW;
				nlfd2->nExtremeW = 0;
			}

			// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
			// �ذ��� �߰��� �Ǹ� �� �ڵ带 �ִ´�.
			nlfd.nExtremeM = nlfd2->nExtremeM;
			nlfd.nExtremeW = nlfd2->nExtremeW;

			// �ذ��� ��/�� ���� ���� ���ڸ� ������ ������ �ִ´�. (��ŷ����)
			m_pClientManager->SetExtremeCreateM( dwClient, nlfd2->nExtremeM );
			m_pClientManager->SetExtremeCreateW( dwClient, nlfd2->nExtremeW );

			SendClient(dwClient, &nlfd); // �������

			// UUID�� Ŭ���̾�Ʈ�� �Ѱ��ش�.
			JAPAN_NET_LOGIN_UUID netMsg;

			StringCchCopy( netMsg.szUUID, UUID_STR_LENGTH, m_pClientManager->JPGetUUID( dwClient ) ); 
			SendClient(dwClient, &netMsg );	

			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if ( m_pClientManager->IsOnline(dwClient) == false )
		{
			// �α׾ƿ� ��Ų��.
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->JapanUserID);
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Already Offline"));

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			COdbcManager::GetInstance()->UserLogoutSimple (nlfd2->JapanUserID);			

			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User IP/ID Wrong(%s)"),
												  nlfd2->JapanUserID);

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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
					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			CConsoleMessage::GetInstance()->Write(
		                                          _T("MsgLoginBack User Login Fail And Offline(%s)"),
												  nlfd2->JapanUserID);

			nlfd.nmg.nType = NET_MSG_LOGIN_FB;
			nlfd.nResult   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

	// UserID�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szExciteUserID);
		CConsoleMessage::GetInstance()->Write( _T("ExciteMsgPassCheckBack User Already Offline(%s)"),
													nlfd2->szExciteUserID);
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("ExciteMsgPassCheckBack Login Error(%s)"),
													nlfd2->szExciteUserID);
		return;
	}
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_SECID_ALREADY )	// �Ƶ� �ߺ�
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;	
	}	

	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;
	}
	else // �α��� �����϶�
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
	//	Memo :	����������, �ѱ����� ���ȴ�.
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

	// TLoginName�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		COdbcManager::GetInstance()->UserLogoutSimple(nlfd2->szTerraTLoginName);
		CConsoleMessage::GetInstance()->Write( _T("TerraMsgPassCheckBack User Already Offline(%s)"),
													nlfd2->szTerraTLoginName);
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("TerraMsgPassCheckBack Login Error(%s)"),
													nlfd2->szTerraTLoginName);
		return;
	}
	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // �α��� �����϶�
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
	//	Memo :	����������, �ѱ����� ���ȴ�.
	if (m_nServiceProvider != SP_KOREA) return;

	if (pMsg == NULL) return;

	DAUM_NET_PASSCHECK_DATA* pPassCheck = NULL;
    pPassCheck = reinterpret_cast<DAUM_NET_PASSCHECK_DATA*> (pMsg->Buffer);

	if (pPassCheck == NULL) return;

	// TLoginName�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		COdbcManager::GetInstance()->UserLogoutSimple(m_pClientManager->GetUserID(dwClient));
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack User Already Offline(%s)"), nlfd2->szDaumGID );
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack Login Error(%s)"),nlfd2->szDaumGID );
		return;
	}
	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // �α��� �����϶�
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		CConsoleMessage::GetInstance()->Write( _T("DaumMsgPassCheckBack Login Fail(%s)"), nlfd2->szDaumGID);
		return;	
	}
}

