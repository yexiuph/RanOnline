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

	bool				m_bACCOUNT_PASS;    // �����˻縦 ����ߴ��� ����
	bool                m_bACCOUNT_PASSING; // �����˻� ��������� �˻�.

	SOCKET				m_Socket;
	EM_NET_STATE        m_emOnline;
	//int				m_nOnline;	
	TCHAR				m_szIp[MAX_IP_LENGTH+1];
	USHORT				m_usPort;
	DWORD				m_dwLastMsgTime;	// ���������� �б� ����� �� �ð�
	DWORD				m_dwLastSaveTime;   // ���������� ĳ���͸� ������ �ð�
	int					m_nPing;			// Ŭ���̾�Ʈ�� ��ſ� �ɸ� �ð�
	int					m_nNetMode;			// Packet Head �� �������ΰ� Body �� �������ΰ� ����	
	CTime				m_LoginTime;		// �α����� ������ �ð�
	
	DWORD				m_dwGaeaID;			// ���������� '���̾�'������ ����ID.
	int					m_nChaNum;          // ���� ���ӿ� �������� ĳ���� ��ȣ
	int					m_nUserType;        // ������ Ÿ�� (�Ϲ�����, GM, Master)
    
	__time64_t          m_tPremiumDate;     // �����̾����� ����ð�.
	__time64_t          m_tChatBlockDate;   // ä�ú�� ����ð�.

	DWORD				m_dwSlotAgentClient;// Agent ���������� Client ���� ��� ����
	DWORD				m_dwSlotAgentField;	// Agent ���������� Field �������� ��� ����
	DWORD				m_dwSlotFieldAgent;	// Field ���������� Agent �������� ��� ����
	DWORD				m_dwSlotFieldClient;// Field ���������� Client ���� ��� ����
	DWORD				m_dwSlotType;

	CRYPT_KEY			m_ck; /// �⺻��ȣȭ ��ȣŰ (���� ������ ����)
	bool				m_bHeartBeat; /// ��Ʈ��Ʈ üũ����
	WORD				m_wChaRemain;
	WORD				m_wChaTestRemain;

	int					m_nChannel;            // ����ڰ� ������ ä��
	WORD                m_wInvalidPacketCount; // �߸��� ��Ŷ�� ���� Ƚ��
	WORD                m_wLoginAttempt;       // �ѹ� ���ӿ� �α��� ��û Ƚ��

	WORD				m_wConSendError;	// ���ӵ� ���� ���� Ƚ��, üũ �� Ŭ���̾�Ʈ ���� ���Ḧ ���� ���.
	
	SERVER_UTIL::CBit	m_Bit;
	
	CSendMsgBuffer*  m_pSendManager; ///< �޽��� ������ �޴���
	CRcvMsgBuffer*   m_pRcvManager; ///< �޽��� �ޱ� �޴���

	int	m_nRandomPass; ///< �����н����� üũ�� ���� ����

	// �ذ��� üũ
	int					m_nExtremeM;
	int					m_nExtremeW;
	int					m_nExtremeCheckM;
	int					m_nExtremeCheckW;

	int					m_nExtremeDBM;
	int					m_nExtremeDBW;

	// �±� User Class Type
	int					m_nThaiCC_Class;

	// �����̽þ� PC�� �̺�Ʈ
	// �����̽þ� User Class Type
	int					m_nMyCC_Class;

	// �߱� �α��� �ð� üũ
	int					m_nChinaGameTime;
	int					m_nChinaOfflineTime;
	__time64_t			m_tLastLoginTime;
	int					m_nChinaUserAge;

	// ��Ʈ�� Ž�й��� ���� �߰�
	int					m_nVTGameTime;

	// ����� �����Ⱚ
	char		m_szRecvGarbageMsg[2][12];
	BYTE		m_dwGarbageNum;

	//DWORD m_dwSendIoCount; //! ������ ��ûȽ��
	//DWORD m_dwRecvIoCount; //! �ޱ� ��û Ƚ��

	//////////////////////////////////////////////////////////////////////////////
	// Thailand
	WORD m_wThaiFlag; /// Thailand ����� �÷���
	WORD m_wThaiTodayGamePlayTime; /// Thailand ������� �����÷��̽ð�(��)	

	//////////////////////////////////////////////////////////////////////////////
	// Malaysia
	TCHAR m_szDecodedTID[TERRA_TID_DECODE+1]; /// Malaysia User information	

	// GSP
	TCHAR m_szUUID[UUID_STR_LENGTH]; /// UUID User length	

	// Encrypt
	TCHAR m_szEncryptKeyHeart[ENCRYPT_KEY+1];

public:
	////! ���� ���� ī����
	//void initSendError()		{ m_wConSendError = 0; }
	//void incSendError()			{ ++m_wConSendError; }
	//WORD getSendError()			{ return m_wConSendError; }

	////! �ޱ� ��û Ƚ�� ī����
	//int incRecvCount();
	//int decRecvCount();
	//int getRecvCount();

	////! ������ ��û Ƚ�� ī����
	//int incSendCount();
	//int decSendCount();
	//int getSendCount();

	//! �������� �޽����� ����Ѵ�.
	int addSendMsg(void* pMsg, DWORD dwSize);

	//! �������� �޽��� ���۸� �����´�.
	void* getSendBuffer();

	//! �������� �޽��� ũ�⸦ �����´�.
	int getSendSize();

	//! Ŭ���̾�Ʈ���� ���� �޽����� �ִ´�.
	int addRcvMsg(void* pMsg, DWORD dwSize);

	//! Ŭ���̾�Ʈ���� ���� �޽����� �ϳ� �����´�.
	void* getRcvMsg( bool bClient );

	//! Ŭ���̾�Ʈ ���� ����
	void Reset();

	//! Ŭ���̾�Ʈ ���� ����
	void ResetRcvBuffer();

	//! Ŭ���̾�Ʈ �����Ŵ
	int CloseClient();

	//! Ŭ���̾�Ʈ���� �޴� �޽����� ������ ���� üũ�Ѵ�.
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

	void	SetSlotAgentClient(DWORD dwSlot); // Agent ���������� Client ���� ��� ����
	void	SetSlotAgentField (DWORD dwSlot); // Agent ���������� Field �������� ��� ����
	void	SetSlotFieldAgent (DWORD dwSlot); // Field ���������� Agent �������� ��� ����
	void	SetSlotFieldClient(DWORD dwSlot); // Field ���������� Client ���� ��� ����	
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
	DWORD	GetSlotAgentClient(); // Agent ���������� Client ���� ��� ����
	DWORD 	GetSlotAgentField (); // Agent ���������� Field �������� ��� ����
	DWORD	GetSlotFieldAgent (); // Field ���������� Agent �������� ��� ����
	DWORD	GetSlotFieldClient(); // Field ���������� Client ���� ��� ����	
	DWORD	GetSlotType();
	
	inline bool IsClientSlot() { return m_dwSlotType == NET_SLOT_CLIENT ? true : false; }
	inline bool IsFieldSlot()  { return m_dwSlotType == NET_SLOT_FIELD  ? true : false; }
	inline bool IsAgentSlot()  { return m_dwSlotType == NET_SLOT_AGENT  ? true : false; }

	bool	IsAccountPass();
	void	SetAccountPass(bool bPASS);

	bool    IsAccountPassing(); // �����˻� ��������� �˻�
	void    SetAccountPassing(bool bPassing); // �����˻� ��������� ����

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
	// �ذ��� üũ 
	//////////////////////////////////////////////////////////////////////////////
	void	SetExtremeCreateM( int nExtremeM );
	void	SetExtremeCreateW( int nExtremeW );
	void	SetExtremeCheckM( int nExtremeCheckM );
	void	SetExtremeCheckW( int nExtremeCheckW );

	int		GetExtremeM();
	int		GetExtremeW();
	int		GetExtremeCheckM();
	int		GetExtremeCheckW();

	// DB���� ����� �ذ��� üũ ���� ������ �д�.
	void	SetExtremeCreateDBM( int nExtremeDBM );
	void	SetExtremeCreateDBW( int nExtremeDBW );
	void	SetExtremeCheckDBM( int nExtremeCheckDBM );
	void	SetExtremeCheckDBW( int nExtremeCheckDBW );

	int		GetExtremeDBM();
	int		GetExtremeDBW();
	int		GetExtremeCheckDBM();
	int		GetExtremeCheckDBW();

	// �±� User Class Type�� �����Ѵ�.
	void SetThaiClass( int nThaiCC_Class );

	// �±� User Class Type�� �����´�.
	int GetThaiClass();

	// �����̽þ� PC�� �̺�Ʈ
	// �����̽þ� User Class Type�� �����Ѵ�.
	void SetMyClass( int nMyCC_Class );

	// �����̽þ� User Class Type�� �����´�.
	int GetMyClass();

	//////////////////////////////////////////////////////////////////////////////
	// �߱� GameTime, OfflineTime üũ
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
	// ��Ʈ�� GameTime üũ
	//////////////////////////////////////////////////////////////////////////////
	void SetVTGameTime( int nVTGameTime );
	int GetVTGameTime();


	//////////////////////////////////////////////////////////////////////////////
	// Ŭ���̾�Ʈ �ʵ尣 �̵��� ���� ����
	//////////////////////////////////////////////////////////////////////////////
	//DWORD	m_dwClientGaeaID;
	//NET_MSG_GENERIC	m_nmg;

 //   // ������ ���� ������ ������ �����Ѵ�
	//void	SetConnectionFieldInfo( DWORD dwGaeaID, NET_MSG_GENERIC* nmg );
	//
	//// ������ �ʵ� ������ ������ ��������� �ʱ�ȭ�Ѵ�.
	//void	ResetConnectionFieldInfo();

	//// ������ �ʵ弭���� �� Ŭ���̾�Ʈ�� GaeaID�� �����´�.
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
	
	//! ���������� ��û���� 1 ȸ ���ӽ� 5 ȸ �̻� �α��� ��û�� 
	//! ������� ������ ����...
	//! �α��� ��û Ƚ�� ������Ŵ
	void IncLoginAttempt();

	//! �α��� ��û Ƚ�� ������
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
	CCSAuth2            m_csa; ///< nProtect GameGuard ������ Ŭ����
	bool                m_bNProtectAuth;
	bool                m_bNProtectFirstCheck; ///< ������ nProtect �� ó�� �����޴°�?	
	bool                m_bNProtectFirstAuthPass; ///< ���ӿ� �����ϱ� ���� ��� ������ �Ϸ��ߴ°�?
	
public:
	//! nProtect ó�� �����ΰ�?
	bool nProtectIsFirstCheck();

	//! nProtect ó�� �������� ����
	void nProtectSetFirstCheck(bool bState);

	//! nProtect GameGuard ������ ������ �������� ���� ȣ��
	bool nProtectSetAuthQuery();
    	
	//! nProtect GameGuard ������ ���� ��������
	GG_AUTH_DATA nProtectGetAuthQuery();

	//! nProtect GameGuard ���� ���� ��������
	GG_AUTH_DATA nProtectGetAuthAnswer();

	//! nProtect GameGuard ���� ���� ����
	void nProtectSetAuthAnswer(GG_AUTH_DATA& ggad);

	//! nProtect GameGuard ���� ��� ��������
	DWORD nProtectCheckAuthAnswer();

	//! nProtect GameGuard ���� ���¸� ���½�Ų��.
	void nProtectResetAuth();

	//! nProtect GameGuard ó�� ������ ����ߴ°�?
	//! �� ������ ����ؾ� �����÷��̸� �� �� �ִ�.
	bool nProtectFirstAuthPass();

	//! nProtect GameGuard ó������ ������� ����
	void nProtectSetFirstAuthPass(bool bState);
};

#endif // S_NETUSER_H_