#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* ĳ���Ͱ� ������ ��, ����Ʈ, ��ġ�� �����´�
* \param nUserNum ����ڹ�ȣ
* \param nChaNum ĳ���͹�ȣ
* \param pChaData ĳ���� �����͸� ���� ����Ʈ
* \return DB_OK, DB_ERROR
*/
int COdbcManager::GetCharacterInfo(int nUserNumber,
								   int nChaNum,
								   GLCHARAG_DATA* pChaData)
{
	assert(pChaData&&"(GLCHARAG_DATA*)�� ���� ��ȿ���� �ʽ��ϴ�.");
	SQLRETURN sReturn = 0;

	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;

	// ĳ���� ������ �����´�.
	//std::strstream strTemp;
	//strTemp << "SELECT UserNum, SGNum, ChaName, ChaClass, ChaBright, ChaStartMap, ";
	//strTemp << "ChaStartGate, ChaPosX, ChaPosY, ChaPosZ, ";    
 //   strTemp << "ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, ";
	//strTemp << "ChaSchool, GuNum, ChaGuSecede ";
	//strTemp << "FROM ChaInfo WHERE ChaNum=" << nChaNum;
	//strTemp << " AND UserNum=" << nUserNumber;
	//strTemp << std::ends;

	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "SELECT UserNum, SGNum, ChaName, ChaClass, ChaBright, ChaStartMap, "
							"ChaStartGate, ChaPosX, ChaPosY, ChaPosZ, "
							"ChaSaveMap, ChaSavePosX, ChaSavePosY, ChaSavePosZ, "
							"ChaSchool, GuNum, ChaGuSecede "
							"FROM ChaInfo WHERE ChaNum=%d"
							" AND UserNum=%d", nChaNum, nUserNumber );
	
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

	SQLINTEGER nUserNum = 0, cbUserNum=SQL_NTS;
	SQLINTEGER nSGNum   = 0, cbSGNum=SQL_NTS;
    SQLINTEGER nGuNum   = 0, cbGuNum=SQL_NTS; // Club ��ȣ
	SQLCHAR    szChaName[CHR_ID_LENGTH+1] = {0}; SQLINTEGER cbChaName = SQL_NTS;
	SQLCHAR    szChaPhoneNumber[SMS_RECEIVER] = {0}; SQLINTEGER cbChaPhoneNumber = SQL_NTS;
	SQLINTEGER nChaClass = 0, cbChaClass=SQL_NTS;
	SQLINTEGER nChaBright = 0, cbChaBright=SQL_NTS;

	SQLINTEGER nChaStartMap = 0, cbChaStartMap=SQL_NTS;
	SQLINTEGER nChaStartGate = 0, cbChaStartGate=SQL_NTS;

	SQLFLOAT   fChaPosX = 0; SQLINTEGER cbChaPosX=SQL_NTS;
	SQLFLOAT   fChaPosY = 0; SQLINTEGER cbChaPosY=SQL_NTS;
	SQLFLOAT   fChaPosZ = 0; SQLINTEGER cbChaPosZ=SQL_NTS;

	SQLINTEGER nChaSaveMap = 0, cbChaSaveMap=SQL_NTS;	

	SQLFLOAT fChaSavePosX = 0; SQLINTEGER cbChaSavePosX=SQL_NTS;
	SQLFLOAT fChaSavePosY = 0; SQLINTEGER cbChaSavePosY=SQL_NTS;
	SQLFLOAT fChaSavePosZ = 0; SQLINTEGER cbChaSavePosZ=SQL_NTS;

	SQLINTEGER nChaSchool = 0, cbChaSchool=SQL_NTS;
    TIMESTAMP_STRUCT sChaGuSecede; SQLINTEGER cbChaGuSecede = SQL_NTS; // Ŭ��Ż��ð�

	while (true)
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
			// Bind data
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nUserNum,	0, &cbUserNum);
			::SQLGetData(pConn->hStmt, 2, SQL_C_LONG, &nSGNum,		0, &cbSGNum);
            ::SQLGetData(pConn->hStmt, 3, SQL_C_CHAR, szChaName,  CHR_ID_LENGTH+1, &cbChaName); 
			::SQLGetData(pConn->hStmt, 4, SQL_C_LONG, &nChaClass,   0, &cbChaClass);
			::SQLGetData(pConn->hStmt, 5, SQL_C_LONG, &nChaBright,  0, &cbChaBright);

            ::SQLGetData(pConn->hStmt, 6, SQL_C_LONG, &nChaStartMap,0, &cbChaStartMap);			

			::SQLGetData(pConn->hStmt, 7, SQL_C_LONG,   &nChaStartGate,  0, &cbChaStartGate);
			::SQLGetData(pConn->hStmt, 8, SQL_C_DOUBLE, &fChaPosX,	0, &cbChaPosX);
			::SQLGetData(pConn->hStmt, 9, SQL_C_DOUBLE, &fChaPosY,	0, &cbChaPosY);
			::SQLGetData(pConn->hStmt, 10, SQL_C_DOUBLE, &fChaPosZ,	0, &cbChaPosZ);
			::SQLGetData(pConn->hStmt, 11, SQL_C_LONG,   &nChaSaveMap, 0, &cbChaSaveMap);

			::SQLGetData(pConn->hStmt, 12, SQL_C_DOUBLE, &fChaSavePosX, 0, &cbChaSavePosX);
			::SQLGetData(pConn->hStmt, 13, SQL_C_DOUBLE, &fChaSavePosY, 0, &cbChaSavePosY);
			::SQLGetData(pConn->hStmt, 14, SQL_C_DOUBLE, &fChaSavePosZ, 0, &cbChaSavePosZ);
			::SQLGetData(pConn->hStmt, 15, SQL_C_LONG, &nChaSchool, 0, &cbChaSchool);
            ::SQLGetData(pConn->hStmt, 16, SQL_C_LONG, &nGuNum,		0, &cbGuNum);

            ::SQLGetData(pConn->hStmt, 17, SQL_C_TYPE_TIMESTAMP, &sChaGuSecede, 0, &cbChaGuSecede);

			pChaData->m_dwUserID = (DWORD) nUserNum;
			pChaData->m_dwServerID = (DWORD) nSGNum;

			if (cbChaName != 0 && cbChaName != -1) 				
				::StringCchCopy(pChaData->m_szName, CHR_ID_LENGTH, (const char*) szChaName);

			pChaData->m_emClass = EMCHARCLASS(nChaClass);
			pChaData->m_nBright			= nChaBright;

			pChaData->m_sStartMapID.dwID	= (DWORD) nChaStartMap;

			pChaData->m_dwStartGate		= (DWORD) nChaStartGate;
			pChaData->m_vStartPos.x		= (float) fChaPosX;
			pChaData->m_vStartPos.y		= (float) fChaPosY;
			pChaData->m_vStartPos.z		= (float) fChaPosZ;

			pChaData->m_sSaveMapID.dwID = (DWORD) nChaSaveMap;
			pChaData->m_vSavePos.x		= (float) fChaSavePosX; 
			pChaData->m_vSavePos.y		= (float) fChaSavePosY; 
			pChaData->m_vSavePos.z		= (float) fChaSavePosZ;

			pChaData->m_wSchool         = (WORD) nChaSchool;
            pChaData->m_dwGuild         = (DWORD) nGuNum;

            // Ŭ��Ż��ð�
            // 1970-02-01 : Default
			// �±����� TimeZone ������ Ʋ���� ������ �߻�
			// �� �ڵ带 �߰��� �����Ѵ�.
			if (sChaGuSecede.year <= 1970 || sChaGuSecede.year >= 2999)
			{
				sChaGuSecede.year = 1970;
				sChaGuSecede.month = 2;
				sChaGuSecede.day = 1;
				sChaGuSecede.hour = 1;
				sChaGuSecede.minute = 1;
				sChaGuSecede.second = 1;
			}

            CTime cTemp(sChaGuSecede.year, sChaGuSecede.month,  sChaGuSecede.day, 
                        sChaGuSecede.hour, sChaGuSecede.minute, sChaGuSecede.second);
            pChaData->m_tSECEDE = cTemp.GetTime();
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

#if defined(KRT_PARAM)
	// ĳ������ ��ȭ��ȣ�� �����´�.
	GetChaPhoneNumber( nChaNum, pChaData->m_szPhoneNumber );
#endif

	return DB_OK;
}

///////////////////////////////////////////////////////////////////////////
// �ش� ������� ĳ���� ������ �����´�.	
int COdbcManager::GetChaAllInfo(int nUsrNum)
{	
	return DB_OK;
}

int COdbcManager::GetChaBAInfo(int nUsrNum, 
							   int nSvrGrp, 
							   NET_CHA_BBA_INFO* ncbi)
{
	if (ncbi == NULL || nUsrNum <= 0) return DB_ERROR;

	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection(); 
	if (!pConn) return DB_ERROR;
	
	int	nPos = 0;
	SQLINTEGER nChaNum = 0, cbChaNum = SQL_NTS; 
	
	//std::strstream strTemp;
	//strTemp << "SELECT TOP 16 ChaNum FROM ChaInfo WITH (NOLOCK) WHERE UserNum=";
	//strTemp << nUsrNum << " AND SGNum=" << nSvrGrp << " AND ChaDeleted=0 ORDER BY ChaNum";
	//strTemp << std::ends;

	TCHAR szTemp[256] = {0};
	_snprintf_s( szTemp, 256, "SELECT TOP 16 ChaNum FROM ChaInfo WITH (NOLOCK) WHERE UserNum=%d"
							" AND SGNum=%d"
							" AND ChaDeleted=0 ORDER BY ChaNum", nUsrNum, nSvrGrp );

	sReturn = ::SQLExecDirect(pConn->hStmt,
							(SQLCHAR*)szTemp, 
							SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{		
		ncbi->nChaSNum = 0;
        Print(szTemp);		
		Print(GetErrorString(pConn->hStmt));
		m_pGameDB->FreeConnection(pConn);

		//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}

	int nLoop = 0;

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
        {
			ncbi->nChaSNum = 0;
            Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
            m_pGameDB->FreeConnection(pConn);

			//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
            return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			// Bind data
			::SQLGetData(pConn->hStmt, 1, SQL_C_LONG, &nChaNum, 0, &cbChaNum);			
			ncbi->nChaNum[nLoop] = nChaNum;			
			nLoop++;
			if (nLoop >= MAX_ONESERVERCHAR_NUM)
				break;
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	ncbi->nChaSNum = nLoop;	
	m_pGameDB->FreeConnection(pConn);

	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	return DB_OK;
}

/**
 * �б�vs�б� ��Ƽ��ð������
 */
int	COdbcManager::LogPartyMatch(int nSGNum, int nSvrNum, WORD wWin, WORD wLost)
{	
	//std::strstream strTemp;
	//strTemp << "{call InsertPartyMatch(";
	//strTemp << nSGNum << ",";
	//strTemp << nSvrNum << ",";
	//strTemp << wWin << ",";
	//strTemp << wLost << ")}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call InsertPartyMatch(%d,%d,%u,%u)}", nSGNum, nSvrNum, wWin, wLost );

	int nReturn = m_pGameDB->ExecuteSp(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// �ش� ĳ������ ����ġ�� �����Ѵ�.
// �Է�
// nChaNum : ĳ���͹�ȣ
// llExp : ����ġ
// ���� 
// ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
// ����ġ�� 0 ���� Ŀ���Ѵ�.
int COdbcManager::SetChaExp(int nChaNum, LONGLONG llExp)
{
    if (nChaNum < 1 || llExp < 0)
    {
        return DB_ERROR;
    }

 //   std::strstream strTemp;
	//strTemp << "{call UpdateChaExp(";
 //   strTemp << llExp << ",";
	//strTemp << nChaNum << ",?)}";
 //   strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaExp(%I64d,%d,?)}", llExp, nChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// �ش� ĳ������ ����ġ�� �����´�.
// �Է�
// nChaNum : ĳ���͹�ȣ
// ����
// LONGLONG : ����ġ
// ���� 
// ĳ���͹�ȣ�� 0 ���� Ŀ���Ѵ�.
// ����ġ�� 0 ���� ������ �����̴�.
LONGLONG COdbcManager::GetChaExp(int nChaNum)
{
    if (nChaNum < 1)
    {
        return DB_ERROR;
    }

    SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pGameDB->GetConnection();
	if (!pConn) return DB_ERROR;
	
	//std::strstream strTemp;
	//strTemp << "SELECT ChaExp From ChaInfo ";
	//strTemp << "WHERE ChaNum=" << nChaNum;
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "SELECT ChaExp From ChaInfo "
							"WHERE ChaNum=%d", nChaNum );
	
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

	LONGLONG   llChaExp = 0;
    SQLINTEGER cbChaExp = SQL_NTS;
	
	while (true)
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
			// Bind data
            ::SQLGetData(pConn->hStmt, 1, SQL_C_SBIGINT, &llChaExp, 0, &cbChaExp);
		}
		else
		{
			break;
		}
		Sleep( 0 );
	}
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pGameDB->FreeConnection(pConn);
	return llChaExp;
}

///////////////////////////////////////////////////////////////////////////
// ������ȯ ī�� ����� ���� �������� �����Ѵ�.
int COdbcManager::SetLastCallPos(int nChaNum, DWORD dwMapID, D3DXVECTOR3 vPos)
{
 //   std::strstream strTemp;
	//strTemp << "{call UpdateChaLastCallPos(";
	//strTemp << dwMapID << ",";
 //   strTemp << vPos.x << ",";
 //   strTemp << vPos.y << ",";
 //   strTemp << vPos.z << ",";
	//strTemp << nChaNum << ",?)}";
 //   strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaLastCallPos(%u,%f,%f,%f,%d,?)}", dwMapID, vPos.x, vPos.y, vPos.z, nChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

    return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// ĳ���͸� �������� ���·� �����.
int COdbcManager::SetCharacterOffline(int nChaNum)
{
	if (nChaNum < 0) return NET_ERROR;

	//std::strstream strTemp;
	//strTemp << "{call UpdateChaOnline(";
	//strTemp << nChaNum << ",0)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaOnline(%d,0)}", nChaNum );

	int nReturn = m_pGameDB->ExecuteSp(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// ĳ���͸� �¶��� ���·� �����.
int COdbcManager::SetCharacterOnline(int nChaNum)
{
	if (nChaNum < 0) return NET_ERROR;

	//std::strstream strTemp;
	//strTemp << "{call UpdateChaOnline(";
	//strTemp << nChaNum << ",1)}";
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateChaOnline(%d,1)}", nChaNum );

	int nReturn = m_pGameDB->ExecuteSp(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

///////////////////////////////////////////////////////////////////////////
// ��� ĳ���͸� �������� ���·� �����.
int COdbcManager::SetAllCharacterOffline()
{
	std::strstream strTemp;
	strTemp << "{call UpdateAllCharacterOffline}";
	strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call UpdateAllCharacterOffline}" );

	int nReturn = m_pGameDB->ExecuteSp(szTemp);
	//strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}