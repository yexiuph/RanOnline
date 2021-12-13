#include "pch.h"
#include "s_COdbcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* call sp_logitemexchange_insert (
    @NIDMain    int, @NIDSub     int,	@SGNum        int,	@SvrNum       int,	@FldNum   int,
	@MakeType   int, @MakeNum    money, @ItemAmount   int,	@ItemFromFlag int,	@ItemFrom int,
	@ItemToFlag int, @ItemTo     int,	@ExchangeFlag int,	@Damage       int,	@Defense  int,
	@Fire       int, @Ice        int,	@Poison       int,	@Electric     int,	@Spirit   int,
	@CostumeMID int, @CostumeSID int,	@TradePrice   money, ?)
*/

// m_nFromType, m_nToType
// 1 : User number
// 2 : Char number
// 3 : Club number
int COdbcManager::LogItemExchange(
	const SLOGITEMEXCHANGE &sEXCHANGE )
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_logitemexchange_insert(";

	strTemp << sEXCHANGE.m_nNID_M << ",";
	strTemp << sEXCHANGE.m_nNID_S << ",";
	strTemp << sEXCHANGE.m_nSGNum << ",";
	strTemp << sEXCHANGE.m_nSvrNum << ",";
	strTemp << sEXCHANGE.m_nFldNum << ",";
	
	strTemp << sEXCHANGE.m_nMakeType << ",";
	strTemp << sEXCHANGE.m_lnMakeNum << ",";
	strTemp << sEXCHANGE.m_nNum << ","; // ItemAmount	
	strTemp << sEXCHANGE.m_nFromType << ",";
	strTemp << sEXCHANGE.m_nItemFrom << ",";

	strTemp << sEXCHANGE.m_nToType << ",";
	strTemp << sEXCHANGE.m_nItemTo << ",";
	strTemp << sEXCHANGE.m_nExchangeFlag << ",";
	strTemp << sEXCHANGE.m_nDamage << ",";	
	strTemp << sEXCHANGE.m_nDefense << ",";

	strTemp << sEXCHANGE.m_nFire << ",";
	strTemp << sEXCHANGE.m_nIce << ",";
	strTemp << sEXCHANGE.m_nPoison << ",";
	strTemp << sEXCHANGE.m_nElectric << ",";	
	strTemp << sEXCHANGE.m_nSpirit << ",";

	strTemp << sEXCHANGE.m_nCOSTUME_MID << ",";
	strTemp << sEXCHANGE.m_nCOSTUME_SID << ",";
	strTemp << "0,?)}"; // TradePrice
	strTemp << std::ends;
*/
	int temp = 0;
	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,%d,%I64d,%d,%d,%d,"
							"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}",
							sEXCHANGE.m_nNID_M, sEXCHANGE.m_nNID_S, sEXCHANGE.m_nSGNum,sEXCHANGE.m_nSvrNum,
							sEXCHANGE.m_nFldNum, sEXCHANGE.m_nMakeType, sEXCHANGE.m_lnMakeNum, sEXCHANGE.m_nNum,
							sEXCHANGE.m_nFromType, sEXCHANGE.m_nItemFrom, sEXCHANGE.m_nToType, sEXCHANGE.m_nItemTo,
							sEXCHANGE.m_nExchangeFlag, sEXCHANGE.m_nDamage, sEXCHANGE.m_nDefense, sEXCHANGE.m_nFire,
							sEXCHANGE.m_nIce, sEXCHANGE.m_nPoison, sEXCHANGE.m_nElectric, sEXCHANGE.m_nSpirit,
							sEXCHANGE.m_nCOSTUME_MID, sEXCHANGE.m_nCOSTUME_SID, temp );

		
	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	if(nReturn == 1)	return DB_OK;
	else				return DB_ERROR;
}

// m_nFromType, m_nToType
// 1 : User number
// 2 : Char number
// 3 : Club number
int	COdbcManager::LogMoneyExchange(
	const SLOGMONEYEXCHANGE &_sLOG )
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_logitemexchange_insert(";

	strTemp << "0,"; // NIDMain
	strTemp << "0,"; // NIDSub
	strTemp << _sLOG.m_nSGNum << ",";
	strTemp << _sLOG.m_nSvrNum << ",";
	strTemp << _sLOG.m_nFldNum << ",";

	strTemp << "0,"; // MakeType
	strTemp << "0,"; // MakeNum
	strTemp << "0,"; // ItemAmount
	strTemp << _sLOG.m_nFromType << ",";
	strTemp << _sLOG.m_nItemFrom << ",";

	strTemp << _sLOG.m_nToType << ",";
	strTemp << _sLOG.m_nItemTo << ",";
	strTemp << _sLOG.m_nExchangeFlag << ",";
	strTemp << "0,"; // Damage
	strTemp << "0,"; // Defense

	strTemp << "0,"; // Fire
	strTemp << "0,"; // Ice
	strTemp << "0,"; // Poison
	strTemp << "0,"; // Electric
	strTemp << "0,"; // Spirit

	strTemp << "0,"; // CostumeMID
	strTemp << "0,"; // CostumeSID
	strTemp << _sLOG.m_lnPrice << ",?)}";	// TradePrice
	strTemp << std::ends;
*/
	int temp = 0;
	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,"
															"%d,%d,%d,%d,%d,"
															"%d,%d,%d,%d,%d,"
															"%d,%d,%d,%d,%d,"
															"%d,%d,%I64d,?)}",temp, temp, _sLOG.m_nSGNum,
											_sLOG.m_nSvrNum, _sLOG.m_nFldNum, temp, temp, temp, 
											_sLOG.m_nFromType, _sLOG.m_nItemFrom, _sLOG.m_nToType, _sLOG.m_nItemTo,
											_sLOG.m_nExchangeFlag, temp, temp, temp, temp, temp, temp, temp,
											temp, temp, _sLOG.m_lnPrice );

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	if (nReturn == 1)	return DB_OK;
	else				return DB_ERROR;
}

// m_nFromType, m_nToType
// 1 : User number
// 2 : Char number
// 3 : Club number
int COdbcManager::LogItemConversion(
	const SLOGITEMCONVERSION &_sLOG )
{
/*
	std::strstream strTemp;
	strTemp << "{call sp_logitemexchange_insert(";

	strTemp << _sLOG.m_nNIDMain << ",";
	strTemp << _sLOG.m_nNIDSub << ",";
	strTemp << _sLOG.m_nSGNum << ",";
	strTemp << _sLOG.m_nSvrNum << ",";
	strTemp << _sLOG.m_nFldNum << ",";	

	strTemp << _sLOG.m_nMakeType << ",";
	strTemp << _sLOG.m_lnMakeNum << ",";
	strTemp << "0,"; // ItemAmount
	strTemp << _sLOG.m_nFromType << ",";
	strTemp << _sLOG.m_nItemFrom << ",";

	strTemp << "0,"; // ItemToFlag
	strTemp << "0,"; // ItemTo
	strTemp << "0,"; // ExchangeFlag
	strTemp << _sLOG.m_nDamage << ",";
	strTemp << _sLOG.m_nDefense << ",";

	strTemp << _sLOG.m_nFire << ",";
	strTemp << _sLOG.m_nIce << ",";
	strTemp << _sLOG.m_nPoison << ",";
	strTemp << _sLOG.m_nElectric << ",";
	strTemp << _sLOG.m_nSpirit << ",";

    strTemp << _sLOG.m_nCOSTUME_MID << ",";
    strTemp << _sLOG.m_nCOSTUME_SID << ",";
	strTemp << "0,?)}"; // TradePrice
	strTemp << std::ends;
*/
	int temp = 0;
	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "{call sp_logitemexchange_insert(%d,%d,%d,%d,%d,"
															"%d,%I64d,%d,%d,%d,"
															"%d,%d,%d,%d,%d,"
															"%d,%d,%d,%d,%d,"
															"%d,%d,%d,?)}",
										_sLOG.m_nNIDMain, _sLOG.m_nNIDSub, _sLOG.m_nSGNum, _sLOG.m_nSvrNum,
										_sLOG.m_nFldNum, _sLOG.m_nMakeType, _sLOG.m_lnMakeNum, temp,
										_sLOG.m_nFromType, _sLOG.m_nItemFrom, temp, temp, temp, 
										_sLOG.m_nDamage, _sLOG.m_nDefense, _sLOG.m_nFire, _sLOG.m_nIce,
										_sLOG.m_nPoison, _sLOG.m_nElectric, _sLOG.m_nSpirit, _sLOG.m_nCOSTUME_MID,
										_sLOG.m_nCOSTUME_SID, temp);

	int nReturn = m_pLogDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	
	if (nReturn == 1)	return DB_OK;
	else				return DB_ERROR;	
}

// m_nFromType, m_nToType
// 1 : User number
// 2 : Char number
// 3 : Club number
LONGLONG COdbcManager::GetItemMaxNum(
	int nSGNum, 
    int nSvrNum, 
    int nFldNum, 
    std::vector<VIEWLOGITEMEXCHANGEMAX> &v)
{
    SQLRETURN sReturn = 0;
	LONGLONG llMaxNum = 0; SQLINTEGER cbMaxNum=SQL_NTS;
    SQLINTEGER nNIDMain = 0, cbNIDMain =SQL_NTS;
    SQLINTEGER nNIDSub  = 0, cbNIDSub  =SQL_NTS;
    SQLINTEGER nMakeType= 0, cbMakeType=SQL_NTS;

    ODBC_STMT* pConn = m_pLogDB->GetConnection();
	if (!pConn) return DB_ERROR;

	/*
    std::strstream strTemp;
    strTemp << "SELECT MaxNum, NIDMain, NIDSub, MakeType FROM viewLogItemExchangeMax WHERE ";
    strTemp << "SGNum="  << nSGNum  << " AND ";
	strTemp << "SvrNum=" << nSvrNum << " AND ";
	strTemp << "FldNum=" << nFldNum ;
    strTemp << std::ends;
	*/

/*
	std::strstream strTemp;
    strTemp << "SELECT MaxNum, NIDMain, NIDSub, MakeType FROM LogItemMax WITH (NOLOCK) WHERE ";
    strTemp << "SGNum="  << nSGNum  << " AND ";
	strTemp << "SvrNum=" << nSvrNum << " AND ";
	strTemp << "FldNum=" << nFldNum ;
    strTemp << std::ends;
*/
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT MaxNum, NIDMain, NIDSub, MakeType FROM LogItemMax WITH (NOLOCK) WHERE "
			"SGNum=%d AND SvrNum=%d AND FldNum=%d", nSGNum, nSvrNum, nFldNum );

    sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*) szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pLogDB->FreeConnection(pConn);

//		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

        return DB_ERROR;
	}

    while(true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pLogDB->FreeConnection(pConn);

//			strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{   
            ::SQLGetData(pConn->hStmt, 1, SQL_C_SBIGINT, &llMaxNum, 0, &cbMaxNum);
            ::SQLGetData(pConn->hStmt, 2, SQL_C_LONG,	 &nNIDMain, 0, &cbNIDMain);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_LONG,	 &nNIDSub,  0, &cbNIDSub);
            ::SQLGetData(pConn->hStmt, 4, SQL_C_LONG,	 &nMakeType,0, &cbMakeType);

            VIEWLOGITEMEXCHANGEMAX sTemp;

            sTemp.llMaxNum  = llMaxNum;
            sTemp.nMakeType = nMakeType;
            sTemp.wItemMain = static_cast<WORD> (nNIDMain);
            sTemp.wItemSub  = static_cast<WORD> (nNIDSub);

            v.push_back(sTemp);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pLogDB->FreeConnection(pConn);
    return DB_OK;
}