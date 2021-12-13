#include "pch.h"
//#include "./DbExecuter.h"
#include "s_COdbcEnv.h"
#include "s_COdbcManager.h"
#include "s_CDbAction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COdbcManager* COdbcManager::SelfInstance = NULL;

COdbcManager::COdbcManager(void) 
	: m_pGameDB(NULL)
	, m_pUserDB(NULL)
	, m_pBoardDB(NULL)
	, m_pLogDB(NULL)
	, m_pShopDB(NULL)
	, m_pTerraDB(NULL)
	, m_pGspDB(NULL)
	, m_pThaiDB(NULL)
	, m_pKorDB(NULL)
	, m_pMyDB(NULL)
	, m_nServiceProvider(0)
{
	COdbcEnv::GetInstance();
}

COdbcManager::~COdbcManager(void)
{
	SAFE_DELETE(m_pGameDB);
	SAFE_DELETE(m_pUserDB);
	SAFE_DELETE(m_pBoardDB);
	SAFE_DELETE(m_pLogDB);
	SAFE_DELETE(m_pShopDB);
	SAFE_DELETE(m_pTerraDB);
	SAFE_DELETE(m_pGspDB);
	SAFE_DELETE(m_pThaiDB);
	SAFE_DELETE(m_pKorDB);
	SAFE_DELETE(m_pMyDB);
	COdbcEnv::GetInstance()->ReleaseInstance();
}

void COdbcManager::SetServiceProvider(int nServiceProvider)
{
	m_nServiceProvider = nServiceProvider;
}

COdbcManager* COdbcManager::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new COdbcManager();
	return SelfInstance;
}

void COdbcManager::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

int	COdbcManager::OpenGameDB(
	TCHAR* szOdbcName, 
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pGameDB);
    m_pGameDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID, 
						szUsrPasswd, 
						szDbName, 
						nPoolSize, 
						nDBTimeOut );
	return m_pGameDB->OpenDB();
}

int	COdbcManager::OpenUserDB(
	TCHAR* szOdbcName, 
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pUserDB);
    m_pUserDB = new COdbcSupervisor(
						szOdbcName, 
						szUsrID, 
						szUsrPasswd, 
						szDbName, 
						nPoolSize, 
						nDBTimeOut );
	return m_pUserDB->OpenDB();
}

int COdbcManager::OpenBoardDB(
	TCHAR* szOdbcName, 
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pBoardDB);
    m_pBoardDB = new COdbcSupervisor(
						szOdbcName, 
						szUsrID, 
						szUsrPasswd, 
						szDbName, 
						nPoolSize, 
						nDBTimeOut );
	return m_pBoardDB->OpenDB();
}

int COdbcManager::OpenLogDB(
	TCHAR* szOdbcName, 
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut)
{
	SAFE_DELETE(m_pLogDB);
    m_pLogDB = new COdbcSupervisor(
						szOdbcName, 
						szUsrID, 
						szUsrPasswd, 
						szDbName, 
						nPoolSize, 
						nDBTimeOut );
	return m_pLogDB->OpenDB();
}

int COdbcManager::OpenShopDB(
	TCHAR* szOdbcName,
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pShopDB);
    m_pShopDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pShopDB->OpenDB();
}

int COdbcManager::OpenTerraDB(
	TCHAR* szOdbcName,
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pTerraDB);
    m_pTerraDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pTerraDB->OpenDB();
}

int	COdbcManager::OpenGspDB(
	TCHAR* szOdbcName,
	TCHAR* szUsrID, 
	TCHAR* szUsrPasswd,
	TCHAR* szDbName,
	int nPoolSize,
	int nDBTimeOut )
{
	SAFE_DELETE(m_pGspDB);
    m_pGspDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pGspDB->OpenDB();
}

int	COdbcManager::OpenThaiDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize,
			int nDBTimeOut )
{
	SAFE_DELETE(m_pThaiDB);
    m_pThaiDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pThaiDB->OpenDB();
}

int	COdbcManager::OpenKorDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize,
			int nDBTimeOut )
{
	SAFE_DELETE(m_pKorDB);
    m_pKorDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pKorDB->OpenDB();
}

// 말레이시아 PC방 이벤트
int	COdbcManager::OpenMyDB(
			TCHAR* szOdbcName,
			TCHAR* szUsrID, 
			TCHAR* szUsrPasswd,
			TCHAR* szDbName,
			int nPoolSize,
			int nDBTimeOut )
{
	SAFE_DELETE(m_pMyDB);
    m_pMyDB = new COdbcSupervisor(
						szOdbcName,
						szUsrID,
						szUsrPasswd,
						szDbName,
						nPoolSize,
						nDBTimeOut );
	return m_pMyDB->OpenDB();
}	

void COdbcManager::CloseGspDB()
{
	SAFE_DELETE(m_pGspDB);
}

void COdbcManager::CloseGameDB()
{
	SAFE_DELETE(m_pGameDB);
}

void COdbcManager::CloseUserDB()
{
	SAFE_DELETE(m_pUserDB);
}

void COdbcManager::CloseBoardDB()
{
	SAFE_DELETE(m_pBoardDB);
}

void COdbcManager::CloseLogDB()
{
	SAFE_DELETE(m_pLogDB);
}

void COdbcManager::CloseShopDB()
{
	SAFE_DELETE(m_pShopDB);
}

void COdbcManager::CloseTerraDB()
{
	SAFE_DELETE(m_pTerraDB);
}

void COdbcManager::CloseThaiDB()
{
	SAFE_DELETE(m_pThaiDB);
}

void COdbcManager::CloseKorDB()
{
	SAFE_DELETE(m_pKorDB);
}

void COdbcManager::CloseMyDB()
{
	SAFE_DELETE(m_pMyDB);
}

void COdbcManager::Print(const CString & strMsg)
{
	// for Console 
	// printf(strMsg.GetString());
	// for GUI
	CConsoleMessage::GetInstance()->WriteDatabase( _T("DB:%s"), strMsg.GetString() );
}


CString	COdbcManager::GetErrorString(SQLHSTMT hStmt)
{
	SQLRETURN sReturn = 0;
	SQLCHAR SqlState[6] = {0};
	SQLCHAR Msg[ODBC_ERROR_MESSAGE_LENGTH+1] = {0};
	SQLSMALLINT MsgLen = 0;
	SQLINTEGER nError = 0;
	int nDiag = 0;

	CString strTemp;

	::SQLGetDiagField(SQL_HANDLE_STMT, hStmt, 0, SQL_DIAG_NUMBER, &nDiag, 0, &MsgLen);

	for (nDiag=1;;nDiag++)
	{
		sReturn = ::SQLGetDiagRec(SQL_HANDLE_STMT, 
								  hStmt, 
								  nDiag,
								  SqlState,
								  &nError,
								  Msg,
								  ODBC_ERROR_MESSAGE_LENGTH, 
								  &MsgLen);
		if (sReturn == SQL_NO_DATA)
			break;
		else
			strTemp.Format(_T("%s, NativeError:%d, %s"),
							(LPCTSTR) SqlState,
							nError,
							(LPCTSTR) Msg);

	}
	return strTemp;
}

void COdbcManager::CheckConnection()
{
	ODBC_STMT* pConn = NULL;
	if (m_pGameDB)
	{
		pConn = m_pGameDB->GetConnection();
		if (pConn)
		{
			m_pGameDB->FreeConnection(pConn);
		}
		else 
		{
			CConsoleMessage::GetInstance()->WriteDatabase( _T("ERROR:Game DB Connection") );
		}
	}
	if (m_pUserDB)
	{
		pConn = m_pUserDB->GetConnection();
		if (pConn) 
		{
			m_pUserDB->FreeConnection(pConn);
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase(
				                                  _T("ERROR:User DB Connection") );
		}
	}		
	if (m_pBoardDB)
	{
		pConn = m_pBoardDB->GetConnection();
		if (pConn)
		{
			m_pBoardDB->FreeConnection(pConn);
		}
		else 
		{
			CConsoleMessage::GetInstance()->WriteDatabase(
				                                  _T("ERROR:Board DB Connection"));
		}
	}
	if (m_pLogDB)
	{
		pConn = m_pLogDB->GetConnection();
		if (pConn)
		{
			m_pLogDB->FreeConnection(pConn);
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase(
				                                  _T("ERROR:Log DB Connection"));
		}
	}
	if (m_pShopDB)
	{
		pConn = m_pShopDB->GetConnection();
		if (pConn)
		{
			m_pShopDB->FreeConnection(pConn);
		}
		else
		{
			CConsoleMessage::GetInstance()->WriteDatabase(
				                                  _T("ERROR:Shop DB Connection"));
		}
	}
}

void COdbcManager::AddJob(
	CDbAction *pDbActJob )
{
	pDbActJob->SetDbManager( this );
	CDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
}

void COdbcManager::AddLogJob(
	CDbAction *pDbActJob )
{
	pDbActJob->SetDbManager( this );
	//CDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
	CLogDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
}

void COdbcManager::AddUserJob( CDbAction* pDbActJob )
{
	/*
	pDbActJob->SetDbManager ( this );	
	CLogDbExecuter::GetInstance()->AddJob ( (CDbAction*) pDbActJob );
	*/	
	pDbActJob->SetDbManager( this );
	//CDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
	CUserDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
}

void COdbcManager::AddWebJob( CDbAction* pDbActJob )
{	
	pDbActJob->SetDbManager( this );
	CWebDbExecuter::GetInstance()->AddJob( (CDbAction*) pDbActJob );
}