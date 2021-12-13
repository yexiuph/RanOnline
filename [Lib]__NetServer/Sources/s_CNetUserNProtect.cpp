///////////////////////////////////////////////////////////////////////////////
// s_CNetUserNProtect.cpp
// 
// class CNetUser
//
// * History
// 2006.03.30 Jgkim 
//
// Copyright(c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_CNetUser.h"
#include "s_CConsoleMessage.h"
#include "GLGaeaServer.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//! nProtect GameGuard ������ ������ �������� ���� ȣ��
bool CNetUser::nProtectSetAuthQuery()
{
    if (m_csa.GetAuthQuery() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//! nProtect GameGuard ������ ���� ��������
GG_AUTH_DATA CNetUser::nProtectGetAuthQuery()
{
	return m_csa.m_AuthQuery;
}

//! nProtect GameGuard ���� ���� ��������
GG_AUTH_DATA CNetUser::nProtectGetAuthAnswer()
{
	return m_csa.m_AuthAnswer;
}

//! nProtect GameGuard ���� ���� ����
void CNetUser::nProtectSetAuthAnswer(GG_AUTH_DATA& ggad)
{
	m_csa.m_AuthAnswer = ggad;
}

//! nProtect GameGuard ���� ��� ��������
DWORD CNetUser::nProtectCheckAuthAnswer()
{
	// �������ӽ� �ѹ��� true �� �ش�.
	if (m_bNProtectAuth == true)
	{
		m_bNProtectAuth = false;
		return 0;
	}
	else
	{
		DWORD dwResult = m_csa.CheckAuthAnswer();
		if (dwResult == 0) // ��������
		{
			return 0;
		}
		else // ��������
		{
			return dwResult;
		}
	}
}

void CNetUser::nProtectResetAuth()
{
	m_bNProtectAuth = false;
}

//! nProtect ó�� �����ΰ�?
bool CNetUser::nProtectIsFirstCheck()
{
	return m_bNProtectFirstCheck;
}

//! nProtect ó�� �������� ����
void CNetUser::nProtectSetFirstCheck(bool bState)
{
	m_bNProtectFirstCheck = bState;
}

bool CNetUser::nProtectFirstAuthPass()
{
	return m_bNProtectFirstAuthPass;
}

void CNetUser::nProtectSetFirstAuthPass(bool bState)
{
	m_bNProtectFirstAuthPass = bState;
}
