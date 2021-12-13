#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Shop 에서 구입한 아이템을 가져온다.
// 가져온 아이템은 실제로 가져 갈 수 있는 아이템이 아니다.
// 아이템을 가져갈때는 실제로 가져갈 수 있는지 다시 확인해야 한다. (SetPurchaseItem)
// 웹을 통한 구매와 취소 및 게임내 캐릭터의 동기화 문제.
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

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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

			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

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
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pShopDB->FreeConnection(pConn);
    return DB_OK;
}

///////////////////////////////////////////////////////////////////////////////
// 실제로 가져갈 수 있는 상품인지 확인한다.
// 입력값
// nFlag  : (0 : 구매, 1 : 구매완료, 2 : 구매취소신청, 3 : 구매취소처리)
// 출력값
// 1 : 가져갈수 있음
// 이외의값 : 가져갈수 없음
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
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}