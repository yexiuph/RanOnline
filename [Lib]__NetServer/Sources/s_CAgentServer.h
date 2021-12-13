///////////////////////////////////////////////////////////////////////////////
// s_CAgentServer.h
//
#ifndef S_CAgentServer_H_
#define S_CAgentServer_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_CServer.h"
#include "s_CClientAgent.h"
#include "s_CMinMd5.h"
#include "s_Util.h"
#include "s_CDbAction.h"
#include "minTea.h"
#include "ApexCloseList.h"
#include "s_CWhiteRock.h"

#include "./ApexProxy.h"
#include "s_CAgentServerApex.h"

#include "../[Lib]__MfcEx/Sources/ExceptionHandler.h"

#include "../[Lib]__Engine/Sources/G-Logic/DxMsgServer.h"

#include "../[Lib]__RanClient/Sources/DbAction/DbActionLogic.h"
#include "../[Lib]__RanClient/Sources/DxServerInstance.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLChar.h"
#include "../[Lib]__RanClient/Sources/G-Logic/GLMsg/GLContrlMsg.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Server/GLAgentServer.h"

#ifndef ERROR_MSG_LENGTH 
#define ERROR_MSG_LENGTH 256
#endif

#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	#import "EGameEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#elif defined ( MY_PARAM )
	#import "MyRossoEncrypt.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids
#endif

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
#define MAX_RANDOM_PASSWORD_NUM 30
#define	HTTP_STATUS_ALREADY_LOGIN 533

#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	extern IEGameEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#elif defined ( MY_PARAM )
	extern IMyRossoEncryptPtr    m_pMyRossoEncrypto; //! Terra Decrypt Module
#endif

class CAgentServer : public CServer, public DxMsgServer
{
public :
	CAgentServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CAgentServer();

protected :
	// SOCKET m_sSession; ///< Socket for session server
	CClientAgent* m_pClientManager; ///< Client 관리용
	int m_ChannelUser[MAX_CHANNEL_NUMBER]; ///< 채널별 사용자 숫자 관리
	
	CDaumGameAuth m_DaumAuth; ///< Daum Game Decrypt Module
	SERVER_UTIL::CMinMd5 m_MinMd5; ///< Excite Japan MD5
	minTea m_Tea; ///< Encrypt algorithm
	bool m_bHeartBeat;

	HMODULE m_hCheckStrDLL;

	BOOL (_stdcall *m_pCheckString)(CString);

	// typedef	std::pair<DWORD,DWORD>		APEX_ID_TIME;
	// typedef	std::list<APEX_ID_TIME>		APEX_CLOSE_LIST;
	// typedef	APEX_CLOSE_LIST::iterator	APEX_CLOSE_LIST_ITER;

	ApexCloseList m_ApexClose;
	// APEX_CLOSE_LIST m_listApexClose;

	// INFO:Reached Max Client Number 메시지가 뜰때 UpdateProc가 제대로 작동하는지를 알아보기 위한 변수
	bool m_bReachedMAX;


	CTime m_VietnamPresetTime;
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
	virtual int  GetServerGroup ()		{ return m_nServerGroup; }
	virtual int  GetServerNum ()		{ return m_nServerNum; }

	virtual int	 Start();
	virtual int	 Stop();	
	virtual	int	 Pause();
	virtual	int  Resume();
	virtual	int	 ReStart();
	
	        int	 StartClientManager();
	        int	 StartDbManager();

			void WriteNprotectLog( DWORD dwClient, DWORD dwResult );
	
	virtual int	 WorkProc();
	virtual int	 UpdateProc();	
	virtual int  ListenProc();	

	virtual	int  DatabaseProc();     ///< Database thread function
	virtual	int  LogDatabaseProc();  ///< Log Database thread function
	virtual	int  UserDatabaseProc(); ///< User Database thread function
	virtual int	 WebDatabaseProc(); ///< Web Database thread function

	virtual int ConnectFieldSvr( DWORD dwClient, int nFieldServer, DWORD dwGaeaID, int nServerChannel=0 );	
	
	virtual int SendAgent ( DWORD dwClient, LPVOID pBuffer )
				{ 
					assert(0&&"에이젼트에서는 SendAgent()를 호출할수 없습니다."); 
					return 0; 
				}
	virtual int SendAgent ( LPVOID pBuffer )		                 
				{ 
					assert(0&&"에이젼트에서는 SendAgent()를 호출할수 없습니다."); 
					return 0; 
				}	
	virtual int SendClient ( DWORD dwClient, LPVOID pBuffer );

			void SendAllClient ( LPVOID pBuffer );
			void SendAllClientChannel ( LPVOID pBuffer, int nServerChannel=0 );

	virtual void SendAllField ( LPVOID pBuffer, int nServerChannel=0 ) 
	             { 
					 BroadcastToField(pBuffer, nServerChannel); 
				 }	
	virtual int SendField ( DWORD dwClient, LPVOID pBuffer );
	virtual int SendFieldSvr ( int nSvrNum, 
		                       LPVOID pBuffer, 
							   int nServerChannel=0 )
				{ 
					return BroadcastToField ( nSvrNum, pBuffer, nServerChannel ); 
				}

	virtual void CloseClient(DWORD dwClient);  // Close Client Connection
            void CloseField (DWORD dwClient);
            void CloseAllClient(); 	// close all client connections
    
	virtual bool IsFieldOnline ( DWORD dwClient );
	virtual bool IsSlotOnline  ( DWORD dwClient );

	void	MsgVersion			(MSG_LIST* pMsg);	
	
	void	MsgCreateCharacter	(MSG_LIST* pMsg);    
    void    MsgCreateCharacterBack(int nChaNum, DWORD dwClient, DWORD dwUserNum, const char* szUserIP, USHORT uPort=0);	
	
	void    MsgSndPing			(DWORD dwClient);
	void	MsgSndPingAnswer	(MSG_LIST* pMsg);

	void	MsgSndChatGlobal	(char* szChatMsg);
	void	MsgSndChatNormal	(DWORD dwClient, const char* szName, const char* szMsg);
	void	MsgSndCryptKey		(DWORD dwClient);
	
	void	MsgSndLoginInfoToField(DWORD dwClient);

	void	MsgSndChaBasicBAInfo(MSG_LIST* pMsg);
	void	MsgSndChaBasicBAInfoBack(NET_CHA_BBA_INFO* pStruct, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	void	MsgSndChaBasicInfo  (MSG_LIST* pMsg);
	void	MsgSndChaBasicInfo  (DWORD dwClient, int nChaNum);
	void	MsgSndChaBasicInfoBack(GLMSG::SNETLOBBY_CHARINFO* snci, DWORD dwClient, const char* szUserIP, USHORT uPort=0);	
	
	void	MsgSndChaDelInfo( MSG_LIST* pMsg ); // Agent->Client:캐릭터 삭제 결과 전송	
	void	MsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // 게임서버->클라이언트 : 캐릭터 삭제 결과 전송	

	//! Agent->Client:캐릭터 삭제
	void DaumMsgSndChaDelInfo(MSG_LIST* pMsg);

	//! Agent->Client:캐릭터 삭제 결과 전송
	void DaumMsgSndChaDelInfoBack(int nChaNum, 
		                          int nUserNum, 
		                          int nResult, 
								  DWORD dwClient, 
								  const char* szUserIP, 
								  USHORT uPort=0);

	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void TerraMsgSndChaDelInfo( MSG_LIST* pMsg );

	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void GspMsgSndChaDelInfo( MSG_LIST* pMsg );

	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void ExciteMsgSndChaDelInfo(MSG_LIST* pMsg);

	/**
	*
	* \param nChaNum 
	* \param nUserNum 
	* \param nResult 
	* \param dwClient 
	* \param szUserIP 
	* \param uPort 
	*/
	void ExciteMsgSndChaDelInfoBack(int nChaNum, 
		                            int nUserNum, 
		                            int nResult, 
								    DWORD dwClient, 
								    const char* szUserIP, 
								    USHORT uPort=0);

	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void JapanMsgSndChaDelInfo(MSG_LIST* pMsg);

	/**
	*
	* \param nChaNum 
	* \param nUserNum 
	* \param nResult 
	* \param dwClient 
	* \param szUserIP 
	* \param uPort 
	*/
	void JapanMsgSndChaDelInfoBack(int nChaNum, 
		                            int nUserNum, 
		                            int nResult, 
								    DWORD dwClient, 
								    const char* szUserIP, 
								    USHORT uPort=0);

	/**
	* Agent->Client:캐릭터 삭제결과전송
	* \param nChaNum 캐릭터번호
	* \param nUserNum 유저번호
	* \param nResult 결과
	* \param dwClient 클라이언트번호
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void TerraMsgSndChaDelInfoBack(int nChaNum, 
	                               int nUserNum, 
								   int nResult, 
								   DWORD dwClient, 
								   const char* szUserIP, 
								   USHORT uPort=0);

	/**
	* Agent->Client:캐릭터 삭제결과전송
	* \param nChaNum 캐릭터번호
	* \param nUserNum 유저번호
	* \param nResult 결과
	* \param dwClient 클라이언트번호
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void GspMsgSndChaDelInfoBack(
			int nChaNum, 
	        int nUserNum, 
			int nResult, 
			DWORD dwClient, 
			const TCHAR* szUserIP, 
			USHORT uPort=0 );


	/**
	* Client->Agent:캐릭터 삭제요청
	* \param pMsg 
	*/
	void	GsMsgSndChaDelInfo( MSG_LIST* pMsg ); // Agent->Client:캐릭터 삭제 결과 전송	

	/**
	* Agent->Client:캐릭터 삭제결과전송
	* \param nChaNum 캐릭터번호
	* \param nUserNum 유저번호
	* \param nResult 결과
	* \param dwClient 클라이언트번호
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void	GsMsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // 게임서버->클라이언트 : 캐릭터 삭제 결과 전송	
	
	//! 클라이언트->에이젼트서버 : 캐릭터 선택후 DB에서 읽어오기 요청.
	void MsgGameJoin(MSG_LIST* pMsg); 
	//! DB 스레드->에이젼트서버 : DB에서 읽어진 정보로 케릭터 접속 시도.
	void MsgAgentReqJoin(MSG_LIST* pMsg);
	//void MsgAgentReqJoin( DWORD dwClientID, GLCHARAG_DATA * pCHAR_DATA );
	//! 필드서버->에이전트서버 : 캐릭터가 게임에 조인했음
	int MsgLobbyCharJoinField(MSG_LIST* pMsg);

	void MsgFieldSvrOutFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgReBirthOutFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgReCallFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgPremiumSetFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );    
	void MsgAttackClientCheck ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );    
	void MsgTeleportFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword); 	

	//! Execute Command Text
	//! @param strCmd 컴멘드
	int	ExecuteCommand(char* strCmd);

	///////////////////////////////////////////////////////////////////////////
	/// Field Server Communication Member Function
	/// If you do not use Field Server, this functions are unnecessary
	///////////////////////////////////////////////////////////////////////////
	void FieldConnectAll();
	
	int FieldConnect(
			int nFieldNum = 0,
			int nChannel = 0 );

	/*
	int	FieldConnect(
			const char* szServerIP,
			int nPort,
			int nFieldNum = 0,
			int nChannel = 0 );
	*/

	int	FieldConnect(
			unsigned long ulServerIP,
			int nPort,
			int nFieldNum=0,
			int nChannel=0 );

	int	FieldCloseConnect(
			int nSvrNum );

	void FieldCloseConnectAll();

	// bool CheckFieldConnection();

	///////////////////////////////////////////////////////////////////////////
	/// 메시지 처리 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	//! Local Message process function ( gaeaserver로 직접 내부 메시지 전송. )
	int	InsertMsg(int nClient, void* pMsg);
	//! 모든 메시지는 일단 이곳을 거친다.
	int	MsgProcess(MSG_LIST* pMsg);
	//! 서버끼리의 통신일때는 이곳을 거친다.
	void ServerMsgProcess(MSG_LIST* pMsg);
	//! 서버끼리의 통신중 session 서버가 아닐때는 이곳을 거친다.
	void OtherMsgProcess(MSG_LIST* pMsg);	

	// 받은 메시지들을 처리한다.
	void RecvMsgProcess();
	// 하트비트 체크를 한다.
	void AgentSrvHeartBeatCheck( DWORD dwUdpateTime );
	// 서버 업데이트 
	void UpdateClientState();
	// 현재 서버의 상태를 프린트 한다.
	void PrintDebugMsg( DWORD dwUdpateTime );
	// UpdateThread 종료
	void EndUpdateThread();

	///////////////////////////////////////////////////////////////////////////
	/// 채널 관련 함수들
	///////////////////////////////////////////////////////////////////////////
	int  IncreaseChannelUser(int nChannel);
	int  DecreaseChannelUser(int nChannel);
	int  GetChannelUser     (int nChannel);
	bool IsChannelFull      (int nChannel);
	virtual void SendChannel(LPVOID pBuffer, int nServerChannel=0 ) { BroadcastToField(pBuffer, nServerChannel); }
	virtual void SendAllChannel( LPVOID pBuffer );

	///////////////////////////////////////////////////////////////////////////
	/// 로그인 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	//! 로그인 message ( 대만, 홍콩, 내부 ) 
	void MsgLogIn( MSG_LIST* pMsg );
	void MsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! China 로그인 메시지 ( 중국 ) 
	void ChinaMsgLogin( MSG_LIST* pMsg );
	void ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Daum 로그인 메시지 ( 한국 ) 
	void DaumMsgLogin(MSG_LIST* pMsg);
	void DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Daum 패스워드 체크 메시지
	void DaumMsgPassCheck(MSG_LIST* pMsg);
	void DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2);

	//! GSP 로그인 메시지
	void GspMsgLogin( MSG_LIST* pMsg );
	void GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra 로그인 메시지 ( 말레이시아, 필리핀, 베트남 )
	void TerraMsgLogin( MSG_LIST* pMsg );
	void TerraMsgLoginBack( TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra 패스워드 체크 메시지
	void TerraMsgPassCheck( MSG_LIST* pMsg );
	void TerraMsgPassCheckBack( TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! Excite Japan 로그인 메시지 ( 일본 )
	void ExciteMsgLogin(MSG_LIST* pMsg);
	void ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Excite Japna 패스워드 체크 메시지
	void ExciteMsgPassCheck( MSG_LIST* pMsg );
	void ExciteMsgPassCheckBack( EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! 일본 Gonzo Japan 로그인 메시지
	void JapanMsgLogin(MSG_LIST* pMsg);
	void JapanMsgLoginBack(JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	// 일본 Gonzo Japan 로그인 관련 함수
	void JapanWRFailed( DWORD dwClient, int nResult );
	void JapanWRSuccess( DWORD dwClient, CString strUserID,
									int nServerGroup, int nServerNum, CString strUUID, int nUserNum );

	//! 로그인 메시지 (Thailand) ( 태국 ) 
	//! \param pMsg 
	void ThaiMsgLogin(MSG_LIST* pMsg);
	void ThaiMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! Encoding 된 TID 에서 Decoding 된 TID 를 얻는다.
	//! @param szEncodedTID Encoding 된 TID
	//! @param szDecodedTID Decoding 된 TID
	//! @return	
	int TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID);

	//! 로그인 message ( 글로벌 서비스 ) 
	void GsMsgLogin( MSG_LIST* pMsg );
	void GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! 랜덤 암호를 위한 번호를 보낸다.
	void MsgSndRandomNumber(DWORD dwClient);

	//! Apex Data 메세지	// Apex 적용
	void MsgApexData( MSG_LIST* pMsg );
	void MsgApexReturn( MSG_LIST* pMsg );	// Apex 적용( 홍콩 )

	//! Apex 가 사용자를 블럭 시킬때 호출됨 Block
	void ApexUserBlock( DWORD dwClient, WORD dwHour );

	//! Apex 가 사용자 채팅을 금지 시킬때 호출됨 
	void ApexUserChatBlock( DWORD dwClient, WORD dwHour );

	//! Apex 가 사용자 로그를 남길때 호출됨
	void ApexUserHackLog( DWORD dwClient, TCHAR* szComment );

	//! Apex 가 경고메시지를 클라이언트에게 보낸다.
	void ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour );

	void ApexCloseClient( DWORD dwClient );

	void ApexCloseClientFinal();

	///////////////////////////////////////////////////////////////////////////
	/// nPortect 관련 함수들
	///////////////////////////////////////////////////////////////////////////	
	
	//! nProtect 인증용 메시지 전송
	void MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga );

	//! nProtect 클라이언트 인증 First
	void MsgSndGameGuardFirstCheck( DWORD dwClient );

	//! nProtect 클라이언트 인증 Second
	void MsgSndGameGuardSecondCheck( DWORD dwClient );

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswer( MSG_LIST* pMsg );

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswerFirst( MSG_LIST* pMsg );

	//! nProtect 인증용 메세지 결과
	void MsgGameGuardAnswerSecond( MSG_LIST* pMsg );

	//! 필드서버와 클라이언트에서 받은 추적 로그를 찍는다.
	void MsgTracingLogPrint( MSG_LIST* pMsg );


	// 추적 ID인지 아닌지 체크
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );


	///////////////////////////////////////////////////////////////////////////
	/// 게임내 이벤트용 함수들
	///////////////////////////////////////////////////////////////////////////	
	//! 여름방학 PC 방 프로모션 아이템 '펑펑' 이벤트
	void MsgLogPungPungCheckBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
    //! 란 선물펑펑!! 이벤트 
	void MsgLogLotteryBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);

	///////////////////////////////////////////////////////////////////////////
	/// GM 명령용 메시지 함수들
	///////////////////////////////////////////////////////////////////////////	
	void MsgGmPkMode ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgGmwherePcMap ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

	// 베트남 타임 리셋 임시 함수
	void MsgVietTimeReset ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID ); //vietnamtest%%% && vietnamtest2

    ///////////////////////////////////////////////////////////////////////////
	/// Session Server Communication Member Function	
	///////////////////////////////////////////////////////////////////////////	
	int	SessionConnect(
			unsigned long ulServerIP,
			int nPort);

	int SessionConnect(
			const char* szServerIP,
			int nPort );

	/**
	* Session 서버와 통신을 체크하고 이상이 있으면 다시 연결한다.
	*/
	int SessionReconnect();
	
	//! Session 서버에 접속하고 서버의 정보를 전송한다.
	int SessionConnectSndSvrInfo(const char* szServerIP, int nPort);
	int SessionConnectSndSvrInfo();

	int SessionCloseConnect();
	int	SendSession(LPVOID pBuffer);
	void SessionMsgProcess(MSG_LIST* pMsg);	

	void SessionMsgHeartbeatServerAns();
	
	//! 세션서버->게임서버 : 채팅메시지
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);

	//  로그인 키
	void SessionMsgEncryptKey( NET_MSG_GENERIC* nmg );
	
	//! Ping Answer	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg);
	//! Send ping
	// void SessionSndPing();
	//! Agent->Session:서버의 모든 정보를 보낸다.
	int SessionSndSvrInfo();
	//! Send Heartbeat
    void SessionSndHeartbeat(); 
	//! Agent->Session:서버의 현재 상태를 보낸다.
	int SessionSndSvrCurState();
	//! Agent->Session:서버의 현재 채널 상태를 보낸다.
	int SessionSndChannelState(int nChannel);	

	//! 서버 다운시 UserLoginState값이 제대로 초기화 되지 않는 문제가 있다.
	//! Agent가 처음 시작할 때 UserDB의 UserLoginState값을 0으로 초기화 시켜준다.
	void InitUserLoginState();

	int SessionSndCreateCharacter(int nUserNum);
	int	DaumSessionSndCreateCharacter(int nUserNum);

	int	SessionSndTestCreateCharacter(int nUserNum);
	int	DaumSessionSndTestCreateCharacter(int nUserNum);

	int	SessionSndDelCharacter     (int nUserNum);
	int	DaumSessionSndDelCharacter (int nUserNum);

	int SessionSndTestDelCharacter (int nUserNum);
	int DaumSessionSndTestDelCharacter(int nUserNum);

	BOOL UpdateTracingCharacter(NET_MSG_GENERIC* nmg );

protected:
	void BroadcastToField(LPVOID pBuffer, int nServerChannel=0);
	int	BroadcastToField(int nSvrNum, LPVOID pBuffer, int nServerChannel=0);
	int	BroadcastToField(DWORD dwClient, LPVOID pBuffer);
};

extern CAgentServer* g_pAgentServer;

#endif // S_CAgentServer_H_
