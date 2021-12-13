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
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::DaumSndDelCharacter(int nChaNum)
{
	DAUM_NET_CHA_DEL ncd;
	ncd.nmg.nType	= DAUM_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	ncd.nChaNum		= nChaNum;

	return Send((char *) &ncd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::TerraSndDelCharacter(int nChaNum)
{
	TERRA_NET_CHA_DEL ncd;
	ncd.nmg.nType	= TERRA_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	ncd.nChaNum		= nChaNum;
	return Send((char *) &ncd);
}

/**
* ĳ���� ���� ��û GSP
* \param nChaNum ĳ���͹�ȣ
* \return 
*/
int CNetClient::GspSndDelCharacter( int nChaNum )
{
	GSP_NET_CHA_DEL ncd;	
	ncd.nChaNum		= nChaNum;
	return Send((char *) &ncd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::SndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	NET_CHA_DEL ncd;
	ncd.nmg.nType	= NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	ncd.nChaNum		= nChaNum;	
	::StringCchCopy( ncd.szPass2, USR_PASS_LENGTH+1, szPasswd ); // �н����� ����	
	m_Tea.encrypt( ncd.szPass2, USR_PASS_LENGTH+1 ); // �н����带 ��ȣȭ �Ѵ�.
	// Agent ������ ����
	return Send( (char *) &ncd );
}

/**
* ĳ���� ���� ��û
* \param nChaNum ������ ĳ���͹�ȣ
* \param strPasswd ��й�ȣ
* \return 
*/
int	CNetClient::ChinaSndDelCharacter( int nChaNum, const TCHAR* szPasswd )
{
	if (szPasswd == NULL) return NET_ERROR;

	CHINA_MD5::MD5 md5; // md5 �ؽ��� Ŭ����
	// ��й�ȣ�� ���ڿ� ���̸� ���Ѵ�.
	size_t nLength = 0;
	HRESULT sRet = StringCchLength( szPasswd, USR_PASS_LENGTH, &nLength );

	if (sRet == STRSAFE_E_INVALID_PARAMETER)
		return NET_ERROR;

	// �ؽ����� ��� ���ؼ� �н����� ���ڿ��� �ִ´�.
	// �ؽ����� ��� ���� ������Ʈ �� �ش�.
	md5.update( reinterpret_cast<const unsigned char*> (szPasswd), nLength );
	// ����� �ؽ����� strPass �� �ִ´�.
	std::string strPass = md5.asString();
	
	NET_CHA_DEL ncd;
	ncd.nmg.nType	= NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	ncd.nChaNum		= nChaNum;
	// ����� �ؽ����� ������ �ִ´�.
	StringCchCopy( ncd.szPass2, USR_PASS_LENGTH, strPass.c_str() );
	m_Tea.encrypt( ncd.szPass2, USR_PASS_LENGTH ); // �н����带 ��ȣȭ �Ѵ�.

	// Agent ������ ����
	return Send( (char *) &ncd );
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::ExciteSndDelCharacter(int nChaNum)
{
	EXCITE_NET_CHA_DEL encd;
	encd.nmg.nType	= EXCITE_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	encd.nChaNum	= nChaNum;
	return Send((char *) &encd);
}

/**
* ĳ���� ���� ������ �����Ѵ�.
*/
int CNetClient::GsSndDelCharacter(int nChaNum)
{
	GS_NET_CHA_DEL ncd;
	ncd.nmg.nType	= GS_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
	ncd.nChaNum		= nChaNum;	

	return Send((char *) &ncd);
}