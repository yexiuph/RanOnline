#include "pch.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
*  실제 캐릭터 삭제 함수
* -2 : 길드마스터 길드삭제가 필요하다
* -1 : db error
*  0 : 삭제성공
*  1 : 극강부 남자 삭제
*  2 : 극강부 여자 삭제
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
//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	return nReturn;
}

/**
* 캐릭터 삭제
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

	// 2차 비밀번호 체크
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
		// Log 에 빈번하게 발생해서 삭제처리 했음
		// Print(strTemp.str());		
		// Print(GetErrorString(pConn->hStmt));
		m_pUserDB->FreeConnection(pConn);

		//		strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
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

			//			strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			return DB_ERROR;
		}

		if (sReturn == SQL_ERROR || sReturn == SQL_SUCCESS_WITH_INFO)
		{
			Print(szTemp);		
			Print(GetErrorString(pConn->hStmt));
			m_pUserDB->FreeConnection(pConn);

			//			strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
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
	//	strTemp.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

	m_pUserDB->FreeConnection(pConn);

	return DeleteCharacter(nUsrNum, nChaNum);    

#endif
}

/**
* 캐릭터 삭제
*/
int	COdbcManager::DaumDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

// GSP 캐릭터 삭제
int	COdbcManager::GspDelCharacter( int nUsrNum, int nChaNum )
{
	return DeleteCharacter( nUsrNum, nChaNum );
}

/**
* 캐릭터 삭제
*/
int	COdbcManager::TerraDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

/**
* 캐릭터 삭제
*/
int COdbcManager::ExciteDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}

/**
* Japan 캐릭터 삭제
*/
int COdbcManager::JapanDelCharacter(int nUsrNum, int nChaNum)
{
	return DeleteCharacter(nUsrNum, nChaNum);
}


/**
* GS 캐릭터 삭제
*/
int	COdbcManager::GsDelCharacter(int nUsrNum, int nChaNum )
{
	return DeleteCharacter(nUsrNum, nChaNum);
}