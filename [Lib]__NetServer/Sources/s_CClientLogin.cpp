///////////////////////////////////////////////////////////////////////////////
// s_CClientLogin.cpp
//
// class CClientLogin
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "s_CClientLogin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CClientLogin::CClientLogin(
	int nMaxClient,
	COverlapped* pSendIOCP,
	COverlapped* pRecvIOCP,
	HANDLE	hIOServer )
	: CClientManager(
		nMaxClient,
		pSendIOCP,
		pRecvIOCP,
		hIOServer )
{
}

CClientLogin::~CClientLogin()
{
}

// 현재 접속중인 클라이언트 숫자
int CClientLogin::GetCurrentClientNumber()
{
	return m_nMaxClient - (int) m_deqSleepCID.size() - NET_RESERVED_SLOT;
}