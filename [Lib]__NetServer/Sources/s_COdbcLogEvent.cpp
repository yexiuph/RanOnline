#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// �������� PC �� ���θ�� ���ǰ���'��������!!' �̺�Ʈ
int	COdbcManager::LogLottery(CString strLottery, CString strIP, CString strUserUID, int nUserNum)
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_lottery_check(";
	strTemp << "'" << strLottery.GetString() << "',";
	strTemp << "'" << strIP.GetString()      << "',";
	strTemp << "'" << strUserUID.GetString() << "',";
	strTemp << nUserNum << ", ?)}";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
    _snprintf_s( szTemp, 128, "{call sp_lottery_check('%s','%s','%s',%d,?)}", 
												strLottery.GetString(), strIP.GetString(), strUserUID.GetString(),
												nUserNum);

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

// �������� PC ���θ�� ������ '����' �̺�Ʈ
int COdbcManager::LogPungPungWrite(CString strIP, CString strUserUID, int nUserNum, int nPrize)
{
/*
    std::strstream strTemp;
	strTemp << "{call sp_LogPungPung(";	
	strTemp << "'" << strIP.GetString()      << "',";
	strTemp << "'" << strUserUID.GetString() << "',";
	strTemp << nUserNum << ",";
    strTemp << nPrize << ")}";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
    _snprintf_s( szTemp, 128, "{call sp_LogPungPung('%s','%s',%d,%d)}", 
												strIP.GetString(), strUserUID.GetString(), nUserNum, nPrize);

	int nReturn = m_pLogDB->ExecuteSp(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	
	return nReturn;
}

///////////////////////////////////////////////////////////////////////////////
// �������� PC ���θ�� ������ '����' �̺�Ʈ
// PC �濡�� �Ϸ翡 �ѻ���� �ѹ��� ����
// ���ϰ�
// 1 : �̺�Ʈ ���𰡴�
// 2 : �̹� �̺�Ʈ�� ��������, �Ϸ翡 �ѹ��� ����.
// 3 : ����Ұ� PC �� IP �ƴ�
int COdbcManager::LogPungPungCheck(CString strIP, int nUserNum)
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_PungPungCheck(";	
	strTemp << "'" << strIP.GetString() << "',";		
	strTemp << nUserNum << ",?)}";
	strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
    _snprintf_s( szTemp, 128, "{call sp_PungPungCheck('%s',%d,?)}", 
												strIP.GetString(), nUserNum);

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
    
	return nReturn;
}

// �⼮�� �̺�Ʈ
// ������ �⼮ �α� �ҷ�����
int COdbcManager::LogUserAttend( int nUserNum, std::vector<USER_ATTEND_INFO> &vecUserAttend )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "Exec sp_UserAttendLog %d", nUserNum );

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
	
	TIMESTAMP_STRUCT sAttendTime;   SQLINTEGER cbAttendTime   = SQL_NTS;
	SQLINTEGER nCount = 0, cbCount=SQL_NTS;
	SQLINTEGER nAttendReward = 0, cbAttendReward=SQL_NTS;

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
			USER_ATTEND_INFO sAttend;

			// Bind data
			::SQLGetData(pConn->hStmt, 1, SQL_C_TYPE_TIMESTAMP, &sAttendTime, 0, &cbAttendTime);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nCount, 0, &cbCount);
			::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nAttendReward, 0, &cbAttendReward);
			
			// ���� �⼮�� �α�
			// 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if (sAttendTime.year <= 1970 || sAttendTime.year >= 2999)
			{
				sAttendTime.year = 1970;
				sAttendTime.month = 2;
				sAttendTime.day = 1;
				sAttendTime.hour = 1;
				sAttendTime.minute = 1;
				sAttendTime.second = 1;
			}
            CTime cTemp(sAttendTime.year, sAttendTime.month,  sAttendTime.day,
                        sAttendTime.hour, sAttendTime.minute, sAttendTime.second);

            sAttend.tAttendTime = cTemp.GetTime();
			sAttend.nComboAttend = nCount;
			sAttend.bAttendReward = (nAttendReward == 1) ? true : false;

			vecUserAttend.push_back ( sAttend );
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

// ������ �⼮ �α� �����ϱ�
int COdbcManager::InsertUserAttend( int nUserNum, int nCount, int nAttendReward )
{
	if ( nUserNum < 0 )
	{
		Print(_T("ERROR:COdbcManager::InsertUserAttend"));
		return DB_ERROR;
	}

	TCHAR szTemp[128] = {0};
    _snprintf_s( szTemp, 128, "{call sp_InsertAttendLog( %d,%d,%d,?)}",
												nUserNum, nCount,nAttendReward );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
    
	return nReturn;
}

// �⼮ �α׿� ���� ������ ����( ������ ��ũ )
int COdbcManager::InsertAttendItem( CString strPurKey, CString strUserID, int nItemMID, int nItemSID )
{
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_InsertItem('%s','%s',%d,%d,?)}",
		strPurKey.GetString(),
		strUserID.GetString(),
		nItemMID,
		nItemSID );

	int nReturn = m_pShopDB->ExecuteSpInt(szTemp);
    
	return nReturn;
}