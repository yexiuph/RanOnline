///////////////////////////////////////////////////////////////////////////////
// s_CFieldServer.h
//
// class CFieldServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////
#ifndef S_CFieldServer_H_
#define S_CFieldServer_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_CServer.h"
#include "s_CClientField.h"
#include "s_CDbAction.h"

#include "../[Lib]__MfcEx/Sources/ExceptionHandler.h"

#include "../[Lib]__Engine/Sources/G-Logic/DxMsgServer.h"

#include "../[Lib]__RanClient/Sources/DbAction/DbActionLogic.h"
#include "../[Lib]__RanClient/Sources/DxServerInstance.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLGaeaServer.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLChar.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlMsg.h"


#define WM_FINALCLEANUPFINISH                      (WM_USER+157)

class CFieldServer : public CServer, public DxMsgServer
{
public :
	CFieldServer(
		HWND hWnd,
		HWND hEditBox,
		HWND hEditBoxInfo );
	~CFieldServer();

protected :	
	// Data member for session server
	// SOCKET				m_sSession;	// Socket for session server
	CClientField*		m_pClientManager;
	DWORD				m_dwFieldServerID;
	DWORD				m_dwAgentSlot;
	DWORD				m_dwHeartBeatStart1;
	DWORD				m_dwHeartBeatStart2;
	bool				m_bReservationServerStop;	
	float				m_fReservationtime;			// 예약시간(5초)

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
	virtual  int Start();
	virtual  int Stop();	
	virtual	 int Pause();
	virtual	 int Resume();
	virtual	 int ReStart();

			 int FinalStop();

	         int StartClientManager();
	         int StartDbManager();
	
	virtual  int GetServerGroup ()		{ return m_nServerGroup; }
	virtual  int GetServerNum ()			{ return m_nServerNum; }

	virtual  int WorkProc();
	virtual  int UpdateProc();
			 int Update();
	virtual  int ListenProc();

	virtual	 int DatabaseProc();
	virtual  int LogDatabaseProc();
	virtual  int UserDatabaseProc();
	virtual  int WebDatabaseProc()  { return 0; }; // Web Database thread function
//	virtual  int WebDatabaseProc(); ///< Web Database thread function

			void WriteNprotectLog( DWORD dwClient, DWORD dwResult );

	        void Send			( DWORD nClient,  LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize );
	virtual  int SendClient		( DWORD dwClient, LPVOID pBuffer );
	virtual  int SendAgent		( DWORD dwClient, LPVOID pBuffer );
	virtual  int SendAgent		( LPVOID pBuffer );

	virtual  int SendField		( DWORD dwClient, LPVOID pBuffer )	{ GASSERT(0&&"CFieldServer::SendField()"); return 0; }
	virtual  int SendFieldSvr	( int nSvrNum, LPVOID pBuffer, int nServerChannel=0 ) { GASSERT(0&&"CFieldServer::SendField()"); return 0; }	

	virtual void SendAllClient	( LPVOID pBuffer );
	virtual void SendAllField	( LPVOID pBuffer, int nServerChannel=0 ) { assert(0&&""); }
	virtual void ResetGaeaID    ( DWORD dwClient );	
	        void CloseClient    ( DWORD dwClient );  // Close Client Connection
	        void CloseAllClient ();

	virtual void SendChannel    ( LPVOID pBuffer, int nServerChannel=0 ) { assert(0&&""); }
	virtual void SendAllChannel ( LPVOID pBuffer ) { assert(0&&""); }

	///////////////////////////////////////////////////////////////////////////////
	// Local Message process function ( gaeaserver로 직접 내부 메시지 전송. )
	int		InsertMsg			(int nClient, void* pMsg);

	// 추적 ID인지 아닌지 체크
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );

	/**
	* Message process function
	*/
	int	MsgProcess( MSG_LIST* pMsg );

	void MsgHeartbeatServerAnswer( MSG_LIST* pMsg );
	void MsgVersion( MSG_LIST* pMsg );
	void MsgLoginInfoAgent( MSG_LIST* pMsg );
	void MsgSndPingAnswer( MSG_LIST* pMsg );
	void MsgSndChatGlobal( char* szChatMsg );
	void MsgSndChatNormal(
			DWORD dwClient,
			const char* szName,
			const char* szMsg );
	void MsgSndCryptKey( DWORD dwClient );
	int MsgSndChaJoinToAgent(
			DWORD dwClient,
			int nChaNum,
			DWORD dwGaeaID,
			DWORD dwMapID );

	/**
	* 클라이언트->게임서버 : 처음 Field 서버에 접속, 케릭터를 생성, 필드와 클라이언트의 연결 준비.
	*/
	void MsgGameJoinChar( MSG_LIST* pMsg );
	/**
	* Client->Field : 조인했음을 알림
	*/
	void MsgJoinInfoFromClient( MSG_LIST* pMsg );
	/**
	* DB 스레드->필드서버:DB에서 읽어진 정보로 케릭터 접속 시도.
	*/
	void MsgFieldReqJoin( MSG_LIST* pMsg );
	void MsgFieldReqJoin( DWORD dwClientID, SCHARDATA2 * pCHAR_DATA, GLMSG::SNETJOIN2FIELDSERVER::SINFO & sINFO );
	void SetUserInfo(
			DWORD nClient,
			const char* strUserId,
			const char* strPassword );
	
	void ServerMsgProcess( MSG_LIST* pMsg );

	bool GetReservationServerStop() { return m_bReservationServerStop; };

	// Execute Command Text
	int	ExecuteCommand( char* strCmd );

    ///////////////////////////////////////////////////////////////////////////
	// Session Server Communication Member Function
	// If not use Session Server, this functions are unnecessary
	///////////////////////////////////////////////////////////////////////////
	int	SessionConnect(
			unsigned long ulServerIP,
			int nPort );

	int	SessionConnect(
			const char* szServerIP,
			int nPort );

	//! Session 서버에 접속하고 서버의 정보를 전송한다.
	int SessionConnectSndSvrInfo(const char* szServerIP, int nPort);
	int SessionConnectSndSvrInfo(void);	

	int	SessionCloseConnect(void);
	int SendSession(LPVOID pBuffer);
	void SessionMsgProcess(MSG_LIST* pMsg);	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg); // Ping Answer
	void SessionMsgLoginFeedBack(NET_MSG_GENERIC* nmg); // 세션서버 -> 게임서버 : 로그인결과 데이타
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg); // 세션서버->게임서버 : 채팅메시지
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);
	void SessionMsgHeartbeatServerAns();

	// void SessionSndPing(); // Send ping
	int SessionSndSvrInfo();
    void SessionSndHeartbeat();
	int SessionSndSvrCurState();
	int	SessionSndLoginInfo(const char* szUsrID, const char* szUsrPass, int nClient);
	void SessionSndLogOutInfo(const char* szUsrID, int nUserNum=0, int nGameTime=0, int nChaNum=0);
	void SessionSndLogOutInfoSimple(const char* szUsrID);

	BOOL UpdateTracingCharacter(NET_MSG_GENERIC* nmg );

	// 받은 메시지들을 처리한다.
	void RecvMsgProcess();
	// 하트비트 체크를 한다.
	void FieldSrvHeartBeatCheck( DWORD dwUdpateTime );
	// 현재 서버의 상태를 프린트 한다.
	void PrintDebugMsg( DWORD dwUdpateTime );
	// UpdateThread 종료
	void EndUpdateThread();
};

#endif // S_CFieldServer_H_