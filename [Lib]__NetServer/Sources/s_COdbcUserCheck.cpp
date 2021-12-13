#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * Daum
 * ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.
 * \param szDaumGID 
 * \param szDaumUID 
 * \param szDaumSSNHEAD 
 * \param szDaumSEX 
 * \param szUsrIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \return 
 */
/*
int	COdbcManager::DaumUserCheck(
	const TCHAR* szDaumGID, 
	const TCHAR* szDaumUID,
	const TCHAR* szDaumSSNHEAD,
	const TCHAR* szDaumSEX,
	const TCHAR* szUsrIP, 
	int nSvrGrp, 
	int nSvrNum )
{	
	//std::strstream strTemp;
	TCHAR szTemp[512] = {0};

	//strTemp << "{call daum_user_verify(";
	//strTemp << "'" << szDaumGID << "',";
	//strTemp << "'" << szDaumUID << "',";
	//strTemp << "'" << szDaumSSNHEAD << "',";
	//strTemp << "'" << szDaumSEX << "',";
	//strTemp << "'" << szUsrIP << "',";
	//strTemp << nSvrGrp << ",";
	//strTemp << nSvrNum << ", ?)}";
	//strTemp << std::ends;

	_snprintf( szTemp, 512, "{call daum_user_verify('%s','%s','%s','%s','%s',%d,%d,?)}", 
													szDaumGID, 
													szDaumUID, 
													szDaumSSNHEAD, 
													szDaumSEX, 
													szUsrIP, 
													nSvrGrp, 
													nSvrNum );
	
	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	return nReturn;
}
*/

int COdbcManager::DaumUserCheck(
			TCHAR* szDaumGID,
		    const TCHAR* szUUID,
			const TCHAR* szUserIP,
			int nSvrGrp,
			int nSvrNum )
{
		// #1 UUID¸¦ ÀÌ¿ëÇØ¼­ UserTemp ¿¡¼­ UserID, UserIP, LoginDate ¸¦ °¡Á®¿Â´Ù.
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pKorDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	//std::strstream strTemp;
	TCHAR szTemp[256] = {0};

	//strTemp << "SELECT UserID, UserIP, LoginDate ";
	//strTemp << "FROM UserTemp WHERE GUID = '";
	//strTemp << szUUID << "'";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 256, "SELECT UserID, LoginDate FROM UserUUID WHERE UserUUID = '%s'", szUUID );

	sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*) szTemp, 
					SQL_NTS);

	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
        Print( szTemp );
		Print( GetErrorString( pConn->hStmt ) );
		m_pKorDB->FreeConnection( pConn );
		//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
        return DB_ERROR;
	}

	SQLCHAR szKorUserID[KOR_MAX_GID_LENGTH] = {0};
//	SQLCHAR szKorUserIP[KOR_USERIP] = {0};
	TIMESTAMP_STRUCT sLoginDate; SQLINTEGER cbLoginDate = SQL_NTS;
	
	SQLINTEGER cbKorUserID = SQL_NTS;	
//	SQLINTEGER cbKorUserIP = SQL_NTS;

	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if ( (sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO) )
        {			
            Print( szTemp );
			Print( GetErrorString( pConn->hStmt ) );
            m_pKorDB->FreeConnection( pConn );
			//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
            return DB_ERROR;
		}

		if ( (sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO) )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_CHAR, szKorUserID, KOR_MAX_GID_LENGTH, &cbKorUserID );
//			::SQLGetData( pConn->hStmt, 2, SQL_C_CHAR, szKorUserIP, GSP_USERIP, &cbKorUserIP );
			::SQLGetData( pConn->hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sLoginDate, 0, &cbLoginDate );

			if ( (cbKorUserID != 0) && (cbKorUserID != -1) )
			{
				StringCchCopy(szDaumGID, KOR_MAX_GID_LENGTH, (const TCHAR*) szKorUserID);
				nRowCount++;
			}			

			// ÇÁ¸®¹Ì¾ö ¸¸·á±â°£
			// 1970-02-01 : Default
			// ÅÂ±¹¿¡¼­ TimeZone ¼¼ÆÃÀÌ Æ²·Á¼­ ¿À·ù°¡ ¹ß»ý
			// ÀÌ ÄÚµå¸¦ Ãß°¡·Î »ðÀÔÇÑ´Ù.
			if ( (sLoginDate.year <= 1970) || (sLoginDate.year >= 2999) )
			{
				sLoginDate.year = 1970;
				sLoginDate.month = 2;
				sLoginDate.day = 1;
				sLoginDate.hour = 1;
				sLoginDate.minute = 1;
				sLoginDate.second = 1;
			}					
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pKorDB->FreeConnection( pConn );
	pConn = NULL;
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	// Row Count °¡ 1 ÀÌ ¾Æ´Ï¸é ¿¡·¯ÀÌ´Ù.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		// Check Code
//		CConsoleMessage::GetInstance()->Write( _T("DB Check:%s"), szTemp );
		//strTemp.freeze( false );

//		CConsoleMessage::GetInstance()->Write( _T("ERROR:DaumUserCheck can't get info UserID from UserTemp") );
		
		return KOR_ROWCOUNT_ERROR;
	}

	// WEB ·Î±×ÀÎ½Ã°£ ¼¼ÆÃ
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ÇöÀç ½Ã°£À» °¡Á®¿Â´Ù.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (ÇöÀç½Ã°£ - WEB ¿¡¼­ ·Î±×ÀÎÇÑ ½Ã°£) À» °è»êÇÑ´Ù.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (ÇöÀç½Ã°£ - WEB ¿¡¼­ ·Î±×ÀÎÇÑ ½Ã°£) ÀÌ ÀÏÁ¤ÇÑ ¹üÀ§³»¿¡ ÀÖ¾î¾ß ·Î±×ÀÎÀ» ÁøÇàÇÑ´Ù.
	// PC ¹æ µî¿¡¼­ ·Î±×¾Æ¿ô ÇÏÁö ¾Ê°í ÀÚ¸®¸¦ ¶°³­ À¯ÀúµîÀÇ °æ¿ì...
	// µÞ »ç¶÷ÀÌ ¾Õ¿¡ ÇÃ·¹ÀÌ ÇÑ Á¤º¸¸¦ °¡Áö°í ±×´ë·Î µé¾î¿Ã °¡´É¼ºÀÌ ÀÖ´Ù.
	if ( (diffTime >= KOR_ALLOW_TIME_DIFF_MINUS) &&
		 (diffTime <= KOR_ALLOW_TIME_DIFF_PLUS) )
	{
		TCHAR szTemp[256] = {0};

		_snprintf_s( szTemp, 256, "{call daum_user_verify('%s','%s',%d,%d,?)}", szKorUserID, szUserIP, nSvrGrp, nSvrNum );

		int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
		//strTemp3.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
		return nReturn;
	}
	else
	{
		// Time is wrong
		return KOR_ALLOW_TIME_ERROR;
	}

}

/**
* Excite (Japan)
* \param szUserID »ç¿ëÀÚID
* \param szUsrIP IP
* \param nSvrGrp ¼­¹ö±×·ì
* \param nSvrNum ¼­¹ö¹øÈ£
* \return 
*/
int COdbcManager::ExciteUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szUserIP, 
	int nSvrGrp, 
	int nSvrNum)
{
	//std::strstream strTemp;
	TCHAR szTemp[512] = {0};

	//strTemp << "{call excite_user_verify(";
	//strTemp << "'" << szUserID << "',";
	//strTemp << "'" << szUserIP << "',";
	//strTemp << nSvrGrp << ",";
	//strTemp << nSvrNum << ", ?)}";
	//strTemp << std::ends;

	_snprintf_s( szTemp, 512, "{call excite_user_verify('%s','%s',%d,%d,?)}", szUserID, szUserIP, nSvrGrp, nSvrNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
	
	return nReturn;
}

/**
* Gonzo (Japan)
* \param szUserID »ç¿ëÀÚID
* \param szUsrIP IP
* \param nSvrGrp ¼­¹ö±×·ì
* \param nSvrNum ¼­¹ö¹øÈ£
* \return 
*/
int COdbcManager::JapanUserCheck(
	const TCHAR* szUserID,
	const TCHAR* szUserIP, 
	int nSvrGrp, 
	int nSvrNum,
	int nUserNum)
{
	TCHAR szTemp[512] = {0};

	_snprintf_s( szTemp, 512, "{call Japan_user_verify('%s','%s',%d,%d,%d,?)}", szUserID, szUserIP,
		nSvrGrp, nSvrNum, nUserNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);	
	
	return nReturn;
}

/**
 * Taiwan / HongKong 
 * ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.	
 * \param szUsrID »ç¿ëÀÚ ID
 * \param szPasswd »ç¿ëÀÚ ÆÐ½º¿öµå
 * \param szUsrIP »ç¿ëÀÚÀÇ IP Address
 * \param nSvrGrp ¼­¹ö±×·ì
 * \param nSvrNum ¼­¹ö¹øÈ£
 * \return DB ÀÎÁõÈÄ °á°ú°ª
 */
int COdbcManager::UserCheck(
	const TCHAR* szUsrID, 
	const TCHAR* szPasswd, 
	const TCHAR* szUsrIP,
	const TCHAR* szRandomPasswd,
	int nRandomNum,
	int nSvrGrp,
	int nSvrNum)
{
	/// SQL ¹®¿¡¼­ ' ´Â ¹®ÀÚ¿­ÀÇ ³¡À» ³ªÅ¸³»±â ¶§¹®¿¡
	/// ' ¸¦ '' ·Î Ä¡È¯ÇØ¾ß ÇÑ´Ù.
	/// DB:{?=call user_verify('ÿ?,'±±?ó³ó¿ÿÿÿ?,'61.57.175.205',1,0, ?)}
	/// À§¿Í°°ÀÌ ¹®ÀÚ¿­ Áß°£¿¡ ' °¡ »ðÀÔµÇ¾î ÀÖÀ»°æ¿ì ¿¡·¯°¡ ¹ß»ýÇÏ°Ô µÈ´Ù.
	CString strUserID = szUsrID;
	strUserID.Trim(_T(" "));
	strUserID.Replace(_T("'"), _T("''"));

	CString strPasswd = szPasswd;
	strPasswd.Trim(_T(" "));
	strPasswd.Replace(_T("'"), _T("''"));

	CString strRandomPasswd = szRandomPasswd;
	strRandomPasswd.Trim(_T(" "));
	strRandomPasswd.Replace(_T("'"), _T("''"));

	TCHAR szRandomNum[10] = {0};
	_itot_s(nRandomNum, szRandomNum, 10);	

	//std::strstream strTemp;
	TCHAR szTemp[512] = {0};

	//strTemp << "{call user_verify(";
	//strTemp << "'" << strUserID.GetString() << "',";
	//strTemp << "'" << strPasswd.GetString() << "',";
	//strTemp << "'" << szUsrIP << "',";
	//strTemp << nSvrGrp << ",";
	//strTemp << nSvrNum << ",";
	//strTemp << "'" << strRandomPasswd.GetString() << "',";
	//strTemp << "'" << szRandomNum << "',?)}";
	//strTemp << std::ends;

	_snprintf_s( szTemp, 512, "{call user_verify('%s','%s','%s',%d,%d,'%s','%s',?)}", 
												strUserID.GetString(),
												strPasswd.GetString(),
												szUsrIP,
												nSvrGrp,
												nSvrNum,
												strRandomPasswd.GetString(),
												szRandomNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	return nReturn;
}

/**
* Thailand
* ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.	
* \param szUsrID »ç¿ëÀÚ ID
* \param szPasswd »ç¿ëÀÚ ÆÐ½º¿öµå
* \param szUsrIP »ç¿ëÀÚÀÇ IP Address
* \param nSvrGrp ¼­¹ö±×·ì
* \param nSvrNum ¼­¹ö¹øÈ£
* \return DB ÀÎÁõÈÄ °á°ú°ª
*/
int	COdbcManager::ThaiUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum )
{
	/// SQL ¹®¿¡¼­ ' ´Â ¹®ÀÚ¿­ÀÇ ³¡À» ³ªÅ¸³»±â ¶§¹®¿¡
	/// ' ¸¦ '' ·Î Ä¡È¯ÇØ¾ß ÇÑ´Ù.
	/// DB:{?=call user_verify('ÿ?,'±±?ó³ó¿ÿÿÿ?,'61.57.175.205',1,0, ?)}
	/// À§¿Í°°ÀÌ ¹®ÀÚ¿­ Áß°£¿¡ ' °¡ »ðÀÔµÇ¾î ÀÖÀ»°æ¿ì ¿¡·¯°¡ ¹ß»ýÇÏ°Ô µÈ´Ù.
	CString strUserID = szUsrID;
	strUserID.Replace( _T("'"), _T("''") );

	CString strPasswd = szPasswd;
	strPasswd.Replace (_T("'"), _T("''"));

	//std::strstream strTemp;
	TCHAR szTemp[512] = {0};

	//strTemp << "{call thai_user_verify(";
	//strTemp << "'" << strUserID.GetString() << "',";
	//strTemp << "'" << strPasswd.GetString() << "',";
	//strTemp << "'" << szUsrIP << "',";
	//strTemp << nSvrGrp << ",";
	//strTemp << nSvrNum << ", ?)}";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 512, "{call thai_user_verify('%s','%s','%s',%d,%d,?)}", strUserID.GetString(), strPasswd.GetString(), szUsrIP, nSvrGrp, nSvrNum );

	int nReturn = m_pUserDB->ExecuteSpInt (szTemp);
	//strTemp.freeze ( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	return nReturn;
}

/**
* GSP
* \n ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.
* \param 
* \param 
* \param szUsrIP ·Î±×ÀÎÇÏ·Á´Â IP Address
* \param nSvrGrp ¼­¹ö±×·ì
* \param nSvrNum ¼­¹ö¹øÈ£
* \return 
*/
int COdbcManager::GspUserCheck(
	TCHAR* szGspUserID,
	const TCHAR* szUUID,
	const TCHAR* szUserIP,
	int nSvrGrp,
	int nSvrNum )
{
	// #1 UUID¸¦ ÀÌ¿ëÇØ¼­ UserTemp ¿¡¼­ UserID, UserIP, LoginDate ¸¦ °¡Á®¿Â´Ù.
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pGspDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	//std::strstream strTemp;
	TCHAR szTemp[256] = {0};

	//strTemp << "SELECT UserID, UserIP, LoginDate ";
	//strTemp << "FROM UserTemp WHERE GUID = '";
	//strTemp << szUUID << "'";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 256, "SELECT UserID, UserIP, LoginDate FROM UserTemp WHERE GUID = '%s'", szUUID );

	sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*) szTemp, 
					SQL_NTS);

	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
        Print( szTemp );
		Print( GetErrorString( pConn->hStmt ) );
		m_pGspDB->FreeConnection( pConn );
		//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
        return DB_ERROR;
	}

	SQLCHAR szGspUserID2[GSP_USERID] = {0};
	SQLCHAR szGspUserIP[GSP_USERIP] = {0};
	TIMESTAMP_STRUCT sLoginDate; SQLINTEGER cbLoginDate = SQL_NTS;
	
	SQLINTEGER cbGspUserID = SQL_NTS;
	SQLINTEGER cbGspUserIP = SQL_NTS;

	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if ( (sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO) )
        {			
            Print( szTemp );
			Print( GetErrorString( pConn->hStmt ) );
            m_pGspDB->FreeConnection( pConn );
			//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
            return DB_ERROR;
		}

		if ( (sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO) )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_CHAR, szGspUserID2, GSP_USERID, &cbGspUserID );
			::SQLGetData( pConn->hStmt, 2, SQL_C_CHAR, szGspUserIP, GSP_USERIP, &cbGspUserIP );
			::SQLGetData( pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sLoginDate, 0, &cbLoginDate );

			if ( (cbGspUserID != 0) && (cbGspUserID != -1) )
			{
				StringCchCopy(szGspUserID, GSP_USERID, (const TCHAR*) szGspUserID2);
				nRowCount++;
			}

			// ÇÁ¸®¹Ì¾ö ¸¸·á±â°£
			// 1970-02-01 : Default
			// ÅÂ±¹¿¡¼­ TimeZone ¼¼ÆÃÀÌ Æ²·Á¼­ ¿À·ù°¡ ¹ß»ý
			// ÀÌ ÄÚµå¸¦ Ãß°¡·Î »ðÀÔÇÑ´Ù.
			if ( (sLoginDate.year <= 1970) || (sLoginDate.year >= 2999) )
			{
				sLoginDate.year = 1970;
				sLoginDate.month = 2;
				sLoginDate.day = 1;
				sLoginDate.hour = 1;
				sLoginDate.minute = 1;
				sLoginDate.second = 1;
			}					
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pGspDB->FreeConnection( pConn );
	pConn = NULL;
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	// Row Count °¡ 1 ÀÌ ¾Æ´Ï¸é ¿¡·¯ÀÌ´Ù.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		// Check Code
		CConsoleMessage::GetInstance()->Write( _T("DB Check:%s"), szTemp );
		//strTemp.freeze( false );

		CConsoleMessage::GetInstance()->Write( _T("ERROR:GspUserCheck can't get info UserID from UserTemp") );
		
		return DB_ERROR;
	}

	// WEB ·Î±×ÀÎ½Ã°£ ¼¼ÆÃ
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ÇöÀç ½Ã°£À» °¡Á®¿Â´Ù.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (ÇöÀç½Ã°£ - WEB ¿¡¼­ ·Î±×ÀÎÇÑ ½Ã°£) À» °è»êÇÑ´Ù.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (ÇöÀç½Ã°£ - WEB ¿¡¼­ ·Î±×ÀÎÇÑ ½Ã°£) ÀÌ ÀÏÁ¤ÇÑ ¹üÀ§³»¿¡ ÀÖ¾î¾ß ·Î±×ÀÎÀ» ÁøÇàÇÑ´Ù.
	// PC ¹æ µî¿¡¼­ ·Î±×¾Æ¿ô ÇÏÁö ¾Ê°í ÀÚ¸®¸¦ ¶°³­ À¯ÀúµîÀÇ °æ¿ì...
	// µÞ »ç¶÷ÀÌ ¾Õ¿¡ ÇÃ·¹ÀÌ ÇÑ Á¤º¸¸¦ °¡Áö°í ±×´ë·Î µé¾î¿Ã °¡´É¼ºÀÌ ÀÖ´Ù.
	if ( (diffTime >= GSP_ALLOW_TIME_DIFF_MINUS) &&
		 (diffTime <= GSP_ALLOW_TIME_DIFF_PLUS) )
	{
		// Time is ok
		//std::strstream strTemp3;
		TCHAR szTemp[256] = {0};

		//strTemp3 << "{call gsp_user_verify(";
		//strTemp3 << "'" << szGspUserID2 << "',";
		//strTemp3 << "'" << szUserIP << "',";
		//strTemp3 << nSvrGrp << ",";
		//strTemp3 << nSvrNum << ", ?)}";
		//strTemp3 << std::ends;
		_snprintf_s( szTemp, 256, "{call gsp_user_verify('%s','%s',%d,%d,?)}", szGspUserID2, szUserIP, nSvrGrp, nSvrNum );

		int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
		//strTemp3.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
		return nReturn;
	}
	else
	{
		// Time is wrong
		return GSP_ALLOW_TIME_ERROR;
	}
}

/**
 * Terra
 * ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.
 * \param szTerraLoginName 
 * \param szTerraDecodedTID 
 * \param szUsrIP 
 * \param nSvrGrp 
 * \param nSvrNum 
 * \return 
 */
int COdbcManager::TerraUserCheck(
	TCHAR* szTerraLoginName,
	const TCHAR* szTerraDecodedTID,
    const TCHAR* szUsrIP,
	int nSvrGrp,
    int nSvrNum )
{
	// #1 Decoded TID ¸¦ ÀÌ¿ëÇØ¼­ TempUser ¿¡¼­ TLoginName À» °¡Á®¿Â´Ù.
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pTerraDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	// ¸ÕÀú µðÄÚµùµÈ TID ¸¦ ÀÌ¿ëÇØ¼­ TLoginName À» °¡Á®¿Â´Ù.
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	//std::strstream strTemp;
	TCHAR szTemp[256] = {0};

	//strTemp << "SELECT TLoginName ";
	//strTemp << "FROM TempUser WHERE UTid = '";
	//strTemp << szTerraDecodedTID << "'";
	//strTemp << std::ends;
	_snprintf( szTemp, 256, "SELECT TLoginName FROM TempUser WHERE UTid = '%s'", szTerraDecodedTID );
#else
	// ¸»·¹ÀÌÁö¾Æ ·Î±×ÀÎ ¹æ½Ä º¯°æÀ¸·Î Äõ¸® º¯°æ
	//std::strstream strTemp;
	TCHAR szTemp[256] = {0};

	//strTemp << "SELECT LoginID ";
	//strTemp << "FROM TempUser WHERE TKeyUser = '";
	//strTemp << szTerraDecodedTID << "'";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 256, "SELECT LoginID FROM TempUser WHERE TKeyUser = '%s'", szTerraDecodedTID );
#endif

	sReturn = ::SQLExecDirect(pConn->hStmt,
							  (SQLCHAR*) szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pTerraDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

        return DB_ERROR;
	}

	SQLCHAR szTLoginName[TERRA_TLOGIN_NAME+1] = {0};
	SQLINTEGER cbTLoginName = SQL_NTS;
	int nRowCount = 0;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {			
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pTerraDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szTLoginName, TERRA_TLOGIN_NAME, &cbTLoginName); 

			if (cbTLoginName != 0 && cbTLoginName != -1) // TLoginName
			{
				StringCchCopy(szTerraLoginName, TERRA_TLOGIN_NAME+1, (const char*) szTLoginName);
				nRowCount++;
			}
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pTerraDB->FreeConnection(pConn);
	pConn = NULL;
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	// Row Count °¡ 1 ÀÌ ¾Æ´Ï¸é ¿¡·¯ÀÌ´Ù.
	if (nRowCount == 1)	
	{
		nRowCount = 0;
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase(
			_T("ERROR:TerraUserCheck can't get info TLoginName from TempUser") );
		
		return DB_ERROR;
	}
	
	// TLoginName ÀÌ "" ·Î ³ªÅ¸³ª´Â »ç¿ëÀÚ ¿À·ù°¡ ÀÖ¾î ¿¡·¯Ã³¸®ÇÑ´Ù.
	if (strlen(szTerraLoginName) < 2) return DB_ERROR;
    
	///////////////////////////////////////////////////////////
	// TLoginName À» °¡Á®¿ÔÀ¸¸é ÀÌÁ¦ »ç¿ëÀÚ Á¤º¸¸¦ °¡Á®¿Â´Ù.
	// TLoginName (varchar 15)
	// TGender (char 1)
	// TIcNo (char 15)
	// TDOB (char 10)
	/*
	pConn = m_pTerraDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	std::strstream strTemp2;
	strTemp2 << "SELECT TGender, TIcNo, TDOB ";
	strTemp2 << "FROM T_Member WHERE TLoginName = '";
	strTemp2 << szTerraLoginName << "'";
	strTemp2 << std::ends;

	sReturn = ::SQLExecDirect(pConn->hStmt,
							  (SQLCHAR*) strTemp2.str(), 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(strTemp2.str());
		Print(GetErrorString(pConn->hStmt));
		m_pTerraDB->FreeConnection(pConn);
		strTemp2.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
        return DB_ERROR;
	}


	SQLCHAR szTGender[TERRA_TGENDER+1] = {0};
	SQLCHAR szTIcNo[TERRA_TICNO+1]     = {0};
	SQLCHAR szTDOB[TERRA_TDOB+1]       = {0};

	SQLINTEGER cbTGender = SQL_NTS;
	SQLINTEGER cbTIcNo = SQL_NTS;
	SQLINTEGER cbTDOB = SQL_NTS;

    while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {			
            Print(strTemp2.str());		
			Print(GetErrorString(pConn->hStmt));
            m_pTerraDB->FreeConnection(pConn);

			strTemp2.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szTGender, TERRA_TGENDER, &cbTGender);
			::SQLGetData(pConn->hStmt, 2, SQL_C_CHAR, szTIcNo,   TERRA_TICNO,   &cbTIcNo);
			::SQLGetData(pConn->hStmt, 3, SQL_C_CHAR, szTDOB,    TERRA_TDOB,    &cbTDOB);
			nRowCount++;
		}
		else
		{
			break;
		}
	}
	m_pTerraDB->FreeConnection(pConn);
	pConn = NULL;
	strTemp2.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	// Row Count °¡ 1 ÀÌ ¾Æ´Ï¸é ¿¡·¯ÀÌ´Ù.
	if (nRowCount == 1)
	{
		nRowCount = 0;
	}
	else
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
			_T("ERROR:TerraUserCheck can't get info TGender,TIcNo,TDOB from T_Member"));
		return DB_ERROR;
	}
	*/

	/*
	std::strstream strTemp3;
	strTemp3 << "{call terra_user_verify(";
	strTemp3 << "'" << szTerraLoginName << "',";
	strTemp3 << "'" << szTGender << "',";
	strTemp3 << "'" << szTIcNo << "',";
	strTemp3 << "'" << szTDOB << "',";
	strTemp3 << "'" << szUsrIP << "',";
	strTemp3 << nSvrGrp << ",";
	strTemp3 << nSvrNum << ", ?)}";
	strTemp3 << std::ends;
	*/

	//std::strstream strTemp3;
	
	//strTemp3 << "{call terra_user_verify(";
	//strTemp3 << "'" << szTerraLoginName << "',";
	//strTemp3 << "'X',";
	//strTemp3 << "'X',";
	//strTemp3 << "'X',";
	//strTemp3 << "'" << szUsrIP << "',";
	//strTemp3 << nSvrGrp << ",";
	//strTemp3 << nSvrNum << ", ?)}";
	//strTemp3 << std::ends;
	_snprintf_s( szTemp, 256, "{call terra_user_verify('%s','X','X','X','%s',%d,%d,?)}", szTerraLoginName, szUsrIP, nSvrGrp, nSvrNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp3.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
	return nReturn;
}


/**
* ±Û·Î¹ú ¼­ºñ½º
* ·Î±×ÀÎÇÏ·Á´Â »ç¿ëÀÚ¸¦ Ã¼Å©ÇÑ´Ù.	
* \param szUsrID »ç¿ëÀÚ ID
* \param szPasswd »ç¿ëÀÚ ÆÐ½º¿öµå
* \param szUsrIP »ç¿ëÀÚÀÇ IP Address
* \param nSvrGrp ¼­¹ö±×·ì
* \param nSvrNum ¼­¹ö¹øÈ£
* \return DB ÀÎÁõÈÄ °á°ú°ª
*/
int	COdbcManager::GsUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum)
{
	
	/// SQL ¹®¿¡¼­ ' ´Â ¹®ÀÚ¿­ÀÇ ³¡À» ³ªÅ¸³»±â ¶§¹®¿¡
	/// ' ¸¦ '' ·Î Ä¡È¯ÇØ¾ß ÇÑ´Ù.
	/// DB:{?=call user_verify('ÿ?,'±±?ó³ó¿ÿÿÿ?,'61.57.175.205',1,0, ?)}
	/// À§¿Í°°ÀÌ ¹®ÀÚ¿­ Áß°£¿¡ ' °¡ »ðÀÔµÇ¾î ÀÖÀ»°æ¿ì ¿¡·¯°¡ ¹ß»ýÇÏ°Ô µÈ´Ù.
	CString strUserID = szUsrID;
	strUserID.Trim(_T(" "));
	strUserID.Replace(_T("'"), _T("''"));

	CString strPasswd = szPasswd;
	strPasswd.Trim(_T(" "));
	strPasswd.Replace(_T("'"), _T("''"));

	//std::strstream strTemp;
	TCHAR szTemp[512] = {0};

	_snprintf_s( szTemp, 512, "{call gs_user_verify('%s','%s','%s',%d,%d,?)}", 
												strUserID.GetString(),
												strPasswd.GetString(),
												szUsrIP,
												nSvrGrp,
												nSvrNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

	return nReturn;
}

/**
* Daum
* À¯ÀúÀÇ ÆÐ½º¿öµå¸¦ ³Ö°Å³ª Ã¼Å©ÇÑ´Ù.
* \param szDaumGID Daum UserGID
* \param szDaumPass »ç¿ëÀÚ ÆÐ½º¿öµå
* \return 
*/
int COdbcManager::DaumUserPassCheck(
				const TCHAR* szDaumGID,
				const TCHAR* szDaumPasswd,
				int	nCheckFlag )
{
	//std::strstream strTemp;
	TCHAR szTemp[128] = {0};

	//strTemp << "{ call daum_user_passcheck(";
	//strTemp << "'" << szDaumGID << "',";
	//strTemp << "'" << szDaumPasswd << "', ?)}";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 128, "{call daum_user_passcheck('%s','%s',%d,?)}", szDaumGID, szDaumPasswd, nCheckFlag );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
	return nReturn;
}

/**
* Terra
* À¯ÀúÀÇ ÆÐ½º¿öµå¸¦ ³Ö°Å³ª Ã¼Å©ÇÑ´Ù.
* \param szTID Terra UserID
* \param szPasswd »ç¿ëÀÚ ÆÐ½º¿öµå
* \return 
*/
int COdbcManager::TerraUserPassCheck(
				const TCHAR* szTID,
				const TCHAR* szPasswd,
				int	nCheckFlag )
{
	//std::strstream strTemp;
	TCHAR szTemp[128] = {0};

	//strTemp << "{ call terra_user_passcheck(";
	//strTemp << "'" << szTID << "',";
	//strTemp << "'" << szPasswd << "', ?)}";
	//strTemp << std::ends;
	_snprintf_s( szTemp, 128, "{call terra_user_passcheck('%s','%s',%d,?)}", szTID, szPasswd, nCheckFlag );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
	return nReturn;
}



/**
* Excite
* À¯ÀúÀÇ ÆÐ½º¿öµå¸¦ ³Ö°Å³ª Ã¼Å©ÇÑ´Ù.
* \param szUID Excite UserID
* \param szPasswd »ç¿ëÀÚ ÆÐ½º¿öµå
* \return 
*/
int COdbcManager::ExciteUserPassCheck(
				const TCHAR* szUID,
				const TCHAR* szUserID,
				const TCHAR* szPasswd,
				int	nCheckFlag )
{
	TCHAR szTemp[128] = {0};

	_snprintf_s( szTemp, 128, "{call excite_user_passcheck('%s','%s','%s',%d,?)}", szUID, szUserID,szPasswd, nCheckFlag );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
	return nReturn;
}





/**
* Thailand
* »ç¿ëÀÚÀÇ PC¹æ Å¬·¡½º¸¦ Ã¼Å©ÇÑ´Ù.	
* \param szUsrIP »ç¿ëÀÚÀÇ IP Address
* \return DB ÀÎÁõÈÄ °á°ú°ª
*/
int	COdbcManager::ThaiUserClassCheck(	
	const TCHAR* szUsrIP)
{
	SQLRETURN	sReturn = 0;
	int			nOutput = 0;
	char		szClass[2] = {0};
	char		temp[2] = "A";
	char		temp1[2] = "B";
	char		temp2[2] = "C";

	// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
	ODBC_STMT* pConn = m_pThaiDB->GetConnection();
//	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	SQLCHAR szCC_Class[2] = {0}; SQLINTEGER cbCC_Class = SQL_NTS;
	
	TCHAR szTemp[256] = {0};

	_snprintf_s( szTemp, 256, "SELECT CC_CLASS FROM TB_CC_IP WHERE CC_IP='%s'", szUsrIP );

	sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
		m_pThaiDB->FreeConnection(pConn);
//		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.

		return DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(szTemp);
		        Print(GetErrorString(pConn->hStmt));
				// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
				m_pThaiDB->FreeConnection(pConn);
				//m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstreamÀÇ freeze. ¾È ÇÏ¸é Leak ¹ß»ý.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt,  1, SQL_C_CHAR, szCC_Class, 3, &cbCC_Class);

				if( cbCC_Class != 0 && cbCC_Class != -1 )
				::StringCchCopy(szClass, 2, (const char*) szCC_Class);

				if( 0 == strcmp(szClass, temp) )
				{
					nOutput = 1;
				}
				else if( 0 == strcmp(szClass, temp1) )
				{
					nOutput = 2;
				}
				else if( 0 == strcmp(szClass, temp2) )
				{
					nOutput = 3;
				}
				else nOutput = 0;
			}
			else
			{
				break;
			}
		}
	}

	// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
	m_pThaiDB->FreeConnection(pConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}

/**
* Malaysia
* »ç¿ëÀÚÀÇ PC¹æ Å¬·¡½º¸¦ Ã¼Å©ÇÑ´Ù.
* \param szUsrIP »ç¿ëÀÚÀÇ IP Address
* \return DB ÀÎÁõÈÄ °á°ú°ª
*/
int	COdbcManager::MyUserClassCheck(
			const TCHAR* szUsrIP)
{
	SQLRETURN	sReturn = 0;
	int			nOutput = 0;
	char		szClass[2] = {0};
	char		temp[2] = "A";
	char		temp1[2] = "B";
	char		temp2[2] = "C";
	char		temp3[2] = "D";

	// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤

	ODBC_STMT* pConn = m_pMyDB->GetConnection();
//	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	SQLCHAR szCC_Class[2] = {0}; SQLINTEGER cbCC_Class = SQL_NTS;
	
	TCHAR szTemp[256] = {0};

	_snprintf_s( szTemp, 256, "SELECT CC_CLASS FROM TB_CC_IP WHERE CC_IP='%s'", szUsrIP );

	sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);
	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
		m_pThaiDB->FreeConnection(pConn);
		// m_pUserDB->FreeConnection(pConn);

		return DB_ERROR;
	}
	else
	{
		while(true)
		{
            sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(szTemp);
		        Print(GetErrorString(pConn->hStmt));
				// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
				m_pMyDB->FreeConnection(pConn);
				// m_pUserDB->FreeConnection(pConn);

				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt,  1, SQL_C_CHAR, szCC_Class, 3, &cbCC_Class);

				if( cbCC_Class != 0 && cbCC_Class != -1 )
				::StringCchCopy(szClass, 2, (const char*) szCC_Class);

				if( 0 == strcmp(szClass, temp) )
				{
					nOutput = 1;
				}
				else if( 0 == strcmp(szClass, temp1) )
				{
					nOutput = 2;
				}
				else if( 0 == strcmp(szClass, temp2) )
				{
					nOutput = 3;
				}
				else if( 0 == strcmp(szClass, temp3) )
				{
					nOutput = 4;
				}
				else nOutput = 0;
			}
			else
			{
				break;
			}
		}
	}

	// ³»ºÎ Å×½ºÆ®¸¦ À§ÇØ¼­ UserDB OpenÀ¸·Î Å×½ºÆ®¸¦ À§ÇØ¼­ ¼öÁ¤
	m_pMyDB->FreeConnection(pConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}