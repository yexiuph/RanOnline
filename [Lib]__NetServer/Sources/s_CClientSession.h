///////////////////////////////////////////////////////////////////////////////
// s_CClientSession.h
//
// class CClientSession
//
// * History
// 2002.05.30 jgkim Create
// 2002.01.23 jgkim Add memberfunction IsGameServer(DWORD dwClient)
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTSESSION_H_
#define S_CCLIENTSESSION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

#define USER_MAP_START_SIZE 100

struct CNT_USR_INFO
{		
	INT		nSvrGrp; // 서버 그룹
	INT		nSvrNum; // 서버 번호
	INT		nUsrNum; // 사용자 번호	(DB)
	INT		nChaNum; // 캐릭터번호(DB)	
	DWORD	dwGaeaID; // 가이아 ID
	
	DWORD	dwSvrClient; // 세션서버 클라이언트 번호
	DWORD	dwGameClient; // 게임서버 클라이언트 번호
	TCHAR	szUserID[GSP_USERID]; // 사용자 ID
	TCHAR	szChaName[CHR_ID_LENGTH+1]; // 플레이중인 캐릭터명	
	CNT_USR_INFO()
	{
		nSvrGrp			= -1;
		nSvrNum			= -1;
		nUsrNum			= -1;
		nChaNum			= -1;
		dwGaeaID		= -1;
		
		dwSvrClient		= -1;
		dwGameClient	= -1;
		memset( szUserID, 0,  sizeof(TCHAR) * (GSP_USERID) );
		memset( szChaName, 0, sizeof(TCHAR) * (CHR_ID_LENGTH+1) );
	}
};
typedef CNT_USR_INFO* LPCNT_USR_INFO;

/**
 * \ingroup NetServerLib
 *
 * \par requirements
 * win2k or later\n
 * MFC\n
 *
 * \version 1.1
 *
 * \date 2003-02-28 
 *
 * \author jgkim
 *
 * \par license
 * Copyright 2002-2006 (c) Mincoms. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 *
 */
class CClientSession : public CClientManager
{
public :
	CClientSession(int nMaxClient,
				COverlapped* pSendIOCP,
				COverlapped* pRecvIOCP,
				HANDLE	hIOServer );
	virtual ~CClientSession();

protected :	
	CMapStringToPtr* m_pUsrMap; 
	LPG_SERVER_INFO	 m_pServerInfo;
	

public :
	void	SetSvrInfo			(DWORD dwClient, G_SERVER_INFO *nsi);
	void	SetSvrCurInfo		(DWORD dwClient, G_SERVER_CUR_INFO *nsi);

	G_SERVER_INFO
			GetSvrInfo			(DWORD dwClient);
	int		GetSvrType			(DWORD dwClient);
	int		GetSvrGrp			(DWORD dwClient);
	int		GetSvrNum			(DWORD dwClient);
	int		GetSvrField			(DWORD dwClient);
	
	int		GetSvrCrtClient		(DWORD dwClient);
	int		GetSvrMaxClient		(DWORD dwClient);
	int		GetSvrServicePort	(DWORD dwClient);
	char*	GetSvrIP			(DWORD dwClient);
	bool	IsGameServer		(DWORD dwClient);

	DWORD	FindServer		(const char* szServerIP, int nServicePort);
	DWORD	FindServer		(int nType, int nSvrGrp, int nSvrNum, int nField);
	
	void	UserInsert		(CString strUsrID, LPCNT_USR_INFO pData);	
	void	UpdateChaJoinOK	(CString strUsrID, 							
							int nUsrNum, 
							int nChaNum, 
							DWORD dwGaeaID,
							const char* szChaName);

	BOOL	UserErase		(CString strUsrID);
	void	UserEraseServer	(int nSvrGrp, int nSvrNum);
	void	UserEraseAll	();
	LPCNT_USR_INFO	
			UserFind		(CString strUsrID);
	
	void	PrintUsrList();

	int		CloseClient(DWORD dwClientNum);
};

#endif // S_CCLIENTSESSION_H_