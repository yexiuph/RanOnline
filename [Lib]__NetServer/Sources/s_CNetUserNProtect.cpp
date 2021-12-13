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

//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
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

//! nProtect GameGuard 인증용 쿼리 가져오기
GG_AUTH_DATA CNetUser::nProtectGetAuthQuery()
{
	return m_csa.m_AuthQuery;
}

//! nProtect GameGuard 인증 응답 가져오기
GG_AUTH_DATA CNetUser::nProtectGetAuthAnswer()
{
	return m_csa.m_AuthAnswer;
}

//! nProtect GameGuard 인증 응답 설정
void CNetUser::nProtectSetAuthAnswer(GG_AUTH_DATA& ggad)
{
	m_csa.m_AuthAnswer = ggad;
}

//! nProtect GameGuard 인증 결과 가져오기
DWORD CNetUser::nProtectCheckAuthAnswer()
{
	// 최초접속시 한번은 true 를 준다.
	if (m_bNProtectAuth == true)
	{
		m_bNProtectAuth = false;
		return 0;
	}
	else
	{
		DWORD dwResult = m_csa.CheckAuthAnswer();
		if (dwResult == 0) // 인증성공
		{
			return 0;
		}
		else // 인증실패
		{
			return dwResult;
		}
	}
}

void CNetUser::nProtectResetAuth()
{
	m_bNProtectAuth = false;
}

//! nProtect 처음 인증인가?
bool CNetUser::nProtectIsFirstCheck()
{
	return m_bNProtectFirstCheck;
}

//! nProtect 처음 인증여부 세팅
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
