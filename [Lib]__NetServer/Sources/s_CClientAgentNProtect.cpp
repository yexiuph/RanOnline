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

//! nProtect GameGuard ������ ������ �������� ���� ȣ��
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

//! nProtect GameGuard ���� ���� ��������
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

//! nProtect GameGuard ������ ���� ��������
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

//! nProtect GameGuard ���� ���� ����
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

//! nProtect GameGuard ���� ��� ��������
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

//! nProtect GameGuard ���� ���¸� ���½�Ų��.
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

//! nProtect ó�� �������� ����
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

//! nProtect ó�� �����ΰ�?
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

//! nProtect GameGuard ó�� ������ ����ߴ°�?
//! �� ������ ����ؾ� �����÷��̸� �� �� �ִ�.
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

//! nProtect GameGuard ó������ ������� ����
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