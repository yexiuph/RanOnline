#include "pch.h"

#include "s_CDbAction.h"
#include "s_CSessionServer.h"
#include "s_CFieldServer.h"
#include "s_CAgentServer.h"

#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLCharData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
** Databse Action for User Database
******************************************************************************/

CAddLogServerState::CAddLogServerState(
	int nSGNum,
	int nSvrNum,
	int nUserNum,
	int nUserMax )
	: m_nSGNum( nSGNum )
	, m_nSvrNum( nSvrNum )
	, m_nUserNum( nUserNum )
	, m_nUserMax( nUserMax )
{	
}

int CAddLogServerState::Execute(
	CServer* pServer )
{
	return COdbcManager::GetInstance()->AddLogServerState(m_nSGNum, m_nSvrNum, m_nUserNum, m_nUserMax);
}

///////////////////////////////////////////////////////////////////////////////
// Session 서버 : 캐릭터 생성숫자 1 감소
CUserChaNumDecrease::CUserChaNumDecrease(int nUserNum) 
{ 
	m_nUserNum = nUserNum;
}

int CUserChaNumDecrease::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// Agent 서버 : 캐릭터 생성숫자 1 감소
CAgentUserChaNumDecrease::CAgentUserChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserChaNumDecrease::Execute(CServer* pServer)
{
    return COdbcManager::GetInstance()->UserChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Session 서버 : 캐릭터 생성숫자 1 감소
CUserTestChaNumDecrease::CUserTestChaNumDecrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumDecrease::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent 서버 : 캐릭터 생성숫자 1 감소
CAgentUserTestChaNumDecrease::CAgentUserTestChaNumDecrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserTestChaNumDecrease::Execute(CServer* pServer)
{
    return COdbcManager::GetInstance()->UserTestChaNumDecrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// Session : 생성할 수 있는 캐릭터 수 1 증가
CUserChaNumIncrease::CUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserChaNumIncrease::Execute(CServer* pServer)
{
    return COdbcManager::GetInstance()->UserChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// Agent : 생성할 수 있는 캐릭터 수 1 증가
CAgentUserChaNumIncrease::CAgentUserChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CAgentUserChaNumIncrease::Execute(CServer* pServer)
{
    return COdbcManager::GetInstance()->UserChaNumIncrease(m_nUserNum);
}


///////////////////////////////////////////////////////////////////////////////
// TEST Session : 생성할 수 있는 캐릭터 수 1 증가
CUserTestChaNumIncrease::CUserTestChaNumIncrease(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserTestChaNumIncrease::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserTestChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
// TEST Agent : 생성할 수 있는 캐릭터 수 1 증가
CAgentUserTestChaNumIncrease::CAgentUserTestChaNumIncrease(int nUserNum)
{
    m_nUserNum = nUserNum;
}

int CAgentUserTestChaNumIncrease::Execute(CServer* pServer)
{
    return COdbcManager::GetInstance()->UserTestChaNumIncrease(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
//
CUserUpdateCha::CUserUpdateCha(int nUserNum, const char* szCharName)
{
	m_nUserNum = nUserNum;
	m_strCharName = szCharName;
    m_strCharName.Trim(_T(" "));
}

int CUserUpdateCha::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserUpdateCha(m_nUserNum, m_strCharName.GetString());
}

///////////////////////////////////////////////////////////////////////////////
// 일본 WhiteRock과의 로그인 통신
CAgentJapnaWRLogin::CAgentJapnaWRLogin( const char* szUserID,
										    const char* szPassword,
										    const char* szIP,
										    int	nServerGroup,
										    int nServerNum,
										    DWORD dwClientNum )
{
	m_strUserID		= szUserID;
	m_strPassword	= szPassword;
    m_strIP			= szIP;
	m_nServerGroup	= nServerGroup;
	m_nServerNum	= nServerNum;
	m_dwClientNum	= dwClientNum;
}

int CAgentJapnaWRLogin::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nResult = CWhiteRock::GetInstance()->SendLoginXML( m_strUserID, m_strPassword, m_strIP );

	if ( HTTP_STATUS_OK != nResult )
	{
		// WhiteRock 서버와 통신 실패
		pTemp->JapanWRFailed( m_dwClientNum, nResult );
	}
	else
	{
		const CString& strUUID   = CWhiteRock::GetInstance()->GetUUID();
		int nUserNum = CWhiteRock::GetInstance()->GetUserNum();
		// WhiteRock 서버와 통신 성공
		pTemp->JapanWRSuccess( m_dwClientNum, m_strUserID,
									m_nServerGroup, m_nServerNum, strUUID, nUserNum );
	}

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일본 WhiteRock과의 로그아웃 통신
CAgentJapnaWRLogout::CAgentJapnaWRLogout( const char* szUserUUID )
{
	m_strUserUUID	= szUserUUID;
}

int CAgentJapnaWRLogout::Execute(CServer* pServer)
{	
	CWhiteRock::GetInstance()->SendLogoutXML( m_strUserUUID );
	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogoutSimple::CUserLogoutSimple(CString strUserID)
{
	m_strUserID = strUserID;
}

int CUserLogoutSimple::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserLogoutSimple(m_strUserID);
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogoutSimple2::CUserLogoutSimple2(int nUserNum)
{
	m_nUserNum = nUserNum;
}

int CUserLogoutSimple2::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserLogoutSimple2(m_nUserNum);
}

///////////////////////////////////////////////////////////////////////////////
//
CUserLogout::CUserLogout(const char* szUserID, 
						int nUserNum, 
						int nGameTime,
						int nChaNum,
						int nSvrGrp,
						int nSvrNum, 
						int nTotalGameTime,
						int nOfflineTime )
{
	m_strUserID = szUserID;
	m_nUserNum = nUserNum;
	m_nGameTime = nGameTime;
	m_nChaNum = nChaNum;
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;
	m_nTotalGameTime = nTotalGameTime;
	m_nOfflineTime = nOfflineTime;
}

int CUserLogout::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserLogout(m_strUserID.GetString(),
													m_nUserNum,
													m_nGameTime,
													m_nChaNum,
													m_nSvrGrp,
													m_nSvrNum,
													m_nTotalGameTime,
													m_nOfflineTime );
}

/**
 * \ingroup NetServerLib
 * 사용자 체크
 * Thailand (태국)
 */
CAgentThaiUserCheck::CAgentThaiUserCheck (const char* szUserID,
	                                      const char* szPasswd, 
			                              const char* szUserIP, 
			                              int nSvrGrp, 
			                              int nSvrNum,
			                              DWORD dwClient,
			                              DWORD dwClientNum)
{
	m_dwClient  = dwClient;
	m_strUserID = szUserID;
	m_strPasswd = szPasswd;
	m_strUserIP = szUserIP;
	m_nSvrGrp   = nSvrGrp;
	m_nSvrNum   = nSvrNum;
	m_dwClientNum = dwClientNum;
}

int CAgentThaiUserCheck::Execute (CServer* pServer)
{
	if (pServer == NULL) return NET_ERROR;

	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);	

	int nRetCode = 0;	
	int nResult = COdbcManager::GetInstance()->ThaiUserCheck( m_strUserID.GetString(),
													          m_strPasswd.GetString(),
													          m_strUserIP.GetString(),
													          m_nSvrGrp,
													          m_nSvrNum );
	int nResult1 = COdbcManager::GetInstance()->ThaiUserClassCheck( m_strUserIP.GetString() );

	// 전송할 구조체를 세팅한다.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nmg.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient   = m_dwClientNum;

	StringCchCopy (NetMsgFB.szIp, MAX_IP_LENGTH+1, m_strUserIP.GetString()); // ip
	StringCchCopy (NetMsgFB.szUserid, USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	NetMsgFB.nThaiCC_Class = nResult1;
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 가져온다.
		nRetCode = COdbcManager::GetInstance()->ThaiGetUserInfo( m_strUserID.GetString(), &NetMsgFB );
		if (nRetCode == DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			COdbcManager::GetInstance()->UserLogoutSimple (m_strUserID.GetString());
			// COdbcManager::GetInstance()->UserLogoutSimple2(NetMsgFB.nUserNum);

			CConsoleMessage::GetInstance()->Write(
				_T("CAgentUserCheck GetUserInfo DB_ERROR") );
		}
		else
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_OK;
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		break;
	case 5 : // 중복접속
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_DUP;
		break;
	case 6 : // Block 된 상태
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_BLOCK;
		break;	
	case 7 : // 18 세미만 3 시간 경과
		NetMsgFB.nResult = EM_LOGIN_FB_THAI_UNDER18_3HOUR;
		break;
	case 8 : // 18 세미만 접속불가능 시간
		NetMsgFB.nResult = EM_LOGIN_FB_THAI_UNDER18_TIME;
		break;
	case 9 : // 18 세이상 접속불가능 시간
		NetMsgFB.nResult = EM_LOGIN_FB_THAI_OVER18_TIME;
		break;
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_FAIL;
		break;
	}

	pTemp->ThaiMsgLogInBack( &NetMsgFB );

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일반 : 사용자 로그인
CAgentUserCheck::CAgentUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szPasswd, 
	const TCHAR* szUserIP, 
	const TCHAR* szRandomPasswd, // Random password
	int nRandomNum,
	int nSvrGrp, 
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum,
	DWORD dwServiceProvider )
	: m_strUserID( szUserID )
	, m_strPasswd( szPasswd )	
	, m_strRandomPasswd( szRandomPasswd )
	, m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )

	, m_dwClientNum( dwClientNum )
	, m_nRandomNum( nRandomNum )
	, m_dwServiceProvider( dwServiceProvider )
{	
	m_dwClient = dwClient;
	m_strUserIP = szUserIP;
}

int CAgentUserCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nRetCode = 0;	
	int nResult = COdbcManager::GetInstance()->UserCheck(m_strUserID.GetString(),
													     m_strPasswd.GetString(),
													     m_strUserIP.GetString(),
														 m_strRandomPasswd.GetString(),
														 m_nRandomNum,
													     m_nSvrGrp,
													     m_nSvrNum);

	// 내부 서버에서 태국 User Class Type 체크를 위해서 추가
//	int nResult1 = COdbcManager::GetInstance()->ThaiUserClassCheck( m_strUserIP.GetString() );
/*
	// 말레이시아 PC방 이벤트
	int nResult1 = COdbcManager::GetInstance()->MyUserClassCheck( m_strUserIP.GetString() );
*/
	// 전송할 구조체를 세팅한다.
	NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nmg.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,		MAX_IP_LENGTH+1, m_strUserIP.GetString()); // ip
	StringCchCopy(NetMsgFB.szUserid,	USR_ID_LENGTH+1, m_strUserID.GetString()); // id

	// 내부 서버에서 태국 User Class Type 체크를 위해서 추가
//	NetMsgFB.nMyCC_Class = nResult1;
//	NetMsgFB.nThaiCC_Class = nResult1;
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 가져온다.
		nRetCode = COdbcManager::GetInstance()->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			COdbcManager::GetInstance()->UserLogoutSimple (m_strUserID.GetString());
			// COdbcManager::GetInstance()->UserLogoutSimple2(NetMsgFB.nUserNum);

			CConsoleMessage::GetInstance()->Write(
                _T("CAgentUserCheck GetUserInfo DB_ERROR") );
		}
		else
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_OK;
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	case 5 : // 중복접속
		// 사용자 정보를 가져온다.
		nRetCode = COdbcManager::GetInstance()->GetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			COdbcManager::GetInstance()->UserLogoutSimple (m_strUserID.GetString());

			CConsoleMessage::GetInstance()->Write(
				_T("CAgentUserCheck GetUserInfo DB_ERROR") );
		}else{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_DUP;
			CConsoleMessage::GetInstance()->Write(_T("CAgentUserCheck result %d ClientNum %d LoginID %s UserIp %s"), 
				nResult, m_dwClientNum, m_strUserID.GetString(), m_strUserIP.GetString() );
		}
		break;
	case 6 : // Block 된 상태
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	case 7 : // EM_LOGIN_FB_SUB_RANDOM_PASS
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_RANDOM_PASS;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write("CAgentUserCheck result %d", nResult);
		break;
	}
	
	if (m_dwServiceProvider == SP_CHINA)
		pTemp->ChinaMsgLoginBack( &NetMsgFB );
	else
        pTemp->MsgLogInBack( &NetMsgFB );

	return NET_OK;
}
/*
///////////////////////////////////////////////////////////////////////////////
// DAUM : 사용자 로그인
CAgentDaumUserCheck::CAgentDaumUserCheck(const char* szDaumGID, 
		                                 const char* szDaumUID,
				                         const char* szDaumSSNHEAD,
				                         const char* szDaumSEX,
				                         const char* szUserIP, 
				                         int nSvrGrp, 
				                         int nSvrNum,				   
				                         DWORD dwClient,
				                         DWORD dwClientNum)
{
	m_strDaumGID		= szDaumGID;
	m_strDaumUID		= szDaumUID;
	m_strDaumSSNHEAD	= szDaumSSNHEAD;
	m_strDaumSEX		= szDaumSEX;
	m_strUserIP			= szUserIP;
	m_nSvrGrp			= nSvrGrp;
	m_nSvrNum			= nSvrNum;		
	m_dwClient			= dwClient; 
	m_dwClientNum		= dwClientNum; // Agent 클라이언트 번호
}
	
int CAgentDaumUserCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) {
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck::Execute pTemp==NULL");
		return NET_ERROR;
	}

	int nResult = COdbcManager::GetInstance()->DaumUserCheck(m_strDaumGID.GetString(),
															 m_strDaumUID.GetString(),
															 m_strDaumSSNHEAD.GetString(),
															 m_strDaumSEX.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum);

	// 전송할 구조체를 세팅한다.
	DAUM_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = DAUM_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString());
	::StringCchCopy(nlfd2.szDaumUID, DAUM_MAX_UID_LENGTH+1, m_strDaumUID.GetString());
	::StringCchCopy(nlfd2.szIp,	     MAX_IP_LENGTH+1,	    m_strUserIP.GetString()); // ip	
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->DaumGetUserInfo(m_strDaumUID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}
*/
///////////////////////////////////////////////////////////////////////////////
// DAUM : 사용자 로그인
CAgentDaumUserCheck::CAgentDaumUserCheck(
		const TCHAR* szUUID,
		const TCHAR* szUserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum )
{
		m_strUUID = szUUID;
		m_strUserIP = szUserIP;
		m_nSvrGrp = nSvrGrp;
		m_nSvrNum = nSvrNum;
		m_dwClientNum = dwClientNum;	
}

int CAgentDaumUserCheck::Execute( CServer *pServer )
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	TCHAR szKorUserID[DAUM_MAX_GID_LENGTH+1] = {0};

	int nResult = COdbcManager::GetInstance()->DaumUserCheck(
		szKorUserID,
		m_strUUID.GetString(),
		m_strUserIP.GetString(),
		m_nSvrGrp,
		m_nSvrNum );

	DAUM_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = DAUM_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, szKorUserID);// gid
	::StringCchCopy(nlfd2.szIp,	     MAX_IP_LENGTH+1,	    m_strUserIP.GetString()); // ip	
	::StringCchCopy( nlfd2.szUUID, UUID_STR_LENGTH, m_strUUID.GetString() ); // UUID

	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->DaumGetUserInfo(szKorUserID, &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 20 : // Web 인증 시간 초과 
		nlfd2.nResult = EM_LOGIN_FB_SUB_EXPIRED;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 21 : // UUID가 web중복 로그인으로 인해 사라짐
		nlfd2.nResult = EM_LOGIN_FB_SUB_UID_ERR;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write("CAgentDaumUserCheck result %d", nResult);
		break;
	}

	pTemp->DaumMsgLoginBack(&nlfd2);
	return NET_OK;
}

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
CAgentGspUserCheck::CAgentGspUserCheck(
		const TCHAR* szUUID,
		const TCHAR* szUserIP,
		int nSvrGrp,
		int nSvrNum,
		DWORD dwClient,
		DWORD dwClientNum )
{
	m_strUUID = szUUID;
	m_strUserIP = szUserIP;
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;
	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentGspUserCheck::Execute( CServer* pServer )
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	TCHAR szGspUserID[GSP_USERID] = {0};

	int nResult = COdbcManager::GetInstance()->GspUserCheck(
		szGspUserID,
		m_strUUID.GetString(),
		m_strUserIP.GetString(),
		m_nSvrGrp,
		m_nSvrNum );

	// 전송할 구조체를 세팅한다.
	GSP_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = GSP_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy( nlfd2.szGspUserID, GSP_USERID, szGspUserID ); // ID	
	StringCchCopy( nlfd2.szIp, GSP_USERIP, m_strUserIP.GetString() ); // ip
	StringCchCopy( nlfd2.szUUID, UUID_STR_LENGTH, m_strUUID.GetString() ); // UUID
	
	switch (nResult)
	{
	/*
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"), 
			nResult);
		break;
	*/
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"),
			nResult );
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->GspGetUserInfo( szGspUserID, &nlfd2 );
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"),
			nResult );
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"),
			nResult );
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"),
			nResult );
		break;
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
	case GSP_ALLOW_TIME_ERROR:
		nlfd2.nResult = EM_LOGIN_FB_SUB_EXPIRED;
		CConsoleMessage::GetInstance()->Write(
			_T("CAgentGspUserCheck result %d"),
			nResult );
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	}

	pTemp->GspMsgLoginBack(&nlfd2);

	return NET_OK;
}

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
CAgentTerraUserCheck::CAgentTerraUserCheck(
	const TCHAR* szDecodedTID,
	const TCHAR* szUserIP, 
	int nSvrGrp,
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum )
{
	m_strDecodedTID = szDecodedTID;
	m_strUserIP     = szUserIP;
	m_nSvrGrp       = nSvrGrp;
	m_nSvrNum       = nSvrNum;
	m_dwClientNum   = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentTerraUserCheck::Execute(CServer* pServer)
{
    CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	char szTLoginName[TERRA_TLOGIN_NAME+1] = {0};

	int nResult = COdbcManager::GetInstance()->TerraUserCheck(szTLoginName,
		                                                      m_strDecodedTID.GetString(),
															  m_strUserIP.GetString(),
															  m_nSvrGrp,
															  m_nSvrNum);
	int nResult1 = COdbcManager::GetInstance()->MyUserClassCheck( m_strUserIP.GetString() );

	// 전송할 구조체를 세팅한다.
	TERRA_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = TERRA_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, szTLoginName); // TLoginName
	StringCchCopy(nlfd2.szTerraDecodedTID, TERRA_TID_DECODE+1, m_strDecodedTID.GetString()); // Decoded TID
	StringCchCopy(nlfd2.szIp,	           MAX_IP_LENGTH+1,	  m_strUserIP.GetString()); // ip

	nlfd2.nMyCC_Class = nResult1;
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s"),
											  nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString() );
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->TerraGetUserInfo(szTLoginName, &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
        COdbcManager::GetInstance()->TerraGetUserInfo(szTLoginName, &nlfd2);
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d ClientNum %d LoginName %s UserIp %s"), 
											  nResult, m_dwClientNum, szTLoginName, m_strUserIP.GetString() );
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	}

	pTemp->TerraMsgLoginBack(&nlfd2);

	return NET_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 일반 : GS 로그인
CAgentGsUserCheck::CAgentGsUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szPasswd, 
	const TCHAR* szUserIP, 
	int nSvrGrp, 
	int nSvrNum,
	DWORD dwClient,
	DWORD dwClientNum,
	DWORD dwServiceProvider )
	: m_strUserID( szUserID )
	, m_strPasswd( szPasswd )	
	, m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )
	, m_dwClientNum( dwClientNum )
	, m_dwServiceProvider( dwServiceProvider )
{	
	m_dwClient = dwClient;
	m_strUserIP = szUserIP;
}

int CAgentGsUserCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;

	int nRetCode = 0;	
	int nResult = COdbcManager::GetInstance()->GsUserCheck(m_strUserID.GetString(),
													     m_strPasswd.GetString(),
													     m_strUserIP.GetString(),
													     m_nSvrGrp,
													     m_nSvrNum);

	// 전송할 구조체를 세팅한다.
	GS_NET_LOGIN_FEEDBACK_DATA2 NetMsgFB;
	NetMsgFB.nmg.nType = NET_MSG_LOGIN_FB;
	NetMsgFB.nClient	= m_dwClientNum;

	StringCchCopy(NetMsgFB.szIp,		MAX_IP_LENGTH+1, m_strUserIP.GetString()); // ip
	StringCchCopy(NetMsgFB.szUserid,	USR_ID_LENGTH+1, m_strUserID.GetString()); // id


	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)	
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 가져온다.
		nRetCode = COdbcManager::GetInstance()->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			COdbcManager::GetInstance()->UserLogoutSimple (m_strUserID.GetString());
			// COdbcManager::GetInstance()->UserLogoutSimple2(NetMsgFB.nUserNum);

			CConsoleMessage::GetInstance()->Write(
                _T("CAgentGsUserCheck GetUserInfo DB_ERROR") );
		}
		else
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_OK;
		}
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	case 5 : // 중복접속
		// 사용자 정보를 가져온다.
		nRetCode = COdbcManager::GetInstance()->GsGetUserInfo(m_strUserID.GetString(), &NetMsgFB);
		if (nRetCode == DB_ERROR) // 가져오기 실패
		{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_SYSTEM;
			// 로그아웃 처리
			COdbcManager::GetInstance()->UserLogoutSimple (m_strUserID.GetString());

			CConsoleMessage::GetInstance()->Write(
				_T("CAgentGsUserCheck GetUserInfo DB_ERROR") );
		}else{
			NetMsgFB.nResult = EM_LOGIN_FB_SUB_DUP;
			CConsoleMessage::GetInstance()->Write(_T("CAgentGsUserCheck result %d ClientNum %d LoginID %s UserIp %s"), 
				nResult, m_dwClientNum, m_strUserID.GetString(), m_strUserIP.GetString() );
		}
		break;
	case 6 : // Block 된 상태
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	case 7 : // EM_LOGIN_FB_SUB_RANDOM_PASS
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_RANDOM_PASS;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	default : // 다른 모든경우 로그인 실패
		NetMsgFB.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write("CAgentGsUserCheck result %d", nResult);
		break;
	}
	
	pTemp->GsMsgLoginBack( &NetMsgFB );

	return NET_OK;
}


/**
 * Daum : 사용자 Password Check
 * \param szUserGID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentDaumPassCheck::CAgentDaumPassCheck(
			const TCHAR* szDaumGID,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strDaumGID	= szDaumGID;
	m_strUserPass	= szUserPass;
	m_nCheckFlag	= nCheckFlag;
	m_dwClientNum	= dwClientNum;
}

int CAgentDaumPassCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if( pTemp == NULL ) return NET_ERROR;

	int nResult = COdbcManager::GetInstance()->DaumUserPassCheck(	m_strDaumGID.GetString(), 
																	m_strUserPass.GetString(),
																	m_nCheckFlag);
		

	// 전송할 구조체를 세팅한다.
	DAUM_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	
	nlfd2.nClient = m_dwClientNum;
	StringCchCopy( nlfd2.szDaumGID, DAUM_MAX_GID_LENGTH+1, m_strDaumGID.GetString() );

	switch(nResult)
	{
	case DB_ERROR : // DB에러로 로그인 실패 (-1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentDaumPassCheck result %d"), nResult);
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentDaumUserCheck result %d"), nResult);
		break;
	case 2 : // PW 입력 실패 (2) 
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentDaumPassCheck result %d"), nResult);
		break;
	}

	pTemp->DaumMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
 * TERRA : 사용자 Password Check
 * \param szTLoginName
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentTerraPassCheck::CAgentTerraPassCheck(
			const TCHAR* szTLoginName,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strTLoginName		= szTLoginName;
	m_strUserPass		= szUserPass;
	m_nCheckFlag		= nCheckFlag;
	m_dwClientNum		= dwClientNum;
}

int CAgentTerraPassCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if( pTemp == NULL ) return NET_ERROR;

	int nResult = COdbcManager::GetInstance()->TerraUserPassCheck( m_strTLoginName.GetString(),
																   m_strUserPass.GetString(),
																   m_nCheckFlag );

	// 전송할 구조체를 세팅한다.
	TERRA_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szTerraTLoginName, TERRA_TLOGIN_NAME+1, m_strTLoginName.GetString()); // TLoginName
	
	switch(nResult)
	{
	case DB_ERROR : // DB에러로 패스워드 업데이트 실패 (-1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraPassCheck result %d"), nResult);
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraUserCheck result %d"), nResult);
		break;
	case 2 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentTerraPassCheck result %d"), nResult);
		break;
	}

	pTemp->TerraMsgPassCheckBack(&nlfd2);

	return NET_OK;
}


/**
* Excite 사용자 체크
*/
CAgentExciteUserCheck::CAgentExciteUserCheck(const char* szUserID,
				                             const char* szUserIP,
				                             int nSvrGrp,
				                             int nSvrNum,
				                             DWORD dwClient,
				                             DWORD dwClientNum)
{
	m_strUserID = szUserID;
	m_strUserIP = szUserIP;
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;

	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
}

int CAgentExciteUserCheck::Execute(CServer* pServer)
{	
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;	

	int nResult = COdbcManager::GetInstance()->ExciteUserCheck(m_strUserID.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum);
	// 전송할 구조체를 세팅한다.
	EXCITE_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = EXCITE_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		// 중복된 접속이라도 유저 정보를 가져와서 셋팅한다.
		COdbcManager::GetInstance()->ExciteGetUserInfo(m_strUserID.GetString(), &nlfd2);
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	}

	pTemp->ExciteMsgLoginBack(&nlfd2);
	
	return NET_OK;
}


/**
* 일본 Gonzo 사용자 체크
*/
CAgentJapanUserCheck::CAgentJapanUserCheck(const char* szUserID,
				                             const char* szUserIP,
				                             int nSvrGrp,
				                             int nSvrNum,
				                             DWORD dwClient,
				                             DWORD dwClientNum,
											 int nUserNum)
{
	m_strUserID = szUserID;
	m_strUserIP = szUserIP;
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;

	m_dwClientNum = dwClientNum; // Agent 에서의 Client 번호
	m_nUserNum = nUserNum;		// WhiteRock System에서의 UserNum
}

int CAgentJapanUserCheck::Execute(CServer* pServer)
{	
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if (pTemp == NULL) return NET_ERROR;	

	int nResult = COdbcManager::GetInstance()->JapanUserCheck(m_strUserID.GetString(),
															 m_strUserIP.GetString(),
															 m_nSvrGrp,
															 m_nSvrNum,
															 m_nUserNum);
	// 전송할 구조체를 세팅한다.
	JAPAN_NET_LOGIN_FEEDBACK_DATA2 nlfd2;
	nlfd2.nmg.nType = JAPAN_NET_MSG_LOGIN_FB;
	nlfd2.nClient	= m_dwClientNum;

	::StringCchCopy(nlfd2.JapanUserID,	  JAPAN_USER_ID+1, m_strUserID.GetString()); // id
	::StringCchCopy(nlfd2.szIp,	          MAX_IP_LENGTH+1,	m_strUserIP.GetString()); // ip
	
	switch (nResult)
	{
	case DB_ERROR : // 시스템에러로 로그인 실패 (-1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_SYSTEM;
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapanUserCheck result %d"), nResult);
		break;
	case 0 : // ID / PWD 불일치 (새로운 계정을 생성 또는 재입력요구)
		nlfd2.nResult = EM_LOGIN_FB_SUB_INCORRECT;
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapaneUserCheck result %d"), nResult);
		break;
	case 1 :
	case 2 : // ID/PWD 가 일치하고, IP 가 사용가능	
	case 3 : // ID/PWD 가 일치하고, IP 정보는 없음 (일반적으로 로그인 성공)
		// 사용자 정보를 세팅한다.
		nlfd2.nResult      = EM_LOGIN_FB_SUB_OK;
        COdbcManager::GetInstance()->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);
		break;
	case 4 : // ID/PWD 가 일치하지만 IP 가 사용불가능 상태(접근 차단된 IP)
		nlfd2.nResult = EM_LOGIN_FB_SUB_IP_BAN;
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapanUserCheck result %d"), nResult);
		break;
	case 5 : // 중복접속
		nlfd2.nResult = EM_LOGIN_FB_SUB_DUP;
		// 중복된 접속이라도 유저 정보를 가져와서 셋팅한다.
		COdbcManager::GetInstance()->JapanGetUserInfo(m_strUserID.GetString(), &nlfd2);
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapanUserCheck result %d"), nResult);
		break;
	case 6 : // Block 된 상태
		nlfd2.nResult = EM_LOGIN_FB_SUB_BLOCK;
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapanUserCheck result %d"), nResult);
		break;
	case 7 : // 유저가 존재합니다. 유저 삽입실패
	default : // 다른 모든경우 로그인 실패
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentJapanUserCheck result %d"), nResult);
		break;
	}

	pTemp->JapanMsgLoginBack(&nlfd2);
	
	return NET_OK;
}

/**
 * EXCITE : 사용자 Password Check
 * \param szExciteUserID
 * \param szUserPass 
 * \param dwClient 
 * \param dwClientNum 
 * \return 
 */
CAgentExcitePassCheck::CAgentExcitePassCheck(
			const TCHAR* szExciteUserID,
			const TCHAR* szExciteUserID2,
			const TCHAR* szUserPass,
			int nCheckFlag,
			DWORD dwClient,
			DWORD dwClientNum )

{
	m_strExciteUserID	= szExciteUserID;
	m_strExciteUserID2	= szExciteUserID2;
	m_strUserPass		= szUserPass;
	m_nCheckFlag		= nCheckFlag;
	m_dwClientNum		= dwClientNum;
}

int CAgentExcitePassCheck::Execute(CServer* pServer)
{
	CAgentServer* pTemp = reinterpret_cast<CAgentServer*> (pServer);

	if( pTemp == NULL ) return NET_ERROR;

	int len		= m_strExciteUserID2.GetLength();
	int len1	= m_strUserPass.GetLength();

	for( int i = 0; i < len; i++ )
	{
		m_strExciteUserID2.SetAt(i, m_strExciteUserID2.GetAt(i) + 1);
	}

	for(int i = 0; i < len1; i++ )
	{
		m_strUserPass.SetAt(i, m_strUserPass.GetAt(i) + 1);
	}

	int nResult = COdbcManager::GetInstance()->ExciteUserPassCheck( m_strExciteUserID.GetString(),
									  m_strExciteUserID2.GetString(),
									  m_strUserPass.GetString(),
									  m_nCheckFlag );

	// 전송할 구조체를 세팅한다.
	EXCITE_NET_PASSCHECK_FEEDBACK_DATA2 nlfd2;
	nlfd2.nClient	= m_dwClientNum;	

	StringCchCopy(nlfd2.szExciteUserID, EXCITE_USER_ID+1, m_strExciteUserID.GetString()); // UserID
	
	switch(nResult)
	{
	case DB_ERROR : // DB에러로 패스워드 업데이트 실패 (-1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExcitePassCheck result %d"), nResult);
		break;	
	case 0 :
		// 사용자 정보를 세팅한다. (0)
		nlfd2.nResult = EM_LOGIN_FB_SUB_OK;
		break;
	case 1 : // ID / PWD 불일치 (로그인 실패) (1)
		nlfd2.nResult = EM_LOGIN_FB_SUB_FAIL;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExciteUserCheck result %d"), nResult);
		break;
	case 2 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_PASS_OK;
		break;			
	case 3 : // 초기 패스워드 업데이트 성공
		nlfd2.nResult = EM_LOGIN_FB_SUB_SECID_ALREADY;
		break;			
	default : // 다른 모든경우 로그인 실패(1)
		nlfd2.nResult = DB_ERROR;
		CConsoleMessage::GetInstance()->Write(_T("CAgentExcitePassCheck result %d"), nResult);
		break;
	}

	pTemp->ExciteMsgPassCheckBack(&nlfd2);

	return NET_OK;
}



CUserLogoutSvr::CUserLogoutSvr(
	int nSvrGrp,
	int nSvrNum )
	: m_nSvrGrp( nSvrGrp )
	, m_nSvrNum( nSvrNum )
{	
}

int CUserLogoutSvr::Execute(CServer* pServer)
{
	return COdbcManager::GetInstance()->UserLogoutSvr(m_nSvrGrp, m_nSvrNum);
}

/**
* 사용자를 일정시간 블럭 시킨다.
*/
CUserBlock::CUserBlock(
	int nUserNum,
	int nDay )
	: m_nUserNum( nUserNum )
	, m_nDay( nDay )
{
}

int CUserBlock::Execute( CServer* pServer )
{
	return COdbcManager::GetInstance()->UserBlock( m_nUserNum, m_nDay );
}

CUserBlockDate::CUserBlockDate(
	int nUserNum,
	__time64_t tBlockTime )
	: m_nUserNum( nUserNum)
{
	m_Date = tBlockTime;
}

int CUserBlockDate::Execute( CServer* pServer )
{
    return COdbcManager::GetInstance()->UserBlockDate( m_nUserNum, m_Date );
}