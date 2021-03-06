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
	CClientAgent* m_pClientManager; ///< Client ??????
	int m_ChannelUser[MAX_CHANNEL_NUMBER]; ///< ?????? ?????? ???? ????
	
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

	// INFO:Reached Max Client Number ???????? ???? UpdateProc?? ?????? ???????????? ???????? ???? ????
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
					assert(0&&"?????????????? SendAgent()?? ???????? ????????."); 
					return 0; 
				}
	virtual int SendAgent ( LPVOID pBuffer )		                 
				{ 
					assert(0&&"?????????????? SendAgent()?? ???????? ????????."); 
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
	
	void	MsgSndChaDelInfo( MSG_LIST* pMsg ); // Agent->Client:?????? ???? ???? ????	
	void	MsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // ????????->?????????? : ?????? ???? ???? ????	

	//! Agent->Client:?????? ????
	void DaumMsgSndChaDelInfo(MSG_LIST* pMsg);

	//! Agent->Client:?????? ???? ???? ????
	void DaumMsgSndChaDelInfoBack(int nChaNum, 
		                          int nUserNum, 
		                          int nResult, 
								  DWORD dwClient, 
								  const char* szUserIP, 
								  USHORT uPort=0);

	/**
	* Client->Agent:?????? ????????
	* \param pMsg 
	*/
	void TerraMsgSndChaDelInfo( MSG_LIST* pMsg );

	/**
	* Client->Agent:?????? ????????
	* \param pMsg 
	*/
	void GspMsgSndChaDelInfo( MSG_LIST* pMsg );

	/**
	* Client->Agent:?????? ????????
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
	* Client->Agent:?????? ????????
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
	* Agent->Client:?????? ????????????
	* \param nChaNum ??????????
	* \param nUserNum ????????
	* \param nResult ????
	* \param dwClient ??????????????
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
	* Agent->Client:?????? ????????????
	* \param nChaNum ??????????
	* \param nUserNum ????????
	* \param nResult ????
	* \param dwClient ??????????????
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
	* Client->Agent:?????? ????????
	* \param pMsg 
	*/
	void	GsMsgSndChaDelInfo( MSG_LIST* pMsg ); // Agent->Client:?????? ???? ???? ????	

	/**
	* Agent->Client:?????? ????????????
	* \param nChaNum ??????????
	* \param nUserNum ????????
	* \param nResult ????
	* \param dwClient ??????????????
	* \param szUserIP IP
	* \param uPort PORT
	*/
	void	GsMsgSndChaDelInfoBack( int nChaNum, 
		                          int nUserNum, 
								  int nResult, 
								  DWORD dwClient, 
								  const TCHAR* szUserIP, 
								  USHORT uPort=0 ); // ????????->?????????? : ?????? ???? ???? ????	
	
	//! ??????????->???????????? : ?????? ?????? DB???? ???????? ????.
	void MsgGameJoin(MSG_LIST* pMsg); 
	//! DB ??????->???????????? : DB???? ?????? ?????? ?????? ???? ????.
	void MsgAgentReqJoin(MSG_LIST* pMsg);
	//void MsgAgentReqJoin( DWORD dwClientID, GLCHARAG_DATA * pCHAR_DATA );
	//! ????????->???????????? : ???????? ?????? ????????
	int MsgLobbyCharJoinField(MSG_LIST* pMsg);

	void MsgFieldSvrOutFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgReBirthOutFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgReCallFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgPremiumSetFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );    
	void MsgAttackClientCheck ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );    
	void MsgTeleportFb ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	
	void SetUserInfo(DWORD nClient, const char* strUserId, const char* strPassword); 	

	//! Execute Command Text
	//! @param strCmd ??????
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
	/// ?????? ???? ???? ??????
	///////////////////////////////////////////////////////////////////////////	
	//! Local Message process function ( gaeaserver?? ???? ???? ?????? ????. )
	int	InsertMsg(int nClient, void* pMsg);
	//! ???? ???????? ???? ?????? ??????.
	int	MsgProcess(MSG_LIST* pMsg);
	//! ?????????? ?????????? ?????? ??????.
	void ServerMsgProcess(MSG_LIST* pMsg);
	//! ?????????? ?????? session ?????? ???????? ?????? ??????.
	void OtherMsgProcess(MSG_LIST* pMsg);	

	// ???? ?????????? ????????.
	void RecvMsgProcess();
	// ???????? ?????? ????.
	void AgentSrvHeartBeatCheck( DWORD dwUdpateTime );
	// ???? ???????? 
	void UpdateClientState();
	// ???? ?????? ?????? ?????? ????.
	void PrintDebugMsg( DWORD dwUdpateTime );
	// UpdateThread ????
	void EndUpdateThread();

	///////////////////////////////////////////////////////////////////////////
	/// ???? ???? ??????
	///////////////////////////////////////////////////////////////////////////
	int  IncreaseChannelUser(int nChannel);
	int  DecreaseChannelUser(int nChannel);
	int  GetChannelUser     (int nChannel);
	bool IsChannelFull      (int nChannel);
	virtual void SendChannel(LPVOID pBuffer, int nServerChannel=0 ) { BroadcastToField(pBuffer, nServerChannel); }
	virtual void SendAllChannel( LPVOID pBuffer );

	///////////////////////////////////////////////////////////////////////////
	/// ?????? ???? ??????
	///////////////////////////////////////////////////////////////////////////	
	//! ?????? message ( ????, ????, ???? ) 
	void MsgLogIn( MSG_LIST* pMsg );
	void MsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! China ?????? ?????? ( ???? ) 
	void ChinaMsgLogin( MSG_LIST* pMsg );
	void ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Daum ?????? ?????? ( ???? ) 
	void DaumMsgLogin(MSG_LIST* pMsg);
	void DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Daum ???????? ???? ??????
	void DaumMsgPassCheck(MSG_LIST* pMsg);
	void DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2);

	//! GSP ?????? ??????
	void GspMsgLogin( MSG_LIST* pMsg );
	void GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra ?????? ?????? ( ??????????, ??????, ?????? )
	void TerraMsgLogin( MSG_LIST* pMsg );
	void TerraMsgLoginBack( TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! Terra ???????? ???? ??????
	void TerraMsgPassCheck( MSG_LIST* pMsg );
	void TerraMsgPassCheckBack( TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! Excite Japan ?????? ?????? ( ???? )
	void ExciteMsgLogin(MSG_LIST* pMsg);
	void ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	//! Excite Japna ???????? ???? ??????
	void ExciteMsgPassCheck( MSG_LIST* pMsg );
	void ExciteMsgPassCheckBack( EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2 );

	//! ???? Gonzo Japan ?????? ??????
	void JapanMsgLogin(MSG_LIST* pMsg);
	void JapanMsgLoginBack(JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2);

	// ???? Gonzo Japan ?????? ???? ????
	void JapanWRFailed( DWORD dwClient, int nResult );
	void JapanWRSuccess( DWORD dwClient, CString strUserID,
									int nServerGroup, int nServerNum, CString strUUID, int nUserNum );

	//! ?????? ?????? (Thailand) ( ???? ) 
	//! \param pMsg 
	void ThaiMsgLogin(MSG_LIST* pMsg);
	void ThaiMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2);	

    //! Encoding ?? TID ???? Decoding ?? TID ?? ??????.
	//! @param szEncodedTID Encoding ?? TID
	//! @param szDecodedTID Decoding ?? TID
	//! @return	
	int TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID);

	//! ?????? message ( ?????? ?????? ) 
	void GsMsgLogin( MSG_LIST* pMsg );
	void GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 );

	//! ???? ?????? ???? ?????? ??????.
	void MsgSndRandomNumber(DWORD dwClient);

	//! Apex Data ??????	// Apex ????
	void MsgApexData( MSG_LIST* pMsg );
	void MsgApexReturn( MSG_LIST* pMsg );	// Apex ????( ???? )

	//! Apex ?? ???????? ???? ?????? ?????? Block
	void ApexUserBlock( DWORD dwClient, WORD dwHour );

	//! Apex ?? ?????? ?????? ???? ?????? ?????? 
	void ApexUserChatBlock( DWORD dwClient, WORD dwHour );

	//! Apex ?? ?????? ?????? ?????? ??????
	void ApexUserHackLog( DWORD dwClient, TCHAR* szComment );

	//! Apex ?? ???????????? ?????????????? ??????.
	void ApexSendMessageToClient( DWORD dwClient, WORD wAction, WORD wHour );

	void ApexCloseClient( DWORD dwClient );

	void ApexCloseClientFinal();

	///////////////////////////////////////////////////////////////////////////
	/// nPortect ???? ??????
	///////////////////////////////////////////////////////////////////////////	
	
	//! nProtect ?????? ?????? ????
	void MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga );

	//! nProtect ?????????? ???? First
	void MsgSndGameGuardFirstCheck( DWORD dwClient );

	//! nProtect ?????????? ???? Second
	void MsgSndGameGuardSecondCheck( DWORD dwClient );

	//! nProtect ?????? ?????? ????
	void MsgGameGuardAnswer( MSG_LIST* pMsg );

	//! nProtect ?????? ?????? ????
	void MsgGameGuardAnswerFirst( MSG_LIST* pMsg );

	//! nProtect ?????? ?????? ????
	void MsgGameGuardAnswerSecond( MSG_LIST* pMsg );

	//! ?????????? ?????????????? ???? ???? ?????? ??????.
	void MsgTracingLogPrint( MSG_LIST* pMsg );


	// ???? ID???? ?????? ????
	int CheckTracingUser( const char *pUserAccount, const int nUserNum );


	///////////////////////////////////////////////////////////////////////////
	/// ?????? ???????? ??????
	///////////////////////////////////////////////////////////////////////////	
	//! ???????? PC ?? ???????? ?????? '????' ??????
	void MsgLogPungPungCheckBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
    //! ?? ????????!! ?????? 
	void MsgLogLotteryBack(int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort=0);

	///////////////////////////////////////////////////////////////////////////
	/// GM ?????? ?????? ??????
	///////////////////////////////////////////////////////////////////////////	
	void MsgGmPkMode ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );
	void MsgGmwherePcMap ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID );

	// ?????? ???? ???? ???? ????
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
	* Session ?????? ?????? ???????? ?????? ?????? ???? ????????.
	*/
	int SessionReconnect();
	
	//! Session ?????? ???????? ?????? ?????? ????????.
	int SessionConnectSndSvrInfo(const char* szServerIP, int nPort);
	int SessionConnectSndSvrInfo();

	int SessionCloseConnect();
	int	SendSession(LPVOID pBuffer);
	void SessionMsgProcess(MSG_LIST* pMsg);	

	void SessionMsgHeartbeatServerAns();
	
	//! ????????->???????? : ??????????
	void SessionMsgChatProcess(NET_MSG_GENERIC* nmg);
	void SessionMsgSvrCmd(NET_MSG_GENERIC* nmg);

	//  ?????? ??
	void SessionMsgEncryptKey( NET_MSG_GENERIC* nmg );
	
	//! Ping Answer	
	void SessionMsgPingAnswer(NET_MSG_GENERIC* nmg);
	//! Send ping
	// void SessionSndPing();
	//! Agent->Session:?????? ???? ?????? ??????.
	int SessionSndSvrInfo();
	//! Send Heartbeat
    void SessionSndHeartbeat(); 
	//! Agent->Session:?????? ???? ?????? ??????.
	int SessionSndSvrCurState();
	//! Agent->Session:?????? ???? ???? ?????? ??????.
	int SessionSndChannelState(int nChannel);	

	//! ???? ?????? UserLoginState???? ?????? ?????? ???? ???? ?????? ????.
	//! Agent?? ???? ?????? ?? UserDB?? UserLoginState???? 0???? ?????? ????????.
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
