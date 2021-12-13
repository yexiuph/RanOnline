#ifndef S_DBACTION_H_
#define S_DBACTION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <queue>

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_COdbcManager.h"
#include "s_CServer.h"
#include "s_CWhiteRock.h"
// #include "s_CSessionServer.h"
// #include "s_CFieldServer.h"
// #include "s_CAgentServer.h"

class CDbExecuter;
class CDbmanager;

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
 * \date 2005-05-31
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

/**
 * class CDbAction
 */
class CDbAction 
{
public:
	CDbAction( DWORD dwClient = -1, USHORT m_uPort = 0, GLDBMan* m_pDbManager = NULL )
		: m_dwClient( dwClient )
		, m_uPort( m_uPort )
		, m_pDbManager( m_pDbManager )
	{
	}

	virtual ~CDbAction() {};

	void SetDbManager ( GLDBMan *pDbMan )
	{
		m_pDbManager = (GLDBMan*) pDbMan;
	}

	virtual int Execute(CServer* m_pServer) { return 0; };

protected:
	DWORD		m_dwClient;	 // �޽����� ���� Ŭ���̾�Ʈ ��ȣ, ����´ٽ� Ŭ���̾�Ʈ ��ȣ�� ���۵Ǿ����� �Ѵ�.
	CString		m_strUserIP; // IP Address
	USHORT		m_uPort;

	GLDBMan*	m_pDbManager;
};

///////////////////////////////////////////////////////////////////////////////
// class CDbExecuter
///////////////////////////////////////////////////////////////////////////////
class CDbExecuter : public CLock
{
public:
	static CDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CDbExecuter();
	CDbExecuter(CServer* pServer);
	virtual ~CDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	void SetServer(CServer* pServer);
	void AddJob(CDbAction* t);
	void ExecuteJob();
	void ExecuteJobLast();
};

class CLogDbExecuter : public CLock
{
public:
	static CLogDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CLogDbExecuter();
	CLogDbExecuter(CServer* pServer);
	virtual ~CLogDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	void SetServer(CServer* pServer);
	void AddJob(CDbAction* t);
	void ExecuteJob();
	void ExecuteJobLast();
};

class CUserDbExecuter : public CLock
{
public:
	static CUserDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CUserDbExecuter();
	CUserDbExecuter(CServer* pServer);
	virtual ~CUserDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	void SetServer(CServer* pServer);
	void AddJob(CDbAction* t);
	void ExecuteJob();
	void ExecuteJobLast();
};

class CWebDbExecuter : public CLock
{
public:
	static CWebDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CWebDbExecuter();
	CWebDbExecuter(CServer* pServer);
	virtual ~CWebDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	void SetServer(CServer* pServer);
	void AddJob(CDbAction* t);
	void ExecuteJob();
	void ExecuteJobLast();
};

/******************************************************************************
* Session Server Database Action
******************************************************************************/
class CAddLogServerState : public CDbAction
{
public:
	CAddLogServerState(int nSGNum, int nSvrNum, int nUserNum, int nUserMax);
	virtual ~CAddLogServerState() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nSGNum;
	int m_nSvrNum;
	int m_nUserNum;
	int m_nUserMax;
};

///////////////////////////////////////////////////////////////////////////////
// Session ���� : ĳ���� �������� 1 ����
class CUserChaNumDecrease : public CDbAction
{
public:
	CUserChaNumDecrease(int nUserNum);
	virtual ~CUserChaNumDecrease() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// Agent ���� : ĳ���� �������� 1 ����
class CAgentUserChaNumDecrease : public CDbAction
{
public:
	CAgentUserChaNumDecrease(int nUserNum);
	virtual ~CAgentUserChaNumDecrease() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// TEST Session ���� : ĳ���� �������� 1 ����
class CUserTestChaNumDecrease : public CDbAction
{
public:
	CUserTestChaNumDecrease(int nUserNum);
	virtual ~CUserTestChaNumDecrease() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// TEST Agent ���� : ĳ���� �������� 1 ����
class CAgentUserTestChaNumDecrease : public CDbAction
{
public:
	CAgentUserTestChaNumDecrease(int nUserNum);
	virtual ~CAgentUserTestChaNumDecrease() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// Session : ������ �� �ִ� ĳ���� �� 1 ����
class CUserChaNumIncrease : public CDbAction
{
public:
	CUserChaNumIncrease(int nUserNum);
	virtual ~CUserChaNumIncrease() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// Agent : ������ �� �ִ� ĳ���� �� 1 ����
class CAgentUserChaNumIncrease : public CDbAction
{
public:
	CAgentUserChaNumIncrease(int nUserNum);
	virtual ~CAgentUserChaNumIncrease() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};


///////////////////////////////////////////////////////////////////////////////
// TEST Session : ������ �� �ִ� ĳ���� �� 1 ����
class CUserTestChaNumIncrease : public CDbAction
{
public:
	CUserTestChaNumIncrease(int nUserNum);
	virtual ~CUserTestChaNumIncrease() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

///////////////////////////////////////////////////////////////////////////////
// TEST Agent : ������ �� �ִ� ĳ���� �� 1 ����
class CAgentUserTestChaNumIncrease : public CDbAction
{
public:
	CAgentUserTestChaNumIncrease(int nUserNum);
	virtual ~CAgentUserTestChaNumIncrease() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

class CUserUpdateCha : public CDbAction
{
public:
	CUserUpdateCha(int nUserNum, const char* szCharName);
	virtual ~CUserUpdateCha() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	CString m_strCharName;
};


///////////////////////////////////////////////////////////////////////////////
// �Ϻ� Agent : �α��� ó���� �� WhiteRock System�� ����ϴ� �κ� �߰�
class CAgentJapnaWRLogin : public CDbAction
{
public:
	CAgentJapnaWRLogin(
						const char* szUserID,
						const char* szPassword,
						const char* szIP,
						int	nServerGroup,
						int nServerNum,
						DWORD dwClientNum 
						);
	virtual ~CAgentJapnaWRLogin() {};
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strPassword;
	CString m_strIP;
	int		m_nServerGroup;
	int		m_nServerNum;
	DWORD	m_dwClientNum;
};

///////////////////////////////////////////////////////////////////////////////
// �Ϻ� Agent : �α׾ƿ� ó���� �� WhiteRock System�� ����ϴ� �κ� �߰�
class CAgentJapnaWRLogout : public CDbAction
{
public:
	CAgentJapnaWRLogout(
				const char* szUserUUID
						);
	virtual ~CAgentJapnaWRLogout() {};
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserUUID;
};


class CUserLogoutSimple : public CDbAction
{
public:
	CUserLogoutSimple(CString strUserID);
	virtual ~CUserLogoutSimple() {};
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
};

class CUserLogoutSimple2 : public CDbAction
{
public:
	CUserLogoutSimple2(int nUserNum);
	virtual ~CUserLogoutSimple2() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
};

class CUserLogout : public CDbAction
{
public:
	CUserLogout(const char* szUserID, 
			 int nUserNum, 
			 int nGameTime,
			 int nChaNum,
			 int nSvrGrp,
			 int nSvrNum,
			 int nTotalGameTime,
			 int nOfflineTime );
	virtual ~CUserLogout() {} ;
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	int m_nUserNum;
	int m_nGameTime;
	int m_nChaNum;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nTotalGameTime;
	int m_nOfflineTime;
};

/**
 * \ingroup NetServerLib
 * ����� üũ
 * Taiwan / Hongkong
 */
class CAgentUserCheck : public CDbAction
{
public:
	CAgentUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
		const TCHAR* szUserIP, // IP
		const TCHAR* szRandomPasswd, // Random password
		int nRandomNum, // �����н����� ��ȣ
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentUserCheck() {} ;
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strPasswd;
	CString m_strRandomPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nRandomNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};

/**
 * \ingroup NetServerLib
 * ����� üũ
 * Thailand (�±�)
 */
class CAgentThaiUserCheck : public CDbAction
{
public:
	CAgentThaiUserCheck (const char* szUserID, // ID 
			             const char* szPasswd, // PWD
			             const char* szUserIP, // IP
			             int nSvrGrp,          // �����׷�
			             int nSvrNum,          // ������ȣ
			             DWORD dwClient,
			             DWORD dwClientNum);
	virtual ~CAgentThaiUserCheck () {} ;
	virtual int Execute (CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strPasswd;	
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
};


/**
* Daum ����� üũ
*/

//class CAgentDaumUserCheck : public CDbAction
//{
//public:
//	/**
//	* Daum ����� üũ
//	* \param szDaumGID 
//	* \param szDaumUID 
//	* \param szDaumSSNHEAD 
//	* \param szDaumSEX 
//	* \param szUserIP 
//	* \param nSvrGrp 
//	* \param nSvrNum 
//	* \param dwClient 
//	* \param dwClientNum 
//	* \return 
//	*/
//	CAgentDaumUserCheck(const char* szDaumGID, 
//		                const char* szDaumUID,
//				        const char* szDaumSSNHEAD,
//				        const char* szDaumSEX,
//				        const char* szUserIP, 
//				        int nSvrGrp, 
//				        int nSvrNum,				   
//				        DWORD dwClient,
//				        DWORD dwClientNum);
//	virtual ~CAgentDaumUserCheck() {} ;
//	virtual int Execute(CServer* pServer);
//
//protected:
//	CString m_strDaumGID;
//	CString m_strDaumUID;
//	CString m_strDaumSSNHEAD;
//	CString m_strDaumSEX;
//	CString m_strUserIP;
//	int m_nSvrGrp;
//	int m_nSvrNum;
//	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
//};

/**
 * Daum ����� üũ
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentDaumUserCheck : public CDbAction
{
public:
	CAgentDaumUserCheck(
		const TCHAR* szUUID,
		const TCHAR* szUserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentDaumUserCheck() {} ;
	virtual int Execute( CServer* pServer );

protected:
	CString m_strUUID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};


/**
 * TERRA : ����� �α���
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentTerraUserCheck : public CDbAction
{
public:
	CAgentTerraUserCheck(
		const TCHAR* szDecodedTID,
		const TCHAR* szUserIP, 
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentTerraUserCheck() {} ;
	virtual int Execute( CServer* pServer );

protected:
	CString m_strDecodedTID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};

/**
 * GSP : ����� �α���
 * \param szDecodedTID 
 * \param szUserIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentGspUserCheck : public CDbAction
{
public:
	CAgentGspUserCheck(
		const TCHAR* szUUID,
		const TCHAR* szUserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentGspUserCheck() {} ;
	virtual int Execute( CServer* pServer );

protected:
	CString m_strUUID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};

/**
* Excite ����� üũ
*/
class CAgentExciteUserCheck : public CDbAction
{
public:	
	CAgentExciteUserCheck(const char* szUserID,
				        const char* szUserIP,
				        int nSvrGrp,
				        int nSvrNum,
				        DWORD dwClient,
				        DWORD dwClientNum);
	virtual ~CAgentExciteUserCheck() {} ;
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};

/**
* �Ϻ� Gonzo ����� üũ
*/
class CAgentJapanUserCheck : public CDbAction
{
public:	
	CAgentJapanUserCheck(const char* szUserID,
				        const char* szUserIP,
				        int nSvrGrp,
				        int nSvrNum,
				        DWORD dwClient,
				        DWORD dwClientNum,
						int nUserNum);
	virtual ~CAgentJapanUserCheck() {} ;
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strUserIP;
	int m_nSvrGrp;
	int m_nSvrNum;
	int m_nUserNum;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};

/**
 * \ingroup NetServerLib
 * ����� üũ
 * Taiwan / Hongkong
 */
class CAgentGsUserCheck : public CDbAction
{
public:
	CAgentGsUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
		const TCHAR* szUserIP, // IP
		int nSvrGrp, // �����׷�
		int nSvrNum, // ������ȣ
		DWORD dwClient,
		DWORD dwClientNum,
		DWORD dwServiceProvider );
	virtual ~CAgentGsUserCheck() {} ;
	virtual int Execute(CServer* pServer);

protected:
	CString m_strUserID;
	CString m_strPasswd;
	int m_nSvrGrp;
	int m_nSvrNum;
	DWORD m_dwClientNum;
	DWORD m_dwServiceProvider;
};

/**
 * Daum : ����� Password Check
 * \param szDaumGID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentDaumPassCheck : public CDbAction
{
public:
	CAgentDaumPassCheck(
		const TCHAR* szDaumGID,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentDaumPassCheck() {};
	virtual int Execute( CServer* pServer );

protected:
	CString m_strDaumGID;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};


/**
 * TERRA : ����� Password Check
 * \param szTLoginName
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentTerraPassCheck : public CDbAction
{
public:
	CAgentTerraPassCheck(
		const TCHAR* szTLoginName,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentTerraPassCheck() {};
	virtual int Execute( CServer* pServer );

protected:
	CString m_strTLoginName;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};


/**
 * EXCITE : ����� Password Check
 * \param szExciteUserID
 * \param szUserPass
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
class CAgentExcitePassCheck : public CDbAction
{
public:
	CAgentExcitePassCheck(
		const TCHAR* szExciteUserID,
		const TCHAR* szExciteUserID2,
		const TCHAR* szUserPass,
		int nCheckFlag,
		DWORD dwClient,
		DWORD dwClientNum );
	virtual ~CAgentExcitePassCheck() {};
	virtual int Execute( CServer* pServer );

protected:
	CString m_strExciteUserID;
	CString m_strExciteUserID2;
	CString m_strUserPass;
	int		m_nCheckFlag;
	DWORD m_dwClientNum; // Agent ������ Client ��ȣ
};

///////////////////////////////////////////////////////////////////////////////
// �Ϲ� : ����� �α׾ƿ�
class CUserLogoutSvr : public CDbAction
{
public:
	CUserLogoutSvr(int nSvrGrp, int nSvrNum);
	virtual ~CUserLogoutSvr() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nSvrGrp;
	int m_nSvrNum;
};

///////////////////////////////////////////////////////////////////////////////
// Agent Server Database Action
///////////////////////////////////////////////////////////////////////////////

// ĳ���� ����
class CCreateNewCharacter : public CDbAction
{
public:
    CCreateNewCharacter(int nIndex, // ĳ���� �ε���
                        DWORD dwUserNum, // ����ڹ�ȣ
                        DWORD dwSvrGrp, // �����׷��ȣ
                        CString strChaName, // ĳ�����̸�
                        WORD wSchool, // ĳ�����б�
                        WORD wHair, // ĳ���� �Ӹ����
                        WORD wFace, // ĳ���� �󱼸��
						WORD wHairColor, // ��� �÷�
						WORD wSex, // ����
                        DWORD dwClient, 
                        const char* szUserIP, 
                        USHORT uPort=0);
	virtual ~CCreateNewCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
    int     m_nIndex; 
    DWORD   m_dwUserNum;
    DWORD   m_dwSvrGrp;
    CString m_strChaName;
    WORD    m_wSchool;
    WORD    m_wHair;
    WORD    m_wFace;
	WORD	m_wHairColor;
	WORD	m_wSex;
};

// ĳ���� ����
class CDelCharacter : public CDbAction
{
public:
	CDelCharacter(int nUserNum, 
                  int nChaNum, 
                  const char* szPass2, 
                  DWORD dwClient, 
                  const char* szUserIP, 
                  USHORT uPort=0);
	virtual ~CDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
	CString m_strPass2;
};

///////////////////////////////////////////////////////////////////////////
// ĳ���͸� �¶��� ���·� �����.
class CSetCharacterOnline : public CDbAction
{
public:
	CSetCharacterOnline(int nChaNum);
	virtual ~CSetCharacterOnline() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};

///////////////////////////////////////////////////////////////////////////
// ĳ���͸� �������� ���·� �����.
class CSetCharacterOffline : public CDbAction
{
public:
	CSetCharacterOffline(int nChaNum);
	virtual ~CSetCharacterOffline() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};

/**
* DAUM:ĳ���� ����
*/
class CDaumDelCharacter : public CDbAction
{
public:
	CDaumDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CDaumDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
};

/**
* TERRA:ĳ���� ����
*/
class CTerraDelCharacter : public CDbAction
{
public:
	CTerraDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CTerraDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
};

/**
* GSP:ĳ���� ����
*/
class CGspDelCharacter : public CDbAction
{
public:
	CGspDelCharacter(
		int nUserNum,
		int nChaNum,
		DWORD dwClient,
		const TCHAR* szUserIP,
		USHORT uPort=0 );
	virtual ~CGspDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
};

/**
* EXCITE:ĳ���� ����
*/
class CExciteDelCharacter : public CDbAction
{
public:
	CExciteDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CExciteDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
};

/**
* JAPAN:ĳ���� ����
*/
class CJapanDelCharacter : public CDbAction
{
public:
	CJapanDelCharacter(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CJapanDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
};

class CGsDelCharacter : public CDbAction
{
public:
	CGsDelCharacter(int nUserNum, 
                  int nChaNum, 
                  const char* szPass2, 
                  DWORD dwClient, 
                  const char* szUserIP, 
                  USHORT uPort=0);
	virtual ~CGsDelCharacter() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
	CString m_strPass2;
};

class CGetChaBAInfo : public CDbAction
{
public:
	CGetChaBAInfo(int nUserNum, int nSvrGrp, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CGetChaBAInfo() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nSvrGrp;
	NET_CHA_BBA_INFO m_msg;
};

class CGetChaBInfo : public CDbAction
{
public:
	CGetChaBInfo(int nUserNum, int nChaNum, DWORD dwClient, const char* szUserIP, USHORT uPort=0);
	virtual ~CGetChaBInfo() {} ;
	virtual int Execute(CServer* pServer);

protected:
	int m_nUserNum;
	int m_nChaNum;
	GLMSG::SNETLOBBY_CHARINFO m_msg;
};

/*
class CGetChaFriend : public CDbAction
{
public:
	CGetChaFriend(
		int nChaNum,
		DWORD dwClient,
		const char* szUserIP,
		USHORT uPort=0 );
	virtual ~CGetChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};
*/

/*
class CGetChaFriendSMS : public CDbAction
{
public:
	CGetChaFriendSMS(
		int nChaNum,
		DWORD dwClient,
		const char* szUserIP,
		USHORT uPort=0 );
	virtual ~CGetChaFriend() {};
	virtual int Execute(CServer* pServer);

protected:
	int m_nChaNum;
};
*/

class CGetChaInfoAndJoin : public CDbAction
{
protected:
	DWORD			m_dwClientID;
	GLCHARAG_DATA	m_cCharAgData;
    __time64_t      m_tPremiumDate;
	__time64_t      m_tChatBlock;

public:
	void Init ( DWORD dwClientID, DWORD dwUserID, DWORD dwUserLvl, __time64_t tPREMIUM, __time64_t tCHATBLOCK, DWORD dwServerID, DWORD dwCharID );
	virtual int Execute ( CServer* pServer );

public:
	CGetChaInfoAndJoin () :
		m_dwClientID(UINT_MAX)
	{
	}
};

/**
* ����ڸ� �����ð� �� ��Ų��.
* nUserNum : ����ڹ�ȣ
* nDay : �� ��ų �Ⱓ(��)
*/
class CUserBlock : public CDbAction
{
public:
	CUserBlock( int nUserNum, int nDay );
	virtual ~CUserBlock() {};	
	virtual int Execute( CServer* pServer );

protected:
    int m_nUserNum;
	int m_nDay;
};

class CUserBlockDate : public CDbAction
{
public:
	CUserBlockDate( int nUserNum, __time64_t tBlockTime );
	virtual ~CUserBlockDate() {};
	virtual int Execute( CServer* pServer );
protected:
	int m_nUserNum;
	__time64_t m_Date;
};

///////////////////////////////////////////////////////////////////////////////
// �� ��������!! �̺�Ʈ 
// ���ϰ� :
// 1  1 �� P-4 PC
// 2  2 �� ȿ����
// 3  3 �� SONY ��ī
// 4  4 �� ���̸��� MP �÷��̾�
// 5  5 �� MAX ���� ��ȭ ��ǰ�� 1 ������
// 6  6 �� MBC Game ���� ���� ��ǰ��
// 7  �̹� ������ �Է��� �����
// 8  PC �� IP �� �ƴ�
// 9  �̹� ����� ����
// 10 �˼����� ����
class CLogLottery : public CDbAction
{
protected:
	CString m_strLottery;
	CString m_strUserUID;
	int     m_nUserNum;

public:
	CLogLottery(CString strLottery,
				CString strUserUID, 
				int nUserNum,
				DWORD dwClient,
				const char* szUserIP,
				USHORT uPort=0);
    virtual ~CLogLottery() {};
	virtual int Execute(CServer* pServer);
};

///////////////////////////////////////////////////////////////////////////////
// �������� �̺�Ʈ ����� �����.
class CLogPungPungWrite : public CDbAction
{
protected:
    CString m_strUserUID;
    int     m_nUserNum;
    int     m_nPrize;
public:
    CLogPungPungWrite(CString strUserUID, int nUserNum, int nPrize, CString strIP);
    virtual ~CLogPungPungWrite() {};
    virtual int Execute(CServer* pServer);
};


///////////////////////////////////////////////////////////////////////////////
// �������� PC ���θ�� ������ '����' �̺�Ʈ
// PC �濡�� �Ϸ翡 �ѻ���� �ѹ��� ����
// ���ϰ�
// 1 : �̺�Ʈ ���𰡴�
// 2 : �̹� �̺�Ʈ�� ��������, �Ϸ翡 �ѹ��� ����.
// 3 : ����Ұ� PC �� IP �ƴ�
class CLogPungPungCheck : public CDbAction
{
protected:
    int m_nUserNum;
public:
    CLogPungPungCheck(int nUserNum, DWORD dwClient, CString strIP, USHORT uPort);
    virtual ~CLogPungPungCheck() {};
    virtual int Execute(CServer* pServer);
};

/******************************************************************************
** Field Server Database Action
******************************************************************************/
class CGetChaInfoAndJoinField : public CDbAction
{
protected:
	DWORD				m_dwClientID;
	SCHARDATA2			m_cCharData;


    __time64_t          m_tPremiumDate;
    __time64_t          m_tChatBlock;

	GLMSG::SNETJOIN2FIELDSERVER::SINFO m_sINFO;



public:
	void Init(	DWORD dwClientID, 
				DWORD dwUserID, 
				const char* szUID, 
				DWORD dwUserLvl, 
				__time64_t tPREMIUM, 
				__time64_t tCHATBLOCK,
				DWORD dwServerID, 
				DWORD dwCharID,
				GLMSG::SNETJOIN2FIELDSERVER::SINFO sINFO );

	/*void Init(	DWORD dwClientID, 
		DWORD dwUserID, 
		const char* szUID, 
		DWORD dwUserLvl, 
		__time64_t tPREMIUM, 
		__time64_t tCHATBLOCK,
		BOOL bUseArmSub,
		DWORD dwServerID, 
		DWORD dwCharID,
		DWORD dwThaiCCafeClass,
		SChinaTime sChinaTime,
		SEventTime sEventTime,
		SVietnamGainSystem sVietnamGainSystem );
	void InitEx ( SNATIVEID nidMAP, DWORD dwGate, D3DXVECTOR3 vPos, EMGAME_JOINTYPE emTYPE, DWORD dwACTSTATE );*/

	virtual int Execute ( CServer* pServer );

public:
	CGetChaInfoAndJoinField () :
		m_dwClientID(UINT_MAX)
	{
	}
};

#endif // S_DBACTION_H_