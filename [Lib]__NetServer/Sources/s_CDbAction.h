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
	DWORD		m_dwClient;	 // 메시지를 보낸 클라이언트 번호, 결과는다시 클라이언트 번호로 전송되어져야 한다.
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
// Session 서버 : 캐릭터 생성숫자 1 감소
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
// Agent 서버 : 캐릭터 생성숫자 1 감소
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
// TEST Session 서버 : 캐릭터 생성숫자 1 감소
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
// TEST Agent 서버 : 캐릭터 생성숫자 1 감소
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
// Session : 생성할 수 있는 캐릭터 수 1 증가
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
// Agent : 생성할 수 있는 캐릭터 수 1 증가
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
// TEST Session : 생성할 수 있는 캐릭터 수 1 증가
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
// TEST Agent : 생성할 수 있는 캐릭터 수 1 증가
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
// 일본 Agent : 로그인 처리할 때 WhiteRock System과 통신하는 부분 추가
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
// 일본 Agent : 로그아웃 처리할 때 WhiteRock System과 통신하는 부분 추가
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
 * 사용자 체크
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
		int nRandomNum, // 랜덤패스워드 번호
		int nSvrGrp, // 서버그룹
		int nSvrNum, // 서버번호
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
 * 사용자 체크
 * Thailand (태국)
 */
class CAgentThaiUserCheck : public CDbAction
{
public:
	CAgentThaiUserCheck (const char* szUserID, // ID 
			             const char* szPasswd, // PWD
			             const char* szUserIP, // IP
			             int nSvrGrp,          // 서버그룹
			             int nSvrNum,          // 서버번호
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
* Daum 사용자 체크
*/

//class CAgentDaumUserCheck : public CDbAction
//{
//public:
//	/**
//	* Daum 사용자 체크
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
//	DWORD m_dwClientNum; // Agent 에서의 Client 번호
//};

/**
 * Daum 사용자 체크
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};


/**
 * TERRA : 사용자 로그인
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};

/**
 * GSP : 사용자 로그인
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};

/**
* Excite 사용자 체크
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};

/**
* 일본 Gonzo 사용자 체크
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};

/**
 * \ingroup NetServerLib
 * 사용자 체크
 * Taiwan / Hongkong
 */
class CAgentGsUserCheck : public CDbAction
{
public:
	CAgentGsUserCheck(
		const TCHAR* szUserID, // ID 
		const TCHAR* szPasswd, // PWD
		const TCHAR* szUserIP, // IP
		int nSvrGrp, // 서버그룹
		int nSvrNum, // 서버번호
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
 * Daum : 사용자 Password Check
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};


/**
 * TERRA : 사용자 Password Check
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};


/**
 * EXCITE : 사용자 Password Check
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
	DWORD m_dwClientNum; // Agent 에서의 Client 번호
};

///////////////////////////////////////////////////////////////////////////////
// 일반 : 사용자 로그아웃
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

// 캐릭터 생성
class CCreateNewCharacter : public CDbAction
{
public:
    CCreateNewCharacter(int nIndex, // 캐릭터 인덱스
                        DWORD dwUserNum, // 사용자번호
                        DWORD dwSvrGrp, // 서버그룹번호
                        CString strChaName, // 캐릭터이름
                        WORD wSchool, // 캐릭터학교
                        WORD wHair, // 캐릭터 머리모양
                        WORD wFace, // 캐릭터 얼굴모양
						WORD wHairColor, // 헤어 컬러
						WORD wSex, // 성별
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

// 캐릭터 삭제
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
// 캐릭터를 온라인 상태로 만든다.
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
// 캐릭터를 오프라인 상태로 만든다.
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
* DAUM:캐릭터 삭제
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
* TERRA:캐릭터 삭제
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
* GSP:캐릭터 삭제
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
* EXCITE:캐릭터 삭제
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
* JAPAN:캐릭터 삭제
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
* 사용자를 일정시간 블럭 시킨다.
* nUserNum : 사용자번호
* nDay : 블럭 시킬 기간(일)
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
// 란 선물펑펑!! 이벤트 
// 리턴값 :
// 1  1 등 P-4 PC
// 2  2 등 효리폰
// 3  3 등 SONY 디카
// 4  4 등 아이리버 MP 플레이어
// 5  5 등 MAX 무비 영화 상품권 1 만원권
// 6  6 등 MBC Game 일일 무료 상품권
// 7  이미 복권을 입력한 사용자
// 8  PC 방 IP 가 아님
// 9  이미 사용한 복권
// 10 알수없는 오류
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
// 선물펑펑 이벤트 기록을 남긴다.
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
// 여름방학 PC 프로모션 아이템 '펑펑' 이벤트
// PC 방에서 하루에 한사람이 한번만 가능
// 리턴값
// 1 : 이벤트 응모가능
// 2 : 이미 이벤트에 응모했음, 하루에 한번만 가능.
// 3 : 응모불가 PC 방 IP 아님
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