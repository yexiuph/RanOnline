#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 대만/홍콩/중국
USER_INFO_BASIC COdbcManager::GetUserInfo(
	const TCHAR* szUsrId,
	const TCHAR* szPasswd )
{
	USER_INFO_BASIC uib;	

	uib.nUserNum = 0;

	if (szUsrId != NULL)
	{
		::StringCchCopy(uib.szUserID, USR_ID_LENGTH+1, szUsrId);
	}

	//std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserID, LastLoginDate ";
	//strTemp << "FROM UserInfo WITH (NOLOCK) WHERE UserID = '";
	//strTemp << szUsrId;
	//strTemp << "' AND UserPass ='";
	//strTemp << szPasswd << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserID, LastLoginDate "
							"FROM UserInfo WITH (NOLOCK) WHERE UserID = '%s' AND UserPass ='%s'",
							szUsrId, szPasswd );

	SQLRETURN	sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return uib;

	SQLINTEGER nUserNum=0, cbUserNum=SQL_NTS;

	sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	else
	{
		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{				
				break;
			}

 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nUserNum, 0, &cbUserNum);
				uib.nUserNum = nUserNum;
			}
			else
			{
				break;		
			}
		}

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	return uib;
}

/**
* 사용자 정보를 가져온다
* Thailand
* \param szUserID 
* \param pMsg 
* \return 
*/
int COdbcManager::ThaiGetUserInfo(
	const TCHAR* szUserID, 
	NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
	SQLINTEGER nUserThaiFlag  = 0, cbUserThaiFlag  = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate, UserThaiFlag FROM UserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
							"ChaTestRemain, ChatBlockDate, UserThaiFlag FROM UserInfo WHERE UserID='%s'",
							szUserID );

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);
				::SQLGetData(pConn->hStmt, 7, SQL_C_LONG,           &nUserThaiFlag,  0, &cbUserThaiFlag);

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;
				pMsg->wThaiFlag      = (WORD) nUserThaiFlag; // Thailand 사용자 타입 플래그

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp,
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* 사용자 정보를 가져와서 Agent 로 전송
*/
int COdbcManager::GetUserInfo(
	const TCHAR* szUserID,
	NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	

    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM UserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;
	TIMESTAMP_STRUCT sLastLoginTime; SQLINTEGER cbLastLoginTime = SQL_NTS;	
#if defined( CH_PARAM )
	// 중국 GameTime Check를 위해서 추가

	SQLINTEGER nChinaGameTime = 0, cbChinaGameTime = SQL_NTS;
	SQLINTEGER nChinaOfflineTime = 0, cbChinaOfflineTime = SQL_NTS;
	SQLINTEGER nChinaUserAge = 0, cbChinaUserAge = SQL_NTS;


	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
							"ChaTestRemain, ChatBlockDate, GameTime, OfflineTime, LastLoginDate, UserAge FROM UserInfo WHERE UserID='%s'",
							szUserID );
#elif defined(_RELEASED)

	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
		"ChaTestRemain, ChatBlockDate, LastLoginDate FROM UserInfo WHERE UserID='%s'",
		szUserID );

#else

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
		"ChaTestRemain, ChatBlockDate FROM UserInfo WHERE UserID='%s'",
		szUserID );
#endif

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

		return DB_ERROR;
	}
	else
	{
#if defined( CH_PARAM )
		while(true)
		{
            sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(szTemp);
		        Print(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

				::SQLGetData(pConn->hStmt, 7, SQL_C_LONG,           &nChinaGameTime, 0, &cbChinaGameTime);
				::SQLGetData(pConn->hStmt, 8, SQL_C_LONG,           &nChinaOfflineTime, 0, &cbChinaOfflineTime);
				::SQLGetData(pConn->hStmt, 9, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);
				::SQLGetData(pConn->hStmt, 10, SQL_C_LONG,           &nChinaUserAge, 0, &cbChinaUserAge);


                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간				
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();

				pMsg->nChinaGameTime	= nChinaGameTime;
				pMsg->nChinaOfflineTime	= nChinaOfflineTime;

				CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
                             sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
				pMsg->tLastLoginTime = cTemp3.GetTime();
				
				pMsg->nChinaUserAge = nChinaUserAge;
			}
			else
			{
				break;
			}
        }
 
#else
		while(true)
		{
            sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(szTemp);
		        Print(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);
#if defined(_RELEASED)
				::SQLGetData(pConn->hStmt, 7, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);
#endif

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간				
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();


#if defined(_RELEASED)
				CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
					sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
				pMsg->tLastLoginTime = cTemp3.GetTime();
#endif

			}
			else
			{
				break;
			}
        }
#endif
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* DAUM : 사용자 정보를 가져와서 Agent 로 전송
*/
int COdbcManager::DaumGetUserInfo(
	const TCHAR* szGID,
	DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
    SQLRETURN	sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum  = 0,  cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM DaumUserInfo WHERE UserUID='" << szUID << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
							"ChaTestRemain, ChatBlockDate FROM DaumUserInfo WHERE UserGID='%s'",
							szGID );

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

    // 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	
	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.	

	m_pGameDB->FreeConnection(pConn);

	// Daum 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
	//std::strstream strTemp2;
	//strTemp2 << "SELECT CheckFlag FROM ";
	//strTemp2 << "DaumUserInfo WHERE UserUID='" << szUID << "'";
	//strTemp2 << std::ends;

	_snprintf_s( szTemp, 256, "SELECT CheckFlag FROM DaumUserInfo WHERE UserGID='%s'", szGID );

	sReturn = ::SQLExecDirect( pConn->hStmt, ( SQLCHAR*)szTemp, SQL_NTS );

	if( (sReturn != SQL_SUCCESS ) && (sReturn != SQL_SUCCESS_WITH_INFO ))
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection( false );

		//strTemp2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while( true )
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pUserDB->FreeConnection(pConn);

			//strTemp2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}
		if( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag );

			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pUserDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* GSP 사용자 정보를 가져온다.
* \param szUserID UserID varchar(50)
* \param pMsg
* \return 
*/
int COdbcManager::GspGetUserInfo(
	const TCHAR* szUserID,
	GSP_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate ";
	//strTemp << "FROM GspUserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate "
							"FROM GspUserInfo WHERE UserID='%s'", szUserID );

    sReturn = ::SQLExecDirect(
					pConn->hStmt,
					(SQLCHAR*)szTemp,
					SQL_NTS );

	if ( (sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO) )
	{
        Print( szTemp );
		Print( GetErrorString( pConn->hStmt ) );
		m_pUserDB->FreeConnection( pConn );
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}
	else
	{
		while (true)
		{
            sReturn = ::SQLFetch( pConn->hStmt );
			if ( (sReturn == SQL_ERROR) || (sReturn == SQL_SUCCESS_WITH_INFO) )
			{
                Print( szTemp );
		        Print( GetErrorString( pConn->hStmt ) );
				m_pUserDB->FreeConnection( pConn );

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if ( (sReturn == SQL_SUCCESS) || (sReturn == SQL_SUCCESS_WITH_INFO) )
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);
    return DB_OK;
}

// 말레이지아
int COdbcManager::TerraGetUserInfo(
	const TCHAR* szTLoginName, 
	TERRA_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate ";
	//strTemp << "FROM TerraUserInfo WHERE TLoginName='" << szTLoginName << "'";
	//strTemp << std::ends;

#if defined( VN_PARAM ) //vietnamtest%%%
// 베트남 탐닉 방지 시스템 추가

	SQLINTEGER nVTGameTime = 0, cbVTGameTime = SQL_NTS;
	TIMESTAMP_STRUCT sLastLoginTime; SQLINTEGER cbLastLoginTime = SQL_NTS;	

	TCHAR szTemp[256] = {0};
	_snprintf( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
			   "ChaTestRemain, ChatBlockDate, GameTime, LastLoginDate FROM TerraUserInfo WHERE TLoginName='%s'", 
			   szTLoginName );
#else

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate "
							"FROM TerraUserInfo WHERE TLoginName='%s'", szTLoginName );
#endif

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

		return DB_ERROR;
	}
	else
	{
#if defined( VN_PARAM ) //vietnamtest%%%
		while(true)
		{
			sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
				Print(szTemp);
				Print(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
				::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
				::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
				::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
				::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

				::SQLGetData(pConn->hStmt, 7, SQL_C_LONG,           &nVTGameTime, 0, &cbVTGameTime);
				::SQLGetData(pConn->hStmt, 8, SQL_C_TYPE_TIMESTAMP, &sLastLoginTime, 0, &cbLastLoginTime);

				pMsg->nUserNum       = nUserNum;
				pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
				pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

				// 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
				CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
					sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
				pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간				
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
					sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();

				pMsg->nVTGameTime	= nVTGameTime;

				CTime cTemp3(sLastLoginTime.year, sLastLoginTime.month,  sLastLoginTime.day,
					sLastLoginTime.hour, sLastLoginTime.minute, sLastLoginTime.second);
				pMsg->tLastLoginTime = cTemp3.GetTime();
			}
			else
			{
				break;
			}
		}
#else
		while(true)
		{
            sReturn = ::SQLFetch(pConn->hStmt);
			if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
			{
                Print(szTemp);
		        Print(GetErrorString(pConn->hStmt));
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }
#endif
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);
/*	
	// 말레이지아 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
	//std::strstream strTemp1;
	//strTemp1 << "SELECT CheckFlag FROM ";
	//strTemp1 << "TerraUserInfo WHERE TLoginName='" << szTLoginName << "'";
	//strTemp1 << std::ends;

	_snprintf( szTemp, 256, "SELECT CheckFlag FROM "
							"TerraUserInfo WHERE TLoginName='%s'",
							szTLoginName );

	sReturn = ::SQLExecDirect( pConn->hStmt, ( SQLCHAR*)szTemp, SQL_NTS );

	if( (sReturn != SQL_SUCCESS ) && (sReturn != SQL_SUCCESS_WITH_INFO ))
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection( false );

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while( true )
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pUserDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}
		if( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag );

			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pUserDB->FreeConnection(pConn);
*/
    return DB_OK;
}

/**
* Excite 사용자 정보를 가져온다.일본.
* \param szUserID UserID varchar(20)
* \param pMsg
* \return 
*/
int COdbcManager::ExciteGetUserInfo(
		const TCHAR* szUserID,
		EXCITE_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;


    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;
    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate ";
	//strTemp << "FROM ExciteUserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate "
							"FROM ExciteUserInfo WHERE UserID='%s'",
							szUserID );

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);	

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);
	
	// 일본 패스워드 입력을 위해서 코드를 추가한다.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User의 패스워드 입력 여부 판단
	_snprintf_s( szTemp, 256, "SELECT CheckFlag FROM "
							"ExciteUserInfo WHERE UserID='%s'",
							szUserID );

	sReturn = ::SQLExecDirect( pConn->hStmt, ( SQLCHAR*)szTemp, SQL_NTS );

	if( (sReturn != SQL_SUCCESS ) && (sReturn != SQL_SUCCESS_WITH_INFO ))
	{
		Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection( false );

		return DB_ERROR;
	}

	while( true )
	{
		sReturn = ::SQLFetch( pConn->hStmt );
		if( sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pUserDB->FreeConnection(pConn);

            return DB_ERROR;
		}
		if( sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO )
		{
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nCheckFlag, 0, &cbCheckFlag );

			pMsg->nCheckFlag = nCheckFlag;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}	

	m_pUserDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* Japan(Gonzo) 사용자 정보를 가져온다.
* \param szUserID UserID varchar(16)
* \param pMsg
* \return 
*/
int COdbcManager::JapanGetUserInfo( const TCHAR* szUserID, JAPAN_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;


    SQLINTEGER nUserNum  = 0, cbUserNum   = SQL_NTS;
    SQLINTEGER nUserType = 0, cbUserType   = SQL_NTS;
    SQLINTEGER nChaRemain = 0, cbChaRemain = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS; 

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate "
							"FROM JapanUserInfo WHERE UserID='%s'",
							szUserID );

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO))
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

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
				m_pUserDB->FreeConnection(pConn);
				
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum  = nUserNum;
                pMsg->nUserType = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);
		
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* 사용자 정보를 가져와서 Agent 로 전송
*/
int COdbcManager::GsGetUserInfo(
	const TCHAR* szUserID,
	GS_NET_LOGIN_FEEDBACK_DATA2* pMsg )
{
	SQLRETURN	sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn)	return DB_ERROR;

    SQLINTEGER nUserNum       = 0, cbUserNum       = SQL_NTS;
    SQLINTEGER nUserType      = 0, cbUserType      = SQL_NTS;
    SQLINTEGER nChaRemain     = 0, cbChaRemain     = SQL_NTS;
    SQLINTEGER nChaTestRemain = 0, cbChaTestRemain = SQL_NTS;
    
    TIMESTAMP_STRUCT sPremiumDate;   SQLINTEGER cbPremiumDate   = SQL_NTS;
	TIMESTAMP_STRUCT sChatBlockDate; SQLINTEGER cbChatBlockDate = SQL_NTS;	

    
 //   std::strstream strTemp;
	//strTemp << "SELECT UserNum, UserType, PremiumDate, ChaRemain, ChaTestRemain, ChatBlockDate FROM UserInfo WHERE UserID='" << szUserID << "'";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT UserNum, UserType, PremiumDate, ChaRemain, "
		"ChaTestRemain, ChatBlockDate FROM GSUserInfo WHERE UserID='%s'",
		szUserID );

    sReturn = ::SQLExecDirect(pConn->hStmt, 
							  (SQLCHAR*)szTemp, 
							  SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				return DB_ERROR;
			}
 			if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
			{	
				::SQLGetData(pConn->hStmt, 1, SQL_C_LONG,           &nUserNum,       0, &cbUserNum);
                ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,           &nUserType,      0, &cbUserType);
                ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sPremiumDate,   0, &cbPremiumDate);
                ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,           &nChaRemain,     0, &cbChaRemain);
                ::SQLGetData(pConn->hStmt, 5, SQL_C_LONG,           &nChaTestRemain, 0, &cbChaTestRemain);
				::SQLGetData(pConn->hStmt, 6, SQL_C_TYPE_TIMESTAMP, &sChatBlockDate, 0, &cbChatBlockDate);

                pMsg->nUserNum       = nUserNum;
                pMsg->nUserType      = nUserType;
				pMsg->uChaRemain     = (USHORT) nChaRemain;
                pMsg->uChaTestRemain = (USHORT) nChaTestRemain;

                // 프리미엄 만료기간
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sPremiumDate.year <= 1970 || sPremiumDate.year >= 2999)
				{
					sPremiumDate.year = 1970;
					sPremiumDate.month = 2;
					sPremiumDate.day = 1;
					sPremiumDate.hour = 1;
					sPremiumDate.minute = 1;
					sPremiumDate.second = 1;
				}
                CTime cTemp(sPremiumDate.year, sPremiumDate.month,  sPremiumDate.day, 
                            sPremiumDate.hour, sPremiumDate.minute, sPremiumDate.second);
                pMsg->tPremiumTime   = cTemp.GetTime();

				// 채팅블록시간				
				// 1970-02-01 : Default
				// 태국에서 TimeZone 세팅이 틀려서 오류가 발생
				// 이 코드를 추가로 삽입한다.
				if (sChatBlockDate.year <= 1970 || sChatBlockDate.year >= 2999)
				{
					sChatBlockDate.year = 1970;
					sChatBlockDate.month = 2;
					sChatBlockDate.day = 1;
					sChatBlockDate.hour = 1;
					sChatBlockDate.minute = 1;
					sChatBlockDate.second = 1;
				}
				CTime cTemp2(sChatBlockDate.year, sChatBlockDate.month,  sChatBlockDate.day,
                             sChatBlockDate.hour, sChatBlockDate.minute, sChatBlockDate.second);
				pMsg->tChatBlockTime = cTemp2.GetTime();
			}
			else
			{
				break;
			}
        }
		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}
	m_pUserDB->FreeConnection(pConn);

	// 극강부 추가
	// 극강부 생성 가능 여부를 체크하기 위해서
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// 극강부 생성가능 여부 체크 
	//std::strstream strTemp1;
	//strTemp1 << "Exec sp_Extreme ";
	//strTemp1 << nUserNum;
	//strTemp1 << std::ends;

	_snprintf_s( szTemp, 256, "Exec sp_Extreme %d", nUserNum );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData( pConn->hStmt, 1, SQL_C_LONG, &nExtremeM, 0, &cbExtremeM );
			::SQLGetData( pConn->hStmt, 2, SQL_C_LONG, &nExtremeW, 0, &cbExtremeW );
			pMsg->nExtremeM = nExtremeM;
			pMsg->nExtremeW = nExtremeW;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	//strTemp1.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* 태국:해당 사용자의 오늘 게임플레이 시간(분)을 가져온다.
* \param nUserNum 
* \return 
*/
WORD COdbcManager::ThaiGetTodayPlayTime( int nUserNum )
{
	//std::strstream strTemp;
	//strTemp << "{call Thai_GetGameTime(";
	//strTemp << nUserNum << ", ?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call Thai_GetGameTime(%d,?)}", nUserNum );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	if (nReturn == DB_ERROR)
	{
		return 0;
	}
	else
	{
		return (WORD) nReturn;
	}
}