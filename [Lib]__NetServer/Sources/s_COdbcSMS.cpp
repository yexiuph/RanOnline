#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* SMS 문자메시지를 발송한다.
*
* \param nSendChaNum SMS 를 발송하는 캐릭터 번호
* \param szSendUserID SMS 를 발송하는 사용자 ID
* \param nReceiveChaNum SMS 를 받을 캐릭터 번호
* \param szReceivePhone SMS 를 받을 캐릭터의 전화번호
* \param szSmsMsg SMS 메시지 내용
*/
int COdbcManager::LogSMS(
	int nSendChaNum,	
	int nReceiveChaNum,
	const TCHAR* szReceivePhone,
	const TCHAR* szSmsMsg )
{
	if (nSendChaNum < 0 ||
		nReceiveChaNum < 0 ||
		szReceivePhone == NULL ||
		szSmsMsg == NULL )
	{
		return DB_ERROR;
	}
	else
	{
		//std::strstream strTemp;
		//strTemp << "{call sp_LogSms_Insert(";
		//strTemp << nSendChaNum << ","; // 보내는 캐릭터 번호
		//strTemp << "'',"; // 보낸사람 [공백]
		//strTemp << nReceiveChaNum << ","; // 받는 캐릭터번호
		//strTemp << "'" << szReceivePhone << "',"; // 받을사람의 폰 번호
		//strTemp << "'" << szSmsMsg << "',";	// SMS 메시지 내용
		//strTemp << "?)}";
		//strTemp << std::ends;

		TCHAR szTemp[256] = {0};
		_snprintf_s( szTemp, 256, "{call sp_LogSms_Insert(%d,'',%d,'%s','%s',?)}",
								nSendChaNum,
								nReceiveChaNum,
								szReceivePhone,
								szSmsMsg );
		
		int nReturn = m_pLogDB->ExecuteSpInt( szTemp );
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

		return nReturn;
	}
}

/**
* 캐릭터의 휴대폰 번호를 저장한다.
* \param nChaNum 캐릭터 번호
* \param szPhoneNumber 전화번호 NULL 문자 포함 14자 SMS_RECEIVER
*/
int COdbcManager::SetChaPhoneNumber(
	int nChaNum,
	const TCHAR* szPhoneNumber )
{
	if (nChaNum < 0 ||
		szPhoneNumber == NULL )
	{
		return DB_ERROR;
	}
	else
	{
		//std::strstream strTemp;
		//strTemp << "{call UpdateChaFriendSms(";
		//strTemp << nChaNum << ","; // 캐릭터 번호
		//strTemp << "'" << szPhoneNumber << "',"; // 전화번호
		//strTemp << "?)}";
		//strTemp << std::ends;

		TCHAR szTemp[128] = {0};
		_snprintf_s( szTemp, 128, "{call UpdateChaFriendSms(%d,'%s',?)}", nChaNum, szPhoneNumber );
		
		int nReturn = m_pGameDB->ExecuteSpInt( szTemp );
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return nReturn;
	}
}

/**
* 캐릭터의 친구정보를 가져온다.
* \param nChaNum 친구정보를 가져올 캐릭터 번호
* \param vFriend 친구정보를 담을 vector
*/
int	COdbcManager::GetChaFriendSMS(
	int nChaNum,
	std::vector<CHA_FRIEND_SMS> &vFriend )
{
	if (nChaNum < 0) return DB_ERROR;

	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT ChaP, ChaS, ChaName, ChaFlag, ChaToPhone From viewChaFriendSms ";
	//strTemp << "WHERE ChaP=" << nChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT ChaP, ChaS, ChaName, ChaFlag, ChaToPhone From viewChaFriendSms WHERE ChaP=%d", nChaNum );
	
	sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*)szTemp, 
					SQL_NTS );

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print( szTemp );
		Print( GetErrorString(pConn->hStmt) );
		m_pGameDB->FreeConnection( pConn );

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	SQLCHAR szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaP = 0, cbChaP=SQL_NTS;
	SQLINTEGER nChaS = 0, cbChaS=SQL_NTS;
	SQLINTEGER nChaFlag = 0, cbChaFlag=SQL_NTS;
	SQLCHAR szPhoneNumber[SMS_RECEIVER] = {0}; SQLINTEGER cbPhoneNumber = SQL_NTS;
	
	while (true)
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print( szTemp );
			Print( GetErrorString(pConn->hStmt) );
            m_pGameDB->FreeConnection( pConn );

			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			CHA_FRIEND_SMS sFriendSMS;

			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG,	&nChaP, 0, &cbChaP );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG,	&nChaS, 0, &cbChaS );
			::SQLGetData( pConn->hStmt, 3, SQL_C_CHAR,  szChaName, CHR_ID_LENGTH, &cbChaName );
			::SQLGetData( pConn->hStmt, 4, SQL_C_LONG,	&nChaFlag, 0, &cbChaFlag );
			::SQLGetData( pConn->hStmt, 5, SQL_C_CHAR,  szPhoneNumber, SMS_RECEIVER, &cbPhoneNumber );

			 if (cbChaName != 0 && cbChaName != -1)
				::StringCchCopy( sFriendSMS.szCharName, CHR_ID_LENGTH+1, (const TCHAR*) szChaName );

			 if (cbPhoneNumber != 0 && cbPhoneNumber != -1)
				::StringCchCopy( sFriendSMS.szPhoneNumber, SMS_RECEIVER, (const TCHAR*) szPhoneNumber );

			 sFriendSMS.nChaS = nChaS;
			 sFriendSMS.nChaFlag = nChaFlag;

			 vFriend.push_back ( sFriendSMS );
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}	  
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return DB_OK;    
}