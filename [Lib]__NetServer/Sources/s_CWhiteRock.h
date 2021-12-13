#ifndef S_CWHITEROCK_H_
#define S_CWHITEROCK_H_

#pragma once

//#include "s_CHttpPatch.h"
#include "s_CWhiteRockXML.h"
#include "s_CHttpPatch.h"

class CWhiteRock
{	
private:
	CWhiteRock();
	~CWhiteRock();
	static CWhiteRock* SelfInstance;

public:
	static CWhiteRock* GetInstance();
	static void ReleaseInstance();

	// Login, Logout XML�� ��� �ִ� ��ü
	CWhiteRockXml m_cWhiteRockLogin;
	CWhiteRockXml m_cWhiteRockLogout;

	CWhiteRockXml m_cWhiteRockLoginResult;
	CWhiteRockXml m_cWhiteRockLogoutResult;

	// WhiteRock�� ����� �ϱ� ���� ��ü
    CHttpPatch	m_cHttpPatch;

	// ��� ó���� �Ϸ�ǰ� ���ϰ��� �����ϴ� ����
	CString m_strLoginResult;
	CString m_strUUID;
	int		m_nUserNum;
	CString m_strLogoutResult;

	// Parameter�� ������ XML�� �ҷ����� �Լ�
	CString GetLoginXML();
	CString GetLogoutXML();

	// WhiteRock System Name, Port
	CString m_sWhiteRockName;
	int m_nWhiteRockPort;

	// �ʱ�ȭ �Լ�
	void init();

	bool SetWhiteRockXML();

	void SetWhiteRockInfo( const TCHAR* szWRName, int nWRPort );

	// Login XML�� ������ ���� Parameter ����
	bool SetUserID( CString strUserIDPath, CString strUserID );
	bool SetUserPass( CString strUserPassPath, CString strUserPass );
	bool SetUserIP( CString strUserIPPath, CString strUserIP );
	bool SetUserUUID( CString strUserUUIDPath, CString strUserUUID );

	// ó���� �Ϸ�� ���ϰ��� �����ϴ� �Լ�
	void SetLoginResult( CString& strResult );
	void SetUUID( CString& strUUID );
	void SetUserNum( int nUserNum );
	void SetLogoutResult( CString& strResult );
	
	// ó���� �Ϸ�� ���ϰ��� ȣ���ϴ� �Լ�
	const CString& GetLoginResult();
	const CString& GetUUID();
	const int GetUserNum();
	const CString& GetLogoutResult();

	int SendLoginXML( const TCHAR* strUserID, const TCHAR* strUserPass, const TCHAR* strUserIP );
	bool SendLogoutXML( const TCHAR* strUserUUID );

};

#endif // S_CWHITEROCK_H_