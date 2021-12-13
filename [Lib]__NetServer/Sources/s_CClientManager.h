#ifndef S_CCLIENTMANAGER_H_
#define S_CCLIENTMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <deque>
#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_CNetUser.h"
#include "s_COverlapped.h"
#include "s_CBit.h"
#include "s_CCfg.h"

/**
 * \ingroup NetServerLib
 *
 * \par requirements
 * win2k or later\n
 * MFC\n
 *
 * \version 1.1
 *
 * \date 2002-05-30 
 *
 * \author Jgkim
 *
 * \par license
 * Copyright(c) Mincoms. All rights reserved.
 *
 * \todo 
 *
 * \bug 
 *
 */
class CClientManager : public CLock
{
	typedef std::vector<int>			VEC_CLIENTS;
	typedef VEC_CLIENTS::iterator		VEC_CLIENTS_ITER;

	typedef std::map<int,BOOL>			MAP_CLIENTS;
	typedef std::deque<int>				DEQ_CLIENTS;

	typedef std::vector<CString>			VEC_CONNECTIP;

public:
	CClientManager(
		int nMaxClient,
		COverlapped* pSendIOCP,
		COverlapped* pRecvIOCP,
		HANDLE hIOServer );
	virtual ~CClientManager();

	DEQ_CLIENTS m_deqSleepCID;	//< 유휴 클라이언트 ID.
	MAP_CLIENTS	m_mapSleepCID;  //< 유휴 클라이언트 ID. ( 검색을 위해..)
	VEC_CLIENTS m_vecSleepCID;	//< 반환되는 클라이언트 ID.
	VEC_CONNECTIP m_vecConnectIP; // 접속하는 IP 리스트

	
protected:	
	int m_nMaxClient; //< 접속가능한 최대 클라이언트수	

	CNetUser* m_pClient; //< 사용자 정보를 담아둘 포인터

	HANDLE m_hIOServer;
	COverlapped* m_pSendIOCP; //< 보내기 IOCP 메모리 관리자
	COverlapped* m_pRecvIOCP; //< 받기 IOCP 메모리 관리자

	SERVER_UTIL::CBit m_Bit; //< 기초적인 메모리 Bit 연산을 위한 멤버

	DWORD m_dwInPacketCount; //< 수신된 총 패킷 갯수
	DWORD m_dwInPacketSize; //< 수신된 총 패킷 사이즈
	
	DWORD m_dwOutPacketCount; //< 송신한 총 패킷 갯수
	DWORD m_dwOutPacketSize; //< 송신한 총 패킷 사이즈

	DWORD m_dwOutCompressCount; //< 송신한 총 압축 패킷 갯수
	DWORD m_dwOutCompressSize; //< 송신한 총 압축 패킷 크기

//public:
//	//! -----------------------------------------------------------------------
//	//! Virtual Function
//	//! 받기 요청 횟수 카운터
//	virtual int incRecvCount( DWORD dwClient );
//	virtual int decRecvCount( DWORD dwClient );
//	virtual int getRecvCount( DWORD dwClient );
//
//	//! 보내기 요청 횟수 카운터
//	virtual int incSendCount( DWORD dwClient );
//	virtual int decSendCount( DWORD dwClient );
//	virtual int getSendCount( DWORD dwClient );
//
//	virtual PER_IO_OPERATION_DATA* getSendIO( DWORD dwClient );
//	virtual PER_IO_OPERATION_DATA* getRecvIO( DWORD dwClient );
//
//	virtual void releaseIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
//	virtual void releaseSendIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
//	virtual void releaseRecvIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
//
//	DWORD getSendUseCount()
//	{
//		return m_pSendIOCP->GetUseCount();
//	}
//
//	DWORD getRecvUseCount()
//	{
//		return m_pRecvIOCP->GetUseCount();
//	}

public:
	//! 수신된 총 패킷 갯수
	DWORD getInPacketCount() { return m_dwInPacketCount; };
	//! 수신된 총 패킷 사이즈
	DWORD getInPacketSize() { return m_dwInPacketSize; };

	//! 송신한 총 패킷 갯수
	DWORD getOutPacketCount() { return m_dwOutPacketCount; };
	//! 송신한 총 패킷 사이즈
	DWORD getOutPacketSize() { return m_dwOutPacketSize; };

	//! 송신한 총 압축 패킷 갯수
	DWORD getOutCompressCount() { return m_dwOutCompressCount; };
	//! 송신한 총 압축 패킷 크기
	DWORD getOutCompressSize() { return m_dwOutCompressSize; };

	//! 모든 패킷 카운터를 초기화 한다.
	void resetPacketCount();    

	void	Reset				(DWORD dwClient);
	int		CloseClient			(DWORD dwClient, bool bOnlineCheck = TRUE);	//! 특정클라이언트의 연결을 종료한다.
	void	CloseAllClient();						//! 모든 클라이언트의 연결을 종료한다.
	
	void	ResetPreSleepCID();
	int		GetFreeClientID		(int nType=0);
	int     ReleaseClientID     (DWORD dwClient);
	void    ConnectIPCheck		(char *pszIP);

	int 	SetAcceptedClient	(DWORD dwClient, SOCKET sSocket);
	void	SetNetMode			(DWORD dwClient, int nMode);
	int		GetNetMode			(DWORD dwClient);	
	char*	GetClientIP			(DWORD dwClient);
	USHORT  GetClientPort       (DWORD dwClient);
	SOCKET	GetSocket			(DWORD dwClient);
	void	CloseSocket			(DWORD dwClient);

	int		GetMaxClient		(void); // Return max client number	
	int		GetCurrentClientNumber(void); // 현재 접속중인 클라이언트 숫자	

	int	addRcvMsg(DWORD dwClient, 
	              void* pMsg, 
				  DWORD dwSize);

	void* getRcvMsg(DWORD dwClient,bool bClient);

	void ResetRcvBuffer(DWORD dwClient);	

	void    SendClientFinal();
	int		SendClient (DWORD dwClient, LPVOID pBuffer);
	int		SendClient2(DWORD dwClient, LPVOID pBuffer);
	int		SendClient2(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);

	bool	IsAccountPass		(DWORD dwClient); // 계정 인증(login) 통과하였는지 점검.
	void	SetAccountPass		(DWORD dwClient, bool bPASS); // 계정 인증(login) 통과 설정.

	CRYPT_KEY GetCryptKey		(DWORD dwClient);
	CRYPT_KEY GetNewCryptKey	(void);
	void	  SetCryptKey		(DWORD dwClient, CRYPT_KEY ck);
	
	// Slot type set and check
	DWORD	GetSlotAgentClient	(DWORD dwClient); // Agent 서버에서의 Client 와의 통신 슬롯
	DWORD 	GetSlotAgentField	(DWORD dwClient); // Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD	GetSlotFieldAgent	(DWORD dwClient); // Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD	GetSlotFieldClient	(DWORD dwClient); // Field 서버에서의 Client 와의 통신 슬롯
	DWORD	GetSlotType			(DWORD dwClient);

	void	SetSlotAgentClient	(DWORD dwClient, DWORD dwSlot); // Agent 서버에서의 Client 와의 통신 슬롯
	void	SetSlotAgentField	(DWORD dwClient, DWORD dwSlot); // Agent 서버에서의 Field 서버와의 통신 슬롯
	void	SetSlotFieldAgent	(DWORD dwClient, DWORD dwSlot); // Field 서버에서의 Agent 서버와의 통신 슬롯
	void	SetSlotFieldClient	(DWORD dwClient, DWORD dwSlot); // Field 서버에서의 Client 와의 통신 슬롯
	void	SetSlotType			(DWORD dwClient, DWORD dwType);	

	void	ResetUserID			(DWORD dwClient);

	void	ResetHeartBeat		(DWORD dwClient);
	void	SetHeartBeat		(DWORD dwClient);
	bool	CheckHeartBeat		(DWORD dwClient);

	bool	IsClientSlot		(DWORD dwClient);
	bool	IsAgentSlot			(DWORD dwClient);
	bool	IsFieldSlot			(DWORD dwClient);
	bool	IsOnline			(DWORD dwClient);
	bool	IsOffline			(DWORD dwClient);

	void	GspSetUserID( DWORD dwClient, const TCHAR* szGspUserID );
	TCHAR*	GspGetUserID( DWORD dwClient );

	void	SetUserID( DWORD dwClient, const TCHAR* szUserID );
	TCHAR*	GetUserID( DWORD dwClient );

	void	SetUserNum          (DWORD dwClient, INT nUserNum);
	void	SetLoginTime        (DWORD dwClient);
	void	SetGaeaID           (DWORD dwClient, DWORD dwGaeaID);
	// void SetLastMsgTime      (DWORD dwClient, DWORD dwTime);
	// void SetLastSaveTime     (DWORD dwClient, DWORD dwTime);
	void	SetChaNum           (DWORD dwClient, int nChaNum);
	void    SetUserType         (DWORD dwClient, int nType=0);

	
	INT		GetUserNum		    (DWORD dwClient);
	DWORD	GetUserCharObjID    (DWORD dwClient);
	INT		GetLoginTime	    (DWORD dwClient);
	CTime	GetLoginTimeEx		(DWORD dwClient);
	DWORD	GetGaeaID		    (DWORD dwClient);
	// DWORD GetLastMsgTime	    (DWORD dwClient);
	// DWORD GetLastSaveTime	    (DWORD dwClient);
	int		GetChaNum		    (DWORD dwClient);
	int		GetUserType         (DWORD dwClient);

	void    SetChaRemain        (DWORD dwClient, WORD wChaNum);
	void	SetChaTestRemain    (DWORD dwClient, WORD wChaNum);

	WORD    GetChaRemain	    (DWORD dwClient);
	WORD    GetChaTestRemain    (DWORD dwClient);

	void     SetChannel         (DWORD dwClient, int nChannel=0);
	int      GetChannel         (DWORD dwClient);

	bool	IsGaming            (DWORD dwClient);

    __time64_t GetPremiumDate   (DWORD dwClient);
    void       SetPremiumDate   (DWORD dwClient, __time64_t tTime);

	__time64_t GetChatBlockDate (DWORD dwClient);
    void       SetChatBlockDate (DWORD dwClient, __time64_t tTime);	

	//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
	bool nProtectSetAuthQuery(DWORD dwClient);
    	
	//! nProtect GameGuard 인증용 쿼리 가져오기
	GG_AUTH_DATA nProtectGetAuthQuery(DWORD dwClient);

	//! nProtect GameGuard 인증 응답 가져오기
	GG_AUTH_DATA nProtectGetAuthAnswer( DWORD dwClient );

	//! nProtect GameGuard 인증 응답 설정
	void nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad);

	//! nProtect GameGuard 인증 결과 가져오기
	DWORD nProtectCheckAuthAnswer(DWORD dwClient);

	//! nProtect GameGuard 인증 상태를 리셋시킨다.
	void nProtectResetAuth(DWORD dwClient);

	LPPER_IO_OPERATION_DATA GetFreeOverIO(int nType=0);
	void ReleaseOperationData(PER_IO_OPERATION_DATA* pData);

	//! 랜덤패스워드 번호를 설정한다.
	void SetRandomPassNumber(DWORD dwClient, int nNumber);

	//! 랜덤패스워드 번호를 가져온다.
	int GetRandomPassNumber(DWORD dwClient);

	// 극강부 남자 생성 갯수를 설정한다.
	void SetExtremeCreateM( DWORD dwClient, int nExtremeM );

	// 극강부 남자 생성 갯수를 가져온다.
	int GetExtremeM(DWORD dwClient);

	// 극강부 여자 생성 갯수를 설정한다.
	void SetExtremeCreateW( DWORD dwClient, int nExtremeW );

	// 극강부 여자 생성 갯수를 가져온다.
	int GetExtremeW(DWORD dwClient);

	// 극강부 남자 생성 가능을 설정한다.
	void SetExtremeCheckM( DWORD dwClient, int nExtremeCheckM );

	// 극강부 여자 생성 가능을 설정한다.
	void SetExtremeCheckW( DWORD dwClient, int nExtremeCheckW );

	// 극강부 남자 생성 가능을 가져온다.
	int GetExtremeCheckM( DWORD dwClient );

	// 극강부 여자 생성 가능을 가져온다.
	int GetExtremeCheckW( DWORD dwClient );

	// 성별 변경 카드에 따른 DB에서 연산된 극강부 생성 조건 저장
	// DB에 연산된 극강부 남자 생성 갯수를 설정한다.
	void SetExtremeCreateDBM( DWORD dwClient, int nExtremeDBM );

	// DB에 연산된 극강부 여자 생성 갯수를 설정한다.
	void SetExtremeCreateDBW( DWORD dwClient, int nExtremeDBW );

	// DB에 연산된 극강부 남자 생성 갯수를 가져온다.
	int GetExtremeDBM(DWORD dwClient);	

	// DB에 연산된 극강부 여자 생성 갯수를 가져온다.
	int GetExtremeDBW(DWORD dwClient);

	// 연결할 퍌드 서버의 정보를 설정한다
	//void SetConnectionFieldInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	
	// 연결할 필드 서버의 정보를 사용했으면 초기화한다.
	//void ResetConnectionFieldInfo( DWORD dwClient);

	// 연결할 필드서버로 갈 클라이언트의 GaeaID를 가져온다.
	//DWORD GetClientGaeaID( DWORD dwClient );

	// 태국 User Class Type을 설정한다.
	void SetThaiClass( DWORD dwClient, int nThaiCC_Class );

	// 태국 User Class Type을 가져온다.
	int GetThaiClass( DWORD dwClient );

	// 말레이시아 PC방 이벤트
	// 말레이시아 User Class Type을 설정한다.
	void SetMyClass( DWORD dwClient, int nMyCC_Class );

	// 말레이시아 User Class Type을 가져온다.
	int GetMyClass( DWORD dwClient );

	// 중국 GameTime을 세팅해 둔다.
	void SetChinaGameTime( DWORD dwClient, int nChinaGameTime );

	// 중국 GameTime을 가지고 온다.
	int GetChinaGameTime( DWORD dwClient );

	// 중국 OfflineTime을 세팅해 둔다.
	void SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime );

	// 중국 OfflineTime을 가지고 온다.
	int GetChinaOfflineTime( DWORD dwClient );

	// 중국 LastLoginDate를 세팅하고 가지고 온다.
	void       SetLastLoginDate   ( DWORD dwClient, __time64_t tTime );
	__time64_t GetLastLoginDate   ( DWORD dwClient );

	// 중국 UserAge를 설정해 둔다.
	void SetChinaUserAge( DWORD dwClient, int nChinaUserAge );

	// 중국 UserAge를 가지고 온다.
	int GetChinaUserAge( DWORD dwClient );

	// 베트남 GameTime을 세팅해 둔다.
	void SetVTGameTime( DWORD dwClient, int nChinaGameTime );

	// 베트남 GameTime을 가지고 온다.
	int GetVTGameTime( DWORD dwClient );

	// 메세지를 저장해 뒀다가 가져온다.
	//NET_MSG_GENERIC * GetNetMsg( DWORD dwClient );
};

#endif // S_CCLIENTMANAGER_H_