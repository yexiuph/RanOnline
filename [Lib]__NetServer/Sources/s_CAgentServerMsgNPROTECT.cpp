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

	// 파일로 로그를 남긴다.
	CConsoleMessage::GetInstance()->WriteNPROTECT( szMsg );
	
	// 2006-08-16 대만요청
	// DB 에도 로그를 남긴다.	
	CLogHackProgram *pDbAction = new CLogHackProgram(
										m_pClientManager->GetUserNum( dwClient ),
										m_pClientManager->GetChaNum( dwClient ),
										NPROTECT_HACK_NUM,
										szMsg );

	COdbcManager::GetInstance()->AddLogJob( (CDbAction*) pDbAction );
}

//! nProtect 인증용 메시지 전송
void CAgentServer::MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga )
{
	// 온라인 상태이고 처음 체크하는 상태가 아니라면...	
	if (m_pClientManager->IsOnline( dwClient ) && 
		m_pClientManager->nProtectIsFirstCheck( dwClient ) == false)

	if (m_pClientManager->IsOnline( dwClient ) )
	{
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth( dwClient );
		// 게임가드 인증 쿼리 가져오기 준비
		if (m_pClientManager->nProtectSetAuthQuery( dwClient ) == true)
		{
			// 게임가드 인증 쿼리 전송
			nga.ggad = m_pClientManager->nProtectGetAuthQuery( dwClient );
			SendClient( dwClient, &nga );
		}
	}
}

// nProtect 서버 인증을 사용한다면 접속후 바로 클라이언트에게 인증용 메시지를 보낸다.
void CAgentServer::MsgSndGameGuardFirstCheck( DWORD dwClient )
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{
		NET_GAMEGUARD_AUTH_1 nga;
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth(dwClient);
		// 게임가드 인증 쿼리 가져오기 준비
		if (m_pClientManager->nProtectSetAuthQuery(dwClient) == true)
		{
			// nProtect 최초 인증상태로 설정한다.
			m_pClientManager->nProtectFirstCheck(dwClient, true);
			// 게임가드 인증 쿼리 전송
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
			SendClient(dwClient, &nga);
		}
	}
}

//! nProtect 클라이언트 인증 Second
void CAgentServer::MsgSndGameGuardSecondCheck( DWORD dwClient )
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{
		NET_GAMEGUARD_AUTH_2 nga;
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth(dwClient);
		// 게임가드 인증 쿼리 가져오기 준비
		if (m_pClientManager->nProtectSetAuthQuery(dwClient) == true)
		{
			// 게임가드 인증 쿼리 전송
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
			SendClient(dwClient, &nga);
		}
	}
}

//! GameGuard 인증용 메세지 결과
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

//! nProtect 인증용 메세지 결과
void CAgentServer::MsgGameGuardAnswerFirst( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAMEGUARD_ANSWER_1* nga = reinterpret_cast<NET_GAMEGUARD_ANSWER_1*> (pMsg->Buffer);
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer( dwClient );
	if ( dwResult != 0)
	{
		// nProtect 인증 실패
		// 연결을 종료시킨다.
		WriteNprotectLog( dwClient, dwResult );
		CloseClient( dwClient );
	}
	else
	{
		// 두번째 인증을 시도한다.
		MsgSndGameGuardSecondCheck( dwClient );
	}
}

//! nProtect 인증용 메세지 결과
void CAgentServer::MsgGameGuardAnswerSecond( MSG_LIST* pMsg )
{
	if (pMsg == NULL) return;

	DWORD dwClient = pMsg->dwClient;
	NET_GAMEGUARD_ANSWER_2* nga = reinterpret_cast<NET_GAMEGUARD_ANSWER_2*> (pMsg->Buffer);
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer( dwClient );
	if (dwResult != 0)
	{
		// nProtect 인증 실패
		// 연결을 종료시킨다.
		WriteNprotectLog( dwClient, dwResult );
		CloseClient( dwClient );
	}
	else
	{		
		m_pClientManager->nProtectFirstCheck( dwClient, false ); // 처음인증 상태 끝
		m_pClientManager->nProtectSetFirstAuthPass( dwClient, true ); // 인증통과 성공
	}
}