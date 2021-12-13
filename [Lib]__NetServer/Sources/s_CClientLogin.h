///////////////////////////////////////////////////////////////////////////////
// s_CClientLogin.h
//
// class CClientLogin
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTLOGIN_H_
#define S_CCLIENTLOGIN_H_

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientLogin : public CClientManager
{
public :
	CClientLogin(int nMaxClient,
				COverlapped* pSendIOCP,
				COverlapped* pRecvIOCP,
				HANDLE	hIOServer );
	virtual ~CClientLogin();

public:
	int	GetCurrentClientNumber(void); // 현재 접속중인 클라이언트 숫자
};

#endif // S_CCLIENTLOGIN_H_