#include "pch.h"
#include "s_COdbcEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COdbcEnv* COdbcEnv::SelfInstance = NULL;

COdbcEnv::COdbcEnv(void) 
	: m_hEnv(NULL)
{
	CloseHandle();
	CreateHandle();	
}

COdbcEnv::~COdbcEnv(void)
{
	CloseHandle();
}

COdbcEnv* COdbcEnv::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new COdbcEnv();
	return SelfInstance;
}

void COdbcEnv::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

int COdbcEnv::CreateHandle()
{
	CloseHandle();

	SQLRETURN sReturn; // SQL return value (short);

	// Connection pooling
	sReturn = ::SQLSetEnvAttr(
					NULL,
					SQL_ATTR_CONNECTION_POOLING,
					(void*) SQL_CP_ONE_PER_HENV,
					0 );
	// sReturn = ::SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (void*) SQL_CP_ONE_PER_DRIVER, 0);	
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		CConsoleMessage::GetInstance()->Write(_T("INFO:SQL_ATTR_CONNECTION_POOLING OK"));
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase(											
											_T("ERROR:SQL_ATTR_CONNECTION_POOLING Failed"));
		return DB_ERROR;
	}

	// ODBC allocate environment memory
	sReturn = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if (sReturn == SQL_SUCCESS || sReturn == SQL_SUCCESS_WITH_INFO)
	{
		CConsoleMessage::GetInstance()->Write(_T("INFO:SQLAllocHandle OK"));
	}
	else
	{
		CConsoleMessage::GetInstance()->WriteDatabase(											
											_T("ERROR:SQLAllocHandle Failed") );
		return DB_ERROR;
	}

	// ODBC set environment
	// version	
	sReturn = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*) SQL_OV_ODBC3, 0);
	if (sReturn != SQL_SUCCESS && sReturn != SQL_SUCCESS_WITH_INFO)
	{		
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = NULL;
		return DB_ERROR;
	}

	return DB_OK;
}

void COdbcEnv::CloseHandle()
{
	if (m_hEnv != NULL)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		m_hEnv = NULL;
	}
}

SQLHENV COdbcEnv::GetHandle()
{
	return m_hEnv;
}