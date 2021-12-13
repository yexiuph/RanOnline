///////////////////////////////////////////////////////////////////////////////
// s_CClientAgent.cpp
//
// class CClientAgent
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright(c) Min Communications. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CClientAgent.h"
#include "s_CConsoleMessage.h"

//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
bool CClientAgent::nProtectSetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectSetAuthQuery();
	}
	else
	{
		return m_pClient[dwClient].nProtectSetAuthQuery();
	}
}

//! nProtect GameGuard 인증 응답 가져오기
GG_AUTH_DATA CClientAgent::nProtectGetAuthAnswer( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectGetAuthAnswer();
	}
	else
	{
		return m_pClient[dwClient].nProtectGetAuthAnswer();
	}
}

//! nProtect GameGuard 인증용 쿼리 가져오기
GG_AUTH_DATA CClientAgent::nProtectGetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectGetAuthQuery();
	}
	else
	{
		return m_pClient[dwClient].nProtectGetAuthQuery();
	}
}

//! nProtect GameGuard 인증 응답 설정
void CClientAgent::nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		m_pClient[dwClient-m_nMaxClient].nProtectSetAuthAnswer(ggad);
	}
	else
	{
		m_pClient[dwClient].nProtectSetAuthAnswer(ggad);
	}
}

//! nProtect GameGuard 인증 결과 가져오기
DWORD CClientAgent::nProtectCheckAuthAnswer(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectCheckAuthAnswer();
	}
	else
	{
		return m_pClient[dwClient].nProtectCheckAuthAnswer();
	}
}

//! nProtect GameGuard 인증 상태를 리셋시킨다.
void CClientAgent::nProtectResetAuth(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		m_pClient[dwClient-m_nMaxClient].nProtectResetAuth();
	}
	else
	{
		m_pClient[dwClient].nProtectResetAuth();
	}
}

//! nProtect 처음 인증여부 세팅
void CClientAgent::nProtectFirstCheck(DWORD dwClient, bool bState)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		m_pClient[dwClient-m_nMaxClient].nProtectSetFirstCheck(bState);
	}
	else
	{
		m_pClient[dwClient].nProtectSetFirstCheck(bState);
	}
}

//! nProtect 처음 인증인가?
bool CClientAgent::nProtectIsFirstCheck(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectIsFirstCheck();
	}
	else
	{
		return m_pClient[dwClient].nProtectIsFirstCheck();
	}
}

//! nProtect GameGuard 처음 인증을 통과했는가?
//! 이 인증을 통과해야 게임플레이를 할 수 있다.
bool CClientAgent::nProtectFirstAuthPass( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		return m_pClient[dwClient-m_nMaxClient].nProtectFirstAuthPass();
	}
	else
	{
		return m_pClient[dwClient].nProtectFirstAuthPass();
	}
}

//! nProtect GameGuard 처음인증 통과상태 세팅
void CClientAgent::nProtectSetFirstAuthPass( DWORD dwClient, bool bState )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		m_pClient[dwClient-m_nMaxClient].nProtectSetFirstAuthPass(bState);
	}
	else
	{
		m_pClient[dwClient].nProtectSetFirstAuthPass(bState);
	}
}