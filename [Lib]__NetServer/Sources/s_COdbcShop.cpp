#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Shop ���� ������ �������� �����´�.
// ������ �������� ������ ���� �� �� �ִ� �������� �ƴϴ�.
// �������� ���������� ������ ������ �� �ִ��� �ٽ� Ȯ���ؾ� �Ѵ�. (SetPurchaseItem)
// ���� ���� ���ſ� ��� �� ���ӳ� ĳ������ ����ȭ ����.
int COdbcManager::GetPurchaseItem(CString strUID, std::vector<SHOPPURCHASE> &v)
{
    ODBC_STMT* pConn = m_pShopDB->GetConnection();
	if (!pConn)	return DB_ERROR;    

	//std::strstream strTemp;
 //   strTemp << "SELECT Purkey, ItemMain, ItemSub FROM viewShopPurchase WHERE ";
 //   strTemp << "useruid='" << strUID.GetString() << "' AND ";
 //   strTemp << "PurFlag=0";
 //   strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT Purkey, ItemMain, ItemSub FROM viewShopPurchase WHERE "
							"useruid='%s' AND PurFlag=0", strUID.GetString() );

	SQLCHAR    szPurKey[PURKEY_LENGTH+1] = {0}; SQLINTEGER cbPurKey = SQL_NTS;
    SQLINTEGER nItemMain = 0, cbItemMain =SQL_NTS;
    SQLINTEGER nItemSub  = 0, cbItemSub  =SQL_NTS;
    SQLRETURN  sReturn=0;

    sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pShopDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

        return DB_ERROR;
	}

    while(true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pShopDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{   
            ::SQLGetData(pConn->hStmt, 1, SQL_C_CHAR, szPurKey,   PURKEY_LENGTH+1, &cbPurKey);
            ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nItemMain, 0,             &cbItemMain);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_LONG, &nItemSub,  0,             &cbItemSub);

            SHOPPURCHASE sTemp;

            if (cbPurKey != 0 && cbPurKey != -1)
                sTemp.strPurKey = szPurKey;
            sTemp.wItemMain = static_cast<WORD> (nItemMain);
            sTemp.wItemSub  = static_cast<WORD> (nItemSub);

            v.push_back(sTemp);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pShopDB->FreeConnection(pConn);
    return DB_OK;
}

///////////////////////////////////////////////////////////////////////////////
// ������ ������ �� �ִ� ��ǰ���� Ȯ���Ѵ�.
// �Է°�
// nFlag  : (0 : ����, 1 : ���ſϷ�, 2 : ������ҽ�û, 3 : �������ó��)
// ��°�
// 1 : �������� ����
// �̿��ǰ� : �������� ����
int COdbcManager::SetPurchaseItem(CString strPurKey, int nFlag)
{
	//std::strstream strTemp;
	//strTemp << "{call sp_purchase_change_state(";	
	//strTemp << "'" << strPurKey.GetString() << "',";		
	//strTemp << nFlag << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "{call sp_purchase_change_state('%s',%d,?)}", strPurKey.GetString(), nFlag );

	int nReturn = m_pShopDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}