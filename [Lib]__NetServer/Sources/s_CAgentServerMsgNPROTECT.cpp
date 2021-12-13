#include "pch.h"
#include "s_CAgentServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAgentServer::WriteNprotectLog( DWORD dwClient, DWORD dwResult )
{
	GG_AUTH_DATA Query = m_pClientManager->nProtectGetAuthQuery( dwClient );
	GG_AUTH_DATA Answer = m_pClientManager->nProtectGetAuthAnswer( dwClient );
    
	TCHAR szMsg[MAX_PATH] = {0};
	StringCchPrintf(
		szMsg,
		MAX_PATH,
		_T("ID(%s) Result(%d) Query(%X %X %X %X) Answer(%X %X %X %X)"),
		m_pClientManager->GetUserID( dwClient ),
		dwResult,
		Query.dwIndex,
		Query.dwValue1,
		Query.dwValue2,
		Query.dwValue3,
		Answer.dwIndex,
		Answer.dwValue1,
		Answer.dwValue2,
		Answer.dwValue3 );

	// ���Ϸ� �α׸� �����.
	CConsoleMessage::GetInstance()->WriteNPROTECT( szMsg );
	
	// 2006-08-16 �븸��û
	// DB ���� �α׸� �����.	
	CLogHackProgram *pDbAction = new CLogHackProgram(
										m_pClientManager->GetUserNum( dwClient ),
										m_pClientManager->GetChaNum( dwClient ),
										NPROTECT_HACK_NUM,
										szMsg );

	COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pDbAction );
}

//! nProtect ������ �޽��� ����
void CAgentServer::MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga )
{
	// �¶��� �����̰� ó�� üũ�ϴ� ���°� �ƴ϶��...	
	if (m_pClientManager->IsOnline( dwClient ) && 
		m_pClientManager->nProtectIsFirstCheck( dwClient ) == false)

	if (m_pClientManager->IsOnline( dwClient ) )
	{
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth( dwClient );
		// ���Ӱ��� ���� ���� �������� �غ�
		if (m_pClientManager->nProtectSetAuthQuery( dwClient ) == true)
		{
			// ���Ӱ��� ���� ���� ����
			nga.ggad = m_pClientManager->nProtectGetAuthQuery( dwClient );
			SendClient( dwClient, &nga );
		}
	}
}

// nProtect ���� ������ ����Ѵٸ� ������ �ٷ� Ŭ���̾�Ʈ���� ������ �޽����� ������.
void CAgentServer::MsgSndGameGuardFirstCheck( DWORD dwClient )
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{
		NET_GAMEGUARD_AUTH_1 nga;
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth(dwClient);
		// ���Ӱ��� ���� ���� �������� �غ�
		if (m_pClientManager->nProtectSetAuthQuery(dwClient) == true)
		{
			// nProtect ���� �������·� �����Ѵ�.
			m_pClientManager->nProtectFirstCheck(dwClient, true);
			// ���Ӱ��� ���� ���� ����
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
			SendClient(dwClient, &nga);
		}
	}
}

//! nProtect Ŭ���̾�Ʈ ���� Second
void CAgentServer::MsgSndGameGuardSecondCheck( DWORD dwClient )
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{
		NET_GAMEGUARD_AUTH_2 nga;
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth(dwClient);
		// ���Ӱ��� ���� ���� �������� �غ�
		if (m_pClientManager->nProtectSetAuthQuery(dwClient) == true)
		{
			// ���Ӱ��� ���� ���� ����
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
			SendClient(dwClient, &nga);
		}
	}
}

//! GameGuard ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswer( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAMEGUARD_ANSWER* nga = reinterpret_cast<NET_GAMEGUARD_ANSWER*> (pMsg->Buffer);
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	TCHAR* szEncKey = m_pClientManager->GetEncKey( dwClient );
	if ( _tcslen( szEncKey) > 0  )
	{				
		TCHAR szEncryptKey[ENCRYPT_KEY+1];
		memcpy( szEncryptKey, nga->szEnCrypt, sizeof(TCHAR) * ENCRYPT_KEY+1 );

		m_Tea.decrypt (szEncryptKey, ENCRYPT_KEY+1);
		if ( _tcscmp( szEncryptKey, szEncKey ) )
		{
			CConsoleMessage::GetInstance()->Write( _T("Error HeartBit Error %s %s , CloseClient( %d )"), szEncryptKey, szEncKey, pMsg->dwClient );					
			return;
		}
	}

	m_pClientManager->SetEncKey( dwClient, nga->szEnCrypt );
	m_pClientManager->SetHeartBeat( pMsg->dwClient );
}

//! nProtect ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswerFirst( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAMEGUARD_ANSWER_1* nga = reinterpret_cast<NET_GAMEGUARD_ANSWER_1*> (pMsg->Buffer);
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer( dwClient );
	if ( dwResult != 0)
	{
		// nProtect ���� ����
		// ������ �����Ų��.
		WriteNprotectLog( dwClient, dwResult );
		CloseClient( dwClient );
	}
	else
	{
		// �ι�° ������ �õ��Ѵ�.
		MsgSndGameGuardSecondCheck( dwClient );
	}
}

//! nProtect ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswerSecond( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAMEGUARD_ANSWER_2* nga = reinterpret_cast<NET_GAMEGUARD_ANSWER_2*> (pMsg->Buffer);
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer( dwClient );
	if (dwResult != 0)
	{
		// nProtect ���� ����
		// ������ �����Ų��.
		WriteNprotectLog( dwClient, dwResult );
		CloseClient( dwClient );
	}
	else
	{		
		m_pClientManager->nProtectFirstCheck( dwClient, false ); // ó������ ���� ��
		m_pClientManager->nProtectSetFirstAuthPass( dwClient, true ); // ������� ����
	}
}