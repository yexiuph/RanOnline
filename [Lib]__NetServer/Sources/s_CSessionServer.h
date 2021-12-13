#ifndef S_SESSIONSERVER_H_
#define S_SESSIONSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_CServer.h"
#include "s_CClientSession.h"
#include "s_CDbAction.h"

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
 * \author jgkim
 *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 * 
 */
class CSessionServer : public CServer
{
public :
	CSessionServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CSessionServer();

protected :
	CClientSession*	m_pClientManager; ///< Client Manager
	G_SERVER_CUR_INFO_LOGIN m_sServerChannel[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	bool					m_bServerChannelFull[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	int						m_dwLoginServerID;	

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

public :
	virtual int	 Start();
	virtual int	 Stop();	
	virtual	int	 Pause();
	virtual	int  Resume();
	virtual	int	 ReStart();

	        int	 StartClientManager();
	        int	 StartDbManager();
	
	virtual int	 WorkProc();
	virtual int	 UpdateProc();	
	virtual int  ListenProc();

	virtual	int  DatabaseProc();					
	virtual	int  LogDatabaseProc()  { return 0; };	//! Log Database thread function
	virtual	int  UserDatabaseProc() { return 0; };	//! User Database thread function
	virtual int	 WebDatabaseProc()  { return 0; }; // Web Database thread function

	virtual int  InsertMsg(int nClient, void* pMsg) { return 0; };

	        void Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
		    int	 SendClient(DWORD dwClient, LPVOID pBuffer);
	        void CloseClient( DWORD dwClient );
	        //! close all client connections
			void CloseAllClient();
	        int  FindServer(CString strIP, int nPort);
	        int  FindServer(int nSvrType, int nSvrGrp, int nSvrNum, int nFieldNum);

	        void UserInsert(CString strUsrID, LPCNT_USR_INFO pData);
			void SetSvrInfo(DWORD dwClient, G_SERVER_INFO *gsi);
			void SetChannelInfo(NET_SERVER_CHANNEL_INFO* pMsg);			
		
	        ///////////////////////////////////////////////////////////////////
	        /// Message process function
			///////////////////////////////////////////////////////////////////
	        int  MsgProcess				(MSG_LIST* pMsg);
	        void MsgChatProcess			(MSG_LIST* pMsg);
			//! Control program->Session server : command message
	        void MsgSvrCmdProcess       (MSG_LIST* pMsg);
			void MsgTracingChar			(MSG_LIST* pMsg, bool bAll);
			void MsgTracingLoginout		(MSG_LIST* pMsg);
	
	        void MsgSndCurSvrInfo		(DWORD nClient);
	        void MsgSndSvrInfoReset		(DWORD nClient);
	        void MsgSndCurSvrInfoToAll	(void);
	        void MsgSndAllSvrInfo		(MSG_LIST* pMsg);
	        void MsgSndChatGlobal		(char* szChatMsg);
	        void MsgSndPingAnswer		(MSG_LIST* pMsg);	
	
	        void MsgSndLoginResult		(MSG_LIST* pMsg);	
	        void MsgSndLoginResultBack  (DWORD dwClient, NET_LOGIN_FEEDBACK_DATA2* pData);

	        void DaumMsgSndLoginResult   (MSG_LIST* pMsg);
	        void DaumMsgSndLoginResultBack(DWORD dwClient, DAUM_NET_LOGIN_FEEDBACK_DATA2* pData);

	        void MsgServerInfo			(MSG_LIST* pMsg);
			void MsgEncryptKey			(MSG_LIST* pMsg);
			void MsgSndEncryptKey		( DWORD dwClient );

			// Server->Server Hearbeat Answer
			void MsgServerHeartbeatAnswer( MSG_LIST* pMsg );
//          void MsgHeartBeat           (MSG_LIST* pMsg);
	        void MsgServerCurInfo		(MSG_LIST* pMsg);	
			void MsgServerChannelInfo   (MSG_LIST* pMsg);
	        void MsgSndChatCtrlGlobal	(MSG_LIST* pMsg);
	        void MsgSndChatCtrlGlobal2	(MSG_LIST* pMsg);

	        //! Session->Agent, Field
	        int	 MsgSndServerStateChange(int nClient, int nState); 

	        //! Agent->Session : 사용자 로그아웃
			void MsgUsrLogout			(MSG_LIST* pMsg);
			void DaumMsgUsrLogout		(MSG_LIST* pMsg);
			void MsgUsrLogoutSimple		(MSG_LIST* pMsg);

			//! Agent->Session : 캐릭터가 게임에 조인했음
			void MsgGameJoinOK			(MSG_LIST* pMsg);

			void MsgReqAllSvrFullInfo	(void);
			void MsgReqSvrFullInfo		(DWORD nClient);
			
			void MsgReqAllSvrCurInfo	(void);
			void MsgReqSvrCurInfo		(DWORD nClient);

			//! Agent->Session : 캐릭터 생성, 갯수 감소
			int MsgChaDecrease(MSG_LIST* pMsg);
			//! Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
			int MsgTestChaDecrease(MSG_LIST* pMsg);
			//! DAUM:Agent->Session : 캐릭터 생성, 갯수 감소
			int DaumMsgChaDecrease(MSG_LIST* pMsg);
			//! DAUM:Agent->Session :TEST 서버  캐릭터 생성, 갯수 감소
			int DaumMsgTestChaDecrease(MSG_LIST* pMsg);

			//! Agent->Session:캐릭터 삭제, 갯수 증가
			int MsgChaIncrease(MSG_LIST* pMsg);
			//! Agent->Session:TEST 서버 캐릭터 삭제, 갯수 증가
			int MsgTestChaIncrease(MSG_LIST* pMsg);
			//! DAUM:Agent->Session : 캐릭터 삭제, 갯수 증가
			int	DaumMsgChaIncrease(MSG_LIST* pMsg);
			//! DAUM:Agent->Session :TEST 서버  캐릭터 삭제, 갯수 증가
			int DaumMsgTestChaIncrease(MSG_LIST* pMsg);
			int ExecuteCommand(char* strCmd);

			int MsgLoginMaintenance( MSG_LIST* pMsg );
};

#endif // S_SESSIONSERVER_H_