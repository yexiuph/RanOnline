///////////////////////////////////////////////////////////////////////////////
// s_NetClientMsg.cpp
//
// class CNetClient
//
///////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "s_NetClient.h"
#include "s_CClientConsoleMsg.h"
#include <stdlib.h>
#include "GLContrlMsg.h"
#include "china_md5.h"
#include <string.h>
#include <wchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 로그인 정보를 전송한다. 
* Taiwan / Hongkong
*/
int CNetClient::SndLogin(
	const char* szUserID, 
	const char* szUserPassword,
	const char* szRandomPassword,
	int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	
	::StringCchCopy(nld.szUserid,         USR_ID_LENGTH+1,        szUserID);
	::StringCchCopy(nld.szPassword,       USR_PASS_LENGTH+1,      szUserPassword);
	::StringCchCopy(nld.szRandomPassword, USR_RAND_PASS_LENGTH+1, szRandomPassword);
	::StringCchCopy(nld.szEnCrypt, ENCRYPT_KEY+1, m_szEncryptKey);


	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = nld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &nld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/	

	m_Tea.encrypt (nld.szUserid, USR_ID_LENGTH+1);
	m_Tea.encrypt (nld.szPassword, USR_PASS_LENGTH+1);
	m_Tea.encrypt (nld.szRandomPassword, USR_RAND_PASS_LENGTH+1);
	m_Tea.encrypt (nld.szEnCrypt, ENCRYPT_KEY+1);
	

	return Send((char *) &nld);
}

int CNetClient::JapanSndLogin(
	const char* szUserID, 
	const char* szUserPassword,
	int nChannel )
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	JAPAN_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	
	::StringCchCopy(nld.szUserid,         JAPAN_USER_ID+1,        szUserID);
	::StringCchCopy(nld.szPassword,       JAPAN_USER_PASS+1,      szUserPassword);
	::StringCchCopy(nld.szEnCrypt, ENCRYPT_KEY+1, m_szEncryptKey);


	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = nld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &nld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/	

	m_Tea.encrypt (nld.szUserid, JAPAN_USER_ID+1);
	m_Tea.encrypt (nld.szPassword, JAPAN_USER_PASS+1);
	m_Tea.encrypt (nld.szEnCrypt, ENCRYPT_KEY+1);
	

	return Send((char *) &nld);
}

/**
* 로그인 정보를 전송한다. 
* China
*/
int CNetClient::ChinaSndLogin(	const TCHAR* szUserID,
								const TCHAR* szUserPassword,
								const TCHAR* szRandomPassword,
								int nChannel )
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	CHINA_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;

	CHINA_MD5::MD5 md5; // md5 해쉬용 클래스
	// 비밀번호의 문자열 길이를 구한다.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szUserPassword, USR_PASS_LENGTH, &nLength );
	
	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// 해쉬값을 얻기 위해서 패스워드 문자열을 넣는다.
	// 해쉬값을 얻기 위해 업데이트 해 준다.
	md5.update( reinterpret_cast<const unsigned char*> (szUserPassword), nLength );	
	// 얻어진 해쉬값을 strPass 에 넣는다.
	std::string strPass = md5.asString();
	// 얻어진 해쉬값을 복사해 넣는다.
	StringCchCopy( nld.szUserid,   USR_ID_LENGTH+1, szUserID ); // ID 를 복사해 넣는다.
	StringCchCopy( nld.szPassword, USR_PASS_LENGTH, strPass.c_str() );
	StringCchCopy( nld.szRandomPassword, USR_RAND_PASS_LENGTH+1, szRandomPassword);
	
	// 넣어진 모든 값들을 암호화 한다.
	m_Tea.encrypt( nld.szUserid, USR_ID_LENGTH+1 );
	m_Tea.encrypt( nld.szPassword, USR_PASS_LENGTH );
	m_Tea.encrypt( nld.szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	//EncryptLoginDataWithApexRSA(	(unsigned char*)nld.szUserid, 
	//								(unsigned char*)nld.szPassword, 
	//								(unsigned char*)nld.szRandomPassword );

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}

/**
* 로그인 정보를 전송한다.
* Daum Game
*/
int CNetClient::DaumSndLogin(const TCHAR* szUUID, int nChannel)
{
	if (szUUID == NULL) return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	DAUM_NET_LOGIN_DATA dnld;

	dnld.nChannel = nChannel;
	::StringCchCopy( dnld.szUUID, UUID_STR_LENGTH, szUUID );
	//::StringCchCopy(dnld.szParameter, DAUM_MAX_PARAM_LENGTH+1, szParameter);

	return Send((char *) &dnld);
}

int CNetClient::DaumSndPassCheck( const TCHAR* szDaumGID, const TCHAR* szUserPassword, int nCheckFlag )
{
	if( szDaumGID == NULL || szUserPassword == NULL ) return NET_ERROR;

	DAUM_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szDaumGID, DAUM_MAX_GID_LENGTH+1, szDaumGID );
	::StringCchCopy( nld.szUserPass, DAUM_USERPASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}

/**
* 로그인 정보를 전송한다. 
* GSP
*/
int CNetClient::GspSndLogin( 
	const TCHAR* szUUID,
	int nChannel )
{
	if (szUUID == NULL) return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	GSP_NET_LOGIN_DATA msg;
	
	msg.nChannel = nChannel;
	::StringCchCopy( msg.szUUID, GSP_USERID, szUUID );
	// m_Tea.encrypt( msg.szUUID, GSP_USERID );

	return Send( (char *) &msg );
}

/**
* 로그인 정보를 전송한다. 
* Terra 
*/
int CNetClient::TerraSndLogin(const char* szTID, int nChannel)
{
	if (szTID == NULL) return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	TERRA_NET_LOGIN_DATA tnld;
	
	tnld.nChannel = nChannel;
	::StringCchCopy(tnld.szTID, TERRA_TID_ENCODE+1, szTID);

	return Send((char *) &tnld);
}

int CNetClient::TerraSndPassCheck( const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag )
{
	if( szUserid == NULL || szUserPassword == NULL ) return NET_ERROR;

	TERRA_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szTLoginName, TERRA_TLOGIN_NAME+1, szUserid );
	::StringCchCopy( nld.szUserPass, TERRA_USERPASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;
	
	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}

/**
* Excite Japan 로그인 데이터 전송
* \param szU UserID char 20
* \param szT 유효기간 char 14
* \param szMD5 MD5 char 32
* \param nChannel 채널번호
* \return 
*/

int CNetClient::ExciteSndLogin(const char* szU,
							   const char* szT,
							   const char* szMD5,
							   int nChannel)
{
	if (szU == NULL || szT == NULL || szMD5 == NULL) return NET_ERROR;

	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	EXCITE_NET_LOGIN_DATA enld;

	enld.nChannel = nChannel;
	::StringCchCopy(enld.szUID,   EXCITE_USER_ID+1, szU);
	::StringCchCopy(enld.szTDATE, EXCITE_TDATE+1,   szT);
	::StringCchCopy(enld.szMD5,   EXCITE_MD5+1,     szMD5);

	return Send((char *) &enld);
}


int CNetClient::ExciteSndPassCheck( const TCHAR* szUserid, const TCHAR* szUserPassword, int nCheckFlag )
{
	if( szUserid == NULL || szUserPassword == NULL ) return NET_ERROR;

	EXCITE_NET_PASSCHECK_DATA nld;

	::StringCchCopy( nld.szExciteUserID, EXCITE_SEC_ID+1, szUserid );
	::StringCchCopy( nld.szUserPass, EXCITE_SEC_PASS+1, szUserPassword );
	nld.nCheckFlag = nCheckFlag;

	// Agent 서버로 전송한다.
	return Send((char *) &nld);
}


/**
* 로그인 정보를 전송한다. 
* Thailand (태국)
*/
int CNetClient::ThaiSndLogin(const char* userid,
	                         const char* userpass,
				             int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	THAI_NET_LOGIN_DATA tnld;
		
	tnld.nChannel  = nChannel;
	
	::StringCchCopy(tnld.szUserid,   USR_ID_LENGTH+1,   userid);
	::StringCchCopy(tnld.szPassword, USR_PASS_LENGTH+1, userpass);

	/// 암호방식 Tea 방식으로 변경됨. 
	/// 2005-11-16 Jgkim
	
	/*
	DWORD dwSize = tnld.nmg.dwSize - sizeof(NET_MSG_GENERIC);

	char* pBuffer = (char*) &tnld;
	m_Bit.buf_encode(pBuffer+sizeof(NET_MSG_GENERIC),
					 dwSize,
					 m_ck.nKeyDirection,
					 m_ck.nKey);
	*/
	m_Tea.encrypt( tnld.szUserid, USR_ID_LENGTH+1 );
	m_Tea.encrypt( tnld.szPassword, USR_PASS_LENGTH+1 );
	
	return Send((char *) &tnld);
}

/**
* 로그인 정보를 전송한다.
* 글로벌 서비스
*/
int CNetClient::GsSndLogin( const char* szUserID, const char* szUserPassword, int nChannel)
{
	if (nChannel < 0 || nChannel >= MAX_CHANNEL_NUMBER) nChannel = 0;

	GS_NET_LOGIN_DATA nld;
		
	nld.nChannel  = nChannel;
	
	::StringCchCopy(nld.szUserid,         GS_USER_ID+1,        szUserID);
	::StringCchCopy(nld.szPassword,       GS_USER_PASS+1,      szUserPassword);
	::StringCchCopy(nld.szEnCrypt, ENCRYPT_KEY+1, m_szEncryptKey);


	m_Tea.encrypt (nld.szUserid, GS_USER_ID+1);
	m_Tea.encrypt (nld.szPassword, GS_USER_PASS+1);
	m_Tea.encrypt (nld.szEnCrypt, ENCRYPT_KEY+1);
	

	return Send((char *) &nld);

}