#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * Daum
 * �α����Ϸ��� ����ڸ� üũ�Ѵ�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

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
		// #1 UUID�� �̿��ؼ� UserTemp ���� UserID, UserIP, LoginDate �� �����´�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			// �����̾� ����Ⱓ
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
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

	// WEB �α��νð� ����
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ���� �ð��� �����´�.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (����ð� - WEB ���� �α����� �ð�) �� ����Ѵ�.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (����ð� - WEB ���� �α����� �ð�) �� ������ �������� �־�� �α����� �����Ѵ�.
	// PC �� ��� �α׾ƿ� ���� �ʰ� �ڸ��� ���� �������� ���...
	// �� ����� �տ� �÷��� �� ������ ������ �״�� ���� ���ɼ��� �ִ�.
	if ( (diffTime >= KOR_ALLOW_TIME_DIFF_MINUS) &&
		 (diffTime <= KOR_ALLOW_TIME_DIFF_PLUS) )
	{
		TCHAR szTemp[256] = {0};

		_snprintf_s( szTemp, 256, "{call daum_user_verify('%s','%s',%d,%d,?)}", szKorUserID, szUserIP, nSvrGrp, nSvrNum );

		int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
		//strTemp3.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
* \param szUserID �����ID
* \param szUsrIP IP
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	
	return nReturn;
}

/**
* Gonzo (Japan)
* \param szUserID �����ID
* \param szUsrIP IP
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
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
 * �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
 * \param szUsrID ����� ID
 * \param szPasswd ����� �н�����
 * \param szUsrIP ������� IP Address
 * \param nSvrGrp �����׷�
 * \param nSvrNum ������ȣ
 * \return DB ������ �����
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
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

/**
* Thailand
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::ThaiUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum )
{
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
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
	//strTemp.freeze ( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

/**
* GSP
* \n �α����Ϸ��� ����ڸ� üũ�Ѵ�.
* \param 
* \param 
* \param szUsrIP �α����Ϸ��� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return 
*/
int COdbcManager::GspUserCheck(
	TCHAR* szGspUserID,
	const TCHAR* szUUID,
	const TCHAR* szUserIP,
	int nSvrGrp,
	int nSvrNum )
{
	// #1 UUID�� �̿��ؼ� UserTemp ���� UserID, UserIP, LoginDate �� �����´�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			// �����̾� ����Ⱓ
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
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

	// WEB �α��νð� ����
    CTime webLoginTime(
			sLoginDate.year,
			sLoginDate.month,
			sLoginDate.day,
			sLoginDate.hour,
			sLoginDate.minute,
			sLoginDate.second );	
	
	// UTC ���� �ð��� �����´�.
	CTime currentTime = CTime::GetCurrentTime(); 
	
	// (����ð� - WEB ���� �α����� �ð�) �� ����Ѵ�.
	__time64_t diffTime = currentTime.GetTime() - webLoginTime.GetTime();
	// (����ð� - WEB ���� �α����� �ð�) �� ������ �������� �־�� �α����� �����Ѵ�.
	// PC �� ��� �α׾ƿ� ���� �ʰ� �ڸ��� ���� �������� ���...
	// �� ����� �տ� �÷��� �� ������ ������ �״�� ���� ���ɼ��� �ִ�.
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
		//strTemp3.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
 * �α����Ϸ��� ����ڸ� üũ�Ѵ�.
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
	// #1 Decoded TID �� �̿��ؼ� TempUser ���� TLoginName �� �����´�.
	SQLRETURN sReturn=0;
	ODBC_STMT* pConn = m_pTerraDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	// ���� ���ڵ��� TID �� �̿��ؼ� TLoginName �� �����´�.
#if defined ( PH_PARAM ) || defined ( VN_PARAM )
	//std::strstream strTemp;
	TCHAR szTemp[256] = {0};

	//strTemp << "SELECT TLoginName ";
	//strTemp << "FROM TempUser WHERE UTid = '";
	//strTemp << szTerraDecodedTID << "'";
	//strTemp << std::ends;
	_snprintf( szTemp, 256, "SELECT TLoginName FROM TempUser WHERE UTid = '%s'", szTerraDecodedTID );
#else
	// ���������� �α��� ��� �������� ���� ����
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

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

			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
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
	
	// TLoginName �� "" �� ��Ÿ���� ����� ������ �־� ����ó���Ѵ�.
	if (strlen(szTerraLoginName) < 2) return DB_ERROR;
    
	///////////////////////////////////////////////////////////
	// TLoginName �� ���������� ���� ����� ������ �����´�.
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
		strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
	strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// Row Count �� 1 �� �ƴϸ� �����̴�.
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
	//strTemp3.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return nReturn;
}


/**
* �۷ι� ����
* �α����Ϸ��� ����ڸ� üũ�Ѵ�.	
* \param szUsrID ����� ID
* \param szPasswd ����� �н�����
* \param szUsrIP ������� IP Address
* \param nSvrGrp �����׷�
* \param nSvrNum ������ȣ
* \return DB ������ �����
*/
int	COdbcManager::GsUserCheck(
	const TCHAR* szUsrID,
	const TCHAR* szPasswd,
	const TCHAR* szUsrIP,
	int nSvrGrp,
	int nSvrNum)
{
	
	/// SQL ������ ' �� ���ڿ��� ���� ��Ÿ���� ������
	/// ' �� '' �� ġȯ�ؾ� �Ѵ�.
	/// DB:{?=call user_verify('�?,'��?�����?,'61.57.175.205',1,0, ?)}
	/// ���Ͱ��� ���ڿ� �߰��� ' �� ���ԵǾ� ������� ������ �߻��ϰ� �ȴ�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

/**
* Daum
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szDaumGID Daum UserGID
* \param szDaumPass ����� �н�����
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return nReturn;
}

/**
* Terra
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szTID Terra UserID
* \param szPasswd ����� �н�����
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return nReturn;
}



/**
* Excite
* ������ �н����带 �ְų� üũ�Ѵ�.
* \param szUID Excite UserID
* \param szPasswd ����� �н�����
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return nReturn;
}





/**
* Thailand
* ������� PC�� Ŭ������ üũ�Ѵ�.	
* \param szUsrIP ������� IP Address
* \return DB ������ �����
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

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
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
		// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
		m_pThaiDB->FreeConnection(pConn);
//		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

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
				// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
				m_pThaiDB->FreeConnection(pConn);
				//m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	m_pThaiDB->FreeConnection(pConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}

/**
* Malaysia
* ������� PC�� Ŭ������ üũ�Ѵ�.
* \param szUsrIP ������� IP Address
* \return DB ������ �����
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

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����

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
		// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
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
				// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
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

	// ���� �׽�Ʈ�� ���ؼ� UserDB Open���� �׽�Ʈ�� ���ؼ� ����
	m_pMyDB->FreeConnection(pConn);
//	m_pUserDB->FreeConnection(pConn);

	return nOutput;
}