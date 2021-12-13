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

	DEQ_CLIENTS m_deqSleepCID;	//< ���� Ŭ���̾�Ʈ ID.
	MAP_CLIENTS	m_mapSleepCID;  //< ���� Ŭ���̾�Ʈ ID. ( �˻��� ����..)
	VEC_CLIENTS m_vecSleepCID;	//< ��ȯ�Ǵ� Ŭ���̾�Ʈ ID.
	VEC_CONNECTIP m_vecConnectIP; // �����ϴ� IP ����Ʈ

	
protected:	
	int m_nMaxClient; //< ���Ӱ����� �ִ� Ŭ���̾�Ʈ��	

	CNetUser* m_pClient; //< ����� ������ ��Ƶ� ������

	HANDLE m_hIOServer;
	COverlapped* m_pSendIOCP; //< ������ IOCP �޸� ������
	COverlapped* m_pRecvIOCP; //< �ޱ� IOCP �޸� ������

	SERVER_UTIL::CBit m_Bit; //< �������� �޸� Bit ������ ���� ���

	DWORD m_dwInPacketCount; //< ���ŵ� �� ��Ŷ ����
	DWORD m_dwInPacketSize; //< ���ŵ� �� ��Ŷ ������
	
	DWORD m_dwOutPacketCount; //< �۽��� �� ��Ŷ ����
	DWORD m_dwOutPacketSize; //< �۽��� �� ��Ŷ ������

	DWORD m_dwOutCompressCount; //< �۽��� �� ���� ��Ŷ ����
	DWORD m_dwOutCompressSize; //< �۽��� �� ���� ��Ŷ ũ��

//public:
//	//! -----------------------------------------------------------------------
//	//! Virtual Function
//	//! �ޱ� ��û Ƚ�� ī����
//	virtual int incRecvCount( DWORD dwClient );
//	virtual int decRecvCount( DWORD dwClient );
//	virtual int getRecvCount( DWORD dwClient );
//
//	//! ������ ��û Ƚ�� ī����
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
	//! ���ŵ� �� ��Ŷ ����
	DWORD getInPacketCount() { return m_dwInPacketCount; };
	//! ���ŵ� �� ��Ŷ ������
	DWORD getInPacketSize() { return m_dwInPacketSize; };

	//! �۽��� �� ��Ŷ ����
	DWORD getOutPacketCount() { return m_dwOutPacketCount; };
	//! �۽��� �� ��Ŷ ������
	DWORD getOutPacketSize() { return m_dwOutPacketSize; };

	//! �۽��� �� ���� ��Ŷ ����
	DWORD getOutCompressCount() { return m_dwOutCompressCount; };
	//! �۽��� �� ���� ��Ŷ ũ��
	DWORD getOutCompressSize() { return m_dwOutCompressSize; };

	//! ��� ��Ŷ ī���͸� �ʱ�ȭ �Ѵ�.
	void resetPacketCount();    

	void	Reset				(DWORD dwClient);
	int		CloseClient			(DWORD dwClient, bool bOnlineCheck = TRUE);	//! Ư��Ŭ���̾�Ʈ�� ������ �����Ѵ�.
	void	CloseAllClient();						//! ��� Ŭ���̾�Ʈ�� ������ �����Ѵ�.
	
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
	int		GetCurrentClientNumber(void); // ���� �������� Ŭ���̾�Ʈ ����	

	int	addRcvMsg(DWORD dwClient, 
	              void* pMsg, 
				  DWORD dwSize);

	void* getRcvMsg(DWORD dwClient,bool bClient);

	void ResetRcvBuffer(DWORD dwClient);	

	void    SendClientFinal();
	int		SendClient (DWORD dwClient, LPVOID pBuffer);
	int		SendClient2(DWORD dwClient, LPVOID pBuffer);
	int		SendClient2(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);

	bool	IsAccountPass		(DWORD dwClient); // ���� ����(login) ����Ͽ����� ����.
	void	SetAccountPass		(DWORD dwClient, bool bPASS); // ���� ����(login) ��� ����.

	CRYPT_KEY GetCryptKey		(DWORD dwClient);
	CRYPT_KEY GetNewCryptKey	(void);
	void	  SetCryptKey		(DWORD dwClient, CRYPT_KEY ck);
	
	// Slot type set and check
	DWORD	GetSlotAgentClient	(DWORD dwClient); // Agent ���������� Client ���� ��� ����
	DWORD 	GetSlotAgentField	(DWORD dwClient); // Agent ���������� Field �������� ��� ����
	DWORD	GetSlotFieldAgent	(DWORD dwClient); // Field ���������� Agent �������� ��� ����
	DWORD	GetSlotFieldClient	(DWORD dwClient); // Field ���������� Client ���� ��� ����
	DWORD	GetSlotType			(DWORD dwClient);

	void	SetSlotAgentClient	(DWORD dwClient, DWORD dwSlot); // Agent ���������� Client ���� ��� ����
	void	SetSlotAgentField	(DWORD dwClient, DWORD dwSlot); // Agent ���������� Field �������� ��� ����
	void	SetSlotFieldAgent	(DWORD dwClient, DWORD dwSlot); // Field ���������� Agent �������� ��� ����
	void	SetSlotFieldClient	(DWORD dwClient, DWORD dwSlot); // Field ���������� Client ���� ��� ����
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

	LPPER_IO_OPERATION_DATA GetFreeOverIO(int nType=0);
	void ReleaseOperationData(PER_IO_OPERATION_DATA* pData);

	//! �����н����� ��ȣ�� �����Ѵ�.
	void SetRandomPassNumber(DWORD dwClient, int nNumber);

	//! �����н����� ��ȣ�� �����´�.
	int GetRandomPassNumber(DWORD dwClient);

	// �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCreateM( DWORD dwClient, int nExtremeM );

	// �ذ��� ���� ���� ������ �����´�.
	int GetExtremeM(DWORD dwClient);

	// �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCreateW( DWORD dwClient, int nExtremeW );

	// �ذ��� ���� ���� ������ �����´�.
	int GetExtremeW(DWORD dwClient);

	// �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCheckM( DWORD dwClient, int nExtremeCheckM );

	// �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCheckW( DWORD dwClient, int nExtremeCheckW );

	// �ذ��� ���� ���� ������ �����´�.
	int GetExtremeCheckM( DWORD dwClient );

	// �ذ��� ���� ���� ������ �����´�.
	int GetExtremeCheckW( DWORD dwClient );

	// ���� ���� ī�忡 ���� DB���� ����� �ذ��� ���� ���� ����
	// DB�� ����� �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCreateDBM( DWORD dwClient, int nExtremeDBM );

	// DB�� ����� �ذ��� ���� ���� ������ �����Ѵ�.
	void SetExtremeCreateDBW( DWORD dwClient, int nExtremeDBW );

	// DB�� ����� �ذ��� ���� ���� ������ �����´�.
	int GetExtremeDBM(DWORD dwClient);	

	// DB�� ����� �ذ��� ���� ���� ������ �����´�.
	int GetExtremeDBW(DWORD dwClient);

	// ������ ���� ������ ������ �����Ѵ�
	//void SetConnectionFieldInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	
	// ������ �ʵ� ������ ������ ��������� �ʱ�ȭ�Ѵ�.
	//void ResetConnectionFieldInfo( DWORD dwClient);

	// ������ �ʵ弭���� �� Ŭ���̾�Ʈ�� GaeaID�� �����´�.
	//DWORD GetClientGaeaID( DWORD dwClient );

	// �±� User Class Type�� �����Ѵ�.
	void SetThaiClass( DWORD dwClient, int nThaiCC_Class );

	// �±� User Class Type�� �����´�.
	int GetThaiClass( DWORD dwClient );

	// �����̽þ� PC�� �̺�Ʈ
	// �����̽þ� User Class Type�� �����Ѵ�.
	void SetMyClass( DWORD dwClient, int nMyCC_Class );

	// �����̽þ� User Class Type�� �����´�.
	int GetMyClass( DWORD dwClient );

	// �߱� GameTime�� ������ �д�.
	void SetChinaGameTime( DWORD dwClient, int nChinaGameTime );

	// �߱� GameTime�� ������ �´�.
	int GetChinaGameTime( DWORD dwClient );

	// �߱� OfflineTime�� ������ �д�.
	void SetChinaOfflineTime( DWORD dwClient, int nChinaOfflineTime );

	// �߱� OfflineTime�� ������ �´�.
	int GetChinaOfflineTime( DWORD dwClient );

	// �߱� LastLoginDate�� �����ϰ� ������ �´�.
	void       SetLastLoginDate   ( DWORD dwClient, __time64_t tTime );
	__time64_t GetLastLoginDate   ( DWORD dwClient );

	// �߱� UserAge�� ������ �д�.
	void SetChinaUserAge( DWORD dwClient, int nChinaUserAge );

	// �߱� UserAge�� ������ �´�.
	int GetChinaUserAge( DWORD dwClient );

	// ��Ʈ�� GameTime�� ������ �д�.
	void SetVTGameTime( DWORD dwClient, int nChinaGameTime );

	// ��Ʈ�� GameTime�� ������ �´�.
	int GetVTGameTime( DWORD dwClient );

	// �޼����� ������ �״ٰ� �����´�.
	//NET_MSG_GENERIC * GetNetMsg( DWORD dwClient );
};

#endif // S_CCLIENTMANAGER_H_