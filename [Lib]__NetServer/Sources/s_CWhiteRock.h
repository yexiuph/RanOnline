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

	// Login, Logout XML을 들고 있는 객체
	CWhiteRockXml m_cWhiteRockLogin;
	CWhiteRockXml m_cWhiteRockLogout;

	CWhiteRockXml m_cWhiteRockLoginResult;
	CWhiteRockXml m_cWhiteRockLogoutResult;

	// WhiteRock과 통신을 하기 위한 객체
    CHttpPatch	m_cHttpPatch;

	// 모든 처리가 완료되고 리턴값을 저장하는 변수
	CString m_strLoginResult;
	CString m_strUUID;
	int		m_nUserNum;
	CString m_strLogoutResult;

	// Parameter를 적용한 XML을 불러오는 함수
	CString GetLoginXML();
	CString GetLogoutXML();

	// WhiteRock System Name, Port
	CString m_sWhiteRockName;
	int m_nWhiteRockPort;

	// 초기화 함수
	void init();

	bool SetWhiteRockXML();

	void SetWhiteRockInfo( const TCHAR* szWRName, int nWRPort );

	// Login XML을 보내기 위한 Parameter 설정
	bool SetUserID( CString strUserIDPath, CString strUserID );
	bool SetUserPass( CString strUserPassPath, CString strUserPass );
	bool SetUserIP( CString strUserIPPath, CString strUserIP );
	bool SetUserUUID( CString strUserUUIDPath, CString strUserUUID );

	// 처리가 완료된 리턴값을 세팅하는 함수
	void SetLoginResult( CString& strResult );
	void SetUUID( CString& strUUID );
	void SetUserNum( int nUserNum );
	void SetLogoutResult( CString& strResult );
	
	// 처리가 완료된 리턴값을 호출하는 함수
	const CString& GetLoginResult();
	const CString& GetUUID();
	const int GetUserNum();
	const CString& GetLogoutResult();

	int SendLoginXML( const TCHAR* strUserID, const TCHAR* strUserPass, const TCHAR* strUserIP );
	bool SendLogoutXML( const TCHAR* strUserUUID );

};

#endif // S_CWHITEROCK_H_