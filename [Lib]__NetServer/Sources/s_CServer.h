///////////////////////////////////////////////////////////////////////////////
// s_CServer.h
// class CServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CSERVER_H_
#define S_CSERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_COverlapped.h"
#include "s_CConsoleMessage.h"

#include "s_COdbcManager.h"
#include "s_CSystemInfo.h"
#include "s_CCfg.h"
#include "s_CBit.h"
#include "s_CSMsgList.h"
#include "s_CAddressChecker.h"
#include "s_CWhiteRock.h"

// Game Logic Define
#include "../[Lib]__Engine/Sources/G-Logic/GLDefine.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLCharDefine.h"

// Daum Game Crypt lib
#include "DaumGameAuth.h"

#define S_HEURISTIC_NUM			1 // IOCP Worker thread number per CPU 
#define S_HEURISTIC_QUEUE_SIZE	20 // 큐사이즈는 기본 최대사용자 X S_HEURISTIC_QUEUE_SIZE 배를 형성한다.

#define S_SERVER_STOP			1
#define S_SERVER_RUNING			2
#define S_SERVER_PAUSE			3

#define CHA_SAVE_TIME			1800000  // 1000 * 60 * 30 캐릭터 저장시간   30분
#define CHA_SAVE_CHK_TIME        600000  // 1000 * 60 * 10 캐릭터 체크시간   10분
#define FPS_UPDATE_TIME			  60000	 // 1000 * 60      FPS 표시 갱신시간  1분
#define SERVER_STATE_CHK_TIME	 3600000 // 1000 * 60 * 60 서버 상태 로그 기록 시간  60분
//#define HEARTBEAT_TIME           3600000 // 1000 * 60 * 60  Heart beat time 60분
#define HEARTBEAT_TIME           120000 // 1000 * 60 * 2  Heart beat time    2분
#define SESSION_CHECK_TIME		 300000  // 1000 * 60 * 5 Session 상태 체크 5분

#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }

// class CServer;

class CServer : public CLock
{
public:
	CServer(
		HWND hWnd,
		HWND hEditBox,
		HWND hEditBoxInfo );
	virtual ~CServer();

	typedef std::vector<STracingData> VEC_TRACING;
	typedef VEC_TRACING::iterator	  VEC_TRACING_ITER;

protected:
    //DWORD m_dwSession; //! Session 서버 슬롯번호

	COverlapped*		m_pRecvIOCP;
	COverlapped*		m_pSendIOCP;
	// CDbmanager*		m_pDB;
	COdbcManager*		m_pDB;
	// COverlapped*		m_pOverlapped;
	CSMsgManager*		m_pRecvMsgManager;
	SERVER_UTIL::CBit	m_Bit;

	DWORD				m_dwHeartBeatTime;
	DWORD				m_dwFrameTime;
	DWORD				m_dwFrameCount;
	
	DWORD m_dwCpuCount; ///< 시스템의 CPU 갯수

	HANDLE				m_hWorkerThread[MAX_WORKER_THREAD];
	HANDLE				m_hIOServer;			// IOCP Service 	
	HANDLE				m_hAcceptThread;		// Listening thread handle

	HANDLE				m_hUpdateThread;		// Game Update thread handle
	HANDLE				m_hUpdateQuitEvent;
	DWORD				m_dwUpdateWaitTime;
	DWORD				m_dwUpdateOldTime;
	DWORD				m_dwUpdateFrame;



	HWND				m_hEditBox;				// Main window, EditBox handle
	HWND				m_hEditBoxInfo;			// Main window, Info EditBox handle
	HWND				m_hWnd;					// Main window handle
	
	// SOCKADDR_IN			m_Addr;
	SOCKET				m_sServer;				// Socket

	bool				m_bTestServer;			// Test Server?
	bool				m_bAdult;				// Server for adult
	bool				m_bIsRunning;			// Status flag
	bool				m_bUpdateEnd;			// Update thread end flag
	bool				m_bPK;					// true:PK ON, false:PK OFF
	bool                m_bPKLess;              /// All attack between chatacter disable
	int					m_nStatus;				// Detail status flag
	int					m_nPort;				// Server port

	int					m_nVersion;				// Server version
	int					m_nPatchVersion;		// Patch Program Version

	int					m_nServerGroup;			// Server Group
	int					m_nServerNum;			// Server Number
	int                 m_nServerChannel;       // Server Channel
	int                 m_nServerChannelNumber; // Server Channel Number (Agent 가 가진 서버 채널 갯수)

	int					m_nMaxClient;			// Maximum numbers of client
	int                 m_nMaxChannelUser;      // Maximum numbers of channel client
	

	int					m_nServiceProvider;     // Service provider
	char				m_szAddress[MAX_IP_LENGTH+1];			// Server IP Address
	char				m_pRecvBuffer[NET_DATA_BUFSIZE];

	DWORD				m_dwWorkerThreadNumber;	// Worker Thread 의 갯수
	DWORD				m_dwCompKey;

	bool				m_bUseEventThread;

	DWORD				m_dwLangSet;

	std::vector< CString > m_vecAllowIPType1;
	std::vector< CString > m_vecAllowIPType2;
	std::vector< CString > m_vecAllowIPType3;
	bool					m_bAllowIP;

	// int					m_nPacketInCount;			// Inbound  Packet Counter
	// int					m_nPacketInSize;			// Inbound  Packet Size (Byte)
	// int					m_nPacketOutCount;			// Outbound Packet Counter
	// int					m_nPacketOutSize;			// Outbound Packet Size (Byte)

	DWORD				m_dwLastDropClient;		    // Last Drop Client;

	bool                m_bGameGuardInit;           // nProtect GameGuard 초기화 성공여부
	bool                m_bGameGuardAuth;           // nProtect GameGuard 인증 사용여부

	VEC_TRACING			m_vecTracingData;           // 추척 캐릭터들 리스트

	char				m_szEncrypt[ENCRYPT_KEY+1];
	
public :
	virtual int	Start()   = 0;
	virtual int	Stop()    = 0;
	virtual	int	Pause()   = 0;
	virtual	int	Resume()  = 0;
	virtual	int	ReStart() = 0;	

			int	StartCfg(BYTE nType);
			int StartAllowIPCfg();
			int	StartIOCPList();
			int	StartMsgManager();
			int	StartIOCP();
	virtual	int	StartClientManager() = 0;
			
			int	StartWorkThread();
			int	StartUpdateThread();
			int	StartListenThread();

			int StartDatabaseThread();
			int StartLogDatabaseThread();
			int StartUserDatabaseThread();
			int StartWebDatabaseThread();
	
			int	StopListenThread();
			int StopIOCPList();
			int	StopWorkThread();
			int	StopIOCP();
	
			int	GetGameVersion()     { return m_nVersion; };
			int	GetLauncherVersion() { return m_nPatchVersion; };

			int GetServerGroup()     { return m_nServerGroup; };
			int GetServerNum()       { return m_nServerNum; };
			int GetServerChannel()   { return m_nServerChannel; };

	virtual	int	WorkProc()        = 0; // I/O Process
	virtual int	ListenProc()      = 0; // Accept new Client connection	

	virtual int	UpdateProc()      = 0; // Message Process
	virtual	int DatabaseProc()    = 0; // Database thread function
	virtual int LogDatabaseProc() = 0; // Log Database thread function
	virtual int UserDatabaseProc()= 0; // User Database thread function
	virtual int WebDatabaseProc() = 0; // Web Database thread function

			void setUpdateFrame( DWORD dwFrame );

	virtual int	InsertMsg( int nClient, void* pMsg ) = 0;	

			bool IsRunning() { return m_bIsRunning; }
			int	GetStatus() { return m_nStatus; }
			void SetStatus(int nStatus) { m_nStatus = nStatus; }	
			
	// I/O operation memory
	LPPER_IO_OPERATION_DATA GetFreeOverIO( int nType=0 );
	void ReleaseOperationData( PER_IO_OPERATION_DATA* pData );
	
	//! -----------------------------------------------------------------------
	//! I/O operation memory
	//virtual PER_IO_OPERATION_DATA* getSendIO( DWORD dwClient ) = 0;
	//virtual PER_IO_OPERATION_DATA* getRecvIO( DWORD dwClient ) = 0;	
	//virtual void releaseSendIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData ) = 0;
	//virtual void releaseRecvIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData ) = 0;
	//virtual void releaseIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData ) = 0;
	
};

static unsigned int WINAPI CServerListenProc( void* pThis )
{ 
	CServer* pTemp = (CServer*) pThis;
	return pTemp->ListenProc();
}

static unsigned int WINAPI CServerWorkProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->WorkProc();
}

static unsigned int WINAPI CServerUpdateProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->UpdateProc();
}

static unsigned int WINAPI CServerDatabaseProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->DatabaseProc(); 
}

static unsigned int WINAPI CServerLogDatabaseProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->LogDatabaseProc();
}

static unsigned int WINAPI CServerUserDatabaseProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->UserDatabaseProc();
}

static unsigned int WINAPI CServerWebDatabaseProc( void* pThis )
{
	CServer* pTemp = (CServer*) pThis;
	return pTemp->WebDatabaseProc();
}

#endif // S_CSERVER_H_
