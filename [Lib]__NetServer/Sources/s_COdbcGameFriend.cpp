#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////
// Get All Friend list
int COdbcManager::GetChaFriend(
	int nChaNum,
	std::vector<CHA_FRIEND> &vecFriend )
{
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT ChaP, ChaS, ChaName, ChaFlag From viewChaFriend ";
	//strTemp << "WHERE ChaP=" << nChaNum;
	//strTemp << std::ends;

#if defined(RZ_PARAM) || defined( KR_PARAM ) || defined(KRT_PARAM )
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT ChaP, ChaS, ChaName, ChaFlag From viewChaFriend "
							"WHERE ChaP=%d", nChaNum );
#else
	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT TOP 100 ChaP, ChaS, ChaName, ChaFlag From viewChaFriend "
							"WHERE ChaP=%d", nChaNum );
#endif
	
	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		return DB_ERROR;
	}

	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLINTEGER nChaP = 0, cbChaP=SQL_NTS;
	SQLINTEGER nChaS = 0, cbChaS=SQL_NTS;
	SQLINTEGER nChaFlag = 0, cbChaFlag=SQL_NTS;
	
	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
            return DB_ERROR;
		}

 		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{	
			CHA_FRIEND sFriend;

			// Bind data			
			::SQLGetData(pConn->hStmt,  1, SQL_C_LONG,	&nChaP,                0, &cbChaP);
			::SQLGetData(pConn->hStmt,  2, SQL_C_LONG,	&nChaS,                0, &cbChaS);
			::SQLGetData(pConn->hStmt,  3, SQL_C_CHAR,  szChaName, CHR_ID_LENGTH, &cbChaName);
			::SQLGetData(pConn->hStmt,  4, SQL_C_LONG,	&nChaFlag,             0, &cbChaFlag);

			 if (cbChaName != 0 && cbChaName != -1)
				::StringCchCopy(sFriend.szCharName, CHR_ID_LENGTH+1, (const TCHAR*) szChaName);

			 sFriend.nChaS = nChaS;
			 sFriend.nChaFlag = nChaFlag;

			 vecFriend.push_back ( sFriend );
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

///////////////////////////////////////////////////////////////////////////
// Delete friend from friend list
int COdbcManager::DelChaFriend(int nChaP, int nChaS)
{
	//std::strstream strTemp;
	//strTemp << "{call DeleteChaFriend(";
	//strTemp << nChaP << ",";
	//strTemp << nChaS << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call DeleteChaFriend(%d,%d,?)}", nChaP, nChaS );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// Add friend to friend list
int COdbcManager::AddChaFriend(int nChaP, int nChaS)
{
	//std::strstream strTemp;
	//strTemp << "{call InsertChaFriend(";
	//strTemp << nChaP << ",";
	//strTemp << nChaS << ",?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call InsertChaFriend(%d,%d,?)}", nChaP, nChaS );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// Set flag to friend list
int COdbcManager::SetChaFriend(int nChaP, int nChaS, int nFlag)
{
	//std::strstream strTemp;
	//strTemp << "{call UpdateChaFriend(";	
	//strTemp << nChaP << ",";
	//strTemp << nChaS << ",";
	//strTemp << nFlag << ",?)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaFriend(%d,%d,%d,?)}", nChaP, nChaS, nFlag );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}