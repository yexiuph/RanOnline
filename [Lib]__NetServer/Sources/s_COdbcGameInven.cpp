#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////
// �ش� ��Ŀ ����ð��� ���Ѵ�.
// 1 �� 5 �� ��Ŀ�� ���� �� ����.
// 2, 3, 4 �� ��Ŀ�� ����ð��� ���� �� �ִ�.
// �Է�
// nChaNum : ĳ���͹�ȣ
// nStorageNum : ���丮�� ��ȣ
// tTime : ����ð�
// ���
// -1 : DB_ERROR
// 0 : DB_OK
int COdbcManager::SetChaStorageDate(int nUserNum, int nStorageNum, __time64_t tTime)
{
    if (nStorageNum < 1 || nStorageNum > 3)
    {
        return DB_ERROR;
    }

    CTime cTemp(tTime);
    CString strTime = cTemp.Format("%Y-%m-%d %H:%M:%S");

	TCHAR szTemp[128] = {0};
	
    //std::strstream strTemp;
    //strTemp << "Update UserInven Set ";

    switch (nStorageNum)
    {
    case 1:
		_snprintf_s( szTemp, 128, "Update UserInven Set ChaStorage2='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage2='" << strTime.GetString() << "'";
        break;
    case 2:
		_snprintf_s( szTemp, 128, "Update UserInven Set ChaStorage3='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage3='" << strTime.GetString() << "'";
        break;
    case 3:
		_snprintf_s( szTemp, 128, "Update UserInven Set ChaStorage4='%s' WHERE UserNum=%d", strTime.GetString(), nUserNum );
        //strTemp << "ChaStorage4='" << strTime.GetString() << "'";
        break;
    default:
        return DB_ERROR;
        break;
    }

    //strTemp << " WHERE UserNum=" << nUserNum;
    //strTemp << std::ends;

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// �ش� ĳ������ �κ��丮 �߰� �ټ��� �����Ѵ�.
// �Է�
// nChaNum : ĳ���͹�ȣ
// wInvenLine : �߰��ټ� 
// ���� 
// ���� 0 ���� ���� �߰��� 1 �� �Է�
// ���� ���� ���¿��� �� ���� �߰��� 2 �� �Է� (���� �ټ��� �Է��Ѵ�)
// ���� ���� ���¿��� ���� �߰��� 3 �� �Է�
// �ִ� 3������ �Է°���.
int COdbcManager::SetChaInvenNum(int nChaNum, WORD wInvenLine)
{
    if (wInvenLine < 1 || wInvenLine > 5 || nChaNum <= 0)
    {
        return DB_ERROR;
    }

    //std::strstream strTemp;
    //strTemp << "Update ChaInfo Set ChaInvenLine=";
    //strTemp << wInvenLine;
    //strTemp << " WHERE ChaNum=" << nChaNum;
    //strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Update ChaInfo Set ChaInvenLine=%u  WHERE ChaNum=%d", wInvenLine, nChaNum );

	int nReturn = m_pGameDB->ExecuteSQL(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// ���� �κ��丮�� �д´�
int COdbcManager::ReadUserInven(SCHARDATA2* pChaData2)
{    
	bool bInven = CheckInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->GetUserID());
	if (!bInven)
	{
		MakeUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->GetUserID());
	}

	// �����κ��丮, money
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	SQLRETURN sReturn = 0;
	int nUserNum = 0;
	
	//std::strstream strTemp;
	//strTemp << "SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 FROM UserInven WHERE UserNum=" << pChaData2->m_dwUserID; // << " AND ";
	//// strTemp << "SGNum=" << CCfg::GetInstance()->GetServerGroup() ;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT UserMoney, ChaStorage2, ChaStorage3, ChaStorage4 FROM UserInven WHERE UserNum=%u", pChaData2->GetUserID() );
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
    {
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));
        m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
        return DB_ERROR;
    }

	LONGLONG   llUserMoney = 0;
    SQLINTEGER cbUserMoney = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate2; SQLINTEGER cbStorageDate2 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate3; SQLINTEGER cbStorageDate3 = SQL_NTS;
    TIMESTAMP_STRUCT sStorageDate4; SQLINTEGER cbStorageDate4 = SQL_NTS;

	while(true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);
		    Print(GetErrorString(pConn->hStmt));
			m_pGameDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
            return DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
            ::SQLGetData(pConn->hStmt, 1, SQL_C_SBIGINT,        &llUserMoney,   0, &cbUserMoney);
            ::SQLGetData(pConn->hStmt, 2, SQL_C_TYPE_TIMESTAMP, &sStorageDate2, 0, &cbStorageDate2);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_TYPE_TIMESTAMP, &sStorageDate3, 0, &cbStorageDate3);
            ::SQLGetData(pConn->hStmt, 4, SQL_C_TYPE_TIMESTAMP, &sStorageDate4, 0, &cbStorageDate4);

            pChaData2->m_lnStorageMoney = llUserMoney;

            // �κ��丮 ��븸��Ⱓ
			
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if (sStorageDate2.year <= 1970 || sStorageDate2.year >= 2999)
			{
				sStorageDate2.year = 1970;
				sStorageDate2.month = 2;
				sStorageDate2.day = 1;
				sStorageDate2.hour = 1;
				sStorageDate2.minute = 1;
				sStorageDate2.second = 1;
			}
            CTime cTemp2(sStorageDate2.year, sStorageDate2.month,  sStorageDate2.day, 
                         sStorageDate2.hour, sStorageDate2.minute, sStorageDate2.second);
            pChaData2->m_tSTORAGE[0] = cTemp2.GetTime();

            // 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if (sStorageDate3.year <= 1970 || sStorageDate3.year >= 2999)
			{
				sStorageDate3.year = 1970;
				sStorageDate3.month = 2;
				sStorageDate3.day = 1;
				sStorageDate3.hour = 1;
				sStorageDate3.minute = 1;
				sStorageDate3.second = 1;
			}
            CTime cTemp3(sStorageDate3.year, sStorageDate3.month,  sStorageDate3.day, 
                         sStorageDate3.hour, sStorageDate3.minute, sStorageDate3.second);
            pChaData2->m_tSTORAGE[1] = cTemp3.GetTime();

			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if (sStorageDate4.year <= 1970 || sStorageDate4.year >= 2999)
			{
				sStorageDate4.year = 1970;
				sStorageDate4.month = 2;
				sStorageDate4.day = 1;
				sStorageDate4.hour = 1;
				sStorageDate4.minute = 1;
				sStorageDate4.second = 1;
			}
            CTime cTemp4(sStorageDate4.year, sStorageDate4.month,  sStorageDate4.day, 
                         sStorageDate4.hour, sStorageDate4.minute, sStorageDate4.second);
            pChaData2->m_tSTORAGE[2] = cTemp4.GetTime();
		}
		else
		{
			break;		
		}
		Sleep( 0 );
    }
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	// image
	CByteStream ByteStream;
	sReturn = ReadUserInven(CCfg::GetInstance()->GetServerGroup(), pChaData2->GetUserID(), ByteStream);
	if (sReturn == DB_ERROR)
		return DB_ERROR;

	pChaData2->SETSTORAGE_BYBUF(ByteStream);
	return DB_OK;
}

///////////////////////////////////////////////////////////////////////////
// ����� �κ��丮���� ����Ÿ�� �о�´�.	
int	COdbcManager::ReadUserInven(int SGNum, DWORD dwUserNum, CByteStream &ByteStream)
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	ByteStream.ClearBuffer();
			
	//std::strstream strTemp;
	//strTemp << "SELECT UserInven.UserInven FROM UserInven where (UserNum=" <<  dwUserNum << ")";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT UserInven.UserInven FROM UserInven where (UserNum=%u)", dwUserNum );

	// Create a result
	sReturn = ::SQLExecDirect(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{
        Print(szTemp);
		Print(GetErrorString(pConn->hStmt));		
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}	
	
	// Retrieve and display each row of data.
	BYTE pBuffer[DB_IMAGE_BUF_SIZE] = {0};
	SQLINTEGER  lSize=0, lTotalSize=0;

	while ((sReturn = ::SQLFetch(pConn->hStmt)) != SQL_NO_DATA) 
	{			
		while (1) 
		{
			::ZeroMemory(pBuffer, DB_IMAGE_BUF_SIZE);
			lSize = 0;
			sReturn = ::SQLGetData(pConn->hStmt, 1, SQL_C_BINARY, pBuffer, DB_IMAGE_BUF_SIZE, &lSize);
			if (lSize > 0)
			{
				if (lSize > DB_IMAGE_BUF_SIZE) lSize = DB_IMAGE_BUF_SIZE;
				lTotalSize += lSize;
				ByteStream.WriteBuffer((LPBYTE) pBuffer, lSize);
			}
			if (sReturn == SQL_NO_DATA || lSize == 0)
				break;
			Sleep( 0 );
		}
		if ( lTotalSize < DB_IMAGE_MIN_SIZE )		
			ByteStream.ClearBuffer ();
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);
	return lTotalSize;
}

///////////////////////////////////////////////////////////////////////////
// �����κ��丮�� �ִ��� üũ�Ѵ�.	
bool COdbcManager::CheckInven(int SGNum, DWORD dwUserNum)
{
	//std::strstream strTemp;
	//strTemp << "{call GetInvenCount(";	
	//strTemp << dwUserNum;
	//strTemp << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call GetInvenCount(%u,?)}", dwUserNum );

	if (m_pGameDB->ExecuteSpInt(szTemp) > 0)
	{
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return true;
	}
	else
	{
		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return false;
	}
}

int COdbcManager::MakeUserInven(int SGNum, DWORD dwUserNum)
{	
	//std::strstream strTemp;
	//strTemp << "{call MakeUserInven(";
	//strTemp << SGNum << ",";
	//strTemp << dwUserNum << ",";
	//strTemp << "?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call MakeUserInven(%d,%u,?)}", SGNum, dwUserNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// ����� �κ��丮�� ����Ÿ�� �����Ѵ�.
int	COdbcManager::WriteUserInven(
	int SGNum, 
	LONGLONG llMoney /* LONGLONG __int64 */, 
	DWORD dwUserNum, 
	BYTE* pData, 
	int nSize)
{	
	/*
    // User Inventory
	// 1. Check User Inven		
	// 2. If exist skip
	// 3. not exist, write iventory image
	bool bInven = CheckInven(SGNum, dwUserNum);
	if (!bInven)
	{
		MakeUserInven(SGNum, dwUserNum);
	}
	*/

	// Update Money
	//std::strstream strMoney;
	//strMoney << "{call UpdateUserMoney(";
	//strMoney << dwUserNum;
	//strMoney << ",";
	//strMoney << llMoney;
	//strMoney << ")}";
	//strMoney << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateUserMoney(%u,%I64d)}", dwUserNum, llMoney );

	m_pGameDB->ExecuteSp( szTemp );
	//strMoney.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	
	// Update Image
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn)		return DB_ERROR;

	//std::strstream strTemp;
	//strTemp << "UPDATE UserInven SET UserInven = ? WHERE (UserNum=" <<  dwUserNum << ") ";
	//strTemp << std::ends;

	_snprintf_s( szTemp, 128, "UPDATE UserInven SET UserInven = ? WHERE (UserNum=%u)", dwUserNum );

	SQLRETURN	sReturn = 0; // SQL return value (short)	

	// Prepare parameter
	sReturn = ::SQLPrepare(pConn->hStmt, (SQLCHAR*)szTemp, SQL_NTS);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}	
	
	// Bind parameter
	SQLINTEGER cbBinaryParam = SQL_LEN_DATA_AT_EXEC(0);

	sReturn = ::SQLBindParameter(pConn->hStmt, 
								1, 
								SQL_PARAM_INPUT,
								SQL_C_BINARY, 
								SQL_LONGVARBINARY,
								nSize, 
								0, 
								(SQLPOINTER) 2, 
								0, 
								&cbBinaryParam);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}

	// Execute SQL
	sReturn = ::SQLExecute(pConn->hStmt);
	if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
        return DB_ERROR;
	}
	
	SQLPOINTER pToken = NULL;
	while (sReturn == SQL_NEED_DATA) 
	{
		sReturn = ::SQLParamData(pConn->hStmt, &pToken);
		if (sReturn == SQL_NEED_DATA) 
		{				
			::SQLPutData(pConn->hStmt, pData, (SQLINTEGER) nSize);
		}
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return DB_OK;
}


int COdbcManager::UpdateUserMoneyAdd( DWORD dwUserID, LONGLONG lnUserMoney )
{
	if ( lnUserMoney <= 0 )	return DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_UpdateUserMoneyAdd(%u,%I64d,?)}", dwUserID, lnUserMoney );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);

	return nReturn;
}

int COdbcManager::UpdateUserLastInfoAdd( DWORD dwUserID, LONGLONG lnUserMoney )
{
	if ( lnUserMoney <= 0 )	return DB_ERROR;

	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT UserMoney FROM UserLastInfo WHERE UserNum=%u", dwUserID );

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

	// UserLastInfo Table�� �ݾ� �÷��� ��ȣȭ�� ���ؼ� ChaMoney �÷��� DataType�� int -> varchar(100)���� �����Ѵ�.
	//	LONGLONG   llUserSaveMoney = 0;
	//	SQLINTEGER cbUserSaveMoney = SQL_NTS;	
	SQLCHAR    szTempStorageMoney[CHAR_TEMPMONEY] = {0}; 
	SQLINTEGER cbTempStorageMoney = SQL_NTS;

	LONGLONG lnTempMoney = 0;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szTempStorageMoney, CHAR_TEMPMONEY, &cbTempStorageMoney);

			if (cbTempStorageMoney != 0 && cbTempStorageMoney != -1) // TempStorageMoney �� ����
			{
				lnTempMoney = 0;
				
				char szTempStorageMoney1[CHAR_TEMPMONEY], szTemp[2];				

				::StringCchCopy(szTempStorageMoney1, CHAR_TEMPMONEY, (const char*) szTempStorageMoney);

				size_t i;
				for( i = 0; i < strlen(szTempStorageMoney1); i++ )
				{
					szTempStorageMoney1[i] -= 20;
					sprintf_s( szTemp, "%c", szTempStorageMoney1[i] );
					lnTempMoney = ( lnTempMoney * 10 ) + atoi(szTemp);
				}

				lnTempMoney = lnTempMoney >> 0x04;
			}
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}

	char szTempMoney[CHAR_TEMPMONEY];

	lnTempMoney += lnUserMoney;

	lnTempMoney <<= 0x04;

	sprintf_s( szTempMoney, "%I64d", lnTempMoney );
	for( int i = 0; i < (int)strlen(szTempMoney); i++ )
	{
		szTempMoney[i] += 20;
	}

	TCHAR szTemp2[256] = {0};
	_snprintf_s( szTemp2, 512, "Exec sp_InsertUserLastInfo %u, '%s'",
		dwUserID,szTempMoney);

	if (m_pGameDB->ExecuteSQL(szTemp2) != DB_OK)
	{
		return DB_ERROR;
	}


	m_pGameDB->FreeConnection(pConn);
	
	
	return DB_OK;
}
