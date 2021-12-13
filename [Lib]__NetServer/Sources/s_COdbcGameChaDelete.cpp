#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
*  ���� ĳ���� ���� �Լ�
* -2 : ��帶���� �������� �ʿ��ϴ�
* -1 : db error
*  0 : ��������
*  1 : �ذ��� ���� ����
*  2 : �ذ��� ���� ����
*/
int COdbcManager::DeleteCharacter(int nUsrNum, int nChaNum)
{
	//std::strstream strTemp;
	//strTemp << "{call sp_delete_character(";
	//strTemp << nChaNum   << ",?)}";	
	//strTemp << std::ends;

	TCHAR szTemp[128] = {0};
	_snprintf_s( szTemp, 128, "{call sp_delete_character(%d, ?)}", nChaNum );

	int nReturn = m_pGameDB->ExecuteSpInt(szTemp);
//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	return nReturn;
}

/**
* ĳ���� ����
*/
int	COdbcManager::DelCharacter(int nUsrNum, 
							   int nChaNum, 
							   const char* szPass2)
{

#if defined (TW_PARAM) || defined (HK_PARAM) || defined (_RELEASED)
	TCHAR szTemp[128] = {0};
	_snprintf( szTemp, 128, "{call sp_UserCheckPass(%d,'%s', ?)}", nUsrNum, szPass2 );

	int nReturn = m_pUserDB->ExecuteSpInt(szTemp);

	if( nReturn == SQL_SUCCESS )
		return DeleteCharacter(nUsrNum, nChaNum);    
	else 
		return DB_ERROR;
#else
	SQLRETURN sReturn = 0;
	ODBC_STMT* pConn = m_pUserDB->GetConnection();
	if (!pConn) return DB_ERROR;

	// 2�� ��й�ȣ üũ
	//std::strstream strTemp;
	//strTemp << "SELECT UserNum FROM UserInfo WITH (NOLOCK) WHERE UserNum=";
	//strTemp << nUsrNum;
	//strTemp << " AND UserPass2='";
	//strTemp << szPass2;
	//strTemp << "'";
	//strTemp << std::ends;

	TCHAR szTemp[512] = {0};
	_snprintf_s( szTemp, 512, "SELECT UserNum FROM UserInfo WITH (NOLOCK) WHERE UserNum=%d AND UserPass2='%s'", nUsrNum, szPass2 );

	sReturn = ::SQLExecDirect(pConn->hStmt,
		(SQLCHAR*) szTemp, 
		SQL_NTS);

	if ((sReturn != SQL_SUCCESS) && (sReturn != SQL_SUCCESS_WITH_INFO)) 
	{
		// Log �� ����ϰ� �߻��ؼ� ����ó�� ����
		// Print(strTemp.str());		
		// Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//		strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
		return DB_ERROR;
	}	

	while (true)
	{
		sReturn = ::SQLFetch(pConn->hStmt);
		if (sReturn == SQL_NO_DATA)
		{
			Print(szTemp);
			Print(GetErrorString(pConn->hStmt));
			m_pUserDB->FreeConnection(pConn);

			//			strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			return DB_ERROR;
		}

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
			m_pUserDB->FreeConnection(pConn);

			//			strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			return DB_ERROR;
		}

		if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			break;
		}
		else
		{
			break;
		}		
		Sleep( 0 );
	}
	//	strTemp.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.

	m_pUserDB->FreeConnection(pConn);

	return DeleteCharacter(nUsrNum, nChaNum);    

#endif
}

/**
* ĳ���� ����
*/
int	COdbcManager::DaumDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

// GSP ĳ���� ����
int	COdbcManager::GspDelCharacter( int nUsrNum, int nChaNum )
{
	return DeleteCharacter( nUsrNum, nChaNum );
}

/**
* ĳ���� ����
*/
int	COdbcManager::TerraDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

/**
* ĳ���� ����
*/
int COdbcManager::ExciteDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

/**
* Japan ĳ���� ����
*/
int COdbcManager::JapanDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}


/**
* GS ĳ���� ����
*/
int	COdbcManager::GsDelCharacter(int nUsrNum, int nChaNum )
{
	return DeleteCharacter(nUsrNum, nChaNum);
}