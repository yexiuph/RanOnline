#ifndef S_CCLIENTAGENT_H_
#define S_CCLIENTAGENT_H_

#include "s_NetGlobal.h"
#include "s_CClientManager.h"

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
 * \date 2002-05-30
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
class CClientAgent : public CClientManager
{
public :
	CClientAgent(int nMaxClient,
				COverlapped* pSendIOCP,
				COverlapped* pRecvIOCP,
				HANDLE	hIOServer );
	virtual ~CClientAgent();

protected :
	CNetUser* m_pField;

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function
	//! 받기 요청 횟수 카운터
	//virtual int incRecvCount( DWORD dwClient );
	//virtual int decRecvCount( DWORD dwClient );
	//virtual int getRecvCount( DWORD dwClient );

	////! 보내기 요청 횟수 카운터
	//virtual int incSendCount( DWORD dwClient );
	//virtual int decSendCount( DWORD dwClient );
	//virtual int getSendCount( DWORD dwClient );

	//virtual PER_IO_OPERATION_DATA* getSendIO( DWORD dwClient );
	//virtual PER_IO_OPERATION_DATA* getRecvIO( DWORD dwClient );
	//
	//virtual void releaseSendIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );
	//virtual void releaseRecvIO( DWORD dwClient, PER_IO_OPERATION_DATA* pData );

public :
	int 	SetAcceptedClient(DWORD dwClient, SOCKET sSocket);

	SOCKET	GetSocket	  (DWORD dwClient);
	SOCKET	GetFieldSocket(DWORD dwClient);

	int		ConnectFieldServer(DWORD dwClient, unsigned long lServerIP, int nPort, DWORD dwGaeaID, int nServerChannel=0);
	int		ConnectFieldServer(DWORD dwClient, TCHAR* szIP, int nPort, DWORD dwGaeaID, int nServerChannel=0);	
	int		ConnectFieldServer(DWORD dwClient, int nFieldServer, DWORD dwGaeaID=0, int nServerChannel=0);

	int		CloseField(DWORD dwClient);

	int		Send      (DWORD dwClient, LPVOID pBuffer);
	int		Send      (DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);
	int		SendField (DWORD dwClient, LPVOID pBuffer);
	int		SendField (DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);

	void	SendClientFinal();

	int addRcvMsg(DWORD dwClient, void* pMsg, DWORD dwSize);

	void	ResetRcvBuffer(DWORD dwClient);

	void*	getRcvMsg(DWORD dwClient);

	DWORD	GetGaeaID	  (DWORD dwClient);
	int		GetChaNum	  (DWORD dwClient);
    bool    IsFieldOnline (DWORD dwClient);
	bool	IsOnline      (DWORD dwClient);

	int		GetCurrentClientNumber	(void); // 현재 접속중인 클라이언트 숫자
	void	SetHeartBeat   (DWORD dwClient);
	void    ResetHeartBeat (DWORD dwClient);
	bool	CheckHeartBeat (DWORD dwClient);
	// void SetLastMsgTime (DWORD dwClient, DWORD dwTime);
	// DWORD GetLastMsgTime (DWORD dwClient);
	// void SetLastSaveTime(DWORD dwClient, DWORD dwTime);
	// DWORD GetLastSaveTime(DWORD dwClient);
	DWORD	GetSlotType    (DWORD dwClient);
	TCHAR*	GetClientIP    (DWORD dwClient);
	USHORT  GetClientPort  (DWORD dwClient);

	// 한국 로그인 방식 변경에 따른 추가
	void    KorSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*  KorGetUUID( DWORD dwClient );

	// 일본 WhiteRock과 통신을 하고 Return된 UUID 세팅을 위해서 추가.
	void JPSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR* JPGetUUID( DWORD dwClient );

	void    GspSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*  GspGetUUID( DWORD dwClient );

	void    TerraSetDecodedTID( DWORD dwClient, const TCHAR* szDecodedTID );
	TCHAR*  TerraGetDecodedTID( DWORD dwClient );

	void    SetEncKey( DWORD dwClient, const TCHAR* szEncKey );
	TCHAR*  GetEncKey( DWORD dwClient );

	void    SetChannel     (DWORD dwClient, int nChannel=0);
	int     GetChannel     (DWORD dwClient);

	bool    IsAccountPass  (DWORD dwClient); // 계정 인증(login) 통과하였는지 점검.
    void    SetAccountPass (DWORD dwClient, bool bPASS); // 계정 인증(login) 통과 설정.

	bool    IsAccountPassing(DWORD dwClient); // 계정 인증중인지 검사
	void    SetAccountPassing(DWORD dwClient, bool bPassing); // 계정 인증중 세팅

	void    SaveFreeSlot   ();

	// 프리미엄 기간을 세팅한다.
    __time64_t GetPremiumDate(DWORD dwClient);
    void       SetPremiumDate(DWORD dwClient, __time64_t tTime);

	// 채팅 블록시간을 세팅한다.
	__time64_t GetChatBlockDate(DWORD dwClient);
    void       SetChatBlockDate(DWORD dwClient, __time64_t tTime);
    
	/**
	* Thailand 의 사용자 플래그를 세팅한다.
	* \param dwClient
	* \param wFlag 
	*/
	void ThaiSetFlag (DWORD dwClient, WORD wFlag);
	WORD ThaiGetFlag (DWORD dwClient);
	void ThaiSetTodayPlayTime (DWORD dwClient, WORD wMinutes);
	bool ThaiIs3HourOver (DWORD dwClient, CTime& currentTime);	

	//! 말레이지아 요청으로 1 회 접속시 5 회 이상 로그인 요청시 
	//! 사용자의 접속을 끊음...
	//! 로그인 요청 횟수 증가시킴
	void IncLoginAttempt(DWORD dwClient);
	//! 로그인 요청 횟수 가져옴
	WORD GetLoginAttempt(DWORD dwClient);	

	//////////////////////////////////////////////////////////////////////////////
	// nProtect
	//////////////////////////////////////////////////////////////////////////////
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

	//! nProtect 처음 인증여부 세팅
	void nProtectFirstCheck(DWORD dwClient, bool bState);

	//! nProtect 처음 인증인가?
	bool nProtectIsFirstCheck(DWORD dwClient);
	
	//! nProtect GameGuard 처음 인증을 통과했는가?
	//! 이 인증을 통과해야 게임플레이를 할 수 있다.
	bool nProtectFirstAuthPass( DWORD dwClient );

	//! nProtect GameGuard 처음인증 통과상태 세팅
	void nProtectSetFirstAuthPass( DWORD dwClient, bool bState );
};

#endif // S_CCLIENTAGENT_H_