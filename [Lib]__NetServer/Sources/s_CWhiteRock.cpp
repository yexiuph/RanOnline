#include "pch.h"
#include "s_CWhiteRock.h"
#include "s_CConsoleMessage.h"

CWhiteRock* CWhiteRock::SelfInstance = NULL;

CWhiteRock::CWhiteRock()
{
}

CWhiteRock::~CWhiteRock()
{
}

CWhiteRock* CWhiteRock::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CWhiteRock();
	return SelfInstance;
}

void CWhiteRock::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CWhiteRock::init()
{
	m_strLoginResult = "";
	m_strUUID = "";
	m_strLogoutResult = "";
}

bool CWhiteRock::SetWhiteRockXML()
{
	if( !m_cWhiteRockLogin.LoadXml( "cfg/Login.xml" ) ) return false;

	if( !m_cWhiteRockLogout.LoadXml( "cfg/Logout.xml" ) ) return false;

	return true;
}

bool CWhiteRock::SetUserID( CString strUserIDPath, CString strUserID )
{
//	m_pCWhiteRockLogin->SetData( "params/userid", strID );
	if( !m_cWhiteRockLogin.SetData( strUserIDPath, strUserID ) ) return false;
	else return true;
}

bool CWhiteRock::SetUserPass( CString strUserPassPath, CString strUserPass )
{
	if( !m_cWhiteRockLogin.SetData( strUserPassPath, strUserPass ) ) return false;
	else return true;
}

bool CWhiteRock::SetUserIP( CString strUserIPPath, CString strUserIP )
{
	if( !m_cWhiteRockLogin.SetData( strUserIPPath, strUserIP ) ) return false;
	return true;
}

bool CWhiteRock::SetUserUUID( CString strUserUUIDPath, CString strUserUUID )
{
	if( !m_cWhiteRockLogout.SetData( strUserUUIDPath, strUserUUID ) ) return false;
	return true;
}

void CWhiteRock::SetWhiteRockInfo( const TCHAR* szWRName, int nWRPort )
{
	m_sWhiteRockName = szWRName;
	m_nWhiteRockPort = nWRPort;

	m_cHttpPatch.SetServer( szWRName );
	m_cHttpPatch.SetPort( nWRPort );
}

void CWhiteRock::SetLoginResult( CString& strResult )
{
	m_strLoginResult = strResult;
}

void CWhiteRock::SetUUID( CString& strUUID )
{
	m_strUUID = strUUID;
}

void CWhiteRock::SetUserNum( int nUserNum )
{
	m_nUserNum = nUserNum;
}

const CString& CWhiteRock::GetLoginResult()
{
	return m_strLoginResult;
}

const CString& CWhiteRock::GetUUID()
{
	return m_strUUID;
}

void CWhiteRock::SetLogoutResult( CString& strResult )
{
	m_strLogoutResult = strResult;
}

const CString& CWhiteRock::GetLogoutResult()
{
	return m_strLogoutResult;
}

const int CWhiteRock::GetUserNum()
{
	return m_nUserNum;
}

int CWhiteRock::SendLoginXML( const TCHAR* strUserID, const TCHAR* strUserPass, const TCHAR* strUserIP )
{
	init();

	CString UserID;
	CString UserPass;
	CString UserIP;
	CString strLoginXML;
	CString strReceive;
//	CString strPath = "/WhiteRock-Game/xml.action";
	CString strPath = "/RanOnline/xml.action";
	int		nResult = 0;

	UserID = strUserID;
	UserPass = strUserPass;
	// 내부에서 테스트 할 경우 IP가 사설IP를 사용하므로 고정 IP로의 변경이 필요.
	UserIP = strUserIP;
//	UserIP = "59.25.186.100";

	SetUserID( "params/account/loginid", UserID );
	SetUserPass( "params/account/password", UserPass );
	SetUserIP( "params/account/ipaddress", UserIP );

	strLoginXML = "data=";
	strLoginXML += m_cWhiteRockLogin.GetXmlString();	

	if( !m_cHttpPatch.SendData( strPath, strLoginXML, strReceive ) ) return nResult;

	if( !m_cWhiteRockLoginResult.LoadString( strReceive ) ) return nResult;

	CString strResult;
	CString strUUID;
	CString strUserNum;

	// UserNum 추가의 경우 Excite의 이관 유저의 경우 Gonzo에서 기존 캐릭터 정보를 그대로 사용하기 
	// 위해서 WhiteRock System에 저장된 UserNum을 리턴받아서 RanUserDB의 UserNum을 동일하게 사용하기 위함.
	strResult	= m_cWhiteRockLoginResult.GetData( "params/account/status/code" ); // 성공 실패 여부 가져오기
	strUUID		= m_cWhiteRockLoginResult.GetData( "params/account/uuid" ); // User의 UUID 가져오기
	strUserNum	= m_cWhiteRockLoginResult.GetData( "params/account/uid" ); // User번호 가져오기

	if ( _ttoi(strResult) != HTTP_STATUS_OK)
	{
		CConsoleMessage::GetInstance()->Write( 
			_T("WhiteRock Login ERROR Restult: (%s) , UserID: (%s)"),
			strResult, strUserID);

		nResult = _ttoi(strResult);

		return nResult;
	}

	SetUUID( strUUID );

	// 리턴받은 UserNum String -> int로 변환
	int nTemp = _ttoi(strUserNum);
	SetUserNum( nTemp );

	nResult = _ttoi(strResult);

	return nResult;
}

bool CWhiteRock::SendLogoutXML( const TCHAR* strUserUUID )
{
	init();

	CString UserUUID;
	CString strLogoutXML;
	CString strReceive;
//	CString strPath = "/WhiteRock-Game/xml.action";
	CString strPath = "/RanOnline/xml.action";

	UserUUID = strUserUUID;	

	SetUserUUID( "params/account/uuid", UserUUID );

	strLogoutXML = "data=";
	strLogoutXML += m_cWhiteRockLogout.GetXmlString();
	
	if( !m_cHttpPatch.SendData( strPath, strLogoutXML, strReceive ) ) return false;

	if( !m_cWhiteRockLogoutResult.LoadString( strReceive ) ) return false;

	CString strResult;

	strResult = m_cWhiteRockLoginResult.GetData( "params/account/status/code" );

	if ( _ttoi(strResult) != HTTP_STATUS_OK)
	{
		CConsoleMessage::GetInstance()->Write(
									_T("WhiteRock Logout ERROR UserID: (%s)"),
									strUserUUID);

		return false;
	}

	return true;
}