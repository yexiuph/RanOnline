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
	//! �ޱ� ��û Ƚ�� ī����
	//virtual int incRecvCount( DWORD dwClient );
	//virtual int decRecvCount( DWORD dwClient );
	//virtual int getRecvCount( DWORD dwClient );

	////! ������ ��û Ƚ�� ī����
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

	int		GetCurrentClientNumber	(void); // ���� �������� Ŭ���̾�Ʈ ����
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

	// �ѱ� �α��� ��� ���濡 ���� �߰�
	void    KorSetUUID( DWORD dwClient, const TCHAR* szUUID );
	TCHAR*  KorGetUUID( DWORD dwClient );

	// �Ϻ� WhiteRock�� ����� �ϰ� Return�� UUID ������ ���ؼ� �߰�.
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

	bool    IsAccountPass  (DWORD dwClient); // ���� ����(login) ����Ͽ����� ����.
    void    SetAccountPass (DWORD dwClient, bool bPASS); // ���� ����(login) ��� ����.

	bool    IsAccountPassing(DWORD dwClient); // ���� ���������� �˻�
	void    SetAccountPassing(DWORD dwClient, bool bPassing); // ���� ������ ����

	void    SaveFreeSlot   ();

	// �����̾� �Ⱓ�� �����Ѵ�.
    __time64_t GetPremiumDate(DWORD dwClient);
    void       SetPremiumDate(DWORD dwClient, __time64_t tTime);

	// ä�� ��Ͻð��� �����Ѵ�.
	__time64_t GetChatBlockDate(DWORD dwClient);
    void       SetChatBlockDate(DWORD dwClient, __time64_t tTime);
    
	/**
	* Thailand �� ����� �÷��׸� �����Ѵ�.
	* \param dwClient
	* \param wFlag 
	*/
	void ThaiSetFlag (DWORD dwClient, WORD wFlag);
	WORD ThaiGetFlag (DWORD dwClient);
	void ThaiSetTodayPlayTime (DWORD dwClient, WORD wMinutes);
	bool ThaiIs3HourOver (DWORD dwClient, CTime& currentTime);	

	//! ���������� ��û���� 1 ȸ ���ӽ� 5 ȸ �̻� �α��� ��û�� 
	//! ������� ������ ����...
	//! �α��� ��û Ƚ�� ������Ŵ
	void IncLoginAttempt(DWORD dwClient);
	//! �α��� ��û Ƚ�� ������
	WORD GetLoginAttempt(DWORD dwClient);	

	//////////////////////////////////////////////////////////////////////////////
	// nProtect
	//////////////////////////////////////////////////////////////////////////////
	//! nProtect GameGuard ������ ������ �������� ���� ȣ��
	bool nProtectSetAuthQuery(DWORD dwClient);
    	
	//! nProtect GameGuard ������ ���� ��������
	GG_AUTH_DATA nProtectGetAuthQuery(DWORD dwClient);

	//! nProtect GameGuard ���� ���� ��������
	GG_AUTH_DATA nProtectGetAuthAnswer( DWORD dwClient );

	//! nProtect GameGuard ���� ���� ����
	void nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad);

	//! nProtect GameGuard ���� ��� ��������
	DWORD nProtectCheckAuthAnswer(DWORD dwClient);

	//! nProtect GameGuard ���� ���¸� ���½�Ų��.
	void nProtectResetAuth(DWORD dwClient);

	//! nProtect ó�� �������� ����
	void nProtectFirstCheck(DWORD dwClient, bool bState);

	//! nProtect ó�� �����ΰ�?
	bool nProtectIsFirstCheck(DWORD dwClient);
	
	//! nProtect GameGuard ó�� ������ ����ߴ°�?
	//! �� ������ ����ؾ� �����÷��̸� �� �� �ִ�.
	bool nProtectFirstAuthPass( DWORD dwClient );

	//! nProtect GameGuard ó������ ������� ����
	void nProtectSetFirstAuthPass( DWORD dwClient, bool bState );
};

#endif // S_CCLIENTAGENT_H_