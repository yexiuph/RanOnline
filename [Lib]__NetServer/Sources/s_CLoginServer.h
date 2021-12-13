#ifndef S_CLoginServer_H_
#define S_CLoginServer_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_CServer.h"
#include "s_CClientLogin.h"
#include "s_COverlapped.h"
#include "s_CConsoleMessage.h"
#include "s_CCfg.h"

/**
 * \ingroup NetServerLib
 *
 *
 * \par requirements 
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2002-05-30
 *
 * \author Jgkim
 *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 *
 * \todo
 *
 * \bug 
 *
 */
class CLoginServer : public CServer
{
public:
	CLoginServer(
		HWND hWnd,
		HWND hEditBox,
		HWND hEditBoxInfo );
	~CLoginServer();

protected:
	//! Data member for login server
	// SOCKET				m_sSession;	///< Socket for session server
	CClientLogin*		m_pClientManager; ///< Client Manager
	bool				m_bInspecting; ///< 서버 점검중 체크 변수
		
	DWORD				m_dwLastGameSvrUpdateTime;
	G_SERVER_CUR_INFO_LOGIN m_sGame[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];

public:
	//! -----------------------------------------------------------------------
	//! I/O operation memory
	//virtual int StopIOCPList();
	//virtual int StartIOCPList() { return NET_OK; }
	//virtual PER_IO_OPERATION_DATA* getSendIO( DWORD dwClient );
	//virtual PER_IO_OPERATION_DATA* getRecvIO( DWORD dwClient );	
	//virtual void releaseSendIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
	//virtual void releaseRecvIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
	//virtual void releaseIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
	

public:
	virtual int	Start();
	virtual int	Stop();	
	virtual	int	Pause();
	virtual	int Resume();
	virtual	int	ReStart();
		
			int	StartClientManager();
	
	virtual int	WorkProc();
	virtual int	UpdateProc();	
	virtual int ListenProc();

	virtual	int DatabaseProc()     { return 0; }; // Database thread function
	virtual	int LogDatabaseProc()  { return 0; }; // Log Database thread function
	virtual	int UserDatabaseProc() { return 0; }; // User Database thread function
	virtual int	 WebDatabaseProc() { return 0; }; // Web Database thread function
    
	virtual int InsertMsg(int nClient,	void* pMsg) { return 0; };

	void	Send(DWORD nClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
	int		SendClient(DWORD dwClient, LPVOID pBuffer);
	void	CloseClient(DWORD dwClient);
	void	CloseAllClient();

	void	SetInspecting(bool bStatus);
	bool	GetInspection();

	void	SetVersion(int nGameVer, int nPatchVer);	

	void	GenerateEncrypt();

	bool	CheckAllowIP( SOCKET socket );

	void	SetAllowIP( bool bAllowIP );
	bool	GetAllowIP();

	//////////////////////////////////////////////////////////////////////////////
	/// Message process function
	//////////////////////////////////////////////////////////////////////////////
	int MsgProcess(MSG_LIST* pMsg);
	void ServerMsgProcess(MSG_LIST* pMsg);

	void MsgVersion(MSG_LIST* pMsg);
	
	//! 게임서버정보를 클라이언트에 전송한다.
	void MsgSndGameSvrInfo(MSG_LIST* pMsg); 
//	void MsgSndPingAnswer(MSG_LIST* pMsg);

	//////////////////////////////////////////////////////////////////////////////
	/// Execute Command Text
	//////////////////////////////////////////////////////////////////////////////
	int	ExecuteCommand(char* strCmd);

	//////////////////////////////////////////////////////////////////////////////
	/// Session Server Communication Member Function	
	//////////////////////////////////////////////////////////////////////////////
	int SessionConnect();

	int	SessionConnect(
			unsigned long ulServerIP,
			int nPort );

	int	SessionConnect(
			const char* szServerIP,
			int nPort );

	//! Session 서버에 접속하고 서버의 정보를 전송한다.	
	int SessionConnectSndSvrInfo(void);

	void SessionCloseConnect(void);
	int SendSession(LPVOID pBuffer);		
	void SessionMsgProcess(MSG_LIST* pMsg);	
	
	//! Ping Answer
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); 
    void SessionSndHeartbeat(void);
	void SessionMsgSvrInfo(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrInfoReset(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);
	void SessionMsgHeartbeatServerAns();

	void SessionSndSvrInfo();
	void SessionSndSvrChannelFullInfo(NET_MSG_GENERIC* nmg);

	void SessionSndSvrLoginMaintenance( NET_MSG_GENERIC* nmg );
	//! Send ping
	// void SessionReqPing(void); 
	//! Request Game Server Info
	void SessionReqSvrInfo(void); 
	void SessionSndKey(void);
};

#endif // S_CLoginServer_H_

//                           /       /
//                        .'<_.-._.'<
//                       /           \      .^.
//     ._               |  -+- -+-    |    (_|_)
//  r- |\                \   /       /      // 
///\ \\  :                \  -=-    /       \\
// `. \\.'           ___.__`..;._.-'---...  //
//   ``\\      __.--"        `;'     __   `-.  
//     /\\.--""      __.,              ""-.  ".
//     ;=r    __.---"   | `__    __'   / .'  .'
//     '=/\\""           \             .'  .'
//         \\             |  __ __    /   |
//          \\            |  -- --   //`'`'
//           \\           |  -- --  ' | //
//            \\          |    .      |// 
// jgkim