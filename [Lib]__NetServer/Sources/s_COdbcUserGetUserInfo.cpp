#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �븸/ȫ��/�߱�
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	return uib;
}

/**
* ����� ������ �����´�
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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
				pMsg->wThaiFlag      = (WORD) nUserThaiFlag; // Thailand ����� Ÿ�� �÷���

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	// �ذ��� �߰�
	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection();
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* ����� ������ �����ͼ� Agent �� ����
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
	// �߱� GameTime Check�� ���ؼ� �߰�

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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

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

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�				
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�				
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	// �ذ��� �߰�
	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* DAUM : ����� ������ �����ͼ� Agent �� ����
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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

    // �ذ��� �߰�
	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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
	
	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.	

	m_pGameDB->FreeConnection(pConn);

	// Daum �н����� �Է��� ���ؼ� �ڵ带 �߰��Ѵ�.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User�� �н����� �Է� ���� �Ǵ�
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

		//strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp2.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pUserDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* GSP ����� ������ �����´�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);
    return DB_OK;
}

// ����������
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
// ��Ʈ�� Ž�� ���� �ý��� �߰�

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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

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

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

				// �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�				
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	// �ذ��� �߰�
	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pGameDB->FreeConnection(pConn);
/*	
	// ���������� �н����� �Է��� ���ؼ� �ڵ带 �߰��Ѵ�.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User�� �н����� �Է� ���� �Ǵ�
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pUserDB->FreeConnection(pConn);
*/
    return DB_OK;
}

/**
* Excite ����� ������ �����´�.�Ϻ�.
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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);
	
	// �Ϻ� �н����� �Է��� ���ؼ� �ڵ带 �߰��Ѵ�.
	sReturn = 0;
	pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLINTEGER nCheckFlag = 0, cbCheckFlag = SQL_NTS;

	// User�� �н����� �Է� ���� �Ǵ�
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
* Japan(Gonzo) ����� ������ �����´�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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
* ����� ������ �����ͼ� Agent �� ����
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
				m_pUserDB->FreeConnection(pConn);

				//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

                // �����̾� ����Ⱓ
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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

				// ä�ú�Ͻð�				
				// 1970-02-01 : Default
				// �±����� TimeZone ������ Ʋ���� ������ �߻�
				// �� �ڵ带 �߰��� �����Ѵ�.
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
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}
	m_pUserDB->FreeConnection(pConn);

	// �ذ��� �߰�
	// �ذ��� ���� ���� ���θ� üũ�ϱ� ���ؼ�
	sReturn = 0;
	pConn = m_pGameDB->GetConnection(); 
	if (!pConn)	return DB_ERROR;

	SQLINTEGER nExtremeM = 0, cbExtremeM = SQL_NTS;
	SQLINTEGER nExtremeW = 0, cbExtremeW = SQL_NTS;

	// �ذ��� �������� ���� üũ 
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

		//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

			//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

	//strTemp1.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pGameDB->FreeConnection(pConn);

    return DB_OK;
}

/**
* �±�:�ش� ������� ���� �����÷��� �ð�(��)�� �����´�.
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
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	
	if (nReturn == DB_ERROR)
	{
		return 0;
	}
	else
	{
		return (WORD) nReturn;
	}
}