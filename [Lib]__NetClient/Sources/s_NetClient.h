///////////////////////////////////////////////////////////////////////////////
// s_NetClient.h
//
// class CNetClient
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_NETCLIENT_H_
#define S_NETCLIENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s_NetGlobal.h"
#include "Shellapi.h"
#include "s_CBit.h"
#include "minTea.h"
#include "DxMsgProcessor.h"
#include "RcvMsgBuffer.h"

#define NET_CLIENT_IP	20
#define NET_NOTIFY		WM_USER + 99

#ifndef MAX_CHAR_LENGTH
#define MAX_CHAR_LENGTH	255
#endif

extern char* AlphaHomepage;
extern char* AlphaDownload;
extern char* AlphaRegister;
extern char* AlphaLogin;

extern char* BetaHomepage;
extern char* BetaDownload;
extern char* BetaRegister;
extern char* BetaLogin;

extern char* RanHomepage;
extern char* RanDownload;
extern char* RanRegister;
extern char* RanLogin;

extern char* g_szClientVerFile;

///////////////////////////////////////////////////////////////////////////////
// Values
extern SOCKET				g_sClient;

///////////////////////////////////////////////////////////////////////////////
// Function
static unsigned int WINAPI NetThread(void* p);

/**
 * \ingroup NetClientLib
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2003-05-25
 *
 * \author jgkim
 *
 * \par license
 * 
 * \todo 
 *
 * \bug 
 *
 */
class CNetClient
{
public:
	CNetClient();
	CNetClient(HWND hWnd);
	~CNetClient();
	
	int			m_nGameProgramVer;
	int			m_nPatchProgramVer;
private:
	enum
	{
		eERROR,		///< ���� ���Ͽ���
		eNORMAL,	///< ���� ��ġ
		eVERUP,		///< ������ �ٿ��ؾ� ��
		ePATCHUP,   ///< ��ġ���α׷� �ٿ��ؾ� ��
		eGAMEUP,	///< ����	�ٿ��ؾ� ��
		eALLUP		///< ��ġ, ���� �ٿ��ؾ� ��
	};

public:
	enum
	{
		//NET_ERR_SNDVER = NET_ERROR - 1,
		NET_ERR_CREATE_EVENT = NET_ERROR - 2,
		NET_ERR_EVENT_SELECT = NET_ERROR - 3,
		NET_ERR_CONNECT		 = NET_ERROR - 4,
		NET_ERR_EVENT_WAIT	 = NET_ERROR - 5,
		NET_ERR_CONTACT_EVENT= NET_ERROR - 6,
		NET_ERR_CREATE_THREAD= NET_ERROR - 7,
	};

protected:
	SOCKET		m_sClient;
	SOCKADDR_IN	m_Addr;
	HWND		m_hWnd;
	SERVER_UTIL::CBit m_Bit; ///< ��Ʈ���� Ŭ����
	minTea		m_Tea; ///< ��ȣȭ Ŭ����

	BOOL		m_bThreadProc;

	BOOL		m_bGameServerInfoEnd; ///< ��������Ʈ�� ���� �޾Ҵ��� ����üũ
	BOOL		m_bKeyReceived;		  ///< Ű�� �޾Ҵ��� üũ�Ѵ�.

	INT			m_nLoginResult;		  ///< �α��� ��� ����
	CRYPT_KEY	m_ck;
	int         m_nRandomNumber;      ///< ���� �н����� ��ȣ

	char		m_cClientIP[MAX_IP_LENGTH+1];
	char		m_cServerIP[MAX_IP_LENGTH+1];
	
	char		m_szAppPath[1024];		

	int			m_cServerPort;
	int			m_nOnline;
	int			m_nClientNetState;
	char*		m_pSndBuffer;
	int			m_nSndBytes;

	CRcvMsgBuffer* m_pRcvManager;	

	int			m_nTotalBytes;
	int			m_nNetMode;

	DWORD		m_dwGaeaID;

	HANDLE		m_hEvent;
	HANDLE		m_hThread;
	HANDLE		m_hEventWait;
	HANDLE		m_hEventWork;
	HANDLE		m_hEventKill;

	CRITICAL_SECTION		m_CriticalSection; // criticalsection object
	G_SERVER_CUR_INFO_LOGIN m_sGame[MAX_SERVER_GROUP][MAX_CHANNEL_NUMBER];
	DxMsgProcessor*			m_pGlobalStage;

	CHAR		m_szEncryptKey[ENCRYPT_KEY+1];
	CHAR		m_szEncryptKeyHeart[ENCRYPT_KEY+1];

	CNetClient*	m_pNetField;
	CNetClient* m_pNetBoard;

	char		m_szSendGarbageMsg[2][12];
	BYTE		m_dwGarbageNum;

	// USHORT		m_nKeyDirection;	  ///< ��ȣŰ ����
	// USHORT		m_nKey;			      ///< ��ȣŰ shift ��
	// char*		m_pRcvBuffer;
	// char*		m_pMsgBuffer;	
	// int			m_nRcvBytes;

public:
	ApexChina::CGameClient m_ApexClient; ///< APEX RSA ��ȣȭ

	void EncryptLoginDataWithApexRSA(	unsigned char * szUserID, 
										unsigned char * szUserPassword, 
										unsigned char * szRandomPassword );
	
public:
	G_SERVER_CUR_INFO_LOGIN* GetSeverCurInfoLogin ()	{ return (G_SERVER_CUR_INFO_LOGIN*)m_sGame; }
	void	LockOn();
	void	LockOff();

	int		ConnectServer     (unsigned long uServerIP,  u_short nPort, int nState);
	int		ConnectServer     (const char*   szServerIP, int nPort,     int nState);
	int		ConnectServer	  (const char*   szServerIP, int nPort,     int nState, CRYPT_KEY ck);
	int		ConnectLoginServer(const char *szAddress, int nPort=5001);
	int		ConnectFieldServer(char *szAddress, int nPort);
	int		ConnectBoardServer(char *szAddress, int nPort);
	int		ConnectAgentServer(int nServerGroup, int nServerNumber);
	int		ConnectGameServer (int nServerGroup, int nServerNumber);
	int		CloseConnect      (void);
	
	BOOL	IsThreadProc ()	{ return m_bThreadProc; }

	void	SetWndHandle ( HWND hWnd, DxMsgProcessor* pGlobalStage=NULL );
	
	int		StartNetThread();
	int		StopNetThread(bool wait);
	int		NetThreadProc();

	BOOL	CheckClientVer(); // Ŭ���̾�Ʈ�� ������ üũ�Ѵ�.
	
	int		GetClientVer(); // ���Ϸκ��� Ŭ���̾�Ʈ ������ �����´�.
	int		GetGameVer();
	int		GetPatchVer();
	char*	GetSreverIP(void);

	int     GetRandomPassNumber(void)		{ return m_nRandomNumber; }
	void    ResetRandomPassNumber()			{ m_nRandomNumber = 0; }

	void	SetAppPath(); // ���α׷��� �����θ� �����Ѵ�.
	void	SetDefault();
	void	SetGaeaID(DWORD dwGaeaID)		{ m_dwGaeaID = dwGaeaID; }
	DWORD	GetGaeaID()						{ return m_dwGaeaID; }

	bool	IsOnline();
	bool	IsOnlineField();
	bool	IsOnlineBoard();
	BOOL	IsGameSvrInfoEnd();
	BOOL	IsKeyReceived();
	int		IsLogin();

	int		Recv(void);
	int		Send	   (CHAR* buff);
	int		Send       (CHAR* buff, INT nSize);
	int		SendNormal (CHAR* buff, INT nSize);
	int		GetGarbageMsg();
	char*   SendMsgAddGarbageValue( CHAR* buff, INT &nSize );
	int		SendToAgent(CHAR* buff, INT nSize);
	int		SendToBoard(CHAR* buff, INT nSize);
	int		SendToField(CHAR* buff, INT nSize);
	int		SendToFieldNormal(CHAR* buff, INT nSize);
	int	    SendBuffer();
	int		SendBuffer2();
	void*	IsValid(void);

	void	RegisterNewAccount(void);

	//////////////////////////////////////////////////////////////////////////////
	void	ClientProcess(WPARAM wParam, LPARAM lParam);
	
	int		SndVersion(int nGameProgramVer, int nPatchProgramVer);
	int		SndVersion(void);

	const TCHAR* DoEncryptKey();	// ��Ʈ��Ʈ�� Ű�� ��ȣȭ�Ѵ�. ( Nprotect ���� ) 

	/**
	* �α��� ������ �����Ѵ�. 
	* Taiwan / Hongkong / ����
	*/
	int SndLogin(const char* szUserID,
		         const char* szUserPassword,
				 const char* szRandomPassword,
		     	 int nChannel=0);

	/**
	* �α��� ������ �����Ѵ�. 
	* China	
	*/
	int ChinaSndLogin( 
				const TCHAR* szUserID,
		        const TCHAR* szUserPassword,
				const TCHAR* szRandomPassword,
		     	int nChannel=0 );

    /**
	* �α��� ������ �����Ѵ�. 
	* Thailand (�±�)
	*/
	int ThaiSndLogin(const char* userid,
		             const char* userpass,
					 int nChannel);

	/**
	* �α��� ������ �����Ѵ�.
	* Daum Game ( �ѱ� ) 
	*/
	int	DaumSndLogin(LPCSTR szParameter, 
		             int nChannel);

	int DaumSndPassCheck(
			const TCHAR* szDaumGID,
			const TCHAR* szUserPassword,
			int nCheckFlag );

	/**
	* �α��� ������ �����Ѵ�.
	* Terra (Malaysia, �ʸ���, ��Ʈ��)
	*/
	int TerraSndLogin(
			const TCHAR* szTID, 
		    int nChannel );

	int TerraSndPassCheck(
			const TCHAR* szUserid,
			const TCHAR* szUserPassword,
			int nCheckFlag );

	/**
	* �α��� ������ �����Ѵ�.
	* GSP
	*/
	int GspSndLogin(
			const TCHAR* szUUID,
		    int nChannel );

	/**
	* Excite Japan �α��� ������ ����
	* \param szU UserID char 20
	* \param szT ��ȿ�Ⱓ char 14
	* \param szMD5 MD5 char 32
	* \param nChannel ä�ι�ȣ
	* \return 
	*/
	int ExciteSndLogin(const char* szU,
					   const char* szT,
					   const char* szMD5,
					   int nChannel);

	int ExciteSndPassCheck(
			const TCHAR* szUserid,			
			const TCHAR* szUserPassword,
			int nCheckFlag );
	

	/**
	* Excite Japan �α��� ������ ����
	* \param szU UserID char 20
	* \param szT ��ȿ�Ⱓ char 14
	* \param szMD5 MD5 char 32
	* \param nChannel ä�ι�ȣ
	* \return 
	*/
	int JapanSndLogin(const char* szUserID,
		         const char* szUserPassword,
		     	 int nChannel=0);

	/**
	* �α��� ������ �����Ѵ�.
	* �۷ι� ����
	*/
	int GsSndLogin( const char* userid,
		            const char* userpass,
					int nChannel);

	

	int		SndAccountInfo(const char* uID, const char* uPass, const char* uName);
	int		SndRequestUserInfo(void);
	int		SndCreateChaInfo(int nIndex, WORD wSchool, WORD wFace, WORD wHair, WORD wHairColor, WORD wSex, const char* strChaName);
	int		SndChaAllBasicInfo(); // ����� ĳ������ �⺻������ ��û�Ѵ�.
	int		SndChaBasicInfo(int nChaNum); // ����� ĳ���� �⺻������ ��û�Ѵ�. ĳ���� ����ȭ�� ���÷��̿�
	
	/**
	* ĳ���� ���� ��û
	* \param nChaNum ������ ĳ���͹�ȣ
	* \param strPasswd ��й�ȣ
	* \return 
	*/
	int	SndDelCharacter( int nChaNum, const TCHAR* szPasswd );

	/**
	* ĳ���� ���� ��û
	* \param nChaNum ������ ĳ���͹�ȣ
	* \param strPasswd ��й�ȣ
	* \return 
	*/
	int	ChinaSndDelCharacter( int nChaNum, const TCHAR* szPasswd );

	/**
	* ĳ���� ���� ��û
	* \param nChaNum ������ ĳ���͹�ȣ
	* \return 
	*/
	int	DaumSndDelCharacter(int nChaNum);

	/**
	* ĳ���� ���� ��û
	* \param nChaNum ĳ���͹�ȣ
	* \return 
	*/
	int TerraSndDelCharacter(int nChaNum);

	/**
	* ĳ���� ���� ��û GSP
	* \param nChaNum ĳ���͹�ȣ
	* \return 
	*/
	int GspSndDelCharacter( int nChaNum );

	/**
	* ĳ���� ���� ��û
	* \param nChaNum ĳ���͹�ȣ
	* \return 
	*/
	int ExciteSndDelCharacter(int nChaNum);

	/**
	* ĳ���� ���� ��û
	* \param nChaNum ������ ĳ���͹�ȣ
	* \return 
	*/
	int	GsSndDelCharacter(int nChaNum);

	/**
	* ������ ����Ű�� ��û�Ѵ�.
	* client->agent
	*/
	int SndRequestRandomKey(void);

	int SndRequestAllChaInfo(void); // �ش� ������� ��� ĳ���� ������ ��û�Ѵ�.
	int	SndPing(void);	
	int	SndReqServerInfo(void); // ���Ӽ����� ������ ������ ��û�Ѵ�.
	int	SndGameJoin(int nChaNum);
	int	SndHeartbeat();
	
	int	SndChatNormal(const char* szName, const char* szMsg);	
	int	SndChatParty(const char* szMsg);
	int	SndChatPrivate(const char* szName, const char* szMsg);
	int	SndChatGuild(const char* szMsg);
	int	SndChatAlliance(const char* szMsg);

	int SndEventLottery(const char* szLotteryName);
	
	//////////////////////////////////////////////////////////////////////////////
	void MessageProcess();
	void MessageProcessLogin (NET_MSG_GENERIC* nmg);
	void MessageProcessGame  (NET_MSG_GENERIC* nmg);
	void MessageProcessCommon(NET_MSG_GENERIC* nmg);

	//////////////////////////////////////////////////////////////////////////////
	// Game Server Message	
	void	MsgCryptKey			(NET_MSG_GENERIC* nmg);
	void    MsgRandomNum        (NET_MSG_GENERIC* nmg);
	void	MsgLoginFeedBack    (NET_MSG_GENERIC* nmg);	
	void	MsgChat				(NET_MSG_GENERIC* nmg);	// ä�ø޽���
	void	MsgChaBAInfo		(NET_MSG_GENERIC* nmg);
	void	MsgChaBInfo			(NET_MSG_GENERIC* nmg);
	int		MsgConnectClientToField(NET_MSG_GENERIC* nmg);
	
	//////////////////////////////////////////////////////////////////////////////
	// Login Server Message
	void	MsgGameSvrInfo		(NET_MSG_GENERIC* nmg);
	void	MsgGameSvrInfoEnd	(NET_MSG_GENERIC* nmg);
		
	//////////////////////////////////////////////////////////////////////////////
	// Common Message	
	int	MsgVersionInfo( NET_MSG_GENERIC* nmg );
	int	MsgEncryptKey( NET_MSG_GENERIC* nmg );
	void MsgNewVersionDown();

};
#endif // S_NETCLIENT_H_


//      ,
//    ,x?n=4??$$n.
//  -Lb$;zPs$Lb;d)%.
// ;d$$$$$$$$$$$$b$;-
// $$$$$$$$$$$$$$$$$;-
// ?$$$$$$$$$$$$$$$$"
//.d$$$$$$$$$$$$$$$$.
// `$$c,,$$$bc,,d$$>"
//  ?$$$$$eed$$$$$F
//   "?$$$L,J$$$F)ee,
//       """";zd$$$$$$$ec,
//        .dF$$$$$$$$$$$$$$$eeu,,.;;!!!!!!!!!i;
//        <Fd$$$$$$$$$$$$$$$$$$$P,!!!!!!!!!!!!!!i
//         '$$$$$$$'<$$$$$$$$$$":!!!'!!!''(!!!!!!!>
//          $$$$$$'d$$$$$$$$$P.<!'./` -'`((((!!!!!!
//          $$$?$'$$$$$$$$$$$ !' `,cucucuc, `'!!!!'
//          $$Jz".,"?$$$$$$$F   u$$$$$$$$$$$r `!'
//         d$$$$ d$$$   """""  d$$$$$$$$$$$$" ueP
//        -$$$$"ci"$"         $$$$$$$$$$$P".,d$$        __    e$ec.
//   e=7?Rbi3P ??$$P         d$$$$$$$$P" <PF??? ..,,ccd$$$$c,`""??$$eec,_
//    -?F""""                ?$$$$$$$$$b$$$$$$$$$$$$$$$$$$$$$$$$$r."""""""
//                             `""""""""""""""""""""```       """``
// jgkim