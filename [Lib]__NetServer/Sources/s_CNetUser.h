#ifndef S_NETUSER_H_
#define S_NETUSER_H_

#include "s_NetGlobal.h"
#include "s_CBit.h"
#include "s_CLock.h"
#include "s_CAddressChecker.h"
#include "SendMsgBuffer.h"
#include "RcvMsgBuffer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
 *       2005-12-12 Jgkim, Add nProtect user query module
 *
 * \author jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 *
 */
class CNetUser : public CLock
{
public:
	CNetUser();
	~CNetUser();

protected:
	USER_INFO_BASIC		m_uib; /// Basic user information

	bool				m_bACCOUNT_PASS;    // 계정검사를 통과했는지 여부
	bool                m_bACCOUNT_PASSING; // 계정검사 통과중인지 검사.

	SOCKET				m_Socket;
	EM_NET_STATE        m_emOnline;
	//int				m_nOnline;	
	TCHAR				m_szIp[MAX_IP_LENGTH+1];
	USHORT				m_usPort;
	DWORD				m_dwLastMsgTime;	// 마지막으로 읽기 통신이 된 시간
	DWORD				m_dwLastSaveTime;   // 마지막으로 캐릭터를 저장한 시간
	int					m_nPing;			// 클라이언트와 통신에 걸린 시간
	int					m_nNetMode;			// Packet Head 를 받을것인가 Body 를 받을것인가 결정	
	CTime				m_LoginTime;		// 로그인을 성공한 시각
	
	DWORD				m_dwGaeaID;			// 게임참여시 '가이아'에서의 고유ID.
	int					m_nChaNum;          // 현재 게임에 참여중인 캐릭터 번호
	int					m_nUserType;        // 유저의 타입 (일반유저, GM, Master)
    
	__time64_t          m_tPremiumDate;     // 프리미엄서비스 만료시간.
	__time64_t          m_tChatBlockDate;   // 채팅블록 만료시간.

	DWORD				m_dwSlotAgentClient;// Agent 서버에서의 Client 와의 통신 슬롯
	DWORD				m_dwSlotAgentField;	// Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD				m_dwSlotFieldAgent;	// Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD				m_dwSlotFieldClient;// Field 서버에서의 Client 와의 통신 슬롯
	DWORD				m_dwSlotType;

	CRYPT_KEY			m_ck; /// 기본암호화 암호키 (현재 사용되지 않음)
	bool				m_bHeartBeat; /// 하트비트 체크여부
	WORD				m_wChaRemain;
	WORD				m_wChaTestRemain;

	int					m_nChannel;            // 사용자가 참여한 채널
	WORD                m_wInvalidPacketCount; // 잘못된 패킷을 보낸 횟수
	WORD                m_wLoginAttempt;       // 한번 접속에 로그인 요청 횟수

	WORD				m_wConSendError;	// 연속된 전송 에러 횟수, 체크 후 클라이언트 강제 종료를 위해 사용.
	
	SERVER_UTIL::CBit	m_Bit;
	
	CSendMsgBuffer*  m_pSendManager; ///< 메시지 보내기 메니저
	CRcvMsgBuffer*   m_pRcvManager; ///< 메시지 받기 메니저

	int	m_nRandomPass; ///< 랜덤패스워드 체크를 위한 변수

	// 극강부 체크
	int					m_nExtremeM;
	int					m_nExtremeW;
	int					m_nExtremeCheckM;
	int					m_nExtremeCheckW;

	int					m_nExtremeDBM;
	int					m_nExtremeDBW;

	// 태국 User Class Type
	int					m_nThaiCC_Class;

	// 말레이시아 PC방 이벤트
	// 말레이시아 User Class Type
	int					m_nMyCC_Class;

	// 중국 로그인 시간 체크
	int					m_nChinaGameTime;
	int					m_nChinaOfflineTime;
	__time64_t			m_tLastLoginTime;
	int					m_nChinaUserAge;

	// 베트남 탐닉방지 관련 추가
	int					m_nVTGameTime;

	// 저장된 쓰레기값
	char		m_szRecvGarbageMsg[2][12];
	BYTE		m_dwGarbageNum;

	//DWORD m_dwSendIoCount; //! 보내기 요청횟수
	//DWORD m_dwRecvIoCount; //! 받기 요청 횟수

	//////////////////////////////////////////////////////////////////////////////
	// Thailand
	WORD m_wThaiFlag; /// Thailand 사용자 플래그
	WORD m_wThaiTodayGamePlayTime; /// Thailand 사용자의 게임플레이시간(분)	

	//////////////////////////////////////////////////////////////////////////////
	// Malaysia
	TCHAR m_szDecodedTID[TERRA_TID_DECODE+1]; /// Malaysia User information	

	// GSP
	TCHAR m_szUUID[UUID_STR_LENGTH]; /// UUID User length	

	// Encrypt
	TCHAR m_szEncryptKeyHeart[ENCRYPT_KEY+1];

public:
	////! 전송 실패 카운터
	//void initSendError()		{ m_wConSendError = 0; }
	//void incSendError()			{ ++m_wConSendError; }
	//WORD getSendError()			{ return m_wConSendError; }

	////! 받기 요청 횟수 카운터
	//int incRecvCount();
	//int decRecvCount();
	//int getRecvCount();

	////! 보내기 요청 횟수 카운터
	//int incSendCount();
	//int decSendCount();
	//int getSendCount();

	//! 보내야할 메시지를 등록한다.
	int addSendMsg(void* pMsg, DWORD dwSize);

	//! 보내야할 메시지 버퍼를 가져온다.
	void* getSendBuffer();

	//! 보내야할 메시지 크기를 가져온다.
	int getSendSize();

	//! 클라이언트에서 받은 메시지를 넣는다.
	int addRcvMsg(void* pMsg, DWORD dwSize);

	//! 클라이언트에서 받은 메시지를 하나 가져온다.
	void* getRcvMsg( bool bClient );

	//! 클라이언트 상태 리셋
	void Reset();

	//! 클라이언트 버퍼 리셋
	void ResetRcvBuffer();

	//! 클라이언트 종료시킴
	int CloseClient();

	//! 클라이언트에서 받는 메시지만 쓰레기 값을 체크한다.
	void CheckGarbageMsg( void *msg );

	inline bool IsOnline()   { return m_emOnline == NET_ONLINE ? true : false; }
	inline bool IsOffline()  { return m_emOnline == NET_OFFLINE ? true : false; }
	inline void SetOnLine()  { m_emOnline = NET_ONLINE; }
	inline void SetOffLine() { m_emOnline = NET_OFFLINE; }

	void	CloseSocket();
	
	int     InsertBuffer( BYTE* buff, DWORD dwSize );

	int		SetAcceptedClient(SOCKET s);
	void	SetSocket(SOCKET s);
	void	SetIP(TCHAR* szIP);
	void	SetLoginTime();	

	void	SetPort(USHORT usPort);
	void	SetNetMode(int nMode);

	void	SetUserNum(int nUserNum);
	
	void    GspSetUUID( const TCHAR* szUUID );
	TCHAR*  GspGetUUID();

	void	KorSetUUID( const TCHAR* szUUID );
	TCHAR*	KorGetUUID();

	void	JPSetUUID( const TCHAR* szUUID );
	TCHAR*	JPGetUUID();

	void    TerraSetDecodedTID( const TCHAR* szDecodedTID );
	TCHAR*  TerraGetDecodedTID();

	void    SetEncKey( const TCHAR* szEncKey );
	TCHAR*  GetEncKey();	
	
	void	SetCryptKey(CRYPT_KEY ck);
	// void SetLastMsgTime();
	// void SetLastMsgTime(DWORD dwTime);
	// void SetLastSaveTime();
	// void SetLastSaveTime(DWORD dwTime);

	void	SetGaeaID         (DWORD dwGaeaID);
	void	SetChaNum		  (int	 nChaNum);
	void	SetUserType       (int   nType);
	void    SetChaRemain	  (WORD wNum);
	void	SetChaTestRemain  (WORD wNum);
	WORD    GetChaRemain      ();
	WORD    GetChaTestRemain  ();
	void	SetChannel        (int nChannel);
	int     GetChannel        ();

	void	SetSlotAgentClient(DWORD dwSlot); // Agent 서버에서의 Client 와의 통신 슬롯
	void	SetSlotAgentField (DWORD dwSlot); // Agent 서버에서의 Field 서버와의 통신 슬롯
	void	SetSlotFieldAgent (DWORD dwSlot); // Field 서버에서의 Agent 서버와의 통신 슬롯
	void	SetSlotFieldClient(DWORD dwSlot); // Field 서버에서의 Client 와의 통신 슬롯	
	void	SetSlotType		  (DWORD dwType);	

	void	ResetUserID();
	TCHAR*	GetUserID();	
	void	SetUserID( const TCHAR* szUserID );

	TCHAR*  GspGetUserID();
	void    GspSetUserID( const TCHAR* szGspUserID );


	SOCKET	GetSOCKET();
	TCHAR*	GetIP();	
	USHORT	GetPort();	
	int		GetNetMode();

	int		GetUserNum();		
	
	DWORD	GetGaeaID();
	int		GetChaNum();
	int		GetUserType();
	DWORD	GetSlotAgentClient(); // Agent 서버에서의 Client 와의 통신 슬롯
	DWORD 	GetSlotAgentField (); // Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD	GetSlotFieldAgent (); // Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD	GetSlotFieldClient(); // Field 서버에서의 Client 와의 통신 슬롯	
	DWORD	GetSlotType();
	
	inline bool IsClientSlot() { return m_dwSlotType == NET_SLOT_CLIENT ? true : false; }
	inline bool IsFieldSlot()  { return m_dwSlotType == NET_SLOT_FIELD  ? true : false; }
	inline bool IsAgentSlot()  { return m_dwSlotType == NET_SLOT_AGENT  ? true : false; }

	bool	IsAccountPass();
	void	SetAccountPass(bool bPASS);

	bool    IsAccountPassing(); // 계정검사 통과중인지 검사
	void    SetAccountPassing(bool bPassing); // 계정검사 통과중인지 세팅

	CTime	GetLoginTime();	
	// DWORD GetLastMsgTime();
	// DWORD GetLastSaveTime();

	inline void ResetHeartBeat() { m_bHeartBeat = false; }
	inline void SetHeartBeat()   { m_bHeartBeat = true; }
	inline bool CheckHeartBeat() { 	return m_bHeartBeat == true ? true : false; }

	CRYPT_KEY GetCryptKey();

    __time64_t GetPremiumDate();
    void       SetPremiumDate(__time64_t tTime);

	__time64_t GetChatBlockDate();
    void       SetChatBlockDate(__time64_t tTime);

	//////////////////////////////////////////////////////////////////////////////
	// 극강부 체크 
	//////////////////////////////////////////////////////////////////////////////
	void	SetExtremeCreateM( int nExtremeM );
	void	SetExtremeCreateW( int nExtremeW );
	void	SetExtremeCheckM( int nExtremeCheckM );
	void	SetExtremeCheckW( int nExtremeCheckW );

	int		GetExtremeM();
	int		GetExtremeW();
	int		GetExtremeCheckM();
	int		GetExtremeCheckW();

	// DB에서 연산된 극강부 체크 값을 저장해 둔다.
	void	SetExtremeCreateDBM( int nExtremeDBM );
	void	SetExtremeCreateDBW( int nExtremeDBW );
	void	SetExtremeCheckDBM( int nExtremeCheckDBM );
	void	SetExtremeCheckDBW( int nExtremeCheckDBW );

	int		GetExtremeDBM();
	int		GetExtremeDBW();
	int		GetExtremeCheckDBM();
	int		GetExtremeCheckDBW();

	// 태국 User Class Type을 설정한다.
	void SetThaiClass( int nThaiCC_Class );

	// 태국 User Class Type을 가져온다.
	int GetThaiClass();

	// 말레이시아 PC방 이벤트
	// 말레이시아 User Class Type을 설정한다.
	void SetMyClass( int nMyCC_Class );

	// 말레이시아 User Class Type을 가져온다.
	int GetMyClass();

	//////////////////////////////////////////////////////////////////////////////
	// 중국 GameTime, OfflineTime 체크
	//////////////////////////////////////////////////////////////////////////////
	void SetChinaGameTime( int nChinaGameTime );
	void SetChinaOfflineTime( int nChinaOfflineTime );
	void SetLastLoginDate( __time64_t tTime );
	void SetChinaUserAge( int nChinaUserAge );

	int GetChinaGameTime();
	int GetChinaOfflineTime();
	__time64_t GetLastLoginDate();
	int GetChinaUserAge();

	//////////////////////////////////////////////////////////////////////////////
	// 베트남 GameTime 체크
	//////////////////////////////////////////////////////////////////////////////
	void SetVTGameTime( int nVTGameTime );
	int GetVTGameTime();


	//////////////////////////////////////////////////////////////////////////////
	// 클라이언트 필드간 이동시 정보 저장
	//////////////////////////////////////////////////////////////////////////////
	//DWORD	m_dwClientGaeaID;
	//NET_MSG_GENERIC	m_nmg;

 //   // 연결할 퍌드 서버의 정보를 설정한다
	//void	SetConnectionFieldInfo( DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	//
	//// 연결할 필드 서버의 정보를 사용했으면 초기화한다.
	//void	ResetConnectionFieldInfo();

	//// 연결할 필드서버로 갈 클라이언트의 GaeaID를 가져온다.
	//DWORD	GetClientGaeaID()			{ return m_dwClientGaeaID; }

	//NET_MSG_GENERIC * GetNetMsg()		{ return &m_nmg; }

	/*
	void       NetInCountIncrease();
	void       NetOutCountIncrease();
	void       NetInCountDecrease();
	void       NetOutCountDecrease();
	int        NetGetInCount();
	int        NetGetOutCount();
	*/
	
	//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
	//! 사용자의 접속을 끊음...
	//! 로그인 요청 횟수 증가시킴
	void IncLoginAttempt();

	//! 로그인 요청 횟수 가져옴
	WORD GetLoginAttempt();	

	void SetRandomPassNumber(int nNumber);
	int GetRandomPassNumber();

	//////////////////////////////////////////////////////////////////////////////
	// Thailand
	//////////////////////////////////////////////////////////////////////////////
	WORD ThaiGetFlag ();
	void ThaiSetFlag (WORD wFlag);
	void ThaiSetTodayPlayTime (WORD wMinutes);
	bool ThaiIs3HourOver (CTime& currentTime);

	//////////////////////////////////////////////////////////////////////////////
	// nProtect
	//////////////////////////////////////////////////////////////////////////////
protected:
	CCSAuth2            m_csa; ///< nProtect GameGuard 인증용 클래스
	bool                m_bNProtectAuth;
	bool                m_bNProtectFirstCheck; ///< 접속후 nProtect 를 처음 인증받는가?	
	bool                m_bNProtectFirstAuthPass; ///< 게임에 진입하기 전에 모든 인증을 완료했는가?
	
public:
	//! nProtect 처음 인증인가?
	bool nProtectIsFirstCheck();

	//! nProtect 처음 인증여부 세팅
	void nProtectSetFirstCheck(bool bState);

	//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
	bool nProtectSetAuthQuery();
    	
	//! nProtect GameGuard 인증용 쿼리 가져오기
	GG_AUTH_DATA nProtectGetAuthQuery();

	//! nProtect GameGuard 인증 응답 가져오기
	GG_AUTH_DATA nProtectGetAuthAnswer();

	//! nProtect GameGuard 인증 응답 설정
	void nProtectSetAuthAnswer(GG_AUTH_DATA& ggad);

	//! nProtect GameGuard 인증 결과 가져오기
	DWORD nProtectCheckAuthAnswer();

	//! nProtect GameGuard 인증 상태를 리셋시킨다.
	void nProtectResetAuth();

	//! nProtect GameGuard 처음 인증을 통과했는가?
	//! 이 인증을 통과해야 게임플레이를 할 수 있다.
	bool nProtectFirstAuthPass();

	//! nProtect GameGuard 처음인증 통과상태 세팅
	void nProtectSetFirstAuthPass(bool bState);
};

#endif // S_NETUSER_H_