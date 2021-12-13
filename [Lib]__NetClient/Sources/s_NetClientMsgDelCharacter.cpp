#include "pch.h"
#include "s_NetClient.h"
#include "s_CClientConsoleMsg.h"
#include <stdlib.h>
#include "GLContrlMsg.h"
#include "china_md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::DaumSndDelCharacter(int nChaNum)
{
	DAUM_NET_CHA_DEL ncd;
	ncd.nmg.nType	= DAUM_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	ncd.nChaNum		= nChaNum;

	return Send((char *) &ncd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::TerraSndDelCharacter(int nChaNum)
{
	TERRA_NET_CHA_DEL ncd;
	ncd.nmg.nType	= TERRA_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	ncd.nChaNum		= nChaNum;
	return Send((char *) &ncd);
}

/**
* 캐릭터 삭제 요청 GSP
* \param nChaNum 캐릭터번호
* \return 
*/
int CNetClient::GspSndDelCharacter( int nChaNum )
{
	GSP_NET_CHA_DEL ncd;	
	ncd.nChaNum		= nChaNum;
	return Send((char *) &ncd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::SndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	NET_CHA_DEL ncd;
	ncd.nmg.nType	= NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	ncd.nChaNum		= nChaNum;	
	::StringCchCopy( ncd.szPass2, USR_PASS_LENGTH+1, szPasswd ); // 패스워드 복사	
	m_Tea.encrypt( ncd.szPass2, USR_PASS_LENGTH+1 ); // 패스워드를 암호화 한다.
	// Agent 서버로 전송
	return Send( (char *) &ncd );
}

/**
* 캐릭터 삭제 요청
* \param nChaNum 삭제할 캐릭터번호
* \param strPasswd 비밀번호
* \return 
*/
int	CNetClient::ChinaSndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	CHINA_MD5::MD5 md5; // md5 해쉬용 클래스
	// 비밀번호의 문자열 길이를 구한다.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szPasswd, USR_PASS_LENGTH, &nLength );

	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// 해쉬값을 얻기 위해서 패스워드 문자열을 넣는다.
	// 해쉬값을 얻기 위해 업데이트 해 준다.
	md5.update( reinterpret_cast<const unsigned char*> (szPasswd), nLength );
	// 얻어진 해쉬값을 strPass 에 넣는다.
	std::string strPass = md5.asString();
	
	NET_CHA_DEL ncd;
	ncd.nmg.nType	= NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	ncd.nChaNum		= nChaNum;
	// 얻어진 해쉬값을 복사해 넣는다.
	StringCchCopy( ncd.szPass2, USR_PASS_LENGTH, strPass.c_str() );
	m_Tea.encrypt( ncd.szPass2, USR_PASS_LENGTH ); // 패스워드를 암호화 한다.

	// Agent 서버로 전송
	return Send( (char *) &ncd );
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::ExciteSndDelCharacter(int nChaNum)
{
	EXCITE_NET_CHA_DEL encd;
	encd.nmg.nType	= EXCITE_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* 캐릭터 삭제 정보를 전송한다.
*/
int CNetClient::GsSndDelCharacter(int nChaNum)
{
	GS_NET_CHA_DEL ncd;
	ncd.nmg.nType	= GS_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
	ncd.nChaNum		= nChaNum;	

	return Send((char *) &ncd);
}